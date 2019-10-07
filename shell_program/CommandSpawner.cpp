#include "CommandSpawner.h"
#include "logger.h"
#include <cstring>
#include <string>
#include <sys/wait.h>
#include <unistd.h>
#include <pwd.h>
#include <iostream>
#include <fstream>
#include <fcntl.h>
#include <vector>

CommandSpawner::CommandSpawner() {
	struct passwd *pw = getpwuid(getuid());
	const char *homedir = pw->pw_dir;
	std::string log_file = std::string(homedir) + "/.jhsh_history";
	log = new logger(log_file);
}


CommandSpawner::~CommandSpawner() {
	delete log;
}

int CommandSpawner::run(std::string command) {
	this->log->add_history_item(command);
	std::vector<struct Command*> command_list  = tokenize(command);
	int return_code = 0;
	int exit_f = 0;

	//Check for redirect
	int redirect_position = check_redirect();
	if (redirect_position != -1) {
		return run_redirect(redirect_position);
	}

	for (struct Command* c : command_list) {
		if (std::string(c->command[0]) != "exit") {
			pid_t pid = fork();
			if (pid < 0)
				return 1;
			else if (pid == 0) {
				exec(*c);
			} else {
				waitpid(pid, 0, 0);
				if (c->in != 0)
				   close(c->in);
				if (c->out != 1)
				   close(c->out);
			}
		} else {
			exit_f = 1;
		}
	}

	if (exit_f == 1) {
		std::exit(0);
	}

	return return_code;
}

// This is run by a child. Shouldn't be run by parent.
void CommandSpawner::exec(struct Command cmd) {
	int return_code = 0;
	if (cmd.in != 0)
		dup2(cmd.in, 0);
	if (cmd.out != 1)
		dup2(cmd.out, 1);

	if ("pwd" == std::string(current_command[0]))
		return_code = pwd();
	else if ("history" == std::string(cmd.command[0]))
		return_code = history();
	else
		return_code = exec_p(cmd.command);

	_exit(return_code);
}

std::vector<struct CommandSpawner::Command*> CommandSpawner::tokenize(std::string command) {
	char* cmd = new char[command.length()+1];
	std::strcpy(cmd, command.c_str());
	int n = 0;
    char *tokenPtr = std::strtok(cmd, " \n\r");
    while (tokenPtr != NULL) {
        current_command[n++] = tokenPtr;
        tokenPtr = std::strtok(NULL, " \n\r");
    }
	current_command[n] = NULL;

	std::vector<struct Command*> cmds;
	struct Command *current_cmd = new struct Command;
	current_cmd->command = &current_command[0];
	for(int x = 0; x < n; x++) {
		if (std::string(current_command[x]) == "|") {
			int fds[3];
			pipe(fds);
			current_command[x] = NULL;
			current_cmd->out = fds[1];
			cmds.push_back(current_cmd);
			current_cmd = new struct Command;
			current_cmd->in = fds[0];
			current_cmd->command = &(current_command[x + 1]);
		}
	}
	cmds.push_back(current_cmd);
	current_cmd = NULL;
	return cmds;

}

int CommandSpawner::pwd() {
	char* buff = new char[1000];
	size_t n = 1000;
	getcwd(buff, n);
	std::cout << buff << std::endl;
	return 0;
}

int CommandSpawner::history() {
	log->print_history();
	return 0;
}

int CommandSpawner::exec_p(char** command) {
	pid_t pid = fork();
	if (pid < 0)
		return 1;
	else if (pid == 0) {
		execvp(command[0], command);
	}
	return WIFEXITED(waitpid(pid, 0, 0));
}


int CommandSpawner::check_redirect() {
	//Set return value
	int return_value = -1;

	//Run through current_command, scan for "<" or ">"
	int position = 0;
	while (current_command[position] != NULL) {
		//If redirection occurs, return position of operator
		if (std::string(current_command[position]) == ">"){
			return_value = position;
		}
		else if (std::string(current_command[position]) == "<") {
			return_value = position;
		}

		//Increment position, and continue search
		position++;
	}

	//Return whatever we got back, or -1 if nothing
	return return_value;
}

int CommandSpawner::run_redirect(int position){
	//Create two more double pointers to handle the current_command
	char** first_half, **second_half;
	first_half = new char*[1000];
	second_half = new char*[1000];
	int return_code;
	int position_iterator = 0;

	//Grab first half
	position_iterator = 0;
	while (position_iterator < position) {
		first_half[position_iterator] = current_command[position_iterator];
		position_iterator++;
	}
	//Null terminate first half
	first_half[position_iterator] = NULL;

	//Grab second half of the command
	position_iterator = 0;
	while (current_command[position_iterator + position + 1]) {
		second_half[position_iterator] = current_command[position_iterator + position + 1];
		position_iterator++;
	}
	//Null terminate the second half
	second_half[position_iterator] = NULL;

	//Determine whether to do input or output redirection
	if (std::string(current_command[position]) == "<") {
		//Do input redirection
		int oldFD = dup(STDIN_FILENO);
		int fd = open(second_half[0], O_RDONLY);

		//Switch STDIN to be the fd given above
		dup2(fd,0);

		//Execute command
		if ("pwd" == std::string(first_half[0]))
			return_code = pwd();
		else if ("history" == std::string(first_half[0]))
			return_code = history();
		else
			return_code = exec_p(first_half);

		//Flush the input, switch STDIN back, and close the file descriptor
		fflush(stdin);
		dup2(oldFD, 0);
		close(fd);

		return return_code;
	}
	else if (std::string(current_command[position]) == ">") {

		//Check to see if file exists by attempting to open it
		std::fstream file_descriptor;
		file_descriptor.open(std::string(second_half[0]), std::fstream::in);

		//If the file doesn't exist, make a new one
		while (!file_descriptor) {
			//Create the file with trunc and write modes
			file_descriptor.open(std::string(second_half[0]), std::fstream::trunc | std::fstream::out);
		}

		//Ensure the file is closed for the rest of the operation
		file_descriptor.close();

		//Do output redirection
		int oldFD = dup(STDOUT_FILENO);
		int fd = open(second_half[0], O_WRONLY | O_CREAT | O_APPEND);

		//Switch STDOUT to be the fd given above
		dup2(fd,1);

		//Execute command
		if ("pwd" == std::string(first_half[0]))
			return_code = pwd();
		else if ("history" == std::string(first_half[0]))
			return_code = history();
		else
			return_code = exec_p(first_half);

		//Flush the output, switch STOUT back, and close the file descriptor
		fflush(stdout);
		dup2(oldFD, 1);
		close(fd);

		return return_code;
	}
	else {
		//Something has gone horribly wrong. Burn the evidence
		std::cout << "Invalid redirection\n" << std::endl;
		return 1;
	}
}

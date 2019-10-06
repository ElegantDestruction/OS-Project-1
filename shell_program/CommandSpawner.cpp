#include "CommandSpawner.h"
#include "logger.h"
#include <cstring>
#include <string>
#include <sys/wait.h>
#include <unistd.h>
#include <pwd.h>
#include <iostream>
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
	std::vector<pid_t> kids;
	int return_code = 0;
	int exit_f = 0;
	for (struct Command* c : command_list) {
		if (std::string(c->command[0]) != "exit") {
			pid_t pid = fork();
			if (pid < 0)
				return 1;
			else if (pid == 0) {
				exec(*c);
			} else {
				kids.push_back(pid);
			}
		} else {
			exit_f = 1;
		}
	}
		

	for (pid_t p : kids) {
		std::cout << p << std::endl;
		while(!WIFEXITED(waitpid(p, 0, WNOHANG)));
	}
	
	if (exit_f == 1) {
		std::exit(0);
	}

	return return_code;
}

// This is run by a child. Shouldn't be run by parent.
void CommandSpawner::exec(struct Command cmd) {
	int return_code = 0;
	dup2(cmd.in, 0);
	dup2(cmd.out, 1);
	dup2(cmd.err, 2);

	if ("pwd" == std::string(cmd.command[0]))
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

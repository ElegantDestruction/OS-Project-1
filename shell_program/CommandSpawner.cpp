#include "CommandSpawner.h"
#include "logger.h"
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <pwd.h>
#include <iostream>

CommandSpawner::CommandSpawner() {
	struct passwd *pw = getpwuid(getuid());
	const char *homedir = pw->pw_dir;
	this->log = new logger(std::string(homedir) + "/.jhsh_history");
	
}


CommandSpawner::~CommandSpawner() {
	delete this->log;
	delete[] current_command;
}

int CommandSpawner::run(std::string command) {
	log->add_history_item(command);

	tokenize(command);
	int return_code = 0;

	if ("pwd" == std::string(current_command[0]))
		return_code = pwd();
	else if ("history" == std::string(current_command[0])) 
		return_code = history();
	else if ("exit" == std::string(current_command[0])) 
		exit();

	else
		return_code = exec_p(current_command);

	return return_code;
}

void CommandSpawner::tokenize(std::string command) {	
	char* tokenPtr = strtok(&(command[0]), " \n\r");
	int pos = 0;
    while (tokenPtr != NULL) {
		current_command[pos++] = tokenPtr;
        tokenPtr = strtok(NULL, " \n\r");
    } 
}

int CommandSpawner::pwd() {
	char* buff = NULL;
	size_t n = 0;
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
	return wait(0);
}

void CommandSpawner::exit() {
	std::exit(0);
}


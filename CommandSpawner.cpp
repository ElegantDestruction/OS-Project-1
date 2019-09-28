#include "CommandSpawner.h"
#include "logger.h"
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <iostream>

CommandSpawner::CommandSpawner() {
	this->history = new logger("");
}


CommandSpawner::~CommandSpawner() {
	delete this->history;
}

int CommandSpawner::run(std::string command) {
	this->history.add_history_item(command);

	char** current_command = tokenize(command);
	int return_code = 0;

	switch(currentCommand[0]) {
		case "pwd":
			return_code = pwd();
			break;
		case "history":
			return_code = history();
			break;
		case "exit":
			exit();
			break
		default:
			return_code = exec_p(current_command);
	}

	return return_code;
}

char* CommandSpawner::tokenize() {
	std::vector
	char *tokenPtr = std::strtok(input, " \n\r");
    while (tokenPtr != NULL) {
        char[]tokenPtr << '\n';
        tokenPtr = std::strtok(NULL, " \n\r");
    } 
}

int CommandSpawner::pwd() {
	char* buff = NULL;
	getcwd(buff);
	std::cout << buff << std::endl;
	return 0;
}

int CommandSpawner::history() {
	return this->history.print_history();
}

int CommandSpawner::exec_p(char* command) {
	pid_t pid = fork();
	if (pid < 0)
		return 1;
	else if (pid == 0) {
		execvp(command[0], command);
	}
	return wait(0);
}

void CommandSpawner::exit() {
	exit(0);
}


#pragma once

#include "logger.h"
#include <vector>

class CommandSpawner {
	public:
		CommandSpawner();
		~CommandSpawner();

		// Runs a line of commands
		int run(std::string command);
		
	private:
		logger* log;
		char** current_command = new char*[100];
		struct Command {
			char** command;
			int in;
			int out;
			int err;

			Command() {
				command = NULL;
				in = 0;
				out = 1;
				err = 2;
			};

		};

		std::vector<struct Command*> tokenize(std::string command);
		void exec(struct Command cmd);
		// Prints working directory
		int pwd();
		// Runs logger's history print command
		int history();
		// Run executable
		int exec_p(char** command);

		//Redirection methods
		int check_redirect(); 			//Detects if <> is used
		int run_redirect(int position);		//Runs command with redirect in mind

};

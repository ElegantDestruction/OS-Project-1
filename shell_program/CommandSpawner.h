#pragma once

#include "logger.h"


class CommandSpawner {
	public:
		CommandSpawner();
		~CommandSpawner();

		// Runs a command
		int run(std::string command);
		
	private:
		logger* log;
		char** current_command; 
		
		void tokenize(std::string command);
		// Prints working directory
		int pwd();
		// Runs logger's history print command
		int history();
		// Quit shell
		void exit();
		// Run executable
		int exec_p(char** command);

		//Redirection methods
		int check_redirect(); 			//Detects if <> is used
		int run_redirect(int position);		//Runs command with redirect in mind

};

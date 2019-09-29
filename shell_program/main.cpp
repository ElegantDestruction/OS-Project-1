#include <iostream>
#include <string>
#include <unistd.h>
#include "CommandSpawner.h"
#include "logger.h"


std::string get_console_header() {
	std::string output = "";
	std::string purple = "\033[1;35m";
	std::string cyan = "\033[1;36m";
	std::string clr = "\033[1;0m";
	char priv_char = '#';
	char* user = getlogin();
	char host[1024];
	gethostname(host, 1024);
	uid_t uid = getuid(); 
	if (uid > 0)
		priv_char = '$';
	if (user != NULL)
		output = std::string(user);
	if (host != NULL)
		output += "@" + std::string(host);

	return cyan + output + clr  + " " + purple + priv_char + clr + " ";	
}

int main(void) {
	std::string console_h = get_console_header();
	std::string buff = "";
	CommandSpawner* spawner = new CommandSpawner();
	int return_code = 0;
	std::cout << console_h;
	
	while(true) {
		getline(std::cin, buff);
		if (buff != "") {
			return_code = spawner->run(buff);
			if (return_code > 0) {
				std::cout << "Error: return code " << return_code << std::endl;
			}
			std::cout << console_h;
		}
		fflush(stdin);
		buff = "";
	}

	// This shouldn't be reachable	
	return 1;
}

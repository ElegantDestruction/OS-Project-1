#include <iostream>
#include <string>
#include "CommandSpawner.h"
#include "logger.h"


int main(char* [] argv) {
	std::string console_h = get_console_header();
	std::string buff;
	CommandSpawner spawner = new CommandSpawner();

	while(true) {
		std::cout << console_h;
		getline(cin, buff);
		return_code = spawner->run(buff);

		if (return_code > 0) {
			std::cout << "Error: return code " << return_code << std::endl;
		}
	}

	// This shouldn't be reachable	
	return 1;
}

std::string get_console_header() {
	std::string user = "";
	char priv_char = "#";
	char* temp = getlogin();
	uid_t uid = getuid(); 
	if (uid > 0)
		priv_char = "$";
	if (temp != NULL)
		user = String(temp);

	return user + " " + priv_char + " ";	
}

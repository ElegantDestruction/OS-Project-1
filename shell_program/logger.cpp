#include "logger.h"
#include<string>
#include<stdio.h>
#include<vector>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<iostream>
#include<fstream>
#include<sys/wait.h>

//This is the implementation for the logger object for storing the history items for the 
//jhsh shell.

//Constructor. Takes in filename as parameter to allow flexible history file usage
logger::logger(std::string history_filename) {

	//Declare the necessary variables
	std::string current_line;
	this->history_filename = history_filename;
	std::fstream file_descriptor;
	
	//Attempt to open the history file to read
	file_descriptor.open(history_filename, std::fstream::in);

	//If the file doesn't exist, make a new one and open it
	while (!file_descriptor) {
		//Give warning that file doesn't exist
		std::cout << "No history file at '" <<
		this->history_filename << "', creating a new one\n" << std::endl;
		
		//Create the file with trunc and write modes, then close it
		file_descriptor.open(history_filename, std::fstream::trunc | std::fstream::out);
		file_descriptor.close();
		
		//Attempt to open the file again for reading
		file_descriptor.open(history_filename, std::fstream::in);

	}

	//Loop through file and get all the commands
	while (getline(file_descriptor, current_line)) {
		live_history.push_back(current_line);	
	}

	//Switch STDIN back to the console, close file descriptor
	file_descriptor.close();
}

//Returns the last history item as a string
std::string logger::get_last_item() {
	return live_history.back();
}

//Adds a history item to both the live vector and the history file
void logger::add_history_item(std::string history_item) {
	//Add string to vector
	live_history.push_back(history_item);

	//Attempt to opent the history file with a file descriptor, message and exit upon failure
	std::ofstream file_descriptor(history_filename, std::fstream::app); // = open((char*)history_filename.c_str(), O_WRONLY | O_APPEND);

	if (!file_descriptor) {
		std::cout << "ERROR: Logger cannot open history file '"<<
		this->history_filename << "'. Does file exist?\n" << std::endl;
		exit(1);
	}

	//Write the string to the file
	file_descriptor << history_item << std::endl;

	//Switch STDOUT back to the console, close the file descriptor
	file_descriptor.close();

}

//Print out the entire history from the live vector
void logger::print_history() {
	//iterate through the vector and print the items
	for (unsigned i = 0; i < live_history.size(); i++) {
		std::cout << live_history[i] << std::endl;
	}
}

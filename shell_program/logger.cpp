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

using namespace std;

//This is the implementation for the logger object for storing the history items for the 
//jhsh shell.

//Constructor. Takes in filename as parameter to allow flexible history file usage
logger::logger(string history_filename) {

	string current_line;
	this->history_filename = history_filename;
	
	//Attempt to open the history file with a file descriptor, message and exit upon failure
	ifstream file_descriptor(history_filename); // = open((char*)history_filename.c_str(), O_RDONLY);
	if (!file_descriptor) {
		cout << "ERROR: Constructor cannot open history file '" <<
		this->history_filename << "'. Does the file exist?\n" << endl;
		exit(1);
	}

	//Loop through file and get all the commands
	while (getline(file_descriptor, current_line)) {
		live_history.push_back(current_line);	
	}

	//Switch STDIN back to the console, close file descriptor
	file_descriptor.close();
}

//Returns the last history item as a string
string logger::get_last_item() {
	return live_history.back();
}

//Adds a history item to both the live vector and the history file
void logger::add_history_item(string history_item) {
	//Add string to vector
	live_history.push_back(history_item);

	//Attempt to opent the history file with a file descriptor, message and exit upon failure
	ofstream file_descriptor(history_filename, fstream::app); // = open((char*)history_filename.c_str(), O_WRONLY | O_APPEND);

	if (!file_descriptor) {
		cout << "ERROR: Logger cannot open history file '"<<
		this->history_filename << "'. Does file exist?\n" << endl;
		exit(1);
	}

	//Write the string to the file
	file_descriptor << history_item << endl;

	//Switch STDOUT back to the console, close the file descriptor
	file_descriptor.close();

}

//Print out the entire history from the live vector
void logger::print_history() {
	//iterate through the vector and print the items
	for (unsigned i = 0; i < live_history.size(); i++) {
		cout << live_history[i] << endl;
	}
}

void logger::get_filename() {
	cout << history_filename << endl;
}

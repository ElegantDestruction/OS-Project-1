#pragma once

//This file is a header file for the logger object, used in storing 
//command history for the psuedo shell 'jhsh'
#include<string>
#include<stdio.h>
#include<vector>

class logger {
	
private:
	std::string history_filename;				//string name for historyfilename
	std::vector<std::string> live_history;			//vectory for storing history live

public:
	logger(std::string history_filename);			//constructor, takes in filename
	std::string get_last_item();				//returns last history item in string
	void add_history_item(std::string history_item);	//add item to history
	void print_history();					//print entire history
	void get_filename();					//TESTING ONLY, remove when done
};

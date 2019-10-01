# OS-Project-1
Project 1 Repository For Operating Systems Fall 2019
\
General Requirements:  
- `Exit' Exits the shell
- implement pwd (use getcdw())
- implement ls, and allow it to be used with flags and relative and/or absolute pathnames
- Keep a history of commands (use vector)
- Implement < and > using STDIN and STDOUT redirection
- Implement simple pipes | (use more than one process when doing this ie. pipe() and wait())

Part 1 Hard Requirements:  
- Create a program that forks off a child process that sends 3 messages to its parent via a pipe  
	- Full Name
	- Department Name
	- Email Address
- Then re-write the program so that write and read fcns are not used (use dup/dup2 to utilize cout and cin instead)  
- Implement parts 1-3 (basically all the requirements through `history')


Part 2 Hard Requirements:  
- Implement parts 4-5 (finish shell)

#include <stdio.h>
#include <unistd.h>
#include <string>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <iostream>

using namespace std;

int main() {

	size_t Maxsize = 150;
	int fd[2], nbytes;
	pid_t childpid; 
	string readbuffer_1;
	string readbuffer_2;
	string readbuffer_3;
	string sharedStr_1 = "Jacob Hillebrand\n";
	string sharedStr_2 = "Computer and Electrical Engineering Dept.\n";
	string sharedStr_3 = "hillebrandj3779@my.uwstout.edu\n";
	int oldFD = dup(STDOUT_FILENO);

	if(pipe(fd)<0) { //create a pipe
		exit(1); //error, no pipe was created
	}

	if((childpid = fork()) == -1) {
		perror("fork did not work; Sorry!");
		exit(1);
	}

	if(childpid == 0) {
		//Child Process closes up input side of pipe
		close(fd[0]);
		dup2(fd[1], 1);
		cout << sharedStr_1 << sharedStr_2 << sharedStr_3;
		//Send "string" through the output side of pip
		/*write(fd[1], sharedStr_1, (strnlen(sharedStr_1, Maxsize/3)));
		write(fd[1], sharedStr_2, (strnlen(sharedStr_2, Maxsize/3)));
		write(fd[1], sharedStr_3, (strnlen(sharedStr_3, Maxsize/3)));*/
		exit(0);
	}

	else {
		wait(NULL);

		//Parent process closes up output side of pipe
		close(fd[1]);
		dup2(fd[0],0);
		//Read in a string from the pipe
		getline(cin, readbuffer_1);
		getline(cin, readbuffer_2);
		getline(cin, readbuffer_3);
//		nbytes = read(fd[0], readbuffer, sizeof(readbuffer));

		fflush(stdout);
		close(fd[0]);
		close(fd[1]);

		dup2(oldFD, 1);
		cout << "From Parent: Recieved String: " << endl << 
			readbuffer_1 << endl <<
			readbuffer_2 << endl << 
			readbuffer_3 << endl;
	}

	return(0);
}

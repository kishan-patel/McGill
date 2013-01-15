#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

//Micro Assignment 3
//Kishan Patel
//260376121
main()
{
	int status;
	int fd[2];
	pipe(fd);//Create a pipe.
	
	if(!fork())
	{//We're in the child process.
	
		dup2(fd[1],1);//Set stdout to the input side of the pipe.
		close(fd[0]);//We never use the output side.
		close(fd[1]);//Close the input side of the pipe.
		char *argv[] = {"ls", NULL};//Command to be executed.
		execlp("ls", "ls", NULL);//Execute the command
	}
	else
	{//We're in the parent process.
	
		dup2(fd[0],0);//Set the stdin to the output side of the pipe.
		close(fd[1]);//We never use the input side of the pipe.
		close(fd[0]);//Close the output side of the pipe.
		char *argv[] = {"wc","-l",NULL};//Command to be executed.
		execlp("wc", "wc", "-l", NULL);//Execute the command.
	}
	
	return 0;
}
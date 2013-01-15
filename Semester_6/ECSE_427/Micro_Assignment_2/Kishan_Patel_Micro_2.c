#include <stdio.h>
#include <stdlib.h>
#include<unistd.h>
#include<sys/wait.h>

main()
{
	pid_t childPid;
	int status;
	char usrInput[100];
	char *command[100];
	char *tkn;
	int count=0;

	while(1)
	{
		switch(childPid=fork())
		{
			case -1: 
				//Fork failed
				perror("fork error");
				exit(EXIT_FAILURE);
			case 0:
				//Perform actions specific to child
				count=0;
				printf("Enter command to execute:\n");
				fgets(usrInput,100,stdin);
				tkn = (char *)strtok(usrInput," \n");
				while (tkn != NULL) {
					command[count] = tkn;
					count++;
					tkn = (char *)strtok(NULL," \n");
				}
				command[count]=NULL;
				execvp(command[0], command);
				printf("The command is not formatted properly. Try again.\n");
			default:
				//Perform actions specific to parent
				wait(&status);
				printf("\n");
		}
	}
}
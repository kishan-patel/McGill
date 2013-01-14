main(void)
{
	int fd[2];
	const int READ=0;
	const int WRITE=1;
	pid_t pid;
	
	pipe(fd);
	
	pid=fork();
	switch(childPid=fork())
	{
		case -1://An error occurred while attempting to fork.
			errExit("Fork");
			
		case 0://We're in the child process.
			dup2(hpipe[WRITE],1); //Wire standard out to the input of the file descriptor.
			char *command[2] = {"ls",NULL}; //ls command that will be executed in this process.
			execv("/bin/ls",command); //execute the ls command.
			
		default://We're in the parent process.
			close(fd[READ]); //The read file descriptor is not used by the parent.
	}
	
	pid=fork();
	switch(childPid=fork())
	{
		case -1://An error occurred while attempting to fork.
			errExit("Fork");
			
		case 0://We're in the child process.
			dup2(fd[READ],0); //Redirect standard input to the input side of the file descriptor.
			
		default://We're in the parent process.
		
	}
	
	waitpid(pid);
}
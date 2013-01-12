main(void)
{
	int pfd[2];
	char buf[BUF_SIZE];
	ssize_t numRead;
	char *childCommand[100];
	
	//Create the pipe
	if(pipe(pfd) == -1)
	{
		errExit("pipe");
	}
	
	switch(fork())
	{
		case -1:
			errExit("fork");
		case 0:
			//Child - reads from pipe.
			//Write end is not used.
			if(close(pfd[1]) == -1)
			{
				errExit("close - child");
			}
			
			for(;;)
			{
				//Read data from the pipe.
				numRead = read(pfd[0], buf, BUF_SIZE);
				if(numRead == -1)
				{
					errExit("child: read");
				}
				if(numRead == 0)
				{
					break;
				}
				
				childCommand[0]="wc";
				childCommand[1]="-1";
				childCommand[2]="";
				
			}
	}
	
}
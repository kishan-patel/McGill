#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>


//Kishan Patel
//260376121
//Assignment 4 - Write part
main()
{
	const char *fileName = "data.dat";
	int fd = open(fileName,O_WRONLY);
	int *x = malloc(sizeof(int));
	int i=0;
	
	for(i=0;i<10;i++)
	{
		printf("Getting integer\n");
		*x=arc4random();
		printf("about to write\n");
		write(fd,x,sizeof(int));
		printf("Wrote int %d\n",*x);
	}

	close(fd);
	return 0;
}
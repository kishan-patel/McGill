#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>


//Kishan Patel
//260376121
//Assignment 4 - Write part
//To compile this program you need to use the -lbsd flag.
//e.g. gcc Kishan_Patel_Micro_4_Write.c -o out -lbsd
main()
{
	const char *fileName = "data.dat";
	int fd = open(fileName,O_WRONLY|O_CREAT);
	int *x = malloc(sizeof(int));
	int i=0;
	
	for(i=0;i<100;i++)
	{
		*x=arc4random();
		write(fd,x,sizeof(int));
	}

	close(fd);
	return 0;
}
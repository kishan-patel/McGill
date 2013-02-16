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
	const char *fileName = "data.dat"; //Name of file.
	int fd = open(fileName,O_WRONLY|O_CREAT|O_TRUNC); //Open the file for writing or create it if it doesn't exist.
	int *x = malloc(sizeof(int));	//Pointer of type int.
	int i=0; //Index variable.
	
	for(i=0;i<100;i++)
	{
		*x=5/*arc4random()*/; //Generates a random number.
		write(fd,x,sizeof(int)); //The generated random number is written to the file.
	}

	close(fd); //The file descriptor is closed.
	return 0;
}
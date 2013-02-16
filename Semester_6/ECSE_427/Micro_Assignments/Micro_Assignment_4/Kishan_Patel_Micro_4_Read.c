#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

//Kishan Patel
//260376121
//Micro Assignment 4 - Read part
main()
{
	const char *fileName = "data.dat"; //The name of the file.
	int fd = open(fileName,O_RDONLY); //Open the file for reading.
	int *position = malloc(sizeof(int)); //Pointer of type int.
	int *intRead = malloc(sizeof(int)); //Pointer of type int.
	int offset; //Will be used when determining the offset of the integer.

	while(1)
	{
		printf("Enter integer position (0-99) to retrieve from file:");
		scanf("%d",position); //Reads the input from the user.
		offset = *position * 4; //Calculates the offset of the integer from the begining of the file. 
		lseek(fd,offset,0); //Moves the file offset to that calculated above.
		read(fd,intRead,sizeof(int)); //Reads the integer from the file.
		printf("Integer at position %d is %d\n",*position,*intRead);
	}
	
	close(fd); //Closes the file.
	return 0;
}
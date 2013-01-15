#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

//Kishan Patel
//260376121
//Micro Assignment 4 - Read part
main()
{
	const char *fileName = "data.dat";
	int fd = open(fileName,O_RDONLY);
	int *position = malloc(sizeof(int));
	int *intRead = malloc(sizeof(int));
	int offset;

	while(1)
	{
		printf("Enter integer position to retrieve from file:");
		scanf("%d",position);
		offset = *position * 4;
		lseek(fd,offset,0);
		read(fd,intRead,sizeof(int));
		printf("Integer at position %d is %d\n",*position,*intRead);
	}
	
	close(fd);
	return 0;
}
/***********************************************************/
/*Kishan Patel																						 */
/*260376121       																				 */
/*Mini Assingment 1                             					 */
/***********************************************************/
#include<stdio.h>
#include<stdlib.h>
#include<alloca.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/resource.h>
extern char etext,edata,end;

void stackSegmentSize()
{
	struct rlimit limit;
	getrlimit(RLIMIT_STACK, &limit);
	char *ptr = (char *)alloca(1);
	char *tmpPtr;
	int memCount = 0;
	printf("Start of stack segment is at: %p\n",ptr);
	ptr = (char *)alloca(limit.rlim_cur*.99);
	printf("End of stack segment is at: %p\n",ptr);
}

void heapSegmentSize()
{
	char *ptr = (char *)malloc(1);
	char *tmpPtr;
	int memCount = 0;
	printf("Start of heap segment is at: %p\n",ptr);	
	while(ptr)
	{
		memCount++;
		ptr = realloc(ptr,  memCount * 1024 * 1024);
		if(ptr)
		{
				tmpPtr = ptr;
		}
	}
	printf("End of heap segment is at: %p\n", tmpPtr);
}

void memoryMappingSegmentSize()
{
  int fd = open("Mini_A1.c", O_RDONLY);  
  struct stat filestat;
  if (fstat(fd, &filestat) == -1)
    perror("Get file  stat");
  char *ptr = (char *)mmap(NULL, filestat.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	char *tmpPtr;
	printf("Start of memory mapping segment is at: %p\n",ptr);
	while(ptr!=MAP_FAILED)
	{
		fd = open("Mini_A1.c", O_RDONLY);
		fstat(fd, &filestat);
		ptr = (char *)mmap(NULL, filestat.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
		if(ptr!=MAP_FAILED)
		{
			tmpPtr = ptr;
		}
	}
	printf("End of memory mapping segment is at: %p\n",tmpPtr);
}

int main()
{	
	//Prints virtual address of text segment.
	printf("Start of text segment is at: %p\n", main);
	printf("End of text segment is at: %p\n", &etext);
	
	//Prints virtual address space of BSS segment.
	printf("Start of data segment is at: %p\n", &etext);
	printf("End of data segment is at: %p\n", &edata);
	
	//Prints virutal address space of data segment.
	printf("Start of BSS segment is at: %p\n", &edata);
	printf("End of BSS segment is at: %p\n", &end);
	
	//Prints virtual address space of heap.
	heapSegmentSize();
	
	//Prints virtual address space of memory mapping segment.
	memoryMappingSegmentSize();
	
	//Prints virtual address space of stack segment.
	stackSegmentSize();
}



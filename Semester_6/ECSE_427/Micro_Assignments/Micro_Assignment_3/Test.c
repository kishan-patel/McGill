#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

enum PIPES {READ, WRITE};

int main(int argc, char**argv)
{
	/*int hpipe[2];
	pipe(hpipe);

	if (fork())
	{
		close(hpipe[READ]);
		dup2(hpipe[WRITE], 1);
		close(hpipe[WRITE]);
		execlp(argv[1], argv[1], NULL);
	}
	else
	{
		close(hpipe[WRITE]);
		dup2(hpipe[READ], 0);
		close(hpipe[READ]);
		execlp(argv[2], argv[2], NULL);
	}

	exit(EXIT_SUCCESS);*/
	
	int *x;
	x=malloc(sizeof(int));
	*x=5;
	printf("x value is: %d\n",*x);
}
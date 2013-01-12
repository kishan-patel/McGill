#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


int main( void ) {
	char *argv[6] = {"ls","-ltr","|","grep","K*",NULL};

	int pid = fork();

	if ( pid == 0 ) {
		execvp( "ls", argv );
	}

	/* Put the parent to sleep for 2 seconds--let the child finished executing */
	wait( 2 );

	printf( "Finished executing the parent process\n"
	        " - the child won't get here--you will only see this once\n" );

	return 0;
}
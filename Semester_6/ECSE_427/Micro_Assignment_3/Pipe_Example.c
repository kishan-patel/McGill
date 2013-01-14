/*#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

enum PIPES {READ, WRITE};

int main(int argc, char**argv)
{
	int hpipe[2];
	pipe(hpipe);

	if (fork())
	{
		close(hpipe[READ]);
		dup2(hpipe[WRITE], 1);
	//	close(hpipe[WRITE]);
		execlp(argv[1], argv[1], NULL);
	}
	else
	{
		close(hpipe[WRITE]);
		dup2(hpipe[READ], 0);
		close(hpipe[READ]);
		execlp(argv[2], argv[2], NULL);
	}

	exit(EXIT_SUCCESS);
}*/
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>

int main(void)
{
    int filedes[2];
    pipe(filedes);
	 int file = open("myfile.txt", O_APPEND | O_WRONLY);
    /* Run LS. */
    pid_t pid = fork();
    if (pid == 0) {
        /* Set stdout to the input side of the pipe, and run 'ls'. */
        dup2(filedes[1], 1);
        char *argv[] = {"ls", NULL};
        execv("/bin/ls", argv);
    } else {
        /* Close the input side of the pipe, to prevent it staying open. */
        close(filedes[1]);
    } 	

    /* Run WC. */
    pid = fork();
    if (pid == 0) {
        dup2(filedes[0], 0);
		printf("%d",filedes[0]);
        char *argv[] = {"wc","-l", NULL};
        //execv("/usr/bin/wc", argv);
    }

    /* Wait for WC to finish. */
    waitpid(pid);
}
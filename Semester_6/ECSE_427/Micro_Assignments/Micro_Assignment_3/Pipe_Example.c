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
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void)
{
    int filedes[2];
    int corpse;
    int status;
    pipe(filedes);

    /* Run LS. */
    pid_t pid = fork();
    if (pid == 0)
    {
        /* Set stdout to the input side of the pipe, and run 'ls'. */
        dup2(filedes[1], 1);
        close(filedes[1]);
        close(filedes[0]);
        char *argv[] = {"ls", NULL};
        execv("/bin/ls", argv);
        fprintf(stderr, "Failed to execute /bin/ls\n");
        exit(1);
    }
    /* Close the input side of the pipe, to prevent it staying open. */
    close(filedes[1]);

    /* Run WC. */
    pid = fork();
    if (pid == 0)
    {
        dup2(filedes[0], 0);
        close(filedes[0]);
        char *argv[] = {"wc", NULL};
        execv("/usr/bin/wc", argv);
        fprintf(stderr, "Failed to execute /usr/bin/wc\n");
        exit(1);
    }

    close(filedes[0]);

    while ((corpse = waitpid(-1, &status, 0)) > 0);
        /*printf("PID %d died 0x%.4X\n", corpse, status);*/
    return(0);

}

// #include <stdio.h>
// #include <stdlib.h>
// #include <unistd.h>

// int main(void)
// {
    // int pfds[2];

    // pipe(pfds);

    // if (!fork()) {
        // close(1);       /* close normal stdout */
        // dup(pfds[1]);   /* make stdout same as pfds[1] */
        // close(pfds[0]); /* we don't need this */
        // execlp("ls", "ls", NULL);
    // } else {
        // close(0);       /* close normal stdin */
        // dup(pfds[0]);   /* make stdin same as pfds[0] */
        // close(pfds[1]); /* we don't need this */
        // execlp("wc", "wc", "-l", NULL);
    // }

    // return 0;
// }
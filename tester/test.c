#include <stdio.h>
#include <stdlib.h>
#include <sysexits.h>
#include <unistd.h>

int main(int argc, char** argv) {
	int pipefds[2];
	pipe(pipefds);

	pid_t childpid = fork();
	if (childpid == -1) {
		fprintf(stderr, "Unable to fork()\n");
		exit(EX_OSERR);
	}

	if (childpid == 0) {
		// child
		close(pipefds[0]);
		printf("child: spawned\n");

		// send stdout and stderr to the pipe
		dup2(pipefds[1], 1);
		dup2(pipefds[1], 2);

		printf("child: stdout\n");
		sleep(1);
		fprintf(stderr, "child: stderr\n");
		sleep(1);
	}
	else {
		// parent
		close(pipefds[1]);
		printf("parent: still running\n");

		char buffer[20];
		while (read(pipefds[0], buffer, 15) > 0) {
			printf("parent: got value from child\n");
			printf("parent: %s\n", buffer);
		}

		int status;
		waitpid(childpid, &status, 0);
		printf("parent: child process ended\n");
	}

	return 0;
}

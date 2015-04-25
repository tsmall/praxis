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
		printf("child: spawned\n");

		close(pipefds[0]);
		write(pipefds[1], "Hello.\n", 8);
		sleep(1);
	}
	else {
		// parent
		printf("parent: still running\n");
		char buffer[10];

		close(pipefds[1]);
		if (read(pipefds[0], buffer, 8) != -1) {
			printf("parent: got value from child\n");
			printf("parent: %s\n", buffer);
		}

		int status;
		waitpid(childpid, &status, 0);
		printf("parent: child process ended\n");
	}

	return 0;
}

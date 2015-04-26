#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>
#include <unistd.h>

int main(int argc, char** argv) {
	int stdoutpipe[2], stdinpipe[2];
	pipe(stdoutpipe);
	pipe(stdinpipe);

	pid_t childpid = fork();
	if (childpid == -1) {
		fprintf(stderr, "Unable to fork()\n");
		exit(EX_OSERR);
	}

	if (childpid == 0) {
		// child
		close(stdoutpipe[0]);
		close(stdinpipe[1]);

		// Redirect stdin, stdout, and stderr to use pipes
		// so parent process can read them.
		dup2(stdinpipe[0], STDIN_FILENO);
		dup2(stdoutpipe[1], STDOUT_FILENO);
		dup2(stdoutpipe[1], STDERR_FILENO);

		char buffer[1024];
		fgets(buffer, sizeof(buffer), stdin);
		buffer[strcspn(buffer, "\n")] = '\0';
		printf("child: received <%s>\n", buffer);

		sleep(1);

		close(stdoutpipe[1]);
		close(stdinpipe[0]);
	}
	else {
		// parent
		close(stdoutpipe[1]);
		close(stdinpipe[0]);

		FILE* instream = fdopen(stdinpipe[1], "w");
		fprintf(instream, "Hi!\n");
		fflush(instream);

		char buffer[1024];
		FILE* outstream = fdopen(stdoutpipe[0], "r");
		while (!feof(outstream) && !ferror(outstream) && fgets(buffer, sizeof(buffer), outstream)) {
			printf("parent: got value from child\n");
			printf("parent: %s", buffer);
		}

		int status;
		waitpid(childpid, &status, 0);
		printf("parent: child process ended\n");

		close(stdoutpipe[0]);
		close(stdinpipe[1]);
	}

	return 0;
}

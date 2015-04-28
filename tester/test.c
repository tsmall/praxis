#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>
#include <unistd.h>

#define READ 0
#define WRITE 1

typedef enum { false, true } bool;

void runproc(const char* path, char* const argv[], const char* input, char* out, int outsize) {
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
		close(stdoutpipe[READ]);
		close(stdinpipe[WRITE]);

		// Redirect stdin, stdout, and stderr to use pipes
		// so parent process can read them.
		dup2(stdinpipe[READ], STDIN_FILENO);
		dup2(stdoutpipe[WRITE], STDOUT_FILENO);
		dup2(stdoutpipe[WRITE], STDERR_FILENO);

		execv(path, argv);
	}
	else {
		// parent
		close(stdoutpipe[WRITE]);
		close(stdinpipe[READ]);

		FILE* instream = fdopen(stdinpipe[WRITE], "w");
		fputs(input, instream);
		fflush(instream);
		fclose(instream);

		/* TODO: Handle multiple line output */
		FILE* outstream = fdopen(stdoutpipe[READ], "r");
		while (!feof(outstream) && !ferror(outstream) && fgets(out, outsize, outstream)) {
			printf("parent: got value from child\n");
			printf("parent: %s", out);
		}

		int status;
		waitpid(childpid, &status, 0);
		printf("parent: child process ended\n");

		// fclose(instream);
		close(stdoutpipe[READ]);
		close(stdinpipe[WRITE]);
	}
}

bool runtest(const char* path, char* const argv[], const char* input, const char* expected) {
	char buffer[1024];
	runproc(path, argv, input, buffer, sizeof(buffer));
	return strcmp(expected, buffer) == 0;
}

int main(int argc, char* argv[]) {
	char* childargv[] = { "cat", "-", NULL };
	char* input = "Hi!\n";
	char* expected = "Hi!\n";

	bool successful = runtest("/bin/cat", childargv, input, expected);
	printf("result: %d\n", successful);

	return 0;
}

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>
#include <unistd.h>

#define READ 0
#define WRITE 1

typedef enum { false, true } bool;

typedef struct ProcInfo {
	char* path;
	char** argv;
} ProcInfo;

typedef struct TestInfo {
	ProcInfo proc;
	char* input;
	char* expected;
	bool passed;
} TestInfo;

void runproc(ProcInfo proc, const char* input, char* out, int outsize) {
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

		execv(proc.path, proc.argv);
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
		}

		int status;
		waitpid(childpid, &status, 0);

		fclose(instream);
		close(stdoutpipe[READ]);
		close(stdinpipe[WRITE]);
	}
}

void runtest(TestInfo* test) {
	char buffer[1024];
	runproc(test->proc, test->input, buffer, sizeof(buffer));
	test->passed = (strcmp(test->expected, buffer) == 0);
}

int main(int argc, char* argv[]) {
	char* childargv[2];
	childargv[0] = "python";
	childargv[1] = "/Users/tom/Projects/praxis/0001-rpn/rpn.py";

	ProcInfo proc;
	proc.path = "/usr/bin/python";
	proc.argv = childargv;

	char* input = "1 1 +\n";
	char* expected = "[2.0]\n";

	TestInfo test;
	test.proc = proc;
	test.input = input;
	test.expected = expected;

	runtest(&test);
	printf("result: %s\n", test.passed ? "passed" : "failed");

	return 0;
}

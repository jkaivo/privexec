#define _POSIX_C_SOURCE 200809L
#include <limits.h>
#include <spawn.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>

#ifndef ARG_MAX
#define ARG_MAX _POSIX_ARG_MAX
#endif

#ifndef PATH_CHECK
#define PATH_CHECK	"/usr/local/lib/privexec/check"
#endif

#ifndef PATH_EXEC
#define PATH_EXEC	"/usr/local/lib/privexec/exec"
#endif

#ifndef DEFAULT_PATH
#define DEFAULT_PATH	"/bin:/usr/bin"
#endif

static int exec_with_privileges(char *argv[])
{
	argv[0] = PATH_EXEC;
	execv(argv[0], argv);
	perror(argv[0]);
	return 1;
}

static int check_privileges(char *argv[])
{
	argv[0] = PATH_CHECK;
	pid_t pid = 0;
	if (posix_spawn(&pid, argv[0], NULL, NULL, argv, NULL) != 0) {
		perror(PATH_CHECK);
		return 1;
	}

	int status = 1;
	if (waitpid(pid, &status, 0) == -1) {
		perror(PATH_CHECK);
		return 1;
	}

	return status;
}

int main(int argc, char *argv[])
{
	const char *name = argv[0];
	if (name == NULL) {
		fprintf(stderr, "NULL execution not supported\n");
		return 1;
	}

	/* no options, but allow for -- */
	int c;
	while ((c = getopt(argc, argv, "")) != -1) {
		switch (c) {
		default:
			return 1;
		}
	}

	argc -= (optind - 1);
	argv += (optind - 1);

	if (argc < 2) {
		printf("%s: missing operands\n", name);
		return 1;
	}

	if (check_privileges(argv) == 0) {
		char path[ARG_MAX];
		if (confstr(_CS_PATH, path, sizeof(path)) < 1) {
			strcpy(path, DEFAULT_PATH);
		}
		setenv("PATH", path, 1);

		return exec_with_privileges(argv);
	}

	/* error message already displayed by check_privileges() */
	return 1;
}

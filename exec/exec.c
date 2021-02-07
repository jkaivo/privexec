#define _POSIX_C_SOURCE 200809L
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define NOT_RUN		(126)
#define NOT_FOUND	(127)

int main(int argc, char *argv[])
{
	if (argc < 2 || argv == NULL) {
		fprintf(stderr, "%s: missing operands\n", argv[0]);
		return NOT_RUN;
	}

	setuid(geteuid());
	execvp(argv[1], argv + 1);
	fprintf(stderr, "%s: %s: %s\n", argv[0], argv[1], strerror(errno));
	if (errno == ENOENT) {
		return NOT_FOUND;
	}
	return NOT_RUN;
}

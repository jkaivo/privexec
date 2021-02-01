#define _POSIX_SOURCE
#include <errno.h>
#include <stdio.h>
#include <unistd.h>

#define NOT_RUN		(126)
#define NOT_FOUND	(127)

int main(int argc, char *argv[])
{
	if (argc < 2 || argv == NULL) {
		fprintf(stderr, "%s: missing operands\n", argv[0]);
		return NOT_RUN;
	}

	execv(argv[1], argv + 1);
	perror(argv[0]);
	if (errno == ENOENT) {
		return NOT_FOUND;
	}
	return NOT_RUN;
}

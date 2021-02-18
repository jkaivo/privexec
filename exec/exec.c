#define _XOPEN_SOURCE 700
#include <errno.h>
#include <inttypes.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>

#define PRIVEXEC_LOG_ID	"privexec"
#define NOT_RUN		(126)
#define NOT_FOUND	(127)

#ifndef ARG_MAX
#define ARG_MAX _POSIX_ARG_MAX
#endif

void log_execution(int argc, char *argv[])
{
	char cmd[ARG_MAX] = "";
	strncat(cmd, argv[1], sizeof(cmd) - 1);
	for (int i = 2; i < argc; i++) {
		strncat(cmd, " ", sizeof(cmd) - 1);
		strncat(cmd, argv[i], sizeof(cmd) - 1);
	}

	openlog(PRIVEXEC_LOG_ID, LOG_PID, LOG_AUTH);
	syslog(LOG_INFO, "UID %jd running \"%s\"", (intmax_t)getuid(), cmd);
	closelog();
}

int main(int argc, char *argv[])
{
	if (argc < 2 || argv == NULL) {
		fprintf(stderr, "%s: missing operands\n", argv[0]);
		return NOT_RUN;
	}

	log_execution(argc, argv);

	setuid(geteuid());
	execvp(argv[1], argv + 1);
	fprintf(stderr, "%s: %s: %s\n", argv[0], argv[1], strerror(errno));
	if (errno == ENOENT) {
		return NOT_FOUND;
	}
	return NOT_RUN;
}

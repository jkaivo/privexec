#define _XOPEN_SOURCE 700
#include <errno.h>
#include <grp.h>
#include <locale.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <pwd.h>
#include <unistd.h>

#include "check.h"

static char *progname = NULL;

void fatal(int include_errno, char *fmt, ...)
{
	fprintf(stderr, "%s: ", progname);
	va_list ap;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
	if (include_errno) {
		fprintf(stderr, ": %s", strerror(errno));
	}
	fputc('\n', stderr);
	exit(EXIT_FAILURE);
}

static char *get_username(void)
{
	struct passwd *pwd = getpwuid(getuid());
	if (pwd == NULL) {
		fatal(1, "unable to determine user name");
	}
	return pwd->pw_name;
}

static char *get_groupname(void)
{
	struct group *grp = getgrgid(getgid());
	if (grp == NULL) {
		fatal(1, "unable to determine group name");
	}
	return grp->gr_name;
}

static char *get_command(int argc, char *argv[])
{
	if (argc < 2) {
		fatal(1, "missing operands");
	}

	return argv[1];
}

int main(int argc, char *argv[])
{
	setlocale(LC_ALL, "");
	progname = argv[0];

	char *cmd = get_command(argc, argv);
	char *user = get_username();
	char *group = get_groupname();

	openlog(PRIVEXEC_LOG_ID, LOG_PID, LOG_AUTH);
	syslog(LOG_INFO, "checking %s:%s for permission to run %s",
		user, group, cmd);

	switch (get_permission(user, group, cmd)) {
	case AUTH_GROUP_ALL:
	case AUTH_GROUP_CMD:
	case AUTH_USER_ALL:
	case AUTH_USER_CMD:
		syslog(LOG_INFO, "%s:%s requires authentication to run %s",
			user, group, cmd);
		if (authenticate(user) != 0) {
			syslog(LOG_NOTICE, "%s:%s failed authentication for %s",
				user, group, cmd);
			fatal(0, "bad authentication");
		}
		/* FALLTHRU */

	case PASS_GROUP_ALL:
	case PASS_GROUP_CMD:
	case PASS_USER_ALL:
	case PASS_USER_CMD:
		syslog(LOG_INFO, "%s:%s authorized to run %s",
			user, group, cmd);
		return 0;

	case DENY_GROUP_ALL:
	case DENY_GROUP_CMD:
	case DENY_USER_ALL:
	case DENY_USER_CMD:
		syslog(LOG_NOTICE,
			"%s:%s explicitly denied permission to run %s",
			user, group, cmd);
		fatal(0, "explicitly denied");
		return 1;
	
	case UNKNOWN:
	default:
		syslog(LOG_NOTICE,
			"%s:%s denied permission to run %s by default",
			user, group, cmd);
		fatal(0, "denied by default");
	}

	return 1;
}

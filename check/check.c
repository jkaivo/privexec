#define _POSIX_C_SOURCE 200809L
#include <errno.h>
#include <grp.h>
#include <locale.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

static int authorize(const char *user)
{
	(void)user;
	return 1;
}

static char *get_username(void)
{
	struct passwd *pwd = getpwuid(getuid());
	if (pwd == NULL) {
		fatal(1, "unable to determine user name");
	}
	printf("checking for user %s\n", pwd->pw_name);
	return pwd->pw_name;
}

static char *get_groupname(void)
{
	struct group *grp = getgrgid(getgid());
	if (grp == NULL) {
		fatal(1, "unable to determine group name");
	}
	printf("checking for group %s\n", grp->gr_name);
	return grp->gr_name;
}

static char *get_command(int argc, char *argv[])
{
	if (argc < 2) {
		fatal(1, "missing operands");
	}

	printf("checking command %s\n", argv[1]);
	return argv[1];
}

int main(int argc, char *argv[])
{
	setlocale(LC_ALL, "");
	progname = argv[0];

	char *cmd = get_command(argc, argv);
	char *user = get_username();
	char *group = get_groupname();

	switch (get_permission(user, group, cmd)) {
	case NEED_AUTH:
		if (authorize(user) != 0) {
			fatal(0, "bad authorization");
		}
		/* FALLTHRU */
	case NO_AUTH:
		puts("granted");
		return 0;

	case DENIED:
		fatal(0, "explicitly denied");
		return 1;
	
	case UNKNOWN:
	default:
		fatal(0, "denied by default");
	}

	return 1;
}
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "check.h"

static enum permission eval(const char *keyword, const char *principal, const char *cmd, const char *user, const char *group, const char *command)
{
	enum permission_principal pp = 0;
	if (!strcmp(user, principal)) {
		pp = PERM_USER;
	} else if (principal[0] == ':' && !strcmp(group, principal + 1)) {
		pp = PERM_GROUP;
	}

	enum permission_command pc = 0;
	if (cmd == NULL) {
		pc = PERM_ALL;
	} else if (!strcmp(cmd, command)) {
		pc = PERM_CMD;
	}

	enum permission_keyword pk = 0;
	if (!strcmp(keyword, "authorize")) {
		pk = PERM_PASS;
	} else if (!strcmp(keyword, "authenticate")) {
		pk = PERM_AUTH;
	} else if (!strcmp(keyword, "deny")) {
		pk = PERM_DENY;
	} else {
		fatal(0, "invalid keyword: %s", keyword);
	}

	if (pp == 0 || pc == 0) {
		return UNKNOWN;
	}

	return pp | pc | pk;
}

enum permission get_permission(const char *user, const char *group, const char *command)
{
	enum permission perm = UNKNOWN;
	FILE *f = fopen(CONFIG_PATH, "r");
	if (f == NULL) {
		fatal(1, CONFIG_PATH);
	}

	ssize_t s = 0;
	char *buf = NULL;
	size_t len = 0;
	while ((s = getline(&buf, &len, f)) > 0) {
		if (*buf == '#') {
			continue;
		}
		if (*buf == '\n') {
			continue;
		}
		buf[s-1] = '\0';

		char *space = strchr(buf, ' ');
		if (!space) {
			fatal(0, "invalid line in config: %s", buf);
		}	

		char *keyword = buf;
		*space = '\0';

		char *principal = space + 1;
		char *cmd = NULL;
		space = strchr(principal, ' ');
		if (space) {
			*space = '\0';
			cmd = space + 1;
		}

		enum permission tmp = eval(keyword, principal, cmd, user, group, command);
		/* only change if a higher precedence is found */
		if (tmp > perm) {
			perm = tmp;
		}
	}
	if (s == -1 && ferror(f)) {
		fatal(1, "reading configuration");
	}

	if (buf) {
		free(buf);
	}
	if (f) {
		fclose(f);
	}
	return perm;
}

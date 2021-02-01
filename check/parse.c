#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "check.h"

enum permission get_permission(const char *user, const char *group, const char *cmd)
{
	(void)user; (void)group; (void)cmd;
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

		char *space = strchr(buf, ' ');
		if (!space) {
			fatal(0, "invalid line in config: %s\n", buf);
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

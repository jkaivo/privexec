#ifndef PRIVEXEC_CHECK_H
#define PRIVEXEC_CHECK_H

#ifndef CONFIG_PATH
#define CONFIG_PATH	"/etc/privexec.conf"
#endif

enum permission { UNKNOWN, NO_AUTH, NEED_AUTH, DENIED };

void fatal(int include_errno, char *fmt, ...);
enum permission get_permission(const char *user, const char *group, const char *cmd);

#endif

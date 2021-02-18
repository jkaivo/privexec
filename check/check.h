#ifndef PRIVEXEC_CHECK_H
#define PRIVEXEC_CHECK_H

#ifndef CONFIG_PATH
#define CONFIG_PATH		"/etc/privexec.conf"
#endif

#ifndef PAM_SERVICE_NAME
#define PAM_SERVICE_NAME	"privexec"
#endif

#ifndef PRIVEXEC_LOG_ID
#define PRIVEXEC_LOG_ID		"privexec"
#endif

enum permission { UNKNOWN, AUTHORIZED, AUTHENTICATE, DENIED };

void fatal(int include_errno, char *fmt, ...);
enum permission get_permission(const char *user, const char *group, const char *cmd);
int authenticate(const char *user);

#endif

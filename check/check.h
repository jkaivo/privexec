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

enum permission_keyword {
	PERM_PASS = 0x1,
	PERM_AUTH = 0x2,
	PERM_DENY = 0x3,
};

enum permission_principal {
	PERM_GROUP = 0x100,
	PERM_USER = 0x200,
};

enum permission_command {
	PERM_ALL = 0x10,
	PERM_CMD = 0x20,
};

enum permission {
	UNKNOWN,

	PASS_GROUP_ALL = PERM_PASS | PERM_GROUP | PERM_ALL,
	AUTH_GROUP_ALL = PERM_AUTH | PERM_GROUP | PERM_ALL,
	DENY_GROUP_ALL = PERM_DENY | PERM_GROUP | PERM_ALL,

	PASS_GROUP_CMD = PERM_PASS | PERM_GROUP | PERM_CMD,
	AUTH_GROUP_CMD = PERM_AUTH | PERM_GROUP | PERM_CMD,
	DENY_GROUP_CMD = PERM_DENY | PERM_GROUP | PERM_CMD,

	PASS_USER_ALL = PERM_PASS | PERM_USER | PERM_ALL,
	AUTH_USER_ALL = PERM_AUTH | PERM_USER | PERM_ALL,
	DENY_USER_ALL = PERM_DENY | PERM_USER | PERM_ALL,

	PASS_USER_CMD = PERM_PASS | PERM_USER | PERM_CMD,
	AUTH_USER_CMD = PERM_AUTH | PERM_USER | PERM_CMD,
	DENY_USER_CMD = PERM_DENY | PERM_USER | PERM_CMD,
};


void fatal(int include_errno, char *fmt, ...);
enum permission get_permission(const char *user, const char *group, const char *cmd);
int authenticate(const char *user);

#endif

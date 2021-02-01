#include <stdio.h>

#include <security/pam_appl.h>
#include <security/pam_misc.h>

#include "check.h"

/*
static int pam_conv_f(int num_msg, const struct pam_message **msg, struct pam_response **resp, void *appdata_ptr)
{
	(void)num_msg;
	(void)msg;
	printf("msg: %s\n", (*msg)->msg);
	(void)resp;
	printf("resp: %s\n", (*resp)->resp);
	(void)appdata_ptr;
	return 0;
}
*/

int authenticate(const char *user)
{
	int ret = 1;
	pam_handle_t *pamh = NULL;
	struct pam_conv conv = {
		.conv = misc_conv,
	};

	if (pam_start(PAM_SERVICE_NAME, user, &conv, &pamh) != PAM_SUCCESS) {
		fatal(0, "PAM error");
		return 1;
	}

	if (pam_authenticate(pamh, 0) == PAM_SUCCESS) {
		ret = 0;
	}

	pam_end(pamh, 0);

	return ret;
}

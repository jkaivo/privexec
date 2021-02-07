#include <stdio.h>

#include <security/pam_appl.h>
#include <security/pam_misc.h>

#include "check.h"

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

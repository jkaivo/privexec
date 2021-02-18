PrivExec is an attempt to provide administrators with a way to allow privelige
escalation in a manner similar to `sudo`, but in a more resilient way by
splitting different stages of the escalation process into separate binaries.

PrivExec consists of three binaries:

- `privexec`
- `check`
- `exec`

The first, `privexec`, is the user-facing binary. It should be installed in
a directory that is part of the global `$PATH`. In order to function properly,
`privexec` must be installed SETGID (*NOT* SETUID) to a group that contains
not actual users (we suggest `_privexec` for this purpose).

The other two binaries should be installed with group ownership set to the
same group, and permissions set to allow user and group execution for both,
with other execution disabled, as well as `root` ownership and SETUID for
`exec`. In summary, you should have something like:

    $ ls -l /usr/local/bin/privexec /usr/local/lib/privexec/check /usr/local/lib/privexec/exec
    -rwxr-sr-x 1 root _privexec 19888 Feb  1 10:32 /usr/local/bin/privexec
    -r-xr-x--- 1 root _privexec 19888 Feb  1 10:33 /usr/local/lib/privexec/check
    -r-sr-x--- 1 root _privexec 19456 Feb  1 10:13 /usr/local/lib/privexec/exec

The front-end binary is responsible for calling the `check` binary to verify
whether the user is authorized to execute the given command, and then calling
the `exec` binary to execute the command with elevated privileges if and only
if the `check` command positively confirms authorization. The `exec` command
is obviously SETUID to enable privilege escalation. The `privexec` command is
SETGID to restrict the execution of the other two commands. The `check` command
is largely unprivileged (though administrators may wish to restrict its
configuration file to be readable only by the `_privexec` group to prohibit
casual inspection of which accounts are allowed to elevate privileges), and
is the location of all complexity. It is responsible for parsing
`/etc/privexec.conf` and determining whether the given user is authorized to
run the given command.

Configuration is done via `/etc/privexec.conf`. The format is intentionally
strict. Each may be blank, a comment (beginning with a '#' character in the
first column), or a directive. Directives are of the form:

    <keyword> <principal> [command]

Where `<keyword>` is one of `authorize`, `authenticate`, or `deny`;
`<principal>` is either a username or a group name prepended with ':', and
`[command]` is an optional command. Tokens must be separated by exactly one
space. Additional white space is not allowed.

The entire configuration file is parsed whenever `privexec` invokes `check`.
Any syntax errors will result in failure. Privilege checking is performed so
as to be most restrictive. In order from least to most:

    `authorize` - The user is authorized to execute the associate command
    without further interaction.

    `authenticate` - The user must authenticate themself before the command
    is executed. This is handled by PAM with the service name `privexec`.

    `deny` - The user is not permitted to execute the command.

A user name match has higher precedence than a group match, and a match
containing a program name has higher precedence than a match without the
program name. So the total ordering of precedence (from least to most) is:

    authorize :group
    authenticate :group
    deny :group
    authorize :group command
    authenticate :group command
    deny :group command
    authorize user
    authenticate user
    deny user
    authorize user command
    authenticate user command
    deny user command

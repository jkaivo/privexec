.POSIX:

PRIVEXECBINDIR=/usr/local/bin
PRIVEXECDIR=/usr/local/lib/privexec
PRIVEXECGROUP=_privexec
PRIVCHECKGROUP=_privcheck

all:
	cd check; make
	cd exec; make
	cd privexec; make

install: all
	mkdir -p $(PRIVEXECDIR)
	cp -f check/check $(PRIVEXECDIR)
	strip $(PRIVEXECDIR)/check
	chown root:$(PRIVCHECKGROUP) $(PRIVEXECDIR)/check
	chmod 2555 $(PRIVEXECDIR)/check

	cp -f exec/exec $(PRIVEXECDIR)
	strip $(PRIVEXECDIR)/exec
	chown root:$(PRIVEXECGROUP) $(PRIVEXECDIR)/exec
	chmod 4550 $(PRIVEXECDIR)/exec

	mkdir -p $(PRIVEXECBINDIR)
	cp -f privexec/privexec $(PRIVEXECBINDIR)
	strip $(PRIVEXECBINDIR)/privexec
	chown root:$(PRIVEXECGROUP) $(PRIVEXECBINDIR)/privexec
	chmod 2555 $(PRIVEXECBINDIR)/privexec

clean:
	cd check; make clean
	cd exec; make clean
	cd privexec; make clean

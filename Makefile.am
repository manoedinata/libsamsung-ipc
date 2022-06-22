NULL =

SUBDIRS = \
	samsung-ipc \
	samsung-ipc/tests \
	include \
	tools \
	tools/ipc-modem \
	$(NULL)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = samsung-ipc.pc

EXTRA_DIST = \
	Android.mk \
	android_versions.mk \
	CONTRIBUTING \
	MAINTAINERS \
	samsung-ipc.pc \
	strict-cflags.sh \
	tools/include/glibc/sysexits.h \
	$(NULL)

MAINTAINERCLEANFILES = \
	aclocal.m4 compile config.guess config.sub \
	configure depcomp install-sh ltmain.sh     \
	Makefile.in missing config.h.in            \
	mkinstalldirs *~ \
	$(NULL)

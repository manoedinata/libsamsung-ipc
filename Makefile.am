NULL =

EXTRA_DIST = $(NULL)

SUBDIRS = \
	samsung-ipc \
	samsung-ipc/tests \
	include \
	tools \
	tools/ipc-modem \
	$(NULL)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = samsung-ipc.pc

EXTRA_DIST += \
	Android.mk \
	android_versions.mk \
	CONTRIBUTING \
	INSTALL \
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

################################################################################
#                                 Extra checks                                 #
################################################################################

if WANT_CODE_COVERAGE
check-local: build-code-coverage check-tarball-builds
install-data-local: install-code-coverage
clean-local: clean-code-coverage
else # WANT_CODE_COVERAGE
check-local: check-tarball-builds
endif # WANT_CODE_COVERAGE

# With the next release of libsamsung-ipc, we will also release
# tarballs.
#
# Until now, no automatic testing was done with the libsamsung-ipc
# tarballs (all tests were done with the libsamsung-ipc git
# repository).
#
# We also need to verify that everything also builds and that the
# tests also work with tarballs. This can spot mistakes where people
# add new files that are not automatically picked up by automake and
# forget to add these files to EXTRA_DIST.
#
# This test runs only from git because it is not necessary with a
# tarball build. If we wanted to run it anyway we would end up in an
# infinite recursion as a given test from a tarball would then spawn
# another build and test from a tarball which would then spawn again
# another build and test.
check-tarball-builds: dist
	if [ -d $(srcdir)/.git ] ; then \
		BUILD_TMPDIR=`mktemp -d` && \
		echo $$BUILD_TMPDIR && \
		mkdir $$BUILD_TMPDIR/build $$BUILD_TMPDIR/destdir && \
		tar xf $(srcdir)/libsamsung-ipc-$(VERSION).tar.xz \
		    -C $$BUILD_TMPDIR && \
		cd $$BUILD_TMPDIR/build && \
		../libsamsung-ipc-$(VERSION)/configure \
			$(CONFIGURE_ARGUMENTS) && \
		make check && \
		make install DESTDIR=`realpath $$BUILD_TMPDIR/destdir` && \
		rm -rf $$BUILD_TMPDIR ; \
	fi

#################
# Code coverage #
#################
if WANT_CODE_COVERAGE

CODE_COVERAGE_DIR = lcov

EXTRA_DIST += $(CODE_COVERAGE_DIR)

build-code-coverage:
	lcov \
		--capture \
		--compat-libtool \
		--directory . \
		--output-file libsamsung-ipc.info
	genhtml -o $(CODE_COVERAGE_DIR) libsamsung-ipc.info

install-code-coverage: check
	install -d $(DESTDIR)/$(datadir)/libsamsung-ipc/$(CODE_COVERAGE_DIR)/
	find $(srcdir)/$(CODE_COVERAGE_DIR) -type f | \
		xargs -I src install -D src \
		$(DESTDIR)/$(datadir)/libsamsung-ipc/$(CODE_COVERAGE_DIR)/

clean-code-coverage:
	rm -rf $(CODE_COVERAGE_DIR)

endif # WANT_CODE_COVERAGE

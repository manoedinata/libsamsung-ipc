AC_INIT([libsamsung-ipc], [0.7.0], [replicant@osuosl.org], [libsamsung-ipc])
AC_CONFIG_SRCDIR([Makefile.am])
AC_CONFIG_HEADERS(config.h)
AM_INIT_AUTOMAKE([dist-bzip2 dist-xz subdir-objects])
XZ_OPT=-v9e
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

OPENSSL_REQUIRED=1.0.0e
LIBCURL_REQUIRED=7.62.0

#------------------------------------------------------------------------------
# pkg-config

PKG_PROG_PKG_CONFIG

#------------------------------------------------------------------------------
# openssl

PKG_CHECK_MODULES(OPENSSL, openssl >= $OPENSSL_REQUIRED)
AC_SUBST(OPENSSL_CFLAGS)
AC_SUBST(OPENSSL_LIBS)

#------------------------------------------------------------------------------
# valgrind
AC_SUBST(VALGRIND)

#------------------------------------------------------------------------------
# libcurl
PKG_CHECK_MODULES(LIBCURL, libcurl >= $LIBCURL_REQUIRED)
AC_SUBST(LIBCURL_CFLAGS)
AC_SUBST(LIBCURL_LIBS)

#------------------------------------------------------------------------------
# check for debugging
AC_ARG_ENABLE(debug,
  [AS_HELP_STRING([--enable-debug], [Enable debug build (default=disabled)])],
  [debug=$enableval],
  [debug="no"])
AM_CONDITIONAL( [WANT_DEBUG], [test x"$debug" = x"yes"])

#------------------------------------------------------------------------------
# TODO: GCC has --enable-checking= for extensive runtime checks and one of the
#       available values is valgrind, so it would be a good idea to convert to
#       it for consistency across free software projects.
AC_ARG_ENABLE(
  [valgrind-tests],
  [AS_HELP_STRING([--enable-valgrind-tests],
                  [Build with -ggdb3, -O0 and use valgrind from PATH during
                   tests (default=disabled)])],
  [valgrind_tests=$enableval],
  [valgrind_tests="no"])

AM_CONDITIONAL( [WANT_VALGRIND_CHECKING], [test x"$valgrind_tests" = x"yes"])

AS_IF([test x"$valgrind_tests" = x"yes"],
  [AC_CHECK_PROG([VALGRIND], [valgrind], [valgrind])
   AS_IF(
     [test x"$VALGRIND" = x""],
     [AC_MSG_ERROR(
       [valgrind tests are enabled but valgrind was not found in PATH ($PATH)])])])
#------------------------------------------------------------------------------
AC_CONFIG_FILES([
    Makefile
    samsung-ipc.pc
    include/Makefile
    samsung-ipc/Makefile
    samsung-ipc/tests/Makefile
    tools/Makefile
    tools/ipc-modem/Makefile
])


#------------------------------------------------------------------------------
AS_IF([test x"$valgrind_tests" = x"yes"], [: ${CFLAGS="-ggdb3 -O0"}],
      [test x"debug" = x"yes"], [: ${CFLAGS="-ggdb -O0"}], [])

AC_PROG_CC
AM_PROG_CC_C_O
AC_GNU_SOURCE
AC_DISABLE_STATIC
AC_PROG_LIBTOOL

AC_SUBST(CFLAGS)
AC_SUBST(CPPFLAGS)
AC_SUBST(LDFLAGS)

AC_OUTPUT
#------------------------------------------------------------------------------
# info

echo "------------------------------------------------------------------------"
echo "$PACKAGE_NAME $PACKAGE_VERSION"
echo "------------------------------------------------------------------------"
echo
echo "Configuration Options:"
echo
echo "  debug build.............: $debug"
echo
echo "  prefix..................: $prefix"
echo
echo "  valgrind tests..........: $valgrind_tests"
echo
echo "------------------------------------------------------------------------"
echo
echo "Now type 'make' to compile and 'make install' to install this package."


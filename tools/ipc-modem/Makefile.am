NULL =
EXTRA_DIST = $(NULL)

AM_CFLAGS = \
	-I$(top_srcdir)/include \
	$(NULL)

if WANT_STRICT_CFLAGS
AM_CFLAGS += $(STRICT_CFLAGS)
endif

bin_PROGRAMS = ipc-modem

# TODO: Find a way to make test more modular and represent each run of the
# ipc-modem in TEST while having it implemented in a single python file
PY_LOG_COMPILER = $(PYTHON)
TEST_EXTENSIONS = .py
TESTS = tests/ipc-modem.py
EXTRA_DIST += $(TESTS)

ipc_modem_SOURCES = \
ipc-modem.c \
ipc-modem.h \
ipc-modem-log.c \
ipc-modem-log.h

ipc_modem_LDADD = $(top_builddir)/samsung-ipc/libsamsung-ipc.la
ipc_modem_LDFLAGS = -lpthread

NULL =

AM_CFLAGS = \
	-I$(top_srcdir)/include \
	$(NULL)

bin_PROGRAMS = ipc-modem

# TODO: Find a way to make test more modular and represent each run of the
# ipc-modem in TEST while having it implemented in a single python file
PY_LOG_COMPILER = $(PYTHON)
TEST_EXTENSIONS = .py
TESTS = tests/ipc-modem.py

ipc_modem_SOURCES = ipc-modem.c ipc-modem-log.c
ipc_modem_LDADD = $(top_builddir)/samsung-ipc/libsamsung-ipc.la
ipc_modem_LDFLAGS = -lpthread

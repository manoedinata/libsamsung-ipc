NULL =
EXTRA_DIST = $(NULL)

AM_CFLAGS = \
	-I$(top_srcdir)/include \
	$(LIBCURL_CFLAGS) \
	$(NULL)

if WANT_STRICT_CFLAGS
AM_CFLAGS += $(STRICT_CFLAGS)
endif

bin_PROGRAMS = \
	https-send-sms \
	ipc-test \
	nv_data-imei \
	nv_data-md5 \
	$(NULL)

# TODO: Find a way to make test more modular and represent each run of the
# nv_data-imei in TEST while having it implemented in a single python file
if WANT_VALGRIND_CHECKING
AM_TESTS_ENVIRONMENT = VALGRIND='$(VALGRIND)'; export VALGRIND;
endif
PY_LOG_COMPILER = $(PYTHON3)
TEST_EXTENSIONS = .py
TESTS = \
	tests/nv_data-imei.py \
	tests/nv_data-md5.py
EXTRA_DIST += $(TESTS)

https_send_sms_SOURCES = https-send-sms.c
https_send_sms_LDADD = $(LIBCURL_LIBS)
https_send_sms_LDFLAGS =

ipc_test_SOURCES = ipc-test.c
ipc_test_LDADD = $(top_builddir)/samsung-ipc/libsamsung-ipc.la
ipc_test_LDFLAGS =

nv_data_md5_SOURCES = nv_data-md5.c
nv_data_md5_LDADD = $(top_builddir)/samsung-ipc/libsamsung-ipc.la
nv_data_md5_LDFLAGS =

nv_data_imei_SOURCES = nv_data-imei.c nv_data-imei.h
nv_data_imei_LDADD = $(top_builddir)/samsung-ipc/libsamsung-ipc.la
nv_data_imei_LDFLAGS =

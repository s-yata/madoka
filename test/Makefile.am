AM_CFLAGS = -Wall -Wextra -I../lib
AM_CXXFLAGS = -Wall -Weffc++ -Wextra -I../lib -std=c++0x

LIBMADOKA_LDADD = ../lib/madoka/libmadoka.la

TESTS = \
  util-test \
  exception-test \
  approx-test \
  header-test \
  file-test \
  croquis-test \
  sketch-test \
  c-test

check_PROGRAMS = ${TESTS}

util_test_SOURCES = util-test.cc
util_test_LDADD = ${LIBMADOKA_LDADD}

exception_test_SOURCES = exception-test.cc
exception_test_LDADD = ${LIBMADOKA_LDADD}

approx_test_SOURCES = approx-test.cc
approx_test_LDADD = ${LIBMADOKA_LDADD}

header_test_SOURCES = header-test.cc
header_test_LDADD = ${LIBMADOKA_LDADD}

file_test_SOURCES = file-test.cc
file_test_LDADD = ${LIBMADOKA_LDADD}

croquis_test_SOURCES = croquis-test.cc
croquis_test_LDADD = ${LIBMADOKA_LDADD}

sketch_test_SOURCES = sketch-test.cc
sketch_test_LDADD = ${LIBMADOKA_LDADD}

c_test_SOURCES = c-test.c
c_test_LDADD = ${LIBMADOKA_LDADD} -lstdc++

if HAVE_PTHREAD
TESTS += thread-test

thread_test_SOURCES = thread-test.cc
thread_test_LDADD = ${LIBMADOKA_LDADD}
thread_test_LDFLAGS = -pthread
endif

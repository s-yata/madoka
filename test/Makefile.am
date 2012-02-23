AM_CFLAGS = -Wall -Wextra -I../lib
AM_CXXFLAGS = -Wall -Weffc++ -Wextra -I../lib -std=c++0x

TESTS = \
  exception-test \
  approx-test \
  header-test \
  file-test \
  croquis-test \
  sketch-test \
  c-test

check_PROGRAMS = $(TESTS)

exception_test_SOURCES = exception-test.cc
exception_test_LDADD = ../lib/libmadoka.la

approx_test_SOURCES = approx-test.cc
approx_test_LDADD = ../lib/libmadoka.la

header_test_SOURCES = header-test.cc
header_test_LDADD = ../lib/libmadoka.la

file_test_SOURCES = file-test.cc
file_test_LDADD = ../lib/libmadoka.la

croquis_test_SOURCES = croquis-test.cc
croquis_test_LDADD = ../lib/libmadoka.la

sketch_test_SOURCES = sketch-test.cc
sketch_test_LDADD = ../lib/libmadoka.la

c_test_SOURCES = c-test.c
c_test_LDADD = ../lib/libmadoka.la -lstdc++

if HAVE_PTHREAD
TESTS += thread-test

thread_test_SOURCES = thread-test.cc
thread_test_LDADD = ../lib/libmadoka.la
thread_test_LDFLAGS = -pthread
endif

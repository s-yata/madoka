AM_CXXFLAGS = -Wall -Weffc++ -Wextra -I../lib

LIBMADOKA_LDADD = ../lib/madoka/libmadoka.la

bin_PROGRAMS = \
  madoka \
  madoka-benchmark

noinst_HEADERS = timer.h

madoka_SOURCES = madoka.cc
madoka_LDADD = ${LIBMADOKA_LDADD}

madoka_benchmark_SOURCES = madoka-benchmark.cc
madoka_benchmark_LDADD = ${LIBMADOKA_LDADD}
madoka_benchmark_LDFLAGS = -lm

if HAVE_CLOCK_GETTIME
madoka_benchmark_LDFLAGS += -lrt
endif

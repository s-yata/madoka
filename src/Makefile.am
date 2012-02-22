AM_CXXFLAGS = -Wall -Weffc++ -Wextra -I../lib

bin_PROGRAMS = \
  madoka \
  madoka-benchmark

noinst_HEADERS = timer.h

madoka_SOURCES = madoka.cc
madoka_LDADD = ../lib/libmadoka.la

madoka_benchmark_SOURCES = madoka-benchmark.cc
madoka_benchmark_LDADD = ../lib/libmadoka.la
madoka_benchmark_LDFLAGS = -lm

if HAVE_CLOCK_GETTIME
madoka_benchmark_LDFLAGS += -lrt
endif

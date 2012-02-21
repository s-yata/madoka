AM_CFLAGS = -Wall -Wextra
AM_CXXFLAGS = -Wall -Weffc++ -Wextra -I../lib

bin_PROGRAMS = \
  madoka \
  madoka-benchmark

madoka_SOURCES = madoka.cc
madoka_LDADD = ../lib/libmadoka.la

madoka_benchmark_SOURCES = madoka-benchmark.cc
madoka_benchmark_LDADD = ../lib/libmadoka.la
madoka_benchmark_LDFLAGS = -lm

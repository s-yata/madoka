AM_CXXFLAGS = -Wall -Weffc++ -Wextra
ACLOCAL_AMFLAGS = -I m4

lib_LTLIBRARIES = libmadoka.la

libmadoka_la_SOURCES = \
  madoka/approx.cc \
  madoka/file.cc \
  madoka/sketch.cc

include_HEADERS = madoka.h

libmadoka_includedir = ${includedir}/madoka
libmadoka_include_HEADERS = \
  madoka/approx.h \
  madoka/croquis.h \
  madoka/exception.h \
  madoka/file.h \
  madoka/hash.h \
  madoka/header.h \
  madoka/random.h \
  madoka/sketch.h \
  madoka/util.h

noinst_HEADERS =

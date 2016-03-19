#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.67])
AC_INIT([madoka], [0.0.2], [susumu.yata@gmail.com])
AC_CONFIG_SRCDIR([lib/madoka.h])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE([foreign])

# Checks for programs.
LT_INIT
AC_PROG_CXX
AC_PROG_CC
AC_PROG_INSTALL

AC_CONFIG_MACRO_DIR([m4])

# Checks for libraries.
AC_CHECK_LIB([pthread], [pthread_create], [have_pthread="true"],
             [have_pthread="false"])
AM_CONDITIONAL([HAVE_PTHREAD], ["${have_pthread}"])

AC_CHECK_LIB([rt], [clock_gettime], [have_clock_gettime="true"],
             [have_clock_gettime="false"])
AM_CONDITIONAL([HAVE_CLOCK_GETTIME], ["${have_clock_gettime}"])

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AC_CONFIG_FILES([Makefile
                 madoka.pc
                 lib/Makefile
                 lib/madoka/Makefile
                 src/Makefile
                 test/Makefile])
AC_OUTPUT

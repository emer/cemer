#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT([maketa],[4.0],pdp-discuss@psych-srv3.Colorado.EDU)
AC_CONFIG_AUX_DIR([aux])
AM_INIT_AUTOMAKE([foreign])

m4_undefine([AC_PROG_F77])
m4_defun([AC_PROG_F77],[])
AC_PROG_LIBTOOL

AC_CONFIG_SRCDIR([mta_parse.y])
AC_CONFIG_HEADER([maketa_config.h])

# Checks for programs.
AC_PROG_CXX

PDP_DETERMINE_OSTYPE
AC_DEFINE([NO_TA_BASE], [],[Don't use ta base...])
AC_DEFINE([TA_NO_GUI], [], [Description])
# Checks for header files.
AC_FUNC_ALLOCA
AC_HEADER_STDC
AC_CHECK_HEADERS([stddef.h stdlib.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T

# Checks for library functions.
AC_TYPE_SIGNAL
AC_CHECK_FUNCS([strtol])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT

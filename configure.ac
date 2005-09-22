#                                            -*- Autoconf -*-
#*************************************************************
# Process this file with autoconf to produce a configure script.
#*************************************************************
#

AC_PREREQ(2.59)
AC_INIT([PDP],[4.0],[pdp-discuss@psych-srv3.Colorado.EDU])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_DISABLE_SHARED
AC_PROG_LIBTOOL
PDP_DETERMINE_OSTYPE
#AC_CONFIG_SRCDIR([src/taglobal.h])

#                                             debug, mpi, gui
#*************************************************************
#          Enables/disables debugging, mpi, gui
#*************************************************************
#

AC_ARG_ENABLE(debug,
              AC_HELP_STRING([--enable-debug],
                             [Enable debugging. Default is disabled.]),
                             [debug=true],
                             [debug=false])
AC_ARG_ENABLE([mpi],
              AC_HELP_STRING([--enable-mpi],
                             [Enable the Message Passing Interface. Default is disabled.]),
                             [mpi=true],
                             [mpi=false])
AC_ARG_ENABLE(gui,
              AC_HELP_STRING([--disable-gui],
                             [Disable compiling with a GUI. Default is enabled]),
                             [gui=false],
                             [gui=true])
AM_CONDITIONAL([DEBUG],[test $debug = true])
AM_CONDITIONAL([MPI],[test $mpi = true])
AM_CONDITIONAL([TA_GUI],[test $gui = true])
AM_CONDITIONAL([NO_TA_GUI],[test $gui = false])

#                                                     programs
#*************************************************************
#          Checks for specific programs
#*************************************************************
#

AC_PROG_AWK
AC_PROG_YACC
AC_PROG_LEX
AC_PROG_CXX
AC_PROG_CC([gcc])

# GNU C Compiler Collection is mandatory
AC_MSG_CHECKING([for the GNU C Compiler Collection])
if test $GCC != yes; then
	AC_MSG_ERROR([GNU C Compiler Collection not detected],[1])
fi
AC_MSG_RESULT([$GCC])

# GNU Make is mandatory. CHECK_GNU_MAKE is a custom macro that
# warns if it is not available
AC_PROG_MAKE_SET
CHECK_GNU_MAKE

#                                                      headers
#*************************************************************
#              Checks for specific headers
#*************************************************************
#
if test $gui = true; then
	AC_PATH_XTRA
fi
AC_FUNC_ALLOCA
AC_HEADER_DIRENT
AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS([fcntl.h float.h limits.h locale.h malloc.h memory.h stddef.h stdlib.h string.h sys/time.h unistd.h])

#                                              typedef, struct
#*************************************************************
#  Checks for typedefs, structures, compiler characteristics
#*************************************************************
#
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_TYPE_PID_T
AC_C_RESTRICT
AC_TYPE_SIZE_T
AC_HEADER_TIME
AC_C_VOLATILE

#                                                    libraries
#*************************************************************
#                     Checks for libraries
#*************************************************************
#
if test $gui = true; then
	gw_CHECK_QT
	AC_CHECK_LIB([Coin], [main])
	AC_CHECK_LIB([GLU], [main])
	AC_CHECK_LIB([GL], [main])
	AC_CHECK_LIB([jpeg], [main])
	AC_CHECK_LIB([SoQt], [main])
fi
AC_CHECK_LIB([dl], [main])
AC_CHECK_LIB([pthread], [main])
AC_CHECK_LIB([readline], [main],[],[VL_LIB_READLINE])
AC_CHECK_LIB([z], [main])

#                                            library functions
#*************************************************************
#                Checks for library functions
#*************************************************************
#
AC_FUNC_CHOWN
AC_FUNC_ERROR_AT_LINE
AC_FUNC_GETPGRP
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_FUNC_SELECT_ARGTYPES
AC_TYPE_SIGNAL
AC_FUNC_STAT
AC_FUNC_STRTOD
AC_CHECK_FUNCS([alarm floor getcwd gettimeofday memmove memset pow putenv rmdir select setlocale sqrt strcasecmp strtol])

#                                                  @variables@
#*************************************************************
#        Substitutes @style@ variables in makefiles
#*************************************************************
#
AC_SUBST([abs_top_builddir])
AC_SUBST([abs_srcdir])
AC_SUBST([includedir])
AC_SUBST([abs_srcdir])

#                                                       output
#*************************************************************
#                 Creation of the build system
#*************************************************************
#

AC_CONFIG_FILES([Makefile
		src/maketa/Makefile
		src/taiqtso/Makefile
		src/ta/Makefile
		src/css/Makefile
		src/tamisc/Makefile
		src/pdp/Makefile
		src/leabra/Makefile
		src/bp/Makefile])
AC_OUTPUT

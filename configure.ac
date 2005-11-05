#					     -*- Autoconf -*-
#*************************************************************
# Process this file with autoconf to produce a configure script.
#*************************************************************
#

# Autoconf 2.59 required
AC_PREREQ(2.59)
AC_INIT([PDP],[4.0],[pdp-discuss@psych-srv3.Colorado.EDU])

# Sticks config.guess  config.sub  depcomp  install-sh  ltmain.sh  missing
# in /aux/ so they don't clutter up the root
AC_CONFIG_AUX_DIR([aux])
AC_CONFIG_SUBDIRS([src/maketa])
# Scan the source with autoheader and create config.h
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE([-Wall -Werror])

# Custom: check results of config.status for OS type
PDP_DETERMINE_OSTYPE

# Disable shared libraries
AC_DISABLE_SHARED

# Set default language  C++
AC_LANG_CPLUSPLUS

# Next two lines are a hack to get rid of those stupid fortran checks
m4_undefine([AC_PROG_F77])
m4_defun([AC_PROG_F77],[])
AC_PROG_LIBTOOL

# Enable make rpm
AX_DIST_RPM([spec.tmpl])

#						     programs
#*************************************************************
#	      Checks for specific programs
#*************************************************************
#
AC_PROG_AWK
AC_PROG_YACC
AC_PROG_LEX
AC_PROG_CXXCPP
AC_REQUIRE_CPP

# GNU C++ compiler is mandatory

# Hack to stop fortran checks in configure
# See: http://lists.gnu.org/archive/html/libtool/2005-09/msg00097.html
m4_undefine([AC_PROG_F77])
m4_defun([AC_PROG_F77],[])

AC_PROG_CXX
AC_MSG_CHECKING([for GNU C++ compiler])
if test $GXX != yes; then
  AC_MSG_ERROR([GNU C++ compiler not detected])
fi
AC_MSG_RESULT([$GXX])
#						      headers
#*************************************************************
#	      Checks for specific headers
#*************************************************************
#
AC_FUNC_ALLOCA
AC_HEADER_DIRENT
AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS([fcntl.h float.h limits.h locale.h malloc.h memory.h stddef.h stdlib.h string.h sys/time.h unistd.h])
#					      typedef, struct
#*************************************************************
#  Checks for typedefs, structures, compiler characteristics
#*************************************************************
#
AC_HEADER_STDBOOL
AC_HEADER_TIME
AC_C_CONST
AC_C_INLINE
AC_C_RESTRICT
AC_C_VOLATILE
AC_TYPE_SIGNAL
AC_TYPE_SIZE_T
AC_TYPE_PID_T
#						    libraries
#*************************************************************
#		     Checks for libraries
#*************************************************************
#
AC_HAVE_LIBRARY([dl],
		[LIBS="$LIBS -ldl"],
		[AC_MSG_WARN([unable to find libdl for Dynamic Loader Function Interfaces. Build may fail.])])

# Souped up readline check
AC_HAVE_LIBRARY([readline],
		[LIBS="$LIBS -ldl"],
		[VL_LIB_READLINE])

AC_HAVE_LIBRARY([z],
		[LIBS="$LIBS -lz"],
		[AC_MSG_WARN([unable to find libz compression library. Build may fail.])])
#					    library functions
#*************************************************************
#		Checks for library functions
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
AC_CHECK_FUNCS([alarm floor getcwd gettimeofday memmove memset pow putenv rmdir select setlocale sqrt strcasecmp strtol],,[AC_MSG_WARN([not found.])])
 
#						  gui enabled
#*************************************************************
#	     Things to do if --enable-gui is set
#*************************************************************
#
AC_ARG_ENABLE([gui],
	      AC_HELP_STRING([--disable-gui],
			     [Disable compiling with a GUI. Default is enabled]),
			     [gui=false],
			     [gui=true])

AM_CONDITIONAL([TA_GUI],[test $gui = true])
AM_CONDITIONAL([NO_TA_GUI],[test $gui = false])

if test $gui = true; then	
  gw_CHECK_QT
  AC_HAVE_LIBRARY([Coin],[LIBS="$LIBS -lCoin"],[AC_MSG_WARN([--enable-gui set but unable to find libCoin.])])
  AC_HAVE_LIBRARY([GLU], [LIBS="$LIBS -lGLU"],[AC_MSG_WARN([--enable-gui set but unable to find libGLU.])])
  AC_HAVE_LIBRARY([GL], [LIBS="$LIBS -lGL"],[AC_MSG_WARN([--enable-gui set but unable to find libGL.])])
  AC_HAVE_LIBRARY([jpeg], [LIBS="$LIBS -ljpeg"],[AC_MSG_WARN([--enable-gui set but unable to find libjpeg.])])
  AC_HAVE_LIBRARY([SoQt], [LIBS="$LIBS -lSoQt"],[AC_MSG_WARN([--enable-gui set but unable to find libSoQt.])])
  AC_DEFINE([TA_GUI],
	    [],
	    [Complementary switches, set when compiling for gui/no gui. Controls all non-QtCore related dependencies. For nogui, there can be no Coin or non-QtCore dependencies.])
  AC_DEFINE([TA_USE_QT],
	    [],
	    [This switch is set when linking with Qt. NOTE: THIS FLAG DOES NOT INDICATE GUI -- ex. ta is compiled with the QtCore library, even for the non-gui version of the program])
  AC_DEFINE([TA_USE_INVENTOR],
	    [],
	    [This switch is set when compiling for gui and linking with Coin. This switch is really only used in the lowest level files. NOTE: Coin/Inventor is never used when building nogui.])
  AC_DEFINE([QT_SHARED],
	    [],
	    [Note from Brian: This switch is here, i'm not quite sure why. Used in qglobal.h.])
  # Tells top-level makefile.am to recurse through src/taiqtso
  AC_SUBST([WANT_TAIQTSO],[src/taiqtso])
  # These libraries are substituted in the makefiles.
  LIBS="$LIBS $X_PRE_LIBS $X_LIBS $X_EXTRA_LIBS $QT_LDADD"
  AC_SUBST([QT_CXXFLAGS])
  AC_SUBST([QT_GUILINK])
  AC_SUBST([X_CFLAGS])
else
  AC_DEFINE([TA_NO_GUI],[],[Set when building nogui])
fi
#						  mpi enabled
#*************************************************************
#	     Things to do if --enable-mpi is set
#*************************************************************
#
AC_ARG_ENABLE([mpi],
	      AC_HELP_STRING([--enable-mpi],
			     [Enable the Message Passing Interface. Default is disabled.]),
			     [mpi=true],
			     [mpi=false])

AM_CONDITIONAL([MPI],[test $mpi = true])

if test $mpi = true; then
  ACX_MPI([],[AC_MSG_ERROR([unable to locate the Message Passing Interface])])
  ACX_PTHREAD([],[AC_MSG_ERROR([unable to locate pthread])])
  AC_DEFINE([DMEM_COMPILE],[],[Causes mpi.h to be included])
  if test $MPICXX != x; then
    CXX="$MPICXX"
  fi
fi

#						debug enabled
#*************************************************************
#	     Things to do if --enable-debug is set
#*************************************************************
#
AC_ARG_ENABLE([debug],
	      AC_HELP_STRING([--enable-debug],
			     [Enable debugging. Default is disabled.]),
			     [debug=true],
			     [debug=false])

AM_CONDITIONAL([DEBUG],[test $debug = true])

if $debug = true; then
  AC_DEFINE([YYDEBUG],[],[Used when debugging])
  AC_DEFINE([DEBUG],[],[Used when debugging])
  AC_DEFINE([TA_PROFILE],[],[Used when debugging])
  AC_DEFINE([RETSIGTYPE],[void],[Used when debugging])
fi

#						  @variables@
#*************************************************************
#	    Substitutes @variables@ in Makefile.am's
# Descending order, so a recent substitution can contain a 
# @reference@ to a substitution that comes after.
#*************************************************************
#

AC_SUBST([abs_top_builddir])
AC_SUBST([abs_srcdir])
AC_SUBST([srcdir])
AC_SUBST([top_srcdir])

#						       output
#*************************************************************
#		 Creation of the build system
#*************************************************************
#
PDP_DETERMINE_SUFFIX
AC_CONFIG_FILES([Makefile
		src/taiqtso/Makefile
		src/ta/Makefile
		src/css/Makefile
		src/tamisc/Makefile
		src/pdp/Makefile
		src/leabra/Makefile
		src/bp/Makefile])
AC_OUTPUT

# This is a hack to get rid of -DHAVE_CONFIG_H
AC_CONFIG_COMMANDS_PRE([DEFS=])
AC_CONFIG_COMMANDS_POST([echo Configure finished successfully. If you encounter problems during make, run `./configure --help' and try to solve the problem that way. For further assistance send mail to pdp-discuss@psych-srv3.Colorado.EDU])

#					     -*- Autoconf -*-
#*************************************************************
# Process this file with autoconf to produce a configure script.
#*************************************************************
#
# General note: AC_MSG_ERROR is not used to halt our script
# in the case that a detection fails. It can make it difficult
# for a user to troubleshoot problems. Instead, issue
# SIM_AC_CONFIGURATION_WARNING which shows up at the very
# end of configure.

# Autoconf 2.59 required
AC_INIT([PDP],[4.0],[pdp-discuss@psych-srv3.Colorado.EDU],[PDP++])
AC_PREREQ([2.59])

AC_COPYRIGHT([
Copyright, 1995-2005, Regents of the University of Colorado,
Carnegie Mellon University, Princeton University.

This file is part of TA/PDP++

  TA/PDP++ is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  TA/PDP++ is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
])

# Sticks config.guess  config.sub  depcomp  install-sh  ltmain.sh  missing
# in /aux/ so they don't clutter up the root
AC_CONFIG_AUX_DIR([misc])
AC_CONFIG_SRCDIR([src/ta/taglobal.h])

# Scan the source with autoheader and create config.h
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE([-Wall -Werror gnu tar-pax nostdinc])

# Custom: check results of config.status for a few OS types
PDP_DETERMINE_OSTYPE

# Set default language  C++
AC_LANG_CPLUSPLUS
PDP_PROG_CXX
AC_CACHE_SAVE

#						  rpm enabled
#*************************************************************
#	     Things to do if --with-rpm is set
#*************************************************************
#

AC_ARG_WITH([rpm],
	      AC_HELP_STRING([--with-rpm],
			     [Enable the creation of rpms with `make rpm'. Note: You must first create a blank file called aminclude.am in the directory with configure. E.g., `touch aminclude.am'. Don't forget to set PLATFORM_SUFFIX. ]),
			     [rpm=true],
			     [rpm=false])

AM_CONDITIONAL([RPM],[test $rpm = true])

if $rpm = true; then
  AX_DIST_RPM([spec.tmpl])
  AC_SUBST([INCLUDE_AMINCLUDE_AM],["include aminclude.am"])
  SIM_AC_CONFIGURATION_SETTING([rpm],[enabled...PLATFORMSUFFIX=${PLATFORM_SUFFIX}])
fi


#						     programs
#*************************************************************
#	      Checks for specific programs
#*************************************************************
#

# Next two lines are a hack to get rid of those stupid fortran checks
m4_undefine([AC_PROG_F77])
m4_defun([AC_PROG_F77],[])

AC_PROG_CXXCPP
SIM_AC_CONFIGURATION_SETTING([Preprocessor],[$CXXCPP])

AC_PROG_LIBTOOL
AC_PROG_RANLIB
AC_PROG_MAKE_SET
AC_PROG_AWK
AC_PROG_YACC
AC_PROG_LEX
AC_CACHE_SAVE

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
AC_CACHE_SAVE

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
AC_CACHE_SAVE

#						    libraries
#*************************************************************
#		     Checks for libraries
#*************************************************************
#

AC_HAVE_LIBRARY([dl],
		[LIBS="$LIBS -ldl"],
		[SIM_AC_CONFIGURATION_WARNING([libdl not found])])
 
PGAC_CHECK_READLINE
CHECK_ZLIB
AC_CACHE_SAVE

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
AC_CHECK_FUNCS([alarm floor getcwd gettimeofday memmove memset pow putenv rmdir select setlocale sqrt strcasecmp strtol],,[AC_MSG_WARN([not found. Build may fail.])])
AC_CACHE_SAVE 

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

  SIM_AC_CONFIGURATION_SETTING([GUI],[Enabled])	
  BNV_HAVE_QT
  LIBS="$LIBS $QT_LIBS"
  CXXFLAGS="$CXXFLAGS $QT_CXXFLAGS"

  # Important macro for OSX, which may use Apple's OpenGL framework. This preference is undetectable.
  #AX_CHECK_GL
  MDL_HAVE_OPENGL

  AC_HAVE_LIBRARY([Coin],[LIBS="$LIBS -lCoin"],[
				      AC_MSG_RESULT([no])
				      SIM_AC_CONFIGURATION_WARNING([Failed to detect libCoin])])

  AC_HAVE_LIBRARY([jpeg], [LIBS="$LIBS -ljpeg"],[
				      AC_MSG_RESULT([no])
				      SIM_AC_CONFIGURATION_WARNING([Failed to detect libjpeg])])

  AC_HAVE_LIBRARY([SoQt],[LIBS="$LIBS -lSoQt"],[
				      AC_MSG_RESULT([no])
				      SIM_AC_CONFIGURATION_WARNING([Failed to detect libSoQt])])

  AC_DEFINE([TA_GUI],,
	    [Complementary switches, set when compiling for gui/no gui. Controls all non-QtCore related dependencies. For nogui, there can be no Coin or non-QtCore dependencies.])

  AC_DEFINE([TA_USE_QT],,
	    [This switch is set when linking with Qt.])

dnl   AH_VERBATIM([TA_USE_INVENTOR],
dnl [/*This switch is set when compiling for gui and linking with Coin. This switch is really only used in the lowest level files. NOTE: Coin/Inventor is never used when building nogui.*/
dnl #ifndef TA_USE_INVENTOR
dnl # define TA_USE_INVENTOR
dnl #endif])

  AC_DEFINE([TA_USE_INVENTOR],,
	    [This switch is set when compiling for gui and linking with Coin. This switch is really only used in the lowest level files. NOTE: Coin/Inventor is never used when building nogui.])

  AC_DEFINE([QT_SHARED],,
	    [Note from Brian: This switch is here, i'm not quite sure why. Used in qglobal.h.])

  # Tells top-level makefile.am to recurse through src/taiqtso in gui mode
  AC_SUBST([WANT_TAIQTSO],[src/taiqtso])

else

  SIM_AC_CONFIGURATION_SETTING([GUI],[Disabled])	
  AC_DEFINE([TA_NO_GUI],,[Set when building nogui])

fi
AC_CACHE_SAVE

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

  ACX_MPI(,SIM_AC_CONFIGURATION_WARNING([Failed to detect the Message Passing Interface. Try CXX=mpiCC]))
  ACX_PTHREAD(,SIM_AC_CONFIGURATION_WARNING([Failed to locate pthread]))
  AC_DEFINE([DMEM_COMPILE],,[Causes mpi.h to be included])

  if test $MPICXX != x; then
    SIM_AC_CONFIGURATION_SETTING([MPI],[Enabled. Using $MPICXX as compiler])
    CXX="$MPICXX"
  fi

else

  SIM_AC_CONFIGURATION_SETTING([MPI],[Disabled])	

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

  SIM_AC_CONFIGURATION_SETTING([Debugging],[Enabled])
  SIM_AC_CONFIGURATION_SETTING([Optimization],[-g -O0])
  AC_DEFINE([YYDEBUG],,[Used when debugging])
  AC_DEFINE([DEBUG],,[Used when debugging])
  AC_DEFINE([TA_PROFILE],,[Used when debugging])

  # No optimization in debug mode
  AC_SUBST([OPT_LEVEL],["-g -O0"])

else

  SIM_AC_CONFIGURATION_SETTING([Debugging],[Disabled])
  SIM_AC_CONFIGURATION_SETTING([Optimization],[-g -O2, except TA files which are -g -O0])  
  AC_SUBST([OPT_LEVEL],["-g -O2"])

fi

#						  @variables@
#*************************************************************
#	    Substitutes @variables@ in Makefile.am's
#*************************************************************
#

AC_SUBST([abs_top_builddir])
AC_SUBST([abs_srcdir])
AC_SUBST([srcdir])
AC_SUBST([top_srcdir])
AC_SUBST([CXXFLAGS])
AC_SUBST([CXXCPP])

#						       output
#*************************************************************
#		 Creation of the build system
#*************************************************************
#

# Custom macro to determine the_build_suffix to use
PDP_DETERMINE_SUFFIX

AC_CONFIG_FILES([Makefile
		src/maketa/Makefile
		src/taiqtso/Makefile
		src/ta/Makefile
		src/css/Makefile
		src/tamisc/Makefile
		src/pdp/Makefile
		src/leabra/Makefile
		src/bp/Makefile
		manual/Makefile])
AC_OUTPUT

SIM_AC_CONFIGURATION_SUMMARY

echo ""

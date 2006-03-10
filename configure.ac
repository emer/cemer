#					     -*- Autoconf -*-
#*************************************************************
# Process this file with autoconf to produce a configure script.
#*************************************************************
#
AC_INIT([PDP],[4.0],[pdp-discuss@psych-srv3.Colorado.EDU],[PDP++])
AC_PREREQ([2.59]) # Autoconf 2.59 required

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

#AC_DISABLE_SHARED                                      # Disable shared libraries
AC_CONFIG_AUX_DIR([cfg])                               # Stick extra stuff in /misc
AC_CONFIG_SRCDIR([src/ta/taglobal.h])                  # Make sure we're in the right place
AC_CONFIG_HEADERS([config.h])                          # Create config.h
AM_INIT_AUTOMAKE([-Wall -Werror gnu tar-pax nostdinc])
PDP_CANONICAL_HOST                                     # Do a few host checks, mostly for MSVC stuff
PDP_PROG_CXX                                           # Modify compiler optimization
PDP_SET_BUILD_MODE                                     # Set up our build modes
m4_undefine([AC_PROG_F77])                             # Get rid of those stupid fortran checks
m4_defun([AC_PROG_F77],[])
AC_PROG_CXXCPP                                         # Detect our C++ preprocessor
SIM_AC_CONFIGURATION_SETTING([Preprocessor],[$CXXCPP])
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_PROG_INSTALL
AC_PROG_LIBTOOL
AC_PROG_MAKE_SET
AC_PROG_AWK
AC_PROG_YACC
AC_PROG_LEX
PGAC_CHECK_READLINE                                    # Check for readline, termcap etc...
AC_HAVE_LIBRARY([dl],[LIBS="$LIBS -ldl"],[SIM_AC_CONFIGURATION_WARNING([libdl not found])])
SIM_AC_HAVE_LIBZLIB_IFELSE(,[SIM_AC_CONFIGURATION_WARNING([Failed to detect libzlib])])

AH_VERBATIM([_GLIBCXX_GTHREAD_USE_WEAK],
[/* Fix pthread bug. See:  http://softwareforums.intel.com/ids/board/message?board.id=16&message.id=3484 */
#ifndef _GLIBCXX_GTHREAD_USE_WEAK
 #define _GLIBCXX_GTHREAD_USE_WEAK 0
#endif
])

#AC_DEFINE([_GLIBCXX_GTHREAD_USE_WEAK],[0],[Fix pthread bug. See:  http://softwareforums.intel.com/ids/board/message?board.id=16&message.id=3484])

# --with-rpm
if test x"$rpm" = x"true"; then
  AX_DIST_RPM([spec.tmpl])
  AC_SUBST([INCLUDE_AMINCLUDE_AM],["include aminclude.am"])
  SIM_AC_CONFIGURATION_SETTING([rpm],[enabled...PLATFORMSUFFIX=${PLATFORM_SUFFIX}])
fi


# --enable-debug
if test x"$debug" = x"true"; then
  # Create css++ in debug mode
  AC_SUBST([WANT_CSS_BIN],[src/css_bin])

  SIM_AC_CONFIGURATION_SETTING([Debugging],[Enabled])
  SIM_AC_CONFIGURATION_SETTING([Optimization],[-g -O0])
  AC_DEFINE([DEBUG],,[Used when debugging])
  AC_DEFINE([TA_PROFILE],,[Used when debugging])
  AC_SUBST([OPT_LEVEL],["-g -O0"])                       # No optimization in debug mode
  # Checks for headers
  AC_FUNC_ALLOCA
  AC_HEADER_DIRENT
  AC_HEADER_STDC
  AC_HEADER_SYS_WAIT
  AC_CACHE_SAVE
  # Checks for typedefs, structures, compiler characteristics
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
  # Checks for library functions
  AC_FUNC_CHOWN
  AC_FUNC_ERROR_AT_LINE
  AC_FUNC_GETPGRP
  AC_FUNC_SELECT_ARGTYPES
  AC_TYPE_SIGNAL
  AC_FUNC_STAT
  AC_FUNC_STRTOD
  AC_CHECK_FUNCS([alarm floor getcwd gettimeofday memmove memset pow putenv rmdir select setlocale sqrt strcasecmp strtol],,[AC_MSG_WARN([not found. Build may fail.])])
  AC_CACHE_SAVE
else
  SIM_AC_CONFIGURATION_SETTING([Debugging],[Disabled])
  SIM_AC_CONFIGURATION_SETTING([Optimization],[-g -O2, except TA files which are -g -O0])  
  AC_SUBST([OPT_LEVEL],["-g -O2"])
fi

  SIM_AC_CHECK_QT([AC_DEFINE([QT3_SUPPORT],,[Set when building with Qt4 support for Qt3])]
                   if test x"$debug" = x"true"; then
                     sim_ac_qt_libs="$sim_ac_qt_libs -lQtTest"
                     sim_ac_qt_cppflags="$sim_ac_qt_cppflags -I${QTDIR}/include/QtTest"
#                     sim_ac_qt_libs="`echo $sim_ac_qt_libs | sed -r 's/(\-lQt[A-Za-z0-9]*)/\1_debug/g'`"
                   fi
                   LIBS="$LIBS $sim_ac_qt_libs"
                   CXXFLAGS="$CXXFLAGS $sim_ac_qt_cppflags"
                   LDFLAGS="$LDFLAGS $sim_ac_qt_ldflags"
                   sim_ac_qt_version="`echo $sim_ac_qt_version | sed -r 's/./&./g;s/.$//'`"
                   SIM_AC_CONFIGURATION_SETTING([Qt],[v.$sim_ac_qt_version $sim_ac_qt_libs]),
                   [SIM_AC_CONFIGURATION_WARNING([Failed to find a functioning Qt installation])])


  SIM_AC_QT_PROG([QT_MOC],[moc])
  if test x"$MOC" = "false"; then
    SIM_AC_CONFIGURATION_WARNING([Failed to find moc])
  fi

# --enable-gui
if test x"$gui" = x"true"; then
  SIM_AC_CONFIGURATION_SETTING([GUI],[Enabled])	
  
  # Functioning SoQt implies functioning Coin!
  SIM_AC_HAVE_SOQT_IFELSE([SIM_AC_CONFIGURATION_SETTING([SoQt],[v.$sim_ac_soqt_version -lSoQt -lCoin])
                           CPPFLAGS="$CPPFLAGS $sim_ac_soqt_cppflags"
                           LDFLAGS="$LDFLAGS $sim_ac_soqt_ldflags"
                           LIBS="$LIBS $sim_ac_soqt_libs"],
                          [SIM_AC_CONFIGURATION_WARNING([Failed to detect the SoQt development system])])
 
  SIM_AC_HAVE_LIBJPEG_IFELSE([CPPFLAGS="$CPPFLAGS $sim_ac_libjpeg_cppflags"
                              LDFLAGS="$LDFLAGS $sim_ac_libjpeg_ldflags"
                              LIBS="$LIBS $sim_ac_libjpeg_libs"],
                             [SIM_AC_CONFIGURATION_WARNING([Failed to detect libjpeg])])

  AC_DEFINE([TA_GUI],,[Complementary switches, set when compiling for gui/no gui. Controls all non-QtCore related dependencies. For nogui, there can be no Coin or non-QtCore dependencies.])
  AC_DEFINE([TA_USE_QT],,[This switch is set when linking with Qt.])
  AC_DEFINE([TA_USE_INVENTOR],,[This switch is set when compiling for gui and linking with Coin. This switch is really only used in the lowest level files. NOTE: Coin/Inventor is never used when building nogui.])
  AC_DEFINE([QT_SHARED],,[Used in qglobal.h.])

  # Tells top-level makefile.am to recurse through src/taiqtso in gui mode
  AC_SUBST([WANT_TAIQTSO],[src/taiqtso])
else
  SIM_AC_CONFIGURATION_SETTING([GUI],[Disabled])	
  AC_DEFINE([TA_NO_GUI],,[Set when building nogui])
fi

# --enable-mpi is set
if test x"$mpi" = x"true"; then
  ACX_MPI(,SIM_AC_CONFIGURATION_WARNING([Failed to detect the Message Passing Interface. Try CXX=mpiCC]))
  AC_DEFINE([DMEM_COMPILE],,[Causes mpi.h to be included])

  if test x"$MPICXX" != x; then
    SIM_AC_CONFIGURATION_SETTING([MPI],[Enabled. Using $MPICXX as compiler])
    CXX="$MPICXX"
  fi
else
  SIM_AC_CONFIGURATION_SETTING([MPI],[Disabled])	
fi

AC_SUBST([abs_top_builddir])
AC_SUBST([abs_srcdir])
AC_SUBST([srcdir])
AC_SUBST([top_srcdir])
AC_SUBST([CXXFLAGS])
AC_SUBST([CXXCPP])

PDP_DETERMINE_SUFFIX                                # Infixing
AC_CONFIG_FILES([Makefile
		src/maketa/Makefile
		src/taiqtso/Makefile
		src/ta/Makefile
		src/css/Makefile
		src/css_bin/Makefile
		src/tamisc/Makefile
		src/ta_lib/Makefile
		src/bp/Makefile
		src/pdp/Makefile
		src/pdp_lib/Makefile
		src/pdp_bin/Makefile
		manual/Makefile])
AC_OUTPUT
SIM_AC_CONFIGURATION_SUMMARY

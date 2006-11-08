#					     -*- Autoconf -*-
#*************************************************************
# Process this file with autoconf to produce a configure script.
#*************************************************************
#

# Notes on library and package versioning:
#
# Package:
# * The package consists of a major and a minor version, set
#   arbitrarily in AC_INIT. It is broken up into two components,
#   which are AX_MAJOR_VERSION and AX_MINOR_VERSION for use elsewhere.
#   Set the package version here. autoconf will append it to the package name
AC_INIT([PDP],[3.5],[pdp-discuss@psych-srv3.Colorado.EDU],[pdp++])

# Library:
# * The libta and libpdp libraries are versioned according to Libtool's
#   recommendations. See [1] for full details. Briefly, it is broken into 
#   three components, which are current.revision.age, and this information 
#   should only be updated immediately before a public release.
#   * Increment current, set revision to 0 if interfaces have been added, 
#     removed, or changed
#   * Increment revision if the library source code has changed at all
#   * Increment age if interfaces have been added
#   * Set age to 0 if interfaces have been removed
#
# [1] http://www.gnu.org/software/libtool/manual.html#Versioning
# Set library version here. Libtool will append it to library names.
LIB_VERSION="0.0.0"

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



AC_DISABLE_STATIC                                      # Disable static libraries
AC_CONFIG_AUX_DIR([cfg])                               # Stick extra stuff in /cfg
AC_CONFIG_SRCDIR([src/ta/taglobal.h])                  # Make sure we're in the right place
AC_CONFIG_HEADERS([config.h])                          # Create config.h
# NOTE: If using OSX, use DarwinPorts to get most recent automake verison
AM_INIT_AUTOMAKE([-Wall -Werror gnu tar-pax nostdinc 1.9.5])
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
AC_HAVE_LIBRARY([dl],[LIBS="$LIBS -ldl"],[SIM_AC_CONFIGURATION_WARNING([libdl not found])])
SIM_AC_HAVE_LIBZLIB_IFELSE(,[SIM_AC_CONFIGURATION_WARNING([Failed to detect libzlib])])

# non-modal, for now
AC_DEFINE([V3_COMPAT],[],[turn on version 3.x compatability code])

# Check to see if a conflicting version of pdp++ or css++ is installed
if test x"$prefix" = x"NONE"; then
  prefix=/usr/local
fi
if test x"$debug" = x"false"; then
  PDP_PROG_PDP
fi

#Check for gsl, the GNU Scientific Library. By default, this #defines HAVE_LIBGSL
LDFLAGS="$LDFLAGS -L/opt/local/lib" #Harmless insertion for DarwinPorts
AC_CHECK_LIB([m],[main])
AC_CHECK_LIB([gslcblas],[main])
AC_CHECK_LIB([gsl],[main],
              [LIBS="$LIBS -lgsl"]
              [AC_DEFINE([HAVE_LIBGSL],[],[Have the gnu scientific library])]
              [SIM_AC_CONFIGURATION_SETTING([gsl],[Enabled])],
               [SIM_AC_CONFIGURATION_WARNING([Failed to detect the GNU Scientific Library])])

# Fix pthread bug. See:  http://softwareforums.intel.com/ids/board/message?board.id=16&message.id=3484 
CXXFLAGS="$CXXFLAGS -D_GLIBCXX_GTHREAD_USE_WEAK=0"

# --with-readline
if test "$readline" = "true"; then
  VL_LIB_READLINE
else
 SIM_AC_CONFIGURATION_SETTING([Readline],[Disabled])
fi

# --with-rpm
if test x"$rpm" = x"true"; then
  AX_DIST_RPM([spec.tmpl])
  AC_SUBST([INCLUDE_AMINCLUDE_AM],["include aminclude.am"])
  SIM_AC_CONFIGURATION_SETTING([rpm],[enabled...PLATFORMSUFFIX=${PLATFORM_SUFFIX}])
fi

# --disable-libta
# note that this substitution occurs by default
if test x"$libta" = x"true"; then
   AC_SUBST([WANT_LIBTA],[src/ta_lib])
fi

# --disable-css-bin
# note that this substitution occurs by default
if test x"$css_bin" = x"true"; then
   AC_SUBST([WANT_CSS_BIN],[src/css_bin])
fi

# --disable-maketa-opt
# controls optimization of maketa executeable
if test x"$maketa_opt" = x"false"; then
  AC_SUBST([MAKETA_OPT],["-g -O0"])
else
  AC_SUBST([MAKETA_OPT],["-g -O2"])
  AC_SUBST([NOOP_MAKETA],["-UDEBUG"])
fi

# --enable-debug
if test x"$debug" = x"true"; then

  PDP_DETERMINE_SUFFIX                                # Infixing

  # Link with any debug-configurated libraries on the system
  export CONFIG=debug

  SIM_AC_CONFIGURATION_SETTING([Debugging],[Enabled])
  SIM_AC_CONFIGURATION_SETTING([Optimization],[-g -Wall])
  AC_DEFINE([DEBUG],,[Used when debugging])
  AC_SUBST([OPT_LEVEL],["-g -Wall"])                       # No optimization in debug mode
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
dnl                    if test x"$debug" = x"true"; then
dnl                      sim_ac_qt_libs="$sim_ac_qt_libs -lQtTest_debug"
dnl                      sim_ac_qt_cppflags="$sim_ac_qt_cppflags -I${QTDIR}/include/QtTest"
dnl #                     sim_ac_qt_libs="`echo $sim_ac_qt_libs | sed -r 's/(\-lQt[A-Za-z0-9]*)/\1_debug/g'`"
dnl                    fi
                   LIBS="$LIBS $sim_ac_qt_libs"
                   CXXFLAGS="$CXXFLAGS $sim_ac_qt_cppflags"
                   LDFLAGS="$LDFLAGS $sim_ac_qt_ldflags"
                   sim_ac_qt_version="`echo $sim_ac_qt_version | sed 's/./&./g;s/.$//'`"
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
 
#  SIM_AC_HAVE_LIBJPEG_IFELSE([CPPFLAGS="$CPPFLAGS $sim_ac_libjpeg_cppflags"
#                              LDFLAGS="$LDFLAGS $sim_ac_libjpeg_ldflags"
#                              LIBS="$LIBS $sim_ac_libjpeg_libs"],
#                             [SIM_AC_CONFIGURATION_WARNING([Failed to detect libjpeg])])

AC_CHECK_LIB([jpeg],[jpeg_read_header],[LIBS="$LIBS -ljpeg"],[SIM_AC_CONFIGURATION_WARNING([Failed to detect libjpeg])])

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

if test x"$debug" = x"true"; then
  SIM_AC_CONFIGURATION_SETTING([LIBS],[$LIBS])
#  SIM_AC_CONFIGURATION_SETTING([CXXFLAGS],[$CXXFLAGS])
fi


SIM_AC_CONFIGURATION_SETTING([Installation prefix],[$prefix])

# Adds package information verison to libs
AX_MAJOR_VERSION=`echo "$VERSION" | sed 's/\([[^.]][[^.]]*\).*/\1/'`
AX_MINOR_VERSION=`echo "$VERSION" | sed 's/[[^.]][[^.]]*.\([[^.]][[^.]]*\).*/\1/'`
PDP_VERSION=-$AX_MAJOR_VERSION.$AX_MINOR_VERSION
AX_LIB_MAJOR_VERSION=`echo "$LIB_VERSION" | sed 's/\([[^.]][[^.]]*\).*/\1/'`
AX_LIB_MINOR_VERSION=`echo "$LIB_VERSION" | sed 's/[[^.]][[^.]]*.\([[^.]][[^.]]*\).*/\1/'`
AX_LIB_POINT_VERSION=`echo "$LIB_VERSION" | sed 's/[[^.]][[^.]]*.[[^.]][[^.]]*.\(.*\)/\1/'`

AC_SUBST([AX_MAJOR_VERSION])
AC_SUBST([AX_MINOR_VERSION])
AC_SUBST([AX_LIB_MAJOR_VERSION])
AC_SUBST([AX_LIB_MINOR_VERSION])
AC_SUBST([AX_LIB_POINT_VERSION])
AC_SUBST([PDP_SUFFIX])
AC_SUBST([PDP_VERSION])
AC_SUBST([abs_top_builddir])
AC_SUBST([abs_srcdir])
AC_SUBST([srcdir])
AC_SUBST([top_srcdir])
AC_SUBST([CXXFLAGS])
AC_SUBST([CXXCPP])

AC_CONFIG_FILES([Makefile
		src/maketa/Makefile
		src/taiqtso/Makefile
		src/ta/Makefile
		src/css/Makefile
		src/css_bin/Makefile
		src/ta_lib/Makefile
		src/bp/Makefile
		src/leabra/Makefile
		src/pdp/Makefile
		src/pdp_lib/Makefile
		src/pdp_bin/Makefile
		manual/Makefile])
AC_OUTPUT
SIM_AC_CONFIGURATION_SUMMARY

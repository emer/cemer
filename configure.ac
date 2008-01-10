dnl 					         -*- Autoconf -*-
dnl *************************************************************
dnl  Process this file with autoconf to produce a configure script.
dnl *************************************************************
dnl 
dnl  Notes on library and package versioning:
dnl  * Package: 
dnl    * The binary version consists of major, minor, point and special
dnl      versions, set via the command-line. Autoconf will append the
dnl      version to the package name. The full version is available in
dnl      makefiles and the c++ source via BIN_VERSION
dnl 
dnl  * Library : 
dnl    * The libraries are versioned according to Libtool's
dnl      recommendations. See [1] for full details. Briefly, it is broken into 
dnl      three components, which are current.revision.age, and this information 
dnl      should only be updated immediately before a public release.
dnl      * Increment current, set revision to 0 if interfaces have been added, 
dnl        removed, or changed
dnl      * Increment revision if the library source code has changed at all
dnl      * Increment age if interfaces have been added
dnl      * Set age to 0 if interfaces have been removed
dnl 
dnl  [1] http://www.gnu.org/software/libtool/manual.html Versioning
dnl
dnl Subst variables that start with PL_ are used in the plugin configuration

# Set binary version here. The format allows for 99.99.99abc
AC_INIT([Emergent],[4.0.11],[Emergent-discuss@psych-srv3.Colorado.EDU],[Emergent]) 
# Change both of these
LIB_VERSION=1:0:0   #Set library version here
LIB_VERSION_2=1.0.0

AC_SUBST([PACKAGE_VERSION])
AC_SUBST([LIB_VERSION])
AC_SUBST([LIB_VERSION_2])
AC_SUBST([PL_LIB_VER],[${PACKAGE_VERSION}])
AH_TEMPLATE([LIB_VERSION],[Library Version])
AC_DEFINE_DIR([LIB_VERSION],[LIB_VERSION]) # Co-opting this macro

SIM_AC_CONFIGURATION_SETTING([Emergent binary version],[${PACKAGE_VERSION}])
SIM_AC_CONFIGURATION_SETTING([The Emergent Toolkit library version],[${LIB_VERSION}])



AC_PREREQ([2.59])                                      dnl Autoconf 2.59 required
AC_COPYRIGHT([
Copyright, 1995-2007, Regents of the University of Colorado,
Carnegie Mellon University, Princeton University.

This file is part of Emergent

  Emergent is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; version 2 of the License.

  Emergent is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
])

AC_DISABLE_STATIC                                      dnl Disable static libraries
AC_CONFIG_AUX_DIR([cfg])                               dnl Stick extra stuff in /cfg
AC_CONFIG_SRCDIR([src/temt/ta/ta_global.h])                 dnl Make sure we're in the right place
AC_CONFIG_HEADERS([config.h])                          dnl Create config.h
                                                       dnl NOTE: If using OSX, use DarwinPorts to get most recent automake verison
AM_INIT_AUTOMAKE([-Wall -Werror gnu tar-pax nostdinc 1.9.6])
EMERGENT_CANONICAL_HOST                                     dnl Do a few host checks, mostly for MSVC stuff
EMERGENT_PROG_CXX                                           dnl Modify compiler optimization
EMERGENT_CANONICAL_COMPILER
EMERGENT_SET_BUILD_MODE                                     dnl Set up our build modes
m4_undefine([AC_PROG_F77])                             dnl Get rid of those stupid fortran checks
m4_defun([AC_PROG_F77],[])
AC_PROG_CXXCPP                                         dnl Detect our C++ preprocessor
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_PROG_INSTALL
AC_PROG_LIBTOOL
AC_PROG_MAKE_SET
AC_PROG_AWK
AC_PROG_YACC
AC_PROG_LEX

SIM_AC_HAVE_LIBZLIB_IFELSE(,[SIM_AC_CONFIGURATION_WARNING([Failed to detect libzlib])])

case $host in
       *-*-mingw32*)
       ;;
        *)
               AC_HAVE_LIBRARY([dl],[LIBS="$LIBS -ldl"],[SIM_AC_CONFIGURATION_WARNING([libdl not found])])
       ;;
esac

dnl  non-modal, for now
AC_DEFINE([V3_COMPAT],[],[turn on version 3.x compatability code])

dnl  Check to see if a conflicting version of Emergent is installed
if test x"$prefix" = x"NONE"; then
  prefix=/usr/local
fi
if test x"$debug" = x"false" -a x"$bundle" = x"false"; then
  EMERGENT_PROG_EMERGENT
fi

dnl  A generic set of systemwide includes. Not all sub libraries
dnl  require all of these includes, but this is less error-prone
dnl  than manually specifying them for each library
dnl TODO: modalize the includes based on whether this is toolkit-only mode
AC_SUBST([EMERGENT_INCLUDES],["-I\$(srcdir)/../../emergent/bp -I\$(srcdir)/../../temt/css -I\$(srcdir)/../../emergent/leabra -I\$(srcdir)/../../emergent/so -I\$(srcdir)/../../emergent/cs -I\$(srcdir)/../../emergent/network -I\$(srcdir)/../../temt/ta -I\$(srcdir)/../../temt/taiqtso -I\$(srcdir)/../../temt/maketa -I\$(abs_top_builddir) -I\$(srcdir) -I\$(srcdir)/src -I\$(srcdir)/../.. -I\$(srcdir)/.."])

AC_CHECK_LIB([ode],[main])
AC_CHECK_LIB([m],[main])
AC_CHECK_LIB([gslcblas],[main])

dnl  Check for gsl, the GNU Scientific Library. By default, this #defines HAVE_LIBGSL
AC_CHECK_LIB([gsl],[main],
              [LIBS="$LIBS -lgsl"]
              [AC_DEFINE([HAVE_LIBGSL],[],[Have the gnu scientific library])],
               [SIM_AC_CONFIGURATION_WARNING([Failed to detect the GNU Scientific Library])])

dnl  svn revision number support
EMERGENT_SVN_REVISION

dnl  Fix pthread bug. See:  http://softwareforums.intel.com/ids/board/message?board.id=16&message.id=3484 
#CXXFLAGS="$CXXFLAGS -D_GLIBCXX_GTHREAD_USE_WEAK=0"

dnl  Let the codebase know where to find auxiliary files (in config.h)
dnl  TODO: Brad says this doesn't work right
AH_TEMPLATE([WHEREAMI],[blah])
AC_DEFINE_DIR([WHEREAMI],[datadir])
dnl not working: AC_SUBST([PL_DATADIR],[`echo $(echo $datadir)`])

dnl --disable-plugins (enabled by default)
if test "$plugins" = "true"; then
 AC_SUBST([WANT_PLUGINS],[plugins])
fi

dnl  --with-readline
if test "$readline" = "true"; then
  VL_LIB_READLINE
else
 SIM_AC_CONFIGURATION_SETTING([Readline],[Disabled])
fi

dnl  --with-rpm
if test x"$rpm" = x"true"; then
  AX_DIST_RPM([spec.tmpl])
  AC_SUBST([INCLUDE_AMINCLUDE_AM],["include aminclude.am"])
  SIM_AC_CONFIGURATION_SETTING([rpm],[enabled...PLATFORMSUFFIX=${PLATFORM_SUFFIX}])
fi

dnl  --disable-css-bin
dnl  note that this substitution occurs by default
if test x"$css_bin" = x"true"; then
   AC_SUBST([WANT_CSS_BIN],[src/temt/css_bin])
fi

dnl  --disable-maketa-opt
dnl  controls optimization of maketa executeable
if test x"$maketa_opt" = x"false"; then
  AC_SUBST([MAKETA_OPT],["-g -O0"])
else
  AC_SUBST([MAKETA_OPT],["-g -O2"])
  AC_SUBST([NOOP_MAKETA],["-UDEBUG"])
fi

dnl  --enable-debug
if test x"$debug" = x"true"; then

  dnl Link with any debug-configurated libraries on the system
  export CONFIG=debug

  SIM_AC_CONFIGURATION_SETTING([Debugging],[Enabled])

  AC_DEFINE([DEBUG],,[Used when debugging])
  AC_SUBST([PL_DEBUG],[debug])

  case $host in
          *darwin*)
                   dnl use DWARF debugging symbols on OSX (for Leopard)
                   AC_SUBST([OPT_LEVEL],["-gstabs+ -Wall"])  dnl No optimization in debug mode
                   SIM_AC_CONFIGURATION_SETTING([Optimization],[-gstabs+ -Wall])
          ;;
          *)
                   dnl do default thing
                   AC_SUBST([OPT_LEVEL],["-g -Wall"])  dnl No optimization in debug mode
                   SIM_AC_CONFIGURATION_SETTING([Optimization],[-g -Wall])
          ;;                   
  esac

  dnl Checks for headers
  AC_FUNC_ALLOCA
  AC_HEADER_DIRENT
  AC_HEADER_STDC
  AC_HEADER_SYS_WAIT
  AC_CACHE_SAVE
  dnl Checks for typedefs, structures, compiler characteristics
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
  dnl Checks for library functions
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
  AC_SUBST([PL_DEBUG],[release])
  SIM_AC_CONFIGURATION_SETTING([Optimization],[-g -O2, except TA files which are -g -O0])  
  AC_SUBST([OPT_LEVEL],["-g -O2"])
fi

dnl  Qt
  SIM_AC_CHECK_QT([AC_DEFINE([QT3_SUPPORT],,[Set when building with Qt4 support for Qt3])]
                   LIBS="$LIBS $sim_ac_qt_libs"
                   CXXFLAGS="$CXXFLAGS $sim_ac_qt_cppflags"
                   LDFLAGS="$LDFLAGS $sim_ac_qt_ldflags"
                   sim_ac_qt_version="`echo $sim_ac_qt_version | sed 's/./&./g;s/.$//'`"
                   SIM_AC_CONFIGURATION_SETTING([Qt],[v.$sim_ac_qt_version]),
                   [SIM_AC_CONFIGURATION_WARNING([Failed to find a functioning Qt installation])])


  SIM_AC_QT_PROG([QT_MOC],[moc])
  if test x"$MOC" = "false"; then
    SIM_AC_CONFIGURATION_WARNING([Failed to find moc])
  fi

dnl  --enable-gui
if test x"$gui" = x"true"; then
  dnl Functioning SoQt implies functioning Coin!
                           
  SIM_AC_HAVE_COIN_IFELSE([SIM_AC_CONFIGURATION_SETTING([Coin],[v.$sim_ac_coin_version])
                           CPPFLAGS="$CPPFLAGS $sim_ac_coin_cppflags"
                           LDFLAGS="$LDFLAGS $sim_ac_coin_ldflags"
                           LIBS="$LIBS $sim_ac_coin_libs"
                           SIM_AC_HAVE_SOQT_IFELSE([SIM_AC_CONFIGURATION_SETTING([SoQt],[v.$sim_ac_soqt_version])
                                                    CPPFLAGS="$CPPFLAGS $sim_ac_soqt_cppflags"
                                                    LDFLAGS="$LDFLAGS $sim_ac_soqt_ldflags"
                                                    LIBS="$LIBS $sim_ac_soqt_libs"
                                                    SIM_AC_HAVE_SIMAGE_IFELSE([SIM_AC_CONFIGURATION_SETTING([Simage],[v.$sim_ac_simage_version])
                                                                               CPPFLAGS="$CPPFLAGS $sim_ac_simage_cppflags"
                                                                               LDFLAGS="$LDFLAGS $sim_ac_simage_ldflags"
                                                                               LIBS="$LIBS $sim_ac_simage_libs"],
                                                                               [SIM_AC_CONFIGURATION_WARNING([Failed to detect Simage. Textures will not work.])])],
                                                   [SIM_AC_CONFIGURATION_WARNING([Failed to detect the SoQt development system])])],
                          [SIM_AC_CONFIGURATION_WARNING([Failed to detect the Coin development system])])

  AC_DEFINE([TA_GUI],,[Complementary switches, set when compiling for gui/no gui. Controls all non-QtCore related dependencies. For nogui, there can be no Coin or non-QtCore dependencies.])
  AC_DEFINE([TA_USE_QT],,[This switch is set when linking with Qt.])
  AC_DEFINE([TA_USE_INVENTOR],,[This switch is set when compiling for gui and linking with Coin. This switch is really only used in the lowest level files. NOTE: Coin/Inventor is never used when building nogui.])
  AC_DEFINE([QT_SHARED],,[Used in qglobal.h.])

  dnl Tells top-level makefile.am to recurse through src/taiqtso in gui mode
  AC_SUBST([WANT_TAIQTSO],[src/temt/taiqtso])
else
  SIM_AC_CONFIGURATION_SETTING([GUI],[Disabled])	
  AC_DEFINE([TA_NO_GUI],,[Set when building nogui])
  AC_SUBST([PL_TA_NO_GUI], [nogui])
fi

dnl  --enable-mpi is set
if test x"$mpi" = x"true"; then
  ACX_MPI(,SIM_AC_CONFIGURATION_WARNING([Failed to detect the Message Passing Interface. Try CXX=mpiCC]))
  AC_DEFINE([DMEM_COMPILE],,[Causes mpi.h to be included])
  AC_SUBST([PL_DMEM_COMPILE],[mpi])

  if test x"$MPICXX" != x; then
    SIM_AC_CONFIGURATION_SETTING([MPI],[Enabled. Using $MPICXX as compiler])
    CXX="$MPICXX"
  fi
else
  SIM_AC_CONFIGURATION_SETTING([MPI],[Disabled])	
fi


dnl  --enable-profile
if test x"$profile" = x"true"; then
  SIM_AC_CONFIGURATION_SETTING([Optimization],[-pg, except TA files which are -g -O0])  
  LDFLAGS="$LDFLAGS -pg"
  AC_SUBST([OPT_LEVEL],["-pg"])
  AC_DEFINE([GPROF],[],[enables profiling control functions])
fi

EMERGENT_DETERMINE_SUFFIX                                dnl Infixing

if test x"bundle" = x"false"; then
  SIM_AC_CONFIGURATION_SETTING([Installation prefix],[${prefix}])
fi

case $host in
    *-*-mingw32*)
            CXXFLAGS=$(echo $CXXFLAGS | sed 's/\/ -I/ -I/g;s/\/$//;s/\\\\/\\/g')
            LIBS=$(echo $LIBS | sed 's/-lm//')
    ;;
    *)
    ;;
esac
    
# Remove duplicate entries. TODO: Will this cause problems on MinGW?
# NOTE: on quadrigraphs: [ is @<:@  and ] is @:>@  

#CXXFLAGS="`echo "$CXXFLAGS" | tr " " "\n" | awk '!seen@<:@$0@:>@++' | tr "\n" " "`"
#LDFLAGS="`echo "$LDFLAGS" | tr " " "\n" | awk '!seen@<:@$0@:>@++' | tr "\n" " "`"
LIBS="`echo "$LIBS" | tr " " "\n" | awk '!seen@<:@$0@:>@++' | tr "\n" " "`"

SIM_AC_UNIVERSAL_BINARIES 

AC_SUBST([EMERGENT_SUFFIX])
AC_SUBST([abs_top_builddir])
AC_SUBST([abs_srcdir])
AC_SUBST([srcdir])
AC_SUBST([top_srcdir])
AC_SUBST([CXXFLAGS])
AC_SUBST([CXXCPP])
AC_SUBST([MAKETA_CXXFLAGS])


# The following series of very hacky nasty subst's are for plugin makefile integration

AC_SUBST([PL_LIBS],["`echo "$LIBS"`"])
AC_SUBST([PL_prefix],["`echo "$prefix"`"])

# note hack, because gd exec_prefix subs out as "NONE" for some whacky reason
# AC_SUBST([PL_exec_prefix],["`echo "$exec_prefix"`"])
AC_SUBST([PL_exec_prefix],["`echo "$prefix"`"])

# replace the single $ with 2 $ so qmake subs at qmake time
# replace whacky "datarootdir" nonsense too
AC_SUBST([PL_bindir],["`echo "$bindir" | sed 's/\$/\$\$/g'`"])

# dnw AC_SUBST([PL_bindir],[$(eval echo ${bindir})])
AC_SUBST([PL_datadir],["`echo "$datadir" | sed 's/\$/[$][$]/'`"])
AC_SUBST([PL_includedir],["`echo "$includedir" | sed 's/\$/\$\$/g'`"])
AC_SUBST([PL_abs_top_builddir],["`echo "$abs_top_builddir"`"])

includedir="${includedir}/emergent"

dnl TODO: definitely broken from emergent name change
if test x"$bundle" = x"true"; then
  bundle_name="Emergent-${PACKAGE_VERSION}.app"
  bundle_dir="`pwd`/${bundle_name}"
  bindir="${bundle_dir}/Contents/MacOS"
  libdir="${bundle_dir}/Contents/Frameworks/libemergenttemt-${LIB_VERSION_2}.framework"
  datadir="${bundle_dir}/Contents/Resources"
  infodir="$datadir"
  includedir="${bundle_dir}/Contents/Frameworks/${bundle_name}.framework/Headers"

  SIM_AC_CONFIGURATION_SETTING([OSX Bundle],[./$bundle_name])

fi

AC_CONFIG_FILES([Makefile
		src/temt/maketa/Makefile
		src/temt/taiqtso/Makefile
		src/temt/ta/Makefile
		src/temt/css/Makefile
		src/temt/css_bin/Makefile
		src/temt/lib/Makefile
		src/emergent/bp/Makefile
		src/emergent/leabra/Makefile
		src/emergent/so/Makefile
		src/emergent/cs/Makefile
		src/emergent/network/Makefile
		src/emergent/lib/Makefile
		src/emergent/bin/Makefile
		prog_lib/Makefile
		3dobj_lib/Makefile
		test/Makefile
		plugins/config.pri
		plugins/Makefile
		plugins/Makefile.plugin
		css_lib/Makefile
		demo/bp/Makefile
		demo/cs/Makefile
		demo/leabra/Makefile
		demo/so/Makefile
		demo/virt_env/Makefile
		demo/data_proc/Makefile
		src/temt/ta/images/Makefile
                postremove-pak
                postinstall-pak])

AC_OUTPUT
SIM_AC_CONFIGURATION_SUMMARY
if test x"$debug" = x"true"; then
  echo "  LIBS:"
  echo $LIBS | fmt -s -w 78 | sed 's/^/    /'
  echo "  CXXFLAGS:"
  echo $CXXFLAGS | fmt -s -w 78  | sed 's/^/    /'
  echo "  LDFLAGS:"
  echo $LDFLAGS | fmt -s -w 78  | sed 's/^/    /'
fi

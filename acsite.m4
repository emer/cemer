dnl 					         EMERGENT_SVN_REVISION
dnl *************************************************************
dnl Set SVN_REV to the revision of the local repository
dnl *************************************************************
dnl Copyright, 1995-2007, Regents of the University of Colorado,
dnl Carnegie Mellon University, Princeton University.
dnl
dnl This file is part of Emergent
dnl
dnl   Emergent is free software; you can redistribute it and/or modify
dnl   it under the terms of the GNU General Public License as published by
dnl   the Free Software Foundation; either version 2 of the License, or
dnl   (at your option) any later version.
dnl
dnl   Emergent is distributed in the hope that it will be useful,
dnl   but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl   GNU General Public License for more details. 
AC_DEFUN([EMERGENT_SVN_REVISION],[

if test "`svn info --xml | grep "<ro"`"='<root>http://grey.colorado.edu/svn/repos</root>'; then 
	SVN_REV=`svn info . | grep Revision | sed 's/Revision: //'`
	CXXFLAGS="$CXXFLAGS -DSVN_REV=`echo $SVN_REV`"
	SIM_AC_CONFIGURATION_SETTING([Svn revision],["$SVN_REV"])
else
    SIM_AC_CONFIGURATION_WARNING([Unable to locate svn. Revision not set.])
fi
])

dnl 					       EMERGENT_SET_BUILD_MODE
dnl *************************************************************
dnl Figure out what kind of build mode we are doing, and prep
dnl the system for it
dnl *************************************************************
dnl Copyright, 1995-2007, Regents of the University of Colorado,
dnl Carnegie Mellon University, Princeton University.
dnl
dnl This file is part of Emergent
dnl
dnl   Emergent is free software; you can redistribute it and/or modify
dnl   it under the terms of the GNU General Public License as published by
dnl   the Free Software Foundation; either version 2 of the License, or
dnl   (at your option) any later version.
dnl
dnl   Emergent is distributed in the hope that it will be useful,
dnl   but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl   GNU General Public License for more details. 
AC_DEFUN([EMERGENT_SET_BUILD_MODE],[
AC_ARG_ENABLE([libta],
              AC_HELP_STRING([--disable-libta],
                             [Disable creation of TypeAccess library.  @<:@default=enabled@:>@]),
                             [libta=false],
                             [libta=true])
AC_ARG_ENABLE([gui],
	      AC_HELP_STRING([--disable-gui],
			     [Disable compiling with a GUI. @<:@default=enabled@:>@]),
			     [gui=false],
			     [gui=true])
AC_ARG_ENABLE([mpi],
	      AC_HELP_STRING([--enable-mpi],
			     [Enable the Message Passing Interface.  @<:@default=disabled@:>@]),
			     [mpi=true],
			     [mpi=false])
AC_ARG_ENABLE([css_bin],
	      AC_HELP_STRING([--disable-css-bin],
			     [Disable creation of the css (C^c) executeable.  @<:@default=enabled@:>@]),
			     [css_bin=false],
			     [css_bin=true])
AC_ARG_ENABLE([debug],
	      AC_HELP_STRING([--enable-debug],
			     [Enable debugging.  @<:@default=disabled@:>@]),
			     [debug=true],
			     [debug=false])
AC_ARG_ENABLE([profile],
	      AC_HELP_STRING([--enable-profile],
			     [Enable profiling.  @<:@default=disabled@:>@]),
			     [profile=true],
			     [profile=false])
AC_ARG_ENABLE([maketa_opt],
	      AC_HELP_STRING([--disable-maketa-opt],
			     [Disable optimization of the maketa executeable. @<:@default=enabled@:>@]),
			     [maketa_opt=false],
			     [maketa_opt=true])
AC_ARG_ENABLE([bundle],
	      AC_HELP_STRING([--enable-bundle],
			     [Create an OSX Bundle upon make install. Creates, e.g., emergent-4.0.2.app  @<:@default=disabled@:>@]),
			     [bundle=true],
			     [bundle=false])

AC_ARG_ENABLE([plugins],
	      AC_HELP_STRING([--enable-plugins],
			     [Enable plugin development support. Source code will be installed to `/usr/local/emergent/src' and `/usr/local/emergent/plugins' will be created.  @<:@default=enabled@:>@]),
			     [plugins=true],
			     [plugins=false])
AC_ARG_ENABLE([readline],
	      AC_HELP_STRING([--disable-readline],
			     [Disable linking against the readline library.  @<:@default=enabled@:>@]),
			     [readline=false],
			     [readline=true])
AC_ARG_ENABLE([nightly],
	      AC_HELP_STRING([--enable-nightly],
			     [Adds the _nightly infix to our bins/libs. For internal use only.]),
			     [nightly=true],
			     [nightly=false])

#NOTE: This actually works with a few minor hiccups
AC_ARG_WITH([rpm],
	      AC_HELP_STRING([--with-rpm],
			     [Enable the creation of rpms with `make rpm'.You must first `touch aminclude.am' and set PLATFORM_SUFFIX (see below).  @<:@default=disabled@:>@ ]),
			     [rpm=true],
			     [rpm=false])



AM_CONDITIONAL([PLUGINS],[test $plugins = true])
AM_CONDITIONAL([RPM],[test $rpm = true])
AM_CONDITIONAL([TA_GUI],[test $gui = true])
AM_CONDITIONAL([NO_TA_GUI],[test $gui = false])
AM_CONDITIONAL([MPI],[test $mpi = true])
AM_CONDITIONAL([PROFILE],[test $profile = true])
AM_CONDITIONAL([DEBUG],[test $debug = true])
AM_CONDITIONAL([LIBTA],[test $libta = true])
AM_CONDITIONAL([CSS_BIN],[test $css_bin = true])
])


dnl 					             EMERGENT_PROG_CXX
dnl *************************************************************
dnl This macro allows us to set our own level of optimization
dnl on C++ files. We use it to set -O0 on all TA files all the
dnl time and to not do any optimization in --enable-debug mode
dnl *************************************************************
dnl Copyright, 1995-2007, Regents of the University of Colorado,
dnl Carnegie Mellon University, Princeton University.
dnl
dnl This file is part of Emergent
dnl
dnl   Emergent is free software; you can redistribute it and/or modify
dnl   it under the terms of the GNU General Public License as published by
dnl   the Free Software Foundation; either version 2 of the License, or
dnl   (at your option) any later version.
dnl
dnl   Emergent is distributed in the hope that it will be useful,
dnl   but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl   GNU General Public License for more details. 

AC_DEFUN([EMERGENT_PROG_CXX],[

AC_LANG_CPLUSPLUS
save_user_CXXFLAGS=${CXXFLAGS}
CXXFLAGS=
AC_PROG_CXX([g++])
CXXFLAGS=${save_user_CXXFLAGS}

# Still enable disabling optimization
SIM_AC_COMPILER_OPTIMIZATION
])

dnl 					             EMERGENT_PROG_EMERGENT
dnl *************************************************************
dnl This macro checks for the existence of the emergent
dnl executable in the chosen installation path and emits a warning
dnl ************************************************************
AC_DEFUN([EMERGENT_PROG_EMERGENT],[
# Did the user specify an installation path?
check_path=$ac_default_prefix/bin
test x"$prefix" != xNONE && check_path=$prefix/bin
test x"$exec_prefix" != xNONE && check_path=$exec_prefix/bin

AC_CHECK_PROG([emergentexists],[emergent],[true],[false],[${check_path}])
if test x"${emergentexists}" = x"true"; then
   SIM_AC_CONFIGURATION_WARNING([Emergent is already installed in ${check_path}.])
fi
])

dnl					   EMERGENT_CANONICAL_COMPILER
dnl *************************************************************
dnl  A couple of compiler checks for setting AM_CONDITIONALs
dnl *************************************************************
dnl Copyright, 1995-2007, Regents of the University of Colorado,
dnl Carnegie Mellon University, Princeton University.
dnl
dnl This file is part of Emergent
dnl
dnl   Emergent is free software; you can redistribute it and/or modify
dnl   it under the terms of the GNU General Public License as published by
dnl   the Free Software Foundation; either version 2 of the License, or
dnl   (at your option) any later version.
dnl
dnl   Emergent is distributed in the hope that it will be useful,
dnl   but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl   GNU General Public License for more details. 
AC_DEFUN([EMERGENT_CANONICAL_COMPILER],[

emergent_gpp=false
emergent_cl=false
emergent_other_compiler=false

case $CXX in
       g++)
          emergent_gpp=true
       ;;
       cl)
          emergent_cl=true        
       ;;
       *)
          emergent_other_compiler=true
          AC_MSG_WARN([$CXX compiler detected. Emergent has only been tested on gcc and cl. Proceed at your own risk.])
       ;;
esac

AM_CONDITIONAL([GCC],[test $emergent_gpp=true])
AM_CONDITIONAL([CL],[test $emergent_cl=true])
AM_CONDITIONAL([OTHER_COMPILER],[test $emergent_other_compiler=true])

])


dnl					       EMERGENT_CANONICAL_HOST
dnl *************************************************************
dnl  These tests are used mainly for Maketa.am at this point.
dnl *************************************************************
dnl Copyright, 1995-2007, Regents of the University of Colorado,
dnl Carnegie Mellon University, Princeton University.
dnl
dnl This file is part of Emergent
dnl
dnl   Emergent is free software; you can redistribute it and/or modify
dnl   it under the terms of the GNU General Public License as published by
dnl   the Free Software Foundation; either version 2 of the License, or
dnl   (at your option) any later version.
dnl
dnl   Emergent is distributed in the hope that it will be useful,
dnl   but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl   GNU General Public License for more details. 

AC_DEFUN([EMERGENT_CANONICAL_HOST],[
AC_REQUIRE([AC_CANONICAL_HOST])
emergent_linux=false
emergent_darwin=false
emergent_cygwin=false
emergent_mingw32=false
emergent_windows=false

case $host in
	*linux*)
                AC_DEFINE([HAVE_QT_CONSOLE],,[Used in Unix/Mac, but not Win])
		AC_DEFINE([LINUX],[1],[When on linux])
                emergent_linux=true
	;;
		*darwin*)
                LDFLAGS="$LDFLAGS -L/opt/local/lib -L/sw/lib"
                AC_DEFINE([HAVE_QT_CONSOLE],,[Used in Unix/Mac, but not Win])
		AC_DEFINE([DARWIN],[1],[When on darwin])
		AC_DEFINE([LINUX],[1],[When on darwin])
                emergent_darwin=true
	;;
	*-*-msdos* | *-*-go32* | *-*-cygwin* | *-*-windows*)

		AC_DEFINE([CYGWIN],[1],[When on cygwin])
		AC_DEFINE([WIN32],[1],[When on cygwin])
                emergent_cygwin=true
                emergent_windows=true
	;;
        *-*-mingw32*)
                CXXFLAGS="$CXXFLAGS -I/usr/local/include"
                LDFLAGS="$LDFLAGS -L/usr/local/lib"
		AC_DEFINE([CYGWIN],[1],[When on cygwin,mingw])
		AC_DEFINE([WIN32],[1],[When on win])
                emergent_mingw32=true
                emergent_windows=true
        ;;
	*)

	;;
esac

AM_CONDITIONAL([LINUX],[test $emergent_linux=true])
AM_CONDITIONAL([DARWIN],[test $emergent_darwin=true])
AM_CONDITIONAL([CYGWIN],[test $emergent_cygwin=true])
AM_CONDITIONAL([WINDOWS],[test $emergent_windows=true])
AM_CONDITIONAL([MINGW32],[test $emergent_mingw32=true])

SIM_AC_CONFIGURATION_SETTING([Host],[$host])
]) dnl EMERGENT_CANONICAL_HOST

        ;;
esac


dnl 					     EMERGENT_DETERMINE_SUFFIX
dnl *************************************************************
dnl  * Adds program version to libraries
dnl  * In debug mode, adds configure flag dependent suffixes. 
dnl    E.g. bp_nogui_debug_mpi++
dnl *************************************************************
dnl Copyright, 1995-2007, Regents of the University of Colorado,
dnl Carnegie Mellon University, Princeton University.
dnl
dnl This file is part of Emergent
dnl
dnl   Emergent is free software; you can redistribute it and/or modify
dnl   it under the terms of the GNU General Public License as published by
dnl   the Free Software Foundation; either version 2 of the License, or
dnl   (at your option) any later version.
dnl
dnl   Emergent is distributed in the hope that it will be useful,
dnl   but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl   GNU General Public License for more details.

AC_DEFUN([EMERGENT_DETERMINE_SUFFIX],[
if test "$gui" = "false" ; then
	EMERGENT_SUFFIX="${EMERGENT_SUFFIX}_nogui"
fi
if test "$debug" = "true" ; then
	EMERGENT_SUFFIX="${EMERGENT_SUFFIX}_debug"
fi
if test x"$mpi" = x"true"; then
	EMERGENT_SUFFIX="${EMERGENT_SUFFIX}_mpi"
fi
if test x"$profile" = x"true"; then
	EMERGENT_SUFFIX="${EMERGENT_SUFFIX}_prof"
fi
if test x"$nightly" = x"true"; then
	EMERGENT_SUFFIX="${EMERGENT_SUFFIX}_nightly"
fi
SIM_AC_CONFIGURATION_SETTING([Mode infix],[${EMERGENT_SUFFIX}])

]) dnl EMERGENT_DETERMINE_SUFFIX

dnl ACX_MPI([ACTION-IF-FOUND[, ACTION-IF-NOT-FOUND]]) (modified)
dnl *************************************************************
dnl @synopsis ACX_MPI([ACTION-IF-FOUND[, ACTION-IF-NOT-FOUND]])
dnl
dnl @summary figure out how to compile/link code with MPI
dnl
dnl This macro tries to find out how to compile programs that use MPI
dnl (Message Passing Interface), a standard API for parallel process
dnl communication (see http://www-unix.mcs.anl.gov/mpi/)
dnl
dnl On success, it sets the MPICC, MPICXX, or MPIF77 output variable to
dnl the name of the MPI compiler, depending upon the current language.
dnl (This may just be $CC/$CXX/$F77, but is more often something like
dnl mpicc/mpiCC/mpif77.) It also sets MPILIBS to any libraries that are
dnl needed for linking MPI (e.g. -lmpi, if a special
dnl MPICC/MPICXX/MPIF77 was not found).
dnl
dnl If you want to compile everything with MPI, you should set:
dnl
dnl     CC="$MPICC" #OR# CXX="$MPICXX" #OR# F77="$MPIF77"
dnl     LIBS="$MPILIBS $LIBS"
dnl	
dnl NOTE: The above assumes that you will use $CC (or whatever) for
dnl linking as well as for compiling. (This is the default for automake
dnl and most Makefiles.)
dnl
dnl The user can force a particular library/compiler by setting the
dnl MPICC/MPICXX/MPIF77 and/or MPILIBS environment variables.
dnl
dnl ACTION-IF-FOUND is a list of shell commands to run if an MPI
dnl library is found, and ACTION-IF-NOT-FOUND is a list of commands to
dnl run it if it is not found. If ACTION-IF-FOUND is not specified, the
dnl default action will define HAVE_MPI.
dnl
dnl @category InstalledPackages
dnl @author Steven G. Johnson <stevenj@alum.mit.edu>
dnl @version 2005-09-02
dnl @license GPLWithACException
AC_DEFUN([ACX_MPI], [
AC_PREREQ(2.50) dnl for AC_LANG_CASE

AC_LANG_CASE([C], [
	AC_REQUIRE([AC_PROG_CC])
	AC_ARG_VAR(MPICC,[MPI C compiler command])
	AC_CHECK_PROGS(MPICC, mpicc hcc mpcc mpcc_r mpxlc cmpicc, $CC)
	acx_mpi_save_CC="$CC"
	CC="$MPICC"
	AC_SUBST(MPICC)
],
[C++], [
	AC_REQUIRE([AC_PROG_CXX])
	AC_ARG_VAR(MPICXX,[MPI C++ compiler command])
	AC_CHECK_PROGS(MPICXX, mpic++ mpiCC mpicxx mpCC hcp mpxlC mpxlC_r cmpic++, $CXX)
	acx_mpi_save_CXX="$CXX"
	CXX="$MPICXX"
	AC_SUBST(MPICXX)
],
[Fortran 77], [
	AC_REQUIRE([AC_PROG_F77])
	AC_ARG_VAR(MPIF77,[MPI Fortran compiler command])
	AC_CHECK_PROGS(MPIF77, mpif77 hf77 mpxlf mpf77 mpif90 mpf90 mpxlf90 mpxlf95 mpxlf_r cmpifc cmpif90c, $F77)
	acx_mpi_save_F77="$F77"
	F77="$MPIF77"
	AC_SUBST(MPIF77)
])

if test x = x"$MPILIBS"; then
	AC_LANG_CASE([C], [AC_CHECK_FUNC(MPI_Init, [MPILIBS=" "])],
		[C++], [AC_CHECK_FUNC(MPI_Init, [MPILIBS=" "])],
		[Fortran 77], [AC_MSG_CHECKING([for MPI_Init])
			AC_TRY_LINK([],[      call MPI_Init], [MPILIBS=" "
				AC_MSG_RESULT(yes)], [AC_MSG_RESULT(no)])])
fi
if test x = x"$MPILIBS"; then
	AC_CHECK_LIB(mpi, MPI_Init, [MPILIBS="-lmpi"])
fi
if test x = x"$MPILIBS"; then
	AC_CHECK_LIB(mpich, MPI_Init, [MPILIBS="-lmpich"])
fi

dnl We have to use AC_TRY_COMPILE and not AC_CHECK_HEADER because the
dnl latter uses $CPP, not $CC (which may be mpicc).
AC_LANG_CASE([C], [if test x != x"$MPILIBS"; then
	AC_MSG_CHECKING([for mpi.h])
	AC_TRY_COMPILE([#include <mpi.h>],[],[AC_MSG_RESULT(yes)], [MPILIBS=""
		AC_MSG_RESULT(no)])
fi],
[C++], [if test x != x"$MPILIBS"; then
	AC_MSG_CHECKING([for mpi.h])
	AC_TRY_COMPILE([#include <mpi.h>],[],[AC_MSG_RESULT(yes)], [MPILIBS=""
		AC_MSG_RESULT(no)])
fi])

AC_LANG_CASE([C], [CC="$acx_mpi_save_CC"],
	[C++], [CXX="$acx_mpi_save_CXX"],
	[Fortran 77], [F77="$acx_mpi_save_F77"])

LIBS="$LIBS $MPILIBS"

dnl Finally, execute ACTION-IF-FOUND/ACTION-IF-NOT-FOUND:
if test x = x"$MPILIBS"; then
	$2
	:
else
	ifelse([$1],,[AC_DEFINE(HAVE_MPI,1,[Define if you have the MPI library.])],[$1])
	:
fi
])dnl ACX_MPI

dnl 			              AX_INSTALL_FILES (modified)
dnl *************************************************************
dnl @synopsis AX_INSTALL_FILES
dnl
dnl Adds target for creating a install_files file, which contains the
dnl list of files that will be installed.
dnl
dnl @category Automake
dnl @author Tom Howard <tomhoward@users.sf.net>
dnl @version 2005-01-14
dnl @license AllPermissive

AC_DEFUN([AX_INSTALL_FILES],
[
AC_MSG_NOTICE([adding install_files support])
AC_REQUIRE([AC_PROG_AWK])

if test "x$AWK" != "x"; then
   AC_MSG_NOTICE([install_files support enabled])
   AX_HAVE_INSTALL_FILES=true
   AX_ADD_AM_MACRO([[
CLEANFILES += \\
\$(top_builddir)/install_files

\$(top_builddir)/install_files: do-mfstamp-recursive
	@if test \"\$(top_builddir)/mfstamp\" -nt \"\$(top_builddir)/install_files\"; then \\
	cd \$(top_builddir) && STAGING=\"\$(PWD)/staging\"; \\
	\$(MAKE) \$(AM_MAKEFLAGS) DESTDIR=\"\$\$STAGING\" install; \\
	cd \"\$\$STAGING\" && find "." ! -type d -print | \\
	$AWK \' \\
	    /^\\.\\/usr\\/local\\/lib/ { \\
		sub( /\\.\\/usr\\/local\\/lib/, \"%%{_libdir}\" ); } \\
	    /^\\.\\/usr\\/local\\/bin/ { \\
		sub( /\\.\\/usr\\/local\\/bin/, \"%%{_bindir}\" ); } \\
	    /^\\.\\/usr\\/local\\/include/ { \\
		sub( /\\.\\/usr\\/local\\/include/, \"%%{_includedir}\" ); } \\
	    /^\\.\\/usr\\/local\\/share/ { \\
		sub( /\\.\\/usr\\/local\\/share/, \"%%{_datadir}\" ); } \\
	    /^\\.\\/usr\\/local/ { \\
		sub( /\\.\\/usr\\/local/, \"%%{_prefix}\" ); } \\
	    /^\\./ { sub( /\\./, \"\" ); } \\
	    /./ { print; }\' > ../install_files; \\
	rm -rf \"\$\$STAGING\"; \\
	else \\
	    echo \"\\\`\$(top_builddir)/install_files\' is up to date.\"; \\
	fi

]])
    AX_ADD_RECURSIVE_AM_MACRO([do-mfstamp],[[
\$(top_builddir)/mfstamp:  do-mfstamp-recursive

do-mfstamp-am do-mfstamp: Makefile.in
	@echo \"timestamp for all Makefile.in files\" > \$(top_builddir)/mfstamp
	@touch ${AX_DOLLAR}@

]])
else
    AX_HAVE_INSTALL_FILES=false;
    AC_MSG_ERROR([install_files support disable... awk not found])
fi
])# AX_INSTALL_FILES

dnl 				       AX_ADD_AM_MACRO([RULE])
dnl *************************************************************
dnl @synopsis AX_ADD_AM_MACRO([RULE])
dnl
dnl Adds the specified rule to $AMINCLUDE
dnl
dnl @category Automake
dnl @author Tom Howard <tomhoward@users.sf.net>
dnl @version 2005-01-14
dnl @license AllPermissive

AC_DEFUN([AX_ADD_AM_MACRO],[
  AC_REQUIRE([AX_AM_MACROS])
  AX_APPEND_TO_FILE([$AMINCLUDE],[$1])
])

dnl 				 AX_APPEND_TO_FILE([FILE],[DATA])
dnl *************************************************************
dnl @synopsis AX_APPEND_TO_FILE([FILE],[DATA])
dnl
dnl Appends the specified data to the specified file.
dnl
dnl @category Automake
dnl @author Tom Howard <tomhoward@users.sf.net>
dnl @version 2005-01-14
dnl @license AllPermissive

AC_DEFUN([AX_APPEND_TO_FILE],[
AC_REQUIRE([AX_FILE_ESCAPES])
printf "$2" >> "$1"
])

dnl 		    AX_ADD_RECURSIVE_AM_MACRO([TARGET],[RULE])
dnl *************************************************************
dnl @synopsis AX_ADD_RECURSIVE_AM_MACRO([TARGET],[RULE])
dnl
dnl Adds the specified rule to $AMINCLUDE along with a TARGET-recursive
dnl rule that will call TARGET for the current directory and TARGET-am
dnl recursively for each subdirectory
dnl
dnl @category Automake
dnl @author Tom Howard <tomhoward@users.sf.net>
dnl @version 2005-01-14
dnl @license AllPermissive
#  Adds the specified rule to $AMINCLUDE along with a TARGET-recursive rule that will call TARGET for the current directory and TARGET-am recursively for each subdirectory

AC_DEFUN([AX_ADD_RECURSIVE_AM_MACRO],[
  AX_ADD_AM_MACRO([
$1-recursive:
	@set fnord ${AX_DOLLAR}${AX_DOLLAR}MAKEFLAGS; amf=${AX_DOLLAR}${AX_DOLLAR}2; \\
	dot_seen=no; \\
	list='${AX_DOLLAR}(SUBDIRS)'; for subdir in ${AX_DOLLAR}${AX_DOLLAR}list; do \\
	  echo \"Making $1 in ${AX_DOLLAR}${AX_DOLLAR}subdir\"; \\
	  if test \"${AX_DOLLAR}${AX_DOLLAR}subdir\" = \".\"; then \\
	    dot_seen=yes; \\
	    local_target=\"$1-am\"; \\
	  else \\
	    local_target=\"$1\"; \\
	  fi; \\
	  (cd ${AX_DOLLAR}${AX_DOLLAR}subdir && ${AX_DOLLAR}(MAKE) ${AX_DOLLAR}(AM_MAKEFLAGS) ${AX_DOLLAR}${AX_DOLLAR}local_target) \\
	   || case \"${AX_DOLLAR}${AX_DOLLAR}amf\" in *=*) exit 1;; *k*) fail=yes;; *) exit 1;; esac; \\
	done; \\
	if test \"${AX_DOLLAR}${AX_DOLLAR}dot_seen\" = \"no\"; then \\
	  ${AX_DOLLAR}(MAKE) ${AX_DOLLAR}(AM_MAKEFLAGS) \"$1-am\" || exit 1; \\
	fi; test -z \"${AX_DOLLAR}${AX_DOLLAR}fail\"

$2
])
])

dnl 						 AX_AM_MACROS
dnl *************************************************************
dnl @synopsis AX_AM_MACROS
dnl
dnl Adds support for macros that create automake rules. You must
dnl manually add the following line
dnl
dnl   @INC_AMINCLUDE@
dnl
dnl to your Makefile.am files.
dnl
dnl @category Automake
dnl @author Tom Howard <tomhoward@users.sf.net>
dnl @version 2005-01-14
dnl @license AllPermissive

AC_DEFUN([AX_AM_MACROS],
[
AC_MSG_NOTICE([adding automake macro support])
AMINCLUDE="aminclude.am"
AC_SUBST(AMINCLUDE)
AC_MSG_NOTICE([creating $AMINCLUDE])
AMINCLUDE_TIME=`date`
AX_PRINT_TO_FILE([$AMINCLUDE],[[
# generated automatically by configure from AX_AUTOMAKE_MACROS
# on $AMINCLUDE_TIME

]])

INC_AMINCLUDE="include \$(top_builddir)/$AMINCLUDE"
AC_SUBST(INC_AMINCLUDE)
])

#				AX_PRINT_TO_FILE([FILE],[DATA])
#*************************************************************
#    http://autoconf-archive.cryp.to/ax_print_to_file.html
#*************************************************************
#  Writes the specified data to the specified file.

AC_DEFUN([AX_PRINT_TO_FILE],[
AC_REQUIRE([AX_FILE_ESCAPES])
printf "$2" > "$1"
])

dnl 					       AX_FILE_ESCAPES
dnl *************************************************************
dnl @synopsis AX_PRINT_TO_FILE([FILE],[DATA])
dnl
dnl Writes the specified data to the specified file.
dnl
dnl @category Automake
dnl @author Tom Howard <tomhoward@users.sf.net>
dnl @version 2005-01-14
dnl @license AllPermissive

AC_DEFUN([AX_FILE_ESCAPES],[
AX_DOLLAR="\$"
AX_SRB="\\135"
AX_SLB="\\133"
AX_BS="\\\\"
AX_DQ="\""
])

dnl 						 AX_EXTRA_DIST
dnl *************************************************************
dnl @synopsis AX_EXTRA_DIST
dnl
dnl Allow support for custom dist targets.
dnl
dnl To add custom dist targets, you must create a dist-<TYPE> target
dnl within your Makefile.am, where <TYPE> is the name of the dist and
dnl then add <TYPE> to EXTRA_SRC_DISTS or EXTRA_BIN_DISTS. For example:
dnl
dnl    dist-foobar:
dnl    	<rules for making the foobar dist>
dnl
dnl    EXTRA_BIN_DISTS += foobar
dnl
dnl You can then build all the src dist targets by running:
dnl
dnl    make dist-src
dnl
dnl You can build all the binary dist targets by running:
dnl
dnl    make dist-bin
dnl
dnl and you can build both the src and dist targets by running:
dnl
dnl    make all-dist
dnl
dnl @category Automake
dnl @author Tom Howard <tomhoward@users.sf.net>
dnl @version 2005-01-14
dnl @license AllPermissive

AC_DEFUN([AX_EXTRA_DIST],
[
AC_MSG_NOTICE([adding custom dist support])
AM_CONDITIONAL(USING_AX_EXTRA_DIST, [true])
AX_ADD_AM_MACRO([[
EXTRA_SRC_DISTS =
EXTRA_BIN_DISTS =
dist-src-extra:
	@echo \"Making custom src targets...\"
	@cd \$(top_builddir); \\
	list='\$(EXTRA_SRC_DISTS)'; \\
	for dist in \$\$list; do \\
	    \$(MAKE) \$(AM_MAKEFLAGS) dist-\$\$dist; \\
	done

dist-src: dist-all dist-src-extra


dist-bin:
	@echo \"Making custom binary targets...\"
	@cd \$(top_builddir); \\
	list='\$(EXTRA_BIN_DISTS)'; \\
	for dist in \$\$list; do \\
	    \$(MAKE) \$(AM_MAKEFLAGS) dist-\$\$dist; \\
	done

all-dist dist2 dist-all2: dist-src dist-bin

all-dist-check dist2-check dist-all-check: dist-check dist-src-extra dist-bin
]])
])# AX_EXTRA_DIST

dnl 					   AX_DIST_RPM([SPEC])
dnl *************************************************************
dnl @synopsis AX_DIST_RPM([SPEC])
dnl
dnl Adds support for a rpm dist target.
dnl
dnl You will need to create a spec template with everything except the
dnl files and the Changlog. @NAME@ will be replaced with the value of
dnl @PACKAGE@ and @VER@ will be replaced with the value of @VERSION@.
dnl The files and ChangeLog will be filled in automatically. For
dnl instance:
dnl
dnl     Summary: Foobar
dnl     Name: @NAME@
dnl     Version: @VER@
dnl     Release: 0
dnl     Copyright: GPL
dnl     Group: Productivity/Networking
dnl     Source0: http://somewhere/Foobar/%{name}-%{version}.tar.gz
dnl     URL: http://somewhere
dnl     BuildRoot: %{_tmppath}/%{name}-root
dnl     Prefix: %{_prefix}
dnl
dnl     %description
dnl     Foobar does something
dnl
dnl     %prep
dnl     %setup
dnl
dnl     %build
dnl     %configure
dnl     make
dnl
dnl     %install
dnl     %makeinstall
dnl
dnl     %clean
dnl     rm -rf $RPM_BUILD_ROOT
dnl
dnl     %files
dnl     %defattr(-,root,root)
dnl
dnl     %doc AUTHORS BUGS COPYING INSTALL NEWS README
dnl
dnl     %changelog
dnl
dnl Make sure ax_upload.am is added to aminclude.am and you have
dnl 'include aminclude.am' in your toplevel Makefile.am
dnl
dnl @category Automake
dnl @author Tom Howard <tomhoward@users.sf.net>
dnl @version 2005-01-14
dnl @license AllPermissive

AC_DEFUN([AX_DIST_RPM],
[
AC_REQUIRE([AX_INSTALL_FILES])
AC_MSG_NOTICE([adding rpm support])
if test "x$AX_HAVE_INSTALL_FILES" = "xtrue"; then
    AX_ADD_AM_MACRO([[

CLEAN_FILES += \$(top_builddir)/RPMChangeLog

\$(top_builddir)/RPMChangeLog: \$(top_srcdir)/ChangeLog
	$AWK \'/^[^0-9]/ { \\
		    if( \$${AX_DOLLAR}1 == \"*\" ) \$${AX_DOLLAR}1 = \"-\"; print; } \\
		/^\$\$/ { \\
		    print; } \\
		/^[0-9][0-9][0-9][0-9]-[0-9][0-9]-[0-9][0-9]/ { \\
		    if( stop ) exit; \\
		    split( \$${AX_DOLLAR}1, date, \"-\" ); \\
		    \$${AX_DOLLAR}1 = strftime( \"%%a %%b %%d %%Y\", \\
				    mktime( date[ 1 ] \" \" date[ 2 ] \" \" date [ 3 ] \" 00 00 00\" ) ); \\
		    print \"* \" \$${AX_DOLLAR}0; } \\
		/Released $PACKAGE-/ { \\
		    gsub( /$PACKAGE-/, \"\", \$${AX_DOLLAR}2 ); \\
		    split( \$${AX_DOLLAR}2, ver, \".\" ); \\
		    split( "$VERSION", curr_ver, \".\" ); \\
		    if( ver[ 1 ] != curr_ver[ 1 ] || ver[ 2 ] != curr_ver[ 2 ] ) stop = 1; } \\
		END { \\
		    print \"[Please see the ChangeLog file for older changes] - Ed.\"; }\' \\
		\$(top_srcdir)/ChangeLog > \$(top_builddir)/RPMChangeLog

]])
    AC_MSG_CHECKING([rpm spec template])
    if test -f "$1.in"; then
	AC_MSG_RESULT([$1.in])
	AX_RPM_SPEC_FILE="$1";
	AC_SUBST(AX_RPM_SPEC_FILE)
	AC_CONFIG_FILES([spec.tmpl])
	AX_ADD_AM_MACRO([[

CLEAN_FILES += \$(top_builddir)/$PACKAGE-$VERSION.spec
EXTRA_DIST += \$(top_builddir)/$PACKAGE-$VERSION.spec \$(top_builddir)/$AX_RPM_SPEC_FILE.in

spec: \$(top_builddir)/$PACKAGE-$VERSION.spec

\$(top_builddir)/$PACKAGE-$VERSION.spec:	\$(top_builddir)/$AX_RPM_SPEC_FILE \$(top_builddir)/install_files \$(top_builddir)/RPMChangeLog
	@cat \"\$(top_builddir)/$AX_RPM_SPEC_FILE\" \\
	| $AWK -v files=\"\$\$files\" \\
	    \'{ print; } \\
	    /%%defattr/ { while((getline < \"install_files\" ) > 0 ) { print; } }\' \\
	    > \"\$(top_builddir)/$PACKAGE-$VERSION.spec\"
	@cat \"\$(top_builddir)/RPMChangeLog\" >> \"\$(top_builddir)/$PACKAGE-$VERSION.spec\"

]])

	AC_ARG_VAR(EDITOR, [default text editor])
	if test "x$EDITOR" = "x"; then
	    AC_CHECK_PROGS(EDITOR,[vim vi emacs])
	fi
	AC_ARG_VAR(RPM, [rpm executable to use])
	if test "x$RPM" = "x"; then
	    AC_CHECK_PROGS(RPM,[rpmbuild rpm echo])
	fi
	if test "x$RPM" != "x"; then
	    AC_ARG_VAR(PLATFORM_SUFFIX, [A short name for your platform that will be added to the name of the the binary RPMs you may choose to create (by running make rpm).  e.g. rh71 for RedHat 7.1, lsb1.3 for a LSB 1.3 compliant system (SuSE 8.2 or RedHat 9), osx103 for OS X 10.3.x, etc])
	    AC_MSG_CHECKING([rpm platform suffix])
	    if test "x$PLATFORM_SUFFIX" != "x"; then
		AC_MSG_RESULT([$PLATFORM_SUFFIX])
		AX_ADD_AM_MACRO([[
CLEANFILES += \\
\$(top_builddir)/*.rpm \\
\$(top_builddir)/rpmmacros

~/.rpmmacros:
	@echo \"~/.rpmmacros not found.  Creating one like the following:\"; \\
	echo \"\"; \\
	echo \"%%packager       <YOUR NAME>\"; \\
	echo \"\"; \\
	echo \"%%distribution   <YOUR_DISTRIBUTION>\"; \\
	echo \"%%vendor	 <YOUR_ORGANISATION>\"; \\
	echo \"\"; \\
	echo \"%%_topdir	\$\$HOME/\"; \\
	echo \"%%_tmppath       /tmp/\"; \\
	echo \"\"; \\
	echo \"%%_rpmtopdir     %%{_topdir}rpm/\"; \\
	echo \"%%_builddir      %%{_tmppath}\"; \\
	echo \"%%_rpmdir	%%{_rpmtopdir}RPMS/\"; \\
	echo \"%%_sourcedir     %%{_rpmtopdir}SOURCES/\"; \\
	echo \"%%_specdir       %%{_rpmtopdir}SPECS/\"; \\
	echo \"%%_srcrpmdir     %%{_rpmtopdir}SRPMS/\"; \\
	echo \"\"; \\
	echo \"Where <YOUR_NAME> is your name and <YOUR_DISTRIBUTION> is the\"; \\
	echo \"distribution you are building on (e.g. SuSE Linux 8.2 or\"; \\
	echo \"RedHat Linux 7.1). and <YOUR_ORGANISATION> is the organisation\"; \\
	echo \"you are associated with.\"; \\
	echo \"\"; \\
	echo \"Running \'make create-rpmmacros\' will create a ~/.rpmmacros file\"; \\
	echo \"for you and try to open it in an editor\"; \\
	exit -1

create-rpmmacros:
	@if test -e ~/.rpmmacros; then \\
	    echo \"Error: ~/.rpmmacros already exists\"; \\
	    echo \"Please remove it if you want to use\"; \\
	    echo \"this command to replace it\"; \\
	    exit -1; \\
	else \\
	    echo \"%%packager       <YOUR NAME>\" > ~/.rpmmacros; \\
	    echo \"\" >> ~/.rpmmacros; \\
	    echo \"%%distribution   <YOUR_DISTRIBUTION>\" >> ~/.rpmmacros; \\
	    echo \"%%vendor	 <YOUR_ORGANISATION>\" >> ~/.rpmmacros; \\
	    echo \"\" >> ~/.rpmmacros; \\
	    echo \"%%_topdir	\$\$HOME/\" >> ~/.rpmmacros; \\
	    echo \"%%_tmppath       /tmp/\" >> ~/.rpmmacros; \\
	    echo \"\" >> ~/.rpmmacros; \\
	    echo \"%%_rpmtopdir     %%{_topdir}rpm/\" >> ~/.rpmmacros; \\
	    echo \"%%_builddir      %%{_tmppath}\" >> ~/.rpmmacros; \\
	    echo \"%%_rpmdir	%%{_rpmtopdir}RPMS/\" >> ~/.rpmmacros; \\
	    echo \"%%_sourcedir     %%{_rpmtopdir}SOURCES/\" >> ~/.rpmmacros; \\
	    echo \"%%_specdir       %%{_rpmtopdir}SPECS/\" >> ~/.rpmmacros; \\
	    echo \"%%_srcrpmdir     %%{_rpmtopdir}SRPMS/\" >> ~/.rpmmacros; \\
	    if test \"x$EDITOR\" = "x"; then \\
		echo \"Error: no editor specified or found.\"; \\
		echo \"Please edit ~/.rpmmacros manually\"; \\
	    else \\
		$EDITOR ~/.rpmmacros; \\
		exit \$\$?; \\
	    fi \\
	fi

rpmmacros: ~/.rpmmacros
	@cp -f ~/.rpmmacros rpmmacros;

dist-rpm: rpm
dist-srpm: srpm

rpm: $PACKAGE-$VERSION-0.i*.$PLATFORM_SUFFIX.rpm
srpm: $PACKAGE-$VERSION-0.src.rpm

\$(top_builddir)/$PACKAGE-$VERSION-0.i*.$PLATFORM_SUFFIX.rpm:   \$(top_builddir)/rpmmacros \$(top_builddir)/$PACKAGE-$VERSION.tar.gz
	@$RPM -tb \$(top_builddir)/$PACKAGE-$VERSION.tar.gz
	@RPMDIR=\`cat \$(top_builddir)/rpmmacros | $AWK \'/%%_rpmdir/ { print \$${AX_DOLLAR}2; }\'\`; \\
	echo \"\$\$RPMDIR\" | $EGREP \"%%{.*}\" > /dev/null 2>&1; \\
	EXIT=\$\$?; \\
	while test \"\$\$EXIT\" == \"0\"; do \\
		RPMDIR=\`echo \"\$\$RPMDIR\" | $AWK \'/%%{.*}/ \\
		{ match( \$${AX_DOLLAR}0, /%%{.*}/, macro ); \\
		  suffix = substr( \$${AX_DOLLAR}0, RSTART + RLENGTH ); \\
		  gsub( /{|}/, \"\", macro[ 0 ] ); \\
		  while( ( getline < \"\$(top_builddir)/rpmmacros\" ) > 0 ) \\
		  { if( \$${AX_DOLLAR}1 == macro[ 0 ] ) { print \$${AX_DOLLAR}2 suffix; exit; } } \\
		  exit 1; \\
		}\'\`; \\
		if test \"\$${AX_DOLLAR}?\" == \"0\"; then \\
			echo \"\$\$RPMDIR\" | $EGREP \"%%{.*}\" > /dev/null 2>&1; \\
			EXIT=\"\$${AX_DOLLAR}?\"; \\
		else \\
			EXIT=\"1\"; \\
		fi; \\
	done; \\
	if test -d \"\$\$RPMDIR\"; then \\
	    for dir in \`ls \"\$\$RPMDIR\"\`; do \\
		ls \"\$\${RPMDIR}\$\${dir}/$PACKAGE-$VERSION-0.\$\${dir}.rpm\" > /dev/null 2>&1; \\
		if test \"\$${AX_DOLLAR}?\" == \"0\"; then \\
		    cp \"\$\${RPMDIR}\$\${dir}/$PACKAGE-$VERSION-0.\$\${dir}.rpm\" \"\$(top_builddir)/$PACKAGE-$VERSION-0.\$\${dir}.$PLATFORM_SUFFIX.rpm\"; \\
		    found=true; \\
		fi; \\
	    done; \\
	    if ! \$\$found; then \\
		echo \"RPM built but not found.\"; \\
		echo \"Please copy it to \$(top_builddir) manually.\"; \\
		exit 1; \\
	    fi; \\
	else \\
	    echo \"RPM built but I cannot find RPM directory.\"; \\
	    echo \"Please copy it to \$(top_builddir) manually.\"; \\
	    exit 1; \\
	fi;

\$(top_builddir)/$PACKAGE-$VERSION.tar.gz: \$(top_builddir)/install_files
	cd \"\$(top_builddir)\" && \$(MAKE) \$(AM_MAKEFLAGS) dist-gzip




$PACKAGE-$VERSION-0.src.rpm:    \$(top_builddir)/rpmmacros \$(top_builddir)/$PACKAGE-$VERSION.tar.gz
	@$RPM -ts \$(top_builddir)/$PACKAGE-$VERSION.tar.gz
	@SRPMDIR=\`cat rpmmacros | $AWK \'/%%_srcrpmdir/ { print ${AX_DOLLAR}${AX_DOLLAR}2; }\'\`; \\
	echo \"${AX_DOLLAR}${AX_DOLLAR}SRPMDIR\" | $EGREP \"%%{.*}\" > /dev/null 2>&1; \\
	EXIT=${AX_DOLLAR}${AX_DOLLAR}?; \\
	while test \"${AX_DOLLAR}${AX_DOLLAR}EXIT\" == \"0\"; do \\
	    SRPMDIR=\`echo \"${AX_DOLLAR}${AX_DOLLAR}SRPMDIR\" | $AWK \'/%%{.*}/ \\
		{ match( ${AX_DOLLAR}${AX_DOLLAR}0, /%%{.*}/, macro ); \\
		  suffix = substr( ${AX_DOLLAR}${AX_DOLLAR}0, RSTART + RLENGTH ); \\
		  gsub( /{|}/, \"\", macro[ 0 ] ); \\
		  while( ( getline < \"rpmmacros\" ) > 0 ) \\
		  { if( ${AX_DOLLAR}${AX_DOLLAR}1 == macro[ 0 ] ) { print ${AX_DOLLAR}${AX_DOLLAR}2 suffix; exit; } } \\
		  exit 1; \\
		}\'\`; \\
		if test \"${AX_DOLLAR}${AX_DOLLAR}?\" == \"0\"; then \\
			echo \"${AX_DOLLAR}${AX_DOLLAR}SRPMDIR\" | $EGREP \"%%{.*}\" > /dev/null 2>&1; \\
			EXIT=\"${AX_DOLLAR}${AX_DOLLAR}?\"; \\
		else \\
			EXIT=\"1\"; \\
		fi; \\
	done; \\
	if test -d \"${AX_DOLLAR}${AX_DOLLAR}SRPMDIR\"; then \\
	    ls \"${AX_DOLLAR}${AX_DOLLAR}{SRPMDIR}/$PACKAGE-$VERSION-0.src.rpm\" > /dev/null 2>&1; \\
	    if test \"${AX_DOLLAR}${AX_DOLLAR}?\" == \"0\"; then \\
		cp \"\$${AX_DOLLAR}{SRPMDIR}/$PACKAGE-$VERSION-0.src.rpm\" \"${AX_DOLLAR}(top_builddir)/.\"; \\
		found=true; \\
	    fi; \\
	    if ! ${AX_DOLLAR}${AX_DOLLAR}found; then \\
		echo \"SRPM built but not found.\"; \\
		echo \"Please copy it to ${AX_DOLLAR}(top_builddir) manually.\"; \\
		exit 1; \\
	    fi; \\
	else \\
	     echo \"SRPM built but I cannot find SRPM directory.\"; \\
	     echo \"Please copy it to ${AX_DOLLAR}(top_builddir) directory manually.\"; \\
	     exit 1; \\
	fi;


]])

		if test "x$USING_AX_EXTRA_DIST" != "x"; then
		    AX_ADD_AM_MACRO([[

EXTRA_BIN_DISTS += rpm
EXTRA_SRC_DISTS += srpm

]])
		fi

		if test "x$USING_AX_UPLOAD" != "x"; then
		    AX_ADD_AM_MACRO([[

UPLOAD_BIN += upload-rpm
UPLOAD_SRC += upload-srpm

UPLOAD_TARGETS += \\
{rpm=>$PACKAGE-$VERSION-0.i*.$PLATFORM_SUFFIX.rpm} \\
{srpm=>$PACKAGE-$VERSION-0.src.rpm}

]])
		fi
	    else
		AC_MSG_RESULT([$PLATFORM_SUFFIX])
		AC_MSG_NOTICE([rpm support disabled... PLATFORM_SUFFIX not set])
	    fi
	else
	    AC_MSG_NOTICE([rpm support disabled... neither rpmbuild or rpm was found])
	fi
    else
	AC_MSG_RESULT([not found])
	AC_MSG_NOTICE([rpm spec template "$1.in" could not be found. `make rpm' likely will not work correctly if you are performing a vpath build.])
    fi
else
    AC_MSG_NOTICE([rpm support disabled... install_files not available])
fi
])
dnl @synopsis AX_LANG_COMPILER_MS
dnl
dnl Check whether the compiler for the current language is Microsoft.
dnl
dnl This macro is modeled after _AC_LANG_COMPILER_GNU in the GNU
dnl Autoconf implementation.
dnl
dnl @category InstalledPackages
dnl @author Braden McDaniel <braden@endoframe.com>
dnl @version 2004-11-15
dnl @license AllPermissive

AC_DEFUN([AX_LANG_COMPILER_MS],
[AC_CACHE_CHECK([whether we are using the Microsoft _AC_LANG compiler],
		[ax_cv_[]_AC_LANG_ABBREV[]_compiler_ms],
[AC_COMPILE_IFELSE([AC_LANG_PROGRAM([], [[#ifndef _MSC_VER
       choke me
#endif
]])],
		   [ax_compiler_ms=yes],
		   [ax_compiler_ms=no])
ax_cv_[]_AC_LANG_ABBREV[]_compiler_ms=$ax_compiler_ms
])])


dnl PostgreSQL Data Base Management System

dnl Portions Copyright (c) 1996-2005, 
dnl PostgreSQL Global Development Group Portions 
dnl Copyright (c) 1994-1996 Regents of the University of California
dnl
dnl Permission to use, copy, modify, and distribute this software and 
dnl its documentation for any purpose, without fee, and without a written
dnl agreement is hereby granted, provided that the above copyright notice
dnl and this paragraph and the following two paragraphs appear in all copies.
dnl
dnl IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY 
dnl FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES, 
dnl INCLUDING LOST PROFITS, ARISING OUT OF THE USE OF THIS SOFTWARE AND
dnl  ITS DOCUMENTATION, EVEN IF THE UNIVERSITY OF CALIFORNIA HAS BEEN 
dnl ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
dnl
dnl THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES, 
dnl INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
dnl AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE PROVIDED HEREUNDER IS
dnl ON AN "AS IS" BASIS, AND THE UNIVERSITY OF CALIFORNIA HAS NO OBLIGATIONS 
dnl TO PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
dnl
dnl Additional note on Postgresql's copyright notice: According to GNU and EFF
dnl it is compatable with GPL. See:
dnl http://directory.fsf.org/postgresql.html
dnl http://www.gnu.org/licenses/info/X11.html
dnl Macro taken from:
dnl http://developer.postgresql.org/cvsweb.cgi/~checkout~/pgsql/config/programs.m4?rev=1.19;content-type=text%2Fplain

dnl PGAC_CHECK_READLINE
dnl -------------------
dnl Check for the readline library and dependent libraries, either
dnl termcap or curses.  Also try libedit, since NetBSD's is compatible.
dnl Add the required flags to LIBS, define HAVE_LIBREADLINE.

AC_DEFUN([PGAC_CHECK_READLINE],
[AC_REQUIRE([AC_CANONICAL_HOST])

AC_CACHE_VAL([pgac_cv_check_readline],
[pgac_cv_check_readline=no
pgac_save_LIBS=$LIBS
for pgac_rllib in -lreadline -ledit; do
  AC_MSG_CHECKING([for ${pgac_rllib}])
  for pgac_lib in "" " -ltermcap" " -lncurses" " -lcurses" ; do
    LIBS="${pgac_rllib}${pgac_lib} $pgac_save_LIBS"
    AC_TRY_LINK_FUNC([readline], [[
      # Older NetBSD, OpenBSD, and Irix have a broken linker that does not
      # reecognize dependent libraries; assume curses is needed if we didn't
      # find any dependency.
      case $host_os in
        netbsd* | openbsd* | irix*)
          if test x"$pgac_lib" = x"" ; then
            pgac_lib=" -lcurses"
          fi ;;
      esac
      pgac_cv_check_readline="${pgac_rllib}${pgac_lib}"
      break
    ]])
  done
  if test "$pgac_cv_check_readline" != no ; then
    AC_MSG_RESULT([yes ($pgac_cv_check_readline)])
    break
  else
    AC_MSG_RESULT([no])
  fi
done
LIBS=$pgac_save_LIBS
])[]dnl AC_CACHE_VAL
if test "$pgac_cv_check_readline" != no ; then
  LIBS="$pgac_cv_check_readline $LIBS"
  AC_DEFINE([TA_USE_READLINE],[1],[Define if you have a function readline library])
  SIM_AC_CONFIGURATION_SETTING([Readline],[enabled...$pgac_cv_check_readline])
else
  SIM_AC_CONFIGURATION_WARNING([none of termcap, ncurses, curses, readline, edit found. Readline support disabled.])
fi
])dnl PGAC_CHECK_READLINE

dnl 					 configuration_summary.m4
dnl *************************************************************
dnl 
dnl *************************************************************
dnl Copyright, 1998-2005, Systems in Motion AS
dnl
dnl This file is part of Coin3d Free Edition.
dnl
dnl   Coin3d is free software; you can redistribute it and/or modify
dnl   it under the terms of the GNU General Public License as published by
dnl   the Free Software Foundation; either version 2 of the License, or
dnl   (at your option) any later version.
dnl
dnl   but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl   GNU General Public License for more details. 

# **************************************************************************
# configuration_summary.m4
#
# This file contains some utility macros for making it easy to have a short
# summary of the important configuration settings printed at the end of the
# configure run.
#
# Authors:
#   Lars J. Aas <larsa@sim.no>
#

# **************************************************************************
# SIM_AC_CONFIGURATION_SETTING( DESCRIPTION, SETTING )
#
# This macro registers a configuration setting to be dumped by the
# SIM_AC_CONFIGURATION_SUMMARY macro.

AC_DEFUN([SIM_AC_CONFIGURATION_SETTING],
[ifelse($#, 2, [], [m4_fatal([SIM_AC_CONFIGURATION_SETTING: takes two arguments])])
if test x"${sim_ac_configuration_settings+set}" = x"set"; then
  sim_ac_configuration_settings="$sim_ac_configuration_settings|$1:$2"
else
  sim_ac_configuration_settings="$1:$2"
fi
]) # SIM_AC_CONFIGURATION_SETTING

# **************************************************************************
# SIM_AC_CONFIGURATION_WARNING( WARNING )
#
# This macro registers a configuration warning to be dumped by the
# SIM_AC_CONFIGURATION_SUMMARY macro.

AC_DEFUN([SIM_AC_CONFIGURATION_WARNING],
[ifelse($#, 1, [], [m4_fatal([SIM_AC_CONFIGURATION_WARNING: takes one argument])])
if test x"${sim_ac_configuration_warnings+set}" = x"set"; then
  sim_ac_configuration_warnings="$sim_ac_configuration_warnings|$1"
else
  sim_ac_configuration_warnings="$1"
fi
]) # SIM_AC_CONFIGURATION_WARNING

# **************************************************************************
# SIM_AC_CONFIGURATION_SUMMARY
#
# This macro dumps the settings and warnings summary.

AC_DEFUN([SIM_AC_CONFIGURATION_SUMMARY],
[ifelse($#, 0, [], [m4_fatal([SIM_AC_CONFIGURATION_SUMMARY: takes no arguments])])
sim_ac_settings="$sim_ac_configuration_settings"
sim_ac_num_settings=`echo "$sim_ac_settings" | tr -d -c "|" | wc -c`
sim_ac_maxlength=0
while test $sim_ac_num_settings -ge 0; do
  sim_ac_description=`echo "$sim_ac_settings" | cut -d: -f1`
  sim_ac_length=`echo "$sim_ac_description" | wc -c`
  if test $sim_ac_length -gt $sim_ac_maxlength; then
    sim_ac_maxlength=`expr $sim_ac_length + 0`
  fi
  sim_ac_settings=`echo $sim_ac_settings | cut -d"|" -f2-`
  sim_ac_num_settings=`expr $sim_ac_num_settings - 1`
done

sim_ac_maxlength=`expr $sim_ac_maxlength + 3`
sim_ac_padding=`echo "                                             " |
  cut -c1-$sim_ac_maxlength`

sim_ac_num_settings=`echo "$sim_ac_configuration_settings" | tr -d -c "|" | wc -c`
echo ""
echo "$PACKAGE configuration settings:"
while test $sim_ac_num_settings -ge 0; do
  sim_ac_setting=`echo $sim_ac_configuration_settings | cut -d"|" -f1`
  sim_ac_description=`echo "$sim_ac_setting" | cut -d: -f1`
  sim_ac_status=`echo "$sim_ac_setting" | cut -d: -f2-`
  # hopefully not too many terminals are too dumb for this
  echo -e "$sim_ac_padding $sim_ac_status\r  $sim_ac_description:"
  sim_ac_configuration_settings=`echo $sim_ac_configuration_settings | cut -d"|" -f2-`
  sim_ac_num_settings=`expr $sim_ac_num_settings - 1`
done

if test x${sim_ac_configuration_warnings+set} = xset; then
sim_ac_num_warnings=`echo "$sim_ac_configuration_warnings" | tr -d -c "|" | wc -c`
echo ""
echo "$PACKAGE configuration warnings:"
while test $sim_ac_num_warnings -ge 0; do
  sim_ac_warning=`echo "$sim_ac_configuration_warnings" | cut -d"|" -f1`
  echo "  * $sim_ac_warning"
  sim_ac_configuration_warnings=`echo $sim_ac_configuration_warnings | cut -d"|" -f2-`
  sim_ac_num_warnings=`expr $sim_ac_num_warnings - 1`
done
fi
]) # SIM_AC_CONFIGURATION_SUMMARY


# **************************************************************************
# SIM_AC_SPACE_IN_PATHS

AC_DEFUN([SIM_AC_SPACE_IN_PATHS], [
sim_ac_full_builddir=`pwd`
sim_ac_full_srcdir=`cd $srcdir; pwd`
if test -z "`echo $sim_ac_full_srcdir | tr -cd ' '`"; then :; else
  AC_MSG_WARN([Detected space character in the path leading up to the Coin source directory - this will probably cause random problems later. You are advised to move the Coin source directory to another location.])
  SIM_AC_CONFIGURATION_WARNING([Detected space character in the path leading up to the Coin source directory - this will probably cause random problems later. You are advised to move the Coin source directory to another location.])
fi
if test -z "`echo $sim_ac_full_builddir | tr -cd ' '`"; then :; else
  AC_MSG_WARN([Detected space character in the path leading up to the Coin build directory - this will probably cause random problems later. You are advised to move the Coin build directory to another location.])
  SIM_AC_CONFIGURATION_WARNING([Detected space character in the path leading up to the Coin build directory - this will probably cause random problems later. You are advised to move the Coin build directory to another location.])
fi
]) # SIM_AC_SPACE_IN_PATHS


# **************************************************************************
# SIM_AC_ERROR_MESSAGE_FILE( FILENAME )
#   Sets the error message file.  Default is $ac_aux_dir/m4/errors.txt.
#
# SIM_AC_ERROR( ERROR [, ERROR ...] )
#   Fetches the error messages from the error message file and displays
#   them on stderr. The configure process will subsequently exit.
#
# SIM_AC_WARN( ERROR [, ERROR ...] )
#   As SIM_AC_ERROR, but will not exit after displaying the message(s).
#
# SIM_AC_WITH_ERROR( WITHARG )
#   Invokes AC_MSG_ERROR in a consistent way for problems with the --with-*
#   $withval argument.
#
# SIM_AC_ENABLE_ERROR( ENABLEARG )
#   Invokes AC_MSG_ERROR in a consistent way for problems with the --enable-*
#   $enableval argument.
#
# Authors:
#   Lars J. Aas <larsa@sim.no>

AC_DEFUN([SIM_AC_ERROR_MESSAGE_FILE], [
sim_ac_message_file=$1
]) # SIM_AC_ERROR_MESSAGE_FILE

AC_DEFUN([SIM_AC_ONE_MESSAGE], [
: ${sim_ac_message_file=$ac_aux_dir/errors.txt}
if test -f $sim_ac_message_file; then
  sim_ac_message="`sed -n -e '/^!$1$/,/^!/ { /^!/ d; p; }' <$sim_ac_message_file`"
  if test x"$sim_ac_message" = x""; then
    AC_MSG_ERROR([no message named '$1' in '$sim_ac_message_file' - notify the $PACKAGE_NAME maintainer(s)])
  else
    eval "echo >&2 \"$sim_ac_message\""
  fi
else
  AC_MSG_ERROR([file '$sim_ac_message_file' not found - notify the $PACKAGE_NAME maintainer(s)])
fi
]) # SIM_AC_ONE_MESSAGE

AC_DEFUN([_SIM_AC_ERROR], [
SIM_AC_ONE_MESSAGE([$1])
ifelse([$2], , , [
echo >&2 ""
_SIM_AC_ERROR(m4_shift($@))])
]) # _SIM_AC_ERROR

AC_DEFUN([SIM_AC_ERROR], [
echo >&2 ""
_SIM_AC_ERROR($@)
echo >&2 ""
AC_MSG_ERROR([aborting])
]) # SIM_AC_ERROR

AC_DEFUN([SIM_AC_WARN], [
echo >&2 ""
_SIM_AC_ERROR($@)
echo >&2 ""
]) # SIM_AC_WARN

AC_DEFUN([SIM_AC_WITH_ERROR], [
AC_MSG_ERROR([invalid value "${withval}" for "$1" configure argument])
]) # SIM_AC_WITH_ERROR

AC_DEFUN([SIM_AC_ENABLE_ERROR], [
AC_MSG_ERROR([invalid value "${enableval}" for "$1" configure argument])
]) # SIM_AC_ENABLE_ERROR


# **************************************************************************
# SIM_AC_CHECK_HEADER_SILENT([header], [if-found], [if-not-found], [includes])
# 
# This macro will not output information, nor will it
# cache the result, so it can be used multiple times on the same header,
# trying out different compiler options.

AC_DEFUN([SIM_AC_CHECK_HEADER_SILENT],
[AS_VAR_PUSHDEF([ac_Header], [ac_cv_header_$1])
m4_ifval([$4],
         [AC_COMPILE_IFELSE([AC_LANG_SOURCE([$4
@%:@include <$1>])],
                            [AS_VAR_SET(ac_Header, yes)],
                            [AS_VAR_SET(ac_Header, no)])],
         [AC_PREPROC_IFELSE([AC_LANG_SOURCE([@%:@include <$1>])],
                            [AS_VAR_SET(ac_Header, yes)],
                            [AS_VAR_SET(ac_Header, no)])])
AS_IF([test AS_VAR_GET(ac_Header) = yes], [$2], [$3])
AS_VAR_POPDEF([ac_Header])
])# SIM_AC_CHECK_HEADER_SILENT
############################################################################
# Usage:
#   SIM_AC_HAVE_SOQT_IFELSE( IF-FOUND, IF-NOT-FOUND )
#
# Description:
#   This macro locates the SoQt development system.  If it is found,
#   the set of variables listed below are set up as described and made
#   available to the configure script.
#
#   The $sim_ac_soqt_desired variable can be set to false externally to
#   make SoQt default to be excluded.
#
# Autoconf Variables:
# > $sim_ac_soqt_desired     true | false (defaults to true)
# < $sim_ac_soqt_avail       true | false
# < $sim_ac_soqt_cppflags    (extra flags the preprocessor needs)
# < $sim_ac_soqt_ldflags     (extra flags the linker needs)
# < $sim_ac_soqt_libs        (link library flags the linker needs)
# < $sim_ac_soqt_datadir     (location of SoQt data files)
# < $sim_ac_soqt_includedir  (location of SoQt headers)
# < $sim_ac_soqt_version     (the libSoQt version)
#
# Authors:
#   Lars J. Aas, <larsa@sim.no>
#   Morten Eriksen, <mortene@sim.no>
#
# TODO:
#

AC_DEFUN([SIM_AC_HAVE_SOQT_IFELSE], [
AC_PREREQ([2.14a])

# official variables
sim_ac_soqt_avail=false
sim_ac_soqt_cppflags=
sim_ac_soqt_ldflags=
sim_ac_soqt_libs=
sim_ac_soqt_datadir=
sim_ac_soqt_includedir=
sim_ac_soqt_version=

# internal variables
: ${sim_ac_soqt_desired=true}
sim_ac_soqt_extrapath=

AC_ARG_WITH([soqt], AC_HELP_STRING([--without-soqt], [disable use of SoQt]))
AC_ARG_WITH([soqt], AC_HELP_STRING([--with-soqt], [enable use of SoQt]))
AC_ARG_WITH([soqt],
  AC_HELP_STRING([--with-soqt=DIR], [give prefix location of SoQt]),
  [ case $withval in
    no)  sim_ac_soqt_desired=false ;;
    yes) sim_ac_soqt_desired=true ;;
    *)   sim_ac_soqt_desired=true
         sim_ac_soqt_extrapath=$withval ;;
    esac],
  [])

if $sim_ac_soqt_desired; then
  sim_ac_path=$PATH
  test -z "$sim_ac_soqt_extrapath" ||   ## search in --with-soqt path
    sim_ac_path=$sim_ac_soqt_extrapath/bin:$sim_ac_path
  test x"$prefix" = xNONE ||          ## search in --prefix path
    sim_ac_path=$sim_ac_path:$prefix/bin

  AC_PATH_PROG(sim_ac_soqt_configcmd, soqt-config, false, $sim_ac_path)

  if test "X$sim_ac_soqt_configcmd" = "Xfalse"; then :; else
    test -n "$CONFIG" &&
      $sim_ac_soqt_configcmd --alternate=$CONFIG >/dev/null 2>/dev/null &&
      sim_ac_soqt_configcmd="$sim_ac_soqt_configcmd --alternate=$CONFIG"
  fi

  if $sim_ac_soqt_configcmd; then
    sim_ac_soqt_cppflags=`$sim_ac_soqt_configcmd --cppflags`
    sim_ac_soqt_ldflags=`$sim_ac_soqt_configcmd --ldflags`
    sim_ac_soqt_libs=`$sim_ac_soqt_configcmd --libs`
    sim_ac_soqt_datadir=`$sim_ac_soqt_configcmd --datadir`
    sim_ac_soqt_includedir=`$sim_ac_soqt_configcmd --includedir`
    sim_ac_soqt_version=`$sim_ac_soqt_configcmd --version`

    case $host in
            *-*-mingw32*)
                sim_ac_soqt_cppflags="-DSOQT_DLL $sim_ac_soqt_cppflags"
                AC_MSG_WARN([sim_ac_soqt_cppflags: $sim_ac_soqt_cppflags])
            ;;
            *)
            ;;
    esac

    AC_CACHE_CHECK(
      [whether libSoQt is available],
      sim_cv_soqt_avail,
      [sim_ac_save_cppflags=$CPPFLAGS
      sim_ac_save_ldflags=$LDFLAGS
      sim_ac_save_libs=$LIBS
      CPPFLAGS="$CPPFLAGS $sim_ac_soqt_cppflags"
      LDFLAGS="$LDFLAGS $sim_ac_soqt_ldflags"
      LIBS="$sim_ac_soqt_libs $LIBS"
      AC_LANG_PUSH(C++)
      AC_TRY_LINK(
        [#include <Inventor/Qt/SoQt.h>],
        [(void)SoQt::init((const char *)0L);],
        [sim_cv_soqt_avail=true],
        [sim_cv_soqt_avail=false])
      AC_LANG_POP
      CPPFLAGS=$sim_ac_save_cppflags
      LDFLAGS=$sim_ac_save_ldflags
      LIBS=$sim_ac_save_libs
    ])
    sim_ac_soqt_avail=$sim_cv_soqt_avail
  else
    locations=`IFS=:; for p in $sim_ac_path; do echo " -> $p/soqt-config"; done`
    AC_MSG_WARN([cannot find 'soqt-config' at any of these locations:
$locations])
  fi
fi

if $sim_ac_soqt_avail; then
  ifelse([$1], , :, [$1])
else
  ifelse([$2], , :, [$2])
fi
]) # SIM_AC_HAVE_SOQT_IFELSE()

############################################################################
# Usage:
#   SIM_AC_HAVE_COIN_IFELSE( IF-FOUND, IF-NOT-FOUND )
#
# Description:
#   This macro locates the Coin development system.  If it is found,
#   the set of variables listed below are set up as described and made
#   available to the configure script.
#
#   The $sim_ac_coin_desired variable can be set to false externally to
#   make Coin default to be excluded.
#
# Autoconf Variables:
# > $sim_ac_coin_desired     true | false (defaults to true)
# < $sim_ac_coin_avail       true | false
# < $sim_ac_coin_cppflags    (extra flags the preprocessor needs)
# < $sim_ac_coin_cflags      (extra flags the C compiler needs)
# < $sim_ac_coin_cxxflags    (extra flags the C++ compiler needs)
# < $sim_ac_coin_ldflags     (extra flags the linker needs)
# < $sim_ac_coin_libs        (link library flags the linker needs)
# < $sim_ac_coin_datadir     (location of Coin data files)
# < $sim_ac_coin_includedir  (location of Coin headers)
# < $sim_ac_coin_version     (the libCoin version)
# < $sim_ac_coin_msvcrt      (the MSVC++ C library Coin was built with)
# < $sim_ac_coin_configcmd   (the path to coin-config or "false")
#
# Authors:
#   Lars J. Aas, <larsa@sim.no>
#   Morten Eriksen, <mortene@sim.no>
#
# TODO:
#

AC_DEFUN([SIM_AC_HAVE_COIN_IFELSE], [
AC_PREREQ([2.14a])

# official variables
sim_ac_coin_avail=false
sim_ac_coin_cppflags=
sim_ac_coin_cflags=
sim_ac_coin_cxxflags=
sim_ac_coin_ldflags=
sim_ac_coin_libs=
sim_ac_coin_datadir=
sim_ac_coin_includedir=
sim_ac_coin_version=

# internal variables
: ${sim_ac_coin_desired=true}
sim_ac_coin_extrapath=

AC_ARG_WITH([coin],
AC_HELP_STRING([--with-coin], [enable use of Coin [[default=yes]]])
AC_HELP_STRING([--with-coin=DIR], [give prefix location of Coin]),
  [ case $withval in
    no)  sim_ac_coin_desired=false ;;
    yes) sim_ac_coin_desired=true ;;
    *)   sim_ac_coin_desired=true
         sim_ac_coin_extrapath=$withval ;;
    esac],
  [])

case $build in
*-mks ) sim_ac_pathsep=";" ;;
* )     sim_ac_pathsep="${PATH_SEPARATOR}" ;;
esac

if $sim_ac_coin_desired; then
  sim_ac_path=$PATH
  test -z "$sim_ac_coin_extrapath" || ## search in --with-coin path
    sim_ac_path="$sim_ac_coin_extrapath/bin${sim_ac_pathsep}$sim_ac_path"
  test x"$prefix" = xNONE ||          ## search in --prefix path
    sim_ac_path="$sim_ac_path${sim_ac_pathsep}$prefix/bin"

  AC_PATH_PROG(sim_ac_coin_configcmd, coin-config, false, $sim_ac_path)

  if test "X$sim_ac_coin_configcmd" != "Xfalse"; then
    test -n "$CONFIG" &&
      $sim_ac_coin_configcmd --alternate=$CONFIG >/dev/null 2>/dev/null &&
      sim_ac_coin_configcmd="$sim_ac_coin_configcmd --alternate=$CONFIG"
  fi

  if $sim_ac_coin_configcmd; then
    sim_ac_coin_version=`$sim_ac_coin_configcmd --version`
    sim_ac_coin_cppflags=`$sim_ac_coin_configcmd --cppflags`
    sim_ac_coin_cflags=`$sim_ac_coin_configcmd --cflags 2>/dev/null`
    sim_ac_coin_cxxflags=`$sim_ac_coin_configcmd --cxxflags`
    sim_ac_coin_ldflags=`$sim_ac_coin_configcmd --ldflags`
    sim_ac_coin_libs=`$sim_ac_coin_configcmd --libs`
    sim_ac_coin_datadir=`$sim_ac_coin_configcmd --datadir`
    # Hide stderr on the following, as ``--includedir'', ``--msvcrt''
    # and ``--cflags'' options were added late to coin-config.
    sim_ac_coin_includedir=`$sim_ac_coin_configcmd --includedir 2>/dev/null`
    sim_ac_coin_msvcrt=`$sim_ac_coin_configcmd --msvcrt 2>/dev/null`
    sim_ac_coin_cflags=`$sim_ac_coin_configcmd --cflags 2>/dev/null`
    AC_CACHE_CHECK(
      [if we can compile and link with the Coin library],
      sim_cv_coin_avail,
      [sim_ac_save_cppflags=$CPPFLAGS
      sim_ac_save_cxxflags=$CXXFLAGS
      sim_ac_save_ldflags=$LDFLAGS
      sim_ac_save_libs=$LIBS
      CPPFLAGS="$CPPFLAGS $sim_ac_coin_cppflags"
      CXXFLAGS="$CXXFLAGS $sim_ac_coin_cxxflags"
      LDFLAGS="$LDFLAGS $sim_ac_coin_ldflags"
      LIBS="$sim_ac_coin_libs $LIBS"
      AC_LANG_PUSH(C++)

      AC_TRY_LINK(
        [#include <Inventor/SoDB.h>],
        [SoDB::init();],
        [sim_cv_coin_avail=true],
        [sim_cv_coin_avail=false])

      AC_LANG_POP
      CPPFLAGS=$sim_ac_save_cppflags
      CXXFLAGS=$sim_ac_save_cxxflags
      LDFLAGS=$sim_ac_save_ldflags
      LIBS=$sim_ac_save_libs
    ])
    sim_ac_coin_avail=$sim_cv_coin_avail

    if $sim_ac_coin_avail; then :; else
      AC_MSG_WARN([
Compilation and/or linking with the Coin main library SDK failed, for
unknown reason. If you are familiar with configure-based configuration
and building, investigate the 'config.log' file for clues.

If you can not figure out what went wrong, please forward the 'config.log'
file to the email address <coin-support@coin3d.org> and ask for help by
describing the situation where this failed.
])
    fi
  else # no 'coin-config' found

# FIXME: test for Coin without coin-config script here
    if test x"$COINDIR" != x""; then
      sim_ac_coindir=`cygpath -u "$COINDIR" 2>/dev/null || echo "$COINDIR"`
      if test -d $sim_ac_coindir/bin && test -d $sim_ac_coindir/lib && test -d $sim_ac_coindir/include/Inventor; then
        # using newest version (last alphabetically) in case of multiple libs
        sim_ac_coin_lib_file=`echo $sim_ac_coindir/lib/coin*.lib | sed -e 's,.* ,,g'`
        if test -f $sim_ac_coin_lib_file; then
          sim_ac_coin_lib_name=`echo $sim_ac_coin_lib_file | sed -e 's,.*/,,g' -e 's,.lib,,'`
          sim_ac_save_cppflags=$CPPFLAGS
          sim_ac_save_libs=$LIBS
          sim_ac_save_ldflags=$LDFLAGS
          CPPFLAGS="$CPPFLAGS -I$sim_ac_coindir/include"
          if test -f $sim_ac_coindir/bin/$sim_ac_coin_lib_name.dll; then
            CPPFLAGS="$CPPFLAGS -DCOIN_DLL"
          fi
          LDFLAGS="$LDFLAGS -L$sim_ac_coindir/lib"
          LIBS="-l$sim_ac_coin_lib_name -lopengl32 $LIBS"
          
          AC_LANG_PUSH(C++)

          AC_TRY_LINK(
            [#include <Inventor/SoDB.h>],
            [SoDB::init();],
            [sim_cv_coin_avail=true],
            [sim_cv_coin_avail=false])

          AC_LANG_POP
          CPPFLAGS=$sim_ac_save_cppflags
          LDFLAGS=$sim_ac_save_ldflags
          LIBS=$sim_ac_save_libs
          sim_ac_coin_avail=$sim_cv_coin_avail
        fi
      fi
    fi

    if $sim_ac_coin_avail; then
      sim_ac_coin_cppflags=-I$sim_ac_coindir/include
      if test -f $sim_ac_coindir/bin/$sim_ac_coin_lib_name.dll; then
        sim_ac_coin_cppflags="$sim_ac_coin_cppflags -DCOIN_DLL"
      fi
      sim_ac_coin_ldflags=-L$sim_ac_coindir/lib
      sim_ac_coin_libs="-l$sim_ac_coin_lib_name -lopengl32"
      sim_ac_coin_datadir=$sim_ac_coindir/data
    else
      locations=`IFS="${sim_ac_pathsep}"; for p in $sim_ac_path; do echo " -> $p/coin-config"; done`
      AC_MSG_WARN([cannot find 'coin-config' at any of these locations:
$locations])
      AC_MSG_WARN([
Need to be able to run 'coin-config' to figure out how to build and link
against the Coin library. To rectify this problem, you most likely need
to a) install Coin if it has not been installed, b) add the Coin install
bin/ directory to your PATH environment variable.
])
    fi
  fi
fi

if $sim_ac_coin_avail; then
  ifelse([$1], , :, [$1])
else
  ifelse([$2], , :, [$2])
fi
]) # SIM_AC_HAVE_COIN_IFELSE()

# **************************************************************************
# SIM_AC_CHECK_HEADER_SILENT([header], [if-found], [if-not-found], [includes])
# 
# This macro will not output any header checking information, nor will it
# cache the result, so it can be used multiple times on the same header,
# trying out different compiler options.

AC_DEFUN([SIM_AC_CHECK_HEADER_SILENT],
[AS_VAR_PUSHDEF([ac_Header], [ac_cv_header_$1])
m4_ifval([$4],
         [AC_COMPILE_IFELSE([AC_LANG_SOURCE([$4
@%:@include <$1>])],
                            [AS_VAR_SET(ac_Header, yes)],
                            [AS_VAR_SET(ac_Header, no)])],
         [AC_PREPROC_IFELSE([AC_LANG_SOURCE([@%:@include <$1>])],
                            [AS_VAR_SET(ac_Header, yes)],
                            [AS_VAR_SET(ac_Header, no)])])
AS_IF([test AS_VAR_GET(ac_Header) = yes], [$2], [$3])
AS_VAR_POPDEF([ac_Header])
])# SIM_AC_CHECK_HEADER_SILENT

# **************************************************************************
# SIM_AC_CHECK_HEADER_GL([IF-FOUND], [IF-NOT-FOUND])
#
# This macro detects how to include the GL header file, and gives you the
# necessary CPPFLAGS in $sim_ac_gl_cppflags, and also sets the config.h
# defines HAVE_GL_GL_H or HAVE_OPENGL_GL_H if one of them is found.

AC_DEFUN([SIM_AC_CHECK_HEADER_GL],
[sim_ac_gl_header_avail=false
AC_MSG_CHECKING([how to include gl.h])
if test x"$with_opengl" != x"no"; then
  sim_ac_gl_save_CPPFLAGS=$CPPFLAGS
  sim_ac_gl_cppflags=

  if test x"$with_opengl" != xyes && test x"$with_opengl" != x""; then
    sim_ac_gl_cppflags="-I${with_opengl}/include"
  else
    # On HP-UX platforms, OpenGL headers and libraries are usually installed
    # at this location.
    sim_ac_gl_hpux=/opt/graphics/OpenGL
    if test -d $sim_ac_gl_hpux; then
      sim_ac_gl_cppflags=-I$sim_ac_gl_hpux/include
    fi
  fi

  # On Mac OS X, GL is part of the optional X11 fraemwork
  case $host_os in
  darwin*)
    AC_REQUIRE([SIM_AC_CHECK_X11])
    if test x$sim_ac_enable_darwin_x11 = xtrue; then
      sim_ac_gl_darwin_x11=/usr/X11R6
      if test -d $sim_ac_gl_darwin_x11; then
        sim_ac_gl_cppflags=-I$sim_ac_gl_darwin_x11/include
      fi
    fi
    ;;
  esac

  CPPFLAGS="$CPPFLAGS $sim_ac_gl_cppflags"

  # Mac OS X framework (no X11, -framework OpenGL) 
  if test x$sim_ac_enable_darwin_x11 = xfalse; then
    SIM_AC_CHECK_HEADER_SILENT([OpenGL/gl.h], [
      sim_ac_gl_header_avail=true
      sim_ac_gl_header=OpenGL/gl.h
      AC_DEFINE([HAVE_OPENGL_GL_H], 1, [define if the GL header should be included as OpenGL/gl.h])
    ])
  else
    SIM_AC_CHECK_HEADER_SILENT([GL/gl.h], [
      sim_ac_gl_header_avail=true
      sim_ac_gl_header=GL/gl.h
      AC_DEFINE([HAVE_GL_GL_H], 1, [define if the GL header should be included as GL/gl.h])
    ])
  fi

  CPPFLAGS="$sim_ac_gl_save_CPPFLAGS"
  if $sim_ac_gl_header_avail; then
    if test x"$sim_ac_gl_cppflags" = x""; then
      AC_MSG_RESULT([@%:@include <$sim_ac_gl_header>])
    else
      AC_MSG_RESULT([$sim_ac_gl_cppflags, @%:@include <$sim_ac_gl_header>])
    fi
    $1
  else
    AC_MSG_RESULT([not found])
    $2
  fi
else
  AC_MSG_RESULT([disabled])
  $2
fi
])# SIM_AC_CHECK_HEADER_GL

# **************************************************************************
# SIM_AC_CHECK_HEADER_GLU([IF-FOUND], [IF-NOT-FOUND])
#
# This macro detects how to include the GLU header file, and gives you the
# necessary CPPFLAGS in $sim_ac_glu_cppflags, and also sets the config.h
# defines HAVE_GL_GLU_H or HAVE_OPENGL_GLU_H if one of them is found.

AC_DEFUN([SIM_AC_CHECK_HEADER_GLU],
[sim_ac_glu_header_avail=false
AC_MSG_CHECKING([how to include glu.h])
if test x"$with_opengl" != x"no"; then
  sim_ac_glu_save_CPPFLAGS=$CPPFLAGS
  sim_ac_glu_cppflags=

  if test x"$with_opengl" != xyes && test x"$with_opengl" != x""; then
    sim_ac_glu_cppflags="-I${with_opengl}/include"
  else
    # On HP-UX platforms, OpenGL headers and libraries are usually installed
    # at this location.
    sim_ac_gl_hpux=/opt/graphics/OpenGL
    if test -d $sim_ac_gl_hpux; then
      sim_ac_glu_cppflags=-I$sim_ac_gl_hpux/include
    fi
  fi

  # On Mac OS X, GL is part of the optional X11 fraemwork
  case $host_os in
  darwin*)
    AC_REQUIRE([SIM_AC_CHECK_X11])
    if test x$sim_ac_enable_darwin_x11 = xtrue; then
      sim_ac_gl_darwin_x11=/usr/X11R6
      if test -d $sim_ac_gl_darwin_x11; then
        sim_ac_gl_cppflags=-I$sim_ac_gl_darwin_x11/include
      fi
    fi
    ;;
  esac

  CPPFLAGS="$CPPFLAGS $sim_ac_glu_cppflags"

  # Mac OS X framework (no X11, -framework OpenGL) 
  if test x$sim_ac_enable_darwin_x11 = xfalse; then
    SIM_AC_CHECK_HEADER_SILENT([OpenGL/glu.h], [
      sim_ac_glu_header_avail=true
      sim_ac_glu_header=OpenGL/glu.h
      AC_DEFINE([HAVE_OPENGL_GLU_H], 1, [define if the GLU header should be included as OpenGL/glu.h])
    ])
  else
    SIM_AC_CHECK_HEADER_SILENT([GL/glu.h], [
      sim_ac_glu_header_avail=true
      sim_ac_glu_header=GL/glu.h
      AC_DEFINE([HAVE_GL_GLU_H], 1, [define if the GLU header should be included as GL/glu.h])
    ])
  fi
 
  CPPFLAGS="$sim_ac_glu_save_CPPFLAGS"
  if $sim_ac_glu_header_avail; then
    if test x"$sim_ac_glu_cppflags" = x""; then
      AC_MSG_RESULT([@%:@include <$sim_ac_glu_header>])
    else
      AC_MSG_RESULT([$sim_ac_glu_cppflags, @%:@include <$sim_ac_glu_header>])
    fi
    $1
  else
    AC_MSG_RESULT([not found])
    $2
  fi
else
  AC_MSG_RESULT([disabled])
  $2
fi
])# SIM_AC_CHECK_HEADER_GLU

# **************************************************************************
# SIM_AC_CHECK_HEADER_GLEXT([IF-FOUND], [IF-NOT-FOUND])
#
# This macro detects how to include the GLEXT header file, and gives you the
# necessary CPPFLAGS in $sim_ac_glext_cppflags, and also sets the config.h
# defines HAVE_GL_GLEXT_H or HAVE_OPENGL_GLEXT_H if one of them is found.

AC_DEFUN([SIM_AC_CHECK_HEADER_GLEXT],
[sim_ac_glext_header_avail=false
AC_MSG_CHECKING([how to include glext.h])
if test x"$with_opengl" != x"no"; then
  sim_ac_glext_save_CPPFLAGS=$CPPFLAGS
  sim_ac_glext_cppflags=

  if test x"$with_opengl" != xyes && test x"$with_opengl" != x""; then
    sim_ac_glext_cppflags="-I${with_opengl}/include"
  else
    # On HP-UX platforms, OpenGL headers and libraries are usually installed
    # at this location.
    sim_ac_gl_hpux=/opt/graphics/OpenGL
    if test -d $sim_ac_gl_hpux; then
      sim_ac_glext_cppflags=-I$sim_ac_gl_hpux/include
    fi
  fi

  # On Mac OS X, GL is part of the optional X11 fraemwork
  case $host_os in
  darwin*)
    AC_REQUIRE([SIM_AC_CHECK_X11])
    if test x$sim_ac_enable_darwin_x11 = xtrue; then
      sim_ac_gl_darwin_x11=/usr/X11R6
      if test -d $sim_ac_gl_darwin_x11; then
        sim_ac_gl_cppflags=-I$sim_ac_gl_darwin_x11/include
      fi
    fi
    ;;
  esac

  CPPFLAGS="$CPPFLAGS $sim_ac_glext_cppflags"

  # Mac OS X framework (no X11, -framework OpenGL) 
  if test x$sim_ac_enable_darwin_x11 = xfalse; then
    SIM_AC_CHECK_HEADER_SILENT([OpenGL/glext.h], [
      sim_ac_glext_header_avail=true
      sim_ac_glext_header=OpenGL/glext.h
      AC_DEFINE([HAVE_OPENGL_GLEXT_H], 1, [define if the GLEXT header should be included as OpenGL/glext.h])
    ])
  else
    SIM_AC_CHECK_HEADER_SILENT([GL/glext.h], [
      sim_ac_glext_header_avail=true
      sim_ac_glext_header=GL/glext.h
      AC_DEFINE([HAVE_GL_GLEXT_H], 1, [define if the GLEXT header should be included as GL/glext.h])
    ])
  fi

  CPPFLAGS="$sim_ac_glext_save_CPPFLAGS"
  if $sim_ac_glext_header_avail; then
    if test x"$sim_ac_glext_cppflags" = x""; then
      AC_MSG_RESULT([@%:@include <$sim_ac_glext_header>])
    else
      AC_MSG_RESULT([$sim_ac_glext_cppflags, @%:@include <$sim_ac_glext_header>])
    fi
    $1
  else
    AC_MSG_RESULT([not found])
    $2
  fi
else
  AC_MSG_RESULT([disabled])
  $2
fi
])# SIM_AC_CHECK_HEADER_GLEXT


# **************************************************************************
# SIM_AC_CHECK_OPENGL([IF-FOUND], [IF-NOT-FOUND])
#
# This macro detects whether or not it's possible to link against OpenGL
# (or Mesa), and gives you the necessary modifications to the
# pre-processor, compiler and linker environment in the envvars
#
#                $sim_ac_ogl_cppflags
#                $sim_ac_ogl_ldflags
#                $sim_ac_ogl_libs (OpenGL library and all dependencies)
#                $sim_ac_ogl_lib (basename of OpenGL library)
#
# The necessary extra options are also automatically added to CPPFLAGS,
# LDFLAGS and LIBS.
#
# Authors: <larsa@sim.no>, <mortene@sim.no>.

AC_DEFUN([SIM_AC_CHECK_OPENGL], [

sim_ac_ogl_cppflags=
sim_ac_ogl_ldflags=
sim_ac_ogl_lib=
sim_ac_ogl_libs=

AC_ARG_WITH(
  [mesa],
  AC_HELP_STRING([--with-mesa],
                 [prefer MesaGL (if found) over OpenGL [[default=no]]]),
  [],
  [with_mesa=no])

## Mac OS X uses some weird "framework" options.

## It's usually libGL.so on UNIX systems and opengl32.lib on MSWindows.
sim_ac_ogl_glnames="GL opengl32"
sim_ac_ogl_mesaglnames=MesaGL

if test "x$with_mesa" = "xyes"; then
  sim_ac_ogl_first=$sim_ac_ogl_mesaglnames
  sim_ac_ogl_second=$sim_ac_ogl_glnames
else
  sim_ac_ogl_first=$sim_ac_ogl_glnames
  sim_ac_ogl_second=$sim_ac_ogl_mesaglnames
fi

AC_ARG_WITH(
  [opengl],
  AC_HELP_STRING([--with-opengl=DIR],
                 [OpenGL/Mesa installation directory]),
  [],
  [with_opengl=yes])

if test x"$with_opengl" != xno; then

  if test x"$with_opengl" != xyes && test x"$with_opengl" != x""; then
    sim_ac_ogl_ldflags=-L$with_opengl/lib
    # $sim_ac_ogl_cppflags is set up in the SIM_AC_CHECK_HEADER_GL
    # invocation further below.
  else
    # On HP-UX platforms, OpenGL headers and libraries are usually installed
    # at this location.
    sim_ac_gl_hpux=/opt/graphics/OpenGL
    if test -d $sim_ac_gl_hpux; then
      sim_ac_ogl_ldflags=-L$sim_ac_gl_hpux/lib
    fi
  fi

  sim_ac_use_framework_option=false;
  case $host_os in
  darwin*)
    AC_REQUIRE([SIM_AC_CHECK_X11])
    if test x"$GCC" = x"yes" -a x$sim_ac_enable_darwin_x11 = xfalse; then
      SIM_AC_CC_COMPILER_OPTION([-framework OpenGL], [sim_ac_use_framework_option=true])
    else
      # On Mac OS X, OpenGL is installed as part of the optional X11 SDK.
      sim_ac_gl_darwin_x11=/usr/X11R6
      if test -d $sim_ac_gl_darwin_x11; then
        sim_ac_ogl_cppflags=-I$sim_ac_gl_darwin_x11/include
        sim_ac_ogl_ldflags=-L$sim_ac_gl_darwin_x11/lib
      fi
    fi
    ;;
  esac

  if $sim_ac_use_framework_option; then
    # hopefully, this is the default behavior and not needed. 20011005 larsa
    # sim_ac_ogl_cppflags="-F/System/Library/Frameworks/OpenGL.framework/"
    sim_ac_ogl_ldflags="-Wl,-framework,OpenGL"
    sim_ac_ogl_lib=OpenGL
  fi

  sim_ac_save_cppflags=$CPPFLAGS
  sim_ac_save_ldflags=$LDFLAGS
  sim_ac_save_libs=$LIBS

  CPPFLAGS="$CPPFLAGS $sim_ac_ogl_cppflags"
  LDFLAGS="$LDFLAGS $sim_ac_ogl_ldflags"

  SIM_AC_CHECK_HEADER_GL([CPPFLAGS="$CPPFLAGS $sim_ac_gl_cppflags"],
                         [AC_MSG_WARN([could not find gl.h])])

  sim_ac_glchk_hit=false
  for sim_ac_tmp_outerloop in barebones withpthreads; do
    if $sim_ac_glchk_hit; then :; else

      sim_ac_oglchk_pthreadslib=""
      if test "$sim_ac_tmp_outerloop" = "withpthreads"; then
        AC_MSG_WARN([couldn't compile or link with OpenGL library -- trying with pthread library in place...])
        LIBS="$sim_ac_save_libs"
        SIM_AC_CHECK_PTHREAD([
          sim_ac_ogl_cppflags="$sim_ac_ogl_cppflags $sim_ac_pthread_cppflags"
          sim_ac_ogl_ldflags="$sim_ac_ogl_ldflags $sim_ac_pthread_ldflags"
          sim_ac_oglchk_pthreadslib="$sim_ac_pthread_libs"
          ],
          [AC_MSG_WARN([couldn't compile or link with pthread library])
          ])
      fi

      AC_MSG_CHECKING([for OpenGL library dev-kit])
      # Mac OS X uses nada (only LDFLAGS), which is why "" was set first
      for sim_ac_ogl_libcheck in "" $sim_ac_ogl_first $sim_ac_ogl_second; do
        if $sim_ac_glchk_hit; then :; else
          if test -n "${sim_ac_ogl_libcheck}"; then
            LIBS="-l${sim_ac_ogl_libcheck} $sim_ac_oglchk_pthreadslib $sim_ac_save_libs"
          else
            LIBS="$sim_ac_oglchk_pthreadslib $sim_ac_save_libs"
          fi
          AC_TRY_LINK(
            [#ifdef HAVE_WINDOWS_H
             #include <windows.h>
             #endif
             #ifdef HAVE_GL_GL_H
             #include <GL/gl.h>
             #endif
             #ifdef HAVE_OPENGL_GL_H
             /* Mac OS X */
             #include <OpenGL/gl.h>
             #endif
            ],
            [glPointSize(1.0f);],
            [
             sim_ac_glchk_hit=true
             sim_ac_ogl_libs=$sim_ac_oglchk_pthreadslib
             if test -n "${sim_ac_ogl_libcheck}"; then
               sim_ac_ogl_lib=$sim_ac_ogl_libcheck
               sim_ac_ogl_libs="-l${sim_ac_ogl_libcheck} $sim_ac_oglchk_pthreadslib"
             fi
            ]
          )
        fi
      done
      if $sim_ac_glchk_hit; then
        AC_MSG_RESULT($sim_ac_ogl_cppflags $sim_ac_ogl_ldflags $sim_ac_ogl_libs)
      else
        AC_MSG_RESULT([unresolved])
      fi
    fi
  done

  if $sim_ac_glchk_hit; then
    LIBS="$sim_ac_ogl_libs $sim_ac_save_libs"
    $1
  else
    CPPFLAGS="$sim_ac_save_cppflags"
    LDFLAGS="$sim_ac_save_ldflags"
    LIBS="$sim_ac_save_libs"
    $2
  fi
fi
])


# **************************************************************************
# SIM_AC_GLU_READY_IFELSE( [ACTION-IF-TRUE], [ACTION-IF-FALSE] )

AC_DEFUN([SIM_AC_GLU_READY_IFELSE], [
sim_ac_glu_save_CPPFLAGS=$CPPFLAGS
SIM_AC_CHECK_HEADER_GLU(, [AC_MSG_WARN([could not find glu.h])])
if test x"$sim_ac_gl_cppflags" != x"$sim_ac_glu_cppflags"; then
  CPPFLAGS="$CPPFLAGS $sim_ac_gl_cppflags $sim_ac_glu_cppflags"
fi
AC_CACHE_CHECK(
  [if GLU is available as part of GL library],
  [sim_cv_glu_ready],
  [AC_TRY_LINK(
    [
#ifdef HAVE_WINDOWS_H
#include <windows.h>
#endif /* HAVE_WINDOWS_H */
#ifdef HAVE_GL_GL_H
#include <GL/gl.h>
#else
#ifdef HAVE_OPENGL_GL_H
#include <OpenGL/gl.h>
#endif
#endif
#ifdef HAVE_GL_GLU_H
#include <GL/glu.h>
#else
#ifdef HAVE_OPENGL_GLU_H
#include <OpenGL/glu.h>
#endif
#endif
],
    [
gluSphere(0L, 1.0, 1, 1);
/* Defect JAGad01283 of HP's aCC compiler causes a link failure unless
   there is at least one "pure" OpenGL call along with GLU calls. */
glEnd();
],
    [sim_cv_glu_ready=true],
    [sim_cv_glu_ready=false])])

if $sim_cv_glu_ready; then
  ifelse([$1], , :, [$1])
else
  ifelse([$2], , :, [$2])
fi
]) # SIM_AC_GLU_READY_IFELSE()

############################################################################
# Usage:
#  SIM_AC_CHECK_GLU([ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
#
#  Try to use the OpenGL utility library; GLU. If it is found,
#  these shell variables are set:
#
#    $sim_ac_glu_cppflags (extra flags the compiler needs for GLU)
#    $sim_ac_glu_ldflags  (extra flags the linker needs for GLU)
#    $sim_ac_glu_libs     (link libraries the linker needs for GLU)
#
#  The CPPFLAGS, LDFLAGS and LIBS flags will also be modified accordingly.
#  In addition, the variable $sim_ac_glu_avail is set to "yes" if GLU
#  is found.
#
#
# Author: Morten Eriksen, <mortene@sim.no>.

AC_DEFUN([SIM_AC_CHECK_GLU], [
sim_ac_glu_save_CPPFLAGS=$CPPFLAGS
SIM_AC_CHECK_HEADER_GLU(, [AC_MSG_WARN([could not find glu.h])])
if test x"$sim_ac_gl_cppflags" != x"$sim_ac_glu_cppflags"; then
  CPPFLAGS="$CPPFLAGS $sim_ac_gl_cppflags $sim_ac_glu_cppflags"
fi
sim_ac_glu_avail=no

# It's usually libGLU.so on UNIX systems and glu32.lib on MSWindows.
sim_ac_glu_names="-lGLU -lglu32"
sim_ac_glu_mesanames=-lMesaGLU

# with_mesa is set from the SIM_AC_CHECK_OPENGL macro.
if test "x$with_mesa" = "xyes"; then
  sim_ac_glu_first=$sim_ac_glu_mesanames
  sim_ac_glu_second=$sim_ac_glu_names
else
  sim_ac_glu_first=$sim_ac_glu_names
  sim_ac_glu_second=$sim_ac_glu_mesanames
fi

AC_ARG_WITH(
  [glu],
  AC_HELP_STRING([--with-glu=DIR],
                 [use the OpenGL utility library [[default=yes]]]),
  [],
  [with_glu=yes])

if test x"$with_glu" != xno; then
  if test x"$with_glu" != xyes; then
    # sim_ac_glu_cppflags="-I${with_glu}/include"
    sim_ac_glu_ldflags="-L${with_glu}/lib"
  fi

  sim_ac_save_cppflags=$CPPFLAGS
  sim_ac_save_ldflags=$LDFLAGS
  sim_ac_save_libs=$LIBS

  CPPFLAGS="$CPPFLAGS $sim_ac_glu_cppflags"
  LDFLAGS="$LDFLAGS $sim_ac_glu_ldflags"

  AC_CACHE_CHECK(
    [whether GLU is available],
    sim_cv_lib_glu,
    [sim_cv_lib_glu=UNRESOLVED

    # Some platforms (like BeOS) have the GLU functionality in the GL
    # library (and no GLU library present).
    for sim_ac_glu_libcheck in "" $sim_ac_glu_first $sim_ac_glu_second; do
      if test "x$sim_cv_lib_glu" = "xUNRESOLVED"; then
        LIBS="$sim_ac_glu_libcheck $sim_ac_save_libs"
        AC_TRY_LINK([
#ifdef HAVE_WINDOWS_H
#include <windows.h>
#endif /* HAVE_WINDOWS_H */
#ifdef HAVE_GL_GL_H
#include <GL/gl.h>
#else
#ifdef HAVE_OPENGL_GL_H
#include <OpenGL/gl.h>
#endif
#endif
#ifdef HAVE_GL_GLU_H
#include <GL/glu.h>
#else
#ifdef HAVE_OPENGL_GLU_H
#include <OpenGL/glu.h>
#endif
#endif
],
                    [
gluSphere(0L, 1.0, 1, 1);
/* Defect JAGad01283 of HP's aCC compiler causes a link failure unless
   there is at least one "pure" OpenGL call along with GLU calls. */
glEnd();
],
                    [sim_cv_lib_glu="$sim_ac_glu_libcheck"])
      fi
    done
    if test x"$sim_cv_lib_glu" = x"" &&
       test x`echo $LDFLAGS | grep -c -- "-Wl,-framework,OpenGL"` = x1; then
      # just for the visual representation on Mac OS X
      sim_cv_lib_glu="-Wl,-framework,OpenGL"
    fi
  ])

  LIBS="$sim_ac_save_libs"

  CPPFLAGS=$sim_ac_glu_save_CPPFLAGS
  if test "x$sim_cv_lib_glu" != "xUNRESOLVED"; then
    if test x"$sim_cv_lib_glu" = x"-Wl,-framework,OpenGL"; then
      sim_ac_glu_libs=""
    else
      sim_ac_glu_libs="$sim_cv_lib_glu"
    fi
    LIBS="$sim_ac_glu_libs $sim_ac_save_libs"
    sim_ac_glu_avail=yes
    $1
  else
    CPPFLAGS=$sim_ac_save_cppflags
    LDFLAGS=$sim_ac_save_ldflags
    LIBS=$sim_ac_save_libs
    $2
  fi
fi
])


############################################################################
# Usage:
#  SIM_AC_GLU_NURBSOBJECT([ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
#
#  Try to find out whether the interface struct against the GLU
#  library NURBS functions is called "GLUnurbs" or "GLUnurbsObj".
#  (This seems to have changed somewhere between release 1.1 and
#  release 1.3 of GLU).
#
#  The variable $sim_ac_glu_nurbsobject is set to the correct name
#  if the nurbs structure is found.
#
# Author: Morten Eriksen, <mortene@sim.no>.

AC_DEFUN([SIM_AC_GLU_NURBSOBJECT], [
AC_CACHE_CHECK(
  [what structure to use in the GLU NURBS interface],
  sim_cv_func_glu_nurbsobject,
  [sim_cv_func_glu_nurbsobject=NONE
   for sim_ac_glu_structname in GLUnurbs GLUnurbsObj; do
    if test "$sim_cv_func_glu_nurbsobject" = NONE; then
      AC_TRY_LINK([
#ifdef HAVE_WINDOWS_H
#include <windows.h>
#endif /* HAVE_WINDOWS_H */
#ifdef HAVE_GL_GL_H
#include <GL/gl.h>
#else
#ifdef HAVE_OPENGL_GL_H
#include <OpenGL/gl.h>
#endif
#endif
#ifdef HAVE_GL_GLU_H
#include <GL/glu.h>
#else
#ifdef HAVE_OPENGL_GLU_H
#include <OpenGL/glu.h>
#endif
#endif
],
                  [
$sim_ac_glu_structname * hepp = gluNewNurbsRenderer();
gluDeleteNurbsRenderer(hepp);
/* Defect JAGad01283 of HP's aCC compiler causes a link failure unless
   there is at least one "pure" OpenGL call along with GLU calls. */
glEnd();
],
                  [sim_cv_func_glu_nurbsobject=$sim_ac_glu_structname])
    fi
  done
])

if test $sim_cv_func_glu_nurbsobject = NONE; then
  sim_ac_glu_nurbsobject=
  $2
else
  sim_ac_glu_nurbsobject=$sim_cv_func_glu_nurbsobject
  $1
fi
])

# **************************************************************************
# SIM_AC_HAVE_GLXGETCURRENTDISPLAY_IFELSE( IF-FOUND, IF-NOT-FOUND )
#
# Check whether the OpenGL implementation includes the method
# glXGetCurrentDisplay().

AC_DEFUN([SIM_AC_HAVE_GLXGETCURRENTDISPLAY_IFELSE], [
AC_CACHE_CHECK(
  [whether glXGetCurrentDisplay() is available],
  sim_cv_have_glxgetcurrentdisplay,
  AC_TRY_LINK([
#include <GL/gl.h>
#include <GL/glx.h>
],
[(void)glXGetCurrentDisplay();],
[sim_cv_have_glxgetcurrentdisplay=true],
[sim_cv_have_glxgetcurrentdisplay=false]))

if ${sim_cv_have_glxgetcurrentdisplay}; then
  ifelse([$1], , :, [$1])
else
  ifelse([$2], , :, [$2])
fi
]) # SIM_AC_HAVE_GLXGETCURRENTDISPLAY_IFELSE()

# **************************************************************************
# SIM_AC_HAVE_GLX_IFELSE( IF-FOUND, IF-NOT-FOUND )
#
# Check whether GLX is on the system.

AC_DEFUN([SIM_AC_HAVE_GLX_IFELSE], [
AC_CACHE_CHECK(
  [whether GLX is on the system],
  sim_cv_have_glx,
  AC_TRY_LINK(
    [
#include <GL/glx.h>
#include <GL/gl.h>
],
    [
(void)glXChooseVisual(0L, 0, 0L);
/* Defect JAGad01283 of HP's aCC compiler causes a link failure unless
   there is at least one "pure" OpenGL call along with GLU calls. */
glEnd();
],
    [sim_cv_have_glx=true],
    [sim_cv_have_glx=false]))

if ${sim_cv_have_glx=false}; then
  ifelse([$1], , :, [$1])
else
  ifelse([$2], , :, [$2])
fi
]) # SIM_AC_HAVE_GLX_IFELSE()

# **************************************************************************
# SIM_AC_HAVE_GLXGETPROCADDRESSARB_IFELSE( IF-FOUND, IF-NOT-FOUND )
#
# Check for glXGetProcAddressARB() function.

AC_DEFUN([SIM_AC_HAVE_GLXGETPROCADDRESSARB_IFELSE], [
AC_CACHE_CHECK(
  [for glXGetProcAddressARB() function],
  sim_cv_have_glxgetprocaddressarb,
  AC_TRY_LINK(
    [
#include <GL/glx.h>
#include <GL/gl.h>
],
    [
      glXGetProcAddressARB((const GLubyte *)"glClearColor");
/* Defect JAGad01283 of HP's aCC compiler causes a link failure unless
   there is at least one "pure" OpenGL call along with GLU calls. */
      glEnd();
],
    [sim_cv_have_glxgetprocaddressarb=true],
    [sim_cv_have_glxgetprocaddressarb=false]))

if ${sim_cv_have_glxgetprocaddressarb=false}; then
  ifelse([$1], , :, [$1])
else
  ifelse([$2], , :, [$2])
fi
]) # SIM_AC_HAVE_GLXGETPROCADDRESSARB_IFELSE()


# **************************************************************************
# SIM_AC_HAVE_WGL_IFELSE( IF-FOUND, IF-NOT-FOUND )
#
# Check whether WGL is on the system.
#
# This macro has one important side-effect: the variable
# sim_ac_wgl_libs will be set to the list of libraries
# needed to link with wgl*() functions.

AC_DEFUN([SIM_AC_HAVE_WGL_IFELSE], [
sim_ac_save_libs=$LIBS
## Not directly needed by the wgl*() calls, but to create a
## context we need functions from this library.
sim_ac_wgl_libs="-lgdi32"
LIBS="$LIBS $sim_ac_wgl_libs"

AC_CACHE_CHECK(
  [whether WGL is on the system],
  sim_cv_have_wgl,
  AC_TRY_LINK(
    [
#include <windows.h>
#include <GL/gl.h>
],
    [(void)wglCreateContext(0L);],
    [sim_cv_have_wgl=true],
    [sim_cv_have_wgl=false]))

LIBS=$sim_ac_save_libs
if ${sim_cv_have_wgl=false}; then
  ifelse([$1], , :, [$1])
else
  ifelse([$2], , :, [$2])
fi
]) # SIM_AC_HAVE_WGL_IFELSE()

# **************************************************************************
# SIM_AC_HAVE_AGL_IFELSE( IF-FOUND, IF-NOT-FOUND )
#
# Check whether AGL is on the system.

AC_DEFUN([SIM_AC_HAVE_AGL_IFELSE], [
sim_ac_save_ldflags=$LDFLAGS
sim_ac_agl_ldflags="-Wl,-framework,ApplicationServices -Wl,-framework,AGL"

LDFLAGS="$LDFLAGS $sim_ac_agl_ldflags"

# see comment in Coin/src/glue/gl_agl.c: regarding __CARBONSOUND__ define 

AC_CACHE_CHECK(
  [whether AGL is on the system],
  sim_cv_have_agl,
  AC_TRY_LINK(
    [#include <AGL/agl.h>
     #define __CARBONSOUND__ 
     #include <Carbon/Carbon.h>],
    [aglGetCurrentContext();],
    [sim_cv_have_agl=true],
    [sim_cv_have_agl=false]))

LDFLAGS=$sim_ac_save_ldflags
if ${sim_cv_have_agl=false}; then
  ifelse([$1], , :, [$1])
else
  ifelse([$2], , :, [$2])
fi
]) # SIM_AC_HAVE_AGL_IFELSE()
 

AC_DEFUN([SIM_AC_HAVE_AGL_PBUFFER], [
  AC_CACHE_CHECK([whether we can use AGL pBuffers],
    sim_cv_agl_pbuffer_avail,
    [AC_TRY_LINK([ #include <AGL/agl.h> ],
                 [AGLPbuffer pbuffer;],
                 [sim_cv_agl_pbuffer_avail=yes],
                 [sim_cv_agl_pbuffer_avail=no])])
  
  if test x"$sim_cv_agl_pbuffer_avail" = xyes; then
    ifelse([$1], , :, [$1])
  else
    ifelse([$2], , :, [$2])
  fi
])

# EOF **********************************************************************
############################################################################
# Usage:
#   SIM_AC_COMPILE_DEBUG([ACTION-IF-DEBUG[, ACTION-IF-NOT-DEBUG]])
#
# Description:
#   Let the user decide if compilation should be done in "debug mode".
#   If compilation is not done in debug mode, all assert()'s in the code
#   will be disabled.
#
#   Also sets enable_debug variable to either "yes" or "no", so the
#   configure.in writer can add package-specific actions. Default is "yes".
#   This was also extended to enable the developer to set up the two first
#   macro arguments following the well-known ACTION-IF / ACTION-IF-NOT
#   concept.
#
# Authors:
#   Morten Eriksen, <mortene@sim.no>
#   Lars J. Aas, <larsa@sim.no>
#

AC_DEFUN([SIM_AC_COMPILE_DEBUG], [
AC_REQUIRE([SIM_AC_CHECK_SIMIAN_IFELSE])

AC_ARG_ENABLE(
  [debug],
  AC_HELP_STRING([--enable-debug], [compile in debug mode [[default=yes]]]),
  [case "${enableval}" in
    yes) enable_debug=true ;;
    no)  enable_debug=false ;;
    true | false) enable_debug=${enableval} ;;
    *) AC_MSG_ERROR(bad value "${enableval}" for --enable-debug) ;;
  esac],
  [enable_debug=true])

if $enable_debug; then
  DSUFFIX=d
  if $sim_ac_simian; then
    case $CXX in
    *wrapmsvc* )
      # uninitialized checks
      if test ${sim_ac_msvc_version-0} -gt 6; then
        SIM_AC_CC_COMPILER_OPTION([/RTCu], [sim_ac_compiler_CFLAGS="$sim_ac_compiler_CFLAGS /RTCu"])
        SIM_AC_CXX_COMPILER_OPTION([/RTCu], [sim_ac_compiler_CXXFLAGS="$sim_ac_compiler_CXXFLAGS /RTCu"])
        # stack frame checks
        SIM_AC_CC_COMPILER_OPTION([/RTCs], [sim_ac_compiler_CFLAGS="$sim_ac_compiler_CFLAGS /RTCs"])
        SIM_AC_CXX_COMPILER_OPTION([/RTCs], [sim_ac_compiler_CXXFLAGS="$sim_ac_compiler_CXXFLAGS /RTCs"])
      fi
      ;;
    esac
  fi

  ifelse([$1], , :, [$1])
else
  DSUFFIX=
  CPPFLAGS="$CPPFLAGS -DNDEBUG"
  ifelse([$2], , :, [$2])
fi
AC_SUBST(DSUFFIX)
])

############################################################################
# Usage:
#   SIM_AC_COMPILER_OPTIMIZATION
#
# Description:
#   Let the user decide if optimization should be attempted turned off
#   by stripping off an "-O[0-9]" option.
# 
#   Note: this macro must be placed after either AC_PROG_CC or AC_PROG_CXX
#   in the configure.in script.
#
# FIXME: this is pretty much just a dirty hack. Unfortunately, this
# seems to be the best we can do without fixing Autoconf to behave
# properly wrt setting optimization options. 20011021 mortene.
# 
# Author: Morten Eriksen, <mortene@sim.no>.
# 

AC_DEFUN([SIM_AC_COMPILER_OPTIMIZATION], [
AC_ARG_ENABLE(
  [optimization],
  AC_HELP_STRING([--enable-optimization],
                 [allow compilers to make optimized code [[default=yes]]]),
  [case "${enableval}" in
    yes) sim_ac_enable_optimization=true ;;
    no)  sim_ac_enable_optimization=false ;;
    *) AC_MSG_ERROR(bad value "${enableval}" for --enable-optimization) ;;
  esac],
  [sim_ac_enable_optimization=true])

if $sim_ac_enable_optimization; then
  :
else
  CFLAGS="`echo $CFLAGS | sed 's/-O[[0-9]]*[[ ]]*//'`"
  CXXFLAGS="`echo $CXXFLAGS | sed 's/-O[[0-9]]*[[ ]]*//'`"
fi
])
# **************************************************************************
# SIM_AC_HAVE_LIBZLIB_IFELSE( IF-FOUND, IF-NOT-FOUND )
#
# Variables:
#   sim_ac_have_libzlib
#   sim_ac_zlib_cppflags
#   sim_ac_zlib_ldflags
#   sim_ac_zlib_libs
#
# Authors:
#   Lars J. Aas <larsa@coin3d.org>
#   Morten Eriksen <mortene@coin3d.org>
#
# Todo:
# - use AS_UNSET to unset internal variables to avoid polluting the environment
#

# **************************************************************************

AC_DEFUN([SIM_AC_HAVE_LIBZLIB_IFELSE],
[: ${sim_ac_have_libzlib=false}
AC_MSG_CHECKING([for zlib])
AC_ARG_WITH(
  [zlib],
  [AC_HELP_STRING([--with-zlib=PATH], [enable/disable zlib support])],
  [case $withval in
  yes | "") sim_ac_want_libzlib=true ;;
  no)       sim_ac_want_libzlib=false ;;
  *)        sim_ac_want_libzlib=true
            sim_ac_libzlib_path=$withval ;;
  esac],
  [sim_ac_want_libzlib=true])
case $sim_ac_want_libzlib in
true)
  $sim_ac_have_libzlib && break
  sim_ac_libzlib_save_CPPFLAGS=$CPPFLAGS
  sim_ac_libzlib_save_LDFLAGS=$LDFLAGS
  sim_ac_libzlib_save_LIBS=$LIBS
  sim_ac_libzlib_debug=false
  test -n "`echo -- $CPPFLAGS $CFLAGS $CXXFLAGS | grep -- '-g\\>'`" &&
    sim_ac_libzlib_debug=true
  # test -z "$sim_ac_libzlib_path" -a x"$prefix" != xNONE &&
  #   sim_ac_libzlib_path=$prefix
  sim_ac_libzlib_name=z
  if test -n "$sim_ac_libzlib_path"; then
    for sim_ac_libzlib_candidate in \
      `( ls $sim_ac_libzlib_path/lib/zlib*.lib;
         ls $sim_ac_libzlib_path/lib/zlib*d.lib ) 2>/dev/null`
    do
      case $sim_ac_libzlib_candidate in
      *d.lib)
        $sim_ac_libzlib_debug &&
          sim_ac_libzlib_name=`basename $sim_ac_libzlib_candidate .lib` ;;
      *.lib)
        sim_ac_libzlib_name=`basename $sim_ac_libzlib_candidate .lib` ;;
      esac
    done
    sim_ac_libzlib_cppflags="-I$sim_ac_libzlib_path/include"
    CPPFLAGS="$CPPFLAGS $sim_ac_libzlib_cppflags"
    sim_ac_libzlib_ldflags="-L$sim_ac_libzlib_path/lib"
    LDFLAGS="$LDFLAGS $sim_ac_libzlib_ldflags"
    # unset sim_ac_libzlib_candidate
    # unset sim_ac_libzlib_path
  fi
  sim_ac_libzlib_libs="-l$sim_ac_libzlib_name"
  LIBS="$sim_ac_libzlib_libs $LIBS"
  AC_TRY_LINK(
    [#include <zlib.h>],
    [(void)zlibVersion();],
    [sim_ac_have_libzlib=true])
  CPPFLAGS=$sim_ac_libzlib_save_CPPFLAGS
  LDFLAGS=$sim_ac_libzlib_save_LDFLAGS
  LIBS=$sim_ac_libzlib_save_LIBS
  # unset sim_ac_libzlib_debug
  # unset sim_ac_libzlib_name
  # unset sim_ac_libzlib_save_CPPFLAGS
  # unset sim_ac_libzlib_save_LDFLAGS
  # unset sim_ac_libzlib_save_LIBS
  ;;
esac
if $sim_ac_want_libzlib; then
  if $sim_ac_have_libzlib; then
    AC_MSG_RESULT([success ($sim_ac_libzlib_libs)])
    $1
  else
    AC_MSG_RESULT([failure])
    $2
  fi
else
  AC_MSG_RESULT([disabled])
  $2
fi
# unset sim_ac_want_libzlib
])

############################################################################
# Usage:
#  SIM_AC_CHECK_ZLIB_READY([ACTION-IF-READY[, ACTION-IF-NOT-READY]])
#
#  Try to link code which needs the ZLIB development system.
#
# Author: Morten Eriksen, <mortene@sim.no>.

AC_DEFUN([SIM_AC_CHECK_ZLIB_READY], [
AC_MSG_CHECKING([if we can use zlib without explicit linkage])
sim_ac_zlib_ready=false
AC_TRY_LINK(
  [#include <zlib.h>],
  [(void)zlibVersion();],
  [sim_ac_zlib_ready=true])
if $sim_ac_zlib_ready; then
  AC_MSG_RESULT([yes])
  $1
else
  AC_MSG_RESULT([no])
  $2
fi
# unset sim_ac_zlib_ready
])

# EOF **********************************************************************
 **************************************************************************
# gendsp.m4
#
# macros:
#   SIM_AC_MSVC_DSP_ENABLE_OPTION
#   SIM_AC_MSVC_DSP_SETUP(PROJECT, Project, project, extra-args)
#
# authors:
#   Lars J. Aas <larsa@coin3d.org>

# **************************************************************************
AC_DEFUN([SIM_AC_MSVC_DSP_ENABLE_OPTION], [
AC_ARG_ENABLE([msvcdsp],
  [AC_HELP_STRING([--enable-msvcdsp], [build .dsp, not library])],
  [case $enableval in
  no | false) sim_ac_make_dsp=false ;;
  *)          sim_ac_make_dsp=true ;;
  esac],
  [sim_ac_make_dsp=false])

if $sim_ac_make_dsp; then
  enable_dependency_tracking=no
  enable_libtool_lock=no
fi
]) # SIM_AC_MSVC_DSP_ENABLE_OPTION

# **************************************************************************
AC_DEFUN([SIM_AC_MSVC_DSP_SETUP], [
AC_REQUIRE([SIM_AC_MSVC_DSP_ENABLE_OPTION])
## Microsoft Developer Studio Project files
$1_DSP_LIBDIRS=
$1_DSP_LIBS=
$1_DSP_INCS=
$1_LIB_DSP_DEFS=
$1_DSP_DEFS=

if $sim_ac_make_dsp; then
  SIM_AC_CONFIGURATION_SETTING([$2 build type], [msvc .dsp])

  # -DHAVE_CONFIG_H is set up in $DEFS too late for us to use, and some
  # include directives are usually set up in the Makefile.am files
  for arg in -DHAVE_CONFIG_H $4 $CPPFLAGS $LDFLAGS $LIBS; do
    case $arg in
    -L* )
      libdir=`echo $arg | cut -c3-`
      $1_DSP_LIBDIRS="[$]$1_DSP_LIBDIRS $libdir"
      ;;
    -l* )
      libname=`echo $arg | cut -c3-`
      for libdir in [$]$1_DSP_LIBDIRS; do
        if test -f $libdir/$libname.lib; then
          # lib is not in any standard location - use full path
          libname=`cygpath -w "$libdir/$libname" 2>/dev/null || echo "$libdir/$libname"`
          break
        fi
      done
      if test x"[$]$1_DSP_LIBS" = x""; then
        $1_DSP_LIBS="$libname.lib"
      else
        $1_DSP_LIBS="[$]$1_DSP_LIBS $libname.lib"
      fi
      ;;
    -I* )
      incdir=`echo $arg | cut -c3-`
      incdir=`cygpath -w "$incdir" 2>/dev/null || echo "$incdir"`
      if test x"[$]$1_DSP_INCS" = x""; then
        $1_DSP_INCS="/I \"$incdir\""
      else
        $1_DSP_INCS="[$]$1_DSP_INCS /I \"$incdir\""
      fi
      ;;
    -D$1_DEBUG* | -DNDEBUG )
      # Defines that vary between release/debug configurations can't be
      # set up dynamically in <lib>_DSP_DEFS - they must be static in the
      # gendsp.sh script.  We therefore catch them here so we can ignore
      # checking for them below.
      ;;
    -D*=* | -D* )
      define=`echo $arg | cut -c3-`
      if test x"[$]$1_DSP_DEFS" = x""; then
        $1_DSP_DEFS="/D \"$define\""
      else
        $1_DSP_DEFS="[$]$1_DSP_DEFS /D \"$define\""
      fi
      if (echo $define | grep _MAKE_DLL) >/dev/null 2>&1; then
        :
      else
        if test x"[$]$1_DSP_DEFS" = x""; then
          $1_LIB_DSP_DEFS="/D \"$define\""
        else
          $1_LIB_DSP_DEFS="[$]$1_LIB_DSP_DEFS /D \"$define\""
        fi
      fi
      ;;
    esac
  done

  CC=[$]$3_build_dir/cfg/gendsp.sh
  CXX=[$]$3_build_dir/cfg/gendsp.sh
  CXXLD=[$]$3_build_dir/cfg/gendsp.sh
  # Yes, this is totally bogus stuff, but don't worry about it.  As long
  # as gendsp.sh recognizes it...  20030219 larsa
  CPPFLAGS="$CPPFLAGS -Ddspfile=[$]$3_build_dir/$3[$]$1_MAJOR_VERSION.dsp"
  LDFLAGS="$LDFLAGS -Wl,-Ddspfile=[$]$3_build_dir/$3[$]$1_MAJOR_VERSION.dsp"
  LIBFLAGS="$LIBFLAGS -o $3[$]$1_MAJOR_VERSION.so.0"

  # this can't be set up at the point the libtool script is generated
  mv libtool libtool.bak
  sed -e "s%^CC=\"gcc\"%CC=\"[$]$3_build_dir/cfg/gendsp.sh\"%" \
      -e "s%^CC=\".*/wrapmsvc.exe\"%CC=\"[$]$3_build_dir/cfg/gendsp.sh\"%" \
      <libtool.bak >libtool
  rm -f libtool.bak
  chmod 755 libtool
fi

AC_SUBST([$1_DSP_LIBS])
AC_SUBST([$1_DSP_INCS])
AC_SUBST([$1_DSP_DEFS])
AC_SUBST([$1_LIB_DSP_DEFS])
])

############################################################################
# Usage:
#  SIM_AC_CHECK_X11([ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
#
#  Try to find the X11 development system. If it is found, these
#  shell variables are set:
#
#    $sim_ac_x11_cppflags (extra flags the compiler needs for X11)
#    $sim_ac_x11_ldflags  (extra flags the linker needs for X11)
#    $sim_ac_x11_libs     (link libraries the linker needs for X11)
#
#  The CPPFLAGS, LDFLAGS and LIBS flags will also be modified accordingly.
#  In addition, the variable $sim_ac_x11_avail is set to "yes" if
#  the X11 development system is found.
#
#
# Author: Morten Eriksen, <mortene@sim.no>.

AC_DEFUN([SIM_AC_CHECK_X11], [
AC_REQUIRE([AC_PATH_XTRA])

sim_ac_enable_darwin_x11=true

case $host_os in
  darwin* ) 
    AC_ARG_ENABLE([darwin-x11],
      AC_HELP_STRING([--enable-darwin-x11],
                     [enable X11 on Darwin [[default=--disable-darwin-x11]]]),
      [case "${enableval}" in
        yes | true) sim_ac_enable_darwin_x11=true ;;
        no | false) sim_ac_enable_darwin_x11=false ;;
        *) SIM_AC_ENABLE_ERROR([--enable-darwin-x11]) ;;
      esac],
      [sim_ac_enable_darwin_x11=false])
  ;;
esac

sim_ac_x11_avail=no

if test x"$no_x" != xyes -a x"$sim_ac_enable_darwin_x11" = xtrue; then
  #  *** DEBUG ***
  #  Keep this around, as it can be handy when testing on new systems.
  # echo "X_CFLAGS: $X_CFLAGS"
  # echo "X_PRE_LIBS: $X_PRE_LIBS"
  # echo "X_LIBS: $X_LIBS"
  # echo "X_EXTRA_LIBS: $X_EXTRA_LIBS"
  # echo
  # exit 0

  sim_ac_x11_cppflags="$X_CFLAGS"
  sim_ac_x11_ldflags="$X_LIBS"
  sim_ac_x11_libs="$X_PRE_LIBS -lX11 $X_EXTRA_LIBS"

  sim_ac_save_cppflags=$CPPFLAGS
  sim_ac_save_ldflags=$LDFLAGS
  sim_ac_save_libs=$LIBS

  CPPFLAGS="$CPPFLAGS $sim_ac_x11_cppflags"
  LDFLAGS="$LDFLAGS $sim_ac_x11_ldflags"
  LIBS="$sim_ac_x11_libs $LIBS"

  AC_CACHE_CHECK(
    [whether we can link against X11],
    sim_cv_lib_x11_avail,
    [AC_TRY_LINK([#include <X11/Xlib.h>],
                 [(void)XOpenDisplay(0L);],
                 [sim_cv_lib_x11_avail=yes],
                 [sim_cv_lib_x11_avail=no])])

  if test x"$sim_cv_lib_x11_avail" = x"yes"; then
    sim_ac_x11_avail=yes
    $1
  else
    CPPFLAGS=$sim_ac_save_cppflags
    LDFLAGS=$sim_ac_save_ldflags
    LIBS=$sim_ac_save_libs
    $2
  fi
fi
])

############################################################################
# Usage:
#  SIM_AC_CHECK_X11SHMEM([ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
#
#  Try to find the X11 shared memory extension. If it is found, this
#  shell variable is set:
#
#    $sim_ac_x11shmem_libs   (link libraries the linker needs for X11 Shm)
#
#  The LIBS flag will also be modified accordingly. In addition, the
#  variable $sim_ac_x11shmem_avail is set to "yes" if the X11 shared
#  memory extension is found.
#
#
# Author: Morten Eriksen, <mortene@sim.no>.
#
# TODO:
#    * [mortene:20000122] make sure this work on MSWin (with
#      Cygwin installation)
#

AC_DEFUN([SIM_AC_CHECK_X11SHMEM], [

sim_ac_x11shmem_avail=no
sim_ac_x11shmem_libs="-lXext"
sim_ac_save_libs=$LIBS
LIBS="$sim_ac_x11shmem_libs $LIBS"

AC_CACHE_CHECK(
  [whether the X11 shared memory extension is available],
  sim_cv_lib_x11shmem_avail,
  [AC_TRY_LINK([#include <X11/Xlib.h>
               #include <X11/extensions/XShm.h>],
               [(void)XShmQueryVersion(0L, 0L, 0L, 0L);],
               [sim_cv_lib_x11shmem_avail=yes],
               [sim_cv_lib_x11shmem_avail=no])])

if test x"$sim_cv_lib_x11shmem_avail" = xyes; then
  sim_ac_x11shmem_avail=yes
  $1
else
  LIBS=$sim_ac_save_libs
  $2
fi
])

############################################################################
# Usage:
#  SIM_AC_CHECK_X11MU([ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
#
#  Try to find the X11 miscellaneous utilities extension. If it is
#  found, this shell variable is set:
#
#    $sim_ac_x11mu_libs   (link libraries the linker needs for X11 MU)
#
#  The LIBS flag will also be modified accordingly. In addition, the
#  variable $sim_ac_x11mu_avail is set to "yes" if the X11 miscellaneous
#  utilities extension is found.
#  CPPFLAGS and LDFLAGS might also be modified, if library is found in a
#  non-standard location.
#
# Author: Morten Eriksen, <mortene@sim.no>.
#
# TODO:
#    * [mortene:20000122] make sure this work on MSWin (with
#      Cygwin installation)

AC_DEFUN([SIM_AC_CHECK_X11MU], [

sim_ac_x11mu_avail=no
sim_ac_x11mu_cppflags=""
sim_ac_x11mu_ldflags=""
sim_ac_x11mu_libs="-lXmu"

sim_ac_save_libs=$LIBS
sim_ac_save_cppflags=$CPPFLAGS
sim_ac_save_ldflags=$LDFLAGS

LIBS="$sim_ac_x11mu_libs $LIBS"

AC_CACHE_CHECK(
  [whether the X11 miscellaneous utilities library is available],
  sim_cv_lib_x11mu_avail,
  [AC_TRY_LINK([#include <X11/Xlib.h>
                #include <X11/Xmu/Xmu.h>
                #include <X11/Xmu/StdCmap.h>],
               [(void)XmuAllStandardColormaps(0L);],
               [sim_cv_lib_x11mu_avail=yes],
               [sim_cv_lib_x11mu_avail=maybe])])

if test x"$sim_cv_lib_x11mu_avail" = xyes; then
  sim_ac_x11mu_avail=yes
else
  # On HP-UX, Xmu might be located under /usr/contrib/X11R6/
  mudir=/usr/contrib/X11R6
  if test -d $mudir; then
    sim_ac_x11mu_cppflags="-I$mudir/include"
    sim_ac_x11mu_ldflags="-L$mudir/lib"
    CPPFLAGS="$sim_ac_x11mu_cppflags $CPPFLAGS"
    LDFLAGS="$sim_ac_x11mu_ldflags $LDFLAGS"

    AC_CACHE_CHECK(
      [once more whether the X11 miscellaneous utilities library is available],
      sim_cv_lib_x11mu_contrib_avail,
      [AC_TRY_LINK([#include <X11/Xlib.h>
                    #include <X11/Xmu/Xmu.h>
                    #include <X11/Xmu/StdCmap.h>],
                   [(void)XmuAllStandardColormaps(0L);],
                   [sim_cv_lib_x11mu_contrib_avail=yes],
                   [sim_cv_lib_x11mu_contrib_avail=no])])
    if test x"$sim_cv_lib_x11mu_contrib_avail" = xyes; then
      sim_ac_x11mu_avail=yes
    else
      sim_ac_x11mu_cppflags=""
      sim_ac_x11mu_ldflags=""
    fi
  fi
fi

if test x"$sim_ac_x11mu_avail" = xyes; then
  :
  $1
else
  LIBS=$sim_ac_save_libs
  CPPFLAGS=$sim_ac_save_cppflags
  LDFLAGS=$sim_ac_save_ldflags
  $2
fi
])

############################################################################
# Usage:
#  SIM_AC_CHECK_X11XID([ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
#
#  Try to find the X11 extension device library. Sets this
#  shell variable:
#
#    $sim_ac_x11xid_libs   (link libraries the linker needs for X11 XID)
#
#  The LIBS flag will also be modified accordingly. In addition, the
#  variable $sim_ac_x11xid_avail is set to "yes" if the X11 extension
#  device library is found.
#
# Author: Morten Eriksen, <mortene@sim.no>.
#
# TODO:
#    * [mortene:20000122] make sure this work on MSWin (with
#      Cygwin installation)
#

AC_DEFUN([SIM_AC_CHECK_X11XID], [

sim_ac_x11xid_avail=no
sim_ac_x11xid_libs="-lXi"
sim_ac_save_libs=$LIBS
LIBS="$sim_ac_x11xid_libs $LIBS"

AC_CACHE_CHECK(
  [whether the X11 extension device library is available],
  sim_cv_lib_x11xid_avail,
  [AC_TRY_LINK([#include <X11/extensions/XInput.h>],
               [(void)XOpenDevice(0L, 0);],
               [sim_cv_lib_x11xid_avail=yes],
               [sim_cv_lib_x11xid_avail=no])])

if test x"$sim_cv_lib_x11xid_avail" = x"yes"; then
  sim_ac_x11xid_avail=yes
  $1
else
  LIBS=$sim_ac_save_libs
  $2
fi
])

############################################################################
# Usage:
#  SIM_AC_CHECK_X_INTRINSIC([ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
#
#  Try to find the Xt intrinsic library. Sets this shell variable:
#
#    $sim_ac_xt_libs   (link library the linker needs for X Intrinsic)
#
#  The LIBS flag will also be modified accordingly. In addition, the
#  variable $sim_ac_xt_avail is set to "yes" if the X11 Intrinsic
#  library is found.
#
# Author: Morten Eriksen, <mortene@sim.no>.
#

AC_DEFUN([SIM_AC_CHECK_X_INTRINSIC], [

sim_ac_xt_avail=no
sim_ac_xt_libs="-lXt"
sim_ac_save_libs=$LIBS
LIBS="$sim_ac_xt_libs $LIBS"

AC_CACHE_CHECK(
  [whether the X11 Intrinsic library is available],
  sim_cv_lib_xt_avail,
  [AC_TRY_LINK([#include <X11/Intrinsic.h>],
               [(void)XtVaCreateWidget("", 0L, 0L);],
               [sim_cv_lib_xt_avail=yes],
               [sim_cv_lib_xt_avail=no])])

if test x"$sim_cv_lib_xt_avail" = xyes; then
  sim_ac_xt_avail=yes
  $1
else
  LIBS=$sim_ac_save_libs
  $2
fi
])

############################################################################
# Usage:
#   SIM_AC_CHECK_LIBXPM( [ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND] )
#
# Description:
#   This macro checks for libXpm.
#
# Variables:
#   $sim_ac_xpm_avail      yes | no
#   $sim_ac_xpm_libs       [link-line libraries]
#
# Authors:
#   Lars J. Aas <larsa@sim.no>
#

AC_DEFUN([SIM_AC_CHECK_LIBXPM], [

sim_ac_xpm_avail=no
sim_ac_xpm_libs="-lXpm"

AC_CACHE_CHECK(
  [whether libXpm is available],
  sim_cv_lib_xpm_avail,
  [sim_ac_save_libs=$LIBS
  LIBS="$sim_ac_xpm_libs $LIBS"
  AC_TRY_LINK([#include <X11/xpm.h>],
              [(void)XpmLibraryVersion();],
              [sim_cv_lib_xpm_avail=yes],
              [sim_cv_lib_xpm_avail=no])
  LIBS="$sim_ac_save_libs"])

if test x"$sim_cv_lib_xpm_avail" = x"yes"; then
  sim_ac_xpm_avail=yes
  LIBS="$sim_ac_xpm_libs $LIBS"
  $1
else
  ifelse([$2], , :, [$2])
fi
])


############################################################################
# Usage:
#  SIM_AC_CHECK_X11_XP([ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
#
#  Try to find the Xp library for printing functionality. Sets this
#  shell variable:
#
#    $sim_ac_xp_libs   (link library the linker needs for the Xp library)
#
#  The LIBS flag will also be modified accordingly. In addition, the
#  variable $sim_ac_xp_avail is set to "yes" if the Xp library is found.
#
# Author: Morten Eriksen, <mortene@sim.no>.
#

AC_DEFUN([SIM_AC_CHECK_X11_XP], [
sim_ac_xp_avail=no
sim_ac_xp_libs="-lXp"
sim_ac_save_libs=$LIBS
LIBS="$sim_ac_xp_libs $LIBS"

AC_CACHE_CHECK(
  [whether the X11 printing library is available],
  sim_cv_lib_xp_avail,
  [AC_TRY_LINK([#include <X11/extensions/Print.h>],
               [XpEndJob(0L);],
               [sim_cv_lib_xp_avail=yes],
               [sim_cv_lib_xp_avail=no])])

if test x"$sim_cv_lib_xp_avail" = xyes; then
  sim_ac_xp_avail=yes
  $1
else
  LIBS=$sim_ac_save_libs
  $2
fi
])

############################################################################
# SIM_AC_CHECK_X11_ATHENA( [ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND] )

AC_DEFUN([SIM_AC_CHECK_X11_ATHENA], [
sim_ac_athena_avail=no
sim_ac_athena_libs="-lXaw"
sim_ac_save_libs=$LIBS
LIBS="$sim_ac_athena_libs $LIBS"

AC_CACHE_CHECK(
  [whether the X11 Athena widgets library is available],
  sim_cv_lib_athena_avail,
  [AC_TRY_LINK([#include <X11/Xfuncproto.h>
                #include <X11/Xaw/XawInit.h>],
               [XawInitializeWidgetSet();],
               [sim_cv_lib_athena_avail=yes],
               [sim_cv_lib_athena_avail=no])])

if test x"$sim_cv_lib_athena_avail" = xyes; then
  sim_ac_athena_avail=yes
  $1
else
  LIBS=$sim_ac_save_libs
  $2
fi
])

# SIM_AC_X11_READY( [ACTION-IF-TRUE], [ACTION-IF-FALSE] )

AC_DEFUN([SIM_AC_CHECK_X11_READY],
[AC_CACHE_CHECK(
  [if X11 linkage is ready],
  [sim_cv_x11_ready],
  [AC_TRY_LINK(
    [#include <X11/Xlib.h>],
    [(void)XOpenDisplay(0L);],
    [sim_cv_x11_ready=true],
    [sim_cv_x11_ready=false])])
if ${sim_cv_x11_ready}; then
  ifelse([$1], , :, [$1])
else
  ifelse([$2], , :, [$2])
fi
]) # SIM_AC_X11_READY()
############################################################################
# Usage:
#  SIM_AC_CHECK_PTHREAD([ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
#
#  Try to find the PTHREAD development system. If it is found, these
#  shell variables are set:
#
#    $sim_ac_pthread_cppflags (extra flags the compiler needs for pthread)
#    $sim_ac_pthread_ldflags  (extra flags the linker needs for pthread)
#    $sim_ac_pthread_libs     (link libraries the linker needs for pthread)
#
#  The CPPFLAGS, LDFLAGS and LIBS flags will also be modified accordingly.
#  In addition, the variable $sim_ac_pthread_avail is set to "true" if the
#  pthread development system is found.
#
#
# Author: Morten Eriksen, <mortene@sim.no>.

AC_DEFUN([SIM_AC_CHECK_PTHREAD], [

AC_ARG_WITH(
  [pthread],
  AC_HELP_STRING([--with-pthread=DIR],
                 [pthread installation directory]),
  [],
  [with_pthread=yes])

sim_ac_pthread_avail=no

if test x"$with_pthread" != xno; then
  if test x"$with_pthread" != xyes; then
    sim_ac_pthread_cppflags="-I${with_pthread}/include"
    sim_ac_pthread_ldflags="-L${with_pthread}/lib"
  fi

  # FIXME: should investigate and document the exact meaning of
  # the _REENTRANT flag. larsa's commit message mentions
  # "glibc-doc/FAQ.threads.html". Also, kintel points to the
  # comp.programming.thrads FAQ, which has an entry on the
  # _REENTRANT define.
  #
  # Preferably, it should only be set up when really needed
  # (as detected by some other configure check).
  #
  # 20030306 mortene.
  sim_ac_pthread_cppflags="${sim_ac_pthread_cppflags}"

  sim_ac_save_cppflags=$CPPFLAGS
  sim_ac_save_ldflags=$LDFLAGS
  sim_ac_save_libs=$LIBS

  CPPFLAGS="$CPPFLAGS $sim_ac_pthread_cppflags"
  LDFLAGS="$LDFLAGS $sim_ac_pthread_ldflags"

  sim_ac_pthread_avail=false

  AC_MSG_CHECKING([for POSIX threads])
  # At least under FreeBSD, we link to pthreads library with -pthread.
  for sim_ac_pthreads_libcheck in "-lpthread" "-pthread"; do
    if $sim_ac_pthread_avail; then :; else
      LIBS="$sim_ac_pthreads_libcheck $sim_ac_save_libs"
      AC_TRY_LINK([#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#include <pthread.h>],
                  [(void)pthread_create(0L, 0L, 0L, 0L);],
                  [sim_ac_pthread_avail=true
                   sim_ac_pthread_libs="$sim_ac_pthreads_libcheck"
                  ])
    fi
  done

  if $sim_ac_pthread_avail; then
    AC_MSG_RESULT($sim_ac_pthread_cppflags $sim_ac_pthread_ldflags $sim_ac_pthread_libs)
  else
    AC_MSG_RESULT(not available)
  fi

  if $sim_ac_pthread_avail; then
    AC_CACHE_CHECK(
      [the struct timespec resolution],
      sim_cv_lib_pthread_timespec_resolution,
      [AC_TRY_COMPILE([#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#include <pthread.h>],
                      [struct timespec timeout;
                       timeout.tv_nsec = 0;],
                      [sim_cv_lib_pthread_timespec_resolution=nsecs],
                      [sim_cv_lib_pthread_timespec_resolution=usecs])])
    if test x"$sim_cv_lib_pthread_timespec_resolution" = x"nsecs"; then
      AC_DEFINE([HAVE_PTHREAD_TIMESPEC_NSEC], 1, [define if pthread's struct timespec uses nsecs and not usecs])
    fi
  fi

  if $sim_ac_pthread_avail; then
    ifelse([$1], , :, [$1])
  else
    CPPFLAGS=$sim_ac_save_cppflags
    LDFLAGS=$sim_ac_save_ldflags
    LIBS=$sim_ac_save_libs
    ifelse([$2], , :, [$2])
  fi
fi
]) # SIM_AC_CHECK_PTHREAD
# SIM_AC_DYNLIB_EXT
# --------------------------------------------
# Find out what the shared library suffix is on this platform.
#
# (Consider this a hack -- the "shrext_cmds" variable from Libtool
# is undocumented and not guaranteed to stick around forever. We've
# already had to change this once (it used to be called "shrext")).
#
# Sets the sim_ac_shlibext variable to the extension name.

AC_DEFUN([SIM_AC_DYNLIB_EXT],
[
AC_MSG_CHECKING([for shared library suffix])
eval "sim_ac_shlibext=$shrext_cmds"
AC_MSG_RESULT($sim_ac_shlibext)
if test x"$sim_ac_shlibext" = x""; then
  AC_MSG_WARN([Could not figure out library suffix! (Has there been a change to the Libtool version used?)])
fi
])


# SIM_AC_CHECK_HEADER(HEADER-FILE, [ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND])
# --------------------------------------------------------------------------
# Modified AC_CHECK_HEADER to use AC_TRY_COMPILE instead of AC_TRY_CPP,
# as we can get false positives and/or false negatives when running under
# Cygwin, using the Microsoft Visual C++ compiler (the configure script will
# pick the GCC preprocessor).

AC_DEFUN([SIM_AS_TR_CPP],
[m4_ifdef([AS_TR_CPP],[AS_TR_CPP([$1])],[AC_TR_CPP([$1])])])

AC_DEFUN([SIM_AC_CHECK_HEADER],
[AC_VAR_PUSHDEF([ac_Header], [ac_cv_header_$1])
AC_ARG_VAR([CPPFLAGS], [C/C++ preprocessor flags, e.g. -I<include dir> if you have headers in a nonstandard directory <include dir>])
AC_CACHE_CHECK(
  [for $1],
  ac_Header,
  [AC_TRY_COMPILE([#include <$1>],
    [],
    [AC_VAR_SET([ac_Header], yes)],
    [AC_VAR_SET([ac_Header], no)])])
if test AC_VAR_GET(ac_Header) = yes; then
  ifelse([$2], , :, [$2])
else
  ifelse([$3], , :, [$3])
fi
AC_VAR_POPDEF([ac_Header])
])# SIM_AC_CHECK_HEADER


# SIM_AC_CHECK_HEADERS(HEADER-FILE...
#                  [ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND])
# ----------------------------------------------------------
AC_DEFUN([SIM_AC_CHECK_HEADERS],
[for ac_header in $1
do
SIM_AC_CHECK_HEADER(
  [$ac_header],
  [AC_DEFINE_UNQUOTED(SIM_AS_TR_CPP(HAVE_$ac_header)) $2],
  [$3])
done
])# SIM_AC_CHECK_HEADERS

#   Use this file to store miscellaneous macros related to checking
#   compiler features.

# Usage:
#   SIM_AC_CC_COMPILER_OPTION(OPTION-TO-TEST, ACTION-IF-TRUE [, ACTION-IF-FALSE])
#   SIM_AC_CXX_COMPILER_OPTION(OPTION-TO-TEST, ACTION-IF-TRUE [, ACTION-IF-FALSE])
#
# Description:
#
#   Check whether the current C or C++ compiler can handle a
#   particular command-line option.
#
#
# Author: Morten Eriksen, <mortene@sim.no>.
#
#   * [mortene:19991218] improve macros by catching and analyzing
#     stderr (at least to see if there was any output there)?
#

AC_DEFUN([SIM_AC_COMPILER_OPTION], [
sim_ac_save_cppflags=$CPPFLAGS
CPPFLAGS="$CPPFLAGS $1"
AC_TRY_COMPILE([], [], [sim_ac_accept_result=yes], [sim_ac_accept_result=no])
AC_MSG_RESULT([$sim_ac_accept_result])
CPPFLAGS=$sim_ac_save_cppflags
# This need to go last, in case CPPFLAGS is modified in arg 2 or arg 3.
if test $sim_ac_accept_result = yes; then
  ifelse([$2], , :, [$2])
else
  ifelse([$3], , :, [$3])
fi
])

AC_DEFUN([SIM_AC_COMPILER_BEHAVIOR_OPTION_QUIET], [
sim_ac_save_cppflags=$CPPFLAGS
CPPFLAGS="$CPPFLAGS $1"
AC_TRY_COMPILE([], [$2], [sim_ac_accept_result=yes], [sim_ac_accept_result=no])
CPPFLAGS=$sim_ac_save_cppflags
# This need to go last, in case CPPFLAGS is modified in arg 3 or arg 4.
if test $sim_ac_accept_result = yes; then
  ifelse([$3], , :, [$3])
else
  ifelse([$4], , :, [$4])
fi
])


AC_DEFUN([SIM_AC_CC_COMPILER_OPTION], [
AC_LANG_SAVE
AC_LANG(C)
AC_MSG_CHECKING([whether $CC accepts $1])
SIM_AC_COMPILER_OPTION([$1], [$2], [$3])
AC_LANG_RESTORE
])

AC_DEFUN([SIM_AC_CC_COMPILER_BEHAVIOR_OPTION_QUIET], [
AC_LANG_SAVE
AC_LANG(C)
SIM_AC_COMPILER_BEHAVIOR_OPTION_QUIET([$1], [$2], [$3], [$4])
AC_LANG_RESTORE
])

AC_DEFUN([SIM_AC_CXX_COMPILER_OPTION], [
AC_LANG_SAVE
AC_LANG(C++)
AC_MSG_CHECKING([whether $CXX accepts $1])
SIM_AC_COMPILER_OPTION([$1], [$2], [$3])
AC_LANG_RESTORE
])

AC_DEFUN([SIM_AC_CXX_COMPILER_BEHAVIOR_OPTION_QUIET], [
AC_LANG_SAVE
AC_LANG(C++)
SIM_AC_COMPILER_BEHAVIOR_OPTION_QUIET([$1], [$2], [$3], [$4])
AC_LANG_RESTORE
])


# *******************************************************************
# SIM_AC_RELATIVE_SRC_DIR
#
# Sets $sim_ac_relative_src_dir to the relative path to the source
# directory, and $sim_ac_relative_src_dir_p to true or false depending
# on whether a relative path can be used or not (in case of different
# drives).
#
# Author:
#   Lars J. Aas <larsa@sim.no>


AC_DEFUN([SIM_AC_RELATIVE_SRC_DIR], [

temp_build_dir=`pwd`
temp_src_dir=`cd "$srcdir"; pwd`

temp_up=""
temp_down=""

while test "$temp_build_dir" != "$temp_src_dir"; do
  srclen=`echo "$temp_src_dir" | wc -c`
  buildlen=`echo "$temp_build_dir" | wc -c`
  if test $srclen -gt $buildlen; then
    # cut source tail, insert into temp_up
    temp_src_tail=`echo "$temp_src_dir" | sed -e 's,.*/,,g'`
    temp_src_dir=`echo "$temp_src_dir" | sed -e 's,/[[^/]]*\$,,g'`
    if test x"$temp_up" = "x"; then
      temp_up="$temp_src_tail"
    else
      temp_up="$temp_src_tail/$temp_up"
    fi
  else
    # cut build tail, increase temp_down
    temp_build_dir=`echo "$temp_build_dir" | sed -e 's,/[[^/]]*\$,,g'`
    if test x"$temp_down" = "x"; then
      temp_down=..
    else
      temp_down="../$temp_down"
    fi
  fi
done

if test x"$temp_down" = "x"; then
  if test x"$temp_up" = "x"; then
    sim_ac_relative_src_dir="."
  else
    sim_ac_relative_src_dir="$temp_up"
  fi
else
  if test x"$temp_up" = "x"; then
    sim_ac_relative_src_dir="$temp_down"
  else
    sim_ac_relative_src_dir="$temp_down/$temp_up"
  fi
fi

# this gives false positives on windows, but that's ok for now...
if test -f $sim_ac_relative_src_dir/$ac_unique_file; then
  sim_ac_relative_src_dir_p=true;
else
  sim_ac_relative_src_dir_p=false;
fi

AC_SUBST(ac_unique_file) # useful to have to check the relative path
AC_SUBST(sim_ac_relative_src_dir)
AC_SUBST(sim_ac_relative_src_dir_p)

]) # SIM_AC_RELATIVE_SRC_DIR


dnl @synopsis AC_SET_RELEASEINFO_VERSIONINFO [(VERSION)]
dnl
dnl   default $1 = $VERSION
dnl
dnl check the $VERSION number and cut the two last digit-sequences off
dnl which will form a -version-info in a @VERSIONINFO@ ac_subst while
dnl the rest is going to the -release name in a @RELEASEINFO@ ac_subst.
dnl
dnl you should keep these two seperate - the release-name may contain
dnl alpha-characters and can be modified later with extra release-hints
dnl e.g. RELEASEINFO="$RELEASINFO-debug" for a debug version of your
dnl lib.
dnl
dnl example: a VERSION="2.4.18" will be transformed into "-release 2
dnl -version-info 4:18" and for a linux-target this will tell libtool
dnl to install the lib as "libmy.so libmy.la libmy.a libmy-2.so.4
dnl libmy-2.so.4.0.18" and executables will get link-resolve-infos for
dnl libmy-2.so.4 - therefore the patch-level is ignored during ldso
dnl linking, and ldso will use the one with the highest patchlevel.
dnl Using just "-release $(VERSION)" during libtool-linking would not
dnl do that - omitting the -version-info will libtool install libmy.so
dnl libmy.la libmy.a libmy-2.4.18.so and executables would get
dnl hardlinked with the 2.4.18 version of your lib.
dnl
dnl This background does also explain the default dll name for a win32
dnl target : libtool will choose to make up libmy-2-4.dll for this
dnl version spec.
dnl
dnl this macro does set the three parts
dnl VERSION_REL.VERSION_REQ.VERSION_REL from the VERSION-spec but does
dnl not ac_subst them like the two INFOs. If you prefer a two-part
dnl VERSION-spec, the VERSION_REL will still be set, either to the
dnl host_cpu or just a simple "00". You may add sublevel parts like
dnl "1.4.2-ac5" where the sublevel is just killed from these
dnl versioninfo/releasinfo substvars.
dnl
dnl @category Misc
dnl @author Guido Draheim <guidod@gmx.de>
dnl @version 2001-08-25
dnl @license GPLWithACException

AC_DEFUN([AC_SET_RELEASEINFO_VERSIONINFO],
[# ------ AC SET RELEASEINFO VERSIONINFO --------------------------------
AC_MSG_CHECKING(version info)
  VERSION_REQ=`echo ifelse( $1, , $VERSION, $1 )` # VERSION_TMP really...
  VERSION_REL=`echo $VERSION_REQ | sed -e 's/[[.]][[^.]]*$//'`  # delete micro
  VERSION_REV=`echo $VERSION_REQ | sed -e "s/^$VERSION_REL.//"` # the rest
  VERSION_REQ=`echo $VERSION_REL | sed -e 's/.*[[.]]//'`  # delete prefix now
  VERSION_REV=`echo $VERSION_REV | sed -e 's/[[^0-9]].*//'` # 5-p4 -> 5
  if test "$VERSION_REQ" != "$VERSION_REL" ; then # three-part version...
  VERSION_REL=`echo $VERSION_REL | sed -e "s/.$VERSION_REQ\$//"`
  else # or has been two-part version - try using host_cpu if available
  VERSION_REL="00" ; test "_$host_cpu" != "_" && VERSION_REL="$host_cpu"
  fi
  RELEASEINFO="-release $VERSION_REL"
  VERSIONINFO="-version-info $VERSION_REQ:$VERSION_REV"
AC_MSG_RESULT([$RELEASEINFO $VERSIONINFO])
AC_SUBST([RELEASEINFO])
AC_SUBST([VERSIONINFO])
])
dnl @synopsis VL_LIB_READLINE
dnl
dnl Searches for a readline compatible library. If found, defines
dnl `HAVE_LIBREADLINE'. If the found library has the `add_history'
dnl function, sets also `HAVE_READLINE_HISTORY'. Also checks for the
dnl locations of the necessary include files and sets `HAVE_READLINE_H'
dnl or `HAVE_READLINE_READLINE_H' and `HAVE_READLINE_HISTORY_H' or
dnl 'HAVE_HISTORY_H' if the corresponding include files exists.
dnl
dnl The libraries that may be readline compatible are `libedit',
dnl `libeditline' and `libreadline'. Sometimes we need to link a
dnl termcap library for readline to work, this macro tests these cases
dnl too by trying to link with `libtermcap', `libcurses' or
dnl `libncurses' before giving up.
dnl
dnl Here is an example of how to use the information provided by this
dnl macro to perform the necessary includes or declarations in a C
dnl file:
dnl
dnl   #ifdef HAVE_LIBREADLINE
dnl   #  if defined(HAVE_READLINE_READLINE_H)
dnl   #    include <readline/readline.h>
dnl   #  elif defined(HAVE_READLINE_H)
dnl   #    include <readline.h>
dnl   #  else /* !defined(HAVE_READLINE_H) */
dnl   extern char *readline ();
dnl   #  endif /* !defined(HAVE_READLINE_H) */
dnl   char *cmdline = NULL;
dnl   #else /* !defined(HAVE_READLINE_READLINE_H) */
dnl     /* no readline */
dnl   #endif /* HAVE_LIBREADLINE */
dnl
dnl   #ifdef HAVE_READLINE_HISTORY
dnl   #  if defined(HAVE_READLINE_HISTORY_H)
dnl   #    include <readline/history.h>
dnl   #  elif defined(HAVE_HISTORY_H)
dnl   #    include <history.h>
dnl   #  else /* !defined(HAVE_HISTORY_H) */
dnl   extern void add_history ();
dnl   extern int write_history ();
dnl   extern int read_history ();
dnl   #  endif /* defined(HAVE_READLINE_HISTORY_H) */
dnl     /* no history */
dnl   #endif /* HAVE_READLINE_HISTORY */
dnl
dnl @category InstalledPackages
dnl @author Ville Laurikari <vl@iki.fi>
dnl @version 2002-04-04
dnl @license AllPermissive

AC_DEFUN([VL_LIB_READLINE], [
  AC_CACHE_CHECK([for a readline compatible library],[vl_cv_lib_readline], [
    ORIG_LIBS="$LIBS"
    for readline_lib in readline edit editline; do
      for termcap_lib in "" termcap curses ncurses; do
        if test -z "$termcap_lib"; then
          TRY_LIB="-l$readline_lib"
        else
          TRY_LIB="-l$readline_lib -l$termcap_lib"
        fi
        LIBS="$ORIG_LIBS $TRY_LIB"
        AC_TRY_LINK_FUNC([readline],[vl_cv_lib_readline="$TRY_LIB"])
        if test -n "$vl_cv_lib_readline"; then
          break
        fi
      done
      if test -n "$vl_cv_lib_readline"; then
        break
      fi
    done
    if test -z "$vl_cv_lib_readline"; then
      vl_cv_lib_readline="no"
      LIBS="$ORIG_LIBS"
      SIM_AC_CONFIGURATION_WARNING([Unable to to find readline, edit, editline, or termcap, curses ncurses. Readline disabled])
    fi
  ])
  if test "$vl_cv_lib_readline" != "no"; then
    AC_DEFINE([TA_USE_READLINE],[1],[Define if you have a readline compatible library])
    AC_CHECK_HEADERS([readline.h readline/readline.h])
#    LIBS="$LIBS $vl_cv_lib_readline"
    SIM_AC_CONFIGURATION_SETTING([Readline],[$vl_cv_lib_readline])
  fi
])dnl

##### http://autoconf-archive.cryp.to/ac_define_dir.html
#
# SYNOPSIS
#
#   AC_DEFINE_DIR(VARNAME, DIR [, DESCRIPTION])
#
# DESCRIPTION
#
#   This macro sets VARNAME to the expansion of the DIR variable,
#   taking care of fixing up ${prefix} and such.
#
#   VARNAME is then offered as both an output variable and a C
#   preprocessor symbol.
#
#   Example:
#
#      AC_DEFINE_DIR([DATADIR], [datadir], [Where data are placed to.])
#
# LAST MODIFICATION
#
#   2006-10-13
#
# COPYLEFT
#
#   Copyright (c) 2006 Stepan Kasal <kasal@ucw.cz>
#   Copyright (c) 2006 Andreas Schwab <schwab@suse.de>
#   Copyright (c) 2006 Guido U. Draheim <guidod@gmx.de>
#   Copyright (c) 2006 Alexandre Oliva
#
#   Copying and distribution of this file, with or without
#   modification, are permitted in any medium without royalty provided
#   the copyright notice and this notice are preserved.

AC_DEFUN([AC_DEFINE_DIR], [
  prefix_NONE=
  exec_prefix_NONE=
  test "x$prefix" = xNONE && prefix_NONE=yes && prefix=$ac_default_prefix
  test "x$exec_prefix" = xNONE && exec_prefix_NONE=yes && exec_prefix=$prefix
dnl In Autoconf 2.60, ${datadir} refers to ${datarootdir}, which in turn
dnl refers to ${prefix}.  Thus we have to use `eval' twice.
  eval ac_define_dir="\"[$]$2\""
  eval ac_define_dir="\"$ac_define_dir\""
  AC_SUBST($1, "$ac_define_dir")
  AC_DEFINE_UNQUOTED($1, "$ac_define_dir", [$3])
  test "$prefix_NONE" && prefix=NONE
  test "$exec_prefix_NONE" && exec_prefix=NONE
])

##### http://autoconf-archive.cryp.to/bnv_have_qt.html
#
# SYNOPSIS
#
#   BNV_HAVE_QT [--with-Qt-dir=DIR] [--with-Qt-lib-dir=DIR] [--with-Qt-lib=LIB]
#   BNV_HAVE_QT [--with-Qt-include-dir=DIR] [--with-Qt-bin-dir=DIR] [--with-Qt-lib-dir=DIR] [--with-Qt-lib=LIB]
#
# DESCRIPTION
#
#   Searches common directories for Qt include files, libraries and Qt
#   binary utilities. The macro supports several different versions of
#   the Qt framework being installed on the same machine. Without
#   options, the macro is designed to look for the latest library,
#   i.e., the highest definition of QT_VERSION in qglobal.h. By use of
#   one or more options a different library may be selected. There are
#   two different sets of options. Both sets contain the option
#   --with-Qt-lib=LIB which can be used to force the use of a
#   particular version of the library file when more than one are
#   available. LIB must be in the form as it would appear behind the
#   "-l" option to the compiler. Examples for LIB would be "qt-mt" for
#   the multi-threaded version and "qt" for the regular version. In
#   addition to this, the first set consists of an option
#   --with-Qt-dir=DIR which can be used when the installation conforms
#   to Trolltech's standard installation, which means that header files
#   are in DIR/include, binary utilities are in DIR/bin and the library
#   is in DIR/lib. The second set of options can be used to indicate
#   individual locations for the header files, the binary utilities and
#   the library file, in addition to the specific version of the
#   library file.
#
#   The following shell variable is set to either "yes" or "no":
#
#     have_qt
#
#   Additionally, the following variables are exported:
#
#     QT_CXXFLAGS
#     QT_LIBS
#     QT_MOC
#     QT_UIC
#     QT_DIR
#
#   which respectively contain an "-I" flag pointing to the Qt include
#   directory (and "-DQT_THREAD_SUPPORT" when LIB is "qt-mt"), link
#   flags necessary to link with Qt and X, the name of the meta object
#   compiler and the user interface compiler both with full path, and
#   finaly the variable QTDIR as Trolltech likes to see it defined (if
#   possible).
#
#   Example lines for Makefile.in:
#
#     CXXFLAGS = @QT_CXXFLAGS@
#     MOC      = @QT_MOC@
#
#   After the variables have been set, a trial compile and link is
#   performed to check the correct functioning of the meta object
#   compiler. This test may fail when the different detected elements
#   stem from different releases of the Qt framework. In that case, an
#   error message is emitted and configure stops.
#
#   No common variables such as $LIBS or $CFLAGS are polluted.
#
#   Options:
#
#   --with-Qt-dir=DIR: DIR is equal to $QTDIR if you have followed the
#   installation instructions of Trolltech. Header files are in
#   DIR/include, binary utilities are in DIR/bin and the library is in
#   DIR/lib.
#
#   --with-Qt-include-dir=DIR: Qt header files are in DIR.
#
#   --with-Qt-bin-dir=DIR: Qt utilities such as moc and uic are in DIR.
#
#   --with-Qt-lib-dir=DIR: The Qt library is in DIR.
#
#   --with-Qt-lib=LIB: Use -lLIB to link with the Qt library.
#
#   If some option "=no" or, equivalently, a --without-Qt-* version is
#   given in stead of a --with-Qt-*, "have_qt" is set to "no" and the
#   other variables are set to the empty string.
#
# LAST MODIFICATION
#
#   2006-03-12
#
# COPYLEFT
#
#   Copyright (c) 2006 Bastiaan Veelo <Bastiaan@Veelo.net>
#
#   Copying and distribution of this file, with or without
#   modification, are permitted in any medium without royalty provided
#   the copyright notice and this notice are preserved.

dnl Copyright (C) 2001, 2002, 2003, 2005, 2006 Bastiaan Veelo

dnl THANKS! This code includes bug fixes and contributions made by:
dnl Tim McClarren,
dnl Dennis R. Weilert,
dnl Qingning Huo,
dnl Brian Mingus,
dnl Jens Hannemann,
dnl Pavel Roskin,
dnl Scott J. Bertin.

dnl ChangeLog
dnl 2006-03-12  * Hide output of ls and fix an m4 quoting problem (due to Scott J. Bertin).
dnl 2006-02-13  * Check compiler return value instead of parsing the error stream,
dnl               which detected warnings as false negatives (due to Jens Hannemann).
dnl 2006-02-02  * Spelling of "Success".
dnl             * Fixed unsave test for $bnv_qt_lib without quotes.
dnl             * Put dnl in front of all comments.
dnl             * Changed -l$bnv_qt_lib_dir into -L$bnv_qt_lib_dir (all due to Pavel Roskin).
dnl 2006-01-19  * Support for 64bit architectures.
dnl             * Updated documentation.
dnl 2006-01-18: * Fix "cat: bnv_qt_test.c: No such file or directory" (due to Jens Hannemann).
dnl             * Hide output of failing ls.
dnl 2006-01-11: * Check in /Developer on Mac OS X; Check in $QTDIR (due to Brian Mingus).

dnl Calls BNV_PATH_QT_DIRECT (contained in this file) as a subroutine.
AC_DEFUN([BNV_HAVE_QT],
[
  AC_REQUIRE([AC_PROG_CXX])
  AC_REQUIRE([AC_PATH_X])
  AC_REQUIRE([AC_PATH_XTRA])

  AC_MSG_CHECKING(for Qt)

  AC_ARG_WITH([Qt-dir],
    [  --with-Qt-dir=DIR       DIR is equal to $QTDIR if you have followed the
                          installation instructions of Trolltech. Header
                          files are in DIR/include, binary utilities are
                          in DIR/bin. The library is in DIR/lib, unless
			  --with-Qt-lib-dir is also set.])
  AC_ARG_WITH([Qt-include-dir],
    [  --with-Qt-include-dir=DIR
                          Qt header files are in DIR])
  AC_ARG_WITH([Qt-bin-dir],
    [  --with-Qt-bin-dir=DIR   Qt utilities such as moc and uic are in DIR])
  AC_ARG_WITH([Qt-lib-dir],
    [  --with-Qt-lib-dir=DIR   The Qt library is in DIR])
  AC_ARG_WITH([Qt-lib],
    [  --with-Qt-lib=LIB       Use -lLIB to link with the Qt library])
  if test x"$with_Qt_dir" = x"no" ||
     test x"$with_Qt_include-dir" = x"no" ||
     test x"$with_Qt_bin_dir" = x"no" ||
     test x"$with_Qt_lib_dir" = x"no" ||
     test x"$with_Qt_lib" = x"no"; then
    # user disabled Qt. Leave cache alone.
    have_qt="User disabled Qt."
  else
    # "yes" is a bogus option
    if test x"$with_Qt_dir" = xyes; then
      with_Qt_dir=
    fi
    if test x"$with_Qt_include_dir" = xyes; then
      with_Qt_include_dir=
    fi
    if test x"$with_Qt_bin_dir" = xyes; then
      with_Qt_bin_dir=
    fi
    if test x"$with_Qt_lib_dir" = xyes; then
      with_Qt_lib_dir=
    fi
    if test x"$with_Qt_lib" = xyes; then
      with_Qt_lib=
    fi
    # No Qt unless we discover otherwise
    have_qt=no
    # Check whether we are requested to link with a specific version
    if test x"$with_Qt_lib" != x; then
      bnv_qt_lib="$with_Qt_lib"
    fi
    # Check whether we were supplied with an answer already
    if test x"$with_Qt_dir" != x; then
      have_qt=yes
      bnv_qt_dir="$with_Qt_dir"
      bnv_qt_include_dir="$with_Qt_dir/include"
      bnv_qt_bin_dir="$with_Qt_dir/bin"
      bnv_qt_lib_dir="$with_Qt_dir/lib"
      # Only search for the lib if the user did not define one already
      if test x"$bnv_qt_lib" = x; then
        bnv_qt_lib="`ls $bnv_qt_lib_dir/libqt* | sed -n 1p |
                     sed s@$bnv_qt_lib_dir/lib@@ | [sed s@[.].*@@]`"
      fi
      bnv_qt_LIBS="-L$bnv_qt_lib_dir -l$bnv_qt_lib $X_PRE_LIBS $X_LIBS -lX11 -lXext -lXmu -lXt -lXi $X_EXTRA_LIBS"
    else
      # Use cached value or do search, starting with suggestions from
      # the command line
      AC_CACHE_VAL(bnv_cv_have_qt,
      [
        # We are not given a solution and there is no cached value.
        bnv_qt_dir=NO
        bnv_qt_include_dir=NO
        bnv_qt_lib_dir=NO
        if test x"$bnv_qt_lib" = x; then
          bnv_qt_lib=NO
        fi
        BNV_PATH_QT_DIRECT
        if test "$bnv_qt_dir" = NO ||
           test "$bnv_qt_include_dir" = NO ||
           test "$bnv_qt_lib_dir" = NO ||
           test "$bnv_qt_lib" = NO; then
          # Problem with finding complete Qt.  Cache the known absence of Qt.
          bnv_cv_have_qt="have_qt=no"
        else
          # Record where we found Qt for the cache.
          bnv_cv_have_qt="have_qt=yes                  \
                       bnv_qt_dir=$bnv_qt_dir          \
               bnv_qt_include_dir=$bnv_qt_include_dir  \
                   bnv_qt_bin_dir=$bnv_qt_bin_dir      \
                      bnv_qt_LIBS=\"$bnv_qt_LIBS\""
        fi
      ])dnl
      eval "$bnv_cv_have_qt"
    fi # all $bnv_qt_* are set
  fi   # $have_qt reflects the system status
  if test x"$have_qt" = xyes; then
    QT_CXXFLAGS="-I$bnv_qt_include_dir"
    if test x"$bnv_qt_lib" = xqt-mt; then
        QT_CXXFLAGS="$QT_CXXFLAGS -DQT_THREAD_SUPPORT"
    fi
    QT_DIR="$bnv_qt_dir"
    QT_LIBS="$bnv_qt_LIBS"
    # If bnv_qt_dir is defined, utilities are expected to be in the
    # bin subdirectory
    if test x"$bnv_qt_dir" != x; then
        if test -x "$bnv_qt_dir/bin/uic"; then
          QT_UIC="$bnv_qt_dir/bin/uic"
        else
          # Old versions of Qt don't have uic
          QT_UIC=
        fi
      QT_MOC="$bnv_qt_dir/bin/moc"
    else
      # Or maybe we are told where to look for the utilities
      if test x"$bnv_qt_bin_dir" != x; then
        if test -x "$bnv_qt_bin_dir/uic"; then
          QT_UIC="$bnv_qt_bin_dir/uic"
        else
          # Old versions of Qt don't have uic
          QT_UIC=
        fi
        QT_MOC="$bnv_qt_bin_dir/moc"
      else
      # Last possibility is that they are in $PATH
        QT_UIC="`which uic`"
        QT_MOC="`which moc`"
      fi
    fi
    # All variables are defined, report the result
    AC_MSG_RESULT([$have_qt:
    QT_CXXFLAGS=$QT_CXXFLAGS
    QT_DIR=$QT_DIR
    QT_LIBS=$QT_LIBS
    QT_UIC=$QT_UIC
    QT_MOC=$QT_MOC])
  else
    # Qt was not found
    QT_CXXFLAGS=
    QT_DIR=
    QT_LIBS=
    QT_UIC=
    QT_MOC=
    AC_MSG_RESULT($have_qt)
  fi
  AC_SUBST(QT_CXXFLAGS)
  AC_SUBST(QT_DIR)
  AC_SUBST(QT_LIBS)
  AC_SUBST(QT_UIC)
  AC_SUBST(QT_MOC)

  #### Being paranoid:
  if test x"$have_qt" = xyes; then
    AC_MSG_CHECKING(correct functioning of Qt installation)
    AC_CACHE_VAL(bnv_cv_qt_test_result,
    [
      cat > bnv_qt_test.h << EOF
#include <qobject.h>
class Test : public QObject
{
Q_OBJECT
public:
  Test() {}
  ~Test() {}
public slots:
  void receive() {}
signals:
  void send();
};
EOF

      cat > bnv_qt_main.$ac_ext << EOF
#include "bnv_qt_test.h"
#include <qapplication.h>
int main( int argc, char **argv )
{
  QApplication app( argc, argv );
  Test t;
  QObject::connect( &t, SIGNAL(send()), &t, SLOT(receive()) );
}
EOF

      bnv_cv_qt_test_result="failure"
      bnv_try_1="$QT_MOC bnv_qt_test.h -o moc_bnv_qt_test.$ac_ext >/dev/null 2>/dev/null"
      AC_TRY_EVAL(bnv_try_1)
      if test x"$ac_status" != x0; then
        echo "$bnv_err_1" >&AC_FD_CC
        echo "configure: could not run $QT_MOC on:" >&AC_FD_CC
        cat bnv_qt_test.h >&AC_FD_CC
      else
        bnv_try_2="$CXX $QT_CXXFLAGS -c $CXXFLAGS -o moc_bnv_qt_test.o moc_bnv_qt_test.$ac_ext >/dev/null 2>/dev/null"
        AC_TRY_EVAL(bnv_try_2)
        if test x"$ac_status" != x0; then
          echo "$bnv_err_2" >&AC_FD_CC
          echo "configure: could not compile:" >&AC_FD_CC
          cat moc_bnv_qt_test.$ac_ext >&AC_FD_CC
        else
          bnv_try_3="$CXX $QT_CXXFLAGS -c $CXXFLAGS -o bnv_qt_main.o bnv_qt_main.$ac_ext >/dev/null 2>/dev/null"
          AC_TRY_EVAL(bnv_try_3)
          if test x"$ac_status" != x0; then
            echo "$bnv_err_3" >&AC_FD_CC
            echo "configure: could not compile:" >&AC_FD_CC
            cat bnv_qt_main.$ac_ext >&AC_FD_CC
          else
            bnv_try_4="$CXX $QT_LIBS $LIBS -o bnv_qt_main bnv_qt_main.o moc_bnv_qt_test.o >/dev/null 2>/dev/null"
            AC_TRY_EVAL(bnv_try_4)
            if test x"$ac_status" != x0; then
              echo "$bnv_err_4" >&AC_FD_CC
            else
              bnv_cv_qt_test_result="success"
            fi
          fi
        fi
      fi
    ])dnl AC_CACHE_VAL bnv_cv_qt_test_result
    AC_MSG_RESULT([$bnv_cv_qt_test_result]);
    if test x"$bnv_cv_qt_test_result" = "xfailure"; then
      AC_MSG_ERROR([Failed to find matching components of a complete
                  Qt installation. Try using more options,
                  see ./configure --help.])
    fi

    rm -f bnv_qt_test.h moc_bnv_qt_test.$ac_ext moc_bnv_qt_test.o \
          bnv_qt_main.$ac_ext bnv_qt_main.o bnv_qt_main
  fi
])

dnl Internal subroutine of BNV_HAVE_QT
dnl Set bnv_qt_dir bnv_qt_include_dir bnv_qt_bin_dir bnv_qt_lib_dir bnv_qt_lib
AC_DEFUN([BNV_PATH_QT_DIRECT],
[
  ## Binary utilities ##
  if test x"$with_Qt_bin_dir" != x; then
    bnv_qt_bin_dir=$with_Qt_bin_dir
  fi
  ## Look for header files ##
  if test x"$with_Qt_include_dir" != x; then
    bnv_qt_include_dir="$with_Qt_include_dir"
  else
    # The following header file is expected to define QT_VERSION.
    qt_direct_test_header=qglobal.h
    # Look for the header file in a standard set of common directories.
    bnv_include_path_list="
      /usr/include
      `ls -dr ${QTDIR}/include/QtCore 2>/dev/null`
      `ls -dr /usr/include/qt* 2>/dev/null`
      `ls -dr /usr/lib/qt*/include 2>/dev/null`
      `ls -dr /usr/local/qt*/include 2>/dev/null`
      `ls -dr /opt/qt*/include 2>/dev/null`
      `ls -dr /Developer/qt*/include 2>/dev/null`
    "
    for bnv_dir in $bnv_include_path_list; do
      if test -r "$bnv_dir/$qt_direct_test_header"; then
        bnv_dirs="$bnv_dirs $bnv_dir"
      fi
    done
    # Now look for the newest in this list
    bnv_prev_ver=0
    for bnv_dir in $bnv_dirs; do
      bnv_this_ver=`egrep -w '#define QT_VERSION' $bnv_dir/$qt_direct_test_header | sed s/'#define QT_VERSION'//`
      if expr $bnv_this_ver '>' $bnv_prev_ver > /dev/null; then
        bnv_qt_include_dir=$bnv_dir
        bnv_prev_ver=$bnv_this_ver
      fi
    done
  fi dnl Found header files.

  # Are these headers located in a traditional Trolltech installation?
  # That would be $bnv_qt_include_dir stripped from its last element:
  bnv_possible_qt_dir=`dirname $bnv_qt_include_dir`
  if (test -x $bnv_possible_qt_dir/bin/moc) &&
     ((ls $bnv_possible_qt_dir/lib/libqt* > /dev/null 2>/dev/null) ||
      (ls $bnv_possible_qt_dir/lib64/libqt* > /dev/null 2>/dev/null)); then
    # Then the rest is a piece of cake
    bnv_qt_dir=$bnv_possible_qt_dir
    bnv_qt_bin_dir="$bnv_qt_dir/bin"
    if test x"$with_Qt_lib_dir" != x; then
      bnv_qt_lib_dir="$with_Qt_lib_dir"
    else
      if (test -d $bnv_qt_dir/lib64); then
	bnv_qt_lib_dir="$bnv_qt_dir/lib64"
      else
	bnv_qt_lib_dir="$bnv_qt_dir/lib"
      fi
    fi
    # Only look for lib if the user did not supply it already
    if test x"$bnv_qt_lib" = xNO; then
      bnv_qt_lib="`ls $bnv_qt_lib_dir/libqt* | sed -n 1p |
                   sed s@$bnv_qt_lib_dir/lib@@ | [sed s@[.].*@@]`"
    fi
    bnv_qt_LIBS="-L$bnv_qt_lib_dir -l$bnv_qt_lib $X_PRE_LIBS $X_LIBS -lX11 -lXext -lXmu -lXt -lXi $X_EXTRA_LIBS"
  else
    # There is no valid definition for $QTDIR as Trolltech likes to see it
    bnv_qt_dir=
    ## Look for Qt library ##
    if test x"$with_Qt_lib_dir" != x; then
      bnv_qt_lib_dir="$with_Qt_lib_dir"
      # Only look for lib if the user did not supply it already
      if test x"$bnv_qt_lib" = xNO; then
        bnv_qt_lib="`ls $bnv_qt_lib_dir/libqt* | sed -n 1p |
                     sed s@$bnv_qt_lib_dir/lib@@ | [sed s@[.].*@@]`"
      fi
      bnv_qt_LIBS="-L$bnv_qt_lib_dir -l$bnv_qt_lib $X_PRE_LIBS $X_LIBS -lX11 -lXext -lXmu -lXt -lXi $X_EXTRA_LIBS"
    else
      # Normally, when there is no traditional Trolltech installation,
      # the library is installed in a place where the linker finds it
      # automatically.
      # If the user did not define the library name, try with qt
      if test x"$bnv_qt_lib" = xNO; then
        bnv_qt_lib=qt
      fi
      qt_direct_test_header=qapplication.h
      qt_direct_test_main="
        int argc;
        char ** argv;
        QApplication app(argc,argv);
      "
      # See if we find the library without any special options.
      # Don't add top $LIBS permanently yet
      bnv_save_LIBS="$LIBS"
      LIBS="-l$bnv_qt_lib $X_PRE_LIBS $X_LIBS -lX11 -lXext -lXmu -lXt -lXi $X_EXTRA_LIBS"
      bnv_qt_LIBS="$LIBS"
      bnv_save_CXXFLAGS="$CXXFLAGS"
      CXXFLAGS="-I$bnv_qt_include_dir"
      AC_TRY_LINK([#include <$qt_direct_test_header>],
        $qt_direct_test_main,
      [
        # Success.
        # We can link with no special library directory.
        bnv_qt_lib_dir=
      ], [
        # That did not work. Try the multi-threaded version
        echo "Non-critical error, please neglect the above." >&AC_FD_CC
        bnv_qt_lib=qt-mt
        LIBS="-l$bnv_qt_lib $X_PRE_LIBS $X_LIBS -lX11 -lXext -lXmu -lXt -lXi $X_EXTRA_LIBS"
        AC_TRY_LINK([#include <$qt_direct_test_header>],
          $qt_direct_test_main,
        [
          # Success.
          # We can link with no special library directory.
          bnv_qt_lib_dir=
        ], [
          # That did not work. Try the OpenGL version
          echo "Non-critical error, please neglect the above." >&AC_FD_CC
          bnv_qt_lib=qt-gl
          LIBS="-l$bnv_qt_lib $X_PRE_LIBS $X_LIBS -lX11 -lXext -lXmu -lXt -lXi $X_EXTRA_LIBS"
          AC_TRY_LINK([#include <$qt_direct_test_header>],
            $qt_direct_test_main,
          [
            # Success.
            # We can link with no special library directory.
            bnv_qt_lib_dir=
          ], [
            # That did not work. Maybe a library version I don't know about?
            echo "Non-critical error, please neglect the above." >&AC_FD_CC
            # Look for some Qt lib in a standard set of common directories.
            bnv_dir_list="
              `echo $bnv_qt_includes | sed ss/includess`
              /lib
	      /usr/lib64
              /usr/lib
	      /usr/local/lib64
              /usr/local/lib
	      /opt/lib64
              /opt/lib
              `ls -dr /usr/lib64/qt* 2>/dev/null`
              `ls -dr /usr/lib64/qt*/lib64 2>/dev/null`
              `ls -dr /usr/lib/qt* 2>/dev/null`
              `ls -dr /usr/local/qt* 2>/dev/null`
              `ls -dr /opt/qt* 2>/dev/null`
            "
            for bnv_dir in $bnv_dir_list; do
              if ls $bnv_dir/libqt* >/dev/null 2>/dev/null; then
                # Gamble that it's the first one...
                bnv_qt_lib="`ls $bnv_dir/libqt* | sed -n 1p |
                            sed s@$bnv_dir/lib@@ | sed s/[[.]].*//`"
                bnv_qt_lib_dir="$bnv_dir"
                break
              fi
            done
            # Try with that one
            LIBS="-l$bnv_qt_lib $X_PRE_LIBS $X_LIBS -lX11 -lXext -lXmu -lXt -lXi $X_EXTRA_LIBS"
            AC_TRY_LINK([#include <$qt_direct_test_header>],
              $qt_direct_test_main,
            [
              # Success.
              # We can link with no special library directory.
              bnv_qt_lib_dir=
            ], [
              # Leave bnv_qt_lib_dir defined
            ])
          ])
        ])
      ])
      if test x"$bnv_qt_lib_dir" != x; then
        bnv_qt_LIBS="-L$bnv_qt_lib_dir $LIBS"
      else
        bnv_qt_LIBS="$LIBS"
      fi
      LIBS="$bnv_save_LIBS"
      CXXFLAGS="$bnv_save_CXXFLAGS"
    fi dnl $with_Qt_lib_dir was not given
  fi dnl Done setting up for non-traditional Trolltech installation
])

############################################################################
# Helper macros for the SIM_AC_CHECK_QT macro below.

# SIM_AC_WITH_QT
#
# Sets sim_ac_with_qt (from --with-qt=[true|false]) and
# sim_ac_qtdir (from either --with-qt=DIR or $QTDIR).

AC_DEFUN([SIM_AC_WITH_QT], [
sim_ac_qtdir=
AC_ARG_WITH(
  [qt],
  AC_HELP_STRING([--with-qt=[true|false|DIR]],
                 [specify if Qt should be used, and optionally the location of the Qt library [default=true]]),
  [case $withval in
  no | false ) sim_ac_with_qt=false ;;
  yes | true ) sim_ac_with_qt=true ;;
  *)
    sim_ac_with_qt=true
    sim_ac_qtdir=$withval
    ;;
  esac],
  [sim_ac_with_qt=true])

if $sim_ac_with_qt; then
  if test -z "$sim_ac_qtdir"; then
    # The Cygwin environment needs to invoke moc with a POSIX-style path.
    #AC_PATH_PROG(sim_ac_qt_cygpath, cygpath, false)
    #if test $sim_ac_qt_cygpath = "false"; then
      sim_ac_qtdir=$QTDIR
    #else
      # Quote $QTDIR in case it's empty.
    #  sim_ac_qtdir=`$sim_ac_qt_cygpath -u "$QTDIR"`
    #fi

    AC_MSG_CHECKING([value of the QTDIR environment variable])
    if test x"$sim_ac_qtdir" = x""; then
      AC_MSG_RESULT([empty])
    else
      AC_MSG_RESULT([$sim_ac_qtdir])

      # list contents of what's in the qt dev environment into config.log
      for i in "" bin lib; do
        echo "Listing contents of $sim_ac_qtdir/$i:" >&5
        ls -l $sim_ac_qtdir/$i >&5 2>&1
      done
    fi
  fi
fi
])

# SIM_AC_QT_PROG(VARIABLE, PROG-TO-CHECK-FOR)
#
# Substs VARIABLE to the path of the PROG-TO-CHECK-FOR, if found
# in either $PATH, $QTDIR/bin or the --with-qt=DIR directories.
#
# If not found, VARIABLE will be set to false.

AC_DEFUN([SIM_AC_QT_PROG], [
AC_REQUIRE([SIM_AC_WITH_QT])

if $sim_ac_with_qt; then

  sim_ac_path=$PATH
  if test -n "$sim_ac_qtdir"; then
    sim_ac_path=$sim_ac_qtdir/bin:$PATH
  fi

  AC_PATH_PROG([$1], $2, false, $sim_ac_path)
  if test x"$$1" = x"false"; then
#     if test -z "$QTDIR"; then
#       AC_MSG_WARN([QTDIR environment variable not set -- this might be an indication of a problem])
#     fi
    AC_MSG_WARN([the ``$2'' Qt pre-processor tool not found])
  fi
else
  AC_SUBST([$1], [false])
fi
])

############################################################################
# Usage:
#  SIM_AC_QT_VERSION
#
# Find version number of the Qt library. sim_ac_qt_version will contain
# the full version number string, and sim_ac_qt_major_version will contain
# only the major version number.

AC_DEFUN([SIM_AC_QT_VERSION], [

AC_MSG_CHECKING([version of Qt library])

cat > conftest.c << EOF
#include <qglobal.h>
int VerQt = QT_VERSION;
EOF

# The " *"-parts of the last sed-expression on the next line are necessary
# because at least the Solaris/CC preprocessor adds extra spaces before and
# after the trailing semicolon.
#AC_MSG_WARN([CXXCPP: $CXXCPP])
#AC_MSG_WARN([CPPFLAGS: $CPPFLAGS])
#AC_MSG_WARN([$CXXCPP -nostdinc $(echo $CPPFLAGS | sed 's/\/ -I/ -I/g') conftest.c 2>/dev/null | grep '^int VerQt' | sed 's%^int VerQt = %%' | sed 's% *;.*$%%'])

# NOTE: -nostdinc was used for MinGW. But the standard compiler paths
# shouldn't be needed for this check anyway, anywhere, so i'll leave
# it -- Brian 5/26/07

# NOTE: MinGW also chokes on trailing newlines.
case $host in
    *-*-mingw32*)
            sim_ac_qt_version=`$CXXCPP -nostdinc $(echo $CPPFLAGS | sed 's/\/ -I/ -I/g;s/\/$//') conftest.c 2>/dev/null | grep '^int VerQt' | sed 's%^int VerQt = %%' | sed 's% *;.*$%%'`
            CXXFLAGS="$(echo $CXXFLAGS | sed 's/\/ -I/ -I/g;s/\/$//')"
    ;;
    *)
            sim_ac_qt_version=`$CXXCPP $CPPFLAGS conftest.c 2>/dev/null | grep '^int VerQt' | sed 's%^int VerQt = %%' | sed 's% *;.*$%%'`
    ;;
esac

case $sim_ac_qt_version in
0x* )
  sim_ac_qt_version=`echo $sim_ac_qt_version | sed -e 's/^0x.\(.\).\(.\).\(.\)/\1\2\3/;'`
  ;;
* )
  # nada
  ;;
esac
sim_ac_qt_major_version=`echo $sim_ac_qt_version | cut -c1`

rm -f conftest.c
AC_MSG_RESULT($sim_ac_qt_version)
])

############################################################################
# Usage:
#  SIM_AC_CHECK_QT([ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
#
#  Try to find the Qt development system. If it is found, these
#  shell variables are set:
#
#    $sim_ac_qt_cppflags (extra flags the compiler needs for Qt lib)
#    $sim_ac_qt_ldflags  (extra flags the linker needs for Qt lib)
#    $sim_ac_qt_libs     (link libraries the linker needs for Qt lib)
#
#  The CPPFLAGS, LDFLAGS and LIBS flags will also be modified accordingly.
#  In addition, the variable $sim_ac_qt_avail is set to "yes" if
#  the Qt development system is found.
#
# Authors:
#   Morten Eriksen <mortene@sim.no>.
#   Lars J. Aas <larsa@sim.no>.

AC_DEFUN([SIM_AC_CHECK_QT], [

AC_REQUIRE([SIM_AC_WITH_QT])

sim_ac_qt_debug=false

sim_ac_qt_avail=no

if $sim_ac_with_qt; then

  sim_ac_save_cppflags=$CPPFLAGS
  sim_ac_save_ldflags=$LDFLAGS
  sim_ac_save_libs=$LIBS

  sim_ac_qt_libs=UNRESOLVED

  # Check for Mac OS framework installation
  if test -z "$QTDIR"; then
    sim_ac_qt_framework_dir=/Library/Frameworks
    # FIXME: Should we also look for the Qt framework in other  
    # default framework locations (such as ~/Library/Frameworks)?
    # Or require the user to specify this explicitly, e.g. by
    # passing --with-qt-framework=xxx? 20050802 kyrah.
  else
    sim_ac_qt_framework_dir=$sim_ac_qtdir/lib
  fi

  SIM_AC_HAVE_QT_FRAMEWORK

  if $sim_ac_have_qt_framework; then 
    sim_ac_qt_cppflags="-I$sim_ac_qt_framework_dir/QtCore.framework/Headers -I$sim_ac_qt_framework_dir/QtOpenGL.framework/Headers -I$sim_ac_qt_framework_dir/QtGui.framework/Headers -I$sim_ac_qt_framework_dir/Qt3Support.framework/Headers -F$sim_ac_qt_framework_dir"
    sim_ac_qt_libs="-Wl,-F$sim_ac_qt_framework_dir -Wl,-framework,QtGui -Wl,-framework,QtOpenGL -Wl,-framework,QtCore -Wl,-framework,Qt3Support -Wl,-framework,QtXml -Wl,-framework,QtNetwork -Wl,-framework,QtSql"
  else

    sim_ac_qglobal_unresolved=true

    sim_ac_QTDIR_cppflags=
    sim_ac_QTDIR_ldflags=
    if test -n "$sim_ac_qtdir"; then
      sim_ac_QTDIR_cppflags="-I$sim_ac_qtdir/include -I$sim_ac_qtdir/include/Qt/ -I$sim_ac_qtdir/include/Qt3Support/ -I$sim_ac_qtdir/include/ -I$sim_ac_qtdir/include/QtCore/ -I$sim_ac_qtdir/include/QtGui/ -I$sim_ac_qtdir/include/QtNetwork/ -I$sim_ac_qtdir/include/QtOpenGL/ -I$sim_ac_qtdir/include/QtSql/ -I$sim_ac_qtdir/include/QtXml/"
      sim_ac_QTDIR_ldflags="-L$sim_ac_qtdir/lib"
      CPPFLAGS="$sim_ac_QTDIR_cppflags $CPPFLAGS"
      LDFLAGS="$LDFLAGS $sim_ac_QTDIR_ldflags"
    fi

    # This should take care of detecting qglobal.h for Qt 3 if QTDIR was set,
    # or if Qt is in default locations, or if it is in any additional "-I..."
    # set up by the user with CPPFLAGS:
    sim_ac_temp="$CPPFLAGS"
    SIM_AC_CHECK_HEADER_SILENT([qglobal.h], [sim_ac_qglobal_unresolved=false])

    # If we're to use Qt 4 and QTDIR is set, this should be able to find it:
    if $sim_ac_qglobal_unresolved; then
      SIM_AC_CHECK_HEADER_SILENT([Qt/qglobal.h],
                                 [sim_ac_qglobal_unresolved=false])
      if $sim_ac_qglobal_unresolved; then
        :
      else
        if test -z "$sim_ac_QTDIR_cppflags"; then
          AC_MSG_ERROR([Qt/qglobal.h detected, but no QTDIR environment variable set. Do not know where the Qt include files are.])
        else
          sim_ac_QTDIR_cppflags="$sim_ac_QTDIR_cppflags $sim_ac_QTDIR_cppflags/Qt"
        fi
      fi
    fi
    CPPFLAGS="$sim_ac_temp"

    # Check for known default locations of Qt installation on various
    # systems:
    if $sim_ac_qglobal_unresolved; then
      sim_ac_temp="$CPPFLAGS"
      # /usr/include/qt: Debian-installations with Qt 3
      # /usr/include/qt4: Debian-installations with Qt 4
      # /sw/include/qt: Mac OS X Fink
      for i in "/usr/include/qt" "/usr/include/qt4" "/sw/include/qt"; do
        if $sim_ac_qglobal_unresolved; then
          CPPFLAGS="-I$i $sim_ac_temp"
          SIM_AC_CHECK_HEADER_SILENT([qglobal.h],
                                     [sim_ac_qglobal_unresolved=false
                                      sim_ac_QTDIR_cppflags="-I$i"
                                     ])
          if $sim_ac_qglobal_unresolved; then
            SIM_AC_CHECK_HEADER_SILENT([Qt/qglobal.h],
                                       [sim_ac_qglobal_unresolved=false
                                        sim_ac_QTDIR_cppflags="-I$i -I$i/Qt -I$i/QtCore -I$i/QtNetwork -I$i/Qt3Support"
                                       ])
          fi
        fi
      done
      CPPFLAGS="$sim_ac_temp"
    fi

    # Mac OS X Darwin ports
    if $sim_ac_qglobal_unresolved; then
      sim_ac_temp="$CPPFLAGS"
      sim_ac_i="-I/opt/local/include/qt3"
      CPPFLAGS="$sim_ac_i $CPPFLAGS"
      SIM_AC_CHECK_HEADER_SILENT([qglobal.h],
                                 [sim_ac_qglobal_unresolved=false
                                  sim_ac_QTDIR_cppflags="$sim_ac_i"
                                  sim_ac_QTDIR_ldflags="-L/opt/local/lib"
                                 ])
      CPPFLAGS="$sim_ac_temp"
    fi

    if $sim_ac_qglobal_unresolved; then
      AC_MSG_WARN([header file qglobal.h not found, can not compile Qt code])
    else
      CPPFLAGS="$sim_ac_QTDIR_cppflags $CPPFLAGS"
      LDFLAGS="$LDFLAGS $sim_ac_QTDIR_ldflags"

      SIM_AC_QT_VERSION

      if test $sim_ac_qt_version -lt 200; then
        SIM_AC_ERROR([too-old-qt])
      fi

      # Too hard to feature-check for the Qt-on-Mac problems, as they involve
      # obscure behavior of the QGLWidget -- so we just resort to do platform
      # and version checking instead.
      case $host_os in
      darwin*)
        if test $sim_ac_qt_version -lt 302; then
          SIM_AC_CONFIGURATION_WARNING([The version of Qt you are using is known to contain some serious bugs on MacOS X. We strongly recommend you to upgrade. (See $srcdir/README.MAC for details.)])
        fi

        if $sim_ac_enable_darwin_x11; then
          # --enable-darwin-x11 specified but attempting Qt/Mac linkage
          AC_TRY_COMPILE([#include <qapplication.h>],
                      [#if defined(__APPLE__) && defined(Q_WS_MAC)
                       #error blah!
                       #endif],[],
                      [SIM_AC_ERROR([mac-qt-but-x11-requested])])
        else 
          # Using Qt/X11 but option --enable-darwin-x11 not given
          AC_TRY_COMPILE([#include <qapplication.h>],
                    [#if defined(__APPLE__) && defined(Q_WS_X11)
                     #error blah!
                     #endif],[],
                    [SIM_AC_ERROR([x11-qt-but-no-x11-requested])])
        fi
        ;;
      esac

      # Known problems:
      #
      #   * Qt v3.0.1 has a bug where SHIFT-PRESS + CTRL-PRESS + CTRL-RELEASE
      #     results in the last key-event coming out completely wrong under X11.
      #     Known to be fixed in 3.0.3, unknown status in 3.0.2.  <mortene@sim.no>.
      #
      if test $sim_ac_qt_version -lt 303; then
        SIM_AC_CONFIGURATION_WARNING([The version of Qt you are compiling against is known to contain bugs which influences functionality in SoQt. We strongly recommend you to upgrade.])
      fi

      sim_ac_qt_cppflags=

      # Do not cache the result, as we might need to play tricks with
      # CPPFLAGS under MSWin.

      # It should be helpful to be able to override the libs-checking with
      # environment variables. Then people won't get completely stuck
      # when the check fails -- we can just take a look at the
      # config.log and give them advice on how to proceed with no updates
      # necessary.
      #
      # (Note also that this makes it possible to select whether to use the
      # mt-safe or the "standard" Qt library if both are installed on the
      # user's system.)
      #
      # mortene.
  
      if test x"$CONFIG_QTLIBS" != x""; then
        AC_MSG_CHECKING([for Qt linking with $CONFIG_QTLIBS])

        for sim_ac_qt_cppflags_loop in "" "-DQT_DLL"; do
          CPPFLAGS="$sim_ac_QTDIR_cppflags $sim_ac_qt_cppflags_loop $sim_ac_save_cppflags"
          LIBS="$CONFIG_QTLIBS $sim_ac_save_libs"
          AC_TRY_LINK([#include <qapplication.h>],
                      [
                       // FIXME: assignment to qApp does no longer work with Qt 4,
                       // should try to find another way to do the same thing. 20050629 mortene.
                       #if QT_VERSION < 0x040000
                       qApp = NULL; /* QT_DLL must be defined for assignment to global variables to work */
                       #endif
                       qApp->exit(0);],
                      [sim_ac_qt_libs="$CONFIG_QTLIBS"
                       sim_ac_qt_cppflags="$sim_ac_QTDIR_cppflags $sim_ac_qt_cppflags_loop"
                       sim_ac_qt_ldflags="$sim_ac_QTDIR_ldflags"
                      ])
        done

        if test "x$sim_ac_qt_libs" = "xUNRESOLVED"; then
          AC_MSG_RESULT([failed!])
        else
          AC_MSG_RESULT([ok])
        fi

      else
        AC_MSG_CHECKING([for Qt library devkit])

        ## Test all known possible combinations of linking against the
        ## Troll Tech Qt library:
        ##
        ## * "-lQtGui -lQt3Support": Qt 4 on UNIX-like systems (with some
        ##   obsoleted Qt 3 widgets)

        sim_ac_qt_suffix=
        ##NOTE: BA 2006-11-28 -- we can't use Qt3 so don't try to support them
        ## also, we only have the _debug ones on the Mac
        ##if $sim_ac_qt_debug; then
        ##  sim_ac_qt_suffix=d
        ##fi

        for sim_ac_qt_cppflags_loop in "" "-DQT_DLL"; do
          for sim_ac_qt_libcheck in \
               "-lQtGui_debug -lQt3Support_debug -lQtNetwork_debug -lQtOpenGL_debug -lQtCore_debug" \
               "-lQtGui -lQt3Support -lQtNetwork -lQtOpenGL -lQtCore"
          do
            if test "x$sim_ac_qt_libs" = "xUNRESOLVED"; then

	      case $host in 
                      *-*-mingw32*)
                          sim_ac_QTDIR_cppflags="$(echo $sim_ac_QTDIR_cppflags | sed 's/\/ -I/ -I/g;s/\/$//')"
                          sim_ac_qt_libcheck="-lQtGui4 -lQt3Support4 -lQtNetwork4 -lQtOpenGL4 -lQtCore4"
                      ;;
                      *)
                      ;;
              esac

              CPPFLAGS="$sim_ac_QTDIR_cppflags $sim_ac_qt_cppflags_loop $sim_ac_save_cppflags"
              LIBS="$sim_ac_qt_libcheck $sim_ac_save_libs"
              AC_TRY_LINK([#include <qapplication.h>],
                          [
                           // FIXME: assignment to qApp does no longer work with Qt 4,
                           // should try to find another way to do the same thing. 20050629 mortene.
                           #if QT_VERSION < 0x040000
                           qApp = NULL; /* QT_DLL must be defined for assignment to global variables to work */
                           #endif
                           qApp->exit(0);],
                          [sim_ac_qt_libs="$sim_ac_qt_libcheck"
                           sim_ac_qt_cppflags="$sim_ac_QTDIR_cppflags $sim_ac_qt_cppflags_loop"
                           sim_ac_qt_ldflags="$sim_ac_QTDIR_ldflags"
                          ])
            fi
          done
        done


        AC_MSG_RESULT([yes])
      fi

    fi # sim_ac_qglobal_unresolved = false

  fi # sim_ac_have_qt_framework

  # We should only *test* availability, not mutate the LIBS/CPPFLAGS
  # variables ourselves inside this macro. 20041021 larsa
  CPPFLAGS=$sim_ac_save_cppflags
  LDFLAGS=$sim_ac_save_ldflags
  LIBS=$sim_ac_save_libs
  if test ! x"$sim_ac_qt_libs" = xUNRESOLVED; then
    sim_ac_qt_avail=yes
    $1

    sim_ac_qt_install=`cd $sim_ac_qtdir; pwd`/bin/install
    AC_MSG_CHECKING([whether Qt's install tool shadows the system install])
    case $INSTALL in
    "${sim_ac_qt_install}"* )
      AC_MSG_RESULT(yes)
      echo "\$INSTALL is '$INSTALL', which matches '$QTDIR/bin/install*'." >&5
      echo "\$QTDIR part is '$QTDIR'." >&5
      SIM_AC_ERROR([qt-install])
      ;;
    * )
      AC_MSG_RESULT(no)
      ;;
    esac

  else
#     if test -z "$QTDIR"; then
#       AC_MSG_WARN([QTDIR environment variable not set -- this might be an indication of a problem])
#     fi
    $2
  fi
fi
])

############################################################################
# Usage:
#  SIM_AC_CHECK_QGL([ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
#
#  Try to find the QGL widget for interfacing Qt with OpenGL. If it
#  is found, these shell variables are set:
#
#    $sim_ac_qgl_cppflags (extra flags the compiler needs for QGL lib)
#    $sim_ac_qgl_ldflags  (extra flags the linker needs for QGL lib)
#    $sim_ac_qgl_libs     (link libraries the linker needs for QGL lib)
#
#  The LIBS flag will also be modified accordingly. In addition, the
#  variable $sim_ac_qgl_avail is set to "yes" if the QGL extension
#  library is found.
#
# Note that all "modern" variants of Qt should come with QGL embedded.
# There's one important deviation: Debian comes with a -lqt which is
# missing QGL support, while it also has a -lqt-mt *with* QGL support.
# The reason for this is because the default GL (Mesa) library on Debian
# is built in mt-safe mode.
#
# Author: Morten Eriksen, <mortene@sim.no>.

AC_DEFUN([SIM_AC_CHECK_QGL], [

AC_REQUIRE([SIM_AC_WITH_QT])

sim_ac_qgl_avail=no
sim_ac_qgl_cppflags=
sim_ac_qgl_ldflags=
sim_ac_qgl_libs=

if $sim_ac_with_qt; then
  # first check if we can link with the QGL widget already
  AC_CACHE_CHECK(
    [whether the QGL widget is part of main Qt library],
    sim_cv_lib_qgl_integrated,
    [AC_TRY_LINK([#include <qgl.h>],
                 [QGLFormat * f = new QGLFormat; f->setDepth(true); ],
                 [sim_cv_lib_qgl_integrated=yes],
                 [sim_cv_lib_qgl_integrated=no])])

  if test x"$sim_cv_lib_qgl_integrated" = xyes; then
    sim_ac_qgl_avail=yes
    $1
  else
    sim_ac_save_LIBS=$LIBS
    LIBS="$sim_ac_qgl_libs $LIBS"

    AC_MSG_CHECKING([for the QGL extension library])

    sim_ac_qgl_libs=UNRESOLVED
    for sim_ac_qgl_libcheck in "-lQtOpenGL4" "-lQtOpenGL" "-lqgl" "-lqgl -luser32"; do
      if test "x$sim_ac_qgl_libs" = "xUNRESOLVED"; then
        LIBS="$sim_ac_qgl_libcheck $sim_ac_save_LIBS"
        AC_TRY_LINK([#include <qgl.h>],
                    [QGLFormat * f = new QGLFormat; f->setDepth(true); ],
                    [sim_ac_qgl_libs="$sim_ac_qgl_libcheck"])
      fi
    done

    if test x"$sim_ac_qgl_libs" != xUNRESOLVED; then
      AC_MSG_RESULT($sim_ac_qgl_libs)
      sim_ac_qgl_avail=yes
      $1
    else
      AC_MSG_RESULT([unavailable])
      LIBS=$sim_ac_save_LIBS
      $2
    fi
  fi
fi
])

# SIM_AC_QGLWIDGET_SETAUTOBUFFERSWAP
# ----------------------------------
#
# Use the macro for its side-effect: it defines
#
#       HAVE_QGLWIDGET_SETAUTOBUFFERSWAP
#
# to 1 in config.h if QGLWidget::setAutoBufferSwap() is available.
#
# Author: Morten Eriksen, <mortene@sim.no>.

AC_DEFUN([SIM_AC_QGLWIDGET_SETAUTOBUFFERSWAP], [
AC_CACHE_CHECK(
  [whether the QGLWidget::setAutoBufferSwap() is available],
  sim_cv_func_qglwidget_setautobufferswap,
  [AC_TRY_LINK([#include <qgl.h>
class MyGLWidget : public QGLWidget {
public: MyGLWidget() {setAutoBufferSwap(FALSE);} };],
               [MyGLWidget * w = new MyGLWidget;],
               [sim_cv_func_qglwidget_setautobufferswap=yes],
               [sim_cv_func_qglwidget_setautobufferswap=no])])

if test x"$sim_cv_func_qglwidget_setautobufferswap" = xyes; then
  AC_DEFINE([HAVE_QGLWIDGET_SETAUTOBUFFERSWAP], 1,
    [Define this to 1 if QGLWidget::setAutoBufferSwap() is available])
fi
])


# SIM_AC_QGLFORMAT_SETOVERLAY
# ---------------------------
#
# Use the macro for its side-effect: it defines
#
#       HAVE_QGLFORMAT_SETOVERLAY
#
# to 1 in config.h if QGLFormat::setOverlay() is available.
#
# Author: Morten Eriksen, <mortene@sim.no>.

AC_DEFUN([SIM_AC_QGLFORMAT_SETOVERLAY], [
AC_CACHE_CHECK(
  [whether QGLFormat::setOverlay() is available],
  sim_cv_func_qglformat_setoverlay,
  [AC_TRY_LINK([#include <qgl.h>],
               [/* The basics: */
                QGLFormat f; f.setOverlay(TRUE);
                /* We've had a bug report about soqt.dll linking fail due
                   to missing the QGLWidget::overlayContext() symbol: */
                QGLWidget * w = NULL; (void)w->overlayContext();
               ],
               [sim_cv_func_qglformat_setoverlay=yes],
               [sim_cv_func_qglformat_setoverlay=no])])

if test x"$sim_cv_func_qglformat_setoverlay" = xyes; then
  AC_DEFINE([HAVE_QGLFORMAT_SETOVERLAY], 1,
    [Define this to 1 if QGLFormat::setOverlay() is available])
fi
])


# SIM_AC_QGLFORMAT_EQ_OP
# ----------------------
#
# Use the macro for its side-effect: it defines
#
#       HAVE_QGLFORMAT_EQ_OP
#
# to 1 in config.h if operator==(QGLFormat&, QGLFormat&) is available.
# (For Qt v2.2.2 at least, Troll Tech forgot to include this method
# in the publicly exported API for MSWindows DLLs.)
#
# Author: Morten Eriksen, <mortene@sim.no>.

AC_DEFUN([SIM_AC_QGLFORMAT_EQ_OP], [
AC_CACHE_CHECK(
  [whether operator==(QGLFormat&,QGLFormat&) is available],
  sim_cv_func_qglformat_eq_op,
  [AC_TRY_LINK([#include <qgl.h>],
               [QGLFormat f; if (f == f) f.setDepth(true);],
               [sim_cv_func_qglformat_eq_op=true],
               [sim_cv_func_qglformat_eq_op=false])])

if $sim_cv_func_qglformat_eq_op; then
  AC_DEFINE([HAVE_QGLFORMAT_EQ_OP], 1,
    [Define this to 1 if operator==(QGLFormat&, QGLFormat&) is available])
fi
])


# SIM_AC_QWIDGET_SHOWFULLSCREEN
# -----------------------------
#
# Use the macro for its side-effect: it defines HAVE_QWIDGET_SHOWFULLSCREEN
# to 1 in config.h if QWidget::showFullScreen() is available (that
# function wasn't introduced in Qt until version 2.1.0).
#
# Author: Morten Eriksen, <mortene@sim.no>.

AC_DEFUN([SIM_AC_QWIDGET_SHOWFULLSCREEN], [
AC_CACHE_CHECK(
  [whether QWidget::showFullScreen() is available],
  sim_cv_def_qwidget_showfullscreen,
  [AC_TRY_LINK([#include <qwidget.h>],
               [QWidget * w = new QWidget(); w->showFullScreen();],
               [sim_cv_def_qwidget_showfullscreen=true],
               [sim_cv_def_qwidget_showfullscreen=false])])

if $sim_cv_def_qwidget_showfullscreen; then
  AC_DEFINE([HAVE_QWIDGET_SHOWFULLSCREEN], 1,
            [Define this if QWidget::showFullScreen() is available])
fi
]) # SIM_AC_QWIDGET_SHOWFULLSCREEN

# SIM_AC_QAPPLICATION_HASPENDINGEVENTS
# -----------------------------
#
# Use the macro for its side-effect: it defines 
# HAVE_QAPPLICATION_HASPENDINGEVENTS to 1 in config.h if 
# QApplication::hasPendingEvents() is available (that
# function wasn't introduced in Qt until version 3.0).
#
# Author: Peder Blekken <pederb@sim.no>.

AC_DEFUN([SIM_AC_QAPPLICATION_HASPENDINGEVENTS], [
AC_CACHE_CHECK(
  [whether QApplication::hasPendingEvents() is available],
  sim_cv_def_qapplication_haspendingevents,
  [AC_TRY_LINK([#include <qapplication.h>],
               [int argc; char ** argv; QApplication app(argc, argv); (void) app.hasPendingEvents();],
               [sim_cv_def_qapplication_haspendingevents=true],
               [sim_cv_def_qapplication_haspendingevents=false])])

if $sim_cv_def_qapplication_haspendingevents; then
  AC_DEFINE([HAVE_QAPPLICATION_HASPENDINGEVENTS], 1,
            [Define this if QApplication::hasPendingEvents() is available])
fi
]) # SIM_AC_QAPPLICATION_HASPENDINGEVENTS


# SIM_AC_QT_KEYPAD_DEFINE
# -----------------------
#
# Use the macro for its side-effect: it defines HAVE_QT_KEYPAD_DEFINE
# to 1 in config.h if Qt::Keypad is available.
#
# Author: Morten Eriksen, <mortene@sim.no>.

AC_DEFUN([SIM_AC_QT_KEYPAD_DEFINE], [
AC_CACHE_CHECK(
  [whether Qt::Keypad is defined],
  sim_cv_def_qt_keypad,
  [AC_TRY_LINK([#include <qkeycode.h>],
               [Qt::ButtonState s = Qt::Keypad;],
               [sim_cv_def_qt_keypad=true],
               [sim_cv_def_qt_keypad=false])])

if $sim_cv_def_qt_keypad; then
  AC_DEFINE([HAVE_QT_KEYPAD_DEFINE], 1,
            [Define this if Qt::Keypad is available])
fi
]) # SIM_AC_QT_KEYPAD_DEFINE


# SIM_AC_QWIDGET_HASSETWINDOWSTATE
# --------------------------------
# QWidget->setWindowState() was added around Qt 3.3

AC_DEFUN([SIM_AC_QWIDGET_HASSETWINDOWSTATE], [
AC_CACHE_CHECK(
  [whether QWidget::setWindowState() exists],
  sim_cv_exists_qwidget_setwindowstate,

  [AC_TRY_LINK([#include <qwidget.h>],
               [QWidget * w = NULL; w->setWindowState(0);],
               [sim_cv_exists_qwidget_setwindowstate=true],
               [sim_cv_exists_qwidget_setwindowstate=false])])

if $sim_cv_exists_qwidget_setwindowstate; then
  AC_DEFINE([HAVE_QWIDGET_SETWINDOWSTATE], 1,
            [Define this if QWidget::setWindowState() is available])
fi
]) # SIM_AC_QWIDGET_HASSETWINDOWSTATE



# SIM_AC_QLINEEDIT_HASSETINPUTMASK
# --------------------------------
# QLineEdit->setInputMask() was added around Qt 3.3

AC_DEFUN([SIM_AC_QLINEEDIT_HASSETINPUTMASK], [
AC_CACHE_CHECK(
  [whether QLineEdit::setInputMask() exists],
  sim_cv_exists_qlineedit_setinputmask,

  [AC_TRY_LINK([#include <qlineedit.h>],
               [QLineEdit * le = NULL; le->setInputMask(0);],
               [sim_cv_exists_qlineedit_setinputmask=true],
               [sim_cv_exists_qlineedit_setinputmask=false])])

if $sim_cv_exists_qlineedit_setinputmask; then
  AC_DEFINE([HAVE_QLINEEDIT_SETINPUTMASK], 1,
            [Define this if QLineEdit::setInputMask() is available])
fi
]) # SIM_AC_QLINEEDIT_HASSETINPUTMASK



# SIM_AC_HAVE_QT_FRAMEWORK
# ----------------------
#
# Determine whether Qt is installed as a Mac OS X framework.  
#
# Uses the variable $sim_ac_qt_framework_dir which should either 
# point to /Library/Frameworks or $QTDIR/lib. 
#
# Sets sim_ac_have_qt_framework to true if Qt is installed as 
# a framework, and to false otherwise. 
#
# Author: Karin Kosina, <kyrah@sim.no>.

AC_DEFUN([SIM_AC_HAVE_QT_FRAMEWORK], [
case $host_os in
  darwin*)
    # First check if framework exists in the specified location, then
    # try to actually link against the framework. This precaution is
    # needed to catch the case where Qt-4 is installed in the default
    # location /Library/Frameworks, but the user wants to override it
    # by setting QTDIR to point to a non-framework install.
    if test -d $sim_ac_qt_framework_dir/QtCore.framework; then
      sim_ac_save_ldflags_fw=$LDFLAGS 
      LDFLAGS="$LDFLAGS -F$sim_ac_qt_framework_dir -framework QtCore"
      AC_CACHE_CHECK(
        [whether Qt is installed as a framework],
        sim_ac_have_qt_framework,
        [AC_TRY_LINK([#include <QtCore/qglobal.h>],
                 [],
                 [sim_ac_have_qt_framework=true],
                 [sim_ac_have_qt_framework=false])
        ])
        LDFLAGS=$sim_ac_save_ldflags_fw
    else 
      sim_ac_have_qt_framework=false
    fi 
    ;;
  *)
    sim_ac_have_qt_framework=false
    ;;
esac
])

# **************************************************************************
# SIM_AC_MAC_CPP_ADJUSTMENTS
#
# Add --no-cpp-precomp if necessary. Without this option, the
# Apple preprocessor is used on Mac OS X platforms, and it is
# known to be very buggy.  It's better to use this option, so
# the GNU preprocessor is preferred.
#


AC_DEFUN([SIM_AC_MAC_CPP_ADJUSTMENTS],
[case $host_os in
darwin*)
  if test x"$GCC" = x"yes"; then
    # FIXME: create a SIM_AC_CPP_OPTION macro
    SIM_AC_CC_COMPILER_OPTION([-no-cpp-precomp], [CPPFLAGS="$CPPFLAGS -no-cpp-precomp"])
  fi
  ;;
esac
]) # SIM_AC_MAC_CPP_ADJUSTMENTS


# **************************************************************************
# This macro sets up the MAC_FRAMEWORK automake conditional, depending on
# the host OS and whether $sim_ac_prefer_framework has been overridden or
# not.

AC_DEFUN([SIM_AC_MAC_FRAMEWORK],
[case $host_os in
darwin*)
  : ${sim_ac_prefer_framework=true}
  ;;
esac
: ${sim_ac_prefer_framework=false}
# This AM_CONDITIONAL can be used to make Mac OS X specific make-rules
# related to installing proper Frameworks instead.
AM_CONDITIONAL([MAC_FRAMEWORK], [$sim_ac_prefer_framework])

if $sim_ac_prefer_framework; then
  ifelse([$1], , :, [$1])
else
  ifelse([$2], , :, [$2])
fi
]) # SIM_AC_MAC_FRAMEWORK


############################################################################
# Usage:
#  SIM_AC_UNIVERSAL_BINARIES([ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
#
#  Determine whether we should build Universal Binaries. If yes, these
#  shell variables are set:
#
#    $sim_ac_enable_universal (true if we are building Universal Binaries)
#    $sim_ac_universal_flags (extra flags needed for Universal Binaries)
#  
#  The CFLAGS and CXXFLAGS variables will also be modified accordingly.
#
#  Note that when building Universal Binaries, dependency tracking will 
#  be turned off.
#
#  Important: This macro must be called _before_ AM_INIT_AUTOMAKE.
#
# Author: Karin Kosina <kyrah@sim.no>.

AC_DEFUN([SIM_AC_UNIVERSAL_BINARIES], [

sim_ac_enable_universal=false


case $host_os in
  darwin* ) 
    AC_ARG_ENABLE(
      [universal],
      AC_HELP_STRING([--enable-universal], [build Universal Binaries]), [
        case $enableval in
          yes | true) sim_ac_enable_universal=true ;;
          *) ;;
        esac])

    AC_MSG_CHECKING([whether we should build Universal Binaries])   
    if $sim_ac_enable_universal; then
      AC_MSG_RESULT([yes])
      SIM_AC_CONFIGURATION_SETTING([Build Universal Binaries], [Yes])

      # need to build against Universal Binary SDK on PPC
      if test x"$host_cpu" = x"powerpc"; then
        sim_ac_universal_sdk_flags="-isysroot /Developer/SDKs/MacOSX10.4u.sdk"
      fi

      sim_ac_universal_flags="-arch i386 -arch ppc $sim_ac_universal_sdk_flags"
      
      CFLAGS="$sim_ac_universal_flags $CFLAGS"
      CXXFLAGS="$sim_ac_universal_flags $CXXFLAGS"

      # disable dependency tracking since we can't use -MD when cross-compiling
      enable_dependency_tracking=no
    else
      AC_MSG_RESULT([no])
      SIM_AC_CONFIGURATION_SETTING([Build Universal Binaries], [No (default)])
    fi
esac
]) # SIM_AC_UNIVERSAL_BINARIES

AC_DEFUN([BASH_CHECK_LIB_TERMCAP],
[
if test "X$bash_cv_termcap_lib" = "X"; then
_bash_needmsg=yes
else
AC_MSG_CHECKING(which library has the termcap functions)
_bash_needmsg=
fi
AC_CACHE_VAL(bash_cv_termcap_lib,
[AC_CHECK_FUNC(tgetent, bash_cv_termcap_lib=libc,
  [AC_CHECK_LIB(termcap, tgetent, bash_cv_termcap_lib=libtermcap,
    [AC_CHECK_LIB(tinfo, tgetent, bash_cv_termcap_lib=libtinfo,
        [AC_CHECK_LIB(curses, tgetent, bash_cv_termcap_lib=libcurses,
	    [AC_CHECK_LIB(ncurses, tgetent, bash_cv_termcap_lib=libncurses,
	        bash_cv_termcap_lib=gnutermcap)])])])])])
if test "X$_bash_needmsg" = "Xyes"; then
AC_MSG_CHECKING(which library has the termcap functions)
fi
AC_MSG_RESULT(using $bash_cv_termcap_lib)
if test $bash_cv_termcap_lib = gnutermcap && test -z "$prefer_curses"; then
LDFLAGS="$LDFLAGS -L./lib/termcap"
TERMCAP_LIB="./lib/termcap/libtermcap.a"
TERMCAP_DEP="./lib/termcap/libtermcap.a"
elif test $bash_cv_termcap_lib = libtermcap && test -z "$prefer_curses"; then
TERMCAP_LIB=-ltermcap
TERMCAP_DEP=
elif test $bash_cv_termcap_lib = libtinfo; then
TERMCAP_LIB=-ltinfo
TERMCAP_DEP=
elif test $bash_cv_termcap_lib = libncurses; then
TERMCAP_LIB=-lncurses
TERMCAP_DEP=
elif test $bash_cv_termcap_lib = libc; then
TERMCAP_LIB=
TERMCAP_DEP=
else
TERMCAP_LIB=-lcurses
TERMCAP_DEP=
fi
])

# **************************************************************************
# SIM_AC_HAVE_SIMAGE_IFELSE( IF-FOUND, IF-NOT-FOUND )
#
# Description:
#   This macro locates the simage development system.  If it is found, the
#   set of variables listed below are set up as described and made available
#   to the configure script.
#
#   The $sim_ac_simage_desired variable can be set to false externally to
#   make SoXt default to be excluded.
#
# Autoconf Variables:
# > $sim_ac_simage_desired   true | false (defaults to true)
# < $sim_ac_simage_avail     true | false
# < $sim_ac_simage_cppflags  (extra flags the compiler needs for simage)
# < $sim_ac_simage_ldflags   (extra flags the linker needs for simage)
# < $sim_ac_simage_libs      (link libraries the linker needs for simage)
# < $sim_ac_simage_version   (the libsimage version)
#
# Authors:
#   Morten Eriksen <mortene@coin3d.org>
#   Lars J. Aas <larsa@coin3d.org>
#
# TODO:
# - rework variable name convention
# - clean up shell scripting redundancy
# - support debug symbols simage library
#

AC_DEFUN([SIM_AC_HAVE_SIMAGE_IFELSE],
[AC_PREREQ([2.14a])

# official variables
sim_ac_simage_avail=false
sim_ac_simage_cppflags=
sim_ac_simage_ldflags=
sim_ac_simage_libs=
sim_ac_simage_version=

# internal variables
: ${sim_ac_simage_desired=true}
sim_ac_simage_extrapath=

AC_ARG_WITH(
  simage,
  AC_HELP_STRING([--with-simage=DIR],
                 [use simage for loading texture files]),
  [case $withval in
   yes) sim_ac_simage_desired=true ;;
   no)  sim_ac_simage_desired=false ;;
   *)   sim_ac_simage_desired=true
        sim_ac_simage_extrapath=$withval ;;
  esac],
  [])

if $sim_ac_simage_desired; then
  sim_ac_path=$PATH
  test -z "$sim_ac_simage_extrapath" ||
    sim_ac_path=$sim_ac_simage_extrapath/bin:$sim_ac_path
  test x"$prefix" = xNONE ||
    sim_ac_path=$sim_ac_path:$prefix/bin

  AC_PATH_PROG(sim_ac_simage_configcmd, simage-config, false, $sim_ac_path)

  if test "X$sim_ac_simage_configcmd" = "Xfalse"; then :; else
    test -n "$CONFIG" &&
      $sim_ac_simage_configcmd --alternate=$CONFIG >/dev/null 2>/dev/null &&
      sim_ac_simage_configcmd="$sim_ac_simage_configcmd --alternate=$CONFIG"
  fi

  if $sim_ac_simage_configcmd; then
    sim_ac_simage_cppflags=`$sim_ac_simage_configcmd --cppflags`
    sim_ac_simage_ldflags=`$sim_ac_simage_configcmd --ldflags`
    sim_ac_simage_libs=`$sim_ac_simage_configcmd --libs`
    sim_ac_simage_version=`$sim_ac_simage_configcmd --version`
    AC_CACHE_CHECK([whether the simage library is available],
      sim_cv_simage_avail,
      [sim_ac_save_cppflags=$CPPFLAGS
      sim_ac_save_ldflags=$LDFLAGS
      sim_ac_save_libs=$LIBS
      CPPFLAGS="$CPPFLAGS $sim_ac_simage_cppflags"
      LDFLAGS="$LDFLAGS $sim_ac_simage_ldflags"
      LIBS="$sim_ac_simage_libs $LIBS"
      AC_TRY_LINK(
        [#include <simage.h>],
        [(void)simage_read_image(0L, 0L, 0L, 0L);],
        [sim_cv_simage_avail=true],
        [sim_cv_simage_avail=false])
      CPPFLAGS=$sim_ac_save_cppflags
      LDFLAGS=$sim_ac_save_ldflags
      LIBS=$sim_ac_save_libs
    ])
    sim_ac_simage_avail=$sim_cv_simage_avail
  else
    locations=`IFS=:
               for p in $sim_ac_path; do echo " -> $p/simage-config"; done`
    AC_MSG_WARN([cannot find 'simage-config' at any of these locations:
$locations])
  fi
fi

if $sim_ac_simage_avail; then
  ifelse([$1], , :, [$1])
else
  ifelse([$2], , :, [$2])
fi
])

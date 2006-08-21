dnl 					       PDP_SET_BUILD_MODE
dnl *************************************************************
dnl Figure out what kind of build mode we are doing, and prep
dnl the system for it
dnl *************************************************************
dnl Copyright, 1995-2005, Regents of the University of Colorado,
dnl Carnegie Mellon University, Princeton University.
dnl
dnl This file is part of TA/PDP++
dnl
dnl   TA/PDP++ is free software; you can redistribute it and/or modify
dnl   it under the terms of the GNU General Public License as published by
dnl   the Free Software Foundation; either version 2 of the License, or
dnl   (at your option) any later version.
dnl
dnl   TA/PDP++ is distributed in the hope that it will be useful,
dnl   but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl   GNU General Public License for more details. 
AC_DEFUN([PDP_SET_BUILD_MODE],[
AC_ARG_ENABLE([readline],
	      AC_HELP_STRING([--disable-readline],
			     [Disable linking against the readline library.  @<:@default=enabled@:>@]),
			     [readline=false],
			     [readline=true])
AC_ARG_WITH([rpm],
	      AC_HELP_STRING([--with-rpm],
			     [Enable the creation of rpms with `make rpm'.You must first `touch aminclude.am' and set PLATFORM_SUFFIX (see below).  @<:@default=disabled@:>@ ]),
			     [rpm=true],
			     [rpm=false])
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
AC_ARG_ENABLE([libta],
	      AC_HELP_STRING([--disable-libta],
			     [Disable creation of TypeAccess library.  @<:@default=enabled@:>@]),
			     [libta=false],
			     [libta=true])
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
AC_ARG_ENABLE([plugins],
	      AC_HELP_STRING([--enable-plugins],
			     [Enable plugin development support. Source code will be installed to `/usr/local/pdp++/src' and `/usr/local/pdp++/plugins' will be created.  @<:@default=disabled@:>@]),
			     [plugins=true],
			     [plugins=false])
AM_CONDITIONAL([PLUGINS],[test $plugins = true])
AM_CONDITIONAL([RPM],[test $rpm = true])
AM_CONDITIONAL([TA_GUI],[test $gui = true])
AM_CONDITIONAL([NO_TA_GUI],[test $gui = false])
AM_CONDITIONAL([MPI],[test $mpi = true])
AM_CONDITIONAL([DEBUG],[test $debug = true])
AM_CONDITIONAL([LIBTA],[test $libta = true])
AM_CONDITIONAL([CSS_BIN],[test $css_bin = true])
])


dnl 					             PDP_PROG_CXX
dnl *************************************************************
dnl This macro allows us to set our own level of optimization
dnl on C++ files. We use it to set -O0 on all TA files all the
dnl time and to not do any optimization in --enable-debug mode
dnl *************************************************************
dnl Copyright, 1995-2005, Regents of the University of Colorado,
dnl Carnegie Mellon University, Princeton University.
dnl
dnl This file is part of TA/PDP++
dnl
dnl   TA/PDP++ is free software; you can redistribute it and/or modify
dnl   it under the terms of the GNU General Public License as published by
dnl   the Free Software Foundation; either version 2 of the License, or
dnl   (at your option) any later version.
dnl
dnl   TA/PDP++ is distributed in the hope that it will be useful,
dnl   but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl   GNU General Public License for more details. 

AC_DEFUN([PDP_PROG_CXX],[

AC_LANG_CPLUSPLUS
save_user_CXXFLAGS=${CXXFLAGS}
CXXFLAGS=
AC_PROG_CXX([g++])
CXXFLAGS=${save_user_CXXFLAGS}

# Still enable disabling optimization
SIM_AC_COMPILER_OPTIMIZATION
])

dnl 					             PDP_PROG_PDP
dnl *************************************************************
dnl This macro checks for the existence of the pdp and css
dnl executeables in the chosen installation path. E.g., we will
dnl refuse to overwrite the executeables, instead preferring that
dnl the user renames them using configure name mangling options.
dnl ************************************************************
AC_DEFUN([PDP_PROG_PDP],[
# Did the user specify an installation path?
check_path=$ac_default_prefix/bin
test x"$prefix" != xNONE && check_path=$prefix/bin
test x"$exec_prefix" != xNONE && check_path=$exec_prefix/bin

AC_CHECK_PROG([pdpexists],[pdp++],[true],[false],[${check_path}])
if test x"${pdpexists}" = x"true"; then
   SIM_AC_CONFIGURATION_WARNING([pdp++ is already installed in ${check_path}. Consider renaming with --program-suffix=SUFFIX])
fi

AC_CHECK_PROG([cssexists],[css++],[true],[false],[${check_path}])
if test x"${cssexists}" = x"true"; then
   SIM_AC_CONFIGURATION_WARNING([css++ is already installed in ${check_path}. Consider renaming with --program-suffix=SUFFIX])
fi
])


dnl 					     PDP_DETERMINE_OSTYPE
dnl *************************************************************
dnl  These tests are used mainly for Maketa.am at this point.
dnl *************************************************************
dnl Copyright, 1995-2005, Regents of the University of Colorado,
dnl Carnegie Mellon University, Princeton University.
dnl
dnl This file is part of TA/PDP++
dnl
dnl   TA/PDP++ is free software; you can redistribute it and/or modify
dnl   it under the terms of the GNU General Public License as published by
dnl   the Free Software Foundation; either version 2 of the License, or
dnl   (at your option) any later version.
dnl
dnl   TA/PDP++ is distributed in the hope that it will be useful,
dnl   but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl   GNU General Public License for more details. 

AC_DEFUN([PDP_CANONICAL_HOST],[
AC_REQUIRE([AC_CANONICAL_HOST])
case $host in
	*linux*)
		AC_DEFINE([LINUX],[1],[When on linux])
	;;
	*darwin*)
		AC_DEFINE([DARWIN],[1],[When on darwin])
		AC_DEFINE([LINUX],[1],[When on darwin])
	;;
	*-*-msdos* | *-*-go32* | *-*-mingw32* | *-*-cygwin* | *-*-windows*)
		AC_DEFINE([CYGWIN],[1],[When on cygwin])
		AC_DEFINE([WIN32],[1],[When on cygwin])
		AC_DEFINE([WIN32],[1],[When on cygwin])
	;;
	*)

	;;
esac
SIM_AC_CONFIGURATION_SETTING([Host],[$host])
]) dnl PDP_CANONICAL_HOST

dnl 					     PDP_DETERMINE_SUFFIX
dnl *************************************************************
dnl  Adds configure flag dependent suffixes. E.g. bp4_nogui_debug_mpi++
dnl *************************************************************
dnl Copyright, 1995-2005, Regents of the University of Colorado,
dnl Carnegie Mellon University, Princeton University.
dnl
dnl This file is part of TA/PDP++
dnl
dnl   TA/PDP++ is free software; you can redistribute it and/or modify
dnl   it under the terms of the GNU General Public License as published by
dnl   the Free Software Foundation; either version 2 of the License, or
dnl   (at your option) any later version.
dnl
dnl   TA/PDP++ is distributed in the hope that it will be useful,
dnl   but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl   GNU General Public License for more details.

AC_DEFUN([PDP_DETERMINE_SUFFIX],[

AC_MSG_CHECKING([whether we are infixing bin and lib names])

# Version of pdp++

if test "$gui" = "false" ; then
	PDP_SUFFIX="${PDP_SUFFIX}_nogui"
fi
if test "$debug" = "true" ; then
	PDP_SUFFIX="${PDP_SUFFIX}_debug"
fi
if test "$mpi" = "true"; then
	PDP_SUFFIX="${PDP_SUFFIX}_mpi"
fi
AC_MSG_RESULT([yes])
AC_SUBST([PDP_SUFFIX])
SIM_AC_CONFIGURATION_SETTING([Infixing],[Prototype of programs is pdp${PDP_SUFFIX}++, libraries libpdp${PDP_SUFFIX}])
]) dnl PDP_DETERMINE_SUFFIX

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
# SIM_AC_SETUP_MSVC_IFELSE( IF-FOUND, IF-NOT-FOUND )
#
# This macro invokes IF-FOUND if the wrapmsvc wrapper can be run, and
# IF-NOT-FOUND if not.
#
# Authors:
#   Morten Eriksen <mortene@coin3d.org>
#   Lars J. Aas <larsa@coin3d.org>

# **************************************************************************

AC_DEFUN([SIM_AC_MSVC_DISABLE_OPTION], [
AC_ARG_ENABLE([msvc],
  [AC_HELP_STRING([--disable-msvc], [don't require MS Visual C++ on Cygwin])],
  [case $enableval in
  no | false) sim_ac_try_msvc=false ;;
  *)          sim_ac_try_msvc=true ;;
  esac],
  [sim_ac_try_msvc=true])
])

# **************************************************************************

AC_DEFUN([SIM_AC_MSVC_VERSION], [
AC_MSG_CHECKING([Visual Studio C++ version])
AC_TRY_COMPILE([],
  [long long number = 0;],
  [sim_ac_msvc_version=7]
  [sim_ac_msvc_version=6])
AC_MSG_RESULT($sim_ac_msvc_version)
])

# **************************************************************************
# Note: the SIM_AC_SETUP_MSVC_IFELSE macro has been OBSOLETED and
# replaced by the one below.
#
# If the Microsoft Visual C++ cl.exe compiler is available, set us up for
# compiling with it and to generate an MSWindows .dll file.

AC_DEFUN([SIM_AC_SETUP_MSVCPP_IFELSE],
[
AC_REQUIRE([SIM_AC_MSVC_DISABLE_OPTION])
AC_REQUIRE([SIM_AC_SPACE_IN_PATHS])

: ${BUILD_WITH_MSVC=false}
if $sim_ac_try_msvc; then
  if test -z "$CC" -a -z "$CXX"; then
    sim_ac_wrapmsvc=`cd $ac_aux_dir; pwd`/wrapmsvc.exe
    echo "$as_me:$LINENO: sim_ac_wrapmsvc=$sim_ac_wrapmsvc" >&AS_MESSAGE_LOG_FD
    AC_MSG_CHECKING([setup for wrapmsvc.exe])
    if $sim_ac_wrapmsvc >&AS_MESSAGE_LOG_FD 2>&AS_MESSAGE_LOG_FD; then
      m4_ifdef([$0_VISITED],
        [AC_FATAL([Macro $0 invoked multiple times])])
      m4_define([$0_VISITED], 1)
      CC=$sim_ac_wrapmsvc
      CXX=$sim_ac_wrapmsvc
      export CC CXX
      BUILD_WITH_MSVC=true
      AC_MSG_RESULT([working])

      # FIXME: why is this here, larsa? 20050714 mortene.
      # SIM_AC_MSVC_VERSION

      # Robustness: we had multiple reports of Cygwin ''link'' getting in
      # the way of MSVC link.exe, so do a little sanity check for that.
      #
      # FIXME: a better fix would be to call link.exe with full path from
      # the wrapmsvc wrapper, to avoid any trouble with this -- I believe
      # that should be possible, using the dirname of the full cl.exe path.
      # 20050714 mortene.
      sim_ac_check_link=`type link`
      AC_MSG_CHECKING([whether Cygwin's /usr/bin/link shadows MSVC link.exe])
      case x"$sim_ac_check_link" in
      x"link is /usr/bin/link"* )
        AC_MSG_RESULT(yes)
        AC_MSG_ERROR([cygwin-link])
        ;;
      * )
        AC_MSG_RESULT(no)
        ;;
      esac

    else
      case $host in
      *-cygwin)
        AC_MSG_RESULT([not working])
        AC_MSG_ERROR([no-msvc++]) ;;
      *)
        AC_MSG_RESULT([not working (as expected)])
        ;;
      esac
    fi
  fi
fi
export BUILD_WITH_MSVC
AC_SUBST(BUILD_WITH_MSVC)

if $BUILD_WITH_MSVC; then
  ifelse([$1], , :, [$1])
else
  ifelse([$2], , :, [$2])
fi
]) # SIM_AC_SETUP_MSVC_IFELSE

# **************************************************************************
# SIM_AC_SETUP_MSVCRT
#
# This macro sets up compiler flags for the MS Visual C++ C library of
# choice.

AC_DEFUN([SIM_AC_SETUP_MSVCRT],
[sim_ac_msvcrt_LDFLAGS=""
sim_ac_msvcrt_LIBS=""

AC_ARG_WITH([msvcrt],
  [AC_HELP_STRING([--with-msvcrt=<crt>],
                  [set which C run-time library to build against])],
  [case `echo "$withval" | tr "[A-Z]" "[a-z]"` in
  default | singlethread-static | ml | /ml | libc | libc\.lib )
    sim_ac_msvcrt=singlethread-static
    sim_ac_msvcrt_CFLAGS="/ML"
    sim_ac_msvcrt_CXXFLAGS="/ML"
    ;;
  default-debug | singlethread-static-debug | mld | /mld | libcd | libcd\.lib )
    sim_ac_msvcrt=singlethread-static-debug
    sim_ac_msvcrt_CFLAGS="/MLd"
    sim_ac_msvcrt_CXXFLAGS="/MLd"
    ;;
  multithread-static | mt | /mt | libcmt | libcmt\.lib )
    sim_ac_msvcrt=multithread-static
    sim_ac_msvcrt_CFLAGS="/MT"
    sim_ac_msvcrt_CXXFLAGS="/MT"
    ;;
  multithread-static-debug | mtd | /mtd | libcmtd | libcmtd\.lib )
    sim_ac_msvcrt=multithread-static-debug
    sim_ac_msvcrt_CFLAGS="/MTd"
    sim_ac_msvcrt_CXXFLAGS="/MTd"
    ;;
  multithread-dynamic | md | /md | msvcrt | msvcrt\.lib )
    sim_ac_msvcrt=multithread-dynamic
    sim_ac_msvcrt_CFLAGS="/MD"
    sim_ac_msvcrt_CXXFLAGS="/MD"
    ;;
  multithread-dynamic-debug | mdd | /mdd | msvcrtd | msvcrtd\.lib )
    sim_ac_msvcrt=multithread-dynamic-debug
    sim_ac_msvcrt_CFLAGS="/MDd"
    sim_ac_msvcrt_CXXFLAGS="/MDd"
    ;;
  *)
    AC_MSG_ERROR([invalid-msvcrt])
    ;;
  esac],
  [sim_ac_msvcrt=singlethread-static])

AC_MSG_CHECKING([MSVC++ C library choice])
AC_MSG_RESULT([$sim_ac_msvcrt])

$1
]) # SIM_AC_SETUP_MSVCRT

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

#SoQt


# Helper macros for the SIM_AC_CHECK_QT macro below.

# SIM_AC_WITH_QT
#
# Sets sim_ac_with_qt (from --with-qt=[true|false]) and
# sim_ac_qtdir (from either --with-qt=DIR or $QTDIR).

AC_DEFUN([SIM_AC_WITH_QT], [
sim_ac_qtdir=
AC_ARG_WITH(
  [qt],
  AC_HELP_STRING([--with-qt=[DIR]],
                 [specify the location of the Qt library]),
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
    AC_PATH_PROG(sim_ac_qt_cygpath, cygpath, false)
    if test $sim_ac_qt_cygpath = "false"; then
      sim_ac_qtdir=$QTDIR
    else
      # Quote $QTDIR in case it's empty.
      sim_ac_qtdir=`$sim_ac_qt_cygpath -u "$QTDIR"`
    fi

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
    if test -z "$QTDIR"; then
      AC_MSG_WARN([QTDIR environment variable not set -- this might be an indication of a problem])
    fi
    AC_MSG_WARN([the ``$2'' Qt pre-processor tool not found])
  fi
else
  AC_SUBST([$1], [false])
fi
])

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

AC_ARG_ENABLE(
  [qt-debug],
  AC_HELP_STRING([--enable-qt-debug], [win32: link with debug versions of Qt libraries]),
  [case $enableval in
  yes | true ) sim_ac_qt_debug=true ;;
  *) sim_ac_qt_debug=false ;;
  esac],
  [sim_ac_qt_debug=false])

sim_ac_qt_avail=no

if $sim_ac_with_qt; then

  sim_ac_save_cppflags=$CPPFLAGS
  sim_ac_save_ldflags=$LDFLAGS
  sim_ac_save_libs=$LIBS

  if test -n "$sim_ac_qtdir"; then
    sim_ac_qt_incpath="-I$sim_ac_qtdir/include"
    sim_ac_qt_ldflags="-L$sim_ac_qtdir/lib"
  fi

  CPPFLAGS="$sim_ac_qt_incpath $CPPFLAGS"
  LDFLAGS="$LDFLAGS $sim_ac_qt_ldflags"

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
    sim_ac_qt_cppflags="-I$sim_ac_qt_framework_dir/QtCore.framework/Headers -I$sim_ac_qt_framework_dir/QtOpenGL.framework/Headers -I$sim_ac_qt_framework_dir/QtGui.framework/Headers -I$sim_ac_qt_framework_dir/Qt3Support.framework/Headers -I$sim_ac_qt_framework_dir/QtNetwork.framework/Headers -F$sim_ac_qt_framework_dir"
    sim_ac_qt_libs="-Wl,-F$sim_ac_qt_framework_dir -Wl,-framework,QtGui -Wl,-framework,QtOpenGL -Wl,-framework,QtCore -Wl,-framework,Qt3Support -Wl,-framework,QtXml -Wl,-framework,QtNetwork -Wl,-framework,QtSql"

  else

  sim_ac_qglobal=false
  SIM_AC_CHECK_HEADER_SILENT([qglobal.h],
    [sim_ac_qglobal=true],
    [
     # Debian Linux and Darwin fink have the Qt-dev installation headers in 
     # a separate subdir, so we reset CPPFLAGS and try with those.
     CPPFLAGS="$sim_ac_save_cppflags"
     sim_ac_debian_qtheaders=/usr/include/qt
     if test -d $sim_ac_debian_qtheaders; then
       sim_ac_qt_incpath="-I$sim_ac_debian_qtheaders $sim_ac_qt_incpath"
       CPPFLAGS="-I$sim_ac_debian_qtheaders $CPPFLAGS"
       SIM_AC_CHECK_HEADER_SILENT([qglobal.h], [sim_ac_qglobal=true])
     else
       sim_ac_fink_qtheaders=/sw/include/qt
       if test -d $sim_ac_fink_qtheaders; then
         sim_ac_qt_incpath="-I$sim_ac_fink_qtheaders $sim_ac_qt_incpath"
         CPPFLAGS="-I$sim_ac_fink_qtheaders $CPPFLAGS"
         SIM_AC_CHECK_HEADER_SILENT([qglobal.h], [sim_ac_qglobal=true])
       else
       sim_ac_darwinports_qtheaders=/opt/local/include/qt3
         if test -d $sim_ac_darwinports_qtheaders; then
           sim_ac_qt_incpath="-I$sim_ac_darwinports_qtheaders $sim_ac_qt_incpath"
           sim_ac_qt_ldflags="-L/opt/local/lib $sim_ac_qt_ldflags"
           CPPFLAGS="-I$sim_ac_darwinports_qtheaders $CPPFLAGS"
           LDFLAGS="$LDFLAGS $sim_ac_qt_ldflags"
           SIM_AC_CHECK_HEADER_SILENT([qglobal.h], [sim_ac_qglobal=true])
         fi     
       fi
     fi])

  # Qt 4 has the headers in various new subdirectories vs Qt 3.
  if $sim_ac_qglobal; then :; else
    AC_MSG_CHECKING([if Qt4 include paths must be used])
    CPPFLAGS="$sim_ac_qt_incpath $sim_ac_qt_incpath/Qt $sim_ac_save_cppflags"
    SIM_AC_CHECK_HEADER_SILENT([qglobal.h],
                               [sim_ac_qglobal=true
                                sim_ac_qt_incpath="$sim_ac_qt_incpath $sim_ac_qt_incpath/Qt $sim_ac_qt_incpath/QtOpenGL $sim_ac_qt_incpath/QtCore $sim_ac_qt_incpath/QtGui $sim_ac_qt_incpath/QtNetwork $sim_ac_qt_incpath/QtXml $sim_ac_qt_incpath/QtTest $sim_ac_qt_incpath/Qt3Support"
                                ])
    AC_MSG_RESULT($sim_ac_qglobal)
  fi

  if $sim_ac_qglobal; then

    # Find version of the Qt library (MSWindows .dll is named with the
    # version number.)
    AC_MSG_CHECKING([version of Qt library])
    cat > conftest.c << EOF
#include <qglobal.h>
int VerQt = QT_VERSION;
EOF
    # The " *"-parts of the last sed-expression on the next line are necessary
    # because at least the Solaris/CC preprocessor adds extra spaces before and
    # after the trailing semicolon.
    sim_ac_qt_version=`$CXXCPP $CPPFLAGS conftest.c 2>/dev/null | grep '^int VerQt' | sed 's%^int VerQt = %%' | sed 's% *; *$%%'`

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

    if test $sim_ac_qt_version -lt 200; then
      SIM_AC_ERROR([too-old-qt])
    fi

    # Too hard to feature-check for the Qt-on-Mac problems, as they involve
    # obscure behavior of the QGLWidget -- so we just resort to do platform
    # and version checking instead.
    case $host_os in
    darwin*)
      if test $sim_ac_qt_version -lt 302; then
        SIM_AC_CONFIGURATION_WARNING([The version of Qt you are using is
known to contain some serious bugs on MacOS X. We strongly recommend you to
upgrade. (See $srcdir/README.MAC for details.)])
      fi

      if test x$sim_ac_enable_darwin_x11 = xfalse; then
      # Using Qt/X11 but option --enable-darwin-x11 not given
      AC_TRY_COMPILE([#include <qapplication.h>],
                  [#if defined(__APPLE__) && defined(Q_WS_X11)
                   #error blah!
                   #endif],[],
                  [SIM_AC_ERROR([x11-qt-but-no-x11-requested])])
      else 
      # --enable-darwin-x11 specified but attempting Qt/Mac linkage
      AC_TRY_COMPILE([#include <qapplication.h>],
                  [#if defined(__APPLE__) && defined(Q_WS_MAC)
                   #error blah!
                   #endif],[],
                  [SIM_AC_ERROR([mac-qt-but-x11-requested])])
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
      SIM_AC_CONFIGURATION_WARNING([The version of Qt you are compiling against
is known to contain bugs which influences functionality in SoQt. We strongly
recommend you to upgrade.])
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
        CPPFLAGS="$sim_ac_qt_incpath $sim_ac_qt_cppflags_loop $sim_ac_save_cppflags"
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
                     sim_ac_qt_cppflags="$sim_ac_qt_incpath $sim_ac_qt_cppflags_loop"])
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
      ##
      ## * "-lqt-gl": links against the standard Debian version of the
      ##   Qt library with embedded QGL
      ##
      ## * "-lqt": should work for most UNIX(-derived) platforms on
      ##   dynamic and static linking with the non-mtsafe library
      ##
      ## * "-lqt-mt": should work for most UNIX(-derived) platforms on
      ##   dynamic and static linking with the mtsafe library
      ##
      ## * "-lqt{version} -lqtmain -lgdi32": w/QT_DLL defined should
      ##   cover dynamic Enterprise Edition linking on Win32 platforms
      ##
      ## * "-lqt -lqtmain -lgdi32": ...unless the {version} suffix is missing,
      ##   which we've had reports about
      ##
      ## * "-lqt-mt{version} -lqtmain -lgdi32": w/QT_DLL defined should
      ##   cover dynamic multi-thread Enterprise Edition linking on Win32
      ##   platforms
      ##
      ## * "-lqt-mt{version}nc -lqtmain -lgdi32": w/QT_DLL defined should
      ##   cover dynamic Non-Commercial Edition linking on Win32 platforms
      ##
      ## * "-lqt -luser32 -lole32 -limm32 -lcomdlg32 -lgdi32": should cover
      ##   static linking on Win32 platforms
      ##
      ## * "-lqt-mt -luser32 -lole32 -limm32 -lcomdlg32 -lgdi32 -lwinspool -lwinmm -ladvapi32 -lws2_32":
      ##   added for the benefit of the Qt 3.0.0 Evaluation Version
      ##   (update: "advapi32.lib" seems to be a new dependency for Qt 3.1.0)
      ##   (update: "ws2_32.lib" seems to be a new dependency for Qt 3.1.2)
      ##
      ## * "-lqt-mt-eval": the Qt/Mac evaluation version
      ##
      ## * "-lqt-mtnc{version}": the non-commercial Qt version that
      ##   comes on the CD with the book "C++ Gui Programming with Qt 3"
      ##   (version==321 there)

      ## FIXME: could probably improve check to not have to go through
      ## all of the above. See bug item #028 in SoQt/BUGS.txt.
      ## 20040805 mortene.

#       sim_ac_qt_suffix=
#       if $sim_ac_qt_debug; then
#         sim_ac_qt_suffix=d
#       f

      # Enable Qt Debugging
      if test x"$debug" = x"true"; then
        sim_ac_qt_suffix=_debug
      fi

      # Note that we need to always check for -lqt-mt before -lqt, because
      # at least the most recent Debian platforms (as of 2003-02-20) comes
      # with a -lqt which is missing QGL support, while it also has a
      # -lqt-mt *with* QGL support. The reason for this is because the
      # default GL (Mesa) library on Debian is built in mt-safe mode,
      # so a non-mt-safe Qt can't use it.
#            "-lQtGui${sim_ac_qt_suffix}${sim_ac_qt_major_version} -lQtCore${sim_ac_qt_suffix}${sim_ac_qt_major_version} -lQt3Support${sim_ac_qt_suffix}${sim_ac_qt_major_version}" \

      for sim_ac_qt_cppflags_loop in "" "-DQT_DLL"; do
        for sim_ac_qt_libcheck in \
            "-lQtGui${sim_ac_qt_suffix}${sim_ac_qt_major_version} -lQtCore${sim_ac_qt_suffix}${sim_ac_qt_major_version} -lQt3Support${sim_ac_qt_suffix}${sim_ac_qt_major_version} -lQtOpenGL${sim_ac_qt_suffix}${sim_ac_qt_major_version} -lQtNetwork${sim_ac_qt_suffix}${sim_ac_qt_major_version}" \
            "-lQtGui${sim_ac_qt_suffix} -lQt3Support${sim_ac_qt_suffix} -lQtOpenGL${sim_ac_qt_suffix} -lQtNetwork${sim_ac_qt_suffix}" \
            "-lqt-gl" \
            "-lqt-mt" \
            "-lqt" \
            "-lqt-mt -luser32 -lole32 -limm32 -lcomdlg32 -lgdi32 -lwinspool -lwinmm -ladvapi32 -lws2_32" \
            "-lqt-mt${sim_ac_qt_version}${sim_ac_qt_suffix} -lqtmain -lgdi32" \
            "-lqt-mt${sim_ac_qt_version}nc${sim_ac_qt_suffix} -lqtmain -lgdi32" \
            "-lqt-mtedu${sim_ac_qt_version}${sim_ac_qt_suffix} -lqtmain -lgdi32" \
            "-lqt -lqtmain -lgdi32" \
            "-lqt${sim_ac_qt_version}${sim_ac_qt_suffix} -lqtmain -lgdi32" \
            "-lqt -luser32 -lole32 -limm32 -lcomdlg32 -lgdi32" \
            "-lqt-mt-eval" \
            "-lqt-mteval${sim_ac_qt_version}" \
            "-lqt-mtnc${sim_ac_qt_version}"
        do
          if test "x$sim_ac_qt_libs" = "xUNRESOLVED"; then
            CPPFLAGS="$sim_ac_qt_incpath $sim_ac_qt_cppflags_loop $sim_ac_save_cppflags"
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
                         sim_ac_qt_cppflags="$sim_ac_qt_incpath $sim_ac_qt_cppflags_loop"])
          fi
        done
      done

      AC_MSG_RESULT([yes])
#      AC_MSG_RESULT([$sim_ac_qt_cppflags $sim_ac_qt_ldflags $sim_ac_qt_libs])
    fi

  else # sim_ac_qglobal = false
    AC_MSG_WARN([header file qglobal.h not found, can not compile Qt code])
  fi

  fi # sim_ac_have_qt_framework

  # We should only *test* availability, not mutate the LIBS/CPPFLAGS
  # variables ourselves inside this macro. 20041021 larsa
  CPPFLAGS=$sim_ac_save_cppflags
  LDFLAGS=$sim_ac_save_ldflags
  LIBS=$sim_ac_save_libs
  if test ! x"$sim_ac_qt_libs" = xUNRESOLVED; then
    sim_ac_qt_avail=yes
    #CPPFLAGS="$sim_ac_qt_cppflags $sim_ac_save_cppflags"
    #LIBS="$sim_ac_qt_libs $sim_ac_save_libs"
    $1

    sim_ac_qt_install=`cd $sim_ac_qtdir; pwd`/bin/install
    AC_MSG_CHECKING([whether Qt's install tool shadows the system install])
    case $INSTALL in
    "${sim_ac_qt_install}"* )
      AC_MSG_RESULT(yes)
      SIM_AC_ERROR([qt-install])
      ;;
    * )
      AC_MSG_RESULT(no)
      ;;
    esac

  else
    if test -z "$QTDIR"; then
      AC_MSG_WARN([QTDIR environment variable not set -- this might be an indication of a problem])
    fi
    $2
  fi
fi
])


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

# **************************************************************************
# SIM_AC_HAVE_LIBJPEG_IFELSE( IF-FOUND, IF-NOT-FOUND )
#
# Variables:
#   sim_ac_have_libjpeg
#   sim_ac_libjpeg_cppflags
#   sim_ac_libjpeg_ldflags
#   sim_ac_libjpeg_libs
#
# Authors:
#   Lars J. Aas <larsa@coin3d.org>
#   Morten Eriksen <mortene@coin3d.org>
#
# Todo:
# - use AS_UNSET to unset internal variables to avoid polluting the environment
#

# **************************************************************************

AC_DEFUN([SIM_AC_HAVE_LIBJPEG_IFELSE],
[: ${sim_ac_have_libjpeg=false}
AC_MSG_CHECKING([for libjpeg])
AC_ARG_WITH(
  [jpeg],
  [AC_HELP_STRING([--with-jpeg=PATH], [enable/disable libjpeg support])],
  [case $withval in
  yes | "") sim_ac_want_libjpeg=true ;;
  no)       sim_ac_want_libjpeg=false ;;
  *)        sim_ac_want_libjpeg=true
            sim_ac_libjpeg_path=$withval ;;
  esac],
  [sim_ac_want_libjpeg=true])
case $sim_ac_want_libjpeg in
true)
  $sim_ac_have_libjpeg && break
  sim_ac_libjpeg_save_CPPFLAGS=$CPPFLAGS
  sim_ac_libjpeg_save_LDFLAGS=$LDFLAGS
  sim_ac_libjpeg_save_LIBS=$LIBS
  sim_ac_libjpeg_debug=false
  test -n "`echo -- $CPPFLAGS $CFLAGS $CXXFLAGS | grep -- '-g\\>'`" &&
    sim_ac_libjpeg_debug=true
  # test -z "$sim_ac_libjpeg_path" -a x"$prefix" != xNONE &&
  #   sim_ac_libjpeg_path=$prefix
  sim_ac_libjpeg_name=jpeg
  if test -n "$sim_ac_libjpeg_path"; then
    for sim_ac_libjpeg_candidate in \
      `( ls $sim_ac_libjpeg_path/lib/jpeg*.lib;
         ls $sim_ac_libjpeg_path/lib/jpeg*d.lib ) 2>/dev/null`
    do
      case $sim_ac_libjpeg_candidate in
      *d.lib)
        $sim_ac_libjpeg_debug &&
          sim_ac_libjpeg_name=`basename $sim_ac_libjpeg_candidate .lib` ;;
      *.lib)
        sim_ac_libjpeg_name=`basename $sim_ac_libjpeg_candidate .lib` ;;
      esac
    done
    sim_ac_libjpeg_cppflags="-I$sim_ac_libjpeg_path/include"
    CPPFLAGS="$CPPFLAGS $sim_ac_libjpeg_cppflags"
    sim_ac_libjpeg_ldflags="-L$sim_ac_libjpeg_path/lib"
    LDFLAGS="$LDFLAGS $sim_ac_libjpeg_ldflags"
    # unset sim_ac_libjpeg_candidate
    # unset sim_ac_libjpeg_path
  fi
  sim_ac_libjpeg_libs="-l$sim_ac_libjpeg_name"
  LIBS="$sim_ac_libjpeg_libs $LIBS"
  AC_TRY_LINK(
    [#include <stdio.h>
#ifdef __cplusplus
extern "C" { // libjpeg header is missing the C++ wrapper
#endif
#include <jpeglib.h>
#ifdef __cplusplus
}
#endif],
  [(void)jpeg_read_header(0L, 0);],
  [sim_ac_have_libjpeg=true])
  CPPFLAGS=$sim_ac_libjpeg_save_CPPFLAGS
  LDFLAGS=$sim_ac_libjpeg_save_LDFLAGS
  LIBS=$sim_ac_libjpeg_save_LIBS
  # unset sim_ac_libjpeg_debug
  # unset sim_ac_libjpeg_name
  # unset sim_ac_libjpeg_save_CPPFLAGS
  # unset sim_ac_libjpeg_save_LDFLAGS
  # unset sim_ac_libjpeg_save_LIBS
  ;;
esac
if $sim_ac_want_libjpeg; then
  if $sim_ac_have_libjpeg; then
    AC_MSG_RESULT([success ($sim_ac_libjpeg_libs)])
    $1
  else
    AC_MSG_RESULT([failure])
    $2
  fi
else
  AC_MSG_RESULT([disabled])
  $2
fi
# unset sim_ac_want_libjpeg
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

dnl @synopsis AX_SPLIT_VERSION
dnl
dnl Splits a version number in the format MAJOR.MINOR.POINT into it's
dnl separeate components.
dnl
dnl Sets the variables.
dnl
dnl @category Automake
dnl @author Tom Howard <tomhoward@users.sf.net>
dnl @version 2005-01-14
dnl @license AllPermissive

AC_DEFUN([AX_SPLIT_VERSION],[
    AX_MAJOR_VERSION=`echo "$VERSION" | sed 's/\([[^.]][[^.]]*\).*/\1/'`
    AX_MINOR_VERSION=`echo "$VERSION" | sed 's/[[^.]][[^.]]*.\([[^.]][[^.]]*\).*/\1/'`
    AC_SUBST([AX_MAJOR_VERSION])
    AC_SUBST([AX_MINOR_VERSION])

    AX_LIB_MAJOR_VERSION=`echo "$LIB_VERSION" | sed 's/\([[^.]][[^.]]*\).*/\1/'`
    AX_LIB_MINOR_VERSION=`echo "$LIB_VERSION" | sed 's/[[^.]][[^.]]*.\([[^.]][[^.]]*\).*/\1/'`
    AX_LIB_POINT_VERSION=`echo "$LIB_VERSION" | sed 's/[[^.]][[^.]]*.[[^.]][[^.]]*.\(.*\)/\1/'`
    AC_SUBST([AX_LIB_MAJOR_VERSION])
    AC_SUBST([AX_LIB_MINOR_VERSION])
    AC_SUBST([AX_LIB_POINT_VERSION])
])


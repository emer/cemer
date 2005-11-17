#					 PDP_DETERMINE_OSTYPE
#*************************************************************
# Compiler and linker flags, includes and defines are set
# based on the results of these tests.
#*************************************************************
#
AC_DEFUN([PDP_DETERMINE_OSTYPE],[
AC_REQUIRE([AC_CANONICAL_BUILD])
AC_MSG_CHECKING([results of config.guess])
case "${build_os}" in
	*linux*)
		AC_SUBST([PDP_PLATFORM],[LINUX])
		AC_DEFINE([LINUX],[1],[When on linux])
	;;
	*darwin*)
		AC_SUBST([PDP_PLATFORM],[DARWIN])
		AC_DEFINE([DARWIN],[1],[When on darwin])
		AC_DEFINE([LINUX],[1],[When on darwin])
	;;
	*cygwin*)
		AC_SUBST([PDP_PLATFORM],[CYGWIN])
		AC_DEFINE([CYGWIN],[1],[When on cygwin])
		AC_DEFINE([LINUX],[1],[When on cygwin])
	;;
	*)
		AC_MSG_ERROR([PDP++ supports Linux, Darwin, and Windows],[1])
	;;
esac
AC_MSG_RESULT([$PDP_PLATFORM])
AM_CONDITIONAL([LINUX],[test $PDP_PLATFORM = LINUX])
AM_CONDITIONAL([DARWIN],[test $PDP_PLATFORM = DARWIN])
AM_CONDITIONAL([CYGWIN],[test $PDP_PLATFORM = CYGWIN])
]) #PDP_DETERMINE_OSTYPE

#					 PDP_DETERMINE_SUFFIX
#*************************************************************
# Adds configure flag dependent suffixes. E.g., 
# bp_nogui_debug_mpi++
#*************************************************************
#
AC_DEFUN([PDP_DETERMINE_SUFFIX],[
AC_MSG_CHECKING([executeable suffixes])
if test "$gui" = "false" ; then
	PDP_SUFFIX="${PDP_SUFFIX}_nogui"
fi
if test "$debug" = "true" ; then
	PDP_SUFFIX="${PDP_SUFFIX}_debug"
fi
if test "$mpi" = "true"; then
	PDP_SUFFIX="${PDP_SUFFIX}_mpi"
fi
AC_MSG_RESULT([$PDP_SUFFIX])
AC_SUBST([PDP_SUFFIX])
]) #PDP_DETERMINE_SUFFIX

#					      VL_LIB_READLINE
#*************************************************************
#
# Searches for a readline compatible library. If found, defines 
# `HAVE_LIBREADLINE'. If the found library has the `add_history'
# function, sets also `HAVE_READLINE_HISTORY'. Also checks for 
# the locations of the necessary include files and sets 
# `HAVE_READLINE_H' or `HAVE_READLINE_READLINE_H' and 
# `HAVE_READLINE_HISTORY_H' or 'HAVE_HISTORY_H' if the 
# corresponding include files exists.
#
# The libraries that may be readline compatible are `libedit', 
# `libeditline' and `libreadline'. 
# Sometimes we need to link a termcap library for readline to 
# work, this macro tests these cases too by trying to link with 
# `libtermcap', `libcurses' or `libncurses' before giving up. 
#
# See: http://autoconf-archive.cryp.to/vl_lib_readline.html
#
#*************************************************************
#
AC_DEFUN([VL_LIB_READLINE], [
  AC_CACHE_CHECK([for a readline compatible library],
		 vl_cv_lib_readline, [
    ORIG_LIBS="$LIBS"
    for readline_lib in readline edit editline; do
      for termcap_lib in "" termcap curses ncurses; do
	if test -z "$termcap_lib"; then
	  TRY_LIB="-l$readline_lib"
	else
	  TRY_LIB="-l$readline_lib -l$termcap_lib"
	fi
	LIBS="$ORIG_LIBS $TRY_LIB"
	AC_TRY_LINK_FUNC(readline, vl_cv_lib_readline="$TRY_LIB")
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
    fi
  ])
  if test "$vl_cv_lib_readline" != "no"; then
    AC_DEFINE(HAVE_LIBREADLINE, 1,
	      [Define if you have a readline compatible library])
    AC_CHECK_HEADERS(readline.h readline/readline.h)
  else
    AC_MSG_ERROR([Unable to find a readline compatible library.])
  fi
])


#					       CHECK_GNU_MAKE
#*************************************************************
#
# This macro searches for a GNU version of make. If a match is
# found, the makefile variable 'GNUmake' is set to 'true'
# otherwise it is set to 'false. This is useful for 
# including a special features in a Makefile, which cannot be 
# handled by other versions of make. The variable 
# _cv_gnu_make_command is set to the command to invoke 
# GNU make if it exists, the empty string otherwise.
#
# See: http://autoconf-archive.cryp.to/check_gnu_make.html
#
#*************************************************************
#

AC_DEFUN([CHECK_GNU_MAKE],[
AC_MSG_CHECKING([for GNU Make])
for a in "$MAKE" make gmake gnumake ; do
	if test -z "$a"; then
	    continue
	fi
	if  ( sh -c "$a --version" 2> /dev/null | grep GNU  2>&1 > /dev/null ); then
		AC_MSG_RESULT([$a])
		break 2
	else
		AC_MSG_ERROR([GNU Make not found. % extensions may fail. Considering uncommenting code in Moc.am])
	fi
done
]) #CHECK_GNU_MAKE


#	    ACX_MPI([ACTION-IF-FOUND[, ACTION-IF-NOT-FOUND]])
#*************************************************************
#
#  This macro tries to find out how to compile programs that
#  use MPI (Message Passing Interface), a standard API for
#  parallel process communication (see http://www-unix.mcs.anl.gov/mpi/)
# 
#  On success, it sets the MPICC, MPICXX, or MPIF90 output variable to
#  the name of the MPI compiler, depending upon the current language.
#  (This may just be $CC/$CXX/$F90, but is more often something like
#  mpicc/mpiCC/mpif90.)  It also sets MPILIBS to any libraries that are
#  needed for linking MPI (e.g. -lmpi, if a special MPICC/MPICXX/MPIF90
#  was not found).
# 
#  If you want to compile everything with MPI, you should set:
# 
#      CC="$MPICC" #OR# CXX="$MPICXX" #OR# F90="$MPIF90"
#      LIBS="$MPILIBS $LIBS"
# 
#  The user can force a particular library/compiler by setting the
#  MPICC/MPICXX/MPIF90 and/or MPILIBS environment variables.
# 
#  ACTION-IF-FOUND is a list of shell commands to run if an MPI
#  library is found, and ACTION-IF-NOT-FOUND is a list of commands
#  to run it if it is not found.  If ACTION-IF-FOUND is not specified,
#  the default action will define HAVE_MPI.
# 
#  @version $Id$
#  @author Steven G. Johnson <stevenj@alum.mit.edu>
#
#*************************************************************
#
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

# Finally, execute ACTION-IF-FOUND/ACTION-IF-NOT-FOUND:
if test x = x"$MPILIBS"; then
	$2
	:
else
	ifelse([$1],,[AC_DEFINE(HAVE_MPI,1,[Define if you have the MPI library.])],[$1])
	:
fi
])dnl ACX_MPI

AC_DEFUN([ACX_PTHREAD], [
AC_REQUIRE([AC_CANONICAL_HOST])
AC_LANG_SAVE
AC_LANG_C
acx_pthread_ok=no

# We used to check for pthread.h first, but this fails if pthread.h
# requires special compiler flags (e.g. on True64 or Sequent).
# It gets checked for in the link test anyway.

# First of all, check if the user has set any of the PTHREAD_LIBS,
# etcetera environment variables, and if threads linking works using
# them:
if test x"$PTHREAD_LIBS$PTHREAD_CFLAGS" != x; then
	save_CFLAGS="$CFLAGS"
	CFLAGS="$CFLAGS $PTHREAD_CFLAGS"
	save_LIBS="$LIBS"
	LIBS="$PTHREAD_LIBS $LIBS"
	AC_MSG_CHECKING([for pthread_join in LIBS=$PTHREAD_LIBS with CFLAGS=$PTHREAD_CFLAGS])
	AC_TRY_LINK_FUNC(pthread_join, acx_pthread_ok=yes)
	AC_MSG_RESULT($acx_pthread_ok)
	if test x"$acx_pthread_ok" = xno; then
		PTHREAD_LIBS=""
		PTHREAD_CFLAGS=""
	fi
	LIBS="$save_LIBS"
	CFLAGS="$save_CFLAGS"
fi


#	ACX_PTHREAD([ACTION-IF-FOUND[, ACTION-IF-NOT-FOUND]])
#*************************************************************
# http://autoconf-archive.cryp.to/acx_pthread.html
#*************************************************************
# This macro figures out how to build C programs using POSIX threads. 
# It sets the PTHREAD_LIBS output variable to the threads library 
# and linker flags, and the PTHREAD_CFLAGS output variable to any 
# special C compiler flags that are needed. (The user can also force 
# certain compiler flags/libs to be tested by setting these 
# environment variables.)
#
#  Also sets PTHREAD_CC to any special C compiler that is needed 
# for multi-threaded programs (defaults to the value of CC otherwise).
# (This is necessary on AIX to use the special cc_r compiler alias.)
#
# NOTE: You are assumed to not only compile your program with these 
# flags, but also link it with them as well. e.g. you should link 
# with $PTHREAD_CC $CFLAGS $PTHREAD_CFLAGS $LDFLAGS ... $PTHREAD_LIBS $LIBS
#
# If you are only building threads programs, you may wish to use these 
# variables in your default LIBS, CFLAGS, and CC:
#
#       LIBS="$PTHREAD_LIBS $LIBS"
#       CFLAGS="$CFLAGS $PTHREAD_CFLAGS"
#       CC="$PTHREAD_CC"
#
# In addition, if the PTHREAD_CREATE_JOINABLE thread-attribute constant 
# has a nonstandard name, defines PTHREAD_CREATE_JOINABLE to that name 
# (e.g. PTHREAD_CREATE_UNDETACHED on AIX).
#
# ACTION-IF-FOUND is a list of shell commands to run if a threads library 
# is found, and ACTION-IF-NOT-FOUND is a list of commands to run it if 
# it is not found. If ACTION-IF-FOUND is not specified, the default action 
# will define HAVE_PTHREAD.
# Please let the authors know if this macro fails on any platform, 
# or if you have any other suggestions or comments. This macro was based 
# on work by SGJ on autoconf scripts for FFTW (www.fftw.org) 
# (with help from M. Frigo), as well as ac_pthread and hb_pthread 
# macros posted by Alejandro Forero Cuervo to the autoconf macro 
# repository. We are also grateful for the helpful feedback of numerous users.

#*************************************************************
# We must check for the threads library under a number of different
# names; the ordering is very important because some systems
# (e.g. DEC) have both -lpthread and -lpthreads, where one of the
# libraries is broken (non-POSIX).

# Create a list of thread flags to try.  Items starting with a "-" are
# C compiler flags, and other items are library names, except for "none"
# which indicates that we try without any flags at all, and "pthread-config"
# which is a program returning the flags for the Pth emulation library.

acx_pthread_flags="pthreads none -Kthread -kthread lthread -pthread -pthreads -mthreads pthread --thread-safe -mt pthread-config"

# The ordering *is* (sometimes) important.  Some notes on the
# individual items follow:

# pthreads: AIX (must check this before -lpthread)
# none: in case threads are in libc; should be tried before -Kthread and
#       other compiler flags to prevent continual compiler warnings
# -Kthread: Sequent (threads in libc, but -Kthread needed for pthread.h)
# -kthread: FreeBSD kernel threads (preferred to -pthread since SMP-able)
# lthread: LinuxThreads port on FreeBSD (also preferred to -pthread)
# -pthread: Linux/gcc (kernel threads), BSD/gcc (userland threads)
# -pthreads: Solaris/gcc
# -mthreads: Mingw32/gcc, Lynx/gcc
# -mt: Sun Workshop C (may only link SunOS threads [-lthread], but it
#      doesn't hurt to check since this sometimes defines pthreads too;
#      also defines -D_REENTRANT)
# pthread: Linux, etcetera
# --thread-safe: KAI C++
# pthread-config: use pthread-config program (for GNU Pth library)

case "${host_cpu}-${host_os}" in
	*solaris*)

	# On Solaris (at least, for some versions), libc contains stubbed
	# (non-functional) versions of the pthreads routines, so link-based
	# tests will erroneously succeed.  (We need to link with -pthread or
	# -lpthread.)  (The stubs are missing pthread_cleanup_push, or rather
	# a function called by this macro, so we could check for that, but
	# who knows whether they'll stub that too in a future libc.)  So,
	# we'll just look for -pthreads and -lpthread first:

	acx_pthread_flags="-pthread -pthreads pthread -mt $acx_pthread_flags"
	;;
esac

if test x"$acx_pthread_ok" = xno; then
for flag in $acx_pthread_flags; do

	case $flag in
		none)
		AC_MSG_CHECKING([whether pthreads work without any flags])
		;;

		-*)
		AC_MSG_CHECKING([whether pthreads work with $flag])
		PTHREAD_CFLAGS="$flag"
		;;

		pthread-config)
		AC_CHECK_PROG(acx_pthread_config, pthread-config, yes, no)
		if test x"$acx_pthread_config" = xno; then continue; fi
		PTHREAD_CFLAGS="`pthread-config --cflags`"
		PTHREAD_LIBS="`pthread-config --ldflags` `pthread-config --libs`"
		;;

		*)
		AC_MSG_CHECKING([for the pthreads library -l$flag])
		PTHREAD_LIBS="-l$flag"
		;;
	esac

	save_LIBS="$LIBS"
	save_CFLAGS="$CFLAGS"
	LIBS="$PTHREAD_LIBS $LIBS"
	CFLAGS="$CFLAGS $PTHREAD_CFLAGS"

	# Check for various functions.  We must include pthread.h,
	# since some functions may be macros.  (On the Sequent, we
	# need a special flag -Kthread to make this header compile.)
	# We check for pthread_join because it is in -lpthread on IRIX
	# while pthread_create is in libc.  We check for pthread_attr_init
	# due to DEC craziness with -lpthreads.  We check for
	# pthread_cleanup_push because it is one of the few pthread
	# functions on Solaris that doesn't have a non-functional libc stub.
	# We try pthread_create on general principles.
	AC_TRY_LINK([#include <pthread.h>],
		    [pthread_t th; pthread_join(th, 0);
		     pthread_attr_init(0); pthread_cleanup_push(0, 0);
		     pthread_create(0,0,0,0); pthread_cleanup_pop(0); ],
		    [acx_pthread_ok=yes])

	LIBS="$save_LIBS"
	CFLAGS="$save_CFLAGS"

	AC_MSG_RESULT($acx_pthread_ok)
	if test "x$acx_pthread_ok" = xyes; then
		break;
	fi

	PTHREAD_LIBS=""
	PTHREAD_CFLAGS=""
done
fi

# Various other checks:
if test "x$acx_pthread_ok" = xyes; then
	save_LIBS="$LIBS"
	LIBS="$PTHREAD_LIBS $LIBS"
	save_CFLAGS="$CFLAGS"
	CFLAGS="$CFLAGS $PTHREAD_CFLAGS"

	# Detect AIX lossage: JOINABLE attribute is called UNDETACHED.
	AC_MSG_CHECKING([for joinable pthread attribute])
	attr_name=unknown
	for attr in PTHREAD_CREATE_JOINABLE PTHREAD_CREATE_UNDETACHED; do
	    AC_TRY_LINK([#include <pthread.h>], [int attr=$attr;],
			[attr_name=$attr; break])
	done
	AC_MSG_RESULT($attr_name)
	if test "$attr_name" != PTHREAD_CREATE_JOINABLE; then
	    AC_DEFINE_UNQUOTED(PTHREAD_CREATE_JOINABLE, $attr_name,
			       [Define to necessary symbol if this constant
				uses a non-standard name on your system.])
	fi

	AC_MSG_CHECKING([if more special flags are required for pthreads])
	flag=no
	case "${host_cpu}-${host_os}" in
	    *-aix* | *-freebsd* | *-darwin*) flag="-D_THREAD_SAFE";AC_DEFINE([_THREAD_SAFE],[1],[Special flags for pthreads on aix, freebsd, darwin]);;
	    *solaris* | *-osf* | *-hpux*) flag="-D_REENTRANT"; AC_DEFINE([_REENTRANT],[1],[Special flags for pthreads on solaris, osf, hpux]);;
	esac
	AC_MSG_RESULT(${flag})
	LIBS="$save_LIBS"
	CFLAGS="$save_CFLAGS"

	# More AIX lossage: must compile with cc_r
	AC_CHECK_PROG(PTHREAD_CC, cc_r, cc_r, ${CC})
else
	PTHREAD_CC="$CC"
fi

AC_SUBST(PTHREAD_LIBS)
AC_SUBST(PTHREAD_CFLAGS)
AC_SUBST(PTHREAD_CC)

# Finally, execute ACTION-IF-FOUND/ACTION-IF-NOT-FOUND:
if test x"$acx_pthread_ok" = xyes; then
	ifelse([$1],,AC_DEFINE(HAVE_PTHREAD,1,[Define if you have POSIX threads libraries and header files.]),[$1])
	:
else
	acx_pthread_ok=no
	$2
fi
AC_LANG_RESTORE
])dnl ACX_PTHREAD

#					     AX_INSTALL_FILES
#*************************************************************
#    http://autoconf-archive.cryp.to/ax_dist_rpm.html
#*************************************************************
# Adds target for creating a install_files file, which contains the list of files that will be installed.
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

#				       AX_ADD_AM_MACRO([RULE])
#*************************************************************
#    http://autoconf-archive.cryp.to/ax_add_recursive_am_macro.html
#*************************************************************
#  Adds the specified rule to $AMINCLUDE

AC_DEFUN([AX_ADD_AM_MACRO],[
  AC_REQUIRE([AX_AM_MACROS])
  AX_APPEND_TO_FILE([$AMINCLUDE],[$1])
])

#				 AX_APPEND_TO_FILE([FILE],[DATA])
#*************************************************************
#    http://autoconf-archive.cryp.to/ax_append_to_file.html
#*************************************************************
#  Appends the specified data to the specified file.

AC_DEFUN([AX_APPEND_TO_FILE],[
AC_REQUIRE([AX_FILE_ESCAPES])
printf "$2" >> "$1"
])

#		    AX_ADD_RECURSIVE_AM_MACRO([TARGET],[RULE])
#*************************************************************
#    http://autoconf-archive.cryp.to/ax_add_am_macro.html
#*************************************************************
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

#						 AX_AM_MACROS
#*************************************************************
#    http://autoconf-archive.cryp.to/ax_am_macros.html
#*************************************************************
#   Adds support for macros that create automake rules. You must manually add @INC_AMINCLUDE@  to your Makefile.am files.

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

#					       AX_FILE_ESCAPES
#*************************************************************
#    http://autoconf-archive.cryp.to/ax_file_escapes.html
#*************************************************************
#  Writes the specified data to the specified file.

AC_DEFUN([AX_FILE_ESCAPES],[
AX_DOLLAR="\$"
AX_SRB="\\135"
AX_SLB="\\133"
AX_BS="\\\\"
AX_DQ="\""
])

#						 AX_EXTRA_DIST
#*************************************************************
#    http://autoconf-archive.cryp.to/ax_extra_dist.html
#*************************************************************
#  Allow support for custom dist targets.

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

#					   AX_DIST_RPM([SPEC])
#*************************************************************
#    http://autoconf-archive.cryp.to/ax_dist_rpm.html
#*************************************************************
# Adds support for a rpm dist target.

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
	AC_MSG_ERROR([rpm spec template "$1.in" could not be found])
    fi
else
    AC_MSG_NOTICE([rpm support disabled... install_files not available])
fi
])

dnl @synopsis AX_CHECK_GL
dnl
dnl Check for an OpenGL implementation. If GL is found, the required
dnl compiler and linker flags are included in the output variables
dnl "GL_CFLAGS" and "GL_LIBS", respectively. This macro adds the
dnl configure option "--with-apple-opengl-framework", which users can
dnl use to indicate that Apple's OpenGL framework should be used on Mac
dnl OS X. If Apple's OpenGL framework is used, the symbol
dnl "HAVE_APPLE_OPENGL_FRAMEWORK" is defined. If no GL implementation
dnl is found, "no_gl" is set to "yes".
dnl
dnl @category InstalledPackages
dnl @author Braden McDaniel <braden@endoframe.com>
dnl @version 2004-11-15
dnl @license AllPermissive

AC_DEFUN([AX_CHECK_GL],
[AC_REQUIRE([AC_PATH_X])dnl
AC_REQUIRE([ACX_PTHREAD])dnl

#
# There isn't a reliable way to know we should use the Apple OpenGL framework
# without a configure option.  A Mac OS X user may have installed an
# alternative GL implementation (e.g., Mesa), which may or may not depend on X.
#
AC_ARG_WITH([apple-opengl-framework],
	    [AC_HELP_STRING([--with-apple-opengl-framework],
			    [use Apple OpenGL framework (Mac OS X only)])])
if test "X$with_apple_opengl_framework" = "Xyes"; then
  AC_DEFINE([HAVE_APPLE_OPENGL_FRAMEWORK], [1],
	    [Use the Apple OpenGL framework.])
  GL_LIBS="-framework OpenGL"
else
  AC_LANG_PUSH(C++)

  AX_LANG_COMPILER_MS
  if test X$ax_compiler_ms = Xno; then
    GL_CFLAGS="${PTHREAD_CFLAGS}"
    GL_LIBS="${PTHREAD_LIBS} -lm"
  fi

  #
  # Use x_includes and x_libraries if they have been set (presumably by
  # AC_PATH_X).
  #
  if test "X$no_x" != "Xyes"; then
    if test -n "$x_includes"; then
      GL_CFLAGS="-I${x_includes} ${GL_CFLAGS}"
    fi
    if test -n "$x_libraries"; then
      GL_LIBS="-L${x_libraries} -lX11 ${GL_LIBS}"
    fi
  fi

  AC_CHECK_HEADERS([windows.h])

  AC_CACHE_CHECK([for OpenGL library], [ax_cv_check_gl_libgl],
  [ax_cv_check_gl_libgl="no"
  ax_save_CPPFLAGS="${CPPFLAGS}"
  CPPFLAGS="${GL_CFLAGS} ${CPPFLAGS}"
  ax_save_LIBS="${LIBS}"
  LIBS=""
  ax_check_libs="-lopengl32 -lGL"
  for ax_lib in ${ax_check_libs}; do
    if test X$ax_compiler_ms = Xyes; then
      ax_try_lib=`echo $ax_lib | sed -e 's/^-l//' -e 's/$/.lib/'`
    else
      ax_try_lib="${ax_lib}"
    fi
    LIBS="${ax_try_lib} ${GL_LIBS} ${ax_save_LIBS}"
    AC_LINK_IFELSE(
    [AC_LANG_PROGRAM([[
# if HAVE_WINDOWS_H && defined(_WIN32)
#   include <windows.h>
# endif
# include <GL/gl.h>]],
		     [[glBegin(0)]])],
    [ax_cv_check_gl_libgl="${ax_try_lib}"; break])
  done
  LIBS=${ax_save_LIBS}
  CPPFLAGS=${ax_save_CPPFLAGS}])

  if test "X${ax_cv_check_gl_libgl}" = "Xno"; then
    no_gl="yes"
    GL_CFLAGS=""
    GL_LIBS=""
  else
    GL_LIBS="${ax_cv_check_gl_libgl} ${GL_LIBS}"
  fi
  AC_LANG_POP([C++])
fi

LIBS="$LIBS $GL_LIBS"
CXXFLAGS="$CXXFLAGS $GLCFLAGS"
AC_SUBST([GL_CFLAGS])
AC_SUBST([GL_LIBS])
])dnl

dnl						      CHECK_ZLIB
dnl *************************************************************
dnl	http://autoconf-archive.cryp.to/check_zlib.html
dnl *************************************************************
dnl @synopsis CHECK_ZLIB()
dnl
dnl This macro searches for an installed zlib library. If nothing was
dnl specified when calling configure, it searches first in /usr/local
dnl and then in /usr. If the --with-zlib=DIR is specified, it will try
dnl to find it in DIR/include/zlib.h and DIR/lib/libz.a. If
dnl --without-zlib is specified, the library is not searched at all.
dnl
dnl If either the header file (zlib.h) or the library (libz) is not
dnl found, the configuration exits on error, asking for a valid zlib
dnl installation directory or --without-zlib.
dnl
dnl The macro defines the symbol HAVE_LIBZ if the library is found. You
dnl should use autoheader to include a definition for this symbol in a
dnl config.h file. Sample usage in a C/C++ source is as follows:
dnl
dnl   #ifdef HAVE_LIBZ
dnl   #include <zlib.h>
dnl   #endif /* HAVE_LIBZ */
dnl
dnl @category InstalledPackages
dnl @author Loic Dachary <loic@senga.org>
dnl @version 2004-09-20
dnl @license GPLWithACException

AC_DEFUN([CHECK_ZLIB],
#
# Handle user hints
#
[AC_MSG_CHECKING(if zlib is wanted)
AC_ARG_WITH(zlib,
[  --with-zlib=DIR root directory path of zlib installation [defaults to
		    /usr/local or /usr if not found in /usr/local]
  --without-zlib to disable zlib usage completely],
[if test "$withval" != no ; then
  AC_MSG_RESULT(yes)
  if test -d "$withval"
  then
    ZLIB_HOME="$withval"
  else
    AC_MSG_NOTICE([$withval does not exist, checking usual places])
  fi
else
  AC_MSG_RESULT(no)
fi])

ZLIB_HOME=/usr/local
if test ! -f "${ZLIB_HOME}/include/zlib.h"
then
	ZLIB_HOME=/usr
fi

#
# Locate zlib, if wanted
#
if test -n "${ZLIB_HOME}"
then
	ZLIB_OLD_LDFLAGS=$LDFLAGS
	ZLIB_OLD_CPPFLAGS=$LDFLAGS
	LDFLAGS="$LDFLAGS -L${ZLIB_HOME}/lib"
	CPPFLAGS="$CPPFLAGS -I${ZLIB_HOME}/include"
	AC_LANG_SAVE
	AC_LANG_C
	AC_CHECK_LIB(z, inflateEnd, [zlib_cv_libz=yes], [zlib_cv_libz=no])
	AC_CHECK_HEADER(zlib.h, [zlib_cv_zlib_h=yes], [zlib_cv_zlib_h=no])
	AC_LANG_RESTORE
	if test "$zlib_cv_libz" = "yes" -a "$zlib_cv_zlib_h" = "yes"
	then
		#
		# If both library and header were found, use them
		#
		AC_CHECK_LIB(z, inflateEnd)
		AC_MSG_CHECKING(zlib in ${ZLIB_HOME})
		AC_MSG_RESULT(ok)
	else
		#
		# If either header or library was not found, revert and bomb
		#
		AC_MSG_CHECKING(zlib in ${ZLIB_HOME})
		LDFLAGS="$ZLIB_OLD_LDFLAGS"
		CPPFLAGS="$ZLIB_OLD_CPPFLAGS"
		AC_MSG_RESULT(failed)
		AC_MSG_ERROR([either specify a valid zlib installation with --with-zlib=DIR or disable zlib usage with --without-zlib])
	fi
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

dnl @synopsis BNV_HAVE_QT [--with-Qt-dir=DIR] [--with-Qt-lib=LIB]
dnl @synopsis BNV_HAVE_QT [--with-Qt-include-dir=DIR] [--with-Qt-bin-dir=DIR] [--with-Qt-lib-dir=DIR] [--with-Qt-lib=LIB]
dnl
dnl @summary Search for Trolltech's Qt GUI framework.
dnl
dnl Searches common directories for Qt include files, libraries and Qt
dnl binary utilities. The macro supports several different versions of
dnl the Qt framework being installed on the same machine. Without
dnl options, the macro is designed to look for the latest library,
dnl i.e., the highest definition of QT_VERSION in qglobal.h. By use of
dnl one or more options a different library may be selected. There are
dnl two different sets of options. Both sets contain the option
dnl --with-Qt-lib=LIB which can be used to force the use of a
dnl particular version of the library file when more than one are
dnl available. LIB must be in the form as it would appear behind the
dnl "-l" option to the compiler. Examples for LIB would be "qt-mt" for
dnl the multi-threaded version and "qt" for the regular version. In
dnl addition to this, the first set consists of an option
dnl --with-Qt-dir=DIR which can be used when the installation conforms
dnl to Trolltech's standard installation, which means that header files
dnl are in DIR/include, binary utilities are in DIR/bin and the library
dnl is in DIR/lib. The second set of options can be used to indicate
dnl individual locations for the header files, the binary utilities and
dnl the library file, in addition to the specific version of the
dnl library file.
dnl
dnl The following shell variable is set to either "yes" or "no":
dnl
dnl   have_qt
dnl
dnl Additionally, the following variables are exported:
dnl
dnl   QT_CXXFLAGS
dnl   QT_LIBS
dnl   QT_MOC
dnl   QT_UIC
dnl   QT_DIR
dnl
dnl which respectively contain an "-I" flag pointing to the Qt include
dnl directory (and "-DQT_THREAD_SUPPORT" when LIB is "qt-mt"), link
dnl flags necessary to link with Qt and X, the name of the meta object
dnl compiler and the user interface compiler both with full path, and
dnl finaly the variable QTDIR as Trolltech likes to see it defined (if
dnl possible).
dnl
dnl Example lines for Makefile.in:
dnl
dnl   CXXFLAGS = @QT_CXXFLAGS@
dnl   MOC      = @QT_MOC@
dnl
dnl After the variables have been set, a trial compile and link is
dnl performed to check the correct functioning of the meta object
dnl compiler. This test may fail when the different detected elements
dnl stem from different releases of the Qt framework. In that case, an
dnl error message is emitted and configure stops.
dnl
dnl No common variables such as $LIBS or $CFLAGS are polluted.
dnl
dnl Options:
dnl
dnl --with-Qt-dir=DIR: DIR is equal to $QTDIR if you have followed the
dnl installation instructions of Trolltech. Header files are in
dnl DIR/include, binary utilities are in DIR/bin and the library is in
dnl DIR/lib.
dnl
dnl --with-Qt-include-dir=DIR: Qt header files are in DIR.
dnl
dnl --with-Qt-bin-dir=DIR: Qt utilities such as moc and uic are in DIR.
dnl
dnl --with-Qt-lib-dir=DIR: The Qt library is in DIR.
dnl
dnl --with-Qt-lib=LIB: Use -lLIB to link with the Qt library.
dnl
dnl If some option "=no" or, equivalently, a --without-Qt-* version is
dnl given in stead of a --with-Qt-*, "have_qt" is set to "no" and the
dnl other variables are set to the empty string.
dnl
dnl @category InstalledPackages
dnl @author Bastiaan Veelo <Bastiaan.N.Veelo@ntnu.no>
dnl @version 2005-01-24
dnl @license AllPermissive

dnl Copyright (C) 2001, 2002, 2003, 2005, Bastiaan Veelo

dnl Calls BNV_PATH_QT_DIRECT (contained in this file) as a subroutine.
AC_DEFUN([BNV_HAVE_QT],
[
  dnl THANKS! This code includes bug fixes and contributions made by:
  dnl Tim McClarren,
  dnl Dennis R. Weilert,
  dnl Qingning Huo.

  AC_REQUIRE([AC_PROG_CXX])
  AC_REQUIRE([AC_PATH_X])
  AC_REQUIRE([AC_PATH_XTRA])

  AC_MSG_CHECKING(for Qt)

  AC_ARG_WITH([Qt-dir],
    [  --with-Qt-dir=DIR       DIR is equal to \$QTDIR if you have followed the
                          installation instructions of Trolltech. Header
                          files are in DIR/include, binary utilities are
                          in DIR/bin and the library is in DIR/lib])
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
    if test $bnv_qt_lib = "qt-mt"; then
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
      bnv_try_1="$QT_MOC bnv_qt_test.h -o moc_bnv_qt_test.$ac_ext >/dev/null 2>bnv_qt_test_1.out"
      AC_TRY_EVAL(bnv_try_1)
      bnv_err_1=`grep -v '^ *+' bnv_qt_test_1.out | grep -v "^bnv_qt_test.h\$"`
      if test x"$bnv_err_1" != x; then
        echo "$bnv_err_1" >&AC_FD_CC
        echo "configure: could not run $QT_MOC on:" >&AC_FD_CC
        cat bnv_qt_test.h >&AC_FD_CC
      else
        bnv_try_2="$CXX $QT_CXXFLAGS -c $CXXFLAGS -o moc_bnv_qt_test.o moc_bnv_qt_test.$ac_ext >/dev/null 2>bnv_qt_test_2.out"
        AC_TRY_EVAL(bnv_try_2)
        bnv_err_2=`grep -v '^ *+' bnv_qt_test_2.out | grep -v "^bnv_qt_test.{$ac_ext}\$"`
        if test x"$bnv_err_2" != x; then
          echo "$bnv_err_2" >&AC_FD_CC
          echo "configure: could not compile:" >&AC_FD_CC
          cat bnv_qt_test.$ac_ext >&AC_FD_CC
        else
          bnv_try_3="$CXX $QT_CXXFLAGS -c $CXXFLAGS -o bnv_qt_main.o bnv_qt_main.$ac_ext >/dev/null 2>bnv_qt_test_3.out"
          AC_TRY_EVAL(bnv_try_3)
          bnv_err_3=`grep -v '^ *+' bnv_qt_test_3.out | grep -v "^bnv_qt_main.{$ac_ext}\$"`
          if test x"$bnv_err_3" != x; then
            echo "$bnv_err_3" >&AC_FD_CC
            echo "configure: could not compile:" >&AC_FD_CC
            cat bnv_qt_main.$ac_ext >&AC_FD_CC
          else
            bnv_try_4="$CXX $QT_LIBS $LIBS -o bnv_qt_main bnv_qt_main.o moc_bnv_qt_test.o >/dev/null 2>bnv_qt_test_4.out"
            AC_TRY_EVAL(bnv_try_4)
            bnv_err_4=`grep -v '^ *+' bnv_qt_test_4.out`
            if test x"$bnv_err_4" != x; then
              echo "$bnv_err_4" >&AC_FD_CC
            else
              bnv_cv_qt_test_result="succes"
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
          bnv_qt_main.$ac_ext bnv_qt_main.o bnv_qt_main \
          bnv_qt_test_1.out bnv_qt_test_2.out bnv_qt_test_3.out bnv_qt_test_4.out
  fi
])

dnl Internal subroutine of BNV_HAVE_QT
dnl Set bnv_qt_dir bnv_qt_include_dir bnv_qt_bin_dir bnv_qt_lib_dir bnv_qt_lib
AC_DEFUN(BNV_PATH_QT_DIRECT,
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
      `ls -dr /usr/include/qt* 2>/dev/null`
      `ls -dr /usr/lib/qt*/include 2>/dev/null`
      `ls -dr /usr/local/qt*/include 2>/dev/null`
      `ls -dr /opt/qt*/include 2>/dev/null`
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
  if test -x $bnv_possible_qt_dir/bin/moc &&
     ls $bnv_possible_qt_dir/lib/libqt* > /dev/null; then
    # Then the rest is a piece of cake
    bnv_qt_dir=$bnv_possible_qt_dir
    bnv_qt_bin_dir="$bnv_qt_dir/bin"
    ### Start patch Dennis Weilert
    #bnv_qt_lib_dir="$bnv_qt_dir/lib"
    if test x"$with_Qt_lib_dir" != x; then
      bnv_qt_lib_dir="$with_Qt_lib_dir"
    else
      bnv_qt_lib_dir="$bnv_qt_dir/lib"
    fi
    ### End patch Dennis Weilert
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
        # Succes.
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
          # Succes.
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
            # Succes.
            # We can link with no special library directory.
            bnv_qt_lib_dir=
          ], [
            # That did not work. Maybe a library version I don't know about?
            echo "Non-critical error, please neglect the above." >&AC_FD_CC
            # Look for some Qt lib in a standard set of common directories.
            bnv_dir_list="
              `echo $bnv_qt_includes | sed ss/includess`
              /lib
              /usr/lib
              /usr/local/lib
              /opt/lib
              `ls -dr /usr/lib/qt* 2>/dev/null`
              `ls -dr /usr/local/qt* 2>/dev/null`
              `ls -dr /opt/qt* 2>/dev/null`
            "
            for bnv_dir in $bnv_dir_list; do
              if ls $bnv_dir/libqt*; then
                # Gamble that it's the first one...
                bnv_qt_lib="`ls $bnv_dir/libqt* | sed -n 1p |
                            sed s@$bnv_dir/lib@@ | sed s/[.].*//`"
                bnv_qt_lib_dir="$bnv_dir"
                break
              fi
            done
            # Try with that one
            LIBS="-l$bnv_qt_lib $X_PRE_LIBS $X_LIBS -lX11 -lXext -lXmu -lXt -lXi $X_EXTRA_LIBS"
            AC_TRY_LINK([#include <$qt_direct_test_header>],
              $qt_direct_test_main,
            [
              # Succes.
              # We can link with no special library directory.
              bnv_qt_lib_dir=
            ], [
              # Leave bnv_qt_lib_dir defined
            ])
          ])
        ])
      ])
      if test x"$bnv_qt_lib_dir" != x; then
        bnv_qt_LIBS="-l$bnv_qt_lib_dir $LIBS"
      else
        bnv_qt_LIBS="$LIBS"
      fi
      LIBS="$bnv_save_LIBS"
      CXXFLAGS="$bnv_save_CXXFLAGS"
    fi dnl $with_Qt_lib_dir was not given
  fi dnl Done setting up for non-traditional Trolltech installation
])

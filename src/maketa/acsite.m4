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

#						  gw_CHECK_QT
#*************************************************************
# qt_CHECK_QT, a.k.a AutoQt, is a macro written by 
# Geoffrey Wossum. The homepage can be found at 
# http://autoqt.sourceforge.net/
#*************************************************************
# Copyright (c) 2002, Geoffrey Wossum
# All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without 
# modification, are permitted provided that the following conditions are 
# met:
# 
#  - Redistributions of source code must retain the above copyright notice, 
#    this list of conditions and the following disclaimer.
# 
#  - Redistributions in binary form must reproduce the above copyright 
#    notice, this list of conditions and the following disclaimer in the 
#    documentation and/or other materials provided with the distribution.
# 
#  - Neither the name of Geoffrey Wossum nor the names of its 
#    contributors may be used to endorse or promote products derived from 
#    this software without specific prior written permission.
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED 
# TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
# PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR 
# CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
# OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
# WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
# OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF 
# ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# Check for Qt compiler flags, linker flags, and binary packages
AC_DEFUN([gw_CHECK_QT],
[
AC_REQUIRE([AC_PROG_CXX])
AC_REQUIRE([AC_PATH_XTRA])

AC_MSG_CHECKING([QTDIR])
AC_ARG_WITH([qtdir], AC_HELP_STRING([--with-qtdir=DIR], [Specify the installation directory of Qt. The default is $QTDIR.]),[QTDIR=$withval])
# Check that QTDIR is defined or that --with-qtdir given
if test x"$QTDIR" = x ; then
    QT_SEARCH="/usr/lib/qt31 /usr/local/qt31 /usr/lib/qt3 /usr/local/qt3 /usr/lib/qt2 /usr/local/qt2 /usr/lib/qt /usr/local/qt"
    for i in $QT_SEARCH; do
	if test -f $i/include/qglobal.h -a x$QTDIR = x; then QTDIR=$i; fi
    done
fi
if test x"$QTDIR" = x ; then
    AC_MSG_ERROR([*** QTDIR must be defined, or --with-qtdir option given])
fi
AC_MSG_RESULT([$QTDIR])

# Change backslashes in QTDIR to forward slashes to prevent escaping
# problems later on in the build process, mainly for Cygwin build
# environment using MSVC as the compiler
# TODO: Use sed instead of perl
QTDIR=`echo $QTDIR | perl -p -e 's/\\\\/\\//g'`

# Figure out which version of Qt we are using
AC_MSG_CHECKING([Qt version])
QT_VER=`grep 'define.*QT_VERSION_STR\W' $QTDIR/include/qglobal.h | perl -p -e 's/\D//g'`
case "${QT_VER}" in
    2*)
	QT_MAJOR="2"
    ;;
    3*)
	QT_MAJOR="3"
    ;;
    *)
	AC_MSG_ERROR([*** Don't know how to handle this Qt major version])
    ;;
esac
AC_MSG_RESULT([$QT_VER ($QT_MAJOR)])

# Check that moc is in path
AC_CHECK_PROG(MOC, moc, moc)
if test x$MOC = x ; then
	AC_MSG_ERROR([*** moc must be in path])
fi

# uic is the Qt user interface compiler
AC_CHECK_PROG(UIC, uic, uic)
if test x$UIC = x ; then
	AC_MSG_ERROR([*** uic must be in path])
fi

# qembed is the Qt data embedding utility.
# It is located in $QTDIR/tools/qembed, and must be compiled and installed
# manually, we'll let it slide if it isn't present
AC_CHECK_PROG(QEMBED, qembed, qembed)


# Calculate Qt include path
QT_CXXFLAGS="-I$QTDIR/include"

QT_IS_EMBEDDED="no"
# On unix, figure out if we're doing a static or dynamic link
case "${host}" in
    *-cygwin)
	AC_DEFINE_UNQUOTED(WIN32, "", Defined if on Win32 platform)
	if test -f "$QTDIR/lib/qt.lib" ; then
	    QT_LIB="qt.lib"
	    QT_IS_STATIC="yes"
	    QT_IS_MT="no"
	elif test -f "$QTDIR/lib/qt-mt.lib" ; then
	    QT_LIB="qt-mt.lib" 
	    QT_IS_STATIC="yes"
	    QT_IS_MT="yes"
	elif test -f "$QTDIR/lib/qt$QT_VER.lib" ; then
	    QT_LIB="qt$QT_VER.lib"
	    QT_IS_STATIC="no"
	    QT_IS_MT="no"
	elif test -f "$QTDIR/lib/qt-mt$QT_VER.lib" ; then
	    QT_LIB="qt-mt$QT_VER.lib"
	    QT_IS_STATIC="no"
	    QT_IS_MT="yes"
	fi
	;;

    *)
	QT_IS_STATIC=`ls $QTDIR/lib/*.a 2> /dev/null`
	if test "x$QT_IS_STATIC" = x; then
	    QT_IS_STATIC="no"
	else
	    QT_IS_STATIC="yes"
	fi
	if test x$QT_IS_STATIC = xno ; then
	    QT_IS_DYNAMIC=`ls $QTDIR/lib/*.so 2> /dev/null` 
	    if test "x$QT_IS_DYNAMIC" = x;  then
		AC_MSG_ERROR([*** Couldn't find any Qt libraries])
	    fi
	fi

	if test "x`ls $QTDIR/lib/libqt.* 2> /dev/null`" != x ; then
	    QT_LIB="-lqt"
	    QT_IS_MT="no"
	elif test "x`ls $QTDIR/lib/libqt-mt.* 2> /dev/null`" != x ; then
	    QT_LIB="-lqt-mt"
	    QT_IS_MT="yes"
	elif test "x`ls $QTDIR/lib/libqte.* 2> /dev/null`" != x ; then
	    QT_LIB="-lqte"
	    QT_IS_MT="no"
	    QT_IS_EMBEDDED="yes"
	elif test "x`ls $QTDIR/lib/libqte-mt.* 2> /dev/null`" != x ; then
	    QT_LIB="-lqte-mt"
	    QT_IS_MT="yes"
	    QT_IS_EMBEDDED="yes"
	fi
	;;
esac
AC_MSG_CHECKING([if Qt is static])
AC_MSG_RESULT([$QT_IS_STATIC])
AC_MSG_CHECKING([if Qt is multithreaded])
AC_MSG_RESULT([$QT_IS_MT])
AC_MSG_CHECKING([if Qt is embedded])
AC_MSG_RESULT([$QT_IS_EMBEDDED])

QT_GUILINK=""
QASSISTANTCLIENT_LDADD="-lqassistantclient"
case "${host}" in

    *linux*)
	QT_LIBS="$QT_LIB"
	if test $QT_IS_STATIC = yes && test $QT_IS_EMBEDDED = no; then
	    QT_LIBS="$QT_LIBS -lm"
	fi
	;;

    *darwin*)
	QT_LIBS="$QT_LIB"
	if test $QT_IS_STATIC = yes && test $QT_IS_EMBEDDED = no; then
	    QT_LIBS="$QT_LIBS  -lm"
	fi
	;;

esac

if test x"$QT_IS_MT" = "xyes" ; then
	AC_DEFINE([_REENTRANT],[1],[Set when qt is multi-threaded])
	AC_DEFINE([QT_THREAD_SUPPORT],[1],[Build with Qt thread support])
fi

QT_LDADD="-L$QTDIR/lib $QT_LIBS"

if test x$QT_IS_STATIC = xyes ; then
    AC_CHECK_LIB([Xft],[XftFontOpen],[LIBS="$LIBS -lXft"])
fi

AC_MSG_CHECKING([QT_CXXFLAGS])
AC_MSG_RESULT([$QT_CXXFLAGS])
AC_MSG_CHECKING([QT_LDADD])
AC_MSG_RESULT([$QT_LDADD])
])

#TODO:
# QT 4, will need pkg-config macros from
# http://cvs.freedesktop.org/*checkout*/pkg-config/pkg-config/pkg.m4
# as well as from
# http://savannah.gnu.org/cgi-bin/viewcvs/*checkout*/classpath/classpath/configure.ac?rev=HEAD&content-type=text/plain 
# see also autoconf mailing list

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
    AC_MSG_WARN([Unable to find a readline compatible library.])
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
		AC_MSG_WARN([GNU Make not found. % extensions may fail. Considering uncommenting code in Moc.am])
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
AC_ARG_VAR(GAWK, [gawk executable to use])
if test "x$GAWK" = "x"; then
   AC_CHECK_PROGS(GAWK,[gawk])
fi

if test "x$GAWK" != "x"; then
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
	$GAWK \' \\
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
    AC_MSG_WARN([install_files support disable... gawk not found])
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
	$GAWK \'/^[^0-9]/ { \\
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
	| $GAWK -v files=\"\$\$files\" \\
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
	@RPMDIR=\`cat \$(top_builddir)/rpmmacros | $GAWK \'/%%_rpmdir/ { print \$${AX_DOLLAR}2; }\'\`; \\
	echo \"\$\$RPMDIR\" | $EGREP \"%%{.*}\" > /dev/null 2>&1; \\
	EXIT=\$\$?; \\
	while test \"\$\$EXIT\" == \"0\"; do \\
		RPMDIR=\`echo \"\$\$RPMDIR\" | $GAWK \'/%%{.*}/ \\
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
	@SRPMDIR=\`cat rpmmacros | $GAWK \'/%%_srcrpmdir/ { print ${AX_DOLLAR}${AX_DOLLAR}2; }\'\`; \\
	echo \"${AX_DOLLAR}${AX_DOLLAR}SRPMDIR\" | $EGREP \"%%{.*}\" > /dev/null 2>&1; \\
	EXIT=${AX_DOLLAR}${AX_DOLLAR}?; \\
	while test \"${AX_DOLLAR}${AX_DOLLAR}EXIT\" == \"0\"; do \\
	    SRPMDIR=\`echo \"${AX_DOLLAR}${AX_DOLLAR}SRPMDIR\" | $GAWK \'/%%{.*}/ \\
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
		AC_MSG_WARN([rpm support disabled... PLATFORM_SUFFIX not set])
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

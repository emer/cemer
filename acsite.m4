#                                         PDP_DETERMINE_OSTYPE
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
	;;
	*darwin*)
		AC_SUBST([PDP_PLATFORM],[DARWIN])
	;;
	*cygwin*)
		AC_SUBST([PDP_PLATFORM],[CYGWIN])
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

#                                         PDP_DETERMINE_SUFFIX
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

#                                                  gw_CHECK_QT
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
        QT_CXXFLAGS="$QT_CXXFLAGS -D_REENTRANT -DQT_THREAD_SUPPORT"
fi

QT_LDADD="-L$QTDIR/lib $QT_LIBS"

if test x$QT_IS_STATIC = xyes ; then
    AC_CHECK_LIB(Xft, XftFontOpen, QT_LDADD="$QT_LDADD -lXft")
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

#                                              VL_LIB_READLINE
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


#                                               CHECK_GNU_MAKE
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


#            ACX_MPI([ACTION-IF-FOUND[, ACTION-IF-NOT-FOUND]])
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

AC_SUBST(MPILIBS)

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


#        ACX_PTHREAD([ACTION-IF-FOUND[, ACTION-IF-NOT-FOUND]])
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
            *-aix* | *-freebsd* | *-darwin*) flag="-D_THREAD_SAFE";;
            *solaris* | *-osf* | *-hpux*) flag="-D_REENTRANT";;
        esac
        AC_MSG_RESULT(${flag})
        if test "x$flag" != xno; then
            PTHREAD_CFLAGS="$flag $PTHREAD_CFLAGS"
        fi

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

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
AC_REQUIRE([AC_PATH_X])

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

AC_SUBST([MPILIBS])

# Finally, execute ACTION-IF-FOUND/ACTION-IF-NOT-FOUND:
if test x = x"$MPILIBS"; then
        $2
        :
else
        ifelse([$1],,[AC_DEFINE(HAVE_MPI,1,[Define if you have the MPI library.])],[$1])
        :
fi

])dnl ACX_MPI
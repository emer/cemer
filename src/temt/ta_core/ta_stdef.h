// Copyright 2017, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of The Emergent Toolkit
//
//   This library is free software; you can redistribute it and/or
//   modify it under the terms of the GNU Lesser General Public
//   License as published by the Free Software Foundation; either
//   version 2.1 of the License, or (at your option) any later version.
//   
//   This library is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//   Lesser General Public License for more details.


#ifndef ta_stdef_h
#define ta_stdef_h 1

// standard definitions, etc

#include "taconfig.h"

#ifndef __MAKETA__
# if defined(_MSC_VER) //&& defined (_WIN64)
#   define __restrict
# endif
# include <stdlib.h>
# include <string.h>
# include <limits.h>

// as of Qt5.6, we need to include qtglobal to get processor information -- it is
// no longer included in qconfig.h, and the qprocessor* file that has it includes
// qglobal anyway
#include <qglobal.h>

// we don't support PA RISC because of the nutty 5-int QAtomicInt
#ifdef QT_ARCH_PARISC
# error "Emergent is not supported on PA RISC architecture"
#endif

// define our own versions of stuff we need from qconfig.h, so remainder
// of the file is independent of that stuff
#define TA_LARGEFILE_SUPPORT QT_LARGEFILE_SUPPORT // usually 64
#define TA_POINTER_SIZE  QT_POINTER_SIZE // 4 or 8, all use MUST error if not 4/8
// byte order -- note, rarely used, primarily to optimize placement of rgb
//
#define TA_BIG_ENDIAN Q_BIG_ENDIAN
#define TA_LITTLE_ENDIAN Q_LITTLE_ENDIAN
// BYTE_ORDER is one of TA_BIG or LITTLE -- tests must fail if neither
#define TA_BYTE_ORDER Q_BYTE_ORDER

#endif


// misc optional debugging extras
#ifdef DEBUG
//#define TA_PROFILE // enable for profiling
#endif


// when building maketa we want to make sure to turn off qt
#ifdef NO_TA_BASE
// configure should handle these, but we make sure we don't include Qt at all, or Inventor for maketa
#  ifdef TA_GUI
#    undef TA_GUI
#  endif
#  ifndef TA_NO_GUI
#    define TA_NO_GUI
#  endif
#  ifdef TA_PROFILE
#    undef TA_PROFILE
#  endif
#endif

// Operating System and Environment codes -- generally mirror those of Qt
// NOTE: many of these are untested for TA/PDP, and are just included to mirror Qt

#if defined(__APPLE__) && (defined(__GNUC__) || defined(__xlC__) || defined(__xlc__))
#  define TA_OS_DARWIN
#  define TA_OS_MAC /* TA_OS_MAC is mostly for compatiblity, but also more clear */
#elif defined(__CYGWIN__) // note: not supported
#  define TA_OS_CYGWIN
#  error "Cygwin is not supported for TA/PDP"
#elif defined(_WIN64)
#  define TA_OS_WIN64
#  undef TA_POINTER_SIZE
#  define TA_POINTER_SIZE 8
#elif defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#  undef TA_POINTER_SIZE
#  define TA_POINTER_SIZE 4
#  define TA_OS_WIN32
#elif defined(__MWERKS__) && defined(__INTEL__)
#  define TA_OS_WIN32
#elif defined(__sun) || defined(sun)
#  define TA_OS_SOLARIS
#elif defined(hpux) || defined(__hpux)
#  define TA_OS_HPUX
#elif defined(__ultrix) || defined(ultrix)
#  define TA_OS_ULTRIX
#elif defined(sinix)
#  define TA_OS_RELIANT
#elif defined(__linux__) || defined(__linux)
#  define TA_OS_LINUX
#elif defined(__FreeBSD__) || defined(__DragonFly__)
#  define TA_OS_FREEBSD
#  define TA_OS_BSD4
#elif defined(__NetBSD__)
#  define TA_OS_NETBSD
#  define TA_OS_BSD4
#elif defined(__OpenBSD__)
#  define TA_OS_OPENBSD
#  define TA_OS_BSD4
#elif defined(__bsdi__)
#  define TA_OS_BSDI
#  define TA_OS_BSD4
#elif defined(__sgi)
#  define TA_OS_IRIX
#elif defined(__osf__)
#  define TA_OS_OSF
#elif defined(_AIX)
#  define TA_OS_AIX
#elif defined(__Lynx__)
#  define TA_OS_LYNX
#elif defined(__GNU_HURD__)
#  define TA_OS_HURD
#elif defined(__DGUX__)
#  define TA_OS_DGUX
#elif defined(__QNXNTO__)
#  define TA_OS_QNX6
#elif defined(__QNX__)
#  define TA_OS_QNX
#elif defined(_SEQUENT_)
#  define TA_OS_DYNIX
#elif defined(_SCO_DS) /* SCO OpenServer 5 + GCC */
#  define TA_OS_SCO
#elif defined(__USLC__) /* all SCO platforms + UDK or OUDK */
#  define TA_OS_UNIXWARE
#elif defined(__svr4__) && defined(i386) /* Open UNIX 8 + GCC */
#  define TA_OS_UNIXWARE
#elif defined(__MAKEDEPEND__)
#else
#  error "TA/PDP has not been ported to this OS - talk to " //TODO: add pdp mailing list
#endif

#if defined(TA_OS_WIN32) || defined(TA_OS_WIN64)
#  define TA_OS_WIN
#endif

#if defined(TA_OS_WIN)
#  undef TA_OS_UNIX
#elif !defined(TA_OS_UNIX)
#  define TA_OS_UNIX
#endif

// NO_TA_USE_THREADS only for config control
#if (!defined(NO_TA_USE_THREADS))
# define TA_USE_THREADS
#endif

// misc stuff

// provide rich error messages for css
#define YYERROR_VERBOSE 1

/* DLL building options under windows */
// define flags as follows:
// DLL      USING         BUILDING
// ta       TA_DLL        TA_DLL,TA_EXPORTS
// emergent EMERGENT_DLL  EMERGENT_DLL,EMERGENT_EXPORTS

#ifdef TA_OS_WIN
// note: cmake on Win automatically defines temt_EXPORTS or emergentlib_EXPORTS
// but we just manually define our own legacy guys, including the xxx_DLL as appropriate
# ifdef TA_DLL
#   define CSS_DLL
# endif
# ifdef TA_EXPORTS
#   define CSS_EXPORTS
# endif
# if defined(HAVE_LIBGSL) || defined(HAVE_LIBGSLCBLAS)
#    define GSL_DLL // see gsl:Readme_GnuWin32.txt
# endif
#endif


// Useful pseudo keywords, for C++

// deals with 'override' keyword in c++11, defined to nothing in older compilers
#if (__cplusplus >= 201103L) || (defined (_MSC_VER) && _MSC_VER >= 1700)
//#pragma message "C++11 supported compiler, NOT defining override"
#ifdef override
#undef override
#endif
#else
//#pragma message "C++11 NOT supported by compiler, defining override"
#define override
#endif

#ifdef TA_OS_WIN
# define _USE_MATH_DEFINES
#else
# include <stdint.h>
#endif

typedef unsigned short  ushort;
typedef unsigned        uint;
typedef unsigned long   ulong;
typedef unsigned char   byte;

#ifndef __MAKETA__ // we define all these in maketa/ta_type.h so don't need them during scanning
// god bless Microsoft c++...
# ifdef _MSC_VER
#pragma warning(disable: 4800 4522) // disable super annoying bool conversion warning
  typedef signed char		int8_t;
  typedef unsigned char         uint8_t;
  typedef short			int16_t;
  typedef unsigned short	uint16_t;
  typedef int		        int32_t;
  typedef unsigned int          uint;
  typedef unsigned int          uint32_t;
  typedef long long             int64_t;
  typedef unsigned long long    uint64_t;
  //note: prob should inline these, rather than macros, but don't want naggling little
  // type differences to cause compile issues
#if _MSC_VER < 1800
# define strtoll _strtoi64
# define strtoull _strtoui64
# define isnan(x) _isnan(x)
# define isinf(x) (!_finite(x))
#endif //_MSC_VER < 1800
# endif // intptr_t hacks

// wordsize dependent stuff -- MSVC is (of course!) different
#if (TA_POINTER_SIZE == 4)
# if (defined(_WIN32))
    typedef int                 intptr_t;
    typedef unsigned int        uintptr_t;
    typedef int                 ta_intptr_t;
    typedef unsigned int        ta_uintptr_t;
# else // gcc
    typedef int                 ta_intptr_t;
    typedef unsigned int        ta_uintptr_t;
# endif
# define QVARIANT_TO_INTPTR(qv) (qv.toInt())

#elif (TA_POINTER_SIZE == 8)
# if defined(_WIN64)
    typedef long long           intptr_t;
    typedef unsigned long long  uintptr_t;
    typedef long long           ta_intptr_t;
    typedef unsigned long long  ta_uintptr_t;
# else // gcc
    typedef long long           ta_intptr_t;
    typedef unsigned long long  ta_uintptr_t;
# endif
# define QVARIANT_TO_INTPTR(qv) (qv.toLongLong())

#else
# error "TA_POINTER_SIZE should be 4 or 8"
#endif

#else  // maketa

// use the TA_POINTER_SIZE == 8 case -- shouldn't matter that much..
# if defined(_WIN64)
    typedef long long           intptr_t;
    typedef unsigned long long  uintptr_t;
    typedef long long           ta_intptr_t;
    typedef unsigned long long  ta_uintptr_t;
# else // gcc
    typedef long long           ta_intptr_t;
    typedef unsigned long long  ta_uintptr_t;
# endif
# define QVARIANT_TO_INTPTR(qv) (qv.toLongLong())

#endif // skip over for maketa



typedef long long             ta_int64_t;
typedef unsigned long long    ta_uint64_t;

#ifdef __MAKETA__
# define INHERITED(c)
#else
# define INHERITED(c) typedef c inherited;
#endif

// define Qt's macros, for Maketa
// for signals, we must put #ifdef's around the signals (note: moc does not accept preprocessor directives)
#ifdef __MAKETA__
  #define emit
  #define slots
  #define SIGNALS(x)
  #define Q_OBJECT
#endif

// Misc useful macros

#ifndef MAX
#define MAX(a,b) (((a) > (b)) ? (a) : (b))
#define MIN(a,b) (((a) < (b)) ? (a) : (b))
#define ABS(a)  ((a) >= 0 ? (a) : -(a))
#endif

// hmm... these are *supposed* to be standard...
// probably added for early version of windows, but it is in 2008
#if (_MSC_VER < 1400) && !defined(LLONG_MAX)
#define LLONG_MAX 9223372036854775807ULL
#endif

// Some OS-specific includes or types
#ifdef TA_OS_WIN
  #ifndef __MAKETA__
    #include <io.h>
  #endif
  #ifdef _MSC_VER
    #define F_OK 00 // Existence only
    #define W_OK 02 // Write permission
    #define R_OK 04 // Read permission
  #endif
  //#define F_OK 06 // Read and write permission

  typedef int pid_t; //
#else
  #include <unistd.h>
#endif

// Qt Event IDs in emergent should be allocated here:
#ifdef TA_USE_QT
enum CustomEventType {
  FirstEvent            = 1000, //note: QT's custom events start at 1000
  cssMisc_StartupShell_EventType, // css_machine.h
  itaViewer_SelectionChanged_EventType // ta_qtviewer.h
};
#endif


#ifdef __MAKETA__
//NOTE: must use "" for msvc
#  include "maketa_iostream.h"
#  include "maketa_fstream.h"
#  include "maketa_sstream.h"
#else
# include <iostream>
# include <fstream>
#endif

#ifdef List
#undef List
#endif

#if (defined(TA_OS_SOLARIS) && !(defined(__MAKETA__)))
  #include <sys/types.h>
#endif

// Posix Signal function definitions

#define SIGNAL_PROC_FUN_TYPE void (*)(int)
#define SIGNAL_PROC_FUN_ARG(x) void (*x)(int)

// Some global forward declarations

class QObject; //
class QWidget; //
class QString; //

class TypeDef; //
class MemberDef; //
class MethodDef; //

// for css support
class cssEl;                    // #IGNORE

typedef cssEl* (*css_fun_stub_ptr)(void*, int, cssEl**);
typedef void (*ta_void_fun)();

// This macro must be used in all class headers that are TA parsed -- it makes
// the TA_ TypeDef avail to any that include the class -- can also use in .cpp
// of files that do not include the header but still need to access the TA info
//
// unfortunately, for windows compatibility, the dllexport business is required
// so we have to have separate ta and e versions of these for ta and emergent
// types -- these "bare" guys can be used only for plugins

#define TypeDef_Of(T) extern TypeDef TA_ ## T
#define TypeInst_Of(T) extern T* TAI_ ## T

typedef void* voidptr; // for maketa, which chokes on void* in a template

// fake version of mpi comm to allow functions to not be conditionalized
#ifndef DMEM_COMPILE
typedef int MPI_Comm;		// mpi communicator
#endif

// maximum number of dimensions in a matrix
#define TA_MATRIX_DIMS_MAX 8

#endif // ta_stdef_h

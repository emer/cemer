// Copyright, 1995-2007, Regents of the University of Colorado,
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

#include "config.h"

//TEMP: comment out to use legacy layout for Qt 4.4; uncomment to enable for testing
#define TA_USE_QFORMLAYOUT

#ifndef __MAKETA__
# if defined(_MSC_VER) //&& defined (_WIN64)
#   define __restrict
# endif
# include <stdlib.h>
# include <string.h>
# include <limits.h>
#endif

//NOTE: qconfig.h has the endianness and ptr size, but no dependencies, and
// no Qt-dependencies -- note that on Mac/frameworks, it is in QtCore
#include <qconfig.h>

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


// misc optional debugging extras
#ifdef DEBUG
//#define TA_PROFILE // enable for profiling
#endif


// when building maketa we want to make sure to turn off qt
#ifdef NO_TA_BASE
// configure should handle these, but we make sure we don't include Qt at all, or Inventor for maketa
#  ifdef TA_USE_QT
#    undef TA_USE_QT
#  endif
#  ifdef TA_GUI
#    undef TA_GUI
#  endif
#  ifndef TA_NO_GUI
#    define TA_NO_GUI
#  endif
#  ifdef TA_USE_INVENTOR
#    undef TA_USE_INVENTOR
#  endif
#  ifdef TA_PROFILE
#    undef TA_PROFILE
#  endif
// we don't try scanning Qt's header files when running maketa
#elif defined(__MAKETA__)
#  include "qtdefs.h" // declares common classes so qxx.h files don't need to be included
#else // normal ta, in all variants
#  ifdef TA_USE_QT // normal to always include QT
#    include <QtGlobal>
#    include <qevent.h>
#    include "qtdefs.h" // declares common qt and related classes often used by pointer
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
// note: we put taiqtso/ta/css in one dll, so slave to TA
// note: we put bp/cs/leabra/network/so in one dll, so slave to EMERGENT
// define flags as follows:
// DLL      USING         BUILDING
// ta       TA_DLL        TA_DLL,TA_EXPORTS
// emergent EMERGENT_DLL  EMERGENT_DLL,EMERGENT_EXPORTS

#ifdef TA_OS_WIN
// note: cmake on Win automatically defines temt_EXPORTS or emergentlib_EXPORTS
// but we just manually define our own legacy guys, including the xxx_DLL as appropriate
# ifdef TA_DLL
#   define TAIQTSO_DLL
#   define CSS_DLL
# endif
# ifdef TA_EXPORTS
#   define TAIQTSO_EXPORTS
#   define CSS_EXPORTS
# endif

# ifdef EMERGENT_DLL
#   define BP_DLL
#   define CS_DLL
#   define LEABRA_DLL
#   define SO_DLL
# endif
# ifdef EMERGENT_EXPORTS
#   define BP_EXPORTS
#   define CS_EXPORTS
#   define LEABRA_EXPORTS
#   define SO_EXPORTS
# endif
# if defined(HAVE_LIBGSL) || defined(HAVE_LIBGSLCBLAS)
#    define GSL_DLL // see gsl:Readme_GnuWin32.txt
# endif
#endif


// Useful pseudo keywords, for C++

#define override virtual  // makes override situation more clear -- use for all descendants

#ifdef TA_OS_WIN
# define _USE_MATH_DEFINES
#else
# include <stdint.h>
#endif

// define useful types -- some will be defined by QT, so don't define them if using QT

typedef void* voidptr; // for maketa, which chokes on void* in a template

#if ((defined TA_GUI) && (!defined __MAKETA__))

#else // !def TA_GUI or making ta

typedef unsigned char   uchar;
typedef unsigned short  ushort;
typedef unsigned        uint;
typedef unsigned long   ulong;
typedef char*           pchar;
typedef uchar*          puchar; //

#endif // TA_GUI

typedef unsigned char   byte;

#ifndef __MAKETA__ // we define all these in maketa/ta_type.h so don't need them during scanning
// god bless Microsoft c++...
# ifdef _MSC_VER
  typedef unsigned char         uint8_t;
  typedef unsigned int          uint;
  typedef unsigned int          uint32_t;
  typedef long long             int64_t;
  typedef unsigned long long    uint64_t;
  //note: prob should inline these, rather than macros, but don't want naggling little
  // type differences to cause compile issues
# define strtoll _strtoi64
# define strtoull _strtoui64
# define isnan(x) _isnan(x)
# define isinf(x) (!_finite(x))
# endif // intptr_t hacks
#endif // skip over for maketa


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

typedef long long             ta_int64_t;
typedef unsigned long long    ta_uint64_t;

// misc. compiler hacks for MAKETA

#ifdef __MAKETA__
# define explicit
# define _(c) // hide the guy from maketa
# define INHERITED(c)
# define STATIC_CONST static
# define CONST_STATIC static const
# define USING(b)
# define volatile
#else
# define _(c) c
# define INHERITED(c) typedef c inherited;
# define STATIC_CONST static const
# define CONST_STATIC const static
# define USING(b) using b; \

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
  iDataViewer_SelectionChanged_EventType // ta_qtviewer.h
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

// Some misc. compatability defines -- made porting to Qt easier

#ifdef TA_USE_QT
// dummy defines for Qt -- its header files are too complex and parameterized for maketa
#  include "ta_maketa_defs.h"
#endif

// required for ODE -- we use single precision only
#define dSINGLE

// Some global forward declarations

class TypeDef; //
class MemberDef; //
class MethodDef; //

// This macro must be used in all class headers that are TA parsed -- it makes
// the TA_ TypeDef avail to any that include the class -- can also use in .cpp
// of files that do not include the header but still need to access the TA info
#define TypeDef_Of(T) extern TypeDef TA_ ## T
#define TypeInst_Of(T) extern T* TAI_ ## T

// fake version of mpi comm to allow functions to not be conditionalized
#ifndef DMEM_COMPILE
typedef int MPI_Comm;		// mpi communicator
#endif

// maximum number of dimensions in a matrix
#define TA_MATRIX_DIMS_MAX 8

#endif // ta_stdef_h

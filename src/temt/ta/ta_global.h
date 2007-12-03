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


// Standard global defines used in all header and source files

#ifndef TAGLOBAL_H
#define TAGLOBAL_H

#include "config.h"

#ifndef __MAKETA__
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#endif

// Qt includes -- we typically only don't include Qt when building maketa, otherwise,
// there are two options:
//   non-gui builds get QtCore, but not Qt gui-related classes
//   gui builds get QtGui etc.



//NOTE: qconfig.h has the endianness and ptr size, but no dependencies, and
// no Qt-dependencies
//TODO: longer-term, we should push these into our config.h
//#include <qconfig.h>
// define our own versions of stuff we need from qconfig.h, so remainder
// of the file is independent of that stuff
#define TA_LARGEFILE_SUPPORT QT_LARGEFILE_SUPPORT // usually 64
#define TA_POINTER_SIZE 4 // QT_POINTER_SIZE // 4 or 8, all use MUST error if not 4/8
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
#elif defined(__CYGWIN__) // note: not supported
#  define TA_OS_CYGWIN
#  error "Cygwin is not supported for TA/PDP"
#elif defined(WIN64) || defined(_WIN64) || defined(__WIN64__)
#  define TA_OS_WIN64
#  undef TA_POINTER_SIZE
#  define TA_POINTER_SIZE 8
#  error "Win64 is not yet supported for TA/PDP"
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

#if defined(TA_OS_DARWIN)
#  define TA_OS_MAC /* TA_OS_MAC is mostly for compatiblity, but also more clear */
#endif

#if defined(TA_OS_WIN)
#  undef TA_OS_UNIX
#elif !defined(TA_OS_UNIX)
#  define TA_OS_UNIX
#endif

// we don't try using threaded engine on non-Intel platforms
// esp legacy PPC Macs wherein xtremely unlikely to have multi threads
//TODO: need to add NO_TA_USE_THREADS, and need a way to auto shut off for PPC
#if (!defined(NO_TA_USE_THREADS))
# define TA_USE_THREADS
#endif

// misc stuff

// provide rich error messages for css
#define YYERROR_VERBOSE 1

/* DLL building options under windows */
// currently, we are putting taiqtso/ta/css/tamisc in one dll, so slave to TA
// define flags as follows:
// DLL	    USING	  BUILDING
// ta	    TA_DLL	  TA_DLL,TA_EXPORTS
// pdp	    PDP_DLL	  PDP_DLL,PDP_EXPORTS

#ifdef TA_OS_WIN
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
#else
# include <stdint.h>
#endif

// define useful types -- some will be defined by QT, so don't define them if using QT

typedef void* voidptr; // for maketa, which chokes on void* in a template

#if ((defined TA_GUI) && (!defined __MAKETA__))
// replicate the builtin bool type for those that don't have it
#ifdef NO_BUILTIN_BOOL
typedef	unsigned int 	bool;
#endif // def  NO_BUILTIN_BOOL

#else // !def TA_GUI or making ta

typedef unsigned char   uchar;
typedef unsigned short  ushort;
typedef unsigned	uint;
typedef unsigned long   ulong;
typedef char*		pchar;
typedef uchar*		puchar; //

#endif // TA_GUI

typedef unsigned char   byte;

#ifndef __MAKETA__ // we define all these in maketa/ta_type.h so don't need them during scanning
// god bless Microsoft c++...
#ifdef TA_OS_WIN
  typedef unsigned char		uint8_t;
  typedef unsigned int		uint;
  typedef unsigned int      	uint32_t;
  typedef signed __int64      	int64_t;
  typedef unsigned __int64    	uint64_t;
# if defined(_WIN64)
    typedef __int64	    	intptr_t;
    typedef unsigned __int64	uintptr_t;
# else
    typedef int			intptr_t;
    typedef unsigned int	uintptr_t;
# endif
  //note: prob should inline these, rather than macros, but don't want naggling little
  // type differences to cause compile issues
# define strtoll _strtoi64
# define strtoull _strtoui64
#elif (defined(TA_OS_MAC))
// and god bless Trolltech and Mac OS...
# if (TA_POINTER_SIZE == 4)
#   define ta_intptr_t int
#   define ta_uintptr_t unsigned int
# elif (TA_POINTER_SIZE == 8)
#   define ta_intptr_t qint64
#   define ta_uintptr_t quint64
# else
#   error "TA_POINTER_SIZE should be 4 or 8"
# endif
#endif // intptr_t hacks

#endif // skip over for maketa

// defaults (and maketa values) for our Trolltech/MacOS hack:
#ifndef ta_intptr_t
# define ta_intptr_t intptr_t
# define ta_uintptr_t uintptr_t
#endif

// when using QVariants for ptrs
#if (TA_POINTER_SIZE == 4)
# define QVARIANT_TO_INTPTR(qv) (qv.toInt())
#elif (TA_POINTER_SIZE == 8)
# define QVARIANT_TO_INTPTR(qv) (qv.toLongLong())
#else
# error "TA_POINTER_SIZE should be 4 or 8"
#endif

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

//note: bool defs deleted -- no modern C++ compiler doesn't provide bool

// Misc useful macros

#ifndef MAX
#define	MAX(a,b) (((a) > (b)) ? (a) : (b))
#define	MIN(a,b) (((a) < (b)) ? (a) : (b))
#define ABS(a)	((a) >= 0 ? (a) : -(a))
#endif

// hmm... these are *supposed* to be standard...
#ifndef LLONG_MAX
#define LLONG_MAX 9223372036854775807ULL
#endif

// Some OS-specific includes or types
#if (defined(TA_OS_WIN))
# ifndef __MAKETA__
#   include <io.h>
# endif
# ifdef _MSC_VER
#   define F_OK 00 // Existence only 
#   define W_OK 02 // Write permission 
#   define R_OK 04 // Read permission 
# endif
//#define F_OK 06 // Read and write permission 

typedef int pid_t; //
#endif


// Memory Management optimizers:
// when about to malloc/calloc etc. a number of bytes, call tweak_alloc to optimally tweak the
//  allocation request size -- especially useful where the allocator use 2^n blocks, or 16n blocks

// Platform	bits	Overhead	Min/Gran Size		Allocator
// Linux	32	8 bytes		16/8 bytes	Doug Lea's malloc
// Linux	64	16 bytes	24/8 bytes	Doug Lea's malloc
// Mac OS       32	0		16/16 bytes

#if (defined(TA_OS_LINUX))
// Linux uses Doug Lea's malloc, which has:
// a 2-int(32/64) overhead; 2-int + 1-ptr min, and 8-byte granularity
// strategy: round up to the amount that we will get allocated regardless
inline size_t tweak_alloc(size_t n) {
  return  (((n + (2 * sizeof(intptr_t))) + 7) & ~((size_t)0x7)) - (2 * sizeof(intptr_t));
}

#elif (defined(TA_OS_WIN))
inline size_t tweak_alloc(size_t n) {
#ifdef DEBUG
  return  (((n + 16) + 15) & ~((size_t)0xF)) - 16;
#else
  return  n;
#endif
}

#elif defined(TA_OS_MAC)
// Mac OS uses an overhead-less allocator (alloc address determines size, based on maps)
// Mac OS has 16-byte granularity
inline size_t tweak_alloc(size_t n) {
  return  (((n + 0) + 15) & ~((size_t)0xF)) - 0;
}

#else
 // don't know anything about this allocator
 //note: rem'ed out, to make sure we always have a definitive version while compiling
//inline size_t tweak_alloc(size_t n) {return n;}

#endif

// exceptions and assertions
// note: maketa doesn't (yet) grok the throw() syntax
#ifdef TA_NO_EXCEPTIONS
class ta_exception { }; //TODO: should this even exist?
#else
#ifdef __MAKETA__
class ta_exception {
public:
  void setWhat(const char* value, int len = -1);
  ta_exception();
  ta_exception(const ta_exception& cp);
  explicit ta_exception(const char* value, int len = -1);
  ~ta_exception();
  const char* what() const; // note: override
  ta_exception& operator =(const ta_exception& cp);
};
#else
#include <exception>
class ta_exception: public std::exception {
public:
  void setWhat(const char* value, int len = -1);
  
  ta_exception() throw();
  ta_exception(const ta_exception& cp) throw();
  explicit ta_exception(const char* value, int len = -1) throw();
  ~ta_exception() throw();
  
  const char* what() const throw() ; // note: override
  
  ta_exception& operator =(const ta_exception& cp) throw();
protected:
  char* m_what; // dynamically allocated
};

#endif
#endif


#ifndef __MAKETA__
inline ta_exception::ta_exception() throw() {
  m_what = NULL;
}

inline ta_exception::ta_exception(const ta_exception& cp) throw() 
#ifndef TA_NO_EXCEPTIONS
:exception(cp)
#endif
{
  m_what = NULL;
  setWhat(cp.m_what);
}

inline ta_exception::ta_exception(const char* value, int len) throw() {
  m_what = NULL;
  setWhat(value, len);
}

inline ta_exception::~ta_exception() throw()  {
  setWhat(NULL);
}

inline ta_exception& ta_exception::operator =(const ta_exception& cp) throw() {
#ifndef TA_NO_EXCEPTIONS
  exception::operator =(cp);
#endif
  setWhat(cp.m_what);
  return *this;
}

inline void ta_exception::setWhat(const char* value, int len) {
  if (m_what != NULL) {
    delete m_what;
    m_what = NULL;
  }
  if (value != NULL) {
    if (len < 0) len = (int)strlen(value);
    m_what = (char*)malloc(len + 1);
    if (m_what != NULL) strcpy(m_what, value);
  }
}

inline const char* ta_exception::what() const throw() {
  if (m_what != NULL) return m_what;
  else return "";
}
#endif // ndef __MAKETA__

// Macros for assertions and exceptions
// An Assertion is something that may be optimized out for the release version
// A Check is something that is typically never optimized away (ex. user-supplied parameter check)

//TODO: debug and non-debug version
//TEMP: until exception handling/signal handling sorted out, we 
// are hacking the behavior of Assert and Check

#ifndef __MAKETA__
#include <iostream>
#endif
/*inline void Assert(bool cond) {if (!(cond)) { int i=0; int k=1/i; std::cerr << k;}}
inline void Assert(bool cond, const char* msg) {if (!(cond)) 
    {std::cerr << msg << std::endl; int i=0; int k=1/i; std::cerr << k; } }

inline void Check(bool cond) {if (!(cond)) { int i=0; int k=1/i; std::cerr << k;}}
inline void Check(bool cond, const char* msg) {if (!(cond))  
    {std::cerr << msg << std::endl; int i=0; int k=1/i; std::cerr << k;} }*/

/*these are the final versions, once exceptions are used
inline void Assert(bool cond) {if (!(cond)) throw ta_exception();}
inline void Assert(bool cond, const char* msg) {if (!(cond)) throw ta_exception(msg);}

inline void Check(bool cond) {if (!(cond)) throw ta_exception();}
inline void Check(bool cond, const char* msg) {if (!(cond)) throw ta_exception(msg);}
*/

#define THROW(msg) throw ta_exception(msg);

// Qt Event IDs in the PDP system should be allocated here:
#ifdef TA_USE_QT
enum CustomEventType {
  FirstEvent		= 1000, //note: QT's custom events start at 1000
  cssMisc_StartupShell_EventType, // css_machine.h
  iDataViewer_SelectionChanged_EventType // ta_qtviewer.h
};
#endif

#ifndef TA_GUI
class VoidClass { // dummy class used as gui obj proxy in NO_GUI builds so all legal C++ class semantics apply
public:
  VoidClass() {} // #IGNORE
};
#endif

#endif // STDEFS_H

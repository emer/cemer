// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/CSS
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

// NOTE: This file is *not* in the make dependency list for other files.

#include "config.h"

#ifndef TAGLOBAL_H
#define TAGLOBAL_H

#include <stdlib.h>

// new Qt Event type codes -- should be globally unique, to avoid problems
#ifdef __MAKETA__
  #define TA_QT_EVENT_BASE (1000	+ 128)
  #ifdef TA_GUI
    #include "qtdefs.h" // declares common classes so qxx.h files don't need to be included
  #endif
#else
  #ifdef TA_GUI
    #include <qglobal.h>
    #include <qevent.h>
  #endif
  #ifdef TA_GUI
    #include "qtdefs.h" // declares common classes so qxx.h files don't need to be included
  #endif
  #define TA_QT_EVENT_BASE (QEvent::User	+ 128)
#endif

#define TA_QT_EVENT_SEL_CHANGED	(TA_QT_EVENT_BASE + 1)


// Useful pseudo keywords, for C++

#define override virtual  // makes override situation more clear -- use for all descendants


// define useful types -- these will be defined by QT, so don't define them if using QT
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
typedef uchar*		puchar;
//typedef const char*	pcchar;  //NOTE: too obscure, don't use this in ta/pdp code

#endif // TA_GUI

typedef unsigned char   byte;

// misc. compiler hacks for MAKETA

#ifdef __MAKETA__
#define explicit
#define struct class
#define INHERITED(c)
#define STATIC_CONST static
#define USING(b)
typedef long long int64_t; //TODO: need to figure out why maketa doesn't grok this
#else
#define INHERITED(c) typedef c inherited;
#define STATIC_CONST static const
#define USING(b) b;
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

// Some OS-specific includes or types
#if (defined(WIN32) && (!defined(CYGWIN)))
#include <io.h>
#define F_OK 00 // Existence only 
#define W_OK 02 // Write permission 
#define R_OK 04 // Read permission 
//#define F_OK 06 // Read and write permission 

typedef int pid_t;
#endif

// Memory Management optimizers:
// when about to malloc/calloc etc. a number of bytes, call tweak_alloc to optimally tweak the
//  allocation request size -- especially useful where the allocator use 2^n blocks, or 16n blocks

#if (defined(LINUX) || defined(CYGWIN))
// Linux uses Doug Lea's malloc, which has a 1-pointer overhead, and 16-byte granularity
// strategy: round up to the amount that we will get allocated regardless
inline size_t tweak_alloc(size_t n) {
  return  (((n + sizeof(void*)) + 15) & ~((size_t)0xF)) - sizeof(void*);
}

#elif (defined(WINDOWS) && (!defined(CYGWIN)))
inline size_t tweak_alloc(size_t n) {
#ifdef DEBUG
  return  (((n + 16) + 15) & ~((size_t)0xF)) - 16;
#else
  return  n;
#endif
}

#elif defined(MAC)
// Mac OS seemingly uses an overhead-less allocator (alloc address determines size, based on maps)
inline size_t tweak_alloc(size_t n) {
  return  (((n + 0) + 15) & ~((size_t)0xF)) - 0;
}

#else
 // don't know anything about this allocator
 //note: rem'ed out, to make sure we always have a definitive version while compiling
//inline size_t tweak_alloc(size_t n) {return n;}

#endif

// exceptions and assertions

#ifdef TA_NO_EXCEPTIONS
class ta_exception {
#else
#ifdef __MAKETA__
class ta_exception {
#else
#include <exception>
class ta_exception: public std::exception {
#endif
#endif

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
    if (len < 0) len = strlen(value);
    m_what = (char*)malloc(len + 1);
    if (m_what != NULL) strcpy(m_what, value);
  }
}

inline const char* ta_exception::what() const throw() {
  if (m_what != NULL) return m_what;
  else return "";
}

// Macros for assertions and exceptions
// An Assertion is something that may be optimized out for the release version
// A Check is something that is typically never optimized away (ex. user-supplied parameter check)

//TODO: debug and non-debug version
//TEMP: until exception handling/signal handling sorted out, we 
// are hacking the behavior of Assert and Check

#ifndef __MAKETA__
#include <iostream>
#endif
inline void Assert(bool cond) {if (!(cond)) {int i=0; int k=1/i;}}
inline void Assert(bool cond, const char* msg) {if (!(cond)) 
  {std::cerr << msg << "\n"; int i=0; int k=1/i;} }

inline void Check(bool cond) {if (!(cond)) throw ta_exception();}
inline void Check(bool cond, const char* msg) {if (!(cond))  
  {std::cerr << msg << "\n"; int i=0; int k=1/i;} }

/*these are the final versions, once exceptions are used
inline void Assert(bool cond) {if (!(cond)) throw ta_exception();}
inline void Assert(bool cond, const char* msg) {if (!(cond)) throw ta_exception(msg);}

inline void Check(bool cond) {if (!(cond)) throw ta_exception();}
inline void Check(bool cond, const char* msg) {if (!(cond)) throw ta_exception(msg);}
*/

#define THROW(msg) throw ta_exception(msg);


#endif // STDEFS_H

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


#endif // STDEFS_H

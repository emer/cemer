/* -*- C++ -*- */
/*=============================================================================
//									      //
// This file is part of the TypeAccess/C-Super-Script software package.	      //
//									      //
// Copyright (C) 1995 Randall C. O'Reilly, Chadley K. Dawson, 		      //
//		      James L. McClelland, and Carnegie Mellon University     //
//     									      //
// Permission to use, copy, modify, and distribute this software and its      //
// documentation for any purpose is hereby granted without fee, provided that //
// the above copyright notice and this permission notice appear in all copies //
// of the software and related documentation.                                 //
// 									      //
// Note that the PDP++ software package, which contains this package, has a   //
// more restrictive copyright, which applies only to the PDP++-specific       //
// portions of the software, which are labeled as such.			      //
//									      //
// Note that the taString class, which is derived from the GNU String class,  //
// is Copyright (C) 1988 Free Software Foundation, written by Doug Lea, and   //
// is covered by the GNU General Public License, see ta_string.h.             //
// The iv_graphic library and some iv_misc classes were derived from the      //
// InterViews morpher example and other InterViews code, which is             //
// Copyright (C) 1987, 1988, 1989, 1990, 1991 Stanford University             //
// Copyright (C) 1991 Silicon Graphics, Inc.				      //
//									      //
// THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,         //
// EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY 	      //
// WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  	      //
// 									      //
// IN NO EVENT SHALL CARNEGIE MELLON UNIVERSITY BE LIABLE FOR ANY SPECIAL,    //
// INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND, OR ANY DAMAGES  //
// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER OR NOT     //
// ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF LIABILITY,      //
// ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS        //
// SOFTWARE. 								      //
==============================================================================*/

// safeptrs_so.h -- safe ptrs for ref counted inventor objects
// NOTE: this file must be MAKETA scannable -- therefore, no concrete type info
// should be required in this header file (ex. this is why all routines are in .cc file)

#ifndef SAFEPTRS_SO_H
#define SAFEPTRS_SO_H

#ifdef TA_USE_INVENTOR

#include "taiqtso_def.h"

// externals
class SoBase;			//
class SoNode;			//
class SoGroup;		//
class SoSeparator;		//
class SoSeparatorKit;		// #IGNORE
class SoSelection;		// #IGNORE
class SoTransform;		// #IGNORE


class TAIQTSO_API SoPtr_impl { // ##NO_INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS "safe" ptr for SoBase objects -- automatically does ref counts
public:
  SoPtr_impl() {m_ptr = 0;}
  ~SoPtr_impl();
protected:
  SoBase*	m_ptr;
  void		set(SoBase* src);
};

template<class T>
class SoPtr: public SoPtr_impl { // "safe" ptr for SoBase objects -- automatically does ref counts
public:
  T*		ptr() const {return (T*)m_ptr;}

  operator T*() const {return (T*)m_ptr;}
  T* operator->() const {return (T*)m_ptr;}

protected:
  void		set(T* src) {SoPtr_impl::set((SoBase*)src);}
};

// macro for creating safe ptrs of SoBase classes

#define SoPtr_Of(T)  class TAIQTSO_API T ## Ptr: public SoPtr<T> { \
public: \
  T* operator=(T ## Ptr& src) {set((T*)src.m_ptr); return (T*)m_ptr;} \
  T* operator=(T* src) {set(src); return (T*)m_ptr;} \
  T ## Ptr() {} \
  T ## Ptr(T ## Ptr& src) {set((T*)src.m_ptr);} \
  T ## Ptr(T* src) {set(src);} \
}

// Common node ptr types are predefined here, for convenience

SoPtr_Of(SoNode);

SoPtr_Of(SoGroup);

SoPtr_Of(SoSeparator);

SoPtr_Of(SoSeparatorKit);

SoPtr_Of(SoSelection);

SoPtr_Of(SoTransform);

// macros for creating SoGroup-derivitives that cache a single instance

#define SOCACHEGROUP_DECL(T) \
protected: \
  static T*	T ## _inst; \
  T() {Initialize(); if (! T ## _inst) T ## _inst = this;} \
public: \
  static T*	GetInstance(); \
  ~T() { if (T ## _inst == this) T ## _inst= NULL; Destroy();}

#define SOCACHEGROUP_IMPL(T) \
  T* T::T ## _inst = NULL; \
  T* T::GetInstance() {if (T ## _inst) return T ## _inst; else return T ## _inst = new T();}

#endif
#endif

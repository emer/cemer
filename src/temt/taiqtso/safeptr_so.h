// Copyright, 1995-2005, Regents of the University of Colorado,
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


// safeptr_so.h -- safe ptrs for ref counted inventor objects
// NOTE: this file must be MAKETA scannable -- therefore, no concrete type info
// should be required in this header file (ex. this is why all routines are in .cc file)

#ifndef SAFEPTR_SO_H
#define SAFEPTR_SO_H

#include "taiqtso_def.h"

#ifdef TA_USE_INVENTOR

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
  inline operator 	bool() const {return (m_ptr);} // for ptr-like NULL checks
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

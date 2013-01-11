// Copyright, 1995-2013, Regents of the University of Colorado,
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

#ifndef taSmartPtr_h
#define taSmartPtr_h 1

// parent includes:
#include "ta_def.h"

// member includes:
#include <taBase>

// declare all other types mentioned but not required to include:
class TypeDef; // 


class TA_API taSmartPtr {
  // ##NO_INSTANCE ##NO_TOKENS ##SMART_POINTER "safe" ptr for taBase objects -- automatically does ref counts; designed to be binary compatible with taBase*
public:
  static TypeDef*       GetBaseType(TypeDef* this_typ);
    // returns a best-guess min type -- hacks by looking at the name

  inline taBase*        ptr() const {return m_ptr;}
  inline void           set(taBase* src) {taBase::SetPointer(&m_ptr, src);}

  inline                operator bool() const {return (m_ptr);}
  inline bool           operator !() const {return !(m_ptr);}
    // needed to avoid ambiguities when we have derived T* operators
  inline                operator taBase*() const {return m_ptr;}
  inline taBase*        operator->() const {return m_ptr;}
  inline taBase**       operator&() const {return &m_ptr;}
    //note: operator& is *usually* verbotten but we are binary compat -- it simplifies low-level compat
  inline taBase*        operator=(const taSmartPtr& src)
    {set(src.m_ptr); return m_ptr;}
  inline taBase*        operator=(taBase* src) {set(src); return m_ptr;}

  inline void           CutLinks() {set(NULL);}

  taSmartPtr(taBase* val) {m_ptr = NULL; set(val);}
  taSmartPtr() {m_ptr = NULL;}
  ~taSmartPtr() {CutLinks();} //note: DO NOT change to be virtual!
protected:
  mutable taBase*       m_ptr;
private:
  taSmartPtr(const taSmartPtr& src); // not defined
};

#endif // taSmartPtr_h

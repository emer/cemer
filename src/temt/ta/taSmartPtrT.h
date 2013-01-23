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

#ifndef taSmartPtrT_h
#define taSmartPtrT_h 1

// parent includes:
#include <taSmartPtr>

// member includes:

// declare all other types mentioned but not required to include:

TypeDef_Of(taSmartPtrT);

template<class T>
class taSmartPtrT: public taSmartPtr {
public:
  inline T*     ptr() const {return (T*)m_ptr;} // typed alias for the base version

  inline        operator T*() const {return (T*)m_ptr;}
  inline T*     operator->() const {return (T*)m_ptr;}
  inline T**    operator&() const {return (T**)(&m_ptr);}
    //note: operator& is *usually* verbotten but we are binary compat -- it simplifies low-level compat
  T*            operator=(const taSmartPtrT<T>& src)
    {set((T*)src.m_ptr); return (T*)m_ptr;}
    //NOTE: copy only implies ptr, NOT the owner!
  T*            operator=(T* src) {set(src); return (T*)m_ptr;}
  T&            operator*() const {return *(T*)m_ptr;}

  friend bool   operator==(const taSmartPtrT<T>& a, const taSmartPtrT<T>& b)
    {return (a.m_ptr == b.m_ptr);}

  taSmartPtrT(T* val): taSmartPtr(val) {}
  taSmartPtrT() {}

private:
  taSmartPtrT(const taSmartPtrT<T>& src); // not defined
};

// macro for creating smart ptrs of taBase classes

#define SmartPtr_Of(T) TypeDef_Of(T ## Ptr); \
  typedef taSmartPtrT<T> T ## Ptr;

SmartPtr_Of(taBase);            // basic ptr if you don't know the type

#endif // taSmartPtrT_h

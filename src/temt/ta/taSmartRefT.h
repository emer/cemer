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

#ifndef taSmartRefT_h
#define taSmartRefT_h 1

// parent includes:
#include <taSmartRef>

// member includes:

// declare all other types mentioned but not required to include:


template<class T>
class taSmartRefT: public taSmartRef {
public:
  inline T*             ptr() const {return (T*)m_ptr;} // typed alias for the base version

  inline                operator T*() const {return (T*)m_ptr;}
  inline T*             operator->() const {return (T*)m_ptr;}
  T*                    operator=(const taSmartRefT<T>& src) {set((T*)src.m_ptr); return (T*)m_ptr;}
  T*                    operator=(T* src) {set(src); return (T*)m_ptr;}
  override TypeDef*     GetBaseType() const {return T::StatTypeDef(0);}
  TypeDef*              GetDataTypeDef() const
    {return (m_ptr) ? m_ptr->GetTypeDef() : T::StatTypeDef(0);}
  taSmartRefT() {}  //

#ifndef __MAKETA__
  template <class U>
  inline friend bool    operator==(U* a, const taSmartRefT<T>& b)
    {return (a == b.m_ptr);}
  template <class U>
  inline friend bool    operator!=(U* a, const taSmartRefT<T>& b)
    {return (a != b.m_ptr);}
  template <class U>
  friend bool           operator==(const taSmartRefT<T>& a, U* b)
    {return (a.m_ptr == b);}
  template <class U>
  friend bool           operator!=(const taSmartRefT<T>& a, U* b)
    {return (a.m_ptr != b);}
  template <class U>
  friend bool           operator==(const taSmartRefT<T>& a, const taSmartRefT<U>& b)
    {return (a.m_ptr == b.m_ptr);}
  template <class U>
  friend bool           operator!=(const taSmartRefT<T>& a, const taSmartRefT<U>& b)
    {return (a.m_ptr != b.m_ptr);}
private:
  taSmartRefT(const taSmartRefT<T>& src); // not defined
#endif
};

// macro for creating smart refs of taBase classes
#define SmartRef_Of(T,td)  typedef taSmartRefT<T> T ## Ref

SmartRef_Of(taBase,);           // basic ref if you don't know the type
SmartPtr_Of(taBase);            // basic ptr if you don't know the type


#endif // taSmartRefT_h

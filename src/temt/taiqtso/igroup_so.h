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


// igroup_so.h -- extensions to group object
// NOTE: this file must be MAKETA scannable -- therefore, no concrete type info
// should be required in this header file (ex. this is why all routines are in .cc file)

#ifndef IGROUP_SO_H
#define IGROUP_SO_H



// externals
class SoBase;			// #IGNORE
class SoNode;			// #IGNORE
class SoGroup;		// #IGNORE
class SoSeparator;		// #IGNORE


class SoPtr_impl { // "safe" ptr for SoBase objects -- automatically does ref counts
public:
  SoPtr_impl() {m_ptr = 0;}
  virtual ~SoPtr_impl();
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

#define SoPtr_Of(T)  class T ## Ptr: public SoPtr<T> { \
public: \
  T* operator=(T ## Ptr& src) {set((T*)src.m_ptr); return (T*)m_ptr;} \
  T* operator=(T* src) {set(src); return (T*)m_ptr;} \
  T ## Ptr() {} \
  T ## Ptr(T ## Ptr& src) {set((T*)src.m_ptr);} \
  T ## Ptr(T* src) {set(src);} \
}


SoPtr_Of(SoNode);

SoPtr_Of(SoGroup);

SoPtr_Of(SoSeparator);

/*
class SoNodePtr: public SoBasePtr {
public:
  SoNode*	ptr() const;
  SoNodePtr(): SoBasePtr() {}
  SoNodePtr(SoNodePtr& src);
  SoNodePtr(SoNode* src);

  SoNodePtr& operator=(SoNodePtr& src);
  SoNode* operator=(SoNode* src);
  operator SoNode*() const;
  SoNode* operator->() const;
};

class SoGroupPtr: public SoNodePtr {
public:
  SoGroup*	ptr() const;
  SoGroupPtr(): SoNodePtr() {}
  SoGroupPtr(SoGroupPtr& src);
  SoGroupPtr(SoGroup* src);

  SoGroupPtr& operator=(SoGroupPtr& src);
  SoGroup* operator=(SoGroup* src);
  operator SoGroup*() const;
  SoGroup* operator->() const;
};

class SoSeparatorPtr: public SoGroupPtr {
public:
  SoSeparator*	ptr() const;
  SoSeparatorPtr(): SoGroupPtr() {}
  SoSeparatorPtr(SoSeparatorPtr& src);
  SoSeparatorPtr(SoSeparator* src);

  SoSeparatorPtr& operator=(SoSeparatorPtr& src);
  SoSeparator* operator=(SoSeparator* src);
  operator SoSeparator*() const;
  SoSeparator* operator->() const;
};
*/
#endif

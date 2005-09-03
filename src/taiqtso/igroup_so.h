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

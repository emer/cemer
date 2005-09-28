// Copyright (C) 1995-2005 Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
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

// css_qttype.h: Qt-css type interface

#ifndef CSS_QTTYPE_H
#define CSS_QTTYPE_H

#include "ta_qttype_def.h"
//#include "ta_qtdata.h"

#include <qobject.h>

// forward declarations
class cssEl; //
class cssiType; //

//class cssClassInst;
//class cssMbrScriptFun;
class cssEnumType;
//class cssProgSpace;
//class cssClassType;
//class cssClassInst;



//////////////////////////////
//   taiTypes for css      //
//////////////////////////////

// no actual functions
#define CSS_TYPE_FUNS(x,y)


class cssiType_QObj : public QObject { // #IGNORE delegate for cssitype
friend class cssiType;
  Q_OBJECT
public:
  cssiType_QObj(cssiType* owner_);
  ~cssiType_QObj();
public slots:
  void		CallEdit();     // invoke an edit dialog
protected:
  cssiType* 	owner;
};

class cssiType : public taiType {
  // css types call their corresponding type for all of the functions
friend class cssiType_QObj;
public:
  cssEl*	orig_obj;	// original object that this type is based on (ie., member)
  void*		cur_base;	// thse are used only once, so base is kept here
  taiType*	use_it;		// if non-null, use this it instead of typ->it


  int		BidForType(TypeDef*)	{ return 0; } // don't do any real types!
  virtual void		CallEdit() {}     // invoke an edit dialog (normally called from qobj)
  override taiData*	GetDataRep(taiDataHost* host_, taiData* par, QWidget* gui_parent);

  void		GetImage(taiData* dat, void* base);
  void		GetImage(taiData* dat)  { GetImage(dat, cur_base); }

  void		GetValue(taiData* dat, void* base);
  void		GetValue(taiData* dat)  { GetValue(dat, cur_base); }

  CSS_TYPE_FUNS(cssiType,taiType);
  cssiType(cssEl* orgo, TypeDef* tp, void* bs, bool use_ptr_type = false);
  // if use_ptr_type is true, type given is the base type, make it a pointer
  ~cssiType();

protected:
  cssiType_QObj*	qobj;
  void		Assert_QObj(); // makes sure it is created
};


class cssiROType : public cssiType {
  // a css read-only type
public:
  override taiData*	GetDataRep(taiDataHost* host_, taiData* par, QWidget* gui_parent);

  void		GetImage(taiData* dat, void* base);
  void		GetImage(taiData* dat)  { GetImage(dat, cur_base); }

  void		GetValue(taiData*, void*) {}
  void		GetValue(taiData* dat)  { GetValue(dat, cur_base); }

  CSS_TYPE_FUNS(cssiROType,cssiType);
  cssiROType(cssEl* orgo, TypeDef* tp, void* bs, bool use_ptr_type = false);
};

class cssiEnumType : public cssiType {
  // a css enum type
public:
  cssEnumType*	enum_type;

  override taiData*	GetDataRep(taiDataHost* host_, taiData* par, QWidget* gui_parent);

  void		GetImage(taiData* dat, void* base);
  void		GetImage(taiData* dat)  { GetImage(dat, cur_base); }

  void		GetValue(taiData* dat, void* base);
  void		GetValue(taiData* dat)  { GetValue(dat, cur_base); }

  CSS_TYPE_FUNS(cssiEnumType,cssiType);
  cssiEnumType(cssEl* orgo, cssEnumType* enm_typ, void* bs);
};

class cssiClassType : public cssiType {
  // a css class type
public:

  override taiData*	GetDataRep(taiDataHost* host_, taiData* par, QWidget* gui_parent);

  override void		CallEdit();     // invoke an edit dialog (normally called from qobj)
  void          GetImage(taiData* dat, void* base);
  void          GetImage(taiData* dat)  { GetImage(dat, cur_base); }

  void          GetValue(taiData* dat, void* base);
  void          GetValue(taiData* dat)  { GetValue(dat, cur_base); }


  CSS_TYPE_FUNS(cssiClassType,cssiType);
  cssiClassType(cssEl* orgo, void* bs);

};

class cssiArrayType : public cssiType {
  // a css array type
public:
  override void		CallEdit();     // invoke an edit dialog (normally called from qobj)

  override taiData*	GetDataRep(taiDataHost* host_, taiData* par, QWidget* gui_parent);

  void          GetImage(taiData* dat, void* base) {}
  void          GetImage(taiData* dat)  { GetImage(dat, cur_base); }

  void          GetValue(taiData* dat, void* base) {}
  void          GetValue(taiData* dat)  { GetValue(dat, cur_base); }

  CSS_TYPE_FUNS(cssiArrayType,taiType);
  cssiArrayType(cssEl* orgo, void* bs);
};


#endif // CSS_QTTYPE_H

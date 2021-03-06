// Copyright 2006-2018, Regents of the University of Colorado,
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


// css_qttype.h: Qt-css type interface

#ifndef CSS_QTTYPE_H
#define CSS_QTTYPE_H

#include "css_def.h"

#include <QObject>
#include <taiType>
#include <taiWidgetMethodMenu>

// forward declarations
class cssEl; //
class cssiType; //
class cssEnumType; //

//////////////////////////////
//   taiTypes for css      //
//////////////////////////////

// no actual functions
#define CSS_TYPE_FUNS(x,y)


class CSS_API cssiType_QObj : public QObject {
  // #IGNORE delegate for cssitype
friend class cssiType;
  Q_OBJECT
public:
  cssiType_QObj(cssiType* owner_);
  ~cssiType_QObj();
public slots:
  void          CallEdit();     // invoke an edit dialog
protected:
  cssiType*     owner;
};

class CSS_API cssiType : public taiType {
  // css types call their corresponding type for all of the functions
friend class cssiType_QObj;
public:
  cssEl*        orig_obj;       // original object that this type is based on (ie., member)
  void*         cur_base;       // thse are used only once, so base is kept here
  taiType*      use_it;         // if non-null, use this it instead of typ->it


  int           BidForType(TypeDef*) override    { return 0; } // don't do any real types!
  virtual void          CallEdit() {}     // invoke an edit dialog (normally called from qobj)
  taiWidget*     GetWidgetRep(IWidgetHost* host_, taiWidget* par, QWidget* gui_parent_,
                                   taiType* parent_type_ = NULL, int flags = 0, MemberDef* mbr = NULL) override;

  void GetImage(taiWidget* dat, const void* base) override;
  void          GetImage(taiWidget* dat)  { GetImage(dat, cur_base); }

  void GetValue(taiWidget* dat, void* base) override;
  void          GetValue(taiWidget* dat)  { GetValue(dat, cur_base); }

  CSS_TYPE_FUNS(cssiType,taiType);
  cssiType(cssEl* orgo, TypeDef* tp, void* bs, bool use_ptr_type = false);
  // if use_ptr_type is true, type given is the base type, make it a pointer
  ~cssiType();

protected:
  cssiType_QObj*        qobj;
  void          Assert_QObj(); // makes sure it is created
};

class CSS_API cssiROType : public cssiType {
  // a css read-only type
public:
  taiWidget*     GetWidgetRep(IWidgetHost* host_, taiWidget* par, QWidget* gui_parent_,
                                   taiType* parent_type_ = NULL, int flags = 0, MemberDef* mbr = NULL) override;

  void GetImage(taiWidget* dat, const void* base) override;
  void          GetImage(taiWidget* dat)  { GetImage(dat, cur_base); }

  void GetValue(taiWidget*, void*) override {}
  void          GetValue(taiWidget* dat)  { GetValue(dat, cur_base); }

  CSS_TYPE_FUNS(cssiROType,cssiType);
  cssiROType(cssEl* orgo, TypeDef* tp, void* bs, bool use_ptr_type = false);
};

class CSS_API cssiEnumType : public cssiType {
  // a css enum type
public:
  cssEnumType*  enum_type;

  taiWidget*     GetWidgetRep(IWidgetHost* host_, taiWidget* par, QWidget* gui_parent_,
                                   taiType* parent_type_ = NULL, int flags = 0, MemberDef* mbr = NULL) override;

  void GetImage(taiWidget* dat, const void* base) override;
  void          GetImage(taiWidget* dat)  { GetImage(dat, cur_base); }

  void GetValue(taiWidget* dat, void* base) override;
  void          GetValue(taiWidget* dat)  { GetValue(dat, cur_base); }

  CSS_TYPE_FUNS(cssiEnumType,cssiType);
  cssiEnumType(cssEl* orgo, cssEnumType* enm_typ, void* bs);
};

class CSS_API cssiClassType : public cssiType {
  // a css class type
public:
  taiWidget*     GetWidgetRep(IWidgetHost* host_, taiWidget* par, QWidget* gui_parent_,
                                   taiType* parent_type_ = NULL, int flags = 0, MemberDef* mbr = NULL) override;

  void         CallEdit() override;     // invoke an edit dialog (normally called from qobj)
  void GetImage(taiWidget* dat, const void* base) override;
  void          GetImage(taiWidget* dat)  { GetImage(dat, cur_base); }

  void GetValue(taiWidget* dat, void* base) override;
  void          GetValue(taiWidget* dat)  { GetValue(dat, cur_base); }


  CSS_TYPE_FUNS(cssiClassType,cssiType);
  cssiClassType(cssEl* orgo, void* bs);

};

class CSS_API cssiArrayType : public cssiType {
  // a css array type
public:
  void         CallEdit() override;     // invoke an edit dialog (normally called from qobj)

  taiWidget*     GetWidgetRep(IWidgetHost* host_, taiWidget* par, QWidget* gui_parent_,
                                   taiType* parent_type_ = NULL, int flags = 0, MemberDef* mbr = NULL) override;

  void GetImage(taiWidget* dat, const void* base) override {}
  void          GetImage(taiWidget* dat)  { GetImage(dat, cur_base); }

  void GetValue(taiWidget* dat, void* base) override {}
  void          GetValue(taiWidget* dat)  { GetValue(dat, cur_base); }

  CSS_TYPE_FUNS(cssiArrayType,taiType);
  cssiArrayType(cssEl* orgo, void* bs);
};

#endif // CSS_QTTYPE_H

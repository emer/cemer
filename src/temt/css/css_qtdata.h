// Copyright, 1995-2007, Regents of the University of Colorado,
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


// css_qt.h: Qt-css interface

#ifndef CSS_QTDATA_H
#define CSS_QTDATA_H

#include "css_def.h"
#include <taiWidget>
#include <taiType_List>
#include <taiWidget_List>
#include <taiWidgetActions>
#include <taiMethMenu>

// forward declarations
class cssClassInst;
class cssMbrScriptFun;
class cssEnumType;
class cssProgSpace;


//////////////////////////////
//   taiWidget for css        //
//////////////////////////////

class CSS_API cssiPolyData : public taiWidget {
  // supports INLINE members for css
public:
  cssClassInst* obj;
  taiWidget_List   data_el;
  taiType_List  type_el; // type elements (not stored on classes, so kept here)

  cssiPolyData(cssClassInst* ob, TypeDef* typ_, IWidgetHost* host_, taiWidget* par,
               QWidget* gui_parent, int flags_ = 0);
  ~cssiPolyData();

  inline QFrame* rep() const {
    // note: actual class may be subclass of QFrame
    return static_cast<QFrame*>(m_rep.data());
  }

protected:
  virtual void  Constr(QWidget* gui_parent_);
  virtual void  GetImage_impl(const void* base);
  virtual void  GetValue_impl(void* base) const;
};

class CSS_API cssiMethMenu : public taiMethMenu {
  // css member functions of css classes
  Q_OBJECT
public:
  cssClassInst*         obj;            // parent object
  cssProgSpace*         top;            // top-level progspace where edit was called
  cssMbrScriptFun*      css_fun;        // the function itself
  cssClassInst*         arg_obj;        // argument object (if necc)

  cssiMethMenu(cssClassInst* ob, cssProgSpace* tp, cssMbrScriptFun* cfn,
        TypeDef* typ_, IWidgetHost* host_, taiWidget* par, QWidget* gui_parent, int flags_ = 0);
//  ~cssiMethMenu();

  void          ShowReturnVal(cssEl* rval);
  void          ApplyBefore();  // apply changes before performing menu call
  void          UpdateAfter();  // update display after performing menu call

  void          GenerateScript(); // output script code equivalent if recording

  override void         AddToMenu(taiWidgetActions* menu);

public slots:
  void          CallFun();      // (override) call the function..
};


#endif // CSS_QTDATA_H

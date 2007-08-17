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


// css_qt.h: Qt-css interface

#ifndef CSS_QTDATA_H
#define CSS_QTDATA_H

#include "ta_qttype.h"
#include "ta_qtdata.h"
#include "css_def.h"

// forward declarations
class cssClassInst;
class cssMbrScriptFun;
class cssEnumType;
class cssProgSpace;


//////////////////////////////
//   taiData for css       //
//////////////////////////////

class CSS_API cssiPolyData : public taiData {
  // supports INLINE members for css
public:
  cssClassInst*	obj;
  taiDataList 	data_el;
  taiType_List type_el;	// type elements (not stored on classes, so kept here)


  cssiPolyData(cssClassInst* ob, TypeDef* typ_, IDataHost* host_, taiData* par,
      QWidget* gui_parent, int flags_ = 0);
  ~cssiPolyData();

  inline QFrame*	rep() const { return (QFrame*)(QWidget*)m_rep; } //note: actual class may be subclass of QFrame

protected:
  virtual void	Constr(QWidget* gui_parent_);
  virtual void  GetImage_impl(const void* base);
  virtual void	GetValue_impl(void* base) const;

};


class CSS_API cssiMethMenu : public taiMethMenu {
  // css member functions of css classes
  Q_OBJECT
public:
  cssClassInst*		obj;		// parent object
  cssProgSpace*		top;		// top-level progspace where edit was called
  cssMbrScriptFun*	css_fun; 	// the function itself
  cssClassInst*		arg_obj; 	// argument object (if necc)

  cssiMethMenu(cssClassInst* ob, cssProgSpace* tp, cssMbrScriptFun* cfn,
	TypeDef* typ_, IDataHost* host_, taiData* par, QWidget* gui_parent, int flags_ = 0);
//  ~cssiMethMenu();

  void		ShowReturnVal(cssEl* rval);
  void		ApplyBefore();	// apply changes before performing menu call
  void		UpdateAfter();	// update display after performing menu call

  void		GenerateScript(); // output script code equivalent if recording

  void		AddToMenu(taiMenu* menu);

public slots:
  void		CallFun();	// (override) call the function..
};


#endif // CSS_QTDATA_H

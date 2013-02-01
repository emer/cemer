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

#ifndef taiMethodDefMenu_h
#define taiMethodDefMenu_h 1

// parent includes:
#include <taiTypeInfoBase>

// member includes:

// declare all other types mentioned but not required to include:

class TA_API taiMethodDefMenu : public taiTypeInfoBase {
  // Menu for memberdefs of a typedef in the object with a MDTYPE_xxx option
  INHERITED(taiTypeInfoBase)
public:
  MethodDef*    md;
  MethodSpace*  sp;

  override void         GetMenu() {GetMenu(NULL);}
  virtual void          GetMenu(const taiMenuAction* actn);
  virtual void          UpdateMenu(const taiMenuAction* actn = NULL);

  virtual MethodDef*    GetValue();

  taiMethodDefMenu(taiWidgetActions::RepType rt, int ft, MethodDef* md_,
    MemberDef* memb_md_, TypeDef* typ_, IWidgetHost* host, taiWidget* par,
    QWidget* gui_parent_, int flags_ = 0);
};

#endif // taiMethodDefMenu_h

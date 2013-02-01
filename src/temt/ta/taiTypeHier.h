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

#ifndef taiTypeHier_h
#define taiTypeHier_h 1

// parent includes:
#include <taiWidget>

// member includes:
#include <taiWidgetActions>

// declare all other types mentioned but not required to include:

class TA_API taiTypeHier : public taiWidget {
  // for menus of type hierarchy
public:
  taiWidgetActions*   ta_actions;
  bool          ownflag;
  bool          enum_mode; // when set, creates a hier of enum TypeDefs, for picking an enum type

  virtual void  GetMenu() {GetMenu(NULL);}
  virtual void  GetMenu(const iMenuAction* acn);
  virtual void  UpdateMenu() {UpdateMenu(NULL);}
  virtual void  UpdateMenu(const iMenuAction* acn);

  override QWidget* GetRep();
  virtual void          GetImage(TypeDef* ths);
  virtual TypeDef*      GetValue();

  taiTypeHier(taiWidgetActions::RepType rt, int ft, TypeDef* typ_, IWidgetHost* host, taiWidget* par,
    QWidget* gui_parent_, int flags_ = 0);
  taiTypeHier(taiWidgetMenu* existing_menu, TypeDef* typ_, IWidgetHost* host, taiWidget* par,
    QWidget* gui_parent_, int flags_ = 0);
  ~taiTypeHier(); //
protected:
  bool          AddType_Enum(TypeDef* typ_);
  bool          AddType_Class(TypeDef* typ_);
  int           CountChildren(TypeDef* typ_);
  int           CountEnums(TypeDef* typ_);
  virtual void  GetMenu_impl(taiWidgetActions* menu, TypeDef* typ_, const iMenuAction* acn);
  void  GetMenu_Enum_impl(taiWidgetActions* menu, TypeDef* typ_, const iMenuAction* acn); //
};

#endif // taiTypeHier_h

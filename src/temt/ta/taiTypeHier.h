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
#include <taiData>

// member includes:
#include <taiActions>

// declare all other types mentioned but not required to include:

class TA_API taiTypeHier : public taiData {
  // for menus of type hierarchy
public:
  taiActions*   ta_actions;
  bool          ownflag;
  bool          enum_mode; // when set, creates a hier of enum TypeDefs, for picking an enum type

  virtual void  GetMenu() {GetMenu(NULL);}
  virtual void  GetMenu(const taiMenuAction* acn);
  virtual void  UpdateMenu() {UpdateMenu(NULL);}
  virtual void  UpdateMenu(const taiMenuAction* acn);

  override QWidget* GetRep();
  virtual void          GetImage(TypeDef* ths);
  virtual TypeDef*      GetValue();

  taiTypeHier(taiActions::RepType rt, int ft, TypeDef* typ_, IDataHost* host, taiData* par,
    QWidget* gui_parent_, int flags_ = 0);
  taiTypeHier(taiMenu* existing_menu, TypeDef* typ_, IDataHost* host, taiData* par,
    QWidget* gui_parent_, int flags_ = 0);
  ~taiTypeHier(); //
protected:
  bool          AddType_Enum(TypeDef* typ_);
  bool          AddType_Class(TypeDef* typ_);
  int           CountChildren(TypeDef* typ_);
  int           CountEnums(TypeDef* typ_);
  virtual void  GetMenu_impl(taiActions* menu, TypeDef* typ_, const taiMenuAction* acn);
  void  GetMenu_Enum_impl(taiActions* menu, TypeDef* typ_, const taiMenuAction* acn); //
};

#endif // taiTypeHier_h

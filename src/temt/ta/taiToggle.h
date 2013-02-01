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

#ifndef taiToggle_h
#define taiToggle_h 1

// parent includes:
#include <taiData>

// member includes:

// declare all other types mentioned but not required to include:
class iCheckBox; //

class TA_API taiToggle : public taiData {
public:
  inline iCheckBox*     rep() const { return (iCheckBox*)(QWidget*)m_rep; }

  taiToggle(TypeDef* typ_, IWidgetHost* host, taiData* par, QWidget* gui_parent_, int flags = 0);
//  ~taiToggle() {}

  void  GetImage(bool val);
  bool  GetValue() const;
protected:
  override void         GetImageVar_impl(const Variant& val) {GetImage(val.toBool());}
  override void         GetValueVar_impl(Variant& val) const {val = GetValue();}
};

#endif // taiToggle_h

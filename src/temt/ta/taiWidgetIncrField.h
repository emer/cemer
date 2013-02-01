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

#ifndef taiWidgetIncrField_h
#define taiWidgetIncrField_h 1

// parent includes:
#include <taiWidget>

// member includes:

// declare all other types mentioned but not required to include:
class iSpinBox; //


// this is for integers -- includes up and down arrow buttons
class TA_API taiWidgetIncrField : public taiWidget { // increment/decrement field
  Q_OBJECT
public:
  inline iSpinBox*      rep() const { return (iSpinBox*)(QWidget*)m_rep; }
  taiWidgetIncrField(TypeDef* typ_, IWidgetHost* host, taiWidget* par, QWidget* gui_parent_, int flags = 0);
    // uses flags: flgPosOnly

  virtual void          setMinimum(const Variant& min); //NOTE: for iSpinBox, this must be an int
  virtual void          setMaximum(const Variant& max); //NOTE: for iSpinBox, this must be an int

  void                  GetImage(int val);
  int                   GetValue() const;

protected slots:
  void                  selectionChanged();

protected:
  override void         GetImageVar_impl(const Variant& val) {GetImage(val.toInt());}
  override void         GetValueVar_impl(Variant& val) const {val = GetValue();}
  override void         this_GetEditActionsEnabled(int& ea); // for when control is clipboard handler
  override void         this_EditAction(int param); // for when control is clipboard handler
  override void         this_SetActionsEnabled(); // for when control is clipboard handler
};

#endif // taiWidgetIncrField_h

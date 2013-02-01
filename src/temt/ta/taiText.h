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

#ifndef taiText_h
#define taiText_h 1

// parent includes:
#include <taiData>

// member includes:

// declare all other types mentioned but not required to include:
class iLineEdit; //
class QToolButton; //


class TA_API taiText : public taiData {
  // Base class for string edit controls that might have a "..." button to bring up an editor dialog or somesuch.
  Q_OBJECT
  INHERITED(taiData)
public:
  taiText(TypeDef* typ_, IWidgetHost* host, taiData* par, QWidget* gui_parent_,
          int flags, bool needs_edit_button, const char *tooltip);

  iLineEdit*            rep() const;
  override bool         fillHor() { return true; }
  void                  setMinCharWidth(int num); // hint for min chars, 0=no min

  void                  GetImage(const String& val);
  String                GetValue() const;

protected slots:
  void                  selectionChanged();
  virtual void          btnEdit_clicked(bool) = 0;
  virtual void          lookupKeyPressed() = 0;

protected:
#ifndef __MAKETA__
  QPointer<iLineEdit>   leText;
  QPointer<QToolButton> btnEdit; // if requested, button to invoke dialog editor
#endif
  override void         GetImage_impl(const void* base)      { GetImage(*((String*)base)); }
  override void         GetValue_impl(void* base) const      { *((String*)base) = GetValue(); }
  override void         GetImageVar_impl(const Variant& val) { GetImage(val.toString()); }
  override void         GetValueVar_impl(Variant& val) const { val.updateFromString(GetValue()); }
  override void         this_GetEditActionsEnabled(int& ea); // for when control is clipboard handler
  override void         this_EditAction(int param); // for when control is clipboard handler
  override void         this_SetActionsEnabled(); // for when control is clipboard handler
};


#endif // taiText_h

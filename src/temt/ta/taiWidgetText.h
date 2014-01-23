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

#ifndef taiWidgetText_h
#define taiWidgetText_h 1

// parent includes:
#include <taiWidget>

// member includes:

// declare all other types mentioned but not required to include:
class iLineEdit; //
class QToolButton; //


class TA_API taiWidgetText : public taiWidget {
  // Base class for string edit controls that might have a "..." button to bring up an editor dialog or somesuch.
  Q_OBJECT
  INHERITED(taiWidget)
public:
  taiWidgetText(TypeDef* typ_, IWidgetHost* host, taiWidget* par, QWidget* gui_parent_,
          int flags, bool needs_edit_button, const char *tooltip);

  iLineEdit*            rep() const;
  bool         fillHor() override { return true; }
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
  void         GetImage_impl(const void* base)      override { GetImage(*((String*)base)); }
  void         GetValue_impl(void* base) const      override { *((String*)base) = GetValue(); }
  void         GetImageVar_impl(const Variant& val) override { GetImage(val.toString()); }
  void         GetValueVar_impl(Variant& val) const override { val.updateFromString(GetValue()); }
  void         this_GetEditActionsEnabled(int& ea) override; // for when control is clipboard handler
  void         this_EditAction(int param) override; // for when control is clipboard handler
  void         this_SetActionsEnabled() override; // for when control is clipboard handler
};


#endif // taiWidgetText_h

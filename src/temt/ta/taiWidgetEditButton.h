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

#ifndef taiWidgetEditButton_h
#define taiWidgetEditButton_h 1

// parent includes:
#include <taiWidgetMenuButton>

// member includes:
#include <taiWidget_List>

// declare all other types mentioned but not required to include:

class TA_API taiWidgetEditButton : public taiWidgetMenuButton {
  // actually an edit menu... -- flgReadOnly creates menu which only allows for #EDIT_READ_ONLY members
  Q_OBJECT
  INHERITED(taiWidgetMenuButton)
public:
  static taiWidgetEditButton* New(void* base, taiEdit *taie, TypeDef* typ_, IWidgetHost* host_, taiWidget* par,
      QWidget* gui_parent_, int flags_ = 0); // uses flags: flgReadOnly, flgEditOnly -- internally sets flgEditOnly if appropriate

  ~taiWidgetEditButton();

public slots:
  virtual void  Edit();         // edit callback
  void setRepLabel(const char* label);
protected:
  void*         cur_base;
  taiEdit*      ie;
  taiWidget_List   meth_el;        // method elements

  using inherited::GetImage_impl;
  void         GetImage_impl(const void* base) override;
  virtual void          GetMethMenus();
  virtual void          SetLabel();

  taiWidgetEditButton(void* base, taiEdit *taie, TypeDef* typ_, IWidgetHost* host_, taiWidget* par,
      QWidget* gui_parent_, int flags_ = 0);
};


#endif // taiWidgetEditButton_h

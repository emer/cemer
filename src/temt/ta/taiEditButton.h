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

#ifndef taiEditButton_h
#define taiEditButton_h 1

// parent includes:
#include <taiButtonMenu>

// member includes:
#include <taiDataList>

// declare all other types mentioned but not required to include:

class TA_API taiEditButton : public taiButtonMenu {
  // actually an edit menu... -- flgReadOnly creates menu which only allows for #EDIT_READ_ONLY members
  Q_OBJECT
  INHERITED(taiButtonMenu)
public:
  static taiEditButton* New(void* base, taiEdit *taie, TypeDef* typ_, IWidgetHost* host_, taiData* par,
      QWidget* gui_parent_, int flags_ = 0); // uses flags: flgReadOnly, flgEditOnly -- internally sets flgEditOnly if appropriate

  ~taiEditButton();

public slots:
  virtual void  Edit();         // edit callback
  void setRepLabel(const char* label);
protected:
  void*         cur_base;
  taiEdit*      ie;
  taiDataList   meth_el;        // method elements

  USING(inherited::GetImage_impl)
  override void         GetImage_impl(const void* base);
  virtual void          GetMethMenus();
  virtual void          SetLabel();

  taiEditButton(void* base, taiEdit *taie, TypeDef* typ_, IWidgetHost* host_, taiData* par,
      QWidget* gui_parent_, int flags_ = 0);
};


#endif // taiEditButton_h

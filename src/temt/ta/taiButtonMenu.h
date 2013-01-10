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

#ifndef taiButtonMenu_h
#define taiButtonMenu_h 1

// parent includes:
#include <taiActions>

// member includes:

// declare all other types mentioned but not required to include:
class QAbstractButton;

class TA_API taiButtonMenu: public taiActions {
  // a button, in which the actions appear as a popup menu; can also just be an Edit button, with no menu (pass flgEditOnly)
  Q_OBJECT
  INHERITED(taiActions)
public:
  inline QAbstractButton* rep() {return (QAbstractButton*)(QWidget*)m_rep;}

  override void         Delete(); // should delete the menu if we didn't use it
  taiButtonMenu(int  sel_type_, int font_spec_, TypeDef* typ_, IDataHost* host,
      taiData* par, QWidget* gui_parent_, int flags_ = 0, taiActions* par_menu_ = NULL);
private:
  void                  init();
};



#endif // taiButtonMenu_h

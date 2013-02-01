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

#ifndef taiMenuBar_h
#define taiMenuBar_h 1

// parent includes:
#include <taiActions>

// member includes:

// declare all other types mentioned but not required to include:
class QMenuBar; //


class TA_API taiMenuBar : public taiActions {
  // top level menu bar
  Q_OBJECT
  INHERITED(taiActions)
  friend class taiMenu_List; // hack because lists return refs to strings, not values
public:
  inline QMenuBar*      rep_bar() {return (QMenuBar*)(QWidget*)m_rep;}

  override void         AddSep(bool new_radio_grp = false) {} // no seps or groups allowed in a menubar

  taiMenuBar(int font_spec_, TypeDef* typ_, IWidgetHost* host,
      taiData* par, QWidget* gui_parent_, int flags_ = 0); // used by taiEditorWidgetsOfClass
  taiMenuBar(QWidget* gui_parent_, int ft, QMenuBar* exist_menu); // used by iDataViewer
  ~taiMenuBar();
protected:
  void                  init(QMenuBar* exist_menu); // #IGNORE
  void                  ConstrBar(QWidget* gui_parent_, QMenuBar* exist_bar = NULL); // #IGNORE
};


#endif // taiMenuBar_h

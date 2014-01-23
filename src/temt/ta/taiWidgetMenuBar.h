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

#ifndef taiWidgetMenuBar_h
#define taiWidgetMenuBar_h 1

// parent includes:
#include <taiWidgetActions>

// member includes:

// declare all other types mentioned but not required to include:
class QMenuBar; //


class TA_API taiWidgetMenuBar : public taiWidgetActions {
  // top level menu bar
  Q_OBJECT
  INHERITED(taiWidgetActions)
  friend class taiWidgetActions_List; // hack because lists return refs to strings, not values
public:
  inline QMenuBar*      rep_bar() {return (QMenuBar*)(QWidget*)m_rep;}

  void         AddSep(bool new_radio_grp = false) CPP11_OVERRIDE {} // no seps or groups allowed in a menubar

  taiWidgetMenuBar(int font_spec_, TypeDef* typ_, IWidgetHost* host,
      taiWidget* par, QWidget* gui_parent_, int flags_ = 0); // used by taiEditorOfClass
  taiWidgetMenuBar(QWidget* gui_parent_, int ft, QMenuBar* exist_menu); // used by itaViewer
  ~taiWidgetMenuBar();
protected:
  void                  init(QMenuBar* exist_menu); // #IGNORE
  void                  ConstrBar(QWidget* gui_parent_, QMenuBar* exist_bar = NULL); // #IGNORE
};


#endif // taiWidgetMenuBar_h

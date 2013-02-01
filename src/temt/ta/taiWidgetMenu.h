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

#ifndef taiWidgetMenu_h
#define taiWidgetMenu_h 1

// parent includes:
#include <taiWidgetActions>

// member includes:

// declare all other types mentioned but not required to include:
class iVec2i;

class TA_API taiWidgetMenu : public taiWidgetActions {
  // (possibly) hierarchical menu for selecting a single item
  Q_OBJECT
  INHERITED(taiWidgetActions)
  friend class taiWidgetActions_List; // hack because lists return refs to strings, not values
public:
  taiWidgetMenu(int  sel_type_, int font_spec_, TypeDef* typ_, IWidgetHost* host,
      taiWidget* par, QWidget* gui_parent_, int flags_ = 0, taiWidgetActions* par_menu_ = NULL);
  taiWidgetMenu(QWidget* gui_parent_, int sel_type_= normal, int font_spec_ = 0,
          QMenu* exist_menu = NULL);
  // constructor for Browser and context menus ft=0 means default font size;
  ~taiWidgetMenu();

  void         exec(const iVec2i& pos);
  iAction*     insertItem(const char* val, const QObject *receiver = NULL,
                          const char* member = NULL,
                          const QKeySequence* accel = NULL);
  // OBS compatability routine with QMenu
  void         insertSeparator() { AddSep(); }
  // Qt-convenience

private:
  void                  init(); // #IGNORE
};

#endif // taiWidgetMenu_h

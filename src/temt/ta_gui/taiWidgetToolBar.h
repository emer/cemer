// Co2018ght 2013-2017, Regents of the University of Colorado,
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

#ifndef taiWidgetToolBar_h
#define taiWidgetToolBar_h 1

// parent includes:
#include <taiWidgetActions>

// member includes:

// declare all other types mentioned but not required to include:
class QToolBar; //


class TA_API taiWidgetToolBar: public taiWidgetActions { // a toolbar, in which the actions appear as toolbuttons or button menus
  Q_OBJECT
  INHERITED(taiWidgetActions)
public:
  inline QToolBar*      rep() {return (QToolBar*)(QWidget*)m_rep;}

  taiWidgetToolBar(QWidget* gui_parent_, int ft, QToolBar* exist_bar); // used by itaViewer
protected:
  void                  init(QToolBar* exist_bar);
};

#endif // taiWidgetToolBar_h

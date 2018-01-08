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

#ifndef iApplicationToolBar_h
#define iApplicationToolBar_h 1

// parent includes:
#include <iToolBar>
#include <QToolButton>

// member includes:

// declare all other types mentioned but not required to include:

class TA_API iApplicationToolBar: public iToolBar {
  // widget for the Application toolbar -- created by ToolBar if name=Application
  INHERITED(iToolBar)
public:
  iApplicationToolBar(ToolBar* viewer, QWidget* parent = NULL)
  :iToolBar(viewer, parent){}

  QMenu*                history_back_menu;
  QMenu*                history_forward_menu;
  QMenu*                edit_undo_menu;
  QMenu*                edit_redo_menu;

protected:
  void                  Constr_post() override;
  
  QToolButton*          history_back_button;
  QToolButton*          history_forward_button;
  QToolButton*          edit_undo_button;
  QToolButton*          edit_redo_button;
  
protected slots:

};

#endif // iApplicationToolBar_h

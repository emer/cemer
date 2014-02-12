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

#include "ConsoleDockViewer.h"
#include <iDockViewer>

#include <QScrollArea>
#include <css_qtconsole.h>

TA_BASEFUNS_CTORS_DEFN(ConsoleDockViewer);

void ConsoleDockViewer::Initialize() {
  dock_flags = (DockViewerFlags)(DV_MOVABLE | DV_FLOATABLE);
}

IViewerWidget* ConsoleDockViewer::ConstrWidget_impl(QWidget* gui_parent) {
  iDockViewer* dv = new iDockViewer(this, gui_parent); // par usually NULL

  QScrollArea* sa = new QScrollArea;
  sa->setWidgetResizable(true);
  dv->setWidget(sa);
//TODO: enable  this for the generic Q&D console, and modalize for QcssConsole
  QcssConsole* con = QcssConsole::getInstance(NULL, cssMisc::TopShell);
  sa->setWidget((QWidget*)con);
  return dv;
}

void ConsoleDockViewer::MakeWinName_impl() {
  win_name = "css Console";
}


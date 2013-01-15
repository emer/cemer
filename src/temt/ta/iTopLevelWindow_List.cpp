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

#include "iTopLevelWindow_List.h"
#include <iMainWindowViewer>
#include <iDockViewer>

iMainWindowViewer* iTopLevelWindow_List::FindMainWindowById(int id) {
  for (int i = 0; i < size; ++i) {
    iMainWindowViewer* rval = SafeElAsMainWindow(i);
    if (rval && (rval->uniqueId() == id)) return rval;
  }
  return NULL;
}

iMainWindowViewer* iTopLevelWindow_List::SafeElAsMainWindow(int i) {
  return dynamic_cast<iMainWindowViewer*>(PosSafeEl(i)->widget());
}

iDockViewer* iTopLevelWindow_List::SafeElAsDockWindow(int i) {
  return dynamic_cast<iDockViewer*>(PosSafeEl(i)->widget());
}

void iTopLevelWindow_List::GotFocus_MainWindow(iMainWindowViewer* imw) {
  int idx = FindEl(imw);
  if (idx > 0) MoveIdx(idx, 0);
}

void iTopLevelWindow_List::GotFocus_DockWindow(iDockViewer* idv) {
  int idx = FindEl(idv);
  if (idx > 0) MoveIdx(idx, 0);
}

iMainWindowViewer* iTopLevelWindow_List::Peek_MainWindow() {
  for (int i = 0; i < size; ++i) {
    iMainWindowViewer* rval = SafeElAsMainWindow(i);
    if (rval) return rval;
  }
  return NULL;
}

iDockViewer* iTopLevelWindow_List::Peek_DockWindow() {
  for (int i = 0; i < size; ++i) {
    iDockViewer* rval = SafeElAsDockWindow(i);
    if (rval) return rval;
  }
  return NULL;
}


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

#include "iTabBarEx.h"
#include <iTabBarBase>
#include <iTabWidget>

#include <QContextMenuEvent>

iTabBarEx::iTabBarEx(iTabWidget* parent)
:inherited(parent)
{
  m_tab_widget = parent;
  setMovable(true);
}

void iTabBarEx::contextMenuEvent(QContextMenuEvent * e) {
  // find the tab being clicked, or -1 if none
  int idx = count() - 1;
  while (idx >= 0) {
    if (tabRect(idx).contains(e->pos())) break;
    --idx;
  }
  QPoint gpos = mapToGlobal(e->pos());
  if (m_tab_widget) m_tab_widget->emit_customContextMenuRequested2(gpos, idx);
}


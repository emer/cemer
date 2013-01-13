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

#include "iTabWidget.h"

iTabWidget::iTabWidget(QWidget* parent)
:inherited(parent)
{
  iTabBarEx* itbex = new iTabBarEx(this);
  setTabBar(itbex);
  //  itbex->setFocusPolicy(Qt::NoFocus); // do not focus on this guy -- nothing useful here
}

iTabBarBase* iTabWidget::GetTabBar() {
  return (iTabBarBase*)tabBar();
}

void iTabWidget::emit_customContextMenuRequested2(const QPoint& pos,
     int tab_idx)
{
  emit customContextMenuRequested2(pos, tab_idx);
}

void iTabWidget::contextMenuEvent(QContextMenuEvent* e) {
  // NOTE: this is not a good thing because the viewer has its own menu that
  // does everything it needs, and this conflicts
//   QPoint gpos = mapToGlobal(e->pos());
//   emit_customContextMenuRequested2(gpos, -1);
}

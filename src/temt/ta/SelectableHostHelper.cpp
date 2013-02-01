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

#include "SelectableHostHelper.h"
#include <QCoreApplication>


void SelectableHostHelper::customEvent(QEvent* ev) {
  switch ((int)ev->type()) {
  case iDataViewer_SelectionChanged_EventType:
    emit NotifySignal(host, ISelectableHost::OP_SELECTION_CHANGED);
    break;
  default:
    inherited::customEvent(ev);
    break;
  }
}

void SelectableHostHelper::EditAction(iAction* act) {
  int ea = act->usr_data.toInt();
  ISelectable::GuiContext gc = (ISelectable::GuiContext)act->data().toInt();
  host->EditAction(ea, gc);
}

void SelectableHostHelper::Emit_NotifySignal(ISelectableHost::NotifyOp op) {
  // selection ops need to go through the event loop or things get weird and nasty...
  if (op == ISelectableHost::OP_SELECTION_CHANGED) {
    QEvent* ev = new QEvent((QEvent::Type)iDataViewer_SelectionChanged_EventType);
    QCoreApplication::postEvent(this, ev); // returns immediately
  } else
    emit NotifySignal(host, op);
}



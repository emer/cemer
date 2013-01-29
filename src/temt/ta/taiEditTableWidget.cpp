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

#include "taiEditTableWidget.h"

#include <taMisc>
#include <taiMisc>

#include <QKeyEvent>


taiEditTableWidget::taiEditTableWidget(QWidget* parent)
:inherited(parent)
{
}

void taiEditTableWidget::keyPressEvent(QKeyEvent* e) {
  if((e->key() == Qt::Key_Tab) || (e->key() == Qt::Key_Backtab)) {
    e->ignore();                        // tell that we don't want this -- send to others
    return;
  }
  bool ctrl_pressed = taiMisc::KeyEventCtrlPressed(e);
  if(ctrl_pressed) {
    QPersistentModelIndex newCurrent;
    switch (e->key()) {
    case Qt::Key_N:
      newCurrent = moveCursor(MoveDown, e->modifiers());
      break;
    case Qt::Key_P:
      newCurrent = moveCursor(MoveUp, e->modifiers());
      break;
    case Qt::Key_U:
      newCurrent = moveCursor(MovePageUp, e->modifiers());
      break;
#ifdef TA_OS_MAC
      // this is a conflict with paste -- only works on mac where cmd and ctrl are diff!
    case Qt::Key_V:
      newCurrent = moveCursor(MovePageDown, e->modifiers());
      break;
#endif
    case Qt::Key_F:
      newCurrent = moveCursor(MoveRight, e->modifiers());
      break;
    case Qt::Key_B:
      newCurrent = moveCursor(MoveLeft, e->modifiers());
      break;
    }
    // from qabstractitemview.cpp
    QPersistentModelIndex oldCurrent = currentIndex();
    if (newCurrent != oldCurrent && newCurrent.isValid()) {
      QItemSelectionModel::SelectionFlags command = selectionCommand(newCurrent, e);
      if (command != QItemSelectionModel::NoUpdate
          || style()->styleHint(QStyle::SH_ItemView_MovementWithoutUpdatingSelection, 0, this)) {
        if (command & QItemSelectionModel::Current) {
          selectionModel()->setCurrentIndex(newCurrent, QItemSelectionModel::NoUpdate);
        } else {
          selectionModel()->setCurrentIndex(newCurrent, QItemSelectionModel::ClearAndSelect);
        }
        return;
      }
    }
  }
  inherited::keyPressEvent(e);
}


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

#include "iTableWidget.h"

#include <taMisc>
#include <taiMisc>

#include <QKeyEvent>
#include <iLineEdit>

iTableWidget::iTableWidget(QWidget* parent)
:inherited(parent)
{
  edit_start_pos = 0;
  edit_start_kill = false;
  iTableWidgetDefaultDelegate* del = new iTableWidgetDefaultDelegate(this);
  setItemDelegate(del);
}

void iTableWidget::keyPressEvent(QKeyEvent* e) {
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
    case Qt::Key_A:
      edit_start_pos = 0;
      edit_start_kill = false;
      edit(currentIndex());
      e->accept();
      break;
    case Qt::Key_E:
      edit_start_pos = -1;
      edit_start_kill = false;
      edit(currentIndex());
      e->accept();
      break;
    case Qt::Key_K:
      edit_start_pos = 0;
      edit_start_kill = true;
      edit(currentIndex());
      e->accept();
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

////////////////////////////////////////////////
//      iTableWidgetDefaultDelegate


iTableWidgetDefaultDelegate::iTableWidgetDefaultDelegate(iTableWidget* own_tw) :
  inherited(own_tw)
{
  own_table_widg = own_tw;
}

QWidget* iTableWidgetDefaultDelegate::createEditor(QWidget *parent,
                                                  const QStyleOptionViewItem &option,
                                                  const QModelIndex &index) const {
  QWidget* widg = inherited::createEditor(parent, option, index);
  QLineEdit* le = dynamic_cast<QLineEdit*>(widg);
  if(le) {
    iLineEdit* il = new iLineEdit(le->text().toLatin1(), parent);
    il->init_start_pos = own_table_widg->edit_start_pos;
    il->init_start_kill = own_table_widg->edit_start_kill;
    
    // if(own_table_widg) {
    //   QObject::connect(il, SIGNAL(lookupKeyPressed(iLineEdit*)),
    //                    own_table_widg, SLOT(lookupKeyPressed(iLineEdit*)) );
    // }
    return il;
  }
  return widg;
}

void iTableWidgetDefaultDelegate::setModelData(QWidget* editor, QAbstractItemModel* model,
                                              const QModelIndex& index) const {
  inherited::setModelData(editor, model, index);
  own_table_widg->edit_start_pos = 0;
  own_table_widg->edit_start_kill = false;
  // if(own_table_widg)
  //   own_table_widg->itemWasEdited(index);
}


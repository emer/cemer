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
#include <QApplication>

iTableWidget::iTableWidget(QWidget* parent)
:inherited(parent)
{
  edit_start_pos = 0;
  edit_start_kill = false;
  iTableWidgetDefaultDelegate* del = new iTableWidgetDefaultDelegate(this);
  setItemDelegate(del);
}

void iTableWidget::keyPressEvent(QKeyEvent* e) {
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


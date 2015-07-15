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

#ifndef iTableWidget_h
#define iTableWidget_h 1

// parent includes:
#include "ta_def.h"

#include <QTableWidget>
#include <QStyledItemDelegate>

// member includes:

// declare all other types mentioned but not required to include:


class TA_API iTableWidget: public QTableWidget {
  // ##NO_INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBER a table widget for an edit dialog
INHERITED(QTableWidget)
  Q_OBJECT
public:
  int           edit_start_pos;  // position to start when editing
  bool          edit_start_kill; // kill to end of line at edit start?

  iTableWidget(QWidget* parent = NULL);


protected:
  void         keyPressEvent(QKeyEvent* e) override;
};

class TA_API iTableWidgetDefaultDelegate: public QStyledItemDelegate {
  // this delegate is used to provide editing feedback info to the standard tree widget
INHERITED(QStyledItemDelegate)
Q_OBJECT
public:
  iTableWidget*   own_table_widg;
 
  iTableWidgetDefaultDelegate(iTableWidget* own_tw);

  QWidget*     createEditor(QWidget *parent,
                            const QStyleOptionViewItem &option,
                            const QModelIndex &index) const override;

  void         setModelData(QWidget* editor, QAbstractItemModel* model,
                            const QModelIndex& index) const override;
};

#endif // iTableWidget_h

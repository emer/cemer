// Copyright, 2017, Regents of the University of Colorado,
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

#include "iTreeListWidget.h"

#include <QTreeWidgetItem>
#include <QDropEvent>

#include <taMisc>

iTreeListWidget::iTreeListWidget(QWidget* parent)
:inherited(parent)
{
  parent_ = parent;
  init();
}

iTreeListWidget::~iTreeListWidget()
{
}

void iTreeListWidget::init() {
  setAutoScroll(true);
  setDragDropMode(DragDrop);
}

void iTreeListWidget::dropEvent(QDropEvent* e) {
  QModelIndex drop_index = indexAt(e->pos());
  DropIndicatorPosition drop_indicator = dropIndicatorPosition();
  
  if (!drop_index.parent().isValid() && drop_index.row() != -1)
  {
    QList<QTreeWidgetItem*> dragItems = selectedItems();
    QTreeWidgetItem* item = dragItems.first();
    
    int from_index;
    int to_index;
    
    switch (drop_indicator)
    {
      case QAbstractItemView::AboveItem:
      case QAbstractItemView::BelowItem:
      {
        from_index = this->indexOfTopLevelItem(item);
        to_index = drop_index.row();
        inherited::dropEvent(e);
        emit ListOrderChange(from_index, to_index);
      }
        break;
        
      case QAbstractItemView::OnItem:
      {
        // remove the dragged item and insert as top level item in new location
        // we don't allow children - this tree emulates a list!
        from_index = this->indexOfTopLevelItem(item);
        to_index = drop_index.row();
        this->takeTopLevelItem(from_index);
        this->insertTopLevelItem(to_index, item);
        emit ListOrderChange(from_index, to_index);
      }
        break;
        
      case QAbstractItemView::OnViewport:
        break;
    }
  }
}


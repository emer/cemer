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

#include "iDataTableColHeaderView.h"
#include <iDataTableView>
#include <DataTable>

#include <Qt>
#include <QHeaderView>
#include <QMouseEvent>

iDataTableColHeaderView::iDataTableColHeaderView(QWidget* parent)
:inherited(Qt::Horizontal, parent) {
  m_old_index = -1;
  m_dragging = false;
  connect(this, SIGNAL(sectionClicked(int)), this, SLOT(clickedSection(int)));
}

iDataTableColHeaderView::~iDataTableColHeaderView() {
}

void iDataTableColHeaderView::mouseEnterEvent(QMouseEvent* event ) {
  event->ignore();
}

void iDataTableColHeaderView::mousePressEvent(QMouseEvent* event ) {
  if (event->modifiers() == Qt::NoModifier) {
    m_dragging = true;
    m_old_index = logicalIndexAt(event->pos());
    setCursor(Qt::ClosedHandCursor);
    event->accept();
  }
  else {
    emit dynamic_cast<iDataTableView*>(parent())->hor_customContextMenuRequested(event->pos());
    event->ignore();
  }
  QHeaderView::mousePressEvent(event);
}

void iDataTableColHeaderView::mouseMoveEvent(QMouseEvent* event) {
  if (m_dragging == true) {
    setCursor(Qt::ClosedHandCursor);
    event->accept();
  }
}

void iDataTableColHeaderView::mouseReleaseEvent(QMouseEvent* event ) {
  m_dragging = false;
  int new_index = logicalIndexAt(event->pos());
  if (parent()) {
    DataTable* dt = dynamic_cast<iDataTableView*>(parent())->dataTable();
    if (dt && m_old_index != -1) {
      dt->MoveCol(m_old_index, new_index);
    }
  }
  m_old_index = -1;
  setCursor(Qt::ArrowCursor);
}

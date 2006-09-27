// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/CSS
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


// icliptoolwidget.cpp -- toolbar widget for copy/drag tools

#include "icliptoolwidget.h"

#include <QClipboard>
#include <QDrag>


iClipToolWidget::iClipToolWidget(QWidget* parent = NULL);

void iClipboardToolWidget::Init() {
  dragEnabled = false;
}

void iClipToolWidget::mousePressEvent(QMouseEvent* event)
{
  if (!dragEnabled()) return;

  if (event->button() == Qt::LeftButton) {
    dragStartPosition = event->pos();
    emit clicked();
  }
}

void iClipToolWidget::mouseMoveEvent(QMouseEvent* event) {
  if (!dragEnabled()) return;
  
  if (!(event->buttons() & Qt::LeftButton))
    return;
  if ((event->pos() - dragStartPosition).manhattanLength()
    < QApplication::startDragDistance())
    return;

  QDrag* drag = new QDrag(this);
  QMimeData* mimeData = mimeData();
  drag->setMimeData(mimeData);

  Qt::DropAction dropAction = drag->start(supportedDropActions());
}

void iClipToolWidget::setAutoCopy(bool value) {
  m_autoCopy = value;
}

void iClipToolWidget::setDragEnabled(bool value) {
  m_dragEnabled = value;
}

Qt::DropActions iClipToolWidget::supportedDropActions() const {
  return Qt::CopyAction;
}


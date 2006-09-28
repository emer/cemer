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

#include <QApplication>
#include <QClipboard>
#include <QDrag>


iClipToolWidget::iClipToolWidget(QWidget* parent) 
:inherited(parent)
{
  Init();
}

void iClipToolWidget::Init() {
  m_autoCopy = false;
  m_dragEnabled = false;
  setAutoRaise(true);
}

void iClipToolWidget::copyToClipboard() {
  QMimeData* md = mimeData();
  if (md)
    QApplication::clipboard()->setMimeData(md);
}

void iClipToolWidget::mousePressEvent(QMouseEvent* event) {
  if (event->button() == Qt::LeftButton) {
    if (dragEnabled()) {
      dragStartPosition = event->pos();
    }
    if (autoCopy()) {
      copyToClipboard();
    }
  }
  inherited::mousePressEvent(event);
}

void iClipToolWidget::mouseMoveEvent(QMouseEvent* event) {
  if (!dragEnabled()) return;
  
  if (!(event->buttons() & Qt::LeftButton))
    return;
  if ((event->pos() - dragStartPosition).manhattanLength()
    < QApplication::startDragDistance())
    return;

  QMimeData* md = mimeData();
  if (md) {
    QDrag* drag = new QDrag(this);
    drag->setMimeData(md);
    Qt::DropAction dropAction = drag->start(supportedDropActions());
  }
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


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

#include "iClipToolWidget.h"

#include <iClipWidgetAction>
#include <QApplication>
#include <QClipboard>
#include <QDrag>


iClipToolWidget::iClipToolWidget(iClipWidgetAction* cwa_, QWidget* parent) 
:inherited(parent)
{
  m_cwa = cwa_;
  setDefaultAction(cwa_);
  Init();
}

void iClipToolWidget::Init() {
  m_autoCopy = false;
  m_dragEnabled = false;
  setAutoRaise(true);
}

void iClipToolWidget::mousePressEvent(QMouseEvent* event) {
  if (event->button() == Qt::LeftButton) {
    if (dragEnabled()) {
      dragStartPosition = event->pos();
    }
  }
  inherited::mousePressEvent(event);
}

void iClipToolWidget::mouseReleaseEvent(QMouseEvent* event) {
  if (event->button() == Qt::LeftButton) {
    if (m_cwa) {
      if (autoCopy())
        m_cwa->copyToClipboard();
    }
  }
  inherited::mouseReleaseEvent(event);
}

void iClipToolWidget::mouseMoveEvent(QMouseEvent* event) {
  if (!dragEnabled() || !m_cwa) return;
  
  if (!(event->buttons() & Qt::LeftButton))
    return;
  if ((event->pos() - dragStartPosition).manhattanLength()
    < QApplication::startDragDistance())
    return;

  QMimeData* md = m_cwa->mimeData();
  if (md) {
    QDrag* drag = new QDrag(this);
    drag->setMimeData(md);
    //Qt::DropAction dropAction = 
    drag->exec(supportedDropActions());
  }
  setDown(false);		// un-down it
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


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

#include "iClipWidgetAction.h"

#include <iClipToolWidget>
#include <QApplication>
#include <QClipboard>
#include <QDrag>

iClipWidgetAction::iClipWidgetAction(QObject* parent)
:inherited(parent)
{
}

void iClipWidgetAction::copyToClipboard() {
  QMimeData* md = mimeData();
  if (md)
    QApplication::clipboard()->setMimeData(md);
}

QWidget* iClipWidgetAction::createWidget(QWidget* parent) {
  iClipToolWidget* rval = 
    new iClipToolWidget(this, parent);
  rval->setDragEnabled(true);
  if (!text().isEmpty())
    rval->setText(text());
  if (!icon().isNull())
    rval->setIcon(icon());
  if (!statusTip().isEmpty())
    rval->setStatusTip(statusTip());
  if (!toolTip().isEmpty())
    rval->setToolTip(toolTip());
  return rval;
}


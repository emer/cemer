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

#include "iDialogColorChooser.h"

#include <taiMisc> // for taiMisc::KeyEventCtrlPressed(e)

#include <QApplication>
#include <QPointer>
#include <QKeyEvent>

iDialogColorChooser::iDialogColorChooser(const QColor &old_clr, QWidget *parent)
  : QColorDialog(old_clr, parent)
{
}

void iDialogColorChooser::keyPressEvent(QKeyEvent *e)
{
  // Copied from iDialog class:
  // support Ctrl-Return/Enter as Accept
  bool ctrl_pressed = taiMisc::KeyEventCtrlPressed(e);
  bool is_enter = e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return;

  if (ctrl_pressed && is_enter) {
    e->accept();
    accept();
    return;
  }

  QColorDialog::keyPressEvent(e);
}

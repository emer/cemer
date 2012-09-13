// Copyright, 1995-2007, Regents of the University of Colorado,
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

#include "idialog.h"
#include "ta_qt.h" // for taiMisc::KeyEventCtrlPressed(e)

#include <QApplication>
#include <QPointer>

iDialog::iDialog(QWidget* parent, Qt::WindowFlags f)
  : QDialog(parent, f)
{
}

int iDialog::exec()
{
  // we're losing focus here so need to restore it!!
  // Note: previously this special handling was only done #ifdef TA_OS_MAC.
  QPointer<QWidget> m_prev_active = QApplication::activeWindow();
  int rval = QDialog::exec();
  if ((bool)m_prev_active) {
    //QApplication::setActiveWindow(m_prev_active);
    // note: above does NOT work! -- likely source of bug in cocoa 4.6.0
    m_prev_active->activateWindow();
  }
  return rval;
}

void iDialog::keyPressEvent(QKeyEvent *e)
{
  // support Ctrl-Return/Enter as Accept
  bool ctrl_pressed = taiMisc::KeyEventCtrlPressed(e);
  bool is_enter = e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return;

  if (ctrl_pressed && is_enter) {
    e->accept();
    accept();
    return;
  }

  // this is the default so should not need reiterating.
  // if(e->key() == Qt::Key_Escape) {
  //   e->accept();
  //   reject();
  //   return;
  // }

  QDialog::keyPressEvent(e);
}

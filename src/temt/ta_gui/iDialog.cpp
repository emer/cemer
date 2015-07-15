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

#include "iDialog.h"

#include <taiMisc> // for taiMisc::KeyEventCtrlPressed(e)

#include <QApplication>
#include <QPointer>
#include <QKeyEvent>

#if defined(TA_OS_MAC) && (QT_VERSION == 0x050200)
// defined in mac_objc_code.mm objective C file:
// per bug ticket: https://bugreports.qt-project.org/browse/QTBUG-38815
extern void TurnOffTouchEventsForWindow(QWindow* qtWindow);
#endif

iDialog::iDialog(QWidget* parent, Qt::WindowFlags f)
  : QDialog(parent, f)
{
}

#ifdef RESTORE_PREV_ACTIVE_WINDOW
int iDialog::exec()
{
  // we're losing focus here so need to restore it!!
  QPointer<QWidget> m_prev_active = QApplication::activeWindow();
  int rval = QDialog::exec();
  if ((bool)m_prev_active) {
    //QApplication::setActiveWindow(m_prev_active);
    // note: above does NOT work! -- likely source of bug in cocoa 4.6.0
    m_prev_active->activateWindow();
  }
  return rval;
}
#endif

void iDialog::keyPressEvent(QKeyEvent* key_event)
{
#if defined(TA_OS_MAC) && (QT_VERSION == 0x050200)
  // needs to be after window is fully up and running..
  TurnOffTouchEventsForWindow(windowHandle());
#endif

  if(taiMisc::KeyEventCtrlPressed(key_event)) {  // knows about mac vs other OS
    if ((key_event->key() == Qt::Key_Return) || (key_event->key() == Qt::Key_Enter)) {
      key_event->accept();
      accept();
      return;
    }
  }
  QDialog::keyPressEvent(key_event);
}

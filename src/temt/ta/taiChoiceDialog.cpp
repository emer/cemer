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

#include "taiChoiceDialog.h"

const String taiChoiceDialog::delimiter("!|");

int taiChoiceDialog::ChoiceDialog(QWidget* parent_, const String& msg,
  const String& but_list, const char* title)
{
  taiChoiceDialog* dlg = new taiChoiceDialog(Question, QString(title),
    msg.toQString(), but_list,  parent_, false);
  // show the dialog
  QApplication::setOverrideCursor(QCursor(Qt::ArrowCursor)); // in case busy, recording, etc
  dlg->setIcon(QMessageBox::Question);
  dlg->setWindowModality(Qt::ApplicationModal);
  dlg->resize( taiM->dialogSize(taiMisc::dlgMedium | taiMisc::dlgVer) );
  int rval = dlg->exec();
  QApplication::restoreOverrideCursor();
  delete dlg;
  return rval;
}


bool taiChoiceDialog::ErrorDialog(QWidget* parent_, const char* msg,
  const char* title, bool copy_but_, bool cancel_errs_but_)
{
  QApplication::setOverrideCursor(QCursor(Qt::ArrowCursor)); // in case busy, recording, etc
  String buts;
  if(cancel_errs_but_)
    buts = "Cancel Remaining Error Dialogs" + delimiter + "OK";
  taiChoiceDialog* dlg = new taiChoiceDialog(Warning, QString(title),
    QString(msg), buts, parent_, copy_but_);
  dlg->setIcon(QMessageBox::Critical);
  dlg->setWindowModality(Qt::ApplicationModal);
  int chs = dlg->exec();
  QApplication::restoreOverrideCursor();
  delete dlg;
  return chs == 0;              // true if cancel
}

void taiChoiceDialog::ConfirmDialog(QWidget* parent_, const char* msg,
  const char* title, bool copy_but_)
{
  QApplication::setOverrideCursor(QCursor(Qt::ArrowCursor)); // in case busy, recording, etc
  taiChoiceDialog* dlg = new taiChoiceDialog(Information, QString(title),
    QString(msg), "", parent_, copy_but_);
  dlg->setIcon(QMessageBox::Question);
  dlg->setWindowModality(Qt::ApplicationModal);
  dlg->resize( taiM->dialogSize(taiMisc::dlgMedium | taiMisc::dlgVer) );
  dlg->exec();
  QApplication::restoreOverrideCursor();
  delete dlg;
}

taiChoiceDialog::taiChoiceDialog(Icon icon, const QString& title,
  const QString& text, String but_list, QWidget* parent,
  bool copy_but_)
:inherited(icon, title, text, NoButton, parent)
{
  if (title.isEmpty()) {
    setWindowTitle(QCoreApplication::instance()->applicationName());
  }
  bgChoiceButtons = new QButtonGroup(this); // note: not a widget, invisible
  bgChoiceButtons->setExclusive(false); // not really applicable

  // create buttons
  String_PArray sa;
  if (but_list.contains(delimiter)) { // has options encoded within prompt string
    // strip leading/trailing delims, and parse all at once, to get count now
    if (but_list.startsWith(delimiter))
      but_list = but_list.after(delimiter);
    if (but_list.endsWith(delimiter))
      but_list = but_list.left(but_list.length() - delimiter.length());
    sa.SetFromString(but_list, delimiter);
  }
  // we always have at least an Ok button
  if (sa.size == 0) {
    sa.Add("&Ok");
  }
  num_chs = sa.size;

  for (int i = 0; i < sa.size; ++i) {
    ButtonRole role = AcceptRole;
    if ((i > 0) && (i == (sa.size - 1)))
      role = RejectRole;
    Constr_OneBut(sa[i], i, role);
  }

  if (copy_but_) {
    // damn copy button
    QAbstractButton* but = Constr_OneBut("Copy to clipboard", -2, ActionRole);
    connect(but, SIGNAL(clicked()), this, SLOT(copyToClipboard()));
  }
}

void taiChoiceDialog::done(int r) {
  QAbstractButton* but = clickedButton();
  int id = -1;
  if (but)
    id = bgChoiceButtons->id(but);
  if (id < -1) return; // ignore
  // if user goes Esc, it returns -1 -- we want our last button instead
  if ((id == -1) || (r < 0))
    r = num_chs - 1;
  inherited::done(r);
}

int taiChoiceDialog::exec() {
  // we're losing focus here so need to restore it!!
// #ifdef TA_OS_MAC
  QPointer<QWidget> m_prev_active = QApplication::activeWindow();
// #endif
  int rval = inherited::exec();
  //#ifdef TA_OS_MAC
  if((bool)m_prev_active) {
//     QApplication::setActiveWindow(m_prev_active);
    // note: above does NOT work! -- likely source of bug in cocoa 4.6.0
    m_prev_active->activateWindow();
  }
// #endif
  return rval;
}

void taiChoiceDialog::copyToClipboard() {
  QApplication::clipboard()->setText(text());
}

QAbstractButton* taiChoiceDialog::Constr_OneBut(String lbl, int id, ButtonRole role) {
  if (lbl.startsWith(" ")) // allow for one space..
    lbl = lbl.after(' ');
  if (lbl.empty())
    return NULL; // not really supposed to happen...

  QPushButton* but = new QPushButton(lbl, this);
  addButton(but, role);
  bgChoiceButtons->addButton(but, id);
  return but;
}

void taiChoiceDialog::keyPressEvent(QKeyEvent* ev) {
  // we allow 0-n numeric keys to be accelerators for the buttons
  int key_code = ev->key();
  if ((key_code >= Qt::Key_0) && (key_code <= Qt::Key_9)) {
    int but_index = key_code - Qt::Key_0;
    QPushButton* but = (QPushButton*)bgChoiceButtons->button(but_index);
    if (but != NULL) {
      // simulate effect of pressing the button
      but->click();
      ev->accept(); // swallow key
    }
  } else {
    QDialog::keyPressEvent(ev);
  }
}

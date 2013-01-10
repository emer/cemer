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

#ifndef taiChoiceDialog_h
#define taiChoiceDialog_h 1

// parent includes:
#include "ta_def.h"
#ifndef __MAKETA__
#include <QMessageBox>
#endif

// member includes:

// declare all other types mentioned but not required to include:

// Button 0 is the default if user presses Enter
// Button 1 is the Cancel, if user presses Esc (only if "no_cancel" option false)
// User can also press the number associated with the button, starting from 0

class TA_API taiChoiceDialog : public QMessageBox {
  // ##IGNORE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS
  Q_OBJECT
  INHERITED(QMessageBox)

public:
  static const String delimiter;

  static int ChoiceDialog(
    QWidget* parent_,
    const String& msg,
    const String& but_list,
    const char* win_title = "");

  static bool ErrorDialog(
    QWidget* parent_,
    const char* msg,
    const char* win_title = "Error",
    bool copy_but = true,
    bool cancel_errs_but = true);

  static void ConfirmDialog(
    QWidget* parent_,
    const char* msg,
    const char* win_title = "",
    bool copy_but = true);

  QButtonGroup* bgChoiceButtons; //note: not a widget

  override void done(int r);

  // Note: QMessageBox::exec() is not virtual, so this is not an override.
  // Make it virtual from this point forward, in case subclasses need
  // to override for some reason.
  virtual int exec();

  ~taiChoiceDialog() { }

protected:
  int num_chs; // number of choices, we return last idx for Esc
  virtual void keyPressEvent(QKeyEvent* ev); // override

protected slots:
  void copyToClipboard(); // copy text of msg to clipboard

#ifndef __MAKETA__
private:
  taiChoiceDialog(
    Icon icon,
    const QString& title,
    const QString& text,
    String but_list,
    QWidget* parent,
    bool copy_but);

  virtual QAbstractButton* Constr_OneBut(String lbl, int id, ButtonRole role);
#endif
};

#endif // taiChoiceDialog_h

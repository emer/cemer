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

#ifndef iDialogChoice_h
#define iDialogChoice_h 1

// parent includes:
#include "ta_def.h"
#include <QMessageBox>
#include <iDialog>

// member includes:
#include <taString>

// declare all other types mentioned but not required to include:
class QButtonGroup; //
class QDialogButtonBox; //
class QHBoxLayout; //

// Button 0 is the default if user presses Enter
// Button 1 is the Cancel, if user presses Esc (only if "no_cancel" option false)
// User can also press the number associated with the button, starting from 0

class TA_API iDialogChoice : public QMessageBox {
  Q_OBJECT
  INHERITED(QMessageBox)
public:
  static const String delimiter;

  static int ChoiceDialog(
    QWidget* parent_,
    const String& msg,
    const String& but_list,
    const char* win_title = "");

  static int ChoiceDialogSepBut(
    QWidget* parent_,
    const String& msg,
    const String& but_list_sep, // separate buttons - come first in ordering
    const String& but_list,     // regular ok / cancel buttons
    const char* win_title = "");
  // makes separate buttons -- prevents diabolical reordering of buttons!

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

  void done(int r) override;

  // Note: QMessageBox::exec() is not virtual, so this is not an override.
  // Make it virtual from this point forward, in case subclasses need
  // to override for some reason.
  virtual int exec();

  ~iDialogChoice() { }

protected:
  int num_chs; // number of choices, we return last idx for Esc
  void keyPressEvent(QKeyEvent* ev) override;

  virtual QAbstractButton* Constr_OneBut(String lbl, int id, ButtonRole role);

protected slots:
  void copyToClipboard(); // copy text of msg to clipboard

private:
  iDialogChoice(
    Icon icon,
    const QString& title,
    const QString& text,
    String but_list,
    QWidget* parent,
    bool copy_but);
};

class TA_API iDialogChoiceSepBut : public iDialog {
  Q_OBJECT
INHERITED(iDialog)
  // separate buttons
public:
  static const String delimiter;
  
  iDialogChoiceSepBut(QMessageBox::Icon icon,
                      const QString& title,
                      const QString& text,
                      String but_list_sep,
                      String but_list,
                      QWidget* parent);

  ~iDialogChoiceSepBut();

  QLabel *label;
  QMessageBox::Icon icon;
  QLabel *iconLabel;
  QDialogButtonBox *buttonBox;
  QWidget* sep_buts;
  QHBoxLayout* sep_but_lay;
  QList<QAbstractButton *> customButtonList;
  QButtonGroup* bgChoiceButtons; //note: not a widget

protected slots:
  void   dialogButtonClicked(int id);

protected:
  int num_chs; // number of choices, we return last idx for Esc

  virtual QAbstractButton* Constr_SepBut(String lbl, int id);
  virtual QAbstractButton* Constr_DlgBut(String lbl, int id, QMessageBox::ButtonRole role);
  
private:
};




#endif // iDialogChoice_h

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

#ifndef iDialogLineEdit_h
#define iDialogLineEdit_h 1

#include "ta_def.h"

#include <iDialog>

// externals
class iLineEdit;
class iLabel;
class QPushButton;

class TA_API iDialogLineEdit : public iDialog {
  Q_OBJECT
INHERITED(iDialog)
public:
  iLabel*	prompt;
  iLineEdit*	lnEdit;
  QPushButton*	btnOk; // read/write only
  QPushButton* 	btnCancel; // or close, if read only

  static bool LineEditDialog
    (QString& str_val, const QString& prompt, 
     const QString& ok_txt = "Ok", const QString cancel_txt = "Cancel");
  // show the dialog with given values, return false if canceled
  
  iDialogLineEdit(QWidget* parent = 0);
  ~iDialogLineEdit();
protected:
  
private:
  void 		init();
};

#endif // iDialogLineEdit_h

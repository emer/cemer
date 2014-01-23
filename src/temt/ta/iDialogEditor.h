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

#ifndef iDialogEditor_h
#define iDialogEditor_h 1

// parent includes:
#include "ta_def.h"
#include <iDialog>

// member includes:

// declare all other types mentioned but not required to include:
class taiEditor; //
class QVBoxLayout; // 
class QScrollArea; //


class TA_API iDialogEditor : public iDialog {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS dialog with taiEditor owner
  INHERITED(iDialog)
  Q_OBJECT
friend class taiEditor;
public:
  iDialogEditor(taiEditor* owner_, QWidget* parent = 0, int wflags = 0);
  ~iDialogEditor();

  bool          post(bool modal); // simplified version of post_xxx routines, returns true if accepted or false (if modal) if cancelled
  void          dismiss(bool accept_);
  void          accept();

  void          iconify();   // Iv compatibility routine
  void          deiconify(); // Iv compatibility routine
  void          setCentralWidget(QWidget* widg); // is put in a scroll area; dialog is limited to screen size
  void          setButtonsWidget(QWidget* widg); // is put at the bottom, not in a scroll
protected:
  taiEditor*      owner;
  QVBoxLayout*  layOuter;
  QWidget*      mcentralWidget;
  QScrollArea*  scr;
  void closeEvent(QCloseEvent* ev) CPP11_OVERRIDE;
};

#endif // iDialogEditor_h

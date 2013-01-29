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

#ifndef iHostDialog_h
#define iHostDialog_h 1

// parent includes:
#include "ta_def.h"
#include <iDialog>

// member includes:

// declare all other types mentioned but not required to include:
class taiDataHostBase; //
class QVBoxLayout; // 
class QScrollArea; //


class TA_API iHostDialog : public iDialog {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS dialog with taiDataHostBase owner
  INHERITED(iDialog)
  Q_OBJECT
friend class taiDataHostBase;
public:
  iHostDialog(taiDataHostBase* owner_, QWidget* parent = 0, int wflags = 0);
  ~iHostDialog();

  bool          post(bool modal); // simplified version of post_xxx routines, returns true if accepted or false (if modal) if cancelled
  void          dismiss(bool accept_);

  void          iconify();   // Iv compatibility routine
  void          deiconify(); // Iv compatibility routine
  void          setCentralWidget(QWidget* widg); // is put in a scroll area; dialog is limited to screen size
  void          setButtonsWidget(QWidget* widg); // is put at the bottom, not in a scroll
protected:
  taiDataHostBase*      owner;
  QVBoxLayout*  layOuter;
  QWidget*      mcentralWidget;
  QScrollArea*  scr;
  override void closeEvent(QCloseEvent* ev);
};

#endif // iHostDialog_h

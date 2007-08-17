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

#ifndef TEST_SIMPLEWIDGETS_H
#define TEST_SIMPLEWIDGETS_H

#include "ilineedit.h"
#include "icheckbox.h"
#include "ispinbox.h"

#include <qlayout.h>
#include <qpushbutton.h>
#include <qdialog.h>

class Test_SimpleWidgets: public QDialog {
public:
  QVBoxLayout*	layOuter;
  iLineEdit*	txt1;
  iSpinBox*	spn1;
  iCheckBox*	chk1;
  QCheckBox*	chkHilight; // for hilight control
  QCheckBox*	chkRO; // for readonly control

  Test_SimpleWidgets(QWidget* parent = 0, const char* name = 0);
};

#endif

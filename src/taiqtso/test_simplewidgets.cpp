// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/CSS
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

#include "test_simplewidgets.h"


Test_SimpleWidgets::Test_SimpleWidgets(QWidget* parent, const char* name)
: QDialog(parent, name)
{
  layOuter = new QVBoxLayout(this);
   //layOuter->addLayout(lay);

  txt1 = new iLineEdit(this, "txt1");
  layOuter->addWidget(txt1);
  layOuter->addSpacing(4);

  // iSpinBox
  spn1 = new iSpinBox(this, "spn1");
  layOuter->addWidget(spn1);
  layOuter->addSpacing(4);
  chk1 = new iCheckBox("test iCheck", this, "chk1");
  layOuter->addSpacing(4);
  layOuter->addWidget(chk1);

  layOuter->addSpacing(6);
  QHBoxLayout* hbl = new QHBoxLayout();
  chkHilight = new QCheckBox("Hilight", this, "chkHilight");
  hbl->addWidget(chkHilight);
  hbl->addSpacing(4);
  chkRO = new QCheckBox("Read Only", this, "chkRO");
  hbl->addWidget(chkRO);
  hbl->addStretch();
  layOuter->addLayout(hbl);
  layOuter->addStretch();

  connect(chkRO, SIGNAL(toggled(bool)),
      txt1, SLOT(setReadOnly(bool)) );
  connect(chkRO, SIGNAL(toggled(bool)),
      spn1, SLOT(setReadOnly(bool)) );
  connect(chkRO, SIGNAL(toggled(bool)),
      chk1, SLOT(setReadOnly(bool)) );

  connect(chkHilight, SIGNAL(toggled(bool)),
      txt1, SLOT(setHilight(bool)) );
  connect(chkHilight, SIGNAL(toggled(bool)),
      spn1, SLOT(setHilight(bool)) );
  connect(chkHilight, SIGNAL(toggled(bool)),
      chk1, SLOT(setHilight(bool)) );

}


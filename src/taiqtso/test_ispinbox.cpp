// Copyright (C) 1995-2005 Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.

#include "test_ispinbox.h"


Test_iSpinBox::Test_iSpinBox(QWidget* parent, const char* name)
: QDialog(parent, name)
{
  layOuter = new QVBoxLayout(this);
  QHBoxLayout* lay = new QHBoxLayout(layOuter);
  //layOuter->addLayout(lay);

  // iSpinBox
  spn1 = new iSpinBox(this, "spn1");
  lay->addWidget(spn1);
  lay->addSpacing(2);
  btnSpinRO = new QPushButton("Read Only", this, "btnSpinRO");
  btnSpinRO->setToggleButton(true);
  connect(btnSpinRO, SIGNAL(toggled(bool)),
      spn1, SLOT(setReadOnly(bool)) );

  lay->addWidget(btnSpinRO);
  lay->addStretch();
}


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

#include "test_iflowlayout.h"

#include <qstring.h>


Test_iFlowLayout::Test_iFlowLayout(QWidget* parent, const char* name)
: QDialog(parent, name)
{
  setMaximumWidth(600);
  setMinimumWidth(400); //start with less than max
  layOuter = new QVBoxLayout(this);
  bgrHAlign = new QHButtonGroup(this, "bgrHAlign");
  bgrHAlign->setRadioButtonExclusive(true);
  bgrHAlign->setTitle("Horizontal Alignment");
  new QRadioButton("Left", bgrHAlign);
  new QRadioButton("Center", bgrHAlign);
  new QRadioButton("Right", bgrHAlign);
  new QRadioButton("Justify", bgrHAlign);
  bgrHAlign->setButton(0);
  connect (bgrHAlign, SIGNAL(clicked(int)),
      this, SLOT(bgrHAlign_clicked(int)) );

  layOuter->addWidget(bgrHAlign);

  layFlow = new iFlowLayout(3, Qt::AlignLeft, "layFlow");
  layOuter->addLayout(layFlow, 1);
  for (int i = 0; i < 20; ++i) {
    QPushButton* new_but = new QPushButton(QString("button_") + QString::number(i), this);
    new_but->setMinimumSize(new_but->width() + (i * 3), new_but->height()); // make them different sizes
    new_but->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    layFlow->addWidget(new_but);
    buttons[i] = new_but;
  }
}

Test_iFlowLayout::~Test_iFlowLayout() {
  //nothing
}

void Test_iFlowLayout::bgrHAlign_clicked(int bnum) {
  switch (bnum) {
  case 0:  layFlow->setAlignment(Qt::AlignLeft); break;
  case 1:  layFlow->setAlignment(Qt::AlignHCenter); break;
  case 2:  layFlow->setAlignment(Qt::AlignRight); break;
  case 3:  layFlow->setAlignment(Qt::AlignJustify); break;
  default: break;
  }
}

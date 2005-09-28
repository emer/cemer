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

#ifndef TEST_IFLOWLAYOUT_H
#define TEST_IFLOWLAYOUT_H

#include "iflowlayout.h"

#include <qhbuttongroup.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qdialog.h>

class Test_iFlowLayout: public QDialog {
  Q_OBJECT
public:
  QVBoxLayout*	layOuter;
  QHButtonGroup*	bgrHAlign;
  iFlowLayout*	layFlow;
  QPushButton*	buttons[20];

  Test_iFlowLayout(QWidget* parent = 0, const char* name = 0);
  ~Test_iFlowLayout();
public slots:
  void bgrHAlign_clicked(int bnum);

};

#endif

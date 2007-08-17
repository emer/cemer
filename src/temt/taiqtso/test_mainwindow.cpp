// Copyright, 1995-2005, Regents of the University of Colorado,
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

#include "test_mainwindow.h"

#include "test_simplewidgets.h"
#include "test_iflowlayout.h"
#include "test_iflowbox.h"
#include "test_itreeview.h"

#include <qapplication.h>

MainWindow::MainWindow(QWidget* parent, const char* name)
: QMainWindow(parent, name)
{
  setMinimumSize(300, 150);
  frmMain = new QFrame(this, "frmMain");
  this->setCentralWidget(frmMain);
  layOuter = new QGridLayout(frmMain, 2, 2, 5, -1, "layOuter");
  layOuter->setColStretch(0, 1); //col0 takes more space
  layOuter->setRowStretch(1, 1); //row1 takes more space
  lblClasses = new QLabel("Test Classes", frmMain, "lblClasses");
  layOuter->addWidget(lblClasses, 0, 0);
  lbxClasses = new QListBox(frmMain, "lbxClasses");
  layOuter->addWidget(lbxClasses, 1, 0);
  QVBoxLayout* tl = new QVBoxLayout();
  layOuter->addLayout(tl, 1, 1);
  btnCreate = new QPushButton("Create tester", frmMain, "btnCreate");
  tl->addWidget(btnCreate);
  tl->addStretch();

  connect(btnCreate, SIGNAL(clicked()), this, SLOT(btnCreate_clicked()) );
  connect(lbxClasses, SIGNAL(doubleClicked(QListBoxItem*)), this, SLOT(btnCreate_clicked()) );
  fillList();
  lbxClasses->setCurrentItem(0);
}

void MainWindow::createTestDialog(int index) {
  switch (index) {
  case 0: (new Test_SimpleWidgets(qApp->mainWidget()))->show(); break;
  case 1: (new Test_iFlowLayout(qApp->mainWidget()))->show(); break;
  case 2: (new Test_iFlowBox(qApp->mainWidget()))->show(); break;
  case 3: (new Test_iTreeView(qApp->mainWidget()))->show(); break;
  default: break;
  }
}

void MainWindow::fillList() {
  lbxClasses->insertItem("Simple Widgets");
  lbxClasses->insertItem("iFlowLayout");
  lbxClasses->insertItem("iFlowBox");
  lbxClasses->insertItem("iTreeView");
}

void MainWindow::btnCreate_clicked() {
  int index = lbxClasses->currentItem();
  createTestDialog(index);
}

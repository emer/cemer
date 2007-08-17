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

#ifndef TEST_MAINWINDOW_H
#define TEST_MAINWINDOW_H

#include <qframe.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlistbox.h>
#include <qpushbutton.h>
#include <qmainwindow.h>

class MainWindow: public QMainWindow {
  Q_OBJECT
public:
  QFrame*	frmMain;
  QGridLayout*	layOuter;
  QLabel*	lblClasses;
  QListBox*	lbxClasses;
  QPushButton*	btnCreate;

  MainWindow(QWidget* parent = 0, const char* name = 0);
  void		createTestDialog(int index); // order of items in list box

private:
  void		fillList();

private slots:
  void btnCreate_clicked();
};

#endif

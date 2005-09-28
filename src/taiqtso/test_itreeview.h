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

#ifndef TEST_ITREEVIEW_H
#define TEST_ITREEVIEW_H

#include "itreeview.h"

#include <qhbuttongroup.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qdialog.h>

class Test_iTreeView: public QDialog {
  Q_OBJECT
public:
  QVBoxLayout*	layOuter;
  QLineEdit*	txtItem;
  iTreeView*	tvw;

  Test_iTreeView(QWidget* parent = 0, const char* name = 0);
  ~Test_iTreeView();

  void AddItems(iTreeViewItem* par, int n);
public slots:
  void tvw_selectionChanged(iTreeViewItem* itm);
};

#endif

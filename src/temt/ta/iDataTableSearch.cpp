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

#include "iDataTableSearch.h"
#include <iLineEdit>
#include <taString>
#include <iDataTableView>
#include <iActionMenuButton>
#include <iMenuButton>

#include <taMisc>
#include <taiMisc>

#include <QHBoxLayout>
#include <QLabel>
#include <QToolBar>
#include <QAction>
#include <QToolButton>


iDataTableSearch::iDataTableSearch(QWidget* parent) : QWidget(parent) {
  table_view = NULL;
  Constr();
}

iDataTableSearch::iDataTableSearch(iDataTableView* table_view_, QWidget* parent)  : QWidget(parent) {
  table_view = table_view_;
  Constr();
}

iDataTableSearch::~iDataTableSearch() {
  
}

void iDataTableSearch::Constr() {

}

void iDataTableSearch::Search() {
  
}

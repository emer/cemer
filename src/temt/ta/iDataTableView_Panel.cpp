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

#include "iDataTableView_Panel.h"
#include <DataTable>
#include <iViewPanelFrame>
#include <DataTableView>
#include <iFlowLayout>
#include <iMethodButtonMgr>

#include <QVBoxLayout>

iDataTableView_Panel::iDataTableView_Panel(DataTableView* lv)
:inherited(lv)
{
  widg = new QWidget();
//  widg->setFrameStyle( QFrame::GroupBoxPanel | QFrame::Sunken );
  layWidg = new QVBoxLayout(widg); //def margin/spacing=2
  layWidg->setMargin(0); layWidg->setSpacing(2);

  ////////////////////////////////////////////////////////////////////////////
  // Command Buttons
  widCmdButtons = new QWidget(widg);
  iFlowLayout* fl = new iFlowLayout(widCmdButtons);
//  layTopCtrls->addWidget(widCmdButtons);
  layOuter->addWidget(widCmdButtons);

  meth_but_mgr = new iMethodButtonMgr(widCmdButtons, fl, widCmdButtons);
  meth_but_mgr->Constr(lv->dataTable());

  MakeButtons(layOuter);

  setCentralWidget(widg);
}

iDataTableView_Panel::~iDataTableView_Panel() {
}

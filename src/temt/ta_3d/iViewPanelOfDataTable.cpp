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

#include "iViewPanelOfDataTable.h"
#include <DataTable>
#include <iViewPanel>
#include <DataTableView>
#include <iFlowLayout>
#include <iMethodButtonMgr>

#include <QVBoxLayout>

iViewPanelOfDataTable::iViewPanelOfDataTable(DataTableView* lv)
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

iViewPanelOfDataTable::~iViewPanelOfDataTable() {
}

String iViewPanelOfDataTable::TabText() const {
  if (m_dv) {
    String rval = m_dv->GetLabel();
    if(((DataTableView*)m_dv)->dataTable()) {
      String dtname = ((DataTableView*)m_dv)->dataTable()->name;
      if(rval.startsWith("Graph"))
        rval = dtname + " Graph";
      else if(rval.startsWith("Grid"))
        rval = dtname + " Grid";
    }
    return rval;
  }
  else {
    return inherited::TabText();
  }
}


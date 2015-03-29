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

#include "iStdDataTableView.h"
#include <taiWidgetMenu>
#include <CellRange>
#include <DataTable>
#include <taiSigLink>

iStdDataTableView::iStdDataTableView(QWidget* parent)
:inherited(parent)
{
}

void iStdDataTableView::FillContextMenu_impl(ContextArea ca, taiWidgetMenu* menu, const CellRange& sel)
{
  inherited::FillContextMenu_impl(ca, menu, sel);
  DataTable* tab = this->dataTable(); // may not exist
  if (!tab) return;
  // only do col items if one selected only
  if ((ca == CA_COL_HDR) && (sel.width() == 1)) {
    DataCol* col = tab->GetColData(sel.col_fr, true);
    if (col) {
      taiSigLink* link = (taiSigLink*)col->GetSigLink();
      if (link) link->FillContextMenu(menu);
    }
  }
  if (ca == CA_ROW_HDR) {
    iAction* act = NULL;
    menu->AddSep();
    act = menu->AddItem("Compare Selected Rows", taiWidgetMenu::normal,
                        iAction::int_act,
                        this, SLOT(RowColOp(int)), (OP_ROW | OP_COMPARE));
    act->setEnabled(sel.height() > 1);  // enable if compare state is on
    
    act = menu->AddItem("Clear Compare Rows", taiWidgetMenu::normal,
                        iAction::int_act,
                        this, SLOT(RowColOp(int)), (OP_ROW | OP_CLEAR_COMPARE));
    act->setEnabled(tab->CompareRowsState());  // enable if compare state is on
  }
}

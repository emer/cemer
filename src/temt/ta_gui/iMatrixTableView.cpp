// Copyright 2013-2018, Regents of the University of Colorado,
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

#include "iMatrixTableView.h"
#include <taMatrix>
#include <iMatrixTableModel>
#include <taiTabularDataMimeFactory>
#include <CellRange>
#include <taiMimeSource>
#include <iClipData>
#include <iTableView>
#include <iDataTableColHeaderView>
#include <DataTable>

#include <taMisc>
#include <taiMisc>

#include <QHeaderView>
#include <QInputDialog>

iMatrixTableView::iMatrixTableView(QWidget* parent)
:inherited(parent)
{
  col_header = new iMatrixTableColHeaderView(this); // subclass header
  this->setHorizontalHeader(col_header);

  connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this,
          SLOT(this_customContextMenuRequested(const QPoint&)) );
  
  UpdateRowHeight();
}

taMatrix* iMatrixTableView::mat() const {
  iMatrixTableModel* mod = qobject_cast<iMatrixTableModel*>(model());
  if (mod) return mod->mat();
  else return NULL; 
}

void iMatrixTableView::EditAction(int ea) {
  taMatrix* mat = this->mat(); // may not have a model/mat!
  if (!mat) return;
  taiTabularDataMimeFactory* fact = taiTabularDataMimeFactory::instance();
  CellRange sel;
  GetSel(sel);
  if (ea & iClipData::EA_SRC_OPS) {
    fact->Mat_EditActionS(mat, sel, ea);
  } else {// dest op
    taiMimeSource* ms = taiMimeSource::NewFromClipboard();
    fact->Mat_EditActionD(mat, sel, ms, ea);
    delete ms;
  }
}

void iMatrixTableView::ViewAction(int ea) {
  taMatrix* mat = this->mat(); // may not have a model/mat!
  if (!mat) return;
  CellRange sel;
  GetSel(sel);
  String str;
  for (int row = sel.row_fr; row <= sel.row_to; ++row) {
    for (int col = sel.col_fr; col <= sel.col_to; ++col) {
      int idx = mat->FastElIndex2d(col, row);
      str += mat->SafeElAsVar_Flat(idx).toString();
    }
  }
  // todo: could do more
  taMisc::Confirm("contents of cell(s):\n", str);
}

void iMatrixTableView::ResetColorsAction(int ea) {
  taMatrix* mat = this->mat(); // may not have a model/mat!
  if (!mat) return;
  mat->ResetColorScale();
}

void iMatrixTableView::GetEditActionsEnabled(int& ea) {
  int allowed = 0;
  int forbidden = 0;
  taMatrix* mat = this->mat(); // may not have a model/mat!
  if (mat) {
    taiTabularDataMimeFactory* fact = taiTabularDataMimeFactory::instance();
    CellRange sel;
    GetSel(sel);
    taiMimeSource* ms = taiMimeSource::NewFromClipboard();
    fact->Mat_QueryEditActions(mat, sel, ms, allowed, forbidden);
    delete ms;
  }
  ea = allowed & ~forbidden;
}

void iMatrixTableView::GetSel(CellRange& sel) {
  iMatrixTableModel* mod = qobject_cast<iMatrixTableModel*>(model());
  if (!mod || !selectionModel()) return;
  // first, get the sel assuming no BOT_0
  sel.SetFromModel(selectionModel()->selectedIndexes());
  // if BOT_0, need to flip the row around (and swap fr/to)
  if (mod->matView() == taMisc::BOT_ZERO) {
    int max_row = mod->rowCount() - 1;
    if ((max_row) < 0) return; // row_xx s/already be 0
    int row_to = max_row - sel.row_fr;
    sel.row_fr = max_row - sel.row_to;
    sel.row_to = row_to;
  }
}

bool iMatrixTableView::isFixedRowCount() const {
  taMatrix* mat = this->mat(); // may not have a model/mat!
  if (!mat) return true;
  return (!mat->canResize());
}

void iMatrixTableView::Refresh() {
  UpdateRowHeight();
  
  if (mat()) {
    DataCol* data_col = (DataCol*)mat()->GetOwner(&TA_DataCol);
    if (data_col) {
      for (int col = mat()->colCount() - 1; col >= 0; --col) {
        this->SetColumnWidth(col, data_col->matrix_col_width);
      }
    }
  }

  update();
}

void iMatrixTableView::UpdateRowHeight() {
  QFont cur_font = this->font();
  cur_font.setPointSize(taMisc::GetCurrentFontSize("table"));
  QFontMetrics metrics(cur_font);
  int max_pixels = metrics.height() + 2 * row_margin;
  verticalHeader()->setDefaultSectionSize(max_pixels);
}

void iMatrixTableView::hor_customContextMenuRequested(const QPoint& pos) {
  taiWidgetMenu* menu = new taiWidgetMenu(this, taiWidgetMenu::normal, taiMisc::fonSmall);
  CellRange sel(selectionModel()->selectedIndexes());
  FillContextMenu_impl(CA_COL_HDR, menu, sel);
  if (menu->count() > 0) {
    menu->exec(horizontalHeader()->mapToGlobal(pos));
  }
  delete menu;
}

void iMatrixTableView::FillContextMenu_impl(ContextArea ca, taiWidgetMenu* menu, const CellRange& sel)
{
  // inherited::FillContextMenu_impl(ca, menu, sel);  // would be okay if we want some other generic items
  
  iAction* act = NULL;
  
  // generic col guys
  if (ca == CA_COL_HDR) {
    act = menu->AddItem("Set Fixed Column Width...", taiWidgetMenu::normal,
                        iAction::int_act,
                        this, SLOT(RowColOp(int)), (OP_COL | OP_SET_WIDTH) );
  }
}

void iMatrixTableView::RowColOp_impl(int op_code, const CellRange& sel) {
  if (op_code & OP_ROW) {
    // nothing for matrix rows
  }
  
  if (op_code & OP_COL) {
    if (op_code & OP_SET_WIDTH) {
      int width = ConvertPixelsToChars(columnWidth(sel.col_fr));
      width = QInputDialog::getInt(0, "Set All Column Widths", "Width in Characters:", width, 1);
      DataCol* data_col = (DataCol*)mat()->GetOwner(&TA_DataCol);
      if (data_col) {
        data_col->matrix_col_width = width;
      }
      
      for (int col = mat()->colCount() - 1; col >= 0; --col) {
        this->SetColumnWidth(col, width);
      }
    }
  }
}

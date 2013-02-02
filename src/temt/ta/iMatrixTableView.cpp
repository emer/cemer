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

#include "iMatrixTableView.h"
#include <taMatrix>
#include <iMatrixTableModel>
#include <taiTabularDataMimeFactory>
#include <CellRange>
#include <taiMimeSource>
#include <iClipData>

#include <taMisc>
#include <taiMisc>



iMatrixTableView::iMatrixTableView(QWidget* parent)
:inherited(parent)
{
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


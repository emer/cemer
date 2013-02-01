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

#include "taiTabularDataMimeFactory.h"
#include <CellRange>
#include <DataTable>
#include <iClipData>
#include <taiMatrixDataMimeItem>
#include <taiTableDataMimeItem>
#include <taiMimeSource>
#include <taiTsvMimeItem>
#include <taProject>

#include <QApplication>
#include <QClipboard>


/* Tabular Data semantics

  In general, we do not try to interpret the tabular data, since there
  are so many combinations, including from external sources -- we will
  make a "best effort" to paste data, by using the built-in string
  converters.

  We can always (try) pasting data into a Matrix, since we can always
  interpret the data in a flattened way.



  Paste semantics

  Matrix:
    * if 1 cell is selected, it is assumed that we want to paste
      "as much as possible";
    * if there is a selection, then we limit the pasting to that
      region
*/

const String taiTabularDataMimeFactory::tacss_matrixdesc("tacss/matrixdesc");
const String taiTabularDataMimeFactory::tacss_tabledesc("tacss/tabledesc");

void taiTabularDataMimeFactory::Initialize() {
}

void taiTabularDataMimeFactory::Mat_Clear(taMatrix* mat,
    const CellRange& sel) const
{
  mat->DataUpdate(true);
  for (int row = sel.row_fr; row <= sel.row_to; ++row) {
    for (int col = sel.col_fr; col <= sel.col_to; ++col) {
      int idx = mat->FastElIndex2D(col, row);
      mat->SetFmVar_Flat(_nilVariant, idx);
    }
  }
  mat->DataUpdate(false);
}

void taiTabularDataMimeFactory::Mat_QueryEditActions(taMatrix* mat,
  const CellRange& sel, taiMimeSource* ms,
  int& allowed, int& forbidden) const
{
  // ops that are never allowed on mats
  forbidden |= (iClipData::EA_CUT | iClipData::EA_DELETE);
  // forbidden on ro
  if (mat->isGuiReadOnly())
    forbidden |= iClipData::EA_FORB_ON_DST_READONLY;
  // src ops
  if (sel.nonempty())
    allowed |= (iClipData::EA_COPY | iClipData::EA_CLEAR);

  if (!ms) return;
  // dst ops -- none allowed if no selection
  if (sel.empty()) {
    forbidden = iClipData::EA_DST_OPS;
    return;
  }

  // Priority is: Mat, Table, Generic
  {taiMatrixDataMimeItem* mi = (taiMatrixDataMimeItem*)
     ms->GetMimeItem(&TA_taiMatrixDataMimeItem);
  if (mi) {
    allowed |= iClipData::EA_PASTE;
    return;
  }}
  // Table
  {taiTableDataMimeItem* mi = (taiTableDataMimeItem*)
     ms->GetMimeItem(&TA_taiTableDataMimeItem);
  if (mi) {
    allowed |= iClipData::EA_PASTE;
    return;
  }}
  // TSV
  {taiTsvMimeItem* mi = (taiTsvMimeItem*)
    ms->GetMimeItem(&TA_taiTsvMimeItem);
  if (mi) {
    allowed |= iClipData::EA_PASTE;
  }}
}

void taiTabularDataMimeFactory::Mat_EditActionD(taMatrix* mat,
  const CellRange& sel, taiMimeSource* ms, int ea) const
{
  int allowed = 0;
  int forbidden = 0;
  Mat_QueryEditActions(mat, sel, ms, allowed, forbidden);
  ea = ea & (allowed & ~forbidden);

  if (ea & iClipData::EA_PASTE) {
    taProject* proj = dynamic_cast<taProject*>(mat->GetThisOrOwner(&TA_taProject));
    if(proj) {
      proj->undo_mgr.SaveUndo(mat, "Paste/Copy", mat);
    }

    CellRange sel2(sel);
    // if sel is a single cell, adjust to max
    if (sel2.single()) {
      sel2.col_to = mat->geom.dim(0) - 1;
      sel2.row_to = mat->rowCount() - 1;
    }
    // Priority is: Mat, Table, Generic
    {taiMatrixDataMimeItem* mi = (taiMatrixDataMimeItem*)
      ms->GetMimeItem(&TA_taiMatrixDataMimeItem);
    if (mi) {
      mi->WriteMatrix(mat, sel2);
      return;
    }}
    // Table
    {taiTableDataMimeItem* mi = (taiTableDataMimeItem*)
      ms->GetMimeItem(&TA_taiTableDataMimeItem);
    if (mi) {
      mi->WriteMatrix(mat, sel2);
      return;
    }}
    // TSV
    {taiTsvMimeItem* mi = (taiTsvMimeItem*)
      ms->GetMimeItem(&TA_taiTsvMimeItem);
    if (mi) {
      mi->WriteMatrix(mat, sel2);
    }}
  }
}

void taiTabularDataMimeFactory::Mat_EditActionS(taMatrix* mat,
  const CellRange& sel, int ea) const
{
  int allowed = 0;
  int forbidden = 0;
  Mat_QueryEditActions(mat, sel, NULL, allowed, forbidden);
  ea = ea & (allowed & ~forbidden);

  if (ea & iClipData::EA_COPY) {
    iClipData* cd = Mat_GetClipData(mat,
      sel, iClipData::EA_SRC_COPY, false);
    QApplication::clipboard()->setMimeData(cd); //cb takes ownership
  } else
  if (ea & iClipData::EA_CLEAR) {
    taProject* proj = dynamic_cast<taProject*>(mat->GetThisOrOwner(&TA_taProject));
    if(proj) {
      proj->undo_mgr.SaveUndo(mat, "Clear", mat);
    }
    Mat_Clear(mat, sel);
  }
}

iClipData* taiTabularDataMimeFactory::Mat_GetClipData(taMatrix* mat,
    const CellRange& sel, int src_edit_action, bool for_drag) const
{
  iClipData* cd = new iClipData(src_edit_action);
  AddMatDesc(cd, mat, sel);
  String str = mat->FlatRangeToTSV(sel);
  cd->setTextFromStr(str);
  return cd;
}

void taiTabularDataMimeFactory::AddMatDesc(QMimeData* md,
  taMatrix* mat, const CellRange& sel) const
{
  String str;
  AddDims(sel, str);
  md->setData(tacss_matrixdesc, StrToByteArray(str));
}

void taiTabularDataMimeFactory::AddDims(const CellRange& sel, String& str) const
{
  str = str + String(sel.width()) + ";" + String(sel.height()) + ";";
}

void taiTabularDataMimeFactory::Table_Clear(DataTable* tab,
    const CellRange& sel) const
{
  tab->DataUpdate(true);
  //note: it is easier and more efficient to clear in col-major order
  for (int col = sel.col_fr; col <= sel.col_to; ++col) {
    DataCol* da = tab->GetColData(col, true); // quiet
    if (!da) continue;
    // use bracketed data update, because these get sent to all slices, including gui
    da->AR()->DataUpdate(true);
    int cell_size = da->cell_size();
    for (int row = sel.row_fr; row <= sel.row_to; ++row) {
      for (int cell = 0; cell < cell_size; ++cell) {
        tab->SetValAsVarM(_nilVariant, col, row, cell);
      }
    }
    da->AR()->DataUpdate(false);
  }
  tab->DataUpdate(false);
}


void taiTabularDataMimeFactory::Table_QueryEditActions(DataTable* tab,
  const CellRange& sel, taiMimeSource* ms,
  int& allowed, int& forbidden) const
{
  // ops that are never allowed on mats
  forbidden |= (iClipData::EA_CUT | iClipData::EA_DELETE);
  // forbidden on ro -- whole table
  if (isGuiReadOnly())
    forbidden |= iClipData::EA_FORB_ON_DST_READONLY;
  // selected cols
  bool sel_ro = false; // we'll or in
  for (int col = sel.col_fr; (col <= sel.col_to) && !sel_ro; ++col) {
    DataCol* da = tab->GetColData(col);
    sel_ro = sel_ro || (da->col_flags & DataCol::READ_ONLY);
  }
  if (sel_ro) {
    forbidden |= iClipData::EA_FORB_ON_DST_READONLY;
  }

  // src ops
  if (sel.nonempty())
    allowed |= (iClipData::EA_COPY | iClipData::EA_CLEAR);

  if (!ms) return;
  // dst ops -- none allowed if no selection
  if (sel.empty()) {
    forbidden = iClipData::EA_DST_OPS;
    return;
  }

  // Priority is: Table, Matrix, Generic
  // Table
  {taiTableDataMimeItem* mi = (taiTableDataMimeItem*)
     ms->GetMimeItem(&TA_taiTableDataMimeItem);
  if (mi) {
    allowed |= iClipData::EA_PASTE;
    return;
  }}
  // Matrx
  {taiMatrixDataMimeItem* mi = (taiMatrixDataMimeItem*)
     ms->GetMimeItem(&TA_taiMatrixDataMimeItem);
  if (mi) {
    allowed |= iClipData::EA_PASTE;
    return;
  }}
  // TSV
  {taiTsvMimeItem* mi = (taiTsvMimeItem*)
    ms->GetMimeItem(&TA_taiTsvMimeItem);
  if (mi) {
    allowed |= iClipData::EA_PASTE;
  }}
}

void taiTabularDataMimeFactory::Table_EditActionD(DataTable* tab,
  const CellRange& sel, taiMimeSource* ms, int ea) const
{
  int allowed = 0;
  int forbidden = 0;
  Table_QueryEditActions(tab, sel, ms, allowed, forbidden);
  ea = ea & (allowed & ~forbidden);

  if (ea & iClipData::EA_PASTE) {
    taProject* proj = dynamic_cast<taProject*>(tab->GetThisOrOwner(&TA_taProject));
    if(proj) {
      proj->undo_mgr.SaveUndo(tab, "Paste/Copy", tab);
    }

    CellRange sel2(sel);
    //NOTE: unlike matrix pastes, we do NOT adjust selection
    // (the Table item may adjust the selection to fit)

    // Priority is: Table, Matrix, Generic
    // Table
    {taiTableDataMimeItem* mi = (taiTableDataMimeItem*)
      ms->GetMimeItem(&TA_taiTableDataMimeItem);
    if (mi) {
      mi->WriteTable(tab, sel2);
      return;
    }}
    // Matrix
    {taiMatrixDataMimeItem* mi = (taiMatrixDataMimeItem*)
      ms->GetMimeItem(&TA_taiMatrixDataMimeItem);
    if (mi) {
      mi->WriteTable(tab, sel2);
      return;
    }}
    // TSV
    {taiTsvMimeItem* mi = (taiTsvMimeItem*)
      ms->GetMimeItem(&TA_taiTsvMimeItem);
    if (mi) {
      mi->WriteTable(tab, sel2);
    }}
  }
}

void taiTabularDataMimeFactory::Table_EditActionS(DataTable* tab,
  const CellRange& sel, int ea) const
{
  int allowed = 0;
  int forbidden = 0;
  Table_QueryEditActions(tab, sel, NULL, allowed, forbidden);
  ea = ea & (allowed & ~forbidden);

  if (ea & iClipData::EA_COPY) {
    iClipData* cd = Table_GetClipData(tab,
      sel, iClipData::EA_SRC_COPY, false);
    QApplication::clipboard()->setMimeData(cd); //cb takes ownership
    return;
  } else
  if (ea & iClipData::EA_CLEAR) {
    taProject* proj = dynamic_cast<taProject*>(tab->GetThisOrOwner(&TA_taProject));
    if(proj) {
      proj->undo_mgr.SaveUndo(tab, "Clear", tab);
    }
    Table_Clear(tab, sel);
    return;
  }
}

iClipData* taiTabularDataMimeFactory::Table_GetClipData(DataTable* tab,
    const CellRange& sel, int src_edit_action, bool for_drag) const
{
  iClipData* cd = new iClipData(src_edit_action);
  AddTableDesc(cd, tab, sel);
  String str = tab->RangeToTSV(sel);
  cd->setTextFromStr(str);
  return cd;
}

void taiTabularDataMimeFactory::AddTableDesc(QMimeData* md,
  DataTable* tab, const CellRange& sel) const
{
  STRING_BUF(str, 250);
  // add the flat dims
  int tot_col = 0; // total flat cols
  int max_cell_row = 0; // max flat rows per cell
  tab->GetFlatGeom(sel, tot_col, max_cell_row);
  str.cat(String(tot_col)).cat(';').cat(String(max_cell_row)).cat(";\n");

  // add the table dims
  AddDims(sel, str);
  str.cat('\n');

  // add the col descs
  for (int col = sel.col_fr; col <= sel.col_to; ++col) {
    DataCol* da = tab->GetColData(col, true); // quiet
    int x; int y;
    da->Get2DCellGeom(x, y);
    str.cat(String(x)).cat(';').cat(String(y)).cat(';');
    str.cat(String(da->isImage())).cat(";\n");
  }
  md->setData(tacss_tabledesc, StrToByteArray(str));
}



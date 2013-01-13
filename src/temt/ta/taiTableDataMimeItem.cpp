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

#include "taiTableDataMimeItem.h"
#include <taiMimeSource>
#include <taiTabularDataMimeItem>
#include <taiTabularDataMimeFactory>
#include <CellRange>
#include <DataTable>

#include <taMisc>

#include <sstream>

using namespace std;


taiMimeItem* taiTableDataMimeItem::Extract(taiMimeSource* ms,
    const String& subkey)
{
  if (!ms->hasFormat(taiTabularDataMimeFactory::tacss_tabledesc))
    return NULL;
  taiTableDataMimeItem* rval = new taiTableDataMimeItem;
  rval->Constr(ms, subkey);
  return rval;
}

void taiTableDataMimeItem::Initialize() {
  m_max_row_geom = 0;
}

bool taiTableDataMimeItem::Constr_impl(const String&) {
  String arg;
  data(mimeData(), taiTabularDataMimeFactory::tacss_tabledesc, arg);

  String str; // temp
  bool ok = ExtractGeom(arg, m_flat_geom);
  if (!ok) goto done;
  arg = arg.after('\n');
  ok = ExtractGeom(arg, m_tab_geom);
  if (!ok) goto done;
  arg = arg.after('\n');

  col_descs.Alloc(m_tab_geom.w);
  m_max_row_geom = 0;
  for (int col = 0; col < m_tab_geom.w; ++col) {
    taiTableColDesc& desc = col_descs[col];
    ok = ExtractGeom(arg, desc.flat_geom);
    if (!ok) goto done;
    str = arg.before(';');
    desc.is_image = str.toBool();
    arg = arg.after('\n');
    m_max_row_geom = MAX(m_max_row_geom, desc.flat_geom.h);
  }

done:
  return ok;
}

void taiTableDataMimeItem::DecodeData_impl() {
//note: maybe nothing!
}

void taiTableDataMimeItem::GetColGeom(int col, int& cols, int& rows) const {
  if ((col >= 0) && (col < col_descs.size)) {
    taiTableColDesc& desc = col_descs[col];
    cols = desc.flat_geom.w;
    rows = desc.flat_geom.h;
  } else {
    cols = 0;
    rows = 0;
  }
}

/*
  A table supports two kinds of paste:
    * Paste -- starting at upper-left of select region, no new rows made
    * Paste Append -- appends all data to new rows; the first dst col is
      marked by the left selected col -- other than that, the sel region
      has no significance
  (Paste Append is especially convenient when selected from the Table
    object itself, rather than in the data grid.)

  For table-to-table copy, we assume the user wants the source col mappings
  to be preserved at the destination. This leads to three possibilities for
  each dimension in each cell:
    * the src and dst have the same dim
    * the src is bigger -- we will ignore the excess
    * the src is smaller -- we will nil out the underflow
  We should iterate, using a parallel set of indexes, one for src, one for
  dst; we will always need to do something for the max of these two, either
  skipping src data, or clearing out dst data; the only exception is row:
  we only need iterate the dst_rows, since we can just stop reading src
  data when we are finished.

  Since we placed the exact dims for each col on the clipboard, and we
  have a deterministic mapping of the flat data, we can therefore "dumbly"
  just read in the data, without checking for tabs/eols etc.


*/
void taiTableDataMimeItem::WriteTable(DataTable* tab, const CellRange& sel_) {
  // for table-to-table copy, we apply data on a table-cell basis
  // (not a fully flattened basis)
  istringstream istr;
  QByteArray ba = data(taiMimeFactory::text_plain);
  istr.str(string(ba.data(), ba.size()));

  String val; // each cell val
//nn  TsvSep sep; // sep after reading the cell val

  // if dest is a single cell, we will extend to size of src
  CellRange sel(sel_);
  if (sel_.single()) {
    sel.SetExtent(tabCols(), tabRows());
    sel.LimitRange(tab->rows - 1, tab->cols() - 1);
  }
  // if dest is larger than src, we shrink to src
  else {
    sel.LimitExtent(tabCols(), tabRows());
  }

  // calculate the controlling params, for efficiency and clarity
  int src_rows = tabRows();
  int dst_max_cell_rows = tab->GetMaxCellRows(sel.col_fr, sel.col_to);
  int src_max_cell_rows = maxCellRows();
  int max_cell_rows = MAX(src_max_cell_rows, dst_max_cell_rows);
  // for cols, we only iterate over src, since we don't clear excess dst cols
  int src_cols = tabCols();
//nn  int dst_cols = sel.width();

  bool underflow = false;
  tab->DataUpdate(true);
  // we only iterate over the dst rows; we'll just stop reading data when done
  int dst_row; int src_row;
  for (dst_row = sel.row_fr, src_row = 0;
       dst_row <= sel.row_to;
       ++dst_row, ++src_row)
  {
    // cell_row is same for src/dst (0-based), but we need to iterate the largest
    for (int cell_row = 0; cell_row < max_cell_rows; ++cell_row) {
      // for cols, we need to iterate over largest of src/dst
      int dst_col; int src_col;
      for (dst_col = sel.col_fr, src_col = 0;
          src_col < src_cols;
          ++dst_col, ++src_col)
      {
        // here, we get the detailed cell geom for src and dst
        // just assume either could be empty, for robustness
        int dst_cell_cols = 0;
        int dst_cell_rows = 0;
        DataCol* da = tab->GetColData(dst_col, true); // quiet
        if (da) da->Get2DCellGeom(dst_cell_cols, dst_cell_rows);
        int src_cell_cols = 0;
        int src_cell_rows = 0;
        if (src_col < src_cols) {
          taiTableColDesc& tcd = col_descs[src_col];
          src_cell_cols = tcd.flat_geom.w;
          src_cell_rows = tcd.flat_geom.h;
        }
        int cell_cols = MAX(src_cell_cols, dst_cell_cols);
        for (int cell_col = 0; cell_col < cell_cols; ++cell_col) {
          // if we are in **flat** source range then read a value
          // note that src values are always zero-based
          if ((src_row < src_rows) && (cell_row < src_max_cell_rows) &&
            (src_col < src_cols) && (cell_col < src_cell_cols))
          {
            underflow = underflow || (!ReadTsvValue(istr, val/*, sep*/));
          } else
            val = _nilString;

          // if we are in dst range then write (the maybe nil) value
          // we only need to check upper bound, since we always started at lower
          if (/*row always valid*/ (cell_row < dst_cell_rows) &&
            (dst_col <= sel.col_to) && (cell_col < dst_cell_cols))
          {
            int dst_cell = (cell_row * dst_cell_cols) + cell_col;
            tab->SetValAsStringM(val, dst_col, dst_row, dst_cell);
          }
        }
      }
    }
  }
  tab->DataUpdate(false);
  if (underflow)
    taMisc::DebugInfo("Unexpected underflow of table data pasting table-to-table");
}

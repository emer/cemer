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

#include "taiTabularDataMimeItem.h"
#include <taiMimeFactory>
#include <CellRange>
#include <DataTable>
#include <taMatrix>

#include <sstream>

using namespace std;

/* TSV reading engine

the most common spreadsheet format is TSV -- note that
tabs and eols are separators, not terminators

value[<tab>value]
[<eol>] value[<tab>value]

We need to be able to read, being able to skip cols,
knowing when we reach eols.

Parsing geoms:

  #rows = #eols - 1
  #cols = row0(#tabs - 1)

*/

taiTabularDataMimeItem::TsvSep taiTabularDataMimeItem::no_sep;

bool taiTabularDataMimeItem::ReadInt(String& arg, int& val) {
  String str;
  bool ok;
  str = arg.before(';'); // cols
  val = str.toInt(&ok);
  arg = arg.after(";");
  return ok;
}

bool taiTabularDataMimeItem::ExtractGeom(String& arg, iSize& val) {
  String str;
  bool ok = ReadInt(arg, val.w);
  if (!ok) goto end;
  ok = ReadInt(arg, val.h);
  if (!ok) goto end;

end:
  return ok;
}

void taiTabularDataMimeItem::WriteMatrix(taMatrix* mat, const CellRange& sel) {
  // get the text/plain data and set it into a stream
  istringstream istr;
  QByteArray ba = data(taiMimeFactory::text_plain);
  istr.str(string(ba.data(), ba.size()));

  String val; // each cell val
  TsvSep sep; // sep after reading the cell val

  // client needs to have adjust paste region if necessary; we take it literally

  int row = sel.row_fr;
  mat->DataUpdate(true);
  while (row <= sel.row_to) {
    int col = sel.col_fr;
    while (col <= sel.col_to) {
      if (!ReadTsvValue(istr, val, sep)) goto done;
      int idx = mat->FastElIndex2D(col, row);
      mat->SetFmStr_Flat(val, idx);
      if (sep == TSV_EOF) goto done;
      // if we've run out of col data, skip to next row
      if (sep == TSV_EOL) goto next_row;
      ++col;

    }
    // if we haven't run out of col data yet, skip input data
    while (sep != TSV_EOL) {
      if (!ReadTsvValue(istr, val, sep)) goto done;
      if (sep == TSV_EOF) goto done;
    }
next_row:
    ++row;
  }
done:
  mat->DataUpdate(false);
  //NOTE: this is VERY hacky, but easiest way to update tables...
  DataCol* col = (DataCol*)mat->GetOwner(&TA_DataCol);
  if (col) {
    col->DataChanged(DCR_ITEM_UPDATED);
  }
}

void taiTabularDataMimeItem::WriteTable(DataTable* tab, const CellRange& sel) {
  // default does the generic -- Table source is more flexible
  WriteTable_Generic(tab, sel);
}

void taiTabularDataMimeItem::WriteTable_Generic(DataTable* tab, const CellRange& sel_) {
  // for generic-to-table copy, we apply data on a fully flattened basis
  // we paste in each direction (rows/cols) until we run out of data, or table
  istringstream istr;
  QByteArray ba = data(taiMimeFactory::text_plain);
  istr.str(string(ba.data(), ba.size()));

  String val; // each cell val
  TsvSep sep; // sep after reading the cell val

  // for generic source, for single-cell, we autoextend the range
  // to the entire extent of the table

  CellRange sel(sel_);
  if (sel_.single()) {
    sel.row_to = tab->rows - 1;
    sel.col_to = tab->cols() - 1;
  }

  // we will use the first row pass to find the max cell rows
  int max_cell_rows = 1; // has to be at least 1 (for all scalars)
  tab->DataUpdate(true);
  // we only iterate over the dst rows; we'll just stop reading data when done
  // and/or break out if we run out of data
  for (int dst_row = sel.row_fr; dst_row <= sel.row_to; ++dst_row) {
    for (int cell_row = 0; cell_row < max_cell_rows; ++cell_row) {
      // we keep iterating while we have data and cols
      for (int dst_col = sel.col_fr; dst_col <= sel.col_to; ++dst_col)
      {
        // here, we get the detailed cell geom for src and dst
        // just assume either could be empty, for robustness
        int dst_cell_cols = 0; //dummy
        int dst_cell_rows = 0;
        DataCol* da = tab->GetColData(dst_col, true); // quiet
        if (da) da->Get2DCellGeom(dst_cell_cols, dst_cell_rows);
        //note: only need to do the following in the very first cell_row
        max_cell_rows = MAX(max_cell_rows, dst_cell_rows);
        // we break if we run out of data
        for (int cell_col = 0; cell_col < dst_cell_cols; ++cell_col)
        {
          // we are always in source range here...
          if (!ReadTsvValue(istr, val, sep)) goto done;

          // if we are in dst range then write the value
          if (/*row always valid*/ (cell_row < dst_cell_rows) &&
             (cell_col < dst_cell_cols))
          {
            int dst_cell = (cell_row * dst_cell_cols) + cell_col;
            tab->SetValAsStringM(val, dst_col, dst_row, dst_cell);
          }

          if (sep == TSV_EOF) goto done;
          // if we've run out of col data, skip to next row
          if (sep == TSV_EOL) {
            // if we've run out of data, set this as last col and break
            sel.col_to = dst_col;
            goto next_row;
          }
        }
        // if we're on the last col, we may have ran out of table -- read the rest of the row here, which will cause the loop
  // to be
        if (dst_col == sel.col_to) while (sep != TSV_EOL) {
          if (!ReadTsvValue(istr, val, sep)) goto done;
          if (sep == TSV_EOF) goto done;
        }
      } // dst tab cols
next_row: ;
    } // dst tab cell rows
  } // dst tab rows
done:
  tab->DataUpdate(false);
}

bool taiTabularDataMimeItem::ReadTsvValue(istringstream& strm, String& val,
  TsvSep& sep)
{
  // unless 1st read is EOF, we will succeed in reading something
  val = _nilString;
  int c = strm.peek();
  if (c == EOF) {
    sep = TSV_EOF;
    return false;
  }
  while (c != EOF) {
    if (c == '\t') {
      strm.get();
      sep = TSV_TAB;
      break;
    }
    if ((c == '\n') || (c == '\r')) {
      strm.get();
      sep = TSV_EOL;
      // Windows/DOS cr+lf
      if ((c == '\r') && (strm.peek() == '\n')) {
        strm.get();
      }
      // we ignore terminating eol
      if (strm.peek() == EOF) {
        strm.get();
        sep = TSV_EOF;
      }
      break;
    }

    val += (char)c;
    strm.get();
    c = strm.peek();
    if (c == EOF) {
      strm.get();
      sep = TSV_EOF;
    }
  }
  return true;
}

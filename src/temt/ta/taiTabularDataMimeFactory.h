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

#ifndef taiTabularDataMimeFactory_h
#define taiTabularDataMimeFactory_h 1

// parent includes:
#include <taiMimeFactory>

// member includes:

// declare all other types mentioned but not required to include:
class CellRange; //
class DataTable; //
class taMatrix; //

/*
  MIME TYPE "tacss/matrixdesc" -- description of matrix data (no content)

    <flat_cols>;<flat_rows>;\n


    The data itself (text/plain) is in TSV format.

    flat_cols/rows (>=1) indicate the flattend 2D rep of the data

    Note that this format is primarily to make decoding of the data faster
    and more definite where tacss is the source of the data, compared with
    just parsing the text/plain data (which the decoder can do, to import
    spreadsheet data.)
    .

  MIME TYPE "tacss/tabledesc" -- description of table data (no content)

    <flat_cols>;<flat_rows>;\n
    <mat_cols>;<mat_rows>;\n
    <col0_flat_cols>;<col0_flat_rows>;<is_image>;\n
    ...
    <colN-1-flat_cols>;<colN-1_flat_rows>;<is_image>;\n

    for scalar cols: colx-cols=colx-rows=1

    The data itself (text/plain) is in a TSV tabular form, of total
    Sigma(colx-cols)x=0:N-1 by <rows> * Max(colx-rows) -- non-existent values
    will just have blank entries.


*/

TypeDef_Of(taiTabularDataMimeFactory);

class TA_API taiTabularDataMimeFactory: public taiMimeFactory {
// this factory handles both Matrix and Table clipboard formats
INHERITED(taiMimeFactory)
public:
  static const String   tacss_matrixdesc; // "tacss/matrixdesc"
  static const String   tacss_tabledesc; // "tacss/tabledesc"
//static taiTabularDataMimeFactory* instance(); // provided by macro

  void                  Mat_QueryEditActions(taMatrix* mat,
    const CellRange& selected, taiMimeSource* ms,
    int& allowed, int& forbidden) const; // determine ops based on clipboard and selected; ms=NULL for source only

  void                  Mat_EditActionD(taMatrix* mat,
    const CellRange& selected, taiMimeSource* ms, int ea) const;
    // dest edit actions; note: this does the requery to insure it is still legal
  void                  Mat_EditActionS(taMatrix* mat,
    const CellRange& selected, int ea) const;
    // src edit actions; note: this does the requery to insure it is still legal

  taiClipData*          Mat_GetClipData(taMatrix* mat,
    const CellRange& sel, int src_edit_action, bool for_drag = false) const;

  void                  Mat_Clear(taMatrix* mat,
    const CellRange& sel) const;
  void                  AddMatDesc(QMimeData* md,
    taMatrix* mat, const CellRange& selected) const;


  void                  Table_QueryEditActions(DataTable* tab,
    const CellRange& selected, taiMimeSource* ms,
    int& allowed, int& forbidden) const; // determine ops based on clipboard and selected; ms=NULL for source only

  void                  Table_EditActionD(DataTable* tab,
    const CellRange& selected, taiMimeSource* ms, int ea) const;
    // dest edit actions; note: this does the requery to insure it is still legal
  void                  Table_EditActionS(DataTable* tab,
    const CellRange& selected, int ea) const;
    // src edit actions; note: this does the requery to insure it is still legal

  taiClipData*          Table_GetClipData(DataTable* tab,
    const CellRange& sel, int src_edit_action, bool for_drag = false) const;

  void                  Table_Clear(DataTable* tab,
    const CellRange& sel) const;

  void                  AddTableDesc(QMimeData* md,
    DataTable* tab, const CellRange& selected) const;


  TA_MFBASEFUNS(taiTabularDataMimeFactory);
protected:
  void                  AddDims(const CellRange& sel, String& str) const;

private:
  void  Initialize();
  void  Destroy() {}
};

#endif // taiTabularDataMimeFactory_h

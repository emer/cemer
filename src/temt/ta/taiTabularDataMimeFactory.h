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

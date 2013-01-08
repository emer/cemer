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

#ifndef taiTableDataMimeItem_h
#define taiTableDataMimeItem_h 1

// parent includes:
#include <taiTabularDataMimeItem>

// member includes:

// declare all other types mentioned but not required to include:


class TA_API taiTableDataMimeItem: public taiTabularDataMimeItem { // for DataTable data
INHERITED(taiTabularDataMimeItem)
public:
  iSize                 tabGeom() const {return m_tab_geom;} // the table size of the data in table rows/cols (same as flat, if all cols are scalar)
  inline int            tabRows() const {return m_tab_geom.h;}
  inline int            tabCols() const {return m_tab_geom.w;}

  virtual void          GetColGeom(int col, int& cols, int& rows) const;
    // 2-d geom of the indicated column; always 1x1 (scalar) for matrix data
  inline int            maxCellRows() const {return m_max_row_geom;}

  override void         WriteTable(DataTable* tab, const CellRange& sel);

  TA_BASEFUNS_NOCOPY(taiTableDataMimeItem);

public: // TAI_xxx instance interface -- used for dynamic creation
  override taiMimeItem* Extract(taiMimeSource* ms,
    const String& subkey = _nilString);
protected:
  iSize                 m_tab_geom;
  int                   m_max_row_geom;
  taiTableColDesc_PArray col_descs;
  override bool         Constr_impl(const String&);
  override void         DecodeData_impl();
private:
  void  Initialize();
  void  Destroy() {}
};

#endif // taiTableDataMimeItem_h

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

#ifndef taiTabularDataMimeItem_h
#define taiTabularDataMimeItem_h 1

// parent includes:
#include <taiMimeItem>

// member includes:
#include <iSize>

// declare all other types mentioned but not required to include:
class CellRange; //
class DataTable; //
class taMatrix; //


taTypeDef_Of(taiTabularDataMimeItem);

class TA_API taiTabularDataMimeItem: public taiMimeItem {
  // #NO_INSTANCE #VIRT_BASE base for matrix, tsv, and table data; this class is not itself instantiated
INHERITED(taiMimeItem)
public: // i/f for tabular data guy
  iSize                 flatGeom() const {return m_flat_geom;} // the (flat) size of the data in rows/cols
  inline int            flatRows() const {return m_flat_geom.h;}
  inline int            flatCols() const {return m_flat_geom.w;}

  virtual void          WriteMatrix(taMatrix* mat, const CellRange& sel);
  virtual void          WriteTable(DataTable* tab, const CellRange& sel);

  TA_ABSTRACT_BASEFUNS(taiTabularDataMimeItem)

protected:
  enum TsvSep { // for reading tsv text streams
    TSV_TAB,  // tab -- item separator
    TSV_EOL,  // eol -- row separator
    TSV_EOF   // eof -- end of file
  };

  static TsvSep         no_sep; // when ignored

  iSize                 m_flat_geom;
  bool                  ReadInt(String& arg, int& val); // read a ; terminated int
  bool                  ExtractGeom(String& arg, iSize& val); // get the cols/rows
#ifndef __MAKETA__
  bool                  ReadTsvValue(std::istringstream& strm, String& val,
     TsvSep& sep = no_sep); // reads value if possible, into val, returning true if a value read, and the separator encountered after the value in sep.
#endif
  virtual void          WriteTable_Generic(DataTable* tab, const CellRange& sel);

private:
  NOCOPY(taiTabularDataMimeItem);
  void  Initialize() {}
  void  Destroy() {}
};

#endif // taiTabularDataMimeItem_h

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

#ifndef CellRange_h
#define CellRange_h 1

// parent includes:
#include "ta_def.h"

// member includes:

// declare all other types mentioned but not required to include:
class QModelIndex;
#ifdef __MAKETA__
class   QModelIndexList;        // #IGNORE
#else
#include <QList>
typedef QList<QModelIndex> QModelIndexList;
#endif

taTypeDef_Of(CellRange);

class TA_API CellRange { // specifies a range of cells
public:
  int row_fr;
  int col_fr;
  int row_to;
  int col_to;

  bool          empty() const {return ((row_to < row_fr) || (col_to < col_fr));}
  inline bool   nonempty() const {return !empty();}
  int           height() const {return row_to - row_fr + 1;}
  int           width() const {return col_to - col_fr + 1;}
  bool          single() const {return ((row_to == row_fr) && (col_to == col_fr));}

  void          SetExtent(int wd, int ht); // set w/h based on current fr values
  void          LimitExtent(int wd, int ht); // limit width and height to these values
  void          LimitRange(int row_to_, int col_to_);
   // limit range to within these values
  void          Set(int row_fr_, int col_fr_, int row_to_, int col_to_)
    {row_fr = row_fr_; col_fr = col_fr_; row_to = row_to_; col_to = col_to_;}
  void          SetFromModel(const QModelIndexList& indexes); //#IGNORE

  CellRange() {Set(0, 0, -1, -1);} // note that default is a null selection
  CellRange(int row_fr_, int col_fr_) // single cell
    {Set(row_fr_, col_fr_, row_fr_, col_fr_);}
  CellRange(int row_fr_, int col_fr_, int row_to_, int col_to_)
    {Set(row_fr_, col_fr_, row_to_, col_to_);}
   explicit CellRange(const QModelIndexList& indexes)
     {SetFromModel(indexes);}
};

#endif // CellRange_h

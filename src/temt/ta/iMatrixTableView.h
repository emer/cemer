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

#ifndef iMatrixTableView_h
#define iMatrixTableView_h 1

// parent includes:
#include <iTableView>

// member includes:

// declare all other types mentioned but not required to include:
class taMatrix; // 
class CellRange; // 


class TA_API iMatrixTableView: public iTableView {
  // table editor; model flattens >2d into 2d by frames
INHERITED(iTableView)
  Q_OBJECT
public:
  taMatrix*             mat() const;

  bool         isFixedRowCount() const override;
  bool         isFixedColCount() const override {return true;}

  void                  GetSel(CellRange& sel);
   // gets current selection, factoring in BOT_ZERO if needed

  iMatrixTableView(QWidget* parent = NULL);

public: // cliphandler i/f
  void         EditAction(int ea) override;
  void         GetEditActionsEnabled(int& ea) override;
  void         ViewAction(int ea) override;
  void         ResetColorsAction(int ea) override;
};

#endif // iMatrixTableView_h

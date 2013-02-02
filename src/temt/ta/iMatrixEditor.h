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

#ifndef iMatrixEditor_h
#define iMatrixEditor_h 1

// parent includes:
#include "ta_def.h"
#ifndef __MAKETA__
#include <QWidget>
#endif

// member includes:

// declare all other types mentioned but not required to include:
class QVBoxLayout; // 
class QHBoxLayout; // 
class iMatrixTableView; // 
class taMatrix; // 


class TA_API iMatrixEditor: public QWidget {
  // ##NO_INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBER widget that includes a table editor; model flattens >2d into 2d by frames
INHERITED(QWidget)
  Q_OBJECT
public:
  QVBoxLayout*          layOuter;
  QHBoxLayout*            layDims;
  iMatrixTableView*       tv;

 // taMatrix*           mat() const;
 // iMatrixTableModel*   model() const;
  void                  setMatrix(taMatrix* mat, bool pat_4d = false);

  void                  Refresh(); // for manual refresh -- note, this also updates all other mat editors too

  iMatrixEditor(QWidget* parent = NULL); //
  ~iMatrixEditor();

protected:
  taMatrix*             m_mat;

private:
  void          init();

};

#endif // iMatrixEditor_h

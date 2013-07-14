// Copyright, 1995-2013, Regents of the University of Roworado,
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

#ifndef iDataTableRowHeaderView_h
#define iDataTableRowHeaderView_h 1

#include "ta_def.h"
#ifndef __MAKETA__
#include <QHeaderView>
#endif

// member includes:

// declare all other types mentioned but not required to include:
class QWidget; //

class TA_API iDataTableRowHeaderView: public QHeaderView {
  INHERITED(QHeaderView)
          Q_OBJECT

public:
          iDataTableRowHeaderView(QWidget* parent = NULL);
          ~iDataTableRowHeaderView();

protected:
          bool                  m_section_move_complete;

protected slots:
          void                  movedSection(int logicalIdx, int oldVisualIdx, int newVisualIdx);
          void                  rowResized(int column, int oldWidth, int newWidth);

#ifndef __MAKETA__
signals:
          void                  tableViewChange();
#endif // ndef __MAKETA__

};

#endif // iDataTableRowHeaderView_h

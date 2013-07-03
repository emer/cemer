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

#ifndef iDataTableColHeaderView_h
#define iDataTableColHeaderView_h 1

#include "ta_def.h"
#ifndef __MAKETA__
#include <QHeaderView>
#endif

// member includes:

// declare all other types mentioned but not required to include:
class QWidget; //

class TA_API iDataTableColHeaderView: public QHeaderView {
  INHERITED(QHeaderView)
          Q_OBJECT

public:
          iDataTableColHeaderView(QWidget* parent = NULL);
          ~iDataTableColHeaderView();

protected:
          bool                  m_section_move_complete;

          protected slots:
          void                  movedSection(int logicalIdx, int oldVisualIdx, int newVisualIdx);
};

#endif // iDataTableColHeaderView_h

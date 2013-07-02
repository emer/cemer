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
//#include <Qt>
#endif

// member includes:

// declare all other types mentioned but not required to include:
class QWidget; //
class QMouseEvent; //

class TA_API iDataTableColHeaderView: public QHeaderView {
  INHERITED(QHeaderView)
  Q_OBJECT

public:
  iDataTableColHeaderView(QWidget* parent = NULL);
  ~iDataTableColHeaderView();

protected:
  int                   m_old_index;
  bool                  m_dragging;

  virtual void          mouseEnterEvent(QMouseEvent* event );
  virtual void          mousePressEvent(QMouseEvent* event );
  virtual void          mouseMoveEvent(QMouseEvent* event );
  virtual void          mouseReleaseEvent(QMouseEvent* event );
};

#endif // iDataTableColHeaderView_h

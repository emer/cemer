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

#ifndef iSplitterHandle_h
#define iSplitterHandle_h 1

#include "taiqtso_def.h"
#include <QSplitter>

class iSplitterHandle : public QSplitterHandle {
  // ##NO_CSS splitter that supports double-click on splitter handles to expand/collapse
INHERITED(QSplitterHandle)
  Q_OBJECT
public:
  iSplitterHandle(Qt::Orientation orientation, QSplitter *parent = 0);
  ~iSplitterHandle();

protected:
  virtual void mouseDoubleClickEvent(QMouseEvent * event);

signals:
  void handleDoubleClicked( int index );
};

#endif // iSplitterHandle_h

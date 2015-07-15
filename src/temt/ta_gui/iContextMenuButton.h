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

#ifndef iContextMenuButton_h
#define iContextMenuButton_h 1

#include "ta_def.h"
#include <QToolButton>

class TA_API iContextMenuButton: public QToolButton {
  // ##NO_CSS automatically calls associated menu when activated by a context menu event
INHERITED(QToolButton)
  Q_OBJECT
public:
  iContextMenuButton(QWidget* parent = 0);
protected:
  void contextMenuEvent(QContextMenuEvent * event);
};


#endif // iContextMenuButton_h

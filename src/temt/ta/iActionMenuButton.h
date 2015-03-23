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

#ifndef iActionMenuButton_h
#define iActionMenuButton_h 1

// parent includes:
#include "ta_def.h"
#include <QToolButton>

class TA_API iActionMenuButton : public QToolButton {
  // a menu button that retains the last chosen item - similar to iMenuButton.
INHERITED(QToolButton)
  Q_OBJECT

public:
  iActionMenuButton(QWidget* parent = 0);
  
private:

};

#endif // iActionMenuButton_h
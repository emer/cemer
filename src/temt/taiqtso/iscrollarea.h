// Copyright, 1995-2005, Regents of the University of Colorado,
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

#ifndef ISCROLLAREA_H
#define ISCROLLAREA_H

#include "taiqtso_def.h"

#include <QScrollArea>

class iScrollArea: public QScrollArea {
  // ##NO_CSS scroll area with better visual semantics than the stock Qt one
INHERITED(QScrollArea)
  Q_OBJECT
public:

  QSize 	sizeHint() const;

  explicit iScrollArea(QWidget* parent = 0);

private:
  void		init();
};


#endif


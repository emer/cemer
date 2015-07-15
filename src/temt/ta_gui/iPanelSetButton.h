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

#ifndef iPanelSetButton_h
#define iPanelSetButton_h 1

// parent includes:
#include "ta_def.h"
#ifndef __MAKETA__
#include <QToolButton>
#endif

// member includes:

// declare all other types mentioned but not required to include:
class iPanelSet;


class TA_API iPanelSetButton: public QToolButton {
  // ##NO_INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS button for datapanelset -- manages keyboard flow
INHERITED(QToolButton)
  Q_OBJECT
public:
  iPanelSetButton(QWidget* parent = 0);

  iPanelSet* m_datapanelset;
  int           m_idx;
protected:
  bool focusNextPrevChild(bool next) override;
  void keyPressEvent(QKeyEvent* e) override;
};

#endif // iPanelSetButton_h

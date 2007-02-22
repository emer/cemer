// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/CSS
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

#ifndef IBUTTON_H
#define IBUTTON_H

#include "taiqtso_def.h"

#include <QPushButton>
#include <QToolButton>

class iMenuButton: public QToolButton {
  // ##NO_CSS button to use for menu buttons (standalone or in toolbars)
INHERITED(QToolButton)
  Q_OBJECT
public:
  explicit iMenuButton(QWidget* parent = 0);
  iMenuButton(const QString& text, QWidget* parent);

private:
  void		init();
};


#endif


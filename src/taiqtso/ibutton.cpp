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

#include "ibutton.h"

iMenuButton::iMenuButton(QWidget* parent)
:inherited(parent)
{
  init();
}

iMenuButton::iMenuButton(const QString& text_, QWidget* parent)
:inherited(parent)
{
  init();
  setText(text_);
}


void iMenuButton::init() {
  setLayoutDirection(Qt:RightToLeft); // puts arrow icon on right
  setPopupMode(InstantPopup); //note: Menu mode looks/behaves awful
  setArrowType(Qt::DownArrow);
  setToolButtonStyle(Qt:ToolButtonTextBesideIcon);
}


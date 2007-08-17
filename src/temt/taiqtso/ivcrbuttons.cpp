// Copyright, 1995-2007, Regents of the University of Colorado,
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

#include "ta_global.h"

#include "ivcrbuttons.h"

iHiLightButton::iHiLightButton(QWidget* parent = 0, const char* name = 0)
:QPushbutton(parent, name)
{
}

iHiLightButton::iHiLightButton(const QString& text, QWidget* parent, const char* name = 0)
:QPushbutton(text, parent, name)
{
}


void iHiLightButton::init() {
  mhilight = false;
}

void iHiLightButton::setHilight(bool value);

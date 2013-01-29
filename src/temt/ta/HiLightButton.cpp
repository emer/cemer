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

#include "HiLightButton.h"

#include <taMisc>

#include <QApplication>


HiLightButton::HiLightButton(QWidget* parent, const char* script_)
:QPushButton(parent)
{
  init(script_);
}

HiLightButton::HiLightButton(const String& text, QWidget* parent, const char* script_)
:QPushButton(text, parent)
{
  init(script_);
}

HiLightButton::~HiLightButton(){
}


void HiLightButton::init(const char* script_) {
  mhiLight = false;
  mouse_button = Qt::NoButton;
  if (script_ != NULL)
    mscript = script_;
  mhiLight_color.setRgb(0x66, 0xFF, 0x66); // medium-light green
}

void HiLightButton::released() {
  if (!mscript.empty()) {
    taMisc::RecordScript(mscript);
  }
  QPushButton::released();
}

void HiLightButton::setHiLight(bool value) {
  if (mhiLight == value) return;
  QPalette palette;
  if (value) {
    palette.setColor(backgroundRole(), mhiLight_color);
    setPalette(palette);    
  }
  else {
    palette.setColor(backgroundRole(),
                     QApplication::palette().color(QPalette::Active, QPalette::Button));
    setPalette(palette);    
  }
  mhiLight = value;
}

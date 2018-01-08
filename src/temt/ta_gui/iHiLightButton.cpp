// Co2018ght 2013-2017, Regents of the University of Colorado,
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

#include "iHiLightButton.h"

#include <taMisc>
#include <taiMisc>

#include <QApplication>
#include <QSize>


iHiLightButton::iHiLightButton(QWidget* parent, const char* script_)
:QPushButton(parent)
{
  init(script_);
}

iHiLightButton::iHiLightButton(const String& text, QWidget* parent, const char* script_)
:QPushButton(text, parent)
{
  init(script_);
}

iHiLightButton::~iHiLightButton(){
}


void iHiLightButton::init(const char* script_) {
  mhiLight = false;
  mouse_button = Qt::NoButton;
  if (script_ != NULL)
    mscript = script_;
  mhiLight_color.setRgb(0x00, 0x80, 0x00); // dark green 

  // button should use same font as other buttons
  taiMisc::SizeSpec currentSizeSpec = taiM->GetCurrentSizeSpec();
  iFont currentFont = taiM->buttonFont(currentSizeSpec);
  this->setFont(currentFont);

  this->setStyleSheet("color: black"); // sometimes button gets white color font!
}

void iHiLightButton::released() {
  if (!mscript.empty()) {
    taMisc::RecordScript(mscript);
  }
  QPushButton::released();
}

void iHiLightButton::setHiLight(bool value) {
  if (mhiLight == value) return;
  // docs for style sheet: http://doc.qt.io/qt-5/stylesheet-examples.html
  if (value) {
    this->setStyleSheet("color: #" + mhiLight_color.toString() + ";");
  }
  else {
    this->setStyleSheet("color: black");
  }
  mhiLight = value;
}

QSize iHiLightButton::sizeHint() const
{
  taiMisc::SizeSpec currentSizeSpec = taiM->GetCurrentSizeSpec();
  int height = taiM->button_height(currentSizeSpec);
  return QSize(this->width(), height*1.2); // make it taller than our other buttons because of the way a pushButton is drawn
}


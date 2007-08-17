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

// Copyright (C) 1995-2007 Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.

#include "ifont.h"

#include <string.h>
#include <ta_type.h>

#ifdef TA_GUI
#include <qfont.h>
#endif

#ifdef TA_USE_INVENTOR
#include <Inventor/nodes/SoFont.h>
#endif

String iFont::defName = "Helvetica";

iFont::iFont() {
  init();
}

void iFont::init() {
  fontName = taMisc::font_name; // defName;
  attribs = (Attributes)0;
  stretch = 100; //note: default for Qt
  pointSize = 10; // TODO: verify default point size
}

iFont::~iFont() {
}

iFont::iFont(const iFont& val) {
  copy(val);
}

void iFont::copy(const iFont& src) {
  fontName = src.fontName;
  attribs = src.attribs;
  pointSize = src.pointSize;
  stretch = src.stretch;
}

bool iFont::equals(const iFont& b) {
//NOTE: pointSize is float, but we are basically interested in whether
// the two fonts are clones, in which case the vals should be identical
  return ( 
    (fontName == b.fontName) &&
    (attribs == b.attribs) &&
    (pointSize == b.pointSize) &&
    (stretch == b.stretch)
  );
}

iFont& iFont::operator=(const iFont& val) {
  copy(val);
  return *this;
}

void iFont::setBold(bool value) {
  if (value) attribs = (Attributes)((int)attribs | (int)Bold);
  else       attribs = (Attributes)((int)attribs & ~(int)Bold);
}

void iFont::setItalic(bool value) {
  if (value) attribs = (Attributes)((int)attribs | (int)Italic);
  else       attribs = (Attributes)((int)attribs & ~(int)Italic);
}

void iFont::setUnderline(bool value) {
  if (value) attribs = (Attributes)((int)attribs | (int)Underline);
  else       attribs = (Attributes)((int)attribs & ~(int)Underline);
}

void iFont::setPointSize(int value) {
  pointSize = (float)value;
}

void iFont::setPointSize(float value) {
  pointSize = value;
}

void iFont::setStretch(int value){
  stretch = value;
}


#ifdef TA_GUI

iFont::iFont(const char* raw_name) {
  init();
  setRawName(raw_name);
}

iFont::iFont(const QFont& val) {
  copyFrom(val);
}

iFont& iFont::operator=(const QFont& val) {
  copyFrom(val);
  return *this;
}

void iFont::copyFrom(const QFont& src) {
  fontName = src.family();
  setBold(src.bold());
  setItalic(src.italic());
  setUnderline(src.underline());
  stretch = src.stretch();
  pointSize = src.pointSizeF();
}

void iFont::copyTo(QFont& dst) const {
  dst.setFamily(fontName);
  dst.setPointSizeF(pointSize);
  dst.setBold(bold());
  dst.setItalic(italic());
  dst.setUnderline(underline());
  dst.setStretch(stretch);
}

void iFont::setRawName(const char* raw_name) {
  //TODO: this concept needs to be tested -- may not be fully Iv/X compatible
  QFont font;
  font.setRawName(QString(raw_name));
  this->copyFrom(font);
}

iFont::operator QFont() const {
  QFont rval(fontName, (int)pointSize, bold() ? QFont::Bold : QFont::Normal, italic());
  if (underline())
    rval.setUnderline(true);
  rval.setStretch(stretch);
  return rval;
}

#endif


#ifdef TA_USE_INVENTOR
void iFont::copyTo(SoFont* dst, float pts_per_unit) const {
  if (!dst) return;
  if (pts_per_unit == 0.0f) pts_per_unit = 36.0f; // bug in caller
  String nm = fontName;
  if (italic()) {
    nm += ":italic";
  }
  if (bold()) {
    nm += ":bold";
  }
  if (underline()) {
    nm += ":underline";
  }
  dst->name.setValue(nm.chars());
  dst->size = pointSize / pts_per_unit;
}
#endif

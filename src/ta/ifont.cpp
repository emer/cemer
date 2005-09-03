/* -*- C++ -*- */
/*=============================================================================
//									      //
// This file is part of the PDP++ software package.			      //
//									      //
// Copyright (C) 1995 Randall C. O'Reilly, Chadley K. Dawson, 		      //
//		      James L. McClelland, and Carnegie Mellon University     //
//     									      //
// Permission to use, copy, and modify this software and its documentation    //
// for any purpose other than distribution-for-profit is hereby granted	      //
// without fee, provided that the above copyright notice and this permission  //
// notice appear in all copies of the software and related documentation.     //
//									      //
// Permission to distribute the software or modified or extended versions     //
// thereof on a not-for-profit basis is explicitly granted, under the above   //
// conditions. 	HOWEVER, THE RIGHT TO DISTRIBUTE THE SOFTWARE OR MODIFIED OR  //
// EXTENDED VERSIONS THEREOF FOR PROFIT IS *NOT* GRANTED EXCEPT BY PRIOR      //
// ARRANGEMENT AND WRITTEN CONSENT OF THE COPYRIGHT HOLDERS.                  //
// 									      //
// Note that the taString class, which is derived from the GNU String class,  //
// is Copyright (C) 1988 Free Software Foundation, written by Doug Lea, and   //
// is covered by the GNU General Public License, see ta_string.h.             //
// The iv_graphic library and some iv_misc classes were derived from the      //
// InterViews morpher example and other InterViews code, which is             //
// Copyright (C) 1987, 1988, 1989, 1990, 1991 Stanford University             //
// Copyright (C) 1991 Silicon Graphics, Inc.				      //
//									      //
// THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,         //
// EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY 	      //
// WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  	      //
// 									      //
// IN NO EVENT SHALL CARNEGIE MELLON UNIVERSITY BE LIABLE FOR ANY SPECIAL,    //
// INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND, OR ANY DAMAGES  //
// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER OR NOT     //
// ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF LIABILITY,      //
// ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS        //
// SOFTWARE. 								      //
==============================================================================*/

#include "ifont.h"

#include <string.h>

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
  fontName = defName;
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
  pointSize = src.pointSizeFloat();
}

void iFont::copyTo(QFont& dst) const {
  dst.setFamily(fontName);
  dst.setPointSizeFloat(pointSize);
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

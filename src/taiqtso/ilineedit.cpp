// Copyright (C) 1995-2005 Regents of the University of Colorado,
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
/* -*- C++ -*- */
/*=============================================================================
//									      //
// This file is part of the TypeAccess/C-Super-Script software package.	      //
//									      //
// Copyright (C) 1995 Randall C. O'Reilly, Chadley K. Dawson, 		      //
//		      James L. McClelland, and Carnegie Mellon University     //
//     									      //
// Permission to use, copy, modify, and distribute this software and its      //
// documentation for any purpose is hereby granted without fee, provided that //
// the above copyright notice and this permission notice appear in all copies //
// of the software and related documentation.                                 //
// 									      //
// Note that the PDP++ software package, which contains this package, has a   //
// more restrictive copyright, which applies only to the PDP++-specific       //
// portions of the software, which are labeled as such.			      //
//									      //
// Note that the taString class, which is derived from the GNU String class,  //
// is Copyright (C) 1988 Free Software Foundation, written by Doug Lea, and   //
// is covered by the GNU General Public License, see ta_string.h.             //
// The iv_graphic library and some iv_misc classes were derived from the      //
// InterViews morpher example and other InterViews code, which is             //
// Copyright (C) 1987, 1988, 1989, 1990, 1991 Stanford University             //
// Copyright (C) 1991 Silicon Graphics, Inc.				      //
//									      //
// THE SOFTWARE IS PROVIDED <AS-IS" AND WITHOUT WARRANTY OF ANY KIND,         //
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

#include "ilineedit.h"

#include <qapplication.h>


iLineEdit::iLineEdit(QWidget* parent)
: QLineEdit(parent)
{
  init();
}

iLineEdit::iLineEdit(const char* text, QWidget* parent)
: QLineEdit(QString(text), parent)
{
  init();
}

void iLineEdit::focusInEvent(QFocusEvent* ev) {
  inherited::focusInEvent(ev);
  emit focusChanged(true);
}

void iLineEdit::focusOutEvent(QFocusEvent* ev) {
  inherited::focusOutEvent(ev);
  emit focusChanged(false);
}

void iLineEdit::init() {
  mhilight = false;
}

void iLineEdit::setHilight(bool value){
  if (mhilight == value) return;
  if (isReadOnly() && value) return; // can't set hilight when ro
  mhilight = value;
  //NOTE: following doesn't really work, and is just temporary until a proper solution is found
  QColor bg;
  if (value) {
    bg = COLOR_HILIGHT;
  } else {
    bg = (QApplication::palette().active().base()); // current text widget background
  }
  setBackgroundColor(bg);
  update();
}

void iLineEdit::setReadOnly(bool value) {
  if (isReadOnly() == value) return;
  QLineEdit::setReadOnly(value);
  if (value) {
    mhilight = false;
//    setFocusPolicy(ClickFocus);
    setPaletteBackgroundColor(QApplication::palette().active().button());
  } else {
//    setFocusPolicy(StrongFocus);
    setPaletteBackgroundColor(QApplication::palette().active().base());
  }
}


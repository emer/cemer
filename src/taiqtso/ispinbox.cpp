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

#include "ispinbox.h"

#include "qlineedit.h"

#include <qapplication.h>
#include <qevent.h>
#include <qrangecontrol.h>

iSpinBox::iSpinBox(QWidget* parent)
: QSpinBox(parent)
{
  init();
}

void iSpinBox::focusInEvent(QFocusEvent* ev) {
  inherited::focusInEvent(ev);
  emit focusChanged(true);
}

void iSpinBox::focusOutEvent(QFocusEvent* ev) {
  inherited::focusOutEvent(ev);
  emit focusChanged(false);
}

void iSpinBox::init() {
  mhilight = false;
  mreadOnly = false;
  updating = 0;
  connect(editor(), SIGNAL(selectionChanged()), this, SIGNAL(selectionChanged()) );

}

bool iSpinBox::hasSelectedText() {
  return editor()->hasSelectedText();
}

void iSpinBox::cut() {
  editor()->cut();
}

void iSpinBox::copy() {
  editor()->copy();
}

void iSpinBox::paste() {
  editor()->paste();
}

void iSpinBox::del() {
  editor()->del();
}

void iSpinBox::setHilight(bool value){
  if (mhilight == value) return;
  if (mreadOnly && value) return;  //hilighting ignored if read-only
  mhilight = value;
  if (value) {
    setPaletteBackgroundColor(COLOR_HILIGHT);
  } else {
    setPaletteBackgroundColor(QApplication::palette().active().base());
  }
  update();
}

void iSpinBox::setReadOnly(bool value) {
  if (mreadOnly == value) return;
  editor()->setReadOnly(value);
  if (value) {
    mhilight = false;
    setFocusPolicy(ClickFocus);
    setPaletteBackgroundColor(QApplication::palette().active().button());
  } else {
    setFocusPolicy(StrongFocus);
    setPaletteBackgroundColor(QApplication::palette().active().base());
  }
  // note: we have to dynamically disable the up/down controls in the paint event, because of how qspinbox is implemented
  mreadOnly = value;
}

void iSpinBox::setPaletteBackgroundColor(const QColor& color) {
   // pushes through to lineEdit
  QSpinBox::setPaletteBackgroundColor(color);
  editor()->setBackgroundColor(color);
}

void iSpinBox::stepUp() {
   if (mreadOnly) return;
   QSpinBox::stepUp();
}

void iSpinBox::stepDown(){
   if (mreadOnly) return;
   QSpinBox::stepDown();
}

void iSpinBox::updateDisplay() {
  // this is a huge implementation-dependent hack to disable the up/down for readOnly
  // if this stops working, it is not critical (controls will be enabled, but won't do anything)
  if (mreadOnly) {
    if (updating > 0) return; // needed because our actions cause reentrance
    ++updating;
    // following tricks updateDisplay() into disabling both up/down levers
    int old_max = maxValue();
    int old_min = minValue();
    bool old_wrap = wrapping();
    setMaxValue(value());
    setMinValue(value());
    setWrapping(false);
    QSpinBox::updateDisplay();
    // restore saved values
    setRange(old_min, old_max);
    setWrapping(old_wrap);
    --updating;
  } else {
    QSpinBox::updateDisplay();
  }
}

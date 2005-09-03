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

#include "icheckbox.h"
#include <qcheckbox.h>
#ifndef QT_NO_CHECKBOX
#include <qpalette.h>
#include <qbrush.h>
#include <qpainter.h>
#include <qdrawutil.h>
#include <qpixmap.h>
#include <qpixmapcache.h>
#include <qbitmap.h>
#include <qtextstream.h>
#include <qapplication.h>
#include <qstyle.h>


iCheckBox::iCheckBox(QWidget* parent)
: QCheckBox(parent)
{
  init();
}

iCheckBox::iCheckBox(const char* text, QWidget* parent)
: QCheckBox(QString(text), parent)
{
  init();
}

void iCheckBox::init() {
  mhilight = false;
  mreadOnly = false;
}

bool iCheckBox::isReadOnly() {
  return mreadOnly;
}

void iCheckBox::setHilight(bool value){
  if (mhilight == value) return;
  if (isReadOnly() && value) return; // can't set hilight when ro
  mhilight = value;
  update();
}

void iCheckBox::setReadOnly(bool value) {
  if (mreadOnly == value) return;
  mreadOnly = value;
  if (value) {
    mhilight = false;
  } else {
    //nothing
  }
  setEnabled(!value); // temp
}

// NOTE: this is the Trolltech routine from QCheckbox, with the Hilight color setting added in the middle

void iCheckBox::drawButton(QPainter* p)
{
  QCheckBox::drawButton(p);
  QRect irect = style().subRect(QStyle::SR_CheckBoxIndicator, this);
  if (mhilight) {
    p->setBrush(COLOR_BRIGHT_HILIGHT);
    p->setPen(COLOR_BRIGHT_HILIGHT);
    p->setRasterOp(Qt::CopyROP);
    QRect r;
    r.setRect(irect.x(), irect.y() - 2, irect.width() + 2, 2);
    p->drawRect(r);
    r.setRect(irect.x() + irect.width() + 1, irect.y(), 2, irect.height() + 2);
    p->drawRect(r);
    r.setRect(irect.x() - 2, irect.y() + irect.height(), irect.width() + 2, 2);
    p->drawRect(r);
    r.setRect(irect.x() - 2, irect.y() - 2, 2, irect.height() + 2);
    p->drawRect(r);
  } else {
    //nothing
  }
}

#endif

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

#include "icombobox.h"

#include <qcheckbox.h>

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


iComboBox::iComboBox(QWidget* parent)
: QComboBox(parent)
{
  defPalette = palette();
  init();
}

iComboBox::iComboBox(bool rw, QWidget* parent)
: QComboBox(rw, parent)
{
  defPalette = palette();
  init();
}

void iComboBox::init() {
  mhilight = false;
}

void iComboBox::setHilight(bool value){
  if (mhilight == value) return;
  mhilight = value;
  if (value) {
    QPalette pal = QPalette(defPalette);
    pal.setColor(QPalette::Active, QColorGroup::Button, COLOR_HILIGHT);
    pal.setColor(QPalette::Inactive, QColorGroup::Button, COLOR_HILIGHT);

    ((iComboBox*)this)->setPalette(pal);
  } else {
    ((iComboBox*)this)->setPalette(defPalette);
  }
  update();
}

/*void iComboBox::paintEvent(QPaintEvent* pe) { // override
  if (mhilight) {
    QPalette pal = QPalette(defPalette);
    pal.setColor(QPalette::Active, QColorGroup::Button, COLOR_HILIGHT);
    pal.setColor(QPalette::Inactive, QColorGroup::Button, COLOR_HILIGHT);

    ((iComboBox*)this)->setPalette(pal);
    QComboBox::paintEvent(pe);
    ((iComboBox*)this)->setPalette(defPalette);
  } else {
    QComboBox::paintEvent(pe);
  }
} */



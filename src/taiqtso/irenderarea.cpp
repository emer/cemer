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

#include "irenderarea.h"
#ifdef TA_USE_INVENTOR
#include <qapplication.h>

#include <Inventor/Qt/SoQt.h>
#include <Inventor/Qt/SoQtRenderArea.h>


iRenderAreaWrapper::iRenderAreaWrapper(SoQtRenderArea* ra, QWidget* parent)
:QWidget(parent)
{
  m_renderArea = ra;
}

iRenderAreaWrapper::iRenderAreaWrapper(QWidget* parent)
:QWidget(parent)
{
  m_renderArea = NULL;
}

iRenderAreaWrapper::~iRenderAreaWrapper() {
}


SoQtRenderArea* iRenderAreaWrapper::renderArea() {
  if (m_renderArea == NULL) {
      m_renderArea = new SoQtRenderArea(this);
  }
  return m_renderArea;
}

void iRenderAreaWrapper::resize(int w, int h) {
  QWidget::resize(w, h);
  if (m_renderArea) {
    m_renderArea->setSize(SbVec2s(w, h));
  }
}

void iRenderAreaWrapper::setRenderArea(SoQtRenderArea* value) {
  if (m_renderArea == value) return;
  if (value && (value->getParentWidget() != this)) {
      /*TODO: output following error message: "iRenderAreaWrapper::setRenderArea"
      "The RenderArea must be owned by ViewspaceWidget being assigned."); */
    return;
  }
  if (m_renderArea) {
    delete m_renderArea;
  }
  m_renderArea = value;
}

#endif // TA_USE_INVENTOR


// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/CSS
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


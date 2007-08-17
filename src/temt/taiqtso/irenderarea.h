// Copyright, 1995-2005, Regents of the University of Colorado,
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

// irenderarea.h -- Qt and SoRenderArea compatability classes

#ifndef IRENDERAREA_H
#define IRENDERAREA_H

#include "taiqtso_def.h"

#ifdef TA_USE_INVENTOR

#include <QWidget>

// externals
class SoQtRenderArea; //#IGNORE

class TAIQTSO_API iRenderAreaWrapper: public QWidget { // ##NO_CSS ##NO_INSTANCE ##NO_TOKENS
  Q_OBJECT
public:
  SoQtRenderArea* 	renderArea(); // autocreated if not assigned
  void			setRenderArea(SoQtRenderArea* value); // must have been created with us as parent!!

  iRenderAreaWrapper(SoQtRenderArea* ra, QWidget* parent = NULL);
  iRenderAreaWrapper(QWidget* parent = NULL);
  ~iRenderAreaWrapper();
protected:
  SoQtRenderArea* 	m_renderArea;
  void resizeEvent(QResizeEvent* ev); // override
  
};

#endif // TA_USE_INVENTOR
#endif // IRENDERAREA_H

// Copyright, 1995-2013, Regents of the University of Colorado,
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

#ifndef T3QuarterWidget_h
#define T3QuarterWidget_h 1

// parent includes:
#include "ta_def.h"
#ifndef __MAKETA__
#include <Quarter/Quarter.h>
#include <Quarter/QuarterWidget.h>
using SIM::Coin3D::Quarter::QuarterWidget;
#endif

// member includes:
#ifndef __MAKETA__
class QGLContext;
#endif

// declare all other types mentioned but not required to include:


class TA_API T3QuarterWidget : public QuarterWidget {
  // ##NO_INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS our custom version of the quarter widget
  Q_OBJECT
  INHERITED(QuarterWidget)
public:

#ifndef __MAKETA__
  explicit T3QuarterWidget(QWidget * parent = 0, const QGLWidget * sharewidget = 0, Qt::WindowFlags f = 0);
  explicit T3QuarterWidget(QGLContext * context, QWidget * parent = 0, const QGLWidget * sharewidget = 0, Qt::WindowFlags f = 0);
  explicit T3QuarterWidget(const QGLFormat & format, QWidget * parent = 0, const QGLWidget * shareWidget = 0, Qt::WindowFlags f = 0);
  ~T3QuarterWidget();
#endif

protected:
  override void paintEvent ( QPaintEvent * event );
};

#endif // T3QuarterWidget_h

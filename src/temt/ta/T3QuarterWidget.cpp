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

#include "T3QuarterWidget.h"

#include <T3ExaminerViewer>
#include <iT3ViewspaceWidget>
#include <T3Panel>
#include <iT3Panel>
#include <T3PanelViewer>
#include <iT3PanelViewer>



T3QuarterWidget::T3QuarterWidget(const QGLFormat & format, QWidget * parent, const QGLWidget * sharewidget, Qt::WindowFlags f)
  : inherited(format, parent, sharewidget, f)
{
}

T3QuarterWidget::T3QuarterWidget(QWidget * parent, const QGLWidget * sharewidget, Qt::WindowFlags f)
  : inherited(parent, sharewidget, f)
{
}

T3QuarterWidget::T3QuarterWidget(QGLContext * context, QWidget * parent, const QGLWidget * sharewidget, Qt::WindowFlags f)
  : inherited(context, parent, sharewidget, f)
{
}

T3QuarterWidget::~T3QuarterWidget() {
}

void T3QuarterWidget::paintEvent(QPaintEvent * event) {
  T3ExaminerViewer* t3v = (T3ExaminerViewer*)parent();
  if(!t3v) return;
  iT3ViewspaceWidget* vsw = t3v->t3vw;
  T3Panel* panl = t3v->GetPanel();
  if(!vsw || !panl) return;

  iT3PanelViewer* ipv = panl->widget()->viewerWidget();
  if(!ipv) return;
  T3PanelViewer* pv = ipv->viewer();
  if(!pv || !pv->isMapped()) return;

  inherited::paintEvent(event);
}


// Copyright, 1995-2007, Regents of the University of Colorado,
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

#include "iscrollarea.h"

#include <QScrollBar>
#include <QTimer>

iScrollArea::iScrollArea(QWidget* parent)
:inherited(parent)
{
  init();
}

void iScrollArea::init() {
}

QSize iScrollArea::sizeHint() const {
  QSize s(inherited::sizeHint());
  // if scrollbars are enabled but not shown, add their size anyway to avoid
  // the annoying slightly scrolled issue
  if (horizontalScrollBarPolicy() == Qt::ScrollBarAsNeeded) {
    QScrollBar* sb = horizontalScrollBar();
    if (!sb->isVisible())
      s.rheight() += sb->sizeHint().height() + 2;
  }
  if (verticalScrollBarPolicy() == Qt::ScrollBarAsNeeded) {
    QScrollBar* sb = verticalScrollBar();
    if (!sb->isVisible())
      s.rwidth() += sb->sizeHint().width() + 2;
  }
  return s;
}

////////////////////////////////////////
// splitter

iSplitter::iSplitter(QWidget *parent) : 
  QSplitter(parent)
{
  connect(this, SIGNAL(splitterMoved(int, int)), this, SLOT(saveSizes()));
}

iSplitter::iSplitter(Qt::Orientation orientation, QWidget *parent) : 
  QSplitter(orientation, parent)
{
}

iSplitter::~iSplitter(){
}

QSplitterHandle* iSplitter::createHandle() {
  iSplitterHandle* handle = new iSplitterHandle(orientation(), this);
  connect(handle, SIGNAL(handleDoubleClicked(int)), this, SLOT(collapseToggle(int)));
  return handle;
}

void iSplitter::showEvent(QShowEvent* event) {
  inherited::showEvent(event);
  QTimer::singleShot(150, this, SLOT(showDelayed()) );
}

void iSplitter::showDelayed() {
  saved_sizes = sizes();
}

void iSplitter::saveSizes() {
  if(in_collapsing) return;	// not now!
  saved_sizes = sizes();
}

void iSplitter::collapseToggle(int index) {
  if(index <= 0 || index >= count()) return;
  in_collapsing = true;
  QList<int> cur_sz = sizes();

  if(index == 1) index = 0;	// collapse to left
  
  if(cur_sz.count() != saved_sizes.count()) {
    saved_sizes = cur_sz;
  }
  if(cur_sz[index] == 0) {	// collapsed -- expand!
    cur_sz = saved_sizes;
  }
  else {
    cur_sz[index] = 0;
  }
  setSizes(cur_sz);
  in_collapsing = false;
}

iSplitterHandle::iSplitterHandle( Qt::Orientation orientation, QSplitter* parent)
  : QSplitterHandle(orientation, parent) {
}

iSplitterHandle::~iSplitterHandle() {
}

void iSplitterHandle::mouseDoubleClickEvent(QMouseEvent * event) {
  int index = splitter()->indexOf(this);
  if(index == -1) return;
  emit handleDoubleClicked(index);
  inherited::mouseDoubleClickEvent(event);
}

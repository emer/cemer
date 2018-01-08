  // Copyright 2013-2017, Regents of the University of Colorado,
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

#include "iTabBarBase.h"

#include <taiMisc>
#include <taMisc>

#include <QKeyEvent>
#include <QStylePainter>
#include <QStyleOptionTab>

iTabBarBase::iTabBarBase(QWidget* parent_) : inherited(parent_) {
  tab_was_selected = false;
  setUsesScrollButtons(false);
}

void iTabBarBase::selectNextTab() {
  int dx = 1;
  int index = currentIndex();
  if (currentIndex() == count() - 1) {  // wrap
    index = -1;
  }
  for (index = index + dx; index >= 0 && index < count();
       index += dx) {
    if (isTabEnabled(index)) {
      setCurrentIndex(index);
      break;
    }
  }
}

void iTabBarBase::selectPrevTab() {
  int dx = -1;
  int index = currentIndex();
  if (currentIndex() == 0) {  // wrap
    index = count();
  }
  for (index = index + dx; index >= 0 && index < count(); index += dx) {
    if (isTabEnabled(index)) {
      setCurrentIndex(index);
      break;
    }
  }
}

void iTabBarBase::keyPressEvent(QKeyEvent* key_event) {
  taiMisc::BoundAction action = taiMisc::GetActionFromKeyEvent(taiMisc::PROJECTWINDOW_CONTEXT, key_event);
  switch (action) {
    case taiMisc::PROJECTWINDOW_SHIFT_TAB_LEFT: // switch tab
    case taiMisc::PROJECTWINDOW_SHIFT_TAB_LEFT_II: // switch tab
      selectPrevTab();
      key_event->accept();
      return;
    case taiMisc::PROJECTWINDOW_SHIFT_TAB_RIGHT: // switch tab
    case taiMisc::PROJECTWINDOW_SHIFT_TAB_RIGHT_II: // switch tab
      selectNextTab();
      key_event->accept();
      return;
    default:
      inherited::keyPressEvent(key_event);
  }
}

void iTabBarBase::mouseReleaseEvent(QMouseEvent * event) {
  tab_was_selected = true;
  inherited::mouseReleaseEvent(event);
}


#if (QT_VERSION >= 0x050000)
QSize iTabBarBase::minimumTabSizeHint(int index) const {
  QSize sz = inherited::minimumTabSizeHint(index);
  sz.setWidth(sz.width() + taMisc::GetCurrentFontSize("labels")); // expand x so it doesn't go away entirely!
  return sz;
}
#endif

// void iTabBarBase::paintEvent(QPaintEvent *event)
// {
//   QStylePainter p(this);

//   bool resized=false;
//   int moveIt = 0;
//   int nrRows = 1 ;
//   tabRectangle.clear();

//   for (int i=0; i< count(); i++) {
//     QStyleOptionTab tab;
//     initStyleOption(&tab, i);

//     if (tab.rect.right() > width()*nrRows) {
//       if (!resized) {
//         setGeometry(0, 0, width(), tab.rect.height()*(nrRows+1));
//         resized=true;
//       }
//       tab.rect.moveTo(moveIt, tab.rect.height()*nrRows);
//       if (tab.rect.right() > width()) {
//         nrRows++;
//         moveIt=0;
//         setGeometry(0, 0, width(), tab.rect.height()*(nrRows+1));
//         tab.rect.moveTo(moveIt, tab.rect.height()*nrRows);
//       }
//       moveIt+=tab.rect.width();
//     }
//     p.drawControl(QStyle::CE_TabBarTab, tab);

//     tabRectangle.append(tab.rect);
//   }
// }

// void iTabBarBase::resizeEvent(QResizeEvent *event)
// {
//   QTabBar::resizeEvent(event);
//   update();
// }

// void iTabBarBase::mousePressEvent(QMouseEvent *event)
// {
//   if (event->button() != Qt::LeftButton) {
//     event->ignore();
//     return;
//   }

//   for (int i=0; i<count(); i++) {
//     if (tabRectangle[i].contains(event->pos())) {
//       setCurrentIndex(i);
//     }
//   }
// }




// Co2018ght 2013-2017, Regents of the University of Colorado,
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

#include "iComboBox.h"
#include <QWheelEvent>
#include <QStylePainter>

iComboBox::iComboBox(QWidget* parent)
  : QComboBox(parent)
{
  // Workaround for bug 1147: combo boxes for DynEnum's weren't wide enough
  // to show the full enum text.
  // The default SizeAdjustPolicy is AdjustToContentsOnFirstShow, which should
  // work as long as items are added to the combo box before it is shown, but
  // for some reason it seems this isn't the case.
  setSizeAdjustPolicy(QComboBox::AdjustToContents);
}

void iComboBox::wheelEvent(QWheelEvent * event) {
  // This override prevents the combo box value from being changed by the
  // mouse wheel -- it's too easy to accidentally change the value that way.

  // Call ignore() to allow the parent widget to process this event.
  event->ignore();
}

void iComboBox::updateSize() {
  if(sizeAdjustPolicy() == AdjustToContents) {
    setFixedWidth(sizeHint().width()-20); // shrink us -- computation is too big
  }
}

void iComboBox::showEvent(QShowEvent* e) {
  QComboBox::showEvent(e);
  updateSize();
}

void iComboBox::paintEvent(QPaintEvent *) {
  QStylePainter painter(this);
  painter.setPen(palette().color(QPalette::Text));

  // draw the combobox frame, focusrect and selected etc.
  QStyleOptionComboBox opt;
  initStyleOption(&opt);
  painter.drawComplexControl(QStyle::CC_ComboBox, opt);

  int idx = currentIndex();
  if(idx >= 0 && idx < item_colors.count()) {
    QString color = item_colors[idx];
    if(!color.isEmpty()) {
      painter.setPen(QColor(color));
    }
  }
    
  // draw the icon and text
  painter.drawControl(QStyle::CE_ComboBoxLabel, opt);
}

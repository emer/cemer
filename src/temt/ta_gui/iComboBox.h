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

#ifndef iComboBox_h
#define iComboBox_h 1

#include "ta_def.h"

#include <QObject>
#include <QComboBox>
#include <QStringList>

class TA_API iComboBox: public QComboBox {
  Q_OBJECT
public:
  QStringList        item_colors;

  void  addColor(const QString& color) { item_colors.append(color); }

  
  iComboBox(QWidget* parent = NULL);

public Q_SLOTS:
  virtual void  updateSize();
  // update the size based on contents -- auto called in showEvent, call manually after updating

protected:
  void wheelEvent(QWheelEvent * event) override;
  void showEvent(QShowEvent *e) override;
  void paintEvent(QPaintEvent* e) override;
};

#endif // iComboBox_h

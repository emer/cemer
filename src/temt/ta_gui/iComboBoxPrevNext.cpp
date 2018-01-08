// Copyright 2017-2017, Regents of the University of Colorado,
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

#include "iComboBoxPrevNext.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QToolButton>

#include <taiMisc>

iComboBoxPrevNext::iComboBoxPrevNext(QWidget* parent) : QWidget(parent) {
  lay = new QHBoxLayout(this);
  lay->setMargin(0); lay->setSpacing(1);

  vlay = new QVBoxLayout();
  vlay->setMargin(0); lay->setSpacing(0);
  lay->addLayout(vlay);

  int sz = taiM->combo_height(taiMisc::defSize) / 2;
  
  prev_button = new QToolButton(this);
  prev_button->setFixedWidth(sz);
  prev_button->setFixedHeight(sz);
  prev_button->setArrowType(Qt::UpArrow);
  connect(prev_button, SIGNAL(pressed()), this, SLOT(PrevItem()));
  vlay->addWidget(prev_button);

  next_button = new QToolButton(this);
  next_button->setFixedWidth(sz);
  next_button->setFixedHeight(sz);
  next_button->setArrowType(Qt::DownArrow);
  connect(next_button, SIGNAL(pressed()), this, SLOT(NextItem()));
  vlay->addWidget(next_button);

  combo_box = new iComboBox(this);
  // combo_box->setSizeAdjustPolicy(QComboBox::AdjustToContents);
  combo_box->setIconSize(QSize(1,1)); // no icon!
  lay->addWidget(combo_box);
  lay->addStretch(0);           // soak up any stretching AFTER this guy, not between

  connect(combo_box, SIGNAL(editTextChanged(const QString &)), this,
          SIGNAL(editTextChanged(const QString &)));
  connect(combo_box, SIGNAL(activated(int)), this, SIGNAL(activated(int)));
  connect(combo_box, SIGNAL(activated(const QString &)), this,
          SIGNAL(activated(const QString &)));
  connect(combo_box, SIGNAL(highlighted(int)), this, SIGNAL(highlighted(int)));
  connect(combo_box, SIGNAL(highlighted(const QString &)), this,
          SIGNAL(highlighted(const QString &)));
  connect(combo_box, SIGNAL(currentIndexChanged(int)), this,
          SIGNAL(currentIndexChanged(int)));
  connect(combo_box, SIGNAL(currentIndexChanged(const QString &)), this,
          SIGNAL(currentIndexChanged(const QString &)));
  connect(combo_box, SIGNAL(currentTextChanged(const QString &)), this,
          SIGNAL(currentTextChanged(const QString &)));

}

iComboBoxPrevNext::~iComboBoxPrevNext() {
}

void iComboBoxPrevNext::PrevItem() {
  int idx = combo_box->currentIndex();
  if(idx == 0) {
    idx = combo_box->count()-1;
  }
  else {
    idx--;
  }
  combo_box->setCurrentIndex(idx);
  emit activated(idx);          // in theory, this should emit already but it doesn't?
}

void iComboBoxPrevNext::NextItem() {
  int idx = combo_box->currentIndex();
  if(idx == count()-1) {
    idx = 0;
  }
  else {
    idx++;
  }
  combo_box->setCurrentIndex(idx);
  emit activated(idx);
}


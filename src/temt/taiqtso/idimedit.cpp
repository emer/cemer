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

#include "idimedit.h"

#include "ispinbox.h"

#include <QLabel>
#include <QHBoxLayout>

//////////////////////////////////
// 	iDimEdit		//
//////////////////////////////////

iDimEdit::iDimEdit(QWidget* parent)
:QWidget(parent)
{
  init();
}

iDimEdit::~iDimEdit()
{
}


void iDimEdit::init() {
  m_maxDims = 0; // updated later
  m_changing = 0;
  m_readOnly = false;
  m_dimsReadOnly = false;
  lay = new QHBoxLayout(this);
  lay->setMargin(0);
  lay->addSpacing(2);
  lblDims = new QLabel("dims:", this);
  lay->addWidget(lblDims);
  spnDims = new iSpinBox(this);
  spnDims->setMinimum(0);
  lay->addSpacing(2);
  lay->addWidget(spnDims);
  
  lay->addSpacing(2);
  wdgDimEdits = new QWidget(this);
  layDimEdits = new QHBoxLayout(wdgDimEdits);
  layDimEdits->setMargin(0);
  layDimEdits->setSpacing(2);
  lay->addWidget(wdgDimEdits);
  lay->addStretch();
  
  connect(spnDims, SIGNAL(valueChanged(int)), 
    this, SLOT(dims_valueChanged(int)) );
    
//  setMaxDims(8);
}


int iDimEdit::dim(int idx) const {
  if (!inRange(idx)) return -1; 
  iSpinBox* spn = (iSpinBox*)dimEdits.at(idx);
  return spn->value();
}

int iDimEdit::dims() const {
  // we use actual count, so access is inherently safe
  return dimEdits.count();
}

void iDimEdit::dim_valueChanged(int value) {
  if (m_changing > 0) return;
  emit changed(this);
}

void iDimEdit::dims_valueChanged(int value) {
  if (m_changing > 0) return;
  setDims(value, true);
  emit changed(this);
}

bool iDimEdit::inRange(int idx) const {
  return ((idx >= 0) && (idx < dims())); 
}

void iDimEdit::setDim(int idx, int value) {
  if (!inRange(idx) || (value < 0)) return; 
  ++m_changing;
  QSpinBox* spn = (QSpinBox*)dimEdits.at(idx);
  spn->setValue(value);
  --m_changing;
}

void iDimEdit::setDims(int value, bool force) {
  if (value > m_maxDims) value = m_maxDims;
  if (!force && ((dims() == value) || (value < 0))) return;
  ++m_changing;
  spnDims->setValue(value); // don't care if it triggers recursive call to us
  dimsUpdated();
  --m_changing;
}

void iDimEdit::setDimsReadOnly(bool value) {
  if (m_dimsReadOnly == value) return;
  spnDims->setReadOnly(value);
  m_dimsReadOnly = value;
}

void iDimEdit::dimsUpdated() {
  // called from both Dims and MaxDims, to get readonly set
  ++m_changing;
  for (int i = 0; i < m_maxDims; ++i) {
    QSpinBox* spn = (QSpinBox*)dimEdits.at(i);
    bool en = (i < dims());
    spn->setEnabled(en);
    if (i >= m_maxDims) {
      spn->setValue(0);
    }
     
  }
  --m_changing;
}

void iDimEdit::setMaxDims(int value) {
  if ((value < 0) || (m_maxDims == value)) return;
  ++m_changing;
  if (m_maxDims > value) {
    // delete controls
    for (int i = m_maxDims - 1; i >= value; --i) {
      QWidget* wdg = dimEdits.takeAt(i);
      disconnect(wdg, NULL, this, NULL);
      wdg->setParent(NULL);
      wdg->deleteLater();
      wdg = dimLabels.takeAt(i);
      wdg->setParent(NULL);
      wdg->deleteLater();
    }
    setDims(value);
    spnDims->setMaximum(value); // don't care if it triggers recursive call to us
  } else {
    spnDims->setMaximum(value); // don't care if it triggers recursive call to us
    // add controls
    for (int i = m_maxDims; i < value; ++i) {
      QLabel* lbl = new QLabel(QString::number(i), wdgDimEdits);
      layDimEdits->addWidget(lbl);
      dimLabels.append(lbl);
      lbl->show();
      iSpinBox* spn = new iSpinBox(wdgDimEdits);
      spn->setMinimum(0); 
      spn->setReadOnly(m_dimsReadOnly);
      layDimEdits->addWidget(spn);
      dimEdits.append(spn);
      spn->show();
      connect(spn, SIGNAL(valueChanged(int)), 
        this, SLOT(dim_valueChanged(int)) );
    }
  }
  m_maxDims = value;
  dimsUpdated();
  --m_changing;
}

void iDimEdit::setReadOnly(bool value) {
  if (m_readOnly == value) return;
  spnDims->setReadOnly(value || m_dimsReadOnly);
  for (int i = 0; i < dims(); ++i) {
    iSpinBox* spn = (iSpinBox*)dimEdits.at(i);
    spn->setReadOnly(value);
  }
  m_readOnly = value;
  setDimsReadOnly(value);
  
}

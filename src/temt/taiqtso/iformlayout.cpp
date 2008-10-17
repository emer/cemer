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

#include "iformlayout.h"

#if (QT_VERSION >= 0x040400)

template class QMap<int, QWidget*>; // instantiate
template class QMap<int, QLayout*>; // instantiate

/////////////////////////////////////////////////////
//  iFormLayout                                    //
/////////////////////////////////////////////////////

iFormLayout::iFormLayout(QWidget *parent)
:inherited(parent)
{
  init();
}

iFormLayout::~iFormLayout()
{

}

void iFormLayout::init() {
}

int iFormLayout::GetIndexFromRowRole(int row, ItemRole role) {
//ARGH! index seems to be random, not monotonic, so we need to exhaustively search

  int index = 0;
  int trow;
  ItemRole trole;
  getItemPosition(index, &trow, &trole);
  if (trow < 0) return -1; // not supposed to happen...
  while ((index >= 0) && ((row != trow) || (role != trole))) {
    if (trow < 0) return -1; // not supposed to happen...
    index++;
    getItemPosition(index, &trow, &trole);
  }
  return index;
  
/*  
  // make an educated guess, then iterate in until same
  int index = (row * 2) + role;
  int trow;
  ItemRole trole;
  getItemPosition(index, &trow, &trole);
  while ((index >= 0) && ((row != trow) || (role != trole))) {
    // following is not optimum, but is simple, and we must be close...
    if ((trow == -1) || (row < trow) || ((row == trow) && (role < trole)))
      index--;
    else index++;
    getItemPosition(index, &trow, &trole);
  }
  return index;
*/  
}

bool iFormLayout::isVisible(int row) {
  // default is visible, so we don't alloc unless needed
  if (row >= m_inv.count()) return true;
  else return !(m_inv.at(row));
}

void iFormLayout::setVisible(int row, bool val) {
  bool cur = isVisible(row);
  if (cur == val) return; // no change
  // set bit
  if (row >= m_inv.count())
    m_inv.resize(row + 1);
  m_inv.setBit(row, !val);
  
  QWidget* label = NULL;
  QLayout* field = NULL;
  if (val) { // going visible
    label = m_inv_labels[row];
    field = m_inv_fields[row];
    setWidget(row, LabelRole, label);
    setLayout(row, FieldRole, field);
  } else { // going invisible
    int index = GetIndexFromRowRole(row, LabelRole);
    QLayoutItem* it = (index >= 0) ? takeAt(index) : NULL;
    if (it) {
      label = it->widget();
      delete it;
    }
    index = GetIndexFromRowRole(row, FieldRole);
    it = (index >= 0) ? takeAt(index) : NULL;
    if (it) {
      field = it->layout();
      // unparent, otherwise complains when we re-add 
      if (field) {
        field->setParent(NULL);
      }
      // the LayoutItem IS the Layout! so don't delete it!
      //delete it;
    }
    m_inv_labels.insert(row, label);
    m_inv_fields.insert(row, field);
  }
}

#endif // Qt 4.4

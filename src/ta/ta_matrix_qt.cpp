// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/CSS
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


#include "ta_matrix_qt.h"

/** \class MatrixTableModel

  The MatrixTableModel provides a 2-d table model for TA Matrix objects.
  
  We can only map the inner 2-most of the N dimensions as follows:
    0: column
    1: row
    
  If there are more than 2 dimensions, then table cells will refer to a submatrix of the remaining
  dimensions.

*/

MatrixTableModel::MatrixTableModel(QObject* parent) 
:inherited(parent)
{
  m_matrix = NULL;
}

MatrixTableModel::~MatrixTableModel() {
  setMatrix(NULL);
}

int MatrixTableModel::columnCount(const QModelIndex& parent) const {
  if (!m_matrix) return 0;
  if (m_matrix->dims() < 1)
    return 0;
  else if (m_matrix->dims() == 1)
    return m_matrix->GetGeom(0);
  else return m_matrix->GetGeom(m_matrix->dims() - 2);
}

QVariant MatrixTableModel::data(const QModelIndex& index, int role) const {
  if (!m_matrix) return QVariant();
  //TEMP
  return "test";
}

Qt::ItemFlags MatrixTableModel::flags(const QModelIndex& index) const {
  if (!m_matrix) return 0;
  Qt::ItemFlags rval = 0;
  if (ValidateIndex(index)) {
    rval = Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    //only editable if 2-d or less
    if (m_matrix->dims() <= 2)  
      rval |= Qt::ItemIsEditable;
  }
  return rval;
}

QVariant MatrixTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
  if (role != Qt::DisplayRole)
    return QVariant();
//TODO: make the headers show the dimenion # when > 2
  if (orientation == Qt::Horizontal)
    return QString("C%1").arg(section);
  else
    return QString("R%1").arg(section);
}

int MatrixTableModel::rowCount(const QModelIndex& parent) const {
  if (!m_matrix) return 0;
  if (m_matrix->dims() < 1)
    return 0;
  else if (m_matrix->dims() == 1)
    return 1;
  else return m_matrix->GetGeom(m_matrix->dims() - 1);
}

bool MatrixTableModel::setData(const QModelIndex& index, const QVariant & value, int role) {
  if (!m_matrix) return false;
  if (index.isValid() && role == Qt::EditRole) {
  //TODO:

//ex      stringList.replace(index.row(), value.toString());
      emit dataChanged(index, index);
      return true;
  }
  return false;
}


void MatrixTableModel::setMatrix(taMatrix* value) {
  if (m_matrix == value) return;
  taBase::SetPointer((TAPtr*)&m_matrix, value);
  //TODO: update things
}

bool MatrixTableModel::ValidateIndex(const QModelIndex& index) const {
  // TODO:
  return false;
}

bool MatrixTableModel::ValidateTranslateIndex(const QModelIndex& index, MatrixGeom& tr_index) const {
  bool rval = ValidateIndex(index);
  if (rval) {
    // TODO:
  }
  return rval;
}

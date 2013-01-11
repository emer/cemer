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

#include "DataTableModel.h"


DataTableModel::DataTableModel(DataTable* dt_)
:inherited(NULL)
{
  m_dt = dt_;
}

DataTableModel::~DataTableModel() {
  // note: following shouldn't really execute since table manages our lifetime
  if (m_dt) {
    m_dt->RemoveDataClient(this);
    m_dt = NULL;
  }
}

int DataTableModel::columnCount(const QModelIndex& parent) const {
  return (m_dt) ? m_dt->cols() : 0;
}

void DataTableModel::DataLinkDestroying(taDataLink* dl) {
  m_dt = NULL;
}

void DataTableModel::DataDataChanged(taDataLink* dl, int dcr,
  void* op1, void* op2)
{ // called from DataTable::DataChanged
  if (notifying) return;
  //this is primarily for code-driven changes
  if ((dcr <= DCR_ITEM_UPDATED_ND) || // data itself updated
    (dcr == DCR_STRUCT_UPDATE_END) ||  // for col insert/deletes
    (dcr == DCR_DATA_UPDATE_END)) // for row insert/deletes
  {
    emit_layoutChanged();
  }
}

QVariant DataTableModel::data(const QModelIndex& index, int role) const {
  if (!m_dt || !index.isValid()) return QVariant();
  //NOTES:
  // * it would be nice to just italicize the "matrix" text, but we have no
  //   no access to the font being used, and cannot only pass modifiers

  DataCol* col = m_dt->GetColData(index.column(), true); // quiet
  // if no col, we really don't care about anything else...
  if (!col) return QVariant(); // nil

  switch (role) {
  case Qt::DisplayRole: //note: we may choose to format different for display, ex floats
  case Qt::EditRole: {
    if (col->is_matrix)
      return QVariant("(matrix)"); // user clicks to edit, or elsewise displayed
    else {
      int dx;
      if(m_dt->idx(index.row(), col->rows(), dx))
        return col->GetValAsString(dx);
      else
        return QVariant();      // nil
    }
  }
// Qt::FontRole: //  QFont: font for the text
//Qt::DecorationRole
//Qt::ToolTipRole
//Qt::StatusTipRole
//Qt::WhatsThisRole
//Qt::SizeHintRole -- QSize
//Qt::FontRole--  QFont: font for the text
  case Qt::TextAlignmentRole: {
    if (col->is_matrix)
      return QVariant(Qt::AlignCenter | Qt::AlignVCenter);
    else if (col->isNumeric())
      return QVariant(Qt::AlignRight | Qt::AlignVCenter);
    else
      return QVariant(Qt::AlignLeft | Qt::AlignVCenter);
    } break;
  case Qt::BackgroundColorRole : //-- QColor
 /* note: only used when !(option.showDecorationSelected && (option.state
    & QStyle::State_Selected)) */
    // note: only make it actual ro color if ro (not for "(matrix)" cells)
    if ((col->col_flags & DataCol::READ_ONLY) || col->isGuiReadOnly())
      return QColor(COLOR_RO_BACKGROUND);
    break;
  case Qt::TextColorRole: { // QColor: color of text
    if (col->is_matrix)
      return QColor(Qt::blue);
    } break;
//Qt::CheckStateRole
  default: break;
  }
  return QVariant();
}

void DataTableModel::emit_dataChanged(int row_fr, int col_fr, int row_to, int col_to) {
  if (!m_dt) return;
  // lookup actual end values when we are called with sentinels
  if (row_to < 0) row_to = rowCount() - 1;
  if (col_to < 0) col_to = columnCount() - 1;

  emit dataChanged(createIndex(row_fr, col_fr), createIndex(row_to, col_to));
}

void DataTableModel::emit_dataChanged(const QModelIndex& topLeft,
    const QModelIndex& bottomRight)
{
  if (!m_dt) return;
  ++notifying;
  emit dataChanged(topLeft, bottomRight);
  --notifying;
}

void DataTableModel::emit_layoutChanged() {
  if (!m_dt) return;
  ++notifying;
  emit layoutChanged();
  --notifying;
}

Qt::ItemFlags DataTableModel::flags(const QModelIndex& index) const {
  if (!m_dt || !index.isValid()) return 0;
  Qt::ItemFlags rval = 0;
  if (ValidateIndex(index)) {
    // don't enable null cells
    if (m_dt->hasData(index.column(), index.row() )) {
      rval = Qt::ItemIsSelectable | Qt::ItemIsEnabled;
      DataCol* col = m_dt->GetColData(index.column(), true); // quiet
      if (col && !(col->is_matrix || (col->col_flags & DataCol::READ_ONLY) ||
         col->isGuiReadOnly()) )
        rval |= Qt::ItemIsEditable;
    }
  }
  return rval;
}

QVariant DataTableModel::headerData(int section, Qt::Orientation orientation,
  int role) const
{
  if (m_dt) {
    if (orientation == Qt::Horizontal) {
      DataCol* col = m_dt->GetColData(section, true); // quiet
      if (col) {
        if (role == Qt::DisplayRole)
          return QString(col->GetDisplayName().chars());
        else if (role == Qt::ToolTipRole)
          return QString(col->GetDesc().chars());
      } else
        return QString();
    } else {
      if (role == Qt::DisplayRole)
        return QString::number(section);
    }
  }
  return QVariant();
}

void DataTableModel::refreshViews() {
  emit_layoutChanged();
/*  emit dataChanged(createIndex(0, 0),
    createIndex(rowCount() - 1, columnCount() - 1));*/
}

int DataTableModel::rowCount(const QModelIndex& parent) const {
  return (m_dt) ? m_dt->rows : 0;
}

void DataTableModel::matDataChanged(int col_idx) {
  if (!m_dt) return;

  DataCol* col = m_dt->GetColData(col_idx, true); // quiet
  // if no col, we really don't care about anything else...
  if (!col) return;
  ++notifying;
  col->DataChanged(DCR_ITEM_UPDATED); // for calc refresh and other clients
  --notifying;
}

bool DataTableModel::setData(const QModelIndex& index, const QVariant & value, int role) {
  if (!m_dt || !index.isValid()) return false;

  DataCol* col = m_dt->GetColData(index.column(), true); // quiet
  // if no col, we really don't care about anything else...
  if (!col) return false;
  //we restrict setData for scalars only -- use delegate for matrix
  if (col->is_matrix) return false;

  bool rval = false;
  switch (role) {
  case Qt::EditRole: {
    taProject* proj = (taProject*)m_dt->GetOwner(&TA_taProject);
    // save undo state!
    if(proj) {
      proj->undo_mgr.SaveUndo(col, "DataTableEdit", col);
    }
    m_dt->SetValAsVar(value, index.column(), index.row());
    ++notifying;
    emit_dataChanged(index, index);
    col->DataChanged(DCR_ITEM_UPDATED); // for calc refresh
    --notifying;
    rval = true;
  }
  default: break;
  }
  return rval;
}

bool DataTableModel::ValidateIndex(const QModelIndex& index) const {
  if (!m_dt) return false;
  return (index.isValid() && (index.row() < m_dt->rows) && (index.column() < m_dt->cols()));
}


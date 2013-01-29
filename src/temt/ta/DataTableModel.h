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

#ifndef DataTableModel_h
#define DataTableModel_h 1

// parent includes:
#ifndef __MAKETA__
#include <QAbstractTableModel>
#else
class QAbstractTableModel; // #IGNORE
#endif
#include <IDataLinkClient>

// member includes:
#include <ContextFlag>

// declare all other types mentioned but not required to include:
class TypeDef; // 
class taiDataLink; // 
class taDataLink; // 
class DataTable; // 
class QModelIndex; //


TypeDef_Of(DataTableModel);

class TA_API DataTableModel: public QAbstractTableModel,
  public IDataLinkClient {
// #NO_INSTANCE #NO_CSS class that implements the Qt Model interface for tables;\ncreated and owned by the DataTable
INHERITED(QAbstractTableModel)
friend class DataTableCols;
friend class DataTable;
  Q_OBJECT
public:
  DataTable*            dataTable() const {return m_dt;}

  void                  refreshViews(); // similar to matrix, issues dataChanged

  void                  emit_dataChanged(int row_fr = 0, int col_fr = 0,
    int row_to = -1, int col_to = -1);// can be called w/o params to issue global change (for manual refresh)
  void                  emit_dataChanged(const QModelIndex& topLeft,
    const QModelIndex& bottomRight);

public slots:
  void                  matDataChanged(int col_idx); // mat editor calls when data changes

protected:
  DataTableModel(DataTable* dt);
  ~DataTableModel(); //

public: // required implementations
#ifndef __MAKETA__
  int                   columnCount(const QModelIndex& parent = QModelIndex()) const; // override
  QVariant              data(const QModelIndex& index, int role = Qt::DisplayRole) const; // override
  Qt::ItemFlags         flags(const QModelIndex& index) const; // override, for editing
  QVariant              headerData(int section, Qt::Orientation orientation,
    int role = Qt::DisplayRole) const; // override
  int                   rowCount(const QModelIndex& parent = QModelIndex()) const; // override
  bool                  setData(const QModelIndex& index, const QVariant& value,
    int role = Qt::EditRole); // override, for editing

public: // IDataLinkClient i/f
  override void*        This() {return this;}
  override TypeDef*     GetTypeDef() const {return &TA_DataTableModel;}
//  override bool               ignoreDataChanged() const;
  override void         DataLinkDestroying(taDataLink* dl);
  override void         DataDataChanged(taDataLink* dl, int dcr, void* op1, void* op2);

protected:
  bool                  ValidateIndex(const QModelIndex& index) const;
#endif
  void                  emit_layoutChanged(); // we call this for most schema changes
//  void                        setDataTable(DataTable* value, bool notify = true);
protected:
  DataTable*            m_dt;
  ContextFlag           notifying; // to avoid responding when we sent notify
};

#endif // DataTableModel_h

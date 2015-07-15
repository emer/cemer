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

#ifndef iDataTableModel_h
#define iDataTableModel_h 1

// parent includes:
#ifndef __MAKETA__
#include <QAbstractTableModel>
#else
class QAbstractTableModel; // #IGNORE
#endif
#include <ISigLinkClient>

// member includes:
#include <ContextFlag>

// declare all other types mentioned but not required to include:
class TypeDef; // 
class taiSigLink; // 
class taSigLink; // 
class DataTable; // 
class QModelIndex; //



taTypeDef_Of(iDataTableModel);

class TA_API iDataTableModel: public QAbstractTableModel,
  public ISigLinkClient {
// #NO_INSTANCE #NO_CSS #NO_MEMBERS class that implements the Qt Model interface for tables;\ncreated and owned by the DataTable
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
                                         const QModelIndex& bottomRight); // #IGNORE
    
    void                AddToFoundList(int row, int col);
    // add the row/col pair of a data table to found_list - for non-matrix columns
    void                ClearFoundList();
    // remove all the previously found items from the list
    const QModelIndex*  GetNextFound();
    // #IGNORE get the row/col of the next item from the current search
    const QModelIndex*  GetPreviousFound();
    // #IGNORE get the row/col of the previous item from the current search
    
public slots:
  void                  matSigEmit(int col_idx); // mat editor calls when data changes

protected:
  iDataTableModel(DataTable* dt);
  ~iDataTableModel(); //
    
#ifndef __MAKETA__
  QList<QModelIndex>    items_found;  // matching items from last search
#endif
  int                   current_found;
  
  public: // required implementations
#ifndef __MAKETA__
  int                   columnCount(const QModelIndex& parent = QModelIndex()) const override; 
  QVariant              data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
  Qt::ItemFlags         flags(const QModelIndex& index) const override; // for editing
  QVariant              headerData(int section, Qt::Orientation orientation,
                                   int role = Qt::DisplayRole) const override;
  int                   rowCount(const QModelIndex& parent = QModelIndex()) const override;
  bool                  setData(const QModelIndex& index, const QVariant& value,
                                int role = Qt::EditRole) override; // for editing

public: // ISigLinkClient i/f
  void*        This() override {return this;}
  TypeDef*     GetTypeDef() const override {return &TA_iDataTableModel;}
//  bool               ignoreSigEmit() const override;
  void         SigLinkDestroying(taSigLink* dl) override;
  void         SigLinkRecv(taSigLink* dl, int sls, void* op1, void* op2) override;

protected:
  bool                  ValidateIndex(const QModelIndex& index) const;
#endif
  void                  emit_layoutChanged(); // we call this for most schema changes
//  void                        setDataTable(DataTable* value, bool notify = true);
protected:
  DataTable*            m_dt;
  ContextFlag           notifying; // to avoid responding when we sent notify
};

#endif // iDataTableModel_h

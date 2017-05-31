// Copyright 2017, Regents of the University of Colorado,
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

#ifndef iMatrixTableModel_h
#define iMatrixTableModel_h 1

// parent includes:
#include <ISigLinkClient>
#ifndef __MAKETA__
#include <QAbstractTableModel>
#endif

// member includes:
#include <ContextFlag>

// declare all other types mentioned but not required to include:
class TypeDef; // 
class taiSigLink; // 
class taSigLink; // 
class taMatrix; // 
class DataCol; // 
class String_Matrix; // 
class QModelIndex;

taTypeDef_Of(iMatrixTableModel);

class TA_API iMatrixTableModel: public QAbstractTableModel,
  public ISigLinkClient
{
  // #NO_INSTANCE #NO_CSS class that implements the Qt Model interface for matrices; we extend it to support N-d, but only 2-d cell display; if the model has a single gui client, then set it, to avoid doing refreshes when it isn't visible
friend class taMatrix;
INHERITED(QAbstractTableModel)
  Q_OBJECT
public:
  int                   col_idx; // when this is a DataTable mat cell, the view sets this, otherwise it is -1 -- used in cell updated signal to iDataTableModel
#ifndef __MAKETA__
  int                   matIndex(const QModelIndex& idx) const; // #IGNORE flat matrix data index
  QMimeData*   mimeData (const QModelIndexList& indexes) const override;
  QStringList  mimeTypes () const override;
  int                   matView() const; // taMisc::MatrixView
#endif //note: bugs in maketa necessitated these sections
  taMatrix*             mat() const {return m_mat;}
  inline bool           pat4D() const {return m_pat_4d;} // for dims>=4 whether to group d0/d1 in row (default is true)
  void                  setPat4D(bool val, bool notify = true);
  void			setDataCol(DataCol* dc);

  String_Matrix*        dimNames()  { return m_dim_names; }
  void                  setDimNames(String_Matrix* dnms);
  // use given matrix to provide optional dimension names

  void                  emit_dataChanged(int row_fr = 0, int col_fr = 0,
    int row_to = -1, int col_to = -1);// can be called w/o params to issue global change (for manual refresh)
  void                  emit_dataChanged(const QModelIndex& topLeft,
                                         const QModelIndex& bottomRight); // #IGNORE 
  void                  emit_layoutChanged();

protected: // only from matrix
  iMatrixTableModel(taMatrix* mat_);
  ~iMatrixTableModel(); //

public: // required implementations
#ifndef __MAKETA__
  int          columnCount(const QModelIndex& parent = QModelIndex()) const override;
  QVariant     data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
  Qt::ItemFlags flags(const QModelIndex& index) const override;
  QVariant     headerData(int section, Qt::Orientation orientation,
                                   int role = Qt::DisplayRole) const override;
  int          rowCount(const QModelIndex& parent = QModelIndex()) const override;
  bool         setData(const QModelIndex& index, const QVariant& value,
                                int role = Qt::EditRole) override;
signals:
  void                  matSigEmit(int col_idx); // only emited during dataChanged if col_idx valid

public: // ISigLinkClient i/f
  void*        This() override {return this;}
  TypeDef*     GetTypeDef() const override {return &TA_iMatrixTableModel;}
//  bool               ignoreSigEmit() const override;
  void         SigLinkDestroying(taSigLink* dl) override;
  void         SigLinkRecv(taSigLink* dl, int sls, void* op1, void* op2) override;

protected:
  taMatrix*             m_mat;
  DataCol*		m_mat_col; // in case this guy is a delegate for a data column
  String_Matrix*        m_dim_names;
  int                   m_view_layout; //#IGNORE taMisc::MatrixView #DEF_TOP_ZERO
  ContextFlag           notifying; // to avoid responding when we sent notify
  bool                  m_pat_4d;

  bool                  ValidateIndex(const QModelIndex& index) const;
#endif
};

#endif // iMatrixTableModel_h

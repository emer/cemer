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

#ifndef iDataTableView_h
#define iDataTableView_h 1

// parent includes:
#include <iTableView>

// member includes:

// declare all other types mentioned but not required to include:
class DataTable; // 


class TA_API iDataTableView: public iTableView {
  // widget with some customizations to display submatrix views
  INHERITED(iTableView)
  Q_OBJECT
friend class iDataTableEditor;
public:
  bool                  gui_edit_op; // true if doing a gui editing operation

  DataTable*            dataTable() const;

  override bool         isFixedRowCount() const {return false;}
  override bool         isFixedColCount() const {return false;}

  iDataTableView(QWidget* parent = NULL);

public: // cliphandler i/f
  override void         EditAction(int ea);
  override void         GetEditActionsEnabled(int& ea);

#ifndef __MAKETA__
signals:
  void                  sig_currentChanged(const QModelIndex& current);
  void                  sig_dataChanged(const QModelIndex& topLeft,
    const QModelIndex & bottomRight); // #IGNORE
#endif

protected:
  override void         currentChanged(const QModelIndex& current,
                                       const QModelIndex& previous);
#if (QT_VERSION >= 0x050000)
  override void         dataChanged(const QModelIndex& topLeft,
				    const QModelIndex & bottomRight, const QVector<int> &roles = QVector<int>());
#else
  override void         dataChanged(const QModelIndex& topLeft,
                                    const QModelIndex & bottomRight);
#endif
  // refresh mat cell if in here
  override void         FillContextMenu_impl(ContextArea ca, taiMenu* menu,
                                             const CellRange& sel);
  override void         RowColOp_impl(int op_code, const CellRange& sel);
};

#endif // iDataTableView_h

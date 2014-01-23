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
class iDataTableColHeaderView; //
class iDataTableRowHeaderView; //


class TA_API iDataTableView: public iTableView {
  // widget with some customizations to display submatrix views
  INHERITED(iTableView)
      Q_OBJECT
      friend class iDataTableEditor;
public:
  bool                  gui_edit_op; // true if doing a gui editing operation

  iDataTableColHeaderView* col_header;
  iDataTableRowHeaderView* row_header;

  DataTable*            dataTable() const;

  bool         isFixedRowCount() const CPP11_OVERRIDE {return false;}
  bool         isFixedColCount() const CPP11_OVERRIDE {return false;}

  iDataTableView(QWidget* parent = NULL);

public: // cliphandler i/f
  void         EditAction(int ea) CPP11_OVERRIDE;
  void         GetEditActionsEnabled(int& ea) CPP11_OVERRIDE;

  void         ViewAction(int ea) CPP11_OVERRIDE;

#ifndef __MAKETA__
  signals:
  void                  sig_currentChanged(const QModelIndex& current);
  void                  sig_dataChanged(const QModelIndex& topLeft,
      const QModelIndex & bottomRight); // #IGNORE
#endif

protected:
  void         currentChanged(const QModelIndex& current,
      const QModelIndex& previous) CPP11_OVERRIDE;
#if (QT_VERSION >= 0x050000)
  void         dataChanged(const QModelIndex& topLeft,
      const QModelIndex & bottomRight, const QVector<int> &roles = QVector<int>()) CPP11_OVERRIDE;
#else
  void         dataChanged(const CPP11_OVERRIDE QModelIndex& topLeft,
      const QModelIndex & bottomRight);
#endif
// refresh mat cell if in here
  void         FillContextMenu_impl(ContextArea ca, taiWidgetMenu* menu,
      const CellRange& sel) CPP11_OVERRIDE;
  void         RowColOp_impl(int op_code, const CellRange& sel) CPP11_OVERRIDE;

};

#endif // iDataTableView_h

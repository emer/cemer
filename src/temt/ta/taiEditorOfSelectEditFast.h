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

#ifndef taiEditorOfSelectEditFast_h
#define taiEditorOfSelectEditFast_h 1

// parent includes:
#include <taiEditorOfSelectEditBase>

// member includes:

// declare all other types mentioned but not required to include:
class iTableWidget;
class taiWidgetDelegateSelectEdit;


class TA_API taiEditorOfSelectEditFast : public taiEditorOfSelectEditBase {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS alternative, using viewer
INHERITED(taiEditorOfSelectEditBase)
  Q_OBJECT
public:
  iTableWidget*   tw;

  override void         Constr_Box();
  override void         GetImage_Item(int row); // called from GetImage and ed->GetValue

  taiEditorOfSelectEditFast(void* base, TypeDef* td, bool read_only_ = false,
        QObject* parent = 0);
  taiEditorOfSelectEditFast()        { Initialize();};
  ~taiEditorOfSelectEditFast();

protected slots:
  override void         DoRemoveSelEdit();
  void                  tw_currentCellChanged( int currentRow,
    int currentColumn, int previousRow, int previousColumn);
  void                  tw_customContextMenuRequested(const QPoint& pos);

protected:
  taiWidgetDelegateSelectEdit*   sed;

  override void         Constr_Body_impl();
  override void         ClearBody_impl();       // we also clear all the methods, and then rebuild them

  override void         Constr_Data_Labels();
  override void         FillLabelContextMenu_SelEdit(QMenu* menu, int& last_id);
  override void         GetImage_Membs_def();
  override void         GetValue_Membs_def();

private:
  void  Initialize();
};

#endif // taiEditorOfSelectEditFast_h

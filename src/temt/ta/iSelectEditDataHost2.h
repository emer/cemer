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

#ifndef iSelectEditDataHost2_h
#define iSelectEditDataHost2_h 1

// parent includes:
#include <iSelectEditDataHostBase>

// member includes:

// declare all other types mentioned but not required to include:
class taiEditTableWidget;
class SelectEditDelegate;


class TA_API iSelectEditDataHost2 : public iSelectEditDataHostBase {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS alternative, using viewer
INHERITED(iSelectEditDataHostBase)
  Q_OBJECT
public:
  taiEditTableWidget*   tw;

  override void         Constr_Box();
  override void         GetImage_Item(int row); // called from GetImage and ed->GetValue

  iSelectEditDataHost2(void* base, TypeDef* td, bool read_only_ = false,
        QObject* parent = 0);
  iSelectEditDataHost2()        { Initialize();};
  ~iSelectEditDataHost2();

protected slots:
  override void         DoRemoveSelEdit();
  void                  tw_currentCellChanged( int currentRow,
    int currentColumn, int previousRow, int previousColumn);
  void                  tw_customContextMenuRequested(const QPoint& pos);

protected:
  SelectEditDelegate*   sed;

  override void         Constr_Body_impl();
  override void         ClearBody_impl();       // we also clear all the methods, and then rebuild them

  override void         Constr_Data_Labels();
  override void         FillLabelContextMenu_SelEdit(QMenu* menu, int& last_id);
  override void         GetImage_Membs_def();
  override void         GetValue_Membs_def();

private:
  void  Initialize();
};

#endif // iSelectEditDataHost2_h

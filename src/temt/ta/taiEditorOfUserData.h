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

#ifndef taiEditorOfUserData_h
#define taiEditorOfUserData_h 1

// parent includes:
#include <taiEditorOfClass>

// member includes:

// declare all other types mentioned but not required to include:
class taiWidgetDelegateUserData; //
class QTableWidget; //
class QTableWidgetItem; //


class TA_API taiEditorOfUserData : public taiEditorOfClass {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS edit only selected items from a range of ta-base objects
INHERITED(taiEditorOfClass)
  Q_OBJECT
public:
  UserDataItem_List*    udil;
  QTableWidget*         tw;

  void         Constr_Box() override;
//obs  taBase* GetMembBase_Flat(int idx) override; // these are overridden by userdata
//obs  taBase* GetMethBase_Flat(int idx) override;
  void         GetImage_Item(int row) override; // called from GetImage and ed->GetValue

  taiEditorOfUserData(void* base, TypeDef* td, bool read_only_ = false,
        QObject* parent = 0);
  taiEditorOfUserData()   { Initialize();};
  ~taiEditorOfUserData();

public: // ISigLinkClient i/f
  void         SigLinkRecv(taSigLink* dl, int sls, void* op1, void* op2) override;

protected:
  int                   sel_item_row;
  taiWidgetDelegateUserData*     udd;

  UserDataItemBase*     GetUserDataItem(int row);

  void         Constr_impl() override;
  void         Constr_Body_impl() override;
  void         ClearBody_impl() override;       // we also clear all the methods, and then rebuild them

  void         Constr_Widget_Labels() override;
  void         FillLabelContextMenu_SelEdit(QMenu* menu, int& last_id) override;
  void         GetImage_Membs_def() override;
  void         GetValue_Membs_def() override;
  void         Constr_Methods_impl() override;

protected slots:
  virtual void          DoDeleteUserDataItem();
  virtual void          DoRenameUserDataItem();
  void                  tw_currentCellChanged( int currentRow,
    int currentColumn, int previousRow, int previousColumn);
  void                  tw_customContextMenuRequested(const QPoint& pos);
  void                  tw_itemChanged(QTableWidgetItem* item);
private:
  void  Initialize();
};

#endif // taiEditorOfUserData_h

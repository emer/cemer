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

#ifndef iUserDataDataHost_h
#define iUserDataDataHost_h 1

// parent includes:
#include <taiEditDataHost>

// member includes:

// declare all other types mentioned but not required to include:
class UserDataDelegate; //


class TA_API iUserDataDataHost : public taiEditDataHost {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS edit only selected items from a range of ta-base objects
INHERITED(taiEditDataHost)
  Q_OBJECT
public:
  UserDataItem_List*    udil;
  QTableWidget*         tw;

  override void         Constr_Box();
//obs  override taBase* GetMembBase_Flat(int idx); // these are overridden by userdata
//obs  override taBase* GetMethBase_Flat(int idx);
  override void         GetImage_Item(int row); // called from GetImage and ed->GetValue

  iUserDataDataHost(void* base, TypeDef* td, bool read_only_ = false,
        QObject* parent = 0);
  iUserDataDataHost()   { Initialize();};
  ~iUserDataDataHost();

public: // IDataLinkClient i/f
  override void         DataDataChanged(taDataLink* dl, int dcr, void* op1, void* op2);

protected:
  int                   sel_item_row;
  UserDataDelegate*     udd;

  UserDataItemBase*     GetUserDataItem(int row);

  override void         Constr_impl();
  override void         Constr_Body_impl();
  override void         ClearBody_impl();       // we also clear all the methods, and then rebuild them

  override void         Constr_Data_Labels();
  override void         FillLabelContextMenu_SelEdit(QMenu* menu, int& last_id);
  override void         GetImage_Membs_def();
  override void         GetValue_Membs_def();
  override void         Constr_Methods_impl();

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

#endif // iUserDataDataHost_h

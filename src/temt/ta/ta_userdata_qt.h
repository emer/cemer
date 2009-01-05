// Copyright, 1995-2007, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of The Emergent Toolkit
//
//   Emergent is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   Emergent is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.

// ta_userdata_qt.h -- user data, gui files

#ifndef TA_USERDATA_QT_H
#define TA_USERDATA_QT_H

#include "ta_group.h"

#include "ta_qtgroup.h"
#include "ta_qtviewer.h"
#include "ta_qtdialog.h"
#include "ta_seledit_qt.h" // for taiDataDelegate

#ifndef __MAKETA__
# include <QPointer>
# include <QItemDelegate>
#endif

#include "ta_TA_type.h"

class iUserDataDataHost;
class UserDataDelegate; // #IGNORE

//
#ifndef __MAKETA__
class TA_API UserDataDelegate: public taiDataDelegate {
INHERITED(taiDataDelegate)
Q_OBJECT
public:
  UserDataItem_List*	udil;
  iUserDataDataHost*	uddh;
  
  override bool		IndexToMembBase(const QModelIndex& index,
    MemberDef*& mbr, taBase*& base) const;

  override QWidget* createEditor(QWidget* parent, 
    const QStyleOptionViewItem& option, const QModelIndex& index) const;
  
  UserDataDelegate(UserDataItem_List* udil_, iUserDataDataHost* uddh_);
};
#endif // !__MAKETA__


class TA_API iUserDataDataHost : public taiEditDataHost {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS edit only selected items from a range of ta-base objects
INHERITED(taiEditDataHost)
  Q_OBJECT
public:
  UserDataItem_List*	udil;
  QTableWidget*		tw;

  override void		Constr_Box();
//obs  override taBase*	GetMembBase_Flat(int idx); // these are overridden by userdata
//obs  override taBase*	GetMethBase_Flat(int idx);
  
  iUserDataDataHost(void* base, TypeDef* td, bool read_only_ = false,
  	QObject* parent = 0);
  iUserDataDataHost()	{ Initialize();};
  ~iUserDataDataHost();

public: // IDataLinkClient i/f
  override void 	DataDataChanged(taDataLink* dl, int dcr, void* op1, void* op2);

protected:
  int			sel_item_row;
  UserDataDelegate*	udd;
  
  UserDataItemBase*	GetUserDataItem(int row);
  
  override void		Constr_impl();
  override void		Constr_Body_impl();
  override void		ClearBody_impl();	// we also clear all the methods, and then rebuild them

  override void		Constr_Data_Labels(); 
  override void		FillLabelContextMenu_SelEdit(QMenu* menu, int& last_id);
  override void 	GetImage_Membs_def();
  override void 	GetValue_Membs_def();
  override void 	Constr_Methods_impl();

protected slots:
  virtual void		DoDeleteUserDataItem();
  virtual void		DoRenameUserDataItem();
  void 			tw_currentCellChanged( int currentRow, 
    int currentColumn, int previousRow, int previousColumn);
  void 			tw_customContextMenuRequested(const QPoint& pos);
  void 			tw_itemChanged(QTableWidgetItem* item);
private:
  void	Initialize();
};


class TA_API iUserDataPanel: public iDataPanelFrame {
// ##NO_CSS note: UserDataItem_List main panel (for users)
INHERITED(iDataPanelFrame)
  Q_OBJECT
public:
  taiDataHost_impl*	se;
  UserDataItem_List*	udil() {return (m_link) ? (UserDataItem_List*)(link()->data()) : NULL;}
  
  override bool		HasChanged(); // 'true' if user has unsaved changes

  override String	panel_type() const {return "User Data";}

  iUserDataPanel(taiDataLink* dl_);
  ~iUserDataPanel();
  
public: // IDataLinkClient interface
  override void*	This() {return (void*)this;}
  override TypeDef*	GetTypeDef() const {return &TA_iUserDataPanel;}
  override bool		ignoreDataChanged() const;
  
protected:
  override void		DataChanged_impl(int dcr, void* op1, void* op2); //
  override void		OnWindowBind_impl(iTabViewer* itv);
  override void		UpdatePanel_impl();
  override void		ResolveChanges_impl(CancelOp& cancel_op);
};

#endif

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

// ta_seledit_qt.h -- select edit, gui files

#ifndef TA_SELEDIT_QT_H
#define TA_SELEDIT_QT_H

#include "ta_seledit.h"

#include "ta_qtgroup.h"
#include "ta_qtviewer.h"
#include "ta_qtdialog.h"

#ifndef __MAKETA__
# include <QPointer>
# include <QItemDelegate>
#endif

#include "ta_TA_type.h"


class TA_API tabSelectEditViewType: public tabOViewType {
INHERITED(tabOViewType)
public:
  override int		BidForView(TypeDef*);
  void			Initialize() {}
  void			Destroy() {}
  TA_VIEW_TYPE_FUNS(tabSelectEditViewType, tabOViewType) //
protected:
//nn  override taiDataLink*	CreateDataLink_impl(taBase* data_);
  override void		CreateDataPanel_impl(taiDataLink* dl_);
};



class TA_API iSelectEditDataHostBase : public taiEditDataHost {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS edit only selected items from a range of ta-base objects
INHERITED(taiEditDataHost)
  Q_OBJECT
public:
  SelectEdit*	sele;

  override void		Constr_Body();
  override taBase*	GetMembBase_Flat(int idx); // these are overridden by seledit
  override taBase*	GetMethBase_Flat(int idx);
  
  iSelectEditDataHostBase(void* base, TypeDef* td, bool read_only_ = false,
  	QObject* parent = 0);
  iSelectEditDataHostBase()	{ Initialize();};
  ~iSelectEditDataHostBase();

protected:
  void 			Constr_Methods();

protected slots:
  virtual void		DoRemoveSelEdit() = 0;
  virtual void		mnuRemoveMember_select(int idx); // #IGNORE removes the indicated member
  virtual void		mnuRemoveMethod_select(int idx); // #IGNORE removes the indicated method
private:
  void	Initialize();
};

//
class TA_API iSelectEditDataHost : public iSelectEditDataHostBase {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS edit only selected items from a range of ta-base objects
INHERITED(iSelectEditDataHostBase)
  Q_OBJECT
public:
  
  iSelectEditDataHost(void* base, TypeDef* td, bool read_only_ = false,
  	QObject* parent = 0);
  iSelectEditDataHost()	{ Initialize();};
  ~iSelectEditDataHost();

protected slots:
  override void		DoRemoveSelEdit();

protected:
  override void		ClearBody_impl();	// we also clear all the methods, and then rebuild them

  override void		Constr_Data_Labels(); 
  override void		FillLabelContextMenu_SelEdit(QMenu* menu, int& last_id);
  override void 	GetImage_Membs_def();
  override void 	GetValue_Membs_def();

private:
  void	Initialize();
};

//
#ifndef __MAKETA__
class TA_API taiDataDelegate: public QItemDelegate {
INHERITED(QItemDelegate)
Q_OBJECT
public:
  taiEditDataHost*	edh;
  
  virtual void		GetImage() const; // callable from edh any time
  virtual void		GetValue() const; // callable from edh
  virtual bool		IndexToMembBase(const QModelIndex& index,
    MemberDef*& mbr, taBase*& base) const = 0;
    
  taiDataDelegate(taiEditDataHost* edh_);
  
public: // overrides
  override QWidget* 	createEditor(QWidget* parent, 
    const QStyleOptionViewItem& option, const QModelIndex& index) const;
  override void 	setEditorData(QWidget* editor, 
    const QModelIndex& index) const;
  override void 	setModelData(QWidget* editor, QAbstractItemModel* model,
    const QModelIndex& index ) const;

protected:
  mutable QPointer<taiData> dat; // most recently created

  override bool 	eventFilter(QObject* object, QEvent* event); // replace

protected slots:
  virtual void		rep_destroyed(QObject* rep); // when dat.rep destroys
};
#endif // !__MAKETA__

class SelectEditDelegate; // #IGNORE

class TA_API iSelectEditDataHost2 : public iSelectEditDataHostBase {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS alternative, using viewer
INHERITED(iSelectEditDataHostBase)
  Q_OBJECT
public:
  QTableWidget*		tw;

  override void		Constr_Box();
  
  iSelectEditDataHost2(void* base, TypeDef* td, bool read_only_ = false,
  	QObject* parent = 0);
  iSelectEditDataHost2()	{ Initialize();};
  ~iSelectEditDataHost2();

protected slots:
  override void		DoRemoveSelEdit();
  void 			tw_currentCellChanged( int currentRow, 
    int currentColumn, int previousRow, int previousColumn);
  void 			tw_customContextMenuRequested(const QPoint& pos);

protected:
  SelectEditDelegate*	sed;
  
  override void		Constr_Body_impl();
  override void		ClearBody_impl();	// we also clear all the methods, and then rebuild them

  override void		Constr_Data_Labels(); 
  override void		FillLabelContextMenu_SelEdit(QMenu* menu, int& last_id);
  override void 	GetImage_Membs_def();
  override void 	GetValue_Membs_def();

private:
  void	Initialize();
};
//
#ifndef __MAKETA__
class TA_API SelectEditDelegate: public taiDataDelegate {
INHERITED(taiDataDelegate)
Q_OBJECT
public:
  SelectEdit*		sele;
  iSelectEditDataHost2*	sedh;
  
  override bool		IndexToMembBase(const QModelIndex& index,
    MemberDef*& mbr, taBase*& base) const;

  override QWidget* createEditor(QWidget* parent, 
    const QStyleOptionViewItem& option, const QModelIndex& index) const;
  
  SelectEditDelegate(SelectEdit* sele_, iSelectEditDataHost2* sedh_);
};
#endif // !__MAKETA__




class TA_API iSelectEditPanel: public iDataPanelFrame {
// ##NO_CSS note: SelectEdit main panel (for users)
INHERITED(iDataPanelFrame)
  Q_OBJECT
public:
  taiDataHost_impl*	se;
  SelectEdit*		sele() {return (m_link) ? (SelectEdit*)(link()->data()) : NULL;}
  
  override bool		HasChanged(); // 'true' if user has unsaved changes

  override String	panel_type() const {return "Select Edit";}

  iSelectEditPanel(taiDataLink* dl_);
  ~iSelectEditPanel();
  
public: // IDataLinkClient interface
  override void*	This() {return (void*)this;}
  override TypeDef*	GetTypeDef() const {return &TA_iSelectEditPanel;}
  override bool		ignoreDataChanged() const;
  
protected:
  override void		DataChanged_impl(int dcr, void* op1, void* op2); //
  override void		OnWindowBind_impl(iTabViewer* itv);
  override void		UpdatePanel_impl();
  override void		ResolveChanges_impl(CancelOp& cancel_op);
};

#endif

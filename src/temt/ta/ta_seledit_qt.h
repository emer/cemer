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
# include <QTableWidget>
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
  override void 	Constr_Methods_impl();

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
  override void 	paint(QPainter* painter, const QStyleOptionViewItem& option,
    const QModelIndex& index) const;
  override void 	setEditorData(QWidget* editor, 
    const QModelIndex& index) const;
  override void 	setModelData(QWidget* editor, QAbstractItemModel* model,
    const QModelIndex& index ) const;
  override QSize	sizeHint(const QStyleOptionViewItem& option,
    const QModelIndex& index) const;
protected:
  mutable QPointer<taiData> dat; // most recently created
  mutable QPointer<QWidget> rep; // most recently created
  mutable int		m_dat_row; // row corresponding to dat, -1 if none
  mutable QSize		sh; // current size hint -- we return greatest
  mutable QPointer<QHBoxLayout>	hbl;

  override bool 	eventFilter(QObject* object, QEvent* event); // replace
  virtual void		EditorCreated(QWidget* parent, QWidget* editor, 
    const QStyleOptionViewItem& option, const QModelIndex& index) const;
  
protected slots:
  virtual void		rep_destroyed(QObject* rep); // when dat.rep destroys
  virtual void 		this_closeEditor(QWidget* editor,
    QAbstractItemDelegate::EndEditHint hint = NoHint);
};
#endif // !__MAKETA__

class SelectEditDelegate; // #IGNORE

class TA_API taiEditTableWidget: public QTableWidget {
  // ##NO_INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBER a table widget for an edit dialog
INHERITED(QTableWidget)
  Q_OBJECT
public:

  taiEditTableWidget(QWidget* parent = NULL);


protected:
  override void 	keyPressEvent(QKeyEvent* e);
};

class TA_API iSelectEditDataHost2 : public iSelectEditDataHostBase {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS alternative, using viewer
INHERITED(iSelectEditDataHostBase)
  Q_OBJECT
public:
  taiEditTableWidget*	tw;

  override void		Constr_Box();
  override void 	GetImage_Item(int row); // called from GetImage and ed->GetValue
  
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

  override void		GetImage() const; // callable from edh any time
  override void		GetValue() const; // callable from edh

  override bool		IndexToMembBase(const QModelIndex& index,
    MemberDef*& mbr, taBase*& base) const;

  override QWidget* createEditor(QWidget* parent, 
    const QStyleOptionViewItem& option, const QModelIndex& index) const;
  
  SelectEditDelegate(SelectEdit* sele_, iSelectEditDataHost2* sedh_);

 protected:
  mutable EditMbrItem*   emi;
  mutable QPointer<taiData> ps_dat; // most recently created
  mutable QPointer<QWidget> ps_rep; // most recently created
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


//////////////////////////////////
// 	taiObjDiffBrowser	//
//////////////////////////////////

class taiObjDiffBrowser;

class TA_API taiODRDelegate: public QItemDelegate {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS delegate for browser
INHERITED(QItemDelegate)
Q_OBJECT
public:
  taiObjDiffBrowser*	odb;
 
  taiODRDelegate(taiObjDiffBrowser* odb_);

#ifndef __MAKETA__  
  override QWidget* 	createEditor(QWidget* parent, const QStyleOptionViewItem& option,
				     const QModelIndex& index) const;
  override void 	setEditorData(QWidget* editor, const QModelIndex& index) const;
  override void 	setModelData(QWidget* editor, QAbstractItemModel* model,
				     const QModelIndex& index ) const;
#endif

};

class TA_API taiObjDiffBrowser: iDialog {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS select items from a list, much like a file chooser; can be tokens from typedef or items on a list
INHERITED(iDialog)
  Q_OBJECT
public:
  enum ODBCols {
    COL_A_FLG,
    COL_A_NM,
    COL_A_VAL,
    COL_B_FLG,
    COL_B_NM,
    COL_B_VAL,
    COL_N,
  };

  static taiObjDiffBrowser* New(const String& caption, taObjDiff_List& diffs, 
				int font_type, QWidget* par_window_ = NULL);

  String		caption; 	// current caption at top of chooser
  taObjDiff_List*	odl;

  QVBoxLayout*		layOuter;
  QTreeWidget* 		  items; 	// list of items
  QPushButton*		    btnOk;
  QPushButton*		    btnCancel;
  taiODRDelegate*	ordel;	// delegate for a toggle

  virtual bool		Browse();
  // main user interface: this actually puts up the dialog, returns true if Ok, false if cancel

  virtual void		Constr();
  virtual void		AddItems();

  taiObjDiffBrowser(const String& captn, QWidget* par_window_);
  ~taiObjDiffBrowser();							      
protected slots:
  override void 	accept(); // override
  override void 	reject(); // override
private:
  void 		init(const String& captn); // called by constructors
};


#endif

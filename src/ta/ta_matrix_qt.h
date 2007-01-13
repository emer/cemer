// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/CSS
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


// ta_matrix_qt.h -- gui header for matrix data (used by data system)

#ifndef TA_MATRIX_QT_H
#define TA_MATRIX_QT_H

#include "ta_stdef.h"
#include "ta_matrix.h"
#include "ta_qttype.h"
#include "ta_qtviewer.h"


class TA_API tabMatrixViewType: public tabOViewType {
INHERITED(tabOViewType)
public:
  override int		BidForView(TypeDef*);
  void			Initialize() {}
  void			Destroy() {}
  TA_VIEW_TYPE_FUNS(tabMatrixViewType, tabOViewType) //
protected:
//nn  override taiDataLink*	CreateDataLink_impl(taBase* data_);
  override void		CreateDataPanel_impl(taiDataLink* dl_);
};

class TA_API iMatTableView: public QTableView, public virtual ISelectable {
  // table editor; model flattens >2d into 2d by frames
INHERITED(QTableView)
  Q_OBJECT
public:
  taMatrix*		mat() const;

  iMatTableView(ISelectableHost* host_, QWidget* parent = NULL);
#ifndef __MAKETA__
signals:
  void			hasFocus(); // we emit anytime something happens which implies we are focused
#endif

 
public: // ITypedObject interface
  override void*	This() {return (void*)this;}
  override TypeDef*	GetTypeDef() const {return &TA_iMatTableView;}

public: // ISelectable interface
  override taiDataLink*	link() const;
//  override MemberDef*	md() const {return m_md;}
//obs  override String	view_name() const; // for members, the member name; for list items, the name if
  override ISelectableHost* host() const {return m_host;}
  override taiClipData*	GetClipDataSingle(int src_edit_action, bool for_drag) const;
//  override int		GetEditActions(taiMimeSource* ms) const; // simpler version uses Query
protected:
  override int		EditActionD_impl_(taiMimeSource* ms, int ea);
  override int		EditActionS_impl_(int ea);
//  override void		FillContextMenu_EditItems_impl(taiMenu* menu, int allowed);
//  override void		FillContextMenu_impl(taiMenu* menu); // this is the one to extend in inherited classes
  override void		QueryEditActionsD_impl_(taiMimeSource* ms, int& allowed, int& forbidden) const;
  override void		QueryEditActionsS_impl_(int& allowed, int& forbidden) const;

protected:
  ISelectableHost* 	m_host;
  
  override bool		event(QEvent* ev);
};

class TA_API iMatrixEditor: public QWidget {
  // widget that includes a table editor; model flattens >2d into 2d by frames
INHERITED(QWidget)
  Q_OBJECT
public:
  QVBoxLayout*		layOuter;
  QHBoxLayout*		  layDims;
  iMatTableView*		  tv;

  MatrixTableModel* 	model() const;
  void			setModel(MatrixTableModel* mod);
  
  void			Refresh(); // for manual refresh -- note, this also updates all other mat editors too
  
  iMatrixEditor(QWidget* parent = NULL); //
  
public slots: // cliphandler i/f; note: no UpdateUI signal
  void 			EditAction(int ea); 
  void			GetEditActionsEnabled(int& ea); 
#ifndef __MAKETA__
signals:
  void			UpdateUi();
#endif

protected:
  
protected slots:
  void 			tv_customContextMenuRequested(const QPoint& pos);

private:
  void		init();
  
};


class TA_API iMatrixPanel: public iDataPanelFrame {
  Q_OBJECT
#ifndef __MAKETA__
typedef iDataPanelFrame inherited;
#endif
public:
  iMatrixEditor*	me;
  
  taMatrix*		mat() {return (m_link) ? (taMatrix*)(link()->data()) : NULL;}
  override String	panel_type() const; // this string is on the subpanel button for this panel

  override int 		EditAction(int ea);
  void			FillList();
  override int		GetEditActions(); // after a change in selection, update the available edit actions (cut, copy, etc.)
  void			GetSelectedItems(ISelectable_PtrList& lst); // list of the selected cells

  iMatrixPanel(taiDataLink* dl_);
  ~iMatrixPanel();

protected slots:
  void			me_hasFocus();

public: // IDataLinkClient interface
  override void*	This() {return (void*)this;}
  override TypeDef*	GetTypeDef() const {return &TA_iMatrixPanel;}
protected:
  override void		Render_impl();
  override void		DataChanged_impl(int dcr, void* op1, void* op2); //
//  override int 		EditAction_impl(taiMimeSource* ms, int ea, ISelectable* single_sel_node = NULL);


protected: // IDataViewWidget i/f
  override void		Refresh_impl();
};


#endif

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

class TA_API iMatrixEditor: public QWidget {
  // widget that includes a table editor; model flattens >2d into 2d by frames
INHERITED(QWidget)
  Q_OBJECT
public:
  QVBoxLayout*		layOuter;
  QHBoxLayout*		  layDims;
  QTableView*		  tv;

  MatrixTableModel* 	model() const;
  void			setModel(MatrixTableModel* mod);
  
  int			QueryEditActions(taiMimeSource* ms);
    // get edit items allowed for this one item
  int			EditAction(int ea);
  void			Refresh(); // for manual refresh -- note, this also updates all other mat editors too
  
  iMatrixEditor(QWidget* parent = NULL);
  
protected:
  virtual void		QueryEditActions_impl(taiMimeSource* ms,
    int& allowed, int& forbidden);
  virtual int		EditAction_impl(taiMimeSource* ms, int ea);
  
protected slots:
  void 			tv_customContextMenuRequested(const QPoint& pos);
  void 			this_editAction(int ea);

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

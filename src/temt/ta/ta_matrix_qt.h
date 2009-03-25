// Copyright, 1995-2007, Regents of the University of Colorado,
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


// ta_matrix_qt.h -- gui header for matrix data (used by data system)

#ifndef TA_MATRIX_QT_H
#define TA_MATRIX_QT_H

#include "ta_stdef.h"
#include "ta_matrix.h"
#include "ta_qttype.h"
#include "ta_qtviewer.h"

class MatrixTableModel; //


class TA_API MatrixTableModel: public QAbstractTableModel
{
  // #NO_INSTANCE #NO_CSS class that implements the Qt Model interface for matrices; we extend it to support N-d, but only 2-d cell display; if the model has a single gui client, then set it, to avoid doing refreshes when it isn't visible
friend class taMatrix;
INHERITED(QAbstractTableModel)
  Q_OBJECT
public:
  int			col_idx; // when this is a DataTable mat cell, the view sets this, otherwise it is -1 -- used in cell updated signal to DataTableModel
#ifndef __MAKETA__
  int			matIndex(const QModelIndex& idx) const; // #IGNORE flat matrix data index
  override QMimeData* 	mimeData (const QModelIndexList& indexes) const;
  override QStringList	mimeTypes () const;
  taMisc::MatrixView 	matView() const;
#endif //note: bugs in maketa necessitated these sections
  taMatrix*		mat() const {return m_mat;}
  inline bool		pat4D() const {return m_pat_4d;} // for dims>=4 whether to group d0/d1 in row (default is true)
  void			setPat4D(bool val, bool notify = true);
  
  void			emit_dataChanged(int row_fr = 0, int col_fr = 0,
    int row_to = -1, int col_to = -1);// can be called w/o params to issue global change (for manual refresh)
  void			emit_dataChanged(const QModelIndex& topLeft,
    const QModelIndex& bottomRight); 
  void			emit_layoutChanged();
  
protected: // only from matrix
  MatrixTableModel(taMatrix* mat_);
  ~MatrixTableModel(); //
  
public: // required implementations
#ifndef __MAKETA__
  int 			columnCount(const QModelIndex& parent = QModelIndex()) const; // override
  QVariant 		data(const QModelIndex& index, int role = Qt::DisplayRole) const; // override
  Qt::ItemFlags 	flags(const QModelIndex& index) const; // override, for editing
  QVariant 		headerData(int section, Qt::Orientation orientation, 
    int role = Qt::DisplayRole) const; // override
  int 			rowCount(const QModelIndex& parent = QModelIndex()) const; // override
  bool 			setData(const QModelIndex& index, const QVariant& value, 
    int role = Qt::EditRole); // override, for editing

signals:
  void			matDataChanged(int col_idx); // only emited during dataChanged if col_idx valid
  
public: // pseudo-IDataLinkClient i/f
  override void		DataDataChanged(int dcr, void* op1, void* op2); 
  
protected:
  taMatrix*		m_mat;
  taMisc::MatrixView	m_view_layout; //#IGNORE #DEF_TOP_ZERO
  ContextFlag		notifying; // to avoid responding when we sent notify
  bool			m_pat_4d;
  
  bool			ValidateIndex(const QModelIndex& index) const;
#endif
};

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

class TA_API iTableView: public QTableView {
  // ##NO_INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBER table editor; model flattens >2d into 2d by frames
INHERITED(QTableView)
  Q_OBJECT
public:
  enum ContextArea { // where ctxt menu was requested
    CA_EMPTY	= 0x01,
    CA_GRID	= 0x02,
    CA_COL_HDR	= 0x04,
    CA_ROW_HDR	= 0x08,
    CA_HDR_MASK = 0x0C
  };

  virtual bool		isFixedRowCount() const = 0; // true, ex. for tab mat cells with fixed rows
  virtual bool		isFixedColCount() const = 0; // true, ex. for tab mat cells with fixed geom
  
  iTableView(QWidget* parent = NULL);
  
#ifndef __MAKETA__
signals:
  void			hasFocus(iTableView* sender); // we emit anytime something happens which implies we are focused
#endif

public slots: // cliphandler i/f
  virtual void 		EditAction(int ea) {}
  virtual void		GetEditActionsEnabled(int& ea) {}
#ifndef __MAKETA__
signals:
  void			UpdateUi();
#endif


protected:
  enum RowColOpCode {
    OP_APPEND		= 0x01,
    OP_INSERT		= 0x02,
    OP_DUPLICATE	= 0x04,
    OP_DELETE		= 0x08,
    OP_ROW		= 0x40, 
    OP_COL		= 0x80
  };
  
  override bool		event(QEvent* ev);
  override void 	keyPressEvent(QKeyEvent* e);
  void 			ContextMenuRequested(ContextArea ca, const QPoint& global_pos);
  virtual void		FillContextMenu_impl(ContextArea ca, taiMenu* menu,
    const CellRange& sel);
  virtual void		RowColOp_impl(int op_code, const CellRange& sel) {} 
    
protected slots:
  void 			this_customContextMenuRequested(const QPoint& pos);
  void 			hor_customContextMenuRequested(const QPoint& pos);
  void 			ver_customContextMenuRequested(const QPoint& pos);
  void			RowColOp(int op_code); // based on selection
};

class TA_API iMatrixTableView: public iTableView {
  // table editor; model flattens >2d into 2d by frames
INHERITED(iTableView)
  Q_OBJECT
public:
  taMatrix*		mat() const;

  override bool		isFixedRowCount() const;
  override bool		isFixedColCount() const {return true;}
  
  void			GetSel(CellRange& sel); 
   // gets current selection, factoring in BOT_ZERO if needed
  
  iMatrixTableView(QWidget* parent = NULL);
  
public: // cliphandler i/f
  override void 	EditAction(int ea);
  override void		GetEditActionsEnabled(int& ea);
};

class TA_API iMatrixEditor: public QWidget {
  // ##NO_INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBER widget that includes a table editor; model flattens >2d into 2d by frames
INHERITED(QWidget)
  Q_OBJECT
public:
  QVBoxLayout*		layOuter;
  QHBoxLayout*		  layDims;
  iMatrixTableView*		  tv;

 // taMatrix*		mat() const;
 // MatrixTableModel* 	model() const;
  void			setMatrix(taMatrix* mat, bool pat_4d = false);
  
  void			Refresh(); // for manual refresh -- note, this also updates all other mat editors too
  
  iMatrixEditor(QWidget* parent = NULL); //
  ~iMatrixEditor();
  
protected:
  taMatrix*		m_mat;
  
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

  override QWidget*	firstTabFocusWidget();

  iMatrixPanel(taiDataLink* dl_);
  ~iMatrixPanel();

protected: 
  override void		UpdatePanel_impl();
  
protected slots:
  void			tv_hasFocus(iTableView* sender);

public: // IDataLinkClient interface
  override void*	This() {return (void*)this;}
  override TypeDef*	GetTypeDef() const {return &TA_iMatrixPanel;}
protected:
  override void		Render_impl();
  override void		DataChanged_impl(int dcr, void* op1, void* op2); //
//  override int 		EditAction_impl(taiMimeSource* ms, int ea, ISelectable* single_sel_node = NULL);

  
};


#endif

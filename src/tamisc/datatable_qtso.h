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



#ifndef datatable_qtso_h
#define datatable_qtso_h

#include "ta_qttype.h"
#include "ta_qtviewer.h"

#include "colorscale.h"
#include "colorbar_qt.h"
#include "datatable.h"
//#include "fontspec.h"

#ifndef __MAKETA__
#include <QAbstractTableModel> 
#endif

// forwards
class DataTableModel;
class tabDataTableViewType;
class iDataTablePanel;


class TAMISC_API DataTableModel: public QAbstractTableModel, public IDataLinkClient { // #NO_INSTANCE #NO_CSS class that implements the Qt Model interface for tables
INHERITED(QAbstractTableModel)
public:

  DataTable*		dataTable() const {return m_dt;}
  void			setDataTable(DataTable* value, bool notify = true);
  
  DataTableModel(QObject* parent = NULL);
  ~DataTableModel(); //
  
public: // IDataLinkClient i/f
  void*		This() {return this;} // reference to the 'this' pointer of the client object
  TypeDef*	GetTypeDef() const {return &TA_DataTableModel;} // typedef of the dlc
  void		DataLinkDestroying(taDataLink* dl); // override
  void		DataDataChanged(taDataLink* dl, int dcr, void* op1, void* op2); // override

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

protected:
  bool			ValidateIndex(const QModelIndex& index) const;
  DataTable*		m_dt;
#endif
};


//////////////////////////
//  tabDataTableViewType//
//////////////////////////

class TAMISC_API tabDataTableViewType: public tabGroupViewType {
#ifndef __MAKETA__
typedef tabGroupViewType inherited;
#endif
public:
  override int		BidForView(TypeDef*);
  void			Initialize() {}
  void			Destroy() {}
  TA_VIEW_TYPE_FUNS(tabDataTableViewType, tabGroupViewType) //
protected:
//nn  override taiDataLink*	CreateDataLink_impl(taBase* data_);
  override void		CreateDataPanel_impl(taiDataLink* dl_);
};


//////////////////////////
//  iDatatableDataPanel //
//////////////////////////

class TAMISC_API iDataTablePanel: public iDataPanelFrame {
  Q_OBJECT
#ifndef __MAKETA__
typedef iDataPanelFrame inherited;
#endif
public:
  QTableView*		tv;
  
  DataTable*		dt() {return (m_link) ? (DataTable*)(link()->data()) : NULL;}
  override String	panel_type() const; // this string is on the subpanel button for this panel

  override int 		EditAction(int ea);
  void			FillList();
  override int		GetEditActions(); // after a change in selection, update the available edit actions (cut, copy, etc.)
  void			GetSelectedItems(ISelectable_PtrList& lst); // list of the selected cells

  iDataTablePanel(taiDataLink* dl_);
  ~iDataTablePanel();

public: // IDataLinkClient interface
  override void*	This() {return (void*)this;}
  override TypeDef*	GetTypeDef() const {return &TA_iDataTablePanel;}
protected:
  override void		DataChanged_impl(int dcr, void* op1, void* op2); //
//  override int 		EditAction_impl(taiMimeSource* ms, int ea, ISelectable* single_sel_node = NULL);

/*protected slots: */
};


#endif

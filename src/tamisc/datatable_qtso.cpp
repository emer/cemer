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



#include "datatable_qtso.h"

// stuff to implement graphical view of datatable

#include "ta_qtgroup.h"
#include "ta_qtclipdata.h"

#include <qlayout.h>
#include <qpalette.h>
#include <QTableView>

#include <limits.h>
#include <float.h>
//nn? #include <unistd.h>

#define DIST(x,y) sqrt((double) ((x * x) + (y*y)))

DataTableModel::DataTableModel(QObject* parent) 
:inherited(parent)
{
  m_dt = NULL;
}

DataTableModel::~DataTableModel() {
  setDataTable(NULL, false);
}

int DataTableModel::columnCount(const QModelIndex& parent) const {
  if (!m_dt) return 0;
  else       return m_dt->leaves;
}

QVariant DataTableModel::data(const QModelIndex& index, int role) const {
  if (!m_dt || !index.isValid()) return QVariant();
  
  switch (role) {
  case Qt::TextAlignmentRole: {
    DataArray_impl* col = m_dt->GetColData(index.column());
    if (col) {
      if (col->is_numeric())
        return QVariant(Qt::AlignRight | Qt::AlignVCenter);
      else
        return QVariant(Qt::AlignLeft | Qt::AlignVCenter);
    } 
    } break;
  case Qt::DisplayRole:
    return m_dt->GetValAsVar(index.column(), index.row());
  default: break;
  }
  return QVariant();
}

void DataTableModel::DataDataChanged(taDataLink* dl, int dcr, void* op1, void* op2) {
  //TODO: change notification more particular????
  emit layoutChanged();
}

void DataTableModel::DataLinkDestroying(taDataLink* dl) {
  setDataTable(NULL);
  //TODO: change notification
}

Qt::ItemFlags DataTableModel::flags(const QModelIndex& index) const {
  if (!m_dt || !index.isValid()) return 0;
  Qt::ItemFlags rval = 0;
  if (ValidateIndex(index)) {
    // don't enable null cells
    if (m_dt->hasData(index.column(), index.row() )) {
      rval = Qt::ItemIsSelectable | Qt::ItemIsEnabled;
      //TODO: determine if not editable, ex. maybe for matrix types
      DataArray_impl* col = m_dt->GetColData(index.column());
      if (col && !col->is_matrix)  
        rval |= Qt::ItemIsEditable;
    }
  }
  return rval;
}

QVariant DataTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
  if (role != Qt::DisplayRole)
    return QVariant();
  if (orientation == Qt::Horizontal) {
    DataArray_impl* col = m_dt->GetColData(section);
    if (col)  
      return QString(col->GetDisplayName().chars());
    else 
      return QString();
  } else {
    return QString::number(section);
  }
}

int DataTableModel::rowCount(const QModelIndex& parent) const {
  if (!m_dt) return 0;
  else       return m_dt->rows;
}

bool DataTableModel::setData(const QModelIndex& index, const QVariant & value, int role) {
  if (!m_dt || !index.isValid()) return false;
  bool rval = false;
  switch (role) {
  case Qt::EditRole:
    m_dt->SetValAsVar(value, index.column(), index.row());
    emit dataChanged(index, index);
    return rval;
  default: return false;
  }
}


void DataTableModel::setDataTable(DataTable* value, bool notify) {
  if (m_dt == value) return;
  if (m_dt) { // disconnect
    m_dt->RemoveDataClient(this);
  }
  m_dt = value;
  if (m_dt) { // connect
    m_dt->AddDataClient(this);
  }
  //TODO: make sure this is the right signal
  if (notify) {
    emit layoutChanged();
  }
}

bool DataTableModel::ValidateIndex(const QModelIndex& index) const {
  if (!m_dt) return false;
  return (index.isValid() && (index.row() < m_dt->rows) && (index.column() < m_dt->leaves));
}


//////////////////////////
// tabDataTableViewType	//
//////////////////////////

int tabDataTableViewType::BidForView(TypeDef* td) {
  if (td->InheritsFrom(&TA_DataTable))
    return (inherited::BidForView(td) +1);
  return 0;
}

/*taiDataLink* tabDataTableViewType::CreateDataLink_impl(taBase* data_) {
  return new tabListDataLink((taList_impl*)data_);
} */

void tabDataTableViewType::CreateDataPanel_impl(taiDataLink* dl_)
{
  inherited::CreateDataPanel_impl(dl_);
  iDataTablePanel* dp = new iDataTablePanel(dl_);
  DataPanelCreated(dp);
}



//////////////////////////
//    iDataTablePanel 	//
//////////////////////////

iDataTablePanel::iDataTablePanel(taiDataLink* dl_)
:inherited(dl_)
{
/*  layOuter = NULL; // nuke
  idt = new iDataTable();
  setCentralWidget(idt);
  idt->setDataTable(dt()); */

  tv = new QTableView();
  setCentralWidget(tv);
  DataTable* dt_ = dt();
  if (dt_) {
    tv->setModel(dt_->GetDataModel());
  }
/*  list->setSelectionMode(QListView::Extended);
  list->setShowSortIndicator(true);
  // set up number of cols, based on link
  list->addColumn("#");
  for (int i = 0; i < link()->NumListCols(); ++i) {
    list->addColumn(link()->GetColHeading(i));
  }
  connect(list, SIGNAL(contextMenuRequested(QListViewItem*, const QPoint &, int)),
      this, SLOT(list_contextMenuRequested(QListViewItem*, const QPoint &, int)) );
  connect(list, SIGNAL(selectionChanged()),
      this, SLOT(list_selectionChanged()) );
  FillList(); */
}

iDataTablePanel::~iDataTablePanel() {
}

void iDataTablePanel::DataChanged_impl(int dcr, void* op1_, void* op2_) {
  inherited::DataChanged_impl(dcr, op1_, op2_);
  //NOTE: don't need to do anything because DataModel will handle it
//TODO: maybe we should do something less crude???
//  idt->updateConfig();
}

int iDataTablePanel::EditAction(int ea) {
  int rval = 0;

  ISelectable_PtrList sel_list;
  GetSelectedItems(sel_list);
  ISelectable* ci = sel_list.SafeEl(0);
  if (ci)  {
    rval = ci->EditAction_(sel_list, ea);
  }
  return rval;
}

int iDataTablePanel::GetEditActions() {
  int rval = 0;

  ISelectable_PtrList sel_list;
  GetSelectedItems(sel_list);
  ISelectable* ci = sel_list.SafeEl(0);
  if (ci)  {
    rval = ci->GetEditActions_(sel_list);
    // certain things disallowed if more than one item selected
    if (sel_list.size > 1) {
      rval &= ~(taiClipData::EA_FORB_ON_MUL_SEL);
    }
  }
  return rval;
}

void iDataTablePanel::GetSelectedItems(ISelectable_PtrList& lst) {
/*TODO  QListViewItemIterator it(list, QListViewItemIterator::Selected);
  while (it.current()) {
    lst.Add((taiListDataNode*)it.current());
    ++it;
  } */
}

/*void iDataTablePanel::idt_contextMenuRequested(QListViewItem* item, const QPoint & pos, int col ) {
  //TODO: 'item' will be whatever is under the mouse, but we could have a multi select!!!
  taiListDataNode* nd = (taiListDataNode*)item;
  if (nd == NULL) return; //TODO: could possibly be multi select

  taiMenu* menu = new taiMenu(this, taiMenu::popupmenu, taiMenu::normal, taiMisc::fonSmall);
  //TODO: any for us first (ex. delete)

  ISelectable_PtrList sel_list;
  GetSelectedItems(sel_list);
  nd->FillContextMenu(sel_list, menu); // also calls link menu filler

  //TODO: any for us last (ex. delete)
  if (menu->count() > 0) { //only show if any items!
    menu->exec(pos);
  }
  delete menu;
}

void iDataTablePanel::list_selectionChanged() {
  viewer_win()->UpdateUi();
}*/


String iDataTablePanel::panel_type() const {
  static String str("Data Table");
  return str;
}


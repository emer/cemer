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


#include "ta_matrix_qt.h"

#include "ta_qtclipdata.h"

#include "ta_qt.h"
#include <QApplication>
#include <QClipboard>
#include <QTableView>
#include <QHBoxLayout>
#include <QVBoxLayout>

//////////////////////////
// tabMatrixViewType	//
//////////////////////////

int tabMatrixViewType::BidForView(TypeDef* td) {
  if (td->InheritsFrom(&TA_taMatrix))
    return (inherited::BidForView(td) +1);
  return 0;
}

/*taiDataLink* tabDataTableViewType::CreateDataLink_impl(taBase* data_) {
  return new tabListDataLink((taList_impl*)data_);
} */

void tabMatrixViewType::CreateDataPanel_impl(taiDataLink* dl_)
{
  // we create ours first, because it should be the default
  iMatrixPanel* dp = new iMatrixPanel(dl_);
  DataPanelCreated(dp);
  inherited::CreateDataPanel_impl(dl_);
}

//////////////////////////
//    iMatrixEditor 	//
//////////////////////////

iMatrixEditor::iMatrixEditor(QWidget* parent)
:inherited(parent)
{
  init();
}

void iMatrixEditor::init() {
  layOuter = new QVBoxLayout(this);
  layOuter->setMargin(2);
  layDims = new QHBoxLayout(layOuter);
  tv = new QTableView(this);
  layOuter->addWidget(tv);
  tv->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(tv, SIGNAL(customContextMenuRequested(const QPoint&)),
    this, SLOT(tv_customContextMenuRequested(const QPoint&)) );
}

MatrixTableModel* iMatrixEditor::model() const {
  return dynamic_cast<MatrixTableModel*>(tv->model());
}

void iMatrixEditor::Refresh() {
  //note: this also updates all other mat editors too, due to Model linking
  MatrixTableModel* mod = model();
  if (mod)
    mod->emit_dataChanged(); // default values mean entire table
}

void iMatrixEditor::setModel(MatrixTableModel* mod) {
  tv->setModel(mod);
}

int iMatrixEditor::QueryEditActions(taiMimeSource* ms) {
  int allowed = 0;
  int forbidden = 0;
  QueryEditActions_impl(ms, allowed, forbidden);
  return allowed & !forbidden;
}
void iMatrixEditor::QueryEditActions_impl(taiMimeSource* ms,
  int& allowed, int& forbidden)
{
  allowed = taiClipData::EA_COPY;
}

int iMatrixEditor::EditAction(int ea) {
  taiMimeSource* ms = NULL;
//TEMP
  return EditAction_impl(ms, ea);
}


int iMatrixEditor::EditAction_impl(taiMimeSource* ms, int ea) {
  if (ea & taiClipData::EA_COPY) {
    QMimeData* md = tv->model()->mimeData(tv->selectionModel()->selectedIndexes());
    QApplication::clipboard()->setMimeData(md, QClipboard::Clipboard);
  }
  return 0;
}

void iMatrixEditor::this_editAction(int ea) {
  EditAction(ea);
}

void iMatrixEditor::tv_customContextMenuRequested(const QPoint& pos) {
  taiMenu* menu = new taiMenu(this, taiMenu::normal, taiMisc::fonSmall);
  //TODO: any for us first (ex. delete)
  
    
// TEMP for test
  menu->AddItem("Copy", taiMenu::normal, taiAction::int_act,
    this, SLOT(this_editAction(int)), taiClipData::EA_COPY );

/*
  menu->AddSep();
  taiMenu* men_exp = menu->AddSubMenu("Expand/Collapse");
  men_exp->AddItem("Expand All", taiMenu::normal, taiAction::action,
    this, SLOT(ExpandAll()) );
  men_exp->AddItem("Collapse All", taiMenu::normal, taiAction::action,
    this, SLOT(CollapseAll()) );
  if (nd && lst.size == 1) {
    men_exp->AddItem("Expand All From Here", taiMenu::normal, taiAction::ptr_act,
      this, SLOT(ExpandAllUnderInt(void*)), (void*)nd );
    men_exp->AddItem("Collapse All From Here", taiMenu::normal, taiAction::ptr_act,
      this, SLOT(CollapseAllUnderInt(void*)), (void*)nd );
  }
*/
  if (menu->count() > 0) { //only show if any items!
    menu->exec(tv->mapToGlobal(pos));
  }
  delete menu;
}



//////////////////////////
//    iMatrixPanel 	//
//////////////////////////

iMatrixPanel::iMatrixPanel(taiDataLink* dl_)
:inherited(dl_)
{
  me = NULL;
}

iMatrixPanel::~iMatrixPanel() {
}

void iMatrixPanel::DataChanged_impl(int dcr, void* op1_, void* op2_) {
  inherited::DataChanged_impl(dcr, op1_, op2_);
  //NOTE: don't need to do anything because DataModel will handle it
//TODO: maybe we should do something less crude???
//  idt->updateConfig();
}

int iMatrixPanel::EditAction(int ea) {
  int rval = 0;

  ISelectable_PtrList sel_list;
  GetSelectedItems(sel_list);
  ISelectable* ci = sel_list.SafeEl(0);
  if (ci)  {
    rval = ci->EditAction_(sel_list, ea);
  }
  return rval;
}

int iMatrixPanel::GetEditActions() {
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

void iMatrixPanel::GetSelectedItems(ISelectable_PtrList& lst) {
/*TODO  QListViewItemIterator it(list, QListViewItemIterator::Selected);
  while (it.current()) {
    lst.Add((taiListDataNode*)it.current());
    ++it;
  } */
}

/*void iMatrixPanel::idt_contextMenuRequested(QListViewItem* item, const QPoint & pos, int col ) {
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

void iMatrixPanel::list_selectionChanged() {
  viewer_win()->UpdateUi();
}*/


String iMatrixPanel::panel_type() const {
  static String str("Edit Matrix");
  return str;
}

void iMatrixPanel::Refresh_impl() {
  if (me) me->Refresh();
  inherited::Refresh_impl();
}

void iMatrixPanel::Render_impl() {
  me = new iMatrixEditor();
  setCentralWidget(me); //sets parent
  taMatrix* mat_ = mat();
  if (mat_) {
    me->setModel(mat_->GetDataModel());
  }
}

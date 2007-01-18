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
#include "ta_datatable_qtso.h" // for factories
#include "ta_qt.h"
#include "ta_datatable_qtso.h" // for factories

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
//    iTableView 	//
//////////////////////////

iTableView::iTableView(QWidget* parent)
:inherited(parent)
{
  setContextMenuPolicy(Qt::CustomContextMenu);
  connect(this, SIGNAL(customContextMenuRequested(const QPoint&)),
    this, SLOT(this_customContextMenuRequested(const QPoint&)) );
  // wire the selection signals to the UpdateUi, to update enabling
//  connect(me->tv, SIGNAL(activated(const QModelIndex&)), me, SIGNAL(UpdateUi()) );
  connect(this, SIGNAL(clicked(const QModelIndex&)), this, SIGNAL(UpdateUi()) );
}



bool iTableView::event(QEvent* ev) {
//NOTE: this probably doesn't even get triggered
  bool rval = inherited::event(ev);
  // look for anything that indicates we are focused
  QEvent::Type t = ev->type();
  if ((t == QEvent::FocusIn) ||
    (t == QEvent::KeyPress) ||
    (t == QEvent::MouseButtonPress) //||
  ) 
    emit hasFocus(this);
  return rval;
}


void iTableView::FillContextMenu_impl(taiMenu* menu) {
  int ea = 0;
  GetEditActionsEnabled(ea);
    
  taiAction* 
  act = menu->AddItem("&Copy", taiMenu::normal, taiAction::int_act,
    this, SLOT(EditAction(int)), taiClipData::EA_COPY );
  act->setShortcut(QKeySequence("Ctrl+C"));
  if (!(ea & taiClipData::EA_COPY))
    act->setEnabled(false);
  act = menu->AddItem("&Paste", taiMenu::normal,
      taiAction::int_act, this, SLOT(EditAction(int)), taiClipData::EA_PASTE);
  act->setShortcut(QKeySequence("Ctrl+P"));
  if (!(ea & taiClipData::EA_PASTE)) 
    act->setEnabled(false);
  act = menu->AddItem("Clear", taiMenu::normal,
      taiAction::int_act, this, SLOT(EditAction(int)), taiClipData::EA_CLEAR);
  if (!(ea & taiClipData::EA_CLEAR)) 
    act->setEnabled(false);
    
  menu->AddSep();
  act = menu->AddItem("Select &All", taiAction::action,
    this, SLOT(selectAll()),_nilVariant,
      QKeySequence("Ctrl+A"));
}


void iTableView::this_customContextMenuRequested(const QPoint& pos) {
  taiMenu* menu = new taiMenu(this, taiMenu::normal, taiMisc::fonSmall);
  FillContextMenu_impl(menu);
  if (menu->count() > 0) { //only show if any items!
    menu->exec(mapToGlobal(pos));
  }
  delete menu;
}



//////////////////////////
//    iMatrixTableView 	//
//////////////////////////

iMatrixTableView::iMatrixTableView(QWidget* parent)
:inherited(parent)
{
}

taMatrix* iMatrixTableView::mat() const {
  MatrixTableModel* mod = qobject_cast<MatrixTableModel*>(model());
  if (mod) return mod->mat();
  else return NULL; 
}

void iMatrixTableView::EditAction(int ea) {
  taMatrix* mat = this->mat(); // may not have a model/mat!
  if (!mat) return;
  taiTabularDataMimeFactory* fact = taiTabularDataMimeFactory::instance();
  CellRange sel(selectionModel()->selectedIndexes());
  if (ea & taiClipData::EA_SRC_OPS) {
    fact->Mat_EditActionS(mat, sel, ea);
  } else {// dest op
    taiMimeSource* ms = taiMimeSource::NewFromClipboard();
    fact->Mat_EditActionD(mat, sel, ms, ea);
    delete ms;
  }
}

void iMatrixTableView::GetEditActionsEnabled(int& ea) {
  int allowed = 0;
  int forbidden = 0;
  taMatrix* mat = this->mat(); // may not have a model/mat!
  if (mat) {
    taiTabularDataMimeFactory* fact = taiTabularDataMimeFactory::instance();
    CellRange sel(selectionModel()->selectedIndexes());
    taiMimeSource* ms = taiMimeSource::NewFromClipboard();
    fact->Mat_QueryEditActions(mat, sel, ms, allowed, forbidden);
    delete ms;
  }
  ea = allowed & ~forbidden;
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
  tv = new iMatrixTableView(this);
  layOuter->addWidget(tv);
}

taMatrix* iMatrixEditor::mat() const {
  MatrixTableModel* mod = model();
  if (mod) return mod->mat();
  else return NULL; 
}

MatrixTableModel* iMatrixEditor::model() const {
  return qobject_cast<MatrixTableModel*>(tv->model());
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

void iMatrixPanel::tv_hasFocus(iTableView* sender) {
  iMainWindowViewer* vw = viewerWindow();
  if (vw)
    vw->SetClipboardHandler(sender,
    SLOT(GetEditActionsEnabled(int&)),
    SLOT(EditAction(int)),
    NULL,
    SIGNAL(UpdateUi()) );
}

String iMatrixPanel::panel_type() const {
  static String str("Edit Matrix");
  return str;
}

void iMatrixPanel::Refresh_impl() {
  if (me) me->Refresh();
  inherited::Refresh_impl();
}

void iMatrixPanel::Render_impl() {
  if (me) return; // shouldn't happen
  me = new iMatrixEditor();
  me->setName("MatrixEditor"); // diagnostic
  setCentralWidget(me); //sets parent
  taMatrix* mat_ = mat();
  if (mat_) {
    me->setModel(mat_->GetDataModel());
  }
  connect(me->tv, SIGNAL(hasFocus(iTableView*)), this, SLOT(tv_hasFocus(iTableView*)) );
}

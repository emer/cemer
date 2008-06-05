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


#include "ta_matrix_qt.h"

#include "ta_qtclipdata.h"
#include "ta_datatable_qtso.h" // for factories
#include "ta_qt.h"
#include "ta_datatable_qtso.h" // for factories

#include <QApplication>
#include <QClipboard>
#include <QHeaderView>
#include <QTableView>
#include <QHBoxLayout>
#include <QVBoxLayout>

//////////////////////////////////////////////////////////////////////////////
// 		 class MatrixTableModel
//  The MatrixTableModel provides a 2-d table model for TA Matrix objects.
//  For Gui editing, we map indexes in 2d flat table.

MatrixGeom MatrixTableModel::tgeom; 

MatrixTableModel::MatrixTableModel(taMatrix* mat_, QWidget* gui_parent_) 
:inherited(NULL)
{
  m_mat = mat_;
  if (m_mat) {
    m_mat->AddDataClient(this);
  }
  gui_parent = gui_parent_;
  m_pat_4d = false;
}

MatrixTableModel::~MatrixTableModel() {
  if (m_mat) {
    m_mat->RemoveDataClient(this);
    m_mat = NULL;
  }
}

int MatrixTableModel::columnCount(const QModelIndex& parent) const {
  if (!m_mat) return 0;
  return m_mat->geom.colCount(pat4D());
}

QVariant MatrixTableModel::data(const QModelIndex& index, int role) const {
  if (!m_mat) return QVariant();
  switch (role) {
  case Qt::DisplayRole: 
  case Qt::EditRole:
    return m_mat->SafeElAsStr_Flat(matIndex(index));
//Qt::DecorationRole
//Qt::ToolTipRole
//Qt::StatusTipRole
//Qt::WhatsThisRole
//Qt::SizeHintRole -- QSize
//Qt::FontRole--  QFont: font for the text
  case Qt::TextAlignmentRole:
    return m_mat->defAlignment();
  case Qt::BackgroundColorRole : //-- QColor
 /* note: only used when !(option.showDecorationSelected && (option.state
    & QStyle::State_Selected)) */
    if (!(flags(index) & Qt::ItemIsEditable))
      return QColor(COLOR_RO_BACKGROUND);
    break;
/*Qt::TextColorRole
  QColor: color of text
Qt::CheckStateRole*/
  default: break;
  }
  return QVariant();
}

void MatrixTableModel::DataLinkDestroying(taDataLink* dl) {
  m_mat = NULL;
}

void MatrixTableModel::DataDataChanged(taDataLink* dl, int dcr,
  void* op1, void* op2)
{
  //this is primarily for code-driven changes
  if (dcr <= DCR_ITEM_UPDATED_ND) {
    emit_dataChanged();
  }
  else if ((dcr == DCR_STRUCT_UPDATE_END)) {
    emit_layoutChanged();
  }
}


void MatrixTableModel::emit_dataChanged(int row_fr, int col_fr, int row_to, int col_to) {
  if (!m_mat) return;
  // lookup actual end values when we are called with sentinels
  if (row_to < 0) row_to = rowCount() - 1;
  if (col_to < 0) col_to = columnCount() - 1;  
  
  emit dataChanged(createIndex(row_fr, col_fr), createIndex(row_to, col_to));
}

void MatrixTableModel::emit_layoutChanged() {
  emit layoutChanged();
}

Qt::ItemFlags MatrixTableModel::flags(const QModelIndex& index) const {
  if (!m_mat) return 0;
  //TODO: maybe need to qualify!, plus drag-drop handling, etc.
  Qt::ItemFlags rval = 0;
  
  if (ValidateIndex(index)) {
    rval = Qt::ItemIsSelectable | Qt::ItemIsEnabled;
  }
  // editability is a property of the whole mat
  if (!m_mat->isGuiReadOnly()) {
    rval |= Qt::ItemIsEditable;
  }
  return rval; 
}
/*
  index = i0 + ((i1*d0) + i2)*d1 etc.
to find i1,i2... from index:
1. divide by d0 gives rowframe -- remainder is i1
2. divide by d1 gives 2d-frame
*/
QVariant MatrixTableModel::headerData(int section, 
  Qt::Orientation orientation, int role) const
{
  if (role != Qt::DisplayRole)
    return QVariant();
  // get an effective pat4D guaranteed true only if applicable
  bool pat_4d = (pat4D() && (m_mat->dims() >= 4));
  if (orientation == Qt::Horizontal) {
    if (pat_4d) {
      // need to break the flat col down 
      div_t qr = div(section, m_mat->dim(0));
      // d0:d2
      return QString("u%1:g%2").arg(qr.rem).arg(qr.quot);
    } else {
      return QString::number(section); //QString("%1").arg(section);
    }
  } else {// in form: d1[[:d2]:d3]
    int eff_row;
    switch (matView()) {
    case taMisc::BOT_ZERO: eff_row = m_mat->rowCount(pat_4d) - section - 1; break;
    default /*case taMisc::TOP_ZERO*/: eff_row = section; break;
    }
    if (m_mat->dims() <= 2) {
      return QString::number(eff_row);
    } else {
      const int cc = m_mat->colCount(pat_4d);
      int row_flat_idx = eff_row * cc;
      MatrixGeom coords;
      m_mat->geom.DimsFmIndex(row_flat_idx, coords);
      QString rval;
      int j;
      if (pat_4d) {
        // d1:d3...
        rval = QString("u%1:g%2").arg(coords[1]).arg(coords[3]);
        j = 4;
      } else {
        // d1...
        rval = QString("%1").arg(coords[1]);
        j = 2;
      }
      for (int i = j; i < coords.size; ++i) {
        rval += QString(":%1").arg(coords[i]);
      }
      
/*obs      int d_this = m_mat->colCount(pat_4d);
      div_t r;
      // find each nextmost dim n by doing modulo remaining dim[n-1]
      for (int i = (pat_4d) ? 2:1; i < (m_mat->dims() - 1); ++i) {
        r = div(eff_row, d_this); 
        eff_row = r.quot;
        if (i > 1) rval = ":" + rval;
        rval = String(r.rem) + rval; 
        d_this = m_mat->geom.SafeEl(i+1); // for next iter, but safe in case done
      }
      rval = String(eff_row) + ":"  + rval; */
      return rval;
    }
  }
}

bool MatrixTableModel::ignoreDataChanged() const {
  return (gui_parent && !gui_parent->isVisible());
}

int MatrixTableModel::matIndex(const QModelIndex& idx) const {
  //note: we dimensionally reduce all dims >1 to 1
  return m_mat->geom.IndexFmDims2D(idx.column(), idx.row(), pat4D(), matView());
}

QMimeData* MatrixTableModel::mimeData (const QModelIndexList& indexes) const {
  if (!m_mat) return NULL;
  CellRange cr(indexes);
  String str = mat()->FlatRangeToTSV(cr);
  QMimeData* rval = new QMimeData;
  rval->setText(str);
  return rval;
}

QStringList MatrixTableModel::mimeTypes () const {
  QStringList types;
  types << "text/plain";
  return types;
}


int MatrixTableModel::rowCount(const QModelIndex& parent) const {
  return m_mat->rowCount(pat4D());
  //note: for visual stuff, there is always at least one row
}

bool MatrixTableModel::setData(const QModelIndex& index, const QVariant & value, int role) {
  if (!m_mat) return false;
  if (index.isValid() && role == Qt::EditRole) {
    m_mat->SetFmStr_Flat(value.toString(), matIndex(index));
    emit dataChanged(index, index);
    return true;
  }
  return false;
}

void MatrixTableModel::setPat4D(bool val, bool notify) {
  if (m_pat_4d == val) return;
  m_pat_4d = val;
  if (notify) 
    emit_layoutChanged();
}


bool MatrixTableModel::ValidateIndex(const QModelIndex& index) const {
  // TODO: maybe need to check bounds???
  return (m_mat);
}

bool MatrixTableModel::ValidateTranslateIndex(const QModelIndex& index, MatrixGeom& tr_index) const {
  bool rval = ValidateIndex(index);
  if (rval) {
    // TODO:
  }
  return rval;
}

taMisc::MatrixView MatrixTableModel::matView() const {
  return taMisc::matrix_view;
}

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
  horizontalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(horizontalHeader(), SIGNAL(customContextMenuRequested(const QPoint&)),
    this, SLOT(hor_customContextMenuRequested(const QPoint&)) );
  verticalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(verticalHeader(), SIGNAL(customContextMenuRequested(const QPoint&)),
    this, SLOT(ver_customContextMenuRequested(const QPoint&)) );
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

void iTableView::ContextMenuRequested(ContextArea ca, const QPoint& global_pos) {
  taiMenu* menu = new taiMenu(this, taiMenu::normal, taiMisc::fonSmall);
  CellRange sel(selectionModel()->selectedIndexes());
  FillContextMenu_impl(ca, menu, sel);
  if (menu->count() > 0) { //only show if any items!
    menu->exec(global_pos);
  }
  delete menu;
}

void iTableView::FillContextMenu_impl(ContextArea ca,
  taiMenu* menu, const CellRange& sel)
{
  taiAction* act = NULL;
  // generic col guys
  if (ca == CA_COL_HDR) {
    if (!isFixedColCount()) {
//note: not include these yet, because fairly complicated semantics, ex
// popping up col editor etc. etc.
/*      act = menu->AddItem("Append Columns", taiMenu::normal, taiAction::int_act,
        this, SLOT(RowColOp(int)), (OP_COL | OP_APPEND) );
      act = menu->AddItem("Insert Columns", taiMenu::normal, taiAction::int_act,
        this, SLOT(RowColOp(int)), (OP_COL | OP_INSERT) );*/
      act = menu->AddItem("Delete Columns", taiMenu::normal, taiAction::int_act,
        this, SLOT(RowColOp(int)), (OP_COL | OP_DELETE) );
      menu->AddSep();
    }
  }
  
  // generic row guys
  if (ca == CA_ROW_HDR) {
    if (!isFixedRowCount()) {
      act = menu->AddItem("Append Rows", taiMenu::normal, taiAction::int_act,
        this, SLOT(RowColOp(int)), (OP_ROW | OP_APPEND) );
      act = menu->AddItem("Insert Rows", taiMenu::normal, taiAction::int_act,
        this, SLOT(RowColOp(int)), (OP_ROW | OP_INSERT) );
      act = menu->AddItem("Delete Rows", taiMenu::normal, taiAction::int_act,
        this, SLOT(RowColOp(int)), (OP_ROW | OP_DELETE) );
    }
    menu->AddSep();
  }
  
  // edit guys
  int ea = 0;
  GetEditActionsEnabled(ea);
    
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

void iTableView::RowColOp(int op_code) {
  CellRange sel(selectionModel()->selectedIndexes());
  RowColOp_impl(op_code, sel);
}

void iTableView::this_customContextMenuRequested(const QPoint& pos) {
  ContextArea ca = CA_GRID; // TODO: determine if in blank
  ContextMenuRequested(ca, mapToGlobal(pos));
}

void iTableView::hor_customContextMenuRequested(const QPoint& pos) {
  ContextArea ca = CA_COL_HDR; // TODO: determine if in blank
  ContextMenuRequested(ca, horizontalHeader()->mapToGlobal(pos));
}

void iTableView::ver_customContextMenuRequested(const QPoint& pos) {
  ContextArea ca = CA_ROW_HDR; // TODO: determine if in blank
  ContextMenuRequested(ca, verticalHeader()->mapToGlobal(pos));
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
  CellRange sel;
  GetSel(sel);
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
    CellRange sel;
    GetSel(sel);
    taiMimeSource* ms = taiMimeSource::NewFromClipboard();
    fact->Mat_QueryEditActions(mat, sel, ms, allowed, forbidden);
    delete ms;
  }
  ea = allowed & ~forbidden;
}

void iMatrixTableView::GetSel(CellRange& sel) {
  MatrixTableModel* mod = qobject_cast<MatrixTableModel*>(model());
  if (!mod) return;
  // first, get the sel assuming no BOT_0
  sel.SetFromModel(selectionModel()->selectedIndexes());
  // if BOT_0, need to flip the row around
  if (mod->matView() == taMisc::BOT_ZERO) {
    int max_row = mod->rowCount() - 1;
    if ((max_row) < 0) return; // row_xx s/already be 0
    sel.row_fr = max_row - sel.row_fr;
    sel.row_to = max_row - sel.row_to;
  }
}

bool iMatrixTableView::isFixedRowCount() const {
  taMatrix* mat = this->mat(); // may not have a model/mat!
  if (!mat) return true;
  return (!mat->canResize());
}


//////////////////////////
//    iMatrixEditor 	//
//////////////////////////

iMatrixEditor::iMatrixEditor(QWidget* parent)
:inherited(parent)
{
  init();
}

iMatrixEditor::~iMatrixEditor() {
  if (m_model) {
    delete m_model;
    m_model = NULL;
  }
}

void iMatrixEditor::init() {
  m_model = NULL;
  layOuter = new QVBoxLayout(this);
  layOuter->setMargin(2); layOuter->setSpacing(2);
//  layDims = new QHBoxLayout(layOuter);
  layDims = new QHBoxLayout; layOuter->addLayout(layDims);
  tv = new iMatrixTableView(this);
  layOuter->addWidget(tv);
}

void iMatrixEditor::Refresh() {
  MatrixTableModel* mod = model();
  if (mod)
    mod->emit_layoutChanged(); // default values mean entire table
}

void iMatrixEditor::setMatrix(taMatrix* mat_) {
  if (m_mat == mat_) return;
  if (m_model) {
    delete m_model;
    m_model = NULL;
  }
  m_mat = mat_;
  if (mat_){
    m_model = new MatrixTableModel(mat_, this);
  }
  tv->setModel(m_model);
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

void iMatrixPanel::UpdatePanel_impl() {
  if (me) me->Refresh();
  inherited::UpdatePanel_impl();
}

void iMatrixPanel::Render_impl() {
  if (me) return; // shouldn't happen
  me = new iMatrixEditor();
  me->setObjectName("MatrixEditor"); // diagnostic
  setCentralWidget(me); //sets parent
  taMatrix* mat_ = mat();
  me->setMatrix(mat_);
  connect(me->tv, SIGNAL(hasFocus(iTableView*)), this, SLOT(tv_hasFocus(iTableView*)) );
}

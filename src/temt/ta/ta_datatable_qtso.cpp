// Carnegie Mellon University, Princeton University.
// Copyright, 1995-2007, Regents of the University of Colorado,
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



#include "ta_datatable_qtso.h"

// stuff to implement graphical view of datatable
#include "igeometry.h"

#include "ta_qtgroup.h"
#include "ta_qtclipdata.h"

#include "ta_datatable_so.h"
#include "ta_math.h"
#include "ta_project.h"

#include "ilineedit.h"
#include "ispinbox.h"
#include "iscrollarea.h"
#include "iflowlayout.h"
#include "NewNetViewHelper.h"

#include <QApplication>
#include <QButtonGroup>
#include <QCheckBox>
#include <QClipboard>
#include <qimage.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qpalette.h>
#include <qpixmap.h>
#include <QPushButton>
#include <QTableView>
#include <QTextStream>
#include <QHeaderView>

#include <Inventor/SbLinear.h>
#include <Inventor/fields/SoMFString.h>
#include <Inventor/nodes/SoAsciiText.h>
#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoDirectionalLight.h>
#include <Inventor/nodes/SoFont.h>
#include <Inventor/nodes/SoLightModel.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/nodes/SoSelection.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoComplexity.h>
#include <Inventor/nodes/SoText2.h>
#include <Inventor/draggers/SoTransformBoxDragger.h>
#include <Inventor/nodes/SoEventCallback.h>
#include <Inventor/events/SoMouseButtonEvent.h>
#include <Inventor/actions/SoRayPickAction.h>
#include <Inventor/SoPickedPoint.h>
#include <Inventor/SoEventManager.h>

#include <limits.h>
#include <float.h>

#define DIST(x,y) sqrt((double) ((x * x) + (y*y)))

//////////////////////////////////
//   DataTableModel		//
//////////////////////////////////

DataTableModel::DataTableModel(DataTable* dt_) 
:inherited(NULL)
{
  m_dt = dt_;
}

DataTableModel::~DataTableModel() {
  // note: following shouldn't really execute since table manages our lifetime
  if (m_dt) {
    m_dt->RemoveDataClient(this);
    m_dt = NULL;
  }
}

int DataTableModel::columnCount(const QModelIndex& parent) const {
  return (m_dt) ? m_dt->cols() : 0;
}

void DataTableModel::DataLinkDestroying(taDataLink* dl) {
  m_dt = NULL;
}

void DataTableModel::DataDataChanged(taDataLink* dl, int dcr,
  void* op1, void* op2)
{ // called from DataTable::DataChanged
  if (notifying) return;
  //this is primarily for code-driven changes
  if ((dcr <= DCR_ITEM_UPDATED_ND) || // data itself updated
    (dcr == DCR_STRUCT_UPDATE_END) ||  // for col insert/deletes
    (dcr == DCR_DATA_UPDATE_END)) // for row insert/deletes
  { 
    emit_layoutChanged();
  }
}

QVariant DataTableModel::data(const QModelIndex& index, int role) const {
  if (!m_dt || !index.isValid()) return QVariant();
  //NOTES:
  // * it would be nice to just italicize the "matrix" text, but we have no
  //   no access to the font being used, and cannot only pass modifiers
  
  DataCol* col = m_dt->GetColData(index.column(), true); // quiet
  // if no col, we really don't care about anything else...
  if (!col) return QVariant(); // nil
  
  switch (role) {
  case Qt::DisplayRole: //note: we may choose to format different for display, ex floats
  case Qt::EditRole: {
    if (col->is_matrix) 
      return QVariant("(matrix)"); // user clicks to edit, or elsewise displayed
    else {
      int dx;
      if(m_dt->idx(index.row(), col->rows(), dx))
	return col->GetValAsString(dx);
      else
	return QVariant();	// nil
    }
  }
// Qt::FontRole: //  QFont: font for the text
//Qt::DecorationRole
//Qt::ToolTipRole
//Qt::StatusTipRole
//Qt::WhatsThisRole
//Qt::SizeHintRole -- QSize
//Qt::FontRole--  QFont: font for the text
  case Qt::TextAlignmentRole: {
    if (col->is_matrix)
      return QVariant(Qt::AlignCenter | Qt::AlignVCenter);
    else if (col->isNumeric())
      return QVariant(Qt::AlignRight | Qt::AlignVCenter);
    else
      return QVariant(Qt::AlignLeft | Qt::AlignVCenter);
    } break;
  case Qt::BackgroundColorRole : //-- QColor
 /* note: only used when !(option.showDecorationSelected && (option.state
    & QStyle::State_Selected)) */
    // note: only make it actual ro color if ro (not for "(matrix)" cells)
    if ((col->col_flags & DataCol::READ_ONLY) || col->isGuiReadOnly())
      return QColor(COLOR_RO_BACKGROUND);
    break;
  case Qt::TextColorRole: { // QColor: color of text
    if (col->is_matrix)
      return QColor(Qt::blue);
    } break;
//Qt::CheckStateRole
  default: break;
  }
  return QVariant();
}

void DataTableModel::emit_dataChanged(int row_fr, int col_fr, int row_to, int col_to) {
  if (!m_dt) return;
  // lookup actual end values when we are called with sentinels
  if (row_to < 0) row_to = rowCount() - 1;
  if (col_to < 0) col_to = columnCount() - 1;  
  
  emit dataChanged(createIndex(row_fr, col_fr), createIndex(row_to, col_to));
}

void DataTableModel::emit_dataChanged(const QModelIndex& topLeft,
    const QModelIndex& bottomRight)
{
  if (!m_dt) return;
  ++notifying;
  emit dataChanged(topLeft, bottomRight);
  --notifying;
}

void DataTableModel::emit_layoutChanged() {
  if (!m_dt) return;
  ++notifying;
  emit layoutChanged();
  --notifying;
}

Qt::ItemFlags DataTableModel::flags(const QModelIndex& index) const {
  if (!m_dt || !index.isValid()) return 0;
  Qt::ItemFlags rval = 0;
  if (ValidateIndex(index)) {
    // don't enable null cells
    if (m_dt->hasData(index.column(), index.row() )) {
      rval = Qt::ItemIsSelectable | Qt::ItemIsEnabled;
      DataCol* col = m_dt->GetColData(index.column(), true); // quiet
      if (col && !(col->is_matrix || (col->col_flags & DataCol::READ_ONLY) ||
         col->isGuiReadOnly()) ) 
        rval |= Qt::ItemIsEditable;
    }
  }
  return rval;
}

QVariant DataTableModel::headerData(int section, Qt::Orientation orientation,
  int role) const 
{
  if (m_dt) {
    if (orientation == Qt::Horizontal) {
      DataCol* col = m_dt->GetColData(section, true); // quiet
      if (col) { 
        if (role == Qt::DisplayRole)
          return QString(col->GetDisplayName().chars());
        else if (role == Qt::ToolTipRole)
          return QString(col->GetDesc().chars());
      } else 
        return QString();
    } else {
      if (role == Qt::DisplayRole)
        return QString::number(section);
    }
  }
  return QVariant();
}

void DataTableModel::refreshViews() {
  emit_layoutChanged();
/*  emit dataChanged(createIndex(0, 0), 
    createIndex(rowCount() - 1, columnCount() - 1));*/
}

int DataTableModel::rowCount(const QModelIndex& parent) const {
  return (m_dt) ? m_dt->rows : 0;
}

void DataTableModel::matDataChanged(int col_idx) {
  if (!m_dt) return;
  
  DataCol* col = m_dt->GetColData(col_idx, true); // quiet
  // if no col, we really don't care about anything else...
  if (!col) return; 
  ++notifying;
  col->DataChanged(DCR_ITEM_UPDATED); // for calc refresh and other clients
  --notifying;
}

bool DataTableModel::setData(const QModelIndex& index, const QVariant & value, int role) {
  if (!m_dt || !index.isValid()) return false;
  
  DataCol* col = m_dt->GetColData(index.column(), true); // quiet
  // if no col, we really don't care about anything else...
  if (!col) return false; 
  //we restrict setData for scalars only -- use delegate for matrix
  if (col->is_matrix) return false;
  
  bool rval = false;
  switch (role) {
  case Qt::EditRole:
    m_dt->SetValAsVar(value, index.column(), index.row());
    ++notifying;
    emit_dataChanged(index, index);
    col->DataChanged(DCR_ITEM_UPDATED); // for calc refresh
    --notifying;
    rval = true;
  default: break;
  }
  return rval;
}

bool DataTableModel::ValidateIndex(const QModelIndex& index) const {
  if (!m_dt) return false;
  return (index.isValid() && (index.row() < m_dt->rows) && (index.column() < m_dt->cols()));
}


//////////////////////////
//   DataTable (gui)	//
//////////////////////////

GridTableView* DataTable::NewGridView(T3DataViewFrame* fr) {
  return GridTableView::New(this, fr);
}

GraphTableView* DataTable::NewGraphView(T3DataViewFrame* fr) {
  return GraphTableView::New(this, fr);
}


GridTableView* DataTable::FindMakeGridView(T3DataViewFrame* fr) {
  taDataLink* dl = data_link();
  if(dl) {
    taDataLinkItr itr;
    GridTableView* el;
    FOR_DLC_EL_OF_TYPE(GridTableView, el, dl, itr) {
      // update from user stuff
      el->InitFromUserData();
      el->InitDisplay();
      el->UpdateDisplay();
      fr = el->GetFrame();
      if(fr) {
	MainWindowViewer* mwv = GET_OWNER(fr, MainWindowViewer);
	if(mwv) {
	  mwv->SelectT3ViewTabName(fr->name);
	}
      }
      return el;
    }
  }

  return GridTableView::New(this, fr);
}

GraphTableView* DataTable::FindMakeGraphView(T3DataViewFrame* fr) {
  taDataLink* dl = data_link();
  if(dl) {
    taDataLinkItr itr;
    GraphTableView* el;
    FOR_DLC_EL_OF_TYPE(GraphTableView, el, dl, itr) {
      // update from user stuff
      el->InitFromUserData();
      el->InitDisplay();
      el->UpdateDisplay();
      fr = el->GetFrame();
      if(fr) {
	MainWindowViewer* mwv = GET_OWNER(fr, MainWindowViewer);
	if(mwv) {
	  mwv->SelectT3ViewTabName(fr->name);
	}
      }
      return el;
    }
  }
  return GraphTableView::New(this, fr);
}

GridTableView* DataTable::FindGridView() {
  taDataLink* dl = data_link();
  if(dl) {
    taDataLinkItr itr;
    GridTableView* el;
    FOR_DLC_EL_OF_TYPE(GridTableView, el, dl, itr) {
      return el;
    }
  }
  return NULL;
}

GraphTableView* DataTable::FindGraphView() {
  taDataLink* dl = data_link();
  if(dl) {
    taDataLinkItr itr;
    GraphTableView* el;
    FOR_DLC_EL_OF_TYPE(GraphTableView, el, dl, itr) {
      return el;
    }
  }
  return NULL;
}

bool DataTable::GridViewGotoRow(int row_no) {
  GridTableView* gv = FindGridView();
  if(!gv) return false;
  gv->ViewRow_At(row_no);
  return true;
}

bool DataTable::GraphViewGotoRow(int row_no) {
  GraphTableView* gv = FindGraphView();
  if(!gv) return false;
  gv->ViewRow_At(row_no);
  return true;
}

//////////////////////////
//   DataColView	//
//////////////////////////

/*
  The main DataColView operations are:
  * initialize ("bind") a new guy from a DataCol
  * update a bound guy (ex things change in the col)
  * unlink a guy (ex. table unlinking)
  * delete a guy (ex. col deletes in table (but table not deleting)
  * bind a guy to a new col (ex., binding to a new table)
  * calculate view-specific parameters based on current state
    -- this is done in Render_impl
*/

void DataColView::Initialize(){
  visible = true;
//   sticky = false;
  data_base = &TA_DataCol;
}

void DataColView::Copy_(const DataColView& cp) {
  visible = cp.visible;
//   sticky = cp.sticky;
}

void DataColView::Destroy() {
  CutLinks();
}

void DataColView::Unbind_impl() {
  if (m_data) setDataCol(NULL);
  inherited::Unbind_impl();
}

void DataColView::DataDestroying() {
  DataColUnlinked();
  inherited::DataDestroying();
}

void DataColView::setDataCol(DataCol* value, bool first_time) {
  if (dataCol() == value) return;
  SetData(value);
  if (value) {
    UpdateFromDataCol(first_time);
  } else {
    DataColUnlinked();
  }
}

void DataColView::UpdateFromDataCol(bool first) {
  UpdateFromDataCol_impl(first);
  DataChanged(DCR_ITEM_UPDATED);
}

void DataColView::UpdateFromDataCol_impl(bool first) {
  DataCol* col = dataCol();
  if (!name.contains(col->name)) {
    SetName(col->name);
  }
  // only copy display options first time, since user may override in view
  if (first) {
    if (col->GetUserData(DataCol::udkey_hidden).toBool())
      visible = false;
  }
}

bool DataColView::isVisible() const {
  return (visible && (bool)m_data);
}

void DataColView::Hide() {
  visible = false;
  DataTableView* par = parent();
  //  par->InitDisplay();
  par->UpdateDisplay();
}

//////////////////////////
// DataTableView	//
//////////////////////////

void DataTableView::Initialize() {
  data_base = &TA_DataTable;
  view_rows = 3; //note: set by actual class based on screen
//obs  view_shift = .2f;
  view_range.min = 0;
  view_range.max = -1;
  display_on = true;
  manip_ctrl_on = true;

  main_xform.translate.x = 1.0f;

  updating = 0;
  m_lvp = NULL;
  m_rows = 0;

  children.SetBaseType(&TA_DataColView); // subclasses need to set this to their type!
}

void DataTableView::InitLinks() {
  inherited::InitLinks();
  taBase::Own(view_range, this);
}

void DataTableView::CutLinks() {
  view_range.CutLinks();
  inherited::CutLinks();
}

void DataTableView::Copy_(const DataTableView& cp) {
  view_rows = cp.view_rows;
  view_range = cp.view_range;
  display_on = cp.display_on;
  manip_ctrl_on = cp.manip_ctrl_on;
}

void DataTableView::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  //note: UAE calls setDirty, which is where we do most of the rejigging
}

void DataTableView::IgnoredDataChanged(taDataLink* dl, int dcr,
    void* op1, void* op2) 
{
  // if it is a struct begin or end, we better not ignore it, because
  // it could require us to reset, so we free locked matrix col slices
  // this could reduce our invisible efficiency BUT is necessary!!!
  if ((dcr == DCR_STRUCT_UPDATE_BEGIN) ||
      (dcr == DCR_STRUCT_UPDATE_END))
  {
    DataDataChanged(dl, dcr, op1, op2);
  }
}

const String DataTableView::caption() const {
  DataTable* dt = dataTable(); 
  String rval;
  if (dt) {
    rval = dt->GetDisplayName();
  } else rval = "(no table)";
  return rval;
}

void DataTableView::ClearData() {
  m_rows = 0;
  if (dataTable()) {
    dataTable()->ResetData();
  }
  ClearViewRange();
  InitDisplay();
  UpdateDisplay();
}

void DataTableView::ClearViewRange() {
  view_range.min = 0;
  view_range.max = -1; // gets adjusted later
}

void DataTableView::Unbind_impl() {
  if (m_data) { 
    SetData(NULL);
    DataTableUnlinked();
  }
  inherited::Unbind_impl(); // will unlink kids
}

void DataTableView::setDataTable(DataTable* dt) {
  if (dataTable() == dt) return;
  if (dt) {
    SetData(dt);
    bool first = (colViewCount() == 0);
    int u_view_rows = dt->GetUserDataAsInt("VIEW_ROWS");
    if (u_view_rows > 0) view_rows = u_view_rows;
    UpdateFromDataTable(first);
    T3DataViewFrame* frame = GetFrame();
    if(frame) {
      main_xform.translate.y = 1.3f * (frame->root_view.children.size - 1); // move to unique position (up)
    }
    //TEST
    UpdatePanel(); // to update name
  } else {
    Unbind(); // also does kids
  }
}

void DataTableView::InitPanel() {
  if (m_lvp)
    m_lvp->InitPanel();
}

void DataTableView::UpdatePanel() {
  if (m_lvp)
    m_lvp->UpdatePanel();
}

bool DataTableView::isVisible() const {
  return (taMisc::gui_active && isMapped());
}

void DataTableView::MakeViewRangeValid() {
  int rows = this->rows();
  if (view_range.min >= rows) {
    view_range.min = MAX(0, (rows - view_rows - 1));
  }
  if(view_rows >= rows) // start at start if we can view whole thing
    view_range.min = 0;
  view_range.max = view_range.min + view_rows - 1; // always keep min and max valid
  view_range.MaxLT(rows - 1); // keep it less than max
}

int DataTableView::CheckRowsChanged(int& orig_rows) {
  DataTable* data_table = dataTable();
  int rval = 0;
  if(!data_table) {
    rval = -m_rows;
    m_rows = 0;
    return rval;
  }
  orig_rows = m_rows;
  rval = data_table->rows - orig_rows;
  m_rows = data_table->rows;
  return rval;
}

void DataTableView::DataUpdateView_impl() {
  if(!display_on) return;
  UpdateDisplay(true);
}

void DataTableView::DoActionChildren_impl(DataViewAction acts) {
  if(acts & RESET_IMPL) return;
  // don't do the reset action on children -- updatefromdatatable does that
  inherited::DoActionChildren_impl(acts);
}


void DataTableView::DataDestroying() {
  Unbind(); //unlinks everyone
  inherited::DataDestroying();
}

void DataTableView::DataTableUnlinked() {
}

void DataTableView::UpdateFromDataTable(bool first) {
  UpdateFromDataTable_child(first);
  UpdateFromDataTable_this(first);
}

void DataTableView::UpdateName() {
  DataTable* dt = dataTable();
  if (dt) {
    if (!name.contains(dt->name))
      SetName(dt->name);
  }
  else {
    if (name.empty())
      SetName("no_table");
  }
}
  
void DataTableView::DataUpdateAfterEdit_impl() {
  UpdateName();
  inherited::DataUpdateAfterEdit_impl();
}


void DataTableView::UpdateFromDataTable_this(bool first) {
  UpdateName();
}

void DataTableView::UpdateFromDataTable_child(bool first) {
  DataTableCols* cols = &(dataTable()->data);
  DataColView* dcs = NULL;
  DataCol* dc = NULL;
  int i;
/*TODO: revise algorithm as follows:
(may require intermediate link list, to track guys)
1. identify potential orphans, put them aside
2. update existing guys
3. attempt to rebind missing guys to potential orphans
  ex. if cols are in same position, then probably just name difference
4. delete true orphans
5. add missing guys
ALSO: need to probably revise the scheme for reordering -- maybe user
  wants the data in the new order? (OR: maybe should provide separate
  "view_index" in the spec, so user can reorder)
*/
//  delete orphans (except sticky guys)
  for (i = colViewCount() - 1; i >= 0; --i) {
    dcs = colView(i);
    // first, see if it is bound to our table, if so, just update it (name may have changed, etc.
    int old_colno = cols->FindEl(dcs->dataCol());
    if (old_colno >= 0) {
      if(old_colno != i)
	dcs->UpdateFromDataCol(true); // first = get options
      else
	dcs->UpdateFromDataCol(false); // no options
      continue;
    }
    // if not bound, try to find it by name
    dc = cols->FindName(dcs->GetName());
    if (dc) {
      // make sure it is this col bound to the guy!
      dcs->setDataCol(dc);
    } else { // no target found in table
//       if (dcs->sticky) {
//         dcs->setDataCol(NULL); //keep him, but unbind from any col
//       } else {
        children.RemoveIdx(i);
//       }
    }
  }  
// items: add missing, order correctly, and update existing (will be only action 1st time)
  for (i = 0; i < cols->size; ++i) {
    dc = cols->FastEl(i);
    bool first = false;
    int fm = children.FindNameIdx(dc->GetName());
    if (fm >= 0) {
      dcs = (DataColView*)children.FastEl(fm);
      if (fm != i) children.MoveIdx(fm, i);
    } else {
      first = true;
      dcs = (DataColView*)taBase::MakeToken(children.el_typ); // of correct type for this
      children.Insert(dcs, i);
    }
    dcs->setDataCol(dc, first);
  }  
//   cerr << "data spec cols updated: " << cols->size << " " << colViewCount() << endl;
}

void DataTableView::BuildAll() {
  UpdateFromDataTable();
}

void DataTableView::Render_pre() {
  if (!node_so()) return; // shouldn't happen
  InitPanel();
  inherited::Render_pre();
}

void DataTableView::Render_impl() {
  // set origin: in allocated area
  FloatTransform* ft = transform(true);
  *ft = main_xform;

  inherited::Render_impl();

  T3Node* node_so = this->node_so(); // cache
  if(!node_so) return;
  SoFont* font = node_so->captionFont(true);
  float font_size = 0.3f;
  font->size.setValue(font_size); // is in same units as geometry units of network
  node_so->setCaption(caption().chars());
  //NOTE: will need to move caption appropriately in subclass
}

void DataTableView::Render_post() {
  inherited::Render_post();
}

void DataTableView::Reset_impl() {
//   ClearViewRange();
  inherited::Reset_impl();
}

void DataTableView::setDirty(bool value) {
  inherited::setDirty(value);
  // this is a bad idea: if the view specs themselves are being updated,
  // it can be for many reasons including rebuilding from original data
  // so we should not do anything here..
  //  if (value) UpdateDisplay();
}

void DataTableView::setDisplay(bool value) {
  if (display_on == value) return;
  display_on = value;
  //  UpdateAfterEdit(); // does the whole kahuna
  UpdateDisplay(false);		// 
}

void DataTableView::ViewRow_At(int start) {
  if (!taMisc::gui_active) return;
  int rows = this->rows();
  if (start >= rows)
    start = rows - 1;
  if (start < 0)
    start = 0;
  view_range.min = start;
  UpdateDisplay();
}

void DataTableView::RowBackAll() {
  ViewRow_At(0);
}
void DataTableView::RowBackPg() {
  int cur_row = view_range.min;
  int goto_row = cur_row - view_rows;
  goto_row = MAX(0, goto_row);
  ViewRow_At(goto_row);
}
void DataTableView::RowBack1() {
  int cur_row = view_range.min;
  int goto_row = cur_row - 1;
  goto_row = MAX(0, goto_row);
  ViewRow_At(goto_row);
}
void DataTableView::RowFwd1() {
  int cur_row = view_range.min;
  int goto_row = cur_row + 1;
  goto_row = MIN(rows()-view_rows, goto_row);

  ViewRow_At(goto_row);
}
void DataTableView::RowFwdPg() {
  int cur_row = view_range.min;
  int goto_row = cur_row + view_rows;
  goto_row = MIN(rows()-view_rows, goto_row);

  ViewRow_At(goto_row);
}
void DataTableView::RowFwdAll() {
  ViewRow_At(rows() - view_rows);
}

///////////////////////////////////////////////////////////////////////////////
//  	Grid View

//////////////////////////////////
//   GridColView		//
//////////////////////////////////

void GridColView::Initialize(){
  text_width = 8;
  scale_on = true;
  mat_layout = taMisc::BOT_ZERO; // typical default for data patterns
  mat_image = false;
  mat_odd_vert = true;
  col_width = 0.0f;
  row_height = 0.0f;
}

void GridColView::Copy_(const GridColView& cp){
  text_width = cp.text_width;
  scale_on = cp.scale_on;
  mat_layout = cp.mat_layout;
  mat_image = cp.mat_image;
  mat_odd_vert = cp.mat_odd_vert;
  // others recalced
}

void GridColView::CopyFromView(GridColView* cp){
  Copy_(*cp);			// get per above
  visible = cp->visible;		// from inh
}

void GridColView::Destroy() {
//   cerr << "grid col nuking" << endl;
}

void GridColView::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if (text_width < 2) text_width = 2; // smallest practical
}

void GridColView::InitFromUserData() {
  DataCol* dc = dataCol(); //note: exists, because we were called
  if(dc->isMatrix()) {
    if(dc->isNumeric()) {
      mat_image = dc->GetUserData("IMAGE").toBool();
    }
    if(dc->HasUserData("TOP_ZERO"))
      mat_layout = taMisc::TOP_ZERO;
  }
  else if(dc->isString()) {
    if(dc->HasUserData("WIDTH"))
      text_width = dc->GetUserData("WIDTH").toInt();
  }
}

void GridColView::UpdateFromDataCol_impl(bool first){
  inherited::UpdateFromDataCol_impl(first);
  DataCol* dc = dataCol(); //note: exists, because we were called
  if (first) {
    text_width = dc->displayWidth(); // this uses user data WIDTH key if present
    InitFromUserData();
  }
}

String GridColView::GetDisplayName() const {
  DataCol* dc = dataCol(); //note: exists, because we were called
  if(dc) return dc->GetDisplayName();
  return inherited::GetDisplayName();
}

void GridColView::DataColUnlinked() {
  col_width = 0.0f;
  row_height = 0.0f;
}

void GridColView::ComputeColSizes() {
  // main point here is to compute size of column based on disp params
  // this is then normalized by the gridview display, so units are arbitrary
  // and are set to be characters (why not?)

  GridTableView* par = parent();
  DataCol* dc = dataCol(); // cache
  col_width = 0.0f;
  row_height = 0.0f;
  if (!dc) return;

  if(dc->isMatrix()) {
    int raw_width = 1;
    int raw_height = 1;
    dc->Get2DCellGeomGui(raw_width, raw_height, mat_odd_vert);
    // just linear in block size between range
    col_width = par->mat_size_range.Clip(raw_width);
    row_height = par->mat_size_range.Clip(raw_height);
  }
  else {
    row_height = 1.0f;		// always just one char high
    col_width = text_width;
  }
}

T3GridColViewNode* GridColView::MakeGridColViewNode() {
//NOTE: assumes that Clear has previously been called, so doesn't check for exist
  T3GridColViewNode* colnd = new T3GridColViewNode(this);
  setNode(colnd);
  return colnd;
}

void GridColView::SetTextWidth(int text_wdth) {
  text_width = text_wdth;
  GridTableView* par = parent();
  if(par)
    par->Render();
}



//////////////////////////////////
//  GridTableView		//
//////////////////////////////////

// Add a new GridTableView object to the frame for the given DataTable.
GridTableView* GridTableView::New(DataTable* dt, T3DataViewFrame*& fr) {
  NewNetViewHelper newNetView(fr, dt, "table");
  if (!newNetView.isValid()) return NULL;

  GridTableView* vw = new GridTableView;
  fr->AddView(vw);
  vw->setDataTable(dt);
  // make sure we get it all setup!
  vw->BuildAll();

  newNetView.showFrame();
  return vw;
}

void GridTableView::Initialize() {
  view_rows = 10;
  col_n = 5;

  width	= 1.0f;
  grid_on = true;
  header_on = true;
  row_num_on = false;
  two_d_font = false;		// true -- this is causing mysterious so crash, disabled for now
  two_d_font_scale = 350.0f;
  mat_val_text = false;

  grid_margin = 0.01f;
  grid_line_size = 0.005f;
  row_num_width = 4;

  mat_block_spc = 0.1f;
  mat_block_height = 0.0f;
  mat_rot = 0.0f;
  mat_trans = 0.6f;

  mat_size_range.min = 4;
  mat_size_range.max = 16;
  text_size_range.min = .02f;
  text_size_range.max = .05f;

  scrolling_ = false;

  colorscale.auto_scale = false;
  colorscale.min = -1.0f; colorscale.max = 1.0f;

  row_height = 0.1f; // non-zero dummy value
  head_height = .1f;
  font_scale = .1f;

  click_vals = false;
  lmb_val = 1.0f;
  mmb_val = 0.0f;

  last_sel_val = 0.0f;
  last_sel_got = false;

  children.SetBaseType(&TA_GridColView); // subclasses need to set this to their type!
}

void GridTableView::InitLinks() {
  inherited::InitLinks();
  taBase::Own(mat_size_range, this);
  taBase::Own(text_size_range, this);
  taBase::Own(col_range, this);
  taBase::Own(colorscale, this);
  taBase::Own(vis_cols, this);
  taBase::Own(col_widths_raw, this);
  taBase::Own(col_widths, this);
}

void GridTableView::CutLinks() {
  colorscale.CutLinks();
  col_range.CutLinks();
  inherited::CutLinks();
}

void GridTableView::Copy_(const GridTableView& cp) {
  col_n = cp.col_n;

  col_range = cp.col_range;
  width = cp.width;
  grid_on = cp.grid_on;
  header_on = cp.header_on;
  row_num_on = cp.row_num_on;
  two_d_font = cp.two_d_font;
  two_d_font_scale = cp.two_d_font_scale;
  mat_val_text = cp.mat_val_text;
  colorscale = cp.colorscale;

  grid_margin = cp.grid_margin;
  grid_line_size = cp.grid_line_size;
  row_num_width = cp.row_num_width;
  mat_block_spc = cp.mat_block_spc;
  mat_block_height = cp.mat_block_height;
  mat_rot = cp.mat_rot;
  mat_trans = cp.mat_trans;

  mat_size_range = cp.mat_size_range;
  text_size_range = cp.text_size_range;
}

void GridTableView::CopyFromView(GridTableView* cp) {
  Copy_(*cp);			// get stuff as above
  for(int i=0;i<children.size;i++) {
    GridColView* cvs = (GridColView*)colView(i);
    GridColView* cpvs = (GridColView*)cp->children.FindName(cvs->name);
    if(cpvs) {
      cvs->CopyFromView(cpvs);
    }
  }
  T3DataViewMain::CopyFromViewFrame(cp);
}

void GridTableView::UpdateAfterEdit_impl(){
  inherited::UpdateAfterEdit_impl();
  if (grid_margin < 0.0f) grid_margin = 0.0f;
  if (grid_line_size <  0.0f) grid_line_size =  0.0f;
}

void GridTableView::UpdateName() {
  DataTable* dt = dataTable();
  if (dt) {
    if (!name.contains(dt->name))
      SetName(dt->name + "_Grid");
  }
  else {
    if (name.empty())
      SetName("grid_no_table");
  }
}
  
const String GridTableView::caption() const {
  String rval = inherited::caption();
  if(last_sel_got) {
//     rval += " val: " + last_sel_col_nm + " = " + String(last_sel_val);
    rval += " val: " + String(last_sel_val);
  }
  return rval;
}

void GridTableView::InitFromUserData() {
  DataTable* dt = dataTable();
  if(!dt) return;
  if(dt->HasUserData("N_ROWS")) {
    view_rows = dt->GetUserDataAsInt("N_ROWS");
  }
  if(dt->HasUserData("AUTO_SCALE")) {
    colorscale.auto_scale = true;
  }
  if(dt->HasUserData("WIDTH")) {
    width = dt->GetUserDataAsFloat("WIDTH");
  }
  if(dt->HasUserData("SCALE_MIN")) {
    colorscale.min = dt->GetUserDataAsFloat("SCALE_MIN");
  }
  if(dt->HasUserData("SCALE_MAX")) {
    colorscale.max = dt->GetUserDataAsFloat("SCALE_MAX");
  }
  if(dt->HasUserData("BLOCK_HEIGHT")) {
    mat_block_height = dt->GetUserDataAsFloat("BLOCK_HEIGHT");
  }
  if(dt->HasUserData("BLOCK_SPACE")) {
    mat_block_spc = dt->GetUserDataAsFloat("BLOCK_SPACE");
  }
  if(dt->HasUserData("MAT_VAL_TEXT")) {
    mat_val_text = true;
  }
  if(dt->HasUserData("HEADER_OFF")) {
    header_on = false;
  }
  if(dt->HasUserData("ROW_NUM_ON")) {
    row_num_on = true;
  }
  if(dt->HasUserData("TWO_D_FONT")) {
    two_d_font = true;
  }
  if(dt->HasUserData("TWO_D_FONT_SCALE")) {
    two_d_font_scale = dt->GetUserDataAsFloat("TWO_D_FONT_SCALE");
  }
  if(dt->HasUserData("MAT_ROT")) {
    mat_rot = dt->GetUserDataAsFloat("MAT_ROT");
  }
  if(dt->HasUserData("MAT_TRANS")) {
    mat_trans = dt->GetUserDataAsFloat("MAT_TRANS");
  }
}

void GridTableView::UpdateFromDataTable_this(bool first) {
  inherited::UpdateFromDataTable_this(first);
  if (first) {
    InitFromUserData();
  }
}

void T3GridViewNode_MouseCB(void* userData, SoEventCallback* ecb);

void GridTableView::Render_pre() {
  bool show_drag = manip_ctrl_on;
  T3ExaminerViewer* vw = GetViewer();
  if(vw) {
    vw->syncViewerMode();
    if(!vw->interactionModeOn())
      show_drag = false;
  }

  setNode(new T3GridViewNode(this, width, show_drag));

  if(vw && vw->interactionModeOn()) {
    SoEventCallback* ecb = new SoEventCallback;
    ecb->addEventCallback(SoMouseButtonEvent::getClassTypeId(), T3GridViewNode_MouseCB, this);
    node_so()->addChild(ecb);
  }

  UpdatePanel();		// otherwise doesn't get updated without explicit click..
  inherited::Render_pre();
}

void GridTableView::Render_impl() {
  inherited::Render_impl();
  T3GridViewNode* node_so = this->node_so(); // cache
  if(!node_so || !dataTable())
    return;
  node_so->setWidth(width);	// does a render too -- ensure always up to date on width
  int orig_rows;
  CheckRowsChanged(orig_rows);	// don't do anything with this here, but just make sure m_rows is up to date
  MakeViewRangeValid();
  CalcViewMetrics();
  GetScaleRange();
  SetScrollBars();
  RenderGrid();
  RenderHeader();
  RenderLines();
}

void GridTableView::InitDisplay(bool init_panel) {
  RemoveLines();
  RemoveHeader();
  RemoveGrid();
  MakeViewRangeValid();

  if(init_panel) {
    InitPanel();
  }
}

void GridTableView::UpdateDisplay(bool update_panel) {
  int old_rows;
  int delta_rows = CheckRowsChanged(old_rows);

  if (!isVisible()) return;

  if(delta_rows > 0) {
    // if we were not at the very end, then don't scroll, but do update the panel
    if(update_panel && (view_range.max < old_rows-1) && (view_range.max > 0)) {
      UpdatePanel();
      return;
    }
    // scroll down to end of new data
    view_range.max = m_rows - 1; 
    view_range.min = view_range.max - view_rows + 1;
    view_range.min = MAX(0, view_range.min);
  }

  if (update_panel) UpdatePanel();
  Render_impl();
}

void GridTableView::MakeViewRangeValid() {
  DataTable* dt = dataTable();
  if(!dt) return;
  if(children.size != dt->data.size) { // just to make sure..
    UpdateFromDataTable();
  }
  inherited::MakeViewRangeValid();

  // get the list of visible columns
  vis_cols.Reset();
  for(int i=0;i<children.size;i++) {
    GridColView* cvs = (GridColView*)colView(i);
    if(cvs->isVisible()) vis_cols.Add(i);
  }

  int cols = vis_cols.size;
  if (col_range.min >= cols) {
    col_range.min = MAX(0, (cols - col_n - 1));
  }
  if(cols == 0) {
    col_range.max = -1;
    return;
  }
  col_range.max = col_range.min + col_n-1;
  col_range.MaxLT(cols - 1); // keep it less than max
}

void GridTableView::CalcViewMetrics() {
  DataTable* dt = dataTable();
  if(!dt) return;

  for(int i=0;i<children.size;i++) {
    GridColView* cvs = (GridColView*)colView(i);
    cvs->ComputeColSizes();
  }

  float tot_wd_raw = 0.0f;
  col_widths_raw.Reset();
  if(row_num_on) {
    col_widths_raw.Add((float)row_num_width);
    tot_wd_raw += (float)row_num_width;
  }

  bool has_mat = false;		// has a matrix?
  row_height_raw = 1.0f;
  for(int col = col_range.min; col<=col_range.max; ++col) {
    GridColView* cvs = (GridColView*)colVis(col);
    if(cvs->dataCol()->is_matrix)
      has_mat = true;
    col_widths_raw.Add(cvs->col_width);
    tot_wd_raw += cvs->col_width;
    row_height_raw = MAX(row_height_raw, cvs->row_height);
  }

  if(tot_wd_raw == 0.0f) return;	// something wrong

  // now normalize
  col_widths.SetSize(col_widths_raw.size);

  bool redo = false;
 normalize: // can goto here to redo..
  for(int i=0;i<col_widths_raw.size;i++) {
    col_widths[i] = width * (col_widths_raw[i] / tot_wd_raw);
  }

  // compute row height: if no header, it is simply 1.0 / view_rows
  float tot_ht_raw = row_height_raw * view_rows;
  if(!header_on) {
    row_height = 1.0f / view_rows;
    head_height = 0.0f;
  }
  else {
    // otherwise, need to figure out how much head height is in proportion to everything else
    tot_ht_raw += 1.0f; // header = 1.0
    float per_raw = 1.0f / tot_ht_raw;
    head_height = per_raw;
    row_height = row_height_raw * per_raw;
  }

//   cerr << "rhr: " << row_height_raw << " tot: " << tot_ht_raw << " rh: " << row_height << endl;

  // finally: compute font scale for entire display.  this is the minimum size based
  // on row/col constraints (units are 1 char, so that should be easy):
  // note that scale is in units of height, so col width needs to be converted to height

  float col_font_scale = t3Misc::char_ht_to_wd_pts / tot_wd_raw;
  //  float row_font_scale = (row_height / row_height_raw);
  float row_font_scale = row_height;

  if(!redo) {
    font_scale = MIN(col_font_scale, row_font_scale);
    if(font_scale > text_size_range.max && has_mat) {
      redo = true;		// don't do more than once
      float red_rat = text_size_range.max / font_scale;
      // we have a matrix and font size is too big: rescale it and then recompute
      font_scale = text_size_range.max;
      tot_wd_raw = 0.0f;
      int cidx = 0;
      if(row_num_on) {
	col_widths_raw[cidx] *= red_rat;
	tot_wd_raw += col_widths_raw[cidx];
	cidx++;
      }
      for(int col = col_range.min; col<=col_range.max; ++col) {
	GridColView* cvs = (GridColView*)colVis(col);
	if(!cvs) continue;
	if(!cvs->dataCol()->is_matrix)
	  col_widths_raw[cidx] *= red_rat;
	tot_wd_raw += col_widths_raw[cidx];
	cidx++;
      }
      goto normalize;		// redo
    }
  }

  if(font_scale < text_size_range.min) {
    font_scale = text_size_range.min; // that's just it!
  }
  // now just enforce max
  if(font_scale > text_size_range.max) {
    font_scale = text_size_range.max; // that's just it!
  }

  // need another iteration now that we know the true font scale!
  if(header_on) {
    head_height = font_scale;
    float per_raw = (1.0f - head_height) / (tot_ht_raw - 1.0f);
    row_height = row_height_raw * per_raw;
  }
}

void GridTableView::GetScaleRange() {
  if(!colorscale.auto_scale) {
    colorscale.FixRangeZero();
    return;
  }
  bool got_one = false;
  MinMax sc_rg;
  for(int col = col_range.min; col<=col_range.max; ++col) {
    GridColView* cvs = (GridColView*)colVis(col);
    if(!cvs || !cvs->scale_on)
      continue;
    DataCol* da = cvs->dataCol();
    if(!da->isNumeric() || !da->is_matrix) continue;
    da->GetMinMaxScale(sc_rg);
    sc_rg.SymRange();		// keep range symmetric around zero!
    if(!got_one)
      colorscale.SetMinMax(sc_rg.min, sc_rg.max);
    else
      colorscale.UpdateMinMax(sc_rg.min, sc_rg.max);
    got_one = true;
  }
}

void GridTableView_ColScrollCB(SoScrollBar* sb, int val, void* user_data) {
  GridTableView* gtv = (GridTableView*)user_data;
  gtv->scrolling_ = true;
  gtv->ViewCol_At(val);
  gtv->scrolling_ = false;
}

void GridTableView_RowScrollCB(SoScrollBar* sb, int val, void* user_data) {
  GridTableView* gtv = (GridTableView*)user_data;
  gtv->scrolling_ = true;
  gtv->ViewRow_At(val);
  gtv->scrolling_ = false;
}

void GridTableView::SetScrollBars() {
  if(scrolling_) return;		     // do't redo if currently doing!
  T3GridViewNode* node_so = this->node_so(); // cache
  if(!node_so) return;

  SoScrollBar* csb = node_so->ColScrollBar();
//   csb->setMinimum(0);
//   csb->setSingleStep(1);
  int eff_col_n = MIN(col_n, vis_cols.size);
  csb->setMaximum(vis_cols.size - eff_col_n);
  csb->setPageStep(eff_col_n);
  csb->setValue(col_range.min);
  csb->setValueChangedCB(GridTableView_ColScrollCB, this);

  SoScrollBar* rsb = node_so->RowScrollBar();
//   rsb->setMinimum(0);
//   rsb->setSingleStep(1);
  int mx = MAX((rows() - view_rows), 0);
  rsb->setMaximum(mx);
  int pg_step = MAX(view_rows, 1);
  rsb->setPageStep(pg_step);
  rsb->setValue(MIN(view_range.min, mx));
  rsb->setValueChangedCB(GridTableView_RowScrollCB, this);
}

void GridTableView::ClearViewRange() {
  col_range.min = 0;
  inherited::ClearViewRange();
}

void GridTableView::Clear_impl() {
  RemoveHeader();
  RemoveGrid();
  RemoveLines();
  inherited::Clear_impl();
}

void GridTableView::OnWindowBind_impl(iT3DataViewFrame* vw) {
  inherited::OnWindowBind_impl(vw);
  if (!m_lvp) {
    m_lvp = new iGridTableView_Panel(this);
    vw->RegisterPanel(m_lvp);
  }
}

void GridTableView::RemoveGrid() {
  T3GridViewNode* node_so = this->node_so();
  if (node_so) {
    node_so->grid()->removeAllChildren();
  }
}

void GridTableView::RemoveHeader() {
  T3GridViewNode* node_so = this->node_so();
  if (node_so) {
    node_so->header()->removeAllChildren();
  }
}

void GridTableView::RemoveLines(){
  T3GridViewNode* node_so = this->node_so();
  if (!node_so) return;
  node_so->body()->removeAllChildren();
}

///////////////////////////////////////////////////////////////
//	Actual Rendering of grid display
//
// margins are applied *within* cells -- cells are a specific height and width
// and then contents are indented by margin size
// grids can be drawn exactly on width/height boundaries

void GridTableView::RenderGrid() {
  T3GridViewNode* node_so = this->node_so();
  if (!node_so) return;
  SoGroup* grid = node_so->grid();
  grid->removeAllChildren(); // should have been done
  if (!grid_on) return;

  float gr_ln_sz = grid_line_size;

  // vertical lines
  SoSeparator* vert = new SoSeparator;
  // note: VERY unlikely to not need at least one line, so always make it
  SoCube* ln = new SoCube;
  ln->ref(); // our unref later will delete it if we never use it
  ln->width = gr_ln_sz; 
  ln->depth = gr_ln_sz; //note: give it depth to try to avoid disappearing lines issue
  ln->height = 1.0f;

  SoTranslation* tr = new SoTranslation();
  vert->addChild(tr);
  tr->translation.setValue(0.0f, -.5f, 0.0f);

  int col_idx = 0;
  float col_wd_lst = 0.0f; // width of last col

  if (row_num_on) {
    col_wd_lst = col_widths[col_idx++];
    SoTranslation* tr = new SoTranslation();
    vert->addChild(tr);
    tr->translation.setValue(col_wd_lst, 0.0f, 0.0f);
    vert->addChild(ln);
  }

  for (int col = col_range.min; col < col_range.max; ++col) {
    GridColView* cvs = (GridColView*)colVis(col);
    if (!cvs) continue;
    col_wd_lst = col_widths[col_idx++];
    SoTranslation* tr = new SoTranslation();
    vert->addChild(tr);
    tr->translation.setValue(col_wd_lst, 0.0f, 0.0f);
    vert->addChild(ln);
  }
  grid->addChild(vert);
  ln->unref(); // deleted if not used

  // horizontal lines
  SoSeparator* horiz = new SoSeparator;
  // note: VERY unlikely to not need at least one line, so always make it
  ln = new SoCube;
  ln->ref(); // our unref later will delete it if we never use it
  ln->width = width; 
  ln->depth = gr_ln_sz; //note: give it depth to try to avoid disappearing lines issue
  ln->height = gr_ln_sz;
  tr = new SoTranslation();
  horiz->addChild(tr);
  tr->translation.setValue(.5f * width, 0.0f, 0.0f);

  if (header_on) {
    SoTranslation* tr = new SoTranslation;
    horiz->addChild(tr);
    tr->translation.setValue(0.0f, -head_height, 0.0f);
    horiz->addChild(ln);
  }

  for (int data_row = view_range.min; data_row <= view_range.max; ++data_row) {
    SoTranslation* tr = new SoTranslation();
    horiz->addChild(tr);
    tr->translation.setValue(0.0f, -row_height, 0.0f);
    horiz->addChild(ln);
  }
  grid->addChild(horiz);
  ln->unref(); // deleted if not used
}

void GridTableView::RenderHeader() {
  T3GridViewNode* node_so = this->node_so();
  if (!node_so) return;
  
  // safely/correctly clear all the column headers
  // we remove first manually from us...
  SoSeparator* hdr = node_so->header();
  hdr->removeAllChildren();
  // and then remove ref in the ColView guys
  for (int i = 0; i < colViewCount(); i++) {
    GridColView* gcv = (GridColView*)colView(i);
    gcv->Clear();
  }
  
  if(!header_on || dataTable()->data.size == 0) return; // normally shouldn't be called if off

  SoComplexity* cplx = new SoComplexity;
  cplx->value.setValue(taMisc::text_complexity);
  hdr->addChild(cplx);

  SoFont* fnt = new SoFont();
  fnt->name = (const char*)taMisc::t3d_font_name;
  if(two_d_font)
    fnt->size.setValue(two_d_font_scale * font_scale);
  else
    fnt->size.setValue(font_scale);
  hdr->addChild(fnt);

  T3DataViewFrame* fr = GetFrame();
  iColor txtcolr = fr->GetTextColor();

  float gr_mg_sz = grid_margin;
  float gr_mg_sz2 = 2.0f * gr_mg_sz;
  // margin and baseline adj
  SoTranslation* tr = new SoTranslation();
  hdr->addChild(tr);
  float base_adj = (head_height * t3Misc::char_base_fract);
  tr->translation.setValue(0.0f, - (head_height - base_adj), -gr_mg_sz); // set back just a hair

  int col_idx = 0;
  float col_wd_lst = 0.0f; // width of last col

  if (row_num_on) {
    col_wd_lst = col_widths[col_idx++];
    SoSeparator* txsep = new SoSeparator;
    hdr->addChild(txsep);
    SoTranslation* ttr = new SoTranslation();
    txsep->addChild(ttr);
    ttr->translation.setValue(.5f * col_wd_lst, 0.0f, 0.0f);
    if(two_d_font) {
      SoText2* txt = new SoText2;
      txt->justification = SoText2::CENTER;
      txsep->addChild(txt);
      txt->string.setValue("#");
    }
    else {
      SoAsciiText* txt = new SoAsciiText;
      txt->justification = SoAsciiText::CENTER;
      txsep->addChild(txt);
      txt->string.setValue("#");
    }
  }

  for (int col = col_range.min; col <= col_range.max; ++col) {
    GridColView* cvs = (GridColView*)colVis(col);
    if (!cvs) continue;
    if (col_wd_lst > 0.0f) { 
      tr = new SoTranslation();
      hdr->addChild(tr);
      tr->translation.setValue(col_wd_lst, 0.0f, 0.0f);
    }
    col_wd_lst = col_widths[col_idx++]; // note incr
    T3GridColViewNode* colnd = cvs->MakeGridColViewNode(); //note: non-standard semantics
    SoSeparator* colsep = colnd->topSeparator();
    colnd->material()->diffuseColor.setValue(txtcolr.redf(), txtcolr.greenf(), txtcolr.bluef());
    SoTranslation* ttr = new SoTranslation();
    ttr->translation.setValue(.5f * col_wd_lst, 0.0f, 0.0f);
    colsep->addChild(ttr);
    hdr->addChild(colnd);
    if(two_d_font) {
      SoText2* txt = new SoText2;
      txt->justification = SoText2::CENTER;
      colsep->addChild(txt);
      int max_chars = (int)(t3Misc::char_ht_to_wd_pts * col_wd_lst / font_scale) + 1;
      txt->string.setValue(cvs->GetDisplayName().elidedTo(max_chars).chars());
    }
    else {
      SoAsciiText* txt = new SoAsciiText;
      txt->justification = SoAsciiText::CENTER;
      colsep->addChild(txt);
      int max_chars = (int)(t3Misc::char_ht_to_wd_pts * col_wd_lst / font_scale) + 1;
      txt->string.setValue(cvs->GetDisplayName().elidedTo(max_chars).chars());
    }
    // light aqua transparent background
    SoTranslation* rectr = new SoTranslation();
    colnd->addChild(rectr);
    rectr->translation.setValue(0.0f, .5f * head_height-base_adj, 0.0f);
    SoMaterial* rectmat = new SoMaterial;
    rectmat->diffuseColor.setValue(0.0f, 1.0f, 1.0f);
    rectmat->transparency.setValue(0.8f);
    colnd->topSeparator()->addChild(rectmat);
    SoCube* rect = new SoCube;
    rect->width = col_wd_lst - gr_mg_sz2;
    rect->height = head_height;
    rect->depth = gr_mg_sz;
    colnd->topSeparator()->addChild(rect);
    cvs->setNode(colnd);
  }
}

// todo: support renderValues() for matrixgrid??

void GridTableView::RenderLine(int view_idx, int data_row) {
  T3GridViewNode* node_so = this->node_so();
  if (!node_so) return;
  float gr_mg_sz = grid_margin;
  float gr_mg_sz2 = 2.0f * gr_mg_sz;
  // origin is top-left of body area
  // make line container
  SoSeparator* ln = new SoSeparator();

  DataTable* dt = dataTable(); //cache
  String el;
  // place row and first margin here, simplifies everything...
  SoTranslation* tr = new SoTranslation();
  ln->addChild(tr);
  // 1st tr places origin for the row
  tr->translation.setValue(0.0f, -(row_height * (float)view_idx), 0.0f);

  float text_ht = font_scale;

  T3DataViewFrame* fr = GetFrame();
  iColor txtcolr = fr->GetTextColor();

  int col_idx = 0;
  float col_wd_lst = 0.0f; // width of last col
  // following metrics will be adjusted for mat font scale, when necessary
  float txt_base_adj = text_ht * t3Misc::char_base_fract;

  float row_ht = row_height - gr_mg_sz2;
  float mat_rot_rad = mat_rot * taMath_float::rad_per_deg;

  // render row_num cell, if on
  if (row_num_on) {
    col_wd_lst = col_widths[col_idx++];
    SoSeparator* row_sep = new SoSeparator;
    tr = new SoTranslation;
    row_sep->addChild(tr);
    SoBaseColor* bc = new SoBaseColor;
    bc->rgb.setValue(txtcolr.redf(), txtcolr.greenf(), txtcolr.bluef());
    row_sep->addChild(bc);
    float y_offs = ((row_height - text_ht) * 0.5f) + text_ht - txt_base_adj;
    el = String(data_row);
    tr->translation.setValue(.5 * col_wd_lst, -y_offs, 0.0f);
    if(two_d_font) {
      SoText2* txt = new SoText2;
      txt->justification = SoText2::CENTER;
      row_sep->addChild(txt);
      txt->string.setValue(el.chars());
    }
    else {
      SoAsciiText* txt = new SoAsciiText;
      txt->justification = SoAsciiText::CENTER;
      row_sep->addChild(txt);
      txt->string.setValue(el.chars());
    }
    ln->addChild(row_sep);
  }
  for (int col = col_range.min; col <= col_range.max; col++) {
    GridColView* cvs = (GridColView*)colVis(col);
    if (!cvs) continue;
    DataCol* dc = cvs->dataCol();

    //calculate the actual col row index, for the case of a jagged data table
    int act_idx; // <0 for null
    dt->idx(data_row, dc->rows(), act_idx);
    
    // translate the col to proper location
    if (col_wd_lst > 0.0f) { 
      tr = new SoTranslation();
      ln->addChild(tr);
      tr->translation.setValue(col_wd_lst, 0.0f, 0.0f);
    }
    col_wd_lst = col_widths[col_idx++]; // index is now +1 already..

    float col_wd = col_wd_lst - gr_mg_sz2;
    
    // null columns just get single "n/a" regardless of geom etc.
    if (act_idx < 0) {
      SoSeparator* nul_sep = new SoSeparator;
      ln->addChild(nul_sep);
      tr = new SoTranslation;
      nul_sep->addChild(tr);
      if(two_d_font) {
	SoText2* txt = new SoText2;
	nul_sep->addChild(txt);
	txt->justification = SoText2::CENTER;
	txt->string.setValue("n/a");
      }
      else {
	SoAsciiText* txt = new SoAsciiText;
	nul_sep->addChild(txt);
	txt->justification = SoAsciiText::CENTER;
	txt->string.setValue("n/a");
      }
      // center the n/a in the cell
      float x_offs = .5f * col_wd_lst;
      float y_offs = (.5f * (row_height - text_ht)) + text_ht - txt_base_adj;
      tr->translation.setValue(x_offs, -y_offs, 0.0f);
      continue;
    }

    if(dc->is_matrix) {
      if(cvs->mat_image) {
	SoSeparator* img = new SoSeparator;
	ln->addChild(img);
	SoTransform* tr = new SoTransform();
	img->addChild(tr);
	// scale the image according to pixel metrics
	// note: image defaults to 1 geom unit width, so we scale by our act width
	tr->scaleFactor.setValue(col_wd, row_ht, 1.0f);
	// center the shape in the middle of the cell
	tr->translation.setValue(col_wd_lst * .5f, -(row_height *.5f), 0.0f);
	SoImageEx* img_so = new SoImageEx;
	img->addChild(img_so);

	taMatrix* cell_mat =  dc->GetValAsMatrix(act_idx);
	if(cell_mat) {
	  taBase::Ref(cell_mat);
	  bool top_zero = (cvs->mat_layout == taMisc::TOP_ZERO);
	  img_so->setImage(*cell_mat, top_zero);
	  taBase::UnRef(cell_mat);
	}
      }
      else {
	float mat_ht = row_ht;
	if(col_wd > mat_ht)	// make it more square as height increases..
	  mat_ht += (col_wd - mat_ht) * fabsf(sinf(mat_rot_rad));
	SoSeparator* grsep = new SoSeparator;
	ln->addChild(grsep);
	SoTransform* tr = new SoTransform(); // todo: add this to image object! get rid of extra
	grsep->addChild(tr);
	tr->scaleFactor.setValue(col_wd, mat_ht, 1.0f);
	tr->translation.setValue(gr_mg_sz, -(row_height-gr_mg_sz), 0.0f);
	tr->rotation.setValue(SbVec3f(1.0f, 0.0f, 0.0f), mat_rot_rad);
	
	taMatrix* cell_mat =  dc->GetValAsMatrix(act_idx);
	if(cell_mat) {
	  taBase::Ref(cell_mat);
	  SoMatrixGrid* sogr = new SoMatrixGrid
	    (cell_mat, cvs->mat_odd_vert, &colorscale, (SoMatrixGrid::MatrixLayout)cvs->mat_layout, 
	     mat_val_text);
	  sogr->spacing = mat_block_spc;
	  sogr->block_height = mat_block_height;
	  sogr->trans_max = mat_trans;
	  sogr->user_data = dc;	// needed for point picking
	  sogr->render();
	  taBase::UnRef(cell_mat);
	  grsep->addChild(sogr);
	}
      }
    }
    else {			// scalar: always text
      SoSeparator* txt_sep = new SoSeparator;
      ln->addChild(txt_sep);
      // text origin is bottom left (Left) or bottom right (Right)
      // and we want to center vertically when  height is greater than txt height
      SoAsciiText::Justification just = SoAsciiText::LEFT;
      float x_offs = gr_mg_sz; // default for left
      tr = new SoTranslation;
      txt_sep->addChild(tr);
      SoBaseColor* bc = new SoBaseColor;
      bc->rgb.setValue(txtcolr.redf(), txtcolr.greenf(), txtcolr.bluef());
      txt_sep->addChild(bc);
      float y_offs = ((row_height - text_ht) *.5f) +
	text_ht - txt_base_adj;
      if (dc->isNumeric()) {
	just = SoAsciiText::RIGHT;
	x_offs = col_wd - gr_mg_sz2;
	el = Variant::formatNumber(dc->GetValAsVar(act_idx),6); // 6 = precision
      } else {
	int max_chars = (int)(t3Misc::char_ht_to_wd_pts * col_wd / font_scale) + 1;
	el = dc->GetValAsString(act_idx).elidedTo(max_chars);
      }
      tr->translation.setValue(x_offs, -y_offs, 0.0f);
      if(two_d_font) {
	SoText2* txt = new SoText2;
	txt_sep->addChild(txt);
	txt->justification = (SoText2::Justification)just;
	txt->string.setValue(el.chars());
      }
      else {
	SoAsciiText* txt = new SoAsciiText;
	txt_sep->addChild(txt);
	txt->justification = just;
	txt->string.setValue(el.chars());
      }
    
    } //text
  }
  node_so->body()->addChild(ln);
}

void GridTableView::RenderLines(){
  // this updates the data area
  T3GridViewNode* node_so = this->node_so();
  if (!node_so) return;
  SoSeparator* body = node_so->body(); // cache
  body->removeAllChildren(); //should already have been done

  if(dataTable()->data.size == 0) return;

  // master font -- we only add a child font if different
  // doesn't seem to make much diff:
  SoComplexity* cplx = new SoComplexity;
  cplx->value.setValue(taMisc::text_complexity);
  body->addChild(cplx);

  SoFont* fnt = new SoFont();
  fnt->name = (const char*)taMisc::t3d_font_name;
  if(two_d_font)
    fnt->size.setValue(two_d_font_scale * font_scale);
  else
    fnt->size.setValue(font_scale);
  body->addChild(fnt);
//   FontSpec& fs = viewSpec()->font;
//   fs.copyTo(fnt);
  body->addChild(fnt);
  if (header_on) {
    SoTranslation* tr = new SoTranslation;
    body->addChild(tr);
    tr->translation.setValue(0.0f, -head_height, 0.0f);
  }

  // this is the index (in view_range units) of the first view line of data in the buffer
  int view_idx = 0;
  for (int data_row = view_range.min; data_row <= view_range.max; ++data_row) {
    RenderLine(view_idx, data_row);
    ++view_idx;
  }
}

////////////////////////////////////////////////////////////////////////
// note on following: basically callbacks from view..

void GridTableView::SetColorSpec(ColorScaleSpec* color_spec) {
  colorscale.SetColorSpec(color_spec);
  UpdateDisplay(true);
}

void GridTableView::ShowAllCols() {
  for (int i = 0; i < colViewCount(); i++) {
    GridColView* gcv = (GridColView*)colView(i);
    gcv->visible = true;
  }
  UpdateDisplay(true);
}

void GridTableView::setWidth(float wdth) {
  width = wdth;
  T3GridViewNode* node_so = this->node_so();
  if (!node_so) return;
  node_so->setWidth(wdth);
//  UpdateDisplay(true);
}

void GridTableView::setScaleData(bool auto_scale_, float min_, float max_) {
  if ((colorscale.auto_scale == auto_scale_) && (colorscale.min == min_) && (colorscale.max == max_)) return;
  colorscale.auto_scale = auto_scale_;
  if(!colorscale.auto_scale)
    colorscale.SetMinMax(min_, max_);
//  UpdateDisplay(true);
}

void GridTableView::VScroll(bool left) {
  if (left) {
    ViewCol_At(col_range.min - 1);
  } else {
    ViewCol_At(col_range.min + 1);
  }
}

void GridTableView::ViewCol_At(int start) {
  if (start < 0) start = 0;
  col_range.min = start;
  UpdateDisplay();		// takes care of keeping col in range 
}

void GridTableView::ColBackAll() {
  ViewCol_At(0);
}
void GridTableView::ColBackPg() {
  int cur_col = col_range.min;
  int goto_col = cur_col - col_n;
  goto_col = MAX(0, goto_col);
  ViewCol_At(goto_col);
}
void GridTableView::ColBack1() {
  int cur_col = col_range.min;
  int goto_col = cur_col - 1;
  goto_col = MAX(0, goto_col);
  ViewCol_At(goto_col);
}
void GridTableView::ColFwd1() {
  int cur_col = col_range.min;
  int goto_col = cur_col + 1;
  goto_col = MIN(vis_cols.size-col_n, goto_col);

  ViewCol_At(goto_col);
}
void GridTableView::ColFwdPg() {
  int cur_col = col_range.min;
  int goto_col = cur_col + col_n;
  goto_col = MIN(vis_cols.size-col_n, goto_col);

  ViewCol_At(goto_col);
}
void GridTableView::ColFwdAll() {
  ViewCol_At(vis_cols.size - col_n);
}

// callback for view transformer dragger
void T3GridViewNode_DragFinishCB(void* userData, SoDragger* dragr) {
  SoTransformBoxDragger* dragger = (SoTransformBoxDragger*)dragr;
  T3GridViewNode* vnd = (T3GridViewNode*)userData;
  GridTableView* nv = static_cast<GridTableView*>(vnd->dataView());

  SbRotation cur_rot;
  cur_rot.setValue(SbVec3f(nv->main_xform.rotate.x, nv->main_xform.rotate.y, 
			   nv->main_xform.rotate.z), nv->main_xform.rotate.rot);

  SbVec3f trans = dragger->translation.getValue();
//   cerr << "trans: " << trans[0] << " " << trans[1] << " " << trans[2] << endl;
  cur_rot.multVec(trans, trans); // rotate translation by current rotation
  trans[0] *= nv->main_xform.scale.x;
  trans[1] *= nv->main_xform.scale.y;
  trans[2] *= nv->main_xform.scale.z;
  FloatTDCoord tr(trans[0], trans[1], trans[2]);
  nv->main_xform.translate += tr;

  const SbVec3f& scale = dragger->scaleFactor.getValue();
//   cerr << "scale: " << scale[0] << " " << scale[1] << " " << scale[2] << endl;
  FloatTDCoord sc(scale[0], scale[1], scale[2]);
  if(sc < .1f) sc = .1f;	// prevent scale from going to small too fast!!
  nv->main_xform.scale *= sc;

  SbVec3f axis;
  float angle;
  dragger->rotation.getValue(axis, angle);
//   cerr << "orient: " << axis[0] << " " << axis[1] << " " << axis[2] << " " << angle << endl;
  if(axis[0] != 0.0f || axis[1] != 0.0f || axis[2] != 1.0f || angle != 0.0f) {
    SbRotation rot;
    rot.setValue(SbVec3f(axis[0], axis[1], axis[2]), angle);
    SbRotation nw_rot = rot * cur_rot;
    nw_rot.getValue(axis, angle);
    nv->main_xform.rotate.SetXYZR(axis[0], axis[1], axis[2], angle);
  }

  vnd->txfm_shape()->scaleFactor.setValue(1.0f, 1.0f, 1.0f);
  vnd->txfm_shape()->rotation.setValue(SbVec3f(0.0f, 0.0f, 1.0f), 0.0f);

  float frmg = T3GridViewNode::frame_margin;
  float frmg2 = 2.0f * frmg;
  float wdth = vnd->getWidth();

  vnd->txfm_shape()->translation.setValue(.5f * wdth, .5f * (1.0f + frmg2), 0.0f);

  dragger->translation.setValue(0.0f, 0.0f, 0.0f);
  dragger->rotation.setValue(SbVec3f(0.0f, 0.0f, 1.0f), 0.0f);
  dragger->scaleFactor.setValue(1.0f, 1.0f, 1.0f);

  nv->UpdateDisplay();
}

// this callback is registered in GridTableView::Render_pre

void T3GridViewNode_MouseCB(void* userData, SoEventCallback* ecb) {
  GridTableView* gv = (GridTableView*)userData;
  T3DataViewFrame* fr = gv->GetFrame();
  SoMouseButtonEvent* mouseevent = (SoMouseButtonEvent*)ecb->getEvent();
  SoMouseButtonEvent::Button but = mouseevent->getButton();
  if(!SoMouseButtonEvent::isButtonReleaseEvent(mouseevent, but)) return; // only releases
//   cerr << "but: " << but << endl;
  bool left_but = false;	// assume other button -- don't really care what it is
  if(but == SoMouseButtonEvent::BUTTON1)
    left_but = true;
  bool got_one = false;
  for(int i=0;i<fr->root_view.children.size;i++) {
    taDataView* dv = fr->root_view.children[i];
    if(dv->InheritsFrom(&TA_GridTableView)) {
      GridTableView* tgv = (GridTableView*)dv;
      T3ExaminerViewer* viewer = tgv->GetViewer();
      SoRayPickAction rp( viewer->getViewportRegion());
      rp.setPoint(mouseevent->getPosition());
      rp.apply(viewer->quarter->getSoEventManager()->getSceneGraph()); // event mgr has full graph!
      SoPickedPoint* pp = rp.getPickedPoint(0);
      if(!pp) continue;
      SoNode* pobj = pp->getPath()->getNodeFromTail(1);
      if(!pobj) continue;
//       cerr << "obj typ: " << pobj->getTypeId().getName() << endl;
      if(!pobj->isOfType(SoMatrixGrid::getClassTypeId())) {
// 	cerr << "not SoMatrixGrid!" << endl;
	continue;
      }
      SoMatrixGrid* mtxg = (SoMatrixGrid*)pobj;
      DataCol* dcol = (DataCol*)mtxg->user_data;
      taMatrix* matrix = mtxg->matrix;
      SbVec3f pt = pp->getObjectPoint(pobj); 
//       cerr << "got: " << pt[0] << " " << pt[1] << " " << pt[2] << endl;

      int geom_x, geom_y;
      matrix->geom.Get2DGeomGui(geom_x, geom_y, mtxg->odd_y, 1);
//      float cl_x = 1.0f / (float)geom_x;	// how big each cell is
//      float cl_y = 1.0f / (float)geom_y;
      int xp = (int)((pt[0] * geom_x));
      int yp = (int)((pt[1] * geom_y));
//       cerr << xp << ", " << yp << endl;

      float val_to_set = 0.0f;
      if(tgv->click_vals) {
	if(left_but) val_to_set = tgv->lmb_val;
	else val_to_set = tgv->mmb_val;
	taProject* proj = (taProject*)tgv->GetOwner(&TA_taProject);
	// save undo state!
	if(proj && dcol) {
	  proj->undo_mgr.SaveUndo(dcol, "GridView Click Vals", dcol);
	}
      }

      // note: all the bot zero stuff is backwards due to coord inversion -- coin is bot zero!
      if(matrix->dims() == 1) {
	int ymax = matrix->dim(0);	// assumes odd_y
	int yeff = yp;
	if(mtxg->mat_layout != SoMatrixGrid::BOT_ZERO) yeff = ymax-1-yp;
	if(tgv->click_vals)
	  matrix->SetFmVar(val_to_set, yeff);
	else
	  tgv->last_sel_val = matrix->SafeElAsFloat(yeff);
      }
      if(matrix->dims() == 2) {
	int yeff = yp;
	if(mtxg->mat_layout != SoMatrixGrid::BOT_ZERO) yeff = geom_y-1-yp;
	if(tgv->click_vals)
	  matrix->SetFmVar(val_to_set, xp, yeff);
	else
	  tgv->last_sel_val = matrix->SafeElAsFloat(xp, yeff);
      }
      if(matrix->dims() == 4) {
	yp--;			// yp has 1 extra in 4d by some reason.. spacing..
	int xmax = matrix->dim(0);	int ymax = matrix->dim(1);
	int xxmax = matrix->dim(2);	int yymax = matrix->dim(3);

	int oxp = xp / (xmax+1);	int oyp = yp / (ymax+1);
	int ixp = xp % (xmax+1);	int iyp = yp % (ymax+1);
// 	cerr << "4d: " << ixp << ", " << iyp << ", " << oxp << ", " << oyp << endl;

	int iyeff = iyp;  int oyeff = oyp;
	if(mtxg->mat_layout != SoMatrixGrid::BOT_ZERO) {
	  iyeff = ymax-1-iyp;	  oyeff = yymax-1-oyp;
	}
	if(tgv->click_vals) 
	  matrix->SetFmVar(val_to_set, ixp, iyeff, oxp, oyeff);
	else
	  tgv->last_sel_val = matrix->SafeElAsFloat(ixp, iyeff, oxp, oyeff);
      }
      if(tgv->click_vals) {
	// this causes a crash
// 	if(dcol)
// 	  dcol->DataChanged(DCR_ITEM_UPDATED); // col drives updating
      }
      else {
	tgv->last_sel_got = true;
	if(dcol) {
	  tgv->last_sel_col_nm = dcol->name;
	}
      }
      tgv->UpdateDisplay();	// update to show last viewed val
      got_one = true;
      break;
    }
  }
  if(got_one)
    ecb->setHandled();
}

//////////////////////////
//    iTableView_Panel //
//////////////////////////

iDataTableView_Panel::iDataTableView_Panel(DataTableView* lv)
:inherited(lv)
{
  widg = new QWidget();
//  widg->setFrameStyle( QFrame::GroupBoxPanel | QFrame::Sunken );
  layWidg = new QVBoxLayout(widg); //def margin/spacing=2  
  layWidg->setMargin(0); layWidg->setSpacing(2);

  ////////////////////////////////////////////////////////////////////////////
  // Command Buttons
  widCmdButtons = new QWidget(widg);
  iFlowLayout* fl = new iFlowLayout(widCmdButtons);
//  layTopCtrls->addWidget(widCmdButtons);
  layOuter->addWidget(widCmdButtons);
  
  meth_but_mgr = new iMethodButtonMgr(widCmdButtons, fl, widCmdButtons); 
  meth_but_mgr->Constr(lv->dataTable());

  MakeButtons(layOuter);

  setCentralWidget(widg);
}

iDataTableView_Panel::~iDataTableView_Panel() {
}


//////////////////////////
// iGridTableView_Panel //
//////////////////////////

String iGridTableView_Panel::panel_type() const {
  static String str("Grid Log");
  return str;
}

iGridTableView_Panel::iGridTableView_Panel(GridTableView* tlv)
:inherited(tlv)
{
  int font_spec = taiMisc::fonMedium;

  layTopCtrls = new QHBoxLayout; layWidg->addLayout(layTopCtrls);
//   layTopCtrls->setSpacing(2);	// plenty of room

  // note: check boxes require spacing after them apparently, at least on mac..

  chkDisplay = new QCheckBox("Disp", widg); chkDisplay->setObjectName("chkDisplay");
  chkDisplay->setToolTip("Whether to update the display when the underlying data changes");
  connect(chkDisplay, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
  layTopCtrls->addWidget(chkDisplay);
  layTopCtrls->addSpacing(taiM->hsep_c);

  chkManip = new QCheckBox("Manip", widg); chkDisplay->setObjectName("chkManip");
  chkManip->setToolTip("Whether to enable manipulation of the view object via a transformation box that supports position, scale and rotation manipulations");
  connect(chkManip, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
  layTopCtrls->addWidget(chkManip);
  layTopCtrls->addSpacing(taiM->hsep_c);

  chkHeaders =  new QCheckBox("Hdrs", widg ); chkHeaders->setObjectName("chkHeaders");
  chkHeaders->setToolTip("Whether to display a top row of headers indicating the name of the columns");
  connect(chkHeaders, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
  layTopCtrls->addWidget(chkHeaders);
  layTopCtrls->addSpacing(taiM->hsep_c);

  chkRowNum =  new QCheckBox("Row\n#", widg); chkRowNum->setObjectName("chkRowNum");
  chkRowNum->setToolTip("Whether to display the row number as the first column");
  connect(chkRowNum, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
  layTopCtrls->addWidget(chkRowNum);
  layTopCtrls->addSpacing(taiM->hsep_c);

  chk2dFont =  new QCheckBox("2d\nFont", widg); chk2dFont->setObjectName("chk2dFont");
  chk2dFont->setToolTip("Whether to use a two-dimensional font that is easier to read but does not obey 3d transformations of the display");
  connect(chk2dFont, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
  layTopCtrls->addWidget(chk2dFont);
  layTopCtrls->addSpacing(taiM->hsep_c);

  lblFontScale = taiM->NewLabel("Font\nScale", widg, font_spec);
  lblFontScale->setToolTip("Scaling of the 2d font to make it roughly the same size as the 3d font -- adjust this to change the size of the 2d text (has no effect if 2d Font is not clicked");
  layTopCtrls->addWidget(lblFontScale);
  fldFontScale = dl.Add(new taiField(&TA_float, this, NULL, widg));
  layTopCtrls->addWidget(fldFontScale->GetRep());
  ((iLineEdit*)fldFontScale->GetRep())->setCharWidth(8);

  layTopCtrls->addStretch();

  butRefresh = new QPushButton("Refresh", widg);
  butRefresh->setFixedHeight(taiM->button_height(taiMisc::sizSmall));
  layTopCtrls->addWidget(butRefresh);
  connect(butRefresh, SIGNAL(pressed()), this, SLOT(butRefresh_pressed()) );

  layVals = new QHBoxLayout; layWidg->addLayout(layVals);
  layVals->setSpacing(2);	// plenty of room

  lblRows = taiM->NewLabel("Rows", widg, font_spec);
  lblRows->setToolTip("Maximum number of rows to display (row height is scaled to fit).");
  layVals->addWidget(lblRows);
  fldRows = dl.Add(new taiIncrField(&TA_int, this, NULL, widg));
  layVals->addWidget(fldRows->GetRep());

  lblCols = taiM->NewLabel("Cols", widg, font_spec);
  lblCols->setToolTip("Maximum number of columns to display (column widths are scaled to fit).");
  layVals->addWidget(lblCols);
  fldCols = dl.Add(new taiIncrField(&TA_int, this, NULL, widg));
  layVals->addWidget(fldCols->GetRep());

  lblWidth = taiM->NewLabel("Width", widg, font_spec);
  lblWidth->setToolTip("Width of grid log display, in normalized units (default is 1.0 = same as height).");
  layVals->addWidget(lblWidth);
  fldWidth = dl.Add(new taiField(&TA_float, this, NULL, widg));
  layVals->addWidget(fldWidth->GetRep());
  ((iLineEdit*)fldWidth->GetRep())->setCharWidth(8);

  lblTxtMin = taiM->NewLabel("Min\nText", widg, font_spec);
  lblTxtMin->setToolTip("Minimum text size in 'view units' (size of entire display is 1.0) -- .02 is default -- increase to make small text more readable");
  layVals->addWidget(lblTxtMin);
  fldTxtMin = dl.Add(new taiField(&TA_float, this, NULL, widg));
  layVals->addWidget(fldTxtMin->GetRep());
  ((iLineEdit*)fldTxtMin->GetRep())->setCharWidth(8);

  lblTxtMax = taiM->NewLabel("Max\nText", widg, font_spec);
  lblTxtMax->setToolTip("Maximum text size in 'view units' (size of entire display is 1.0) -- .05 is default");
  layVals->addWidget(lblTxtMax);
  fldTxtMax = dl.Add(new taiField(&TA_float, this, NULL, widg));
  layVals->addWidget(fldTxtMax->GetRep());
  ((iLineEdit*)fldTxtMax->GetRep())->setCharWidth(8);

  layVals->addStretch();

  ////////////////////////////////////////////////////////////////////////////
  layMatrix = new QHBoxLayout; layWidg->addLayout(layMatrix);
  layMatrix->setSpacing(2);	// plenty of room

  lblMatrix = taiM->NewLabel("Matrix\nDisplay", widg, font_spec);
  lblMatrix->setToolTip("This row contains parameters that control the display of matrix values (shown in a grid of colored blocks)");
  layMatrix->addWidget(lblMatrix);

  chkValText =  new QCheckBox("Val\nTxt", widg); chkValText->setObjectName( "chkValText");
  chkValText->setToolTip("Whether to display text of the matrix block values.");
  connect(chkValText, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
  layMatrix->addWidget(chkValText);
  layMatrix->addSpacing(taiM->hsep_c);

  lblTrans = taiM->NewLabel("Trans-\nparency", widg, font_spec);
  lblTrans->setToolTip("Maximum transparency of the grid blocks (0 = fully opaque, 1 = fully transparent)\nBlocks with smaller magnitude values are more transparent.");
  layMatrix->addWidget(lblTrans);
  fldTrans = dl.Add(new taiField(&TA_float, this, NULL, widg));
  layMatrix->addWidget(fldTrans->GetRep());
  ((iLineEdit*)fldTrans->GetRep())->setCharWidth(8);

  lblRot = taiM->NewLabel("Mat\nRot", widg, font_spec);
  lblRot->setToolTip("Rotation (in degrees) of the matrix in the Z axis, producing a denser stacking of patterns.");
  layMatrix->addWidget(lblRot);
  fldRot = dl.Add(new taiField(&TA_float, this, NULL, widg));
  layMatrix->addWidget(fldRot->GetRep());
  ((iLineEdit*)fldRot->GetRep())->setCharWidth(8);

  lblBlockHeight = taiM->NewLabel("Blk\nHgt", widg, font_spec);
  lblBlockHeight->setToolTip("Maximum height of grid blocks (in Z dimension), as a proportion of their overall X-Y size.");
  layMatrix->addWidget(lblBlockHeight);
  fldBlockHeight = dl.Add(new taiField(&TA_float, this, NULL, widg));
  layMatrix->addWidget(fldBlockHeight->GetRep());
  ((iLineEdit*)fldBlockHeight->GetRep())->setCharWidth(8);

  layMatrix->addStretch();

  ////////////////////////////////////////////////////////////////////////////
  // 	Colorscale etc
  layColorScale = new QHBoxLayout; layWidg->addLayout(layColorScale);
  
  chkAutoScale = new QCheckBox("auto\nscale", widg);
  connect(chkAutoScale, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
  layColorScale->addWidget(chkAutoScale);
  layColorScale->addSpacing(taiM->hsep_c);

  cbar = new HCScaleBar(&tlv->colorscale, ScaleBar::RANGE, true, true, widg);
//  cbar->setMaximumWidth(30);
  connect(cbar, SIGNAL(scaleValueChanged()), this, SLOT(cbar_scaleValueChanged()) );
  layColorScale->addWidget(cbar); // stretchfact=1 so it stretches to fill the space

  butSetColor = new QPushButton("Colors", widg);
  butSetColor->setFixedHeight(taiM->button_height(taiMisc::sizSmall));
  butSetColor->setMinimumWidth(taiM->maxButtonWidth() / 2);
  layColorScale->addWidget(butSetColor);
  connect(butSetColor, SIGNAL(pressed()), this, SLOT(butSetColor_pressed()) );

  ////////////////////////////////////////////////////////////////////////////
  layClickVals = new QHBoxLayout; layWidg->addLayout(layClickVals);
  layClickVals->setSpacing(2);	// plenty of room

  chkClickVals =  new QCheckBox("Click\nVals", widg); chkClickVals->setObjectName( "chkClickVals");
  chkClickVals->setToolTip("If on, then clicking on cell values in the grid view display in interact mode (red arrow) will change the values.");
  connect(chkClickVals, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
  layClickVals->addWidget(chkClickVals);
  layClickVals->addSpacing(taiM->hsep_c);

  lblLMBVal = taiM->NewLabel("Left Click\nValue", widg, font_spec);
  lblLMBVal->setToolTip("Value that will be set in the cell if you click with the left mouse button (if Click Vals is on).");
  layClickVals->addWidget(lblLMBVal);
  fldLMBVal = dl.Add(new taiField(&TA_float, this, NULL, widg));
  layClickVals->addWidget(fldLMBVal->GetRep());
  ((iLineEdit*)fldLMBVal->GetRep())->setCharWidth(8);

  lblMMBVal = taiM->NewLabel("Middle Click\nValue", widg, font_spec);
  lblMMBVal->setToolTip("Value that will be set in the cell if you click with the middle mouse button (if Click Vals is on).");
  layClickVals->addWidget(lblMMBVal);
  fldMMBVal = dl.Add(new taiField(&TA_float, this, NULL, widg));
  layClickVals->addWidget(fldMMBVal->GetRep());
  ((iLineEdit*)fldMMBVal->GetRep())->setCharWidth(8);

  layClickVals->addStretch();

  ////////////////////////////////////////////////////////////////////////////
  layRowNav = new QHBoxLayout; layWidg->addLayout(layRowNav);
  //  layRowNav->setSpacing(2);	// plenty of room

  rowNavTB = new QToolBar(widg);
  layRowNav->addWidget(rowNavTB);

  lblRowGoto = taiM->NewLabel("Row: 00000 Goto", widg, font_spec);
  lblRowGoto->setToolTip("Row number to go to when the Go button is pressed -- rest of buttons provide one-click movment of the visible row in grid view");
  rowNavTB->addWidget(lblRowGoto);
  fldRowGoto = dl.Add(new taiField(&TA_int, this, NULL, widg));
  rowNavTB->addWidget(fldRowGoto->GetRep());
  ((iLineEdit*)fldRowGoto->GetRep())->setCharWidth(6);
  connect(fldRowGoto->rep(), SIGNAL(returnPressed()), this, SLOT(butRowGoto_pressed()) );

  actRowGoto = rowNavTB->addAction("Go");
  actRowGoto->setToolTip("Go to row number indicated in prior field");
  connect(actRowGoto, SIGNAL(triggered()), this, SLOT(butRowGoto_pressed()) );

  rowNavTB->addSeparator();

  actRowBackAll = rowNavTB->addAction("|<");
  actRowBackAll->setToolTip("Back all the way to first row");
  connect(actRowBackAll, SIGNAL(triggered()), this, SLOT(RowBackAll()) );

  actRowBackPg = rowNavTB->addAction("<<");
  actRowBackPg->setToolTip("Back one page of rows");
  connect(actRowBackPg, SIGNAL(triggered()), this, SLOT(RowBackPg()) );

  actRowBack1 = rowNavTB->addAction("<");
  actRowBack1->setToolTip("Back one row");
  connect(actRowBack1, SIGNAL(triggered()), this, SLOT(RowBack1()) );

  actRowFwd1 = rowNavTB->addAction(">" );
  actRowFwd1->setToolTip("Forward one row");
  connect(actRowFwd1, SIGNAL(triggered()), this, SLOT(RowFwd1()) );

  actRowFwdPg = rowNavTB->addAction(">>" );
  actRowFwdPg->setToolTip("Forward one page of rows");
  connect(actRowFwdPg, SIGNAL(triggered()), this, SLOT(RowFwdPg()) );

  actRowFwdAll = rowNavTB->addAction(">|" );
  actRowFwdAll->setToolTip("Forward all the way to the end");
  connect(actRowFwdAll, SIGNAL(triggered()), this, SLOT(RowFwdAll()) );

  layRowNav->addStretch();

  ////////////////////////////////////////////////////////////////////////////
  layColNav = new QHBoxLayout; layWidg->addLayout(layColNav);
  //  layColNav->setSpacing(2);	// plenty of room

  colNavTB = new QToolBar(widg);
  layColNav->addWidget(colNavTB);

  lblColGoto = taiM->NewLabel("Col: 000 Goto", widg, font_spec);
  lblColGoto->setToolTip("Col number to go to when the Go button is pressed -- rest of buttons provide one-click movment of the visible col in grid view");
  colNavTB->addWidget(lblColGoto);
  fldColGoto = dl.Add(new taiField(&TA_int, this, NULL, widg));
  colNavTB->addWidget(fldColGoto->GetRep());
  ((iLineEdit*)fldColGoto->GetRep())->setCharWidth(6);
  connect(fldColGoto->rep(), SIGNAL(returnPressed()), this, SLOT(butColGoto_pressed()) );

  actColGoto = colNavTB->addAction("Go");
  actColGoto->setToolTip("Go to col number indicated in prior field");
  connect(actColGoto, SIGNAL(triggered()), this, SLOT(butColGoto_pressed()) );

  colNavTB->addSeparator();

  actColBackAll = colNavTB->addAction("|<");
  actColBackAll->setToolTip("Back all the way to first col");
  connect(actColBackAll, SIGNAL(triggered()), this, SLOT(ColBackAll()) );

  actColBackPg = colNavTB->addAction("<<");
  actColBackPg->setToolTip("Back one page of cols");
  connect(actColBackPg, SIGNAL(triggered()), this, SLOT(ColBackPg()) );

  actColBack1 = colNavTB->addAction("<");
  actColBack1->setToolTip("Back one col");
  connect(actColBack1, SIGNAL(triggered()), this, SLOT(ColBack1()) );

  actColFwd1 = colNavTB->addAction(">" );
  actColFwd1->setToolTip("Forward one col");
  connect(actColFwd1, SIGNAL(triggered()), this, SLOT(ColFwd1()) );

  actColFwdPg = colNavTB->addAction(">>" );
  actColFwdPg->setToolTip("Forward one page of cols");
  connect(actColFwdPg, SIGNAL(triggered()), this, SLOT(ColFwdPg()) );

  actColFwdAll = colNavTB->addAction(">|" );
  actColFwdAll->setToolTip("Forward all the way to the end");
  connect(actColFwdAll, SIGNAL(triggered()), this, SLOT(ColFwdAll()) );

  layColNav->addStretch();

  ////////////////////
  // all done..

  layWidg->addStretch();

  MakeButtons(layOuter);
}

iGridTableView_Panel::~iGridTableView_Panel() {
}

void iGridTableView_Panel::InitPanel_impl() {
  // nothing structural here (could split out cols, but not worth it)
}

void iGridTableView_Panel::GetValue_impl() {
  inherited::GetValue_impl(); // prob nothing

  GridTableView* glv = this->glv(); //cache
  if (!glv) return;
  
  glv->display_on = chkDisplay->isChecked();
  glv->manip_ctrl_on = chkManip->isChecked();
  glv->header_on = chkHeaders->isChecked();
  glv->row_num_on = chkRowNum->isChecked();
  glv->two_d_font = chk2dFont->isChecked();
  glv->two_d_font_scale = (float)fldFontScale->GetValue();
  glv->view_rows = (int)fldRows->GetValue();
  glv->col_n = (int)fldCols->GetValue();
  glv->setWidth((float)fldWidth->GetValue());
  glv->text_size_range.min = fldTxtMin->GetValue();
  glv->text_size_range.max = fldTxtMax->GetValue();
  glv->mat_val_text = chkValText->isChecked();
  glv->mat_trans = (float)fldTrans->GetValue();
  glv->mat_rot = (float)fldRot->GetValue();
  glv->mat_block_height = (float)fldBlockHeight->GetValue();
  glv->setScaleData(chkAutoScale->isChecked(), cbar->min(), cbar->max());
  glv->click_vals = chkClickVals->isChecked();
  glv->lmb_val = (float)fldLMBVal->GetValue();
  glv->mmb_val = (float)fldMMBVal->GetValue();
  
  glv->UpdateDisplay(false); // don't update us, because logic will do that anyway
}

void iGridTableView_Panel::CopyFrom_impl() {
  GridTableView* glv = this->glv(); //cache
  if (!glv) return;
  glv->CallFun("CopyFromView");
}

void iGridTableView_Panel::UpdatePanel_impl() {
  inherited::UpdatePanel_impl();

  GridTableView* glv = this->glv(); //cache
  if (!glv) return;

  chkDisplay->setChecked(glv->display_on);
  chkManip->setChecked(glv->manip_ctrl_on);
  chkHeaders->setChecked(glv->header_on);
  chkRowNum->setChecked(glv->row_num_on);
  chk2dFont->setChecked(glv->two_d_font);
  fldFontScale->GetImage((String)glv->two_d_font_scale);

  fldRows->GetImage((String)glv->view_rows);
  fldCols->GetImage((String)glv->col_n);
  fldWidth->GetImage((String)glv->width);
  fldTxtMin->GetImage((String)glv->text_size_range.min);
  fldTxtMax->GetImage((String)glv->text_size_range.max);

  chkValText->setChecked(glv->mat_val_text);
  fldTrans->GetImage((String)glv->mat_trans);
  fldRot->GetImage((String)glv->mat_rot);
  fldBlockHeight->GetImage((String)glv->mat_block_height);

  cbar->UpdateScaleValues();
  chkAutoScale->setChecked(glv->colorscale.auto_scale);

  chkClickVals->setChecked(glv->click_vals);
  fldLMBVal->GetImage((String)glv->lmb_val);
  fldMMBVal->GetImage((String)glv->mmb_val);

  String rwtxt = "Row: " + taMisc::LeadingZeros(glv->view_range.min, 5) + " Goto";
  lblRowGoto->setText(rwtxt);
  String cltxt = "Col: " + taMisc::LeadingZeros(glv->col_range.min, 3) + " Goto";
  lblColGoto->setText(cltxt);
}

void iGridTableView_Panel::butRefresh_pressed() {
  GridTableView* glv = this->glv(); //cache
  if (updating || !glv) return;

  glv->InitDisplay();
  glv->UpdateDisplay();
}

void iGridTableView_Panel::butRowGoto_pressed() {
  GridTableView* glv = this->glv(); //cache
  if (updating || !glv) return;

  glv->ViewRow_At(fldRowGoto->GetValue());
}

void iGridTableView_Panel::RowBackAll() {
  GridTableView* glv = this->glv(); //cache
  if (updating || !glv) return;
  glv->RowBackAll();
}
void iGridTableView_Panel::RowBackPg() {
  GridTableView* glv = this->glv(); //cache
  if (updating || !glv) return;
  glv->RowBackPg();
}
void iGridTableView_Panel::RowBack1() {
  GridTableView* glv = this->glv(); //cache
  if (updating || !glv) return;
  glv->RowBack1();
}
void iGridTableView_Panel::RowFwd1() {
  GridTableView* glv = this->glv(); //cache
  if (updating || !glv) return;
  glv->RowFwd1();
}
void iGridTableView_Panel::RowFwdPg() {
  GridTableView* glv = this->glv(); //cache
  if (updating || !glv) return;
  glv->RowFwdPg();
}
void iGridTableView_Panel::RowFwdAll() {
  GridTableView* glv = this->glv(); //cache
  if (updating || !glv) return;
  glv->RowFwdAll();
}

void iGridTableView_Panel::butColGoto_pressed() {
  GridTableView* glv = this->glv(); //cache
  if (updating || !glv) return;

  glv->ViewCol_At(fldColGoto->GetValue());
}

void iGridTableView_Panel::ColBackAll() {
  GridTableView* glv = this->glv(); //cache
  if (updating || !glv) return;
  glv->ColBackAll();
}
void iGridTableView_Panel::ColBackPg() {
  GridTableView* glv = this->glv(); //cache
  if (updating || !glv) return;
  glv->ColBackPg();
}
void iGridTableView_Panel::ColBack1() {
  GridTableView* glv = this->glv(); //cache
  if (updating || !glv) return;
  glv->ColBack1();
}
void iGridTableView_Panel::ColFwd1() {
  GridTableView* glv = this->glv(); //cache
  if (updating || !glv) return;
  glv->ColFwd1();
}
void iGridTableView_Panel::ColFwdPg() {
  GridTableView* glv = this->glv(); //cache
  if (updating || !glv) return;
  glv->ColFwdPg();
}
void iGridTableView_Panel::ColFwdAll() {
  GridTableView* glv = this->glv(); //cache
  if (updating || !glv) return;
  glv->ColFwdAll();
}

void iGridTableView_Panel::butSetColor_pressed() {
  GridTableView* glv = this->glv(); //cache
  if (updating || !glv) return;

  glv->CallFun("SetColorSpec");
}

void iGridTableView_Panel::cbar_scaleValueChanged() {
  GridTableView* glv = this->glv(); //cache
  if (updating || !glv) return;

  //note: user changed value, so must no longer be autoscale
  ++updating;
  chkAutoScale->setChecked(false); //note: raises signal on widget! (grr...)
  --updating;
  Changed();
//  glv->setScaleData(false, cbar->min(), cbar->max());
}


///////////////////////////////////////////////////////////////////////////////
//  	Graph View

#define UNIT_LEGEND_OFFSET 0.04f // space between end of axis and unit legend text
#define TICK_OFFSET 0.01f // gap between tick and label

float GraphTableView::tick_size = 0.05f;

//////////////////////////////////
//   GraphColView		//
//////////////////////////////////

void GraphColView::Initialize(){
}

void GraphColView::Destroy() {
//   cerr << "grid col nuking" << endl;
}

void GraphColView::InitLinks() {
  inherited::InitLinks();
}

void GraphColView::CopyFromView(GraphColView* cp){
  fixed_range = cp->fixed_range;
}

String GraphColView::GetDisplayName() const {
  DataCol* dc = dataCol(); //note: exists, because we were called
  if(dc) return dc->GetDisplayName();
  return inherited::GetDisplayName();
}

//////////////////////////
// 	GraphAxisBase	//
//////////////////////////

void GraphAxisBase::Initialize() {
  on = true;
  axis = Y;
  col_lookup = NULL;
  col_list = NULL;
  color.name = taMisc::t3d_text_color;
  n_ticks = 10;
  axis_length = 1.0f;
  start_tick = 0;
  tick_incr = 1;
  act_n_ticks = 11;
  units = 1.;
}

void GraphAxisBase::Destroy() {
  CutLinks();
}

void GraphAxisBase::InitLinks() {
  inherited::InitLinks();
  taBase::Own(range, this);
  taBase::Own(data_range, this);
  taBase::Own(fixed_range, this);
  taBase::Own(color, this);

  GraphTableView* gtv = GetGTV();
  if(gtv) {
    taBase::SetPointer((taBase**)&col_list, &(gtv->children));
  }
}

void GraphAxisBase::CutLinks() {
  if(col_lookup)
    taBase::SetPointer((taBase**)&col_lookup, NULL);
  if(col_list)
    taBase::SetPointer((taBase**)&col_list, NULL);
  fixed_range.CutLinks();
  data_range.CutLinks();
  range.CutLinks();
  color.CutLinks();
  inherited::CutLinks();
}

void GraphAxisBase::CopyFromView_base(GraphAxisBase* cp){
  on = cp->on;
  col_name = cp->col_name;
  fixed_range= cp->fixed_range;
  color= cp->color;
  n_ticks = cp->n_ticks;
}

void GraphAxisBase::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  color.UpdateAfterEdit_NoGui();
  UpdateFmColLookup();
  UpdateOnFlag();
}

void GraphAxisBase::UpdateOnFlag() {
  // nop at base level
}

void GraphAxisBase::UpdateFmColLookup() {
  if(col_lookup) {
    // first save fixed_range values back to that guy
    GraphColView* gcv = GetColPtr();
    if(gcv) {
      gcv->fixed_range = fixed_range;
    }
    col_name = col_lookup->GetName();
//     cerr << "updated fm col: " << col_name << endl;
    fixed_range = col_lookup->fixed_range;	     // get range from that guy
    if(taBase::GetRefn(col_lookup) <= 1) {
//       cerr << "oops: refn <= 1" << endl;
    }
    else {
      taBase::SetPointer((taBase**)&col_lookup, NULL); // reset as soon as used -- just a temp guy!
    }
  }
}

void GraphAxisBase::SetColPtr(GraphColView* cgv) {
  taBase::SetPointer((taBase**)&col_lookup, cgv);
  UpdateFmColLookup();
  UpdateOnFlag();
}


GraphColView* GraphAxisBase::GetColPtr() {
  if(col_name.empty()) return NULL;
  GraphTableView* gv = (GraphTableView*)owner;
  if(!gv) return NULL;
  return (GraphColView*)gv->children.FindName(col_name);
}

DataCol* GraphAxisBase::GetDAPtr() {
  GraphColView* cv = GetColPtr();
  if(!cv) return NULL;
  return cv->dataCol();
}

bool GraphAxisBase::isString() {
  DataCol* da = GetDAPtr();
  if(!da) return false;
  return da->isString();
}

void GraphAxisBase::InitFromUserData() {
  GraphColView* cv = GetColPtr();
  if(!cv) return;
  DataCol* da = cv->dataCol();
  if(da->HasUserData("MIN")) {
    fixed_range.fix_min = true;
    fixed_range.min = da->GetUserDataAsFloat("MIN");
  }
  if(da->HasUserData("MAX")) {
    fixed_range.fix_max = true;
    fixed_range.max = da->GetUserDataAsFloat("MAX");
  }
}

///////////////////////////////////////////////////
// 	Range Management

void GraphAxisBase::SetRange_impl(float first, float last) {
  data_range.Set(first, last);
  // update to relevant actual data values..
  if(!fixed_range.fix_min) fixed_range.min = data_range.min;
  if(!fixed_range.fix_max) fixed_range.max = data_range.max;
  fixed_range.FixRange(data_range); // keep range fixed!
  range = data_range;
  UpdateRange_impl(first, last);
}

// limits are needed to prevent numerical overflow!
static const float range_min_limit = 1.0e-18f;
static const float range_zero_range = 5.0e-17f; // half-range for zero-range values
static const float range_zero_label_range = 5.0e-6f; // for tick labels

// updates range based on new data and returns true if it really is a new range
bool GraphAxisBase::UpdateRange_impl(float first, float last) {
  data_range.UpdateRange(first);
  data_range.UpdateRange(last);

  if(((first >= range.min) && (last <= range.max)) && (range.Range() >= range_min_limit)) { // special case
//     cerr << "not changing range: " << first << ", " << last << " rng: " << range.min << ", " << range.max << endl;
    return false;			// not changed
  }

  first = data_range.min;
  last = data_range.max;

  if (((last - first) < range_min_limit) || (range.Range() < range_min_limit)) {
    if (last - first >= range_min_limit) {
      range.max = last;
      range.min = first;
    } else {
      double maxval = MAX(fabs(last), fabs(first));
      if (maxval > .01f) {
	last += range_zero_range * maxval;
	first -= range_zero_range * maxval;
      } else {
	last += .51 * range_min_limit;
	first -= .51 * range_min_limit;
      }
    }
  }

  double rng = nicenum(last - first, false); // nicenum def'd in minmax
  rng = MAX(range_min_limit, rng);

  double inc = nicenum((rng / (double)n_ticks), true);
  double newmin = floor(first / inc) * inc;
  double newmax = ceil(last / inc) * inc;

//   cerr << "rng: " << rng << ", inc: " << inc << ", newmin: " << newmin << ", newmax: " << newmax << endl;

  range.Set(newmin, newmax);
  fixed_range.FixRange(range);
  return true;			// changed
}

void GraphAxisBase::ComputeRange() {
  if(!on) return;
  GraphColView* gcv = GetColPtr();
  if(gcv) {
    gcv->dataCol()->GetMinMaxScale(data_range);
  }
  else {
    data_range.min = 0.0f; data_range.max = 1.0f;
  }
  SetRange_impl(data_range.min, data_range.max);
}

bool GraphAxisBase::UpdateRange() {
  bool rval = false;
  GraphColView* gcv = GetColPtr();
  if(gcv) {
    DataCol* da = gcv->dataCol();
    if(da->rows() > 0) {
      if(da->is_matrix) {
	float first = da->GetValAsFloatM(-1,0);
	float last = first;
	for(int i=1;i<da->cell_size();i++) {
	  float val = da->GetValAsFloatM(-1,i);
	  first = MIN(val, first);
	  last = MAX(val, last);
	}
	rval = UpdateRange_impl(first, last);
      }
      else {
	float lstval = da->GetValAsFloat(-1);
	rval = UpdateRange_impl(lstval, lstval);
      }
    }
  }
  return rval;
}

void GraphAxisBase::ComputeTicks() {
  double min = range.min;
  double max = range.max;
  double rng = nicenum(max-min, false);
  if(rng == 0.0) {
    double unitsval = fabs(min);
    if(unitsval == 0.0)
      units = 1.0;
    else
      units = log10(unitsval);
    start_tick = min;
    tick_incr = 1.0 * units;
    act_n_ticks = 1;
    return;
  }

//   if(rng < 1.0e-8) rng = 1.0e-8;

  tick_incr = nicenum((rng / (double)n_ticks), true);
  double newmin = floor(min / tick_incr) * tick_incr;

  // make sure we start within range
  start_tick = newmin;
  while(start_tick < min) start_tick += tick_incr;

  double units_val = MAX(fabs(min), fabs(max));
  int units_order;
  if (units_val == 0)
    units_order = 0;
  else
    units_order = (int)log10(units_val);

  units_order /= 3;		// introduce a factor of 2 rounding here..
  units_order *= 3;

  units = pow(10.0, (double)units_order);

  // empirically compute the actual number of ticks..
  double chk_max = max + (tick_incr / 100.0f); // give a little tolerance..
  double val;
  int i;
  for (i=0, val = start_tick; val <= chk_max; val += tick_incr, i++);
  act_n_ticks = i;
}

/////////////////////////////////////////////////////////
//	rendering

void GraphAxisBase::RenderAxis(T3Axis* t3ax, int n_ax, bool ticks_only) {
  t3ax->clear();
  if(!on) return;
  ComputeTicks();		// do this always..
  SoMaterial* mat = t3ax->material();
  color.color().copyTo(mat->diffuseColor);
  switch (axis) {
  case X: RenderAxis_X(t3ax, ticks_only);
    break;
  case Y: RenderAxis_Y(t3ax, n_ax, ticks_only);
    break;
  case Z: RenderAxis_Z(t3ax, ticks_only);
    break;
  }
}

/* Axis rendering notes
  act_n_ticks is the number of sections, so tick marks will be +1 (to include ends)

*/
void GraphAxisBase::RenderAxis_X(T3Axis* t3ax, bool ticks_only) {
  iVec3f fm;			// init to 0
  iVec3f to;

  // axis line itself
  to.x = axis_length;
  t3ax->addLine(fm, to);

  bool use_str_labels = false;

  if(!ticks_only) { 
    // units legend
    if(units != 1.0) {
      fm.x = axis_length + UNIT_LEGEND_OFFSET;
      fm.y = -(.5f * GraphTableView::tick_size + TICK_OFFSET + t3ax->fontSize());
      String label = "x " + String(units,"%.5g");
      t3ax->addLabel(label.chars(), fm, SoAsciiText::LEFT);
    }

    if(!col_name.empty()) {
      fm.x = .5f * axis_length;
      fm.y = -(GraphTableView::tick_size + TICK_OFFSET + 1.5f * t3ax->fontSize());
      String label = col_name;
      taMisc::SpaceLabel(label);
      if(((GraphAxisView*)this)->row_num) {
	if(isString()) {
	  use_str_labels = true;
	}
	else {
	  label = "Row Number";
	}
      }
      t3ax->addLabel(label.chars(), fm, SoAsciiText::CENTER);
    }
  }

  // ticks
  fm = 0.0f;
  to = 0.0f;
  fm.y = -(.5f * GraphTableView::tick_size);
  to.y =  (.5f * GraphTableView::tick_size);

  float y_lab_off = (TICK_OFFSET + t3ax->fontSize());

  DataCol* da = GetDAPtr();

  int i;
  float val;
  String label;
  for (i = 0, val = start_tick; i < act_n_ticks; val += tick_incr, ++i) {
    fm.x = DataToPlot(val);
    to.x = DataToPlot(val);
    t3ax->addLine(fm, to);
    if(!ticks_only) {
      float lab_val = val / units;
      if (fabsf(val / tick_incr) < range_zero_label_range)
	lab_val = 0.0f;		// the 0 can be screwy
      label = String(lab_val);
      if(use_str_labels && da) {
	int rnum = (int)lab_val;// lab_val is row number!
	if((float)rnum == lab_val && rnum >= 0 && rnum < da->rows()) // only int and in range
	  label = da->GetValAsString(rnum);
	else
	  label = "";		// empty it!
      }
      if(label.nonempty()) {
	t3ax->addLabel(label.chars(),
		     iVec3f(fm.x, fm.y - y_lab_off, fm.z),
		     SoAsciiText::CENTER);
      }
    }
  }
}

void GraphAxisBase::RenderAxis_Y(T3Axis* t3ax, int n_ax, bool ticks_only) {
  iVec3f fm;
  iVec3f to;

  // axis line itself
  to.y = axis_length;
  t3ax->addLine(fm, to);

  if(!ticks_only) { 
    // units legend
    if(units != 1.0) {
      fm.y = axis_length + UNIT_LEGEND_OFFSET;
      String label = "x " + String(units,"%.5g");
      if(n_ax > 0) {
	fm.x = TICK_OFFSET;
	t3ax->addLabel(label.chars(), fm, SoAsciiText::LEFT);
      }
      else {
	fm.x = -TICK_OFFSET;
	t3ax->addLabel(label.chars(), fm, SoAsciiText::RIGHT);
      }
    }

    if(!col_name.empty()) {
      SbRotation rot;
      rot.setValue(SbVec3f(0.0, 0.0f, 1.0f), .5f * taMath_float::pi);
      fm.y = .5f * axis_length;
      String label = col_name; taMisc::SpaceLabel(label);
      if(n_ax > 0) {
	fm.x = GraphTableView::tick_size + TICK_OFFSET + 1.2f * t3ax->fontSize();
	t3ax->addLabelRot(label.chars(), fm, SoAsciiText::CENTER, rot);
      }
      else {
	fm.x = -GraphTableView::tick_size - TICK_OFFSET - 1.2f * t3ax->fontSize();
	t3ax->addLabelRot(label.chars(), fm, SoAsciiText::CENTER, rot);
      }
    }
  }

  // ticks
  fm = 0.0f;
  to = 0.0f;
  fm.x = -(GraphTableView::tick_size / 2.0f);
  to.x =  (GraphTableView::tick_size / 2.0f);

  int i;
  float val;
  String label;
  for (i = 0, val = start_tick; i < act_n_ticks; val += tick_incr, ++i) {
    fm.y = DataToPlot(val);
    to.y = DataToPlot(val);
    t3ax->addLine(fm, to);
    if(!ticks_only) {
      float lab_val = val / units;
      if (fabsf(val / tick_incr) < range_zero_label_range)
	lab_val = 0.0f;		// the 0 can be screwy
      label = String(lab_val);
      if(n_ax > 0) {
	t3ax->addLabel(label.chars(),
		       iVec3f(to.x + TICK_OFFSET, fm.y - (.5f * t3ax->fontSize()), fm.z));
      }
      else {
	t3ax->addLabel(label.chars(),
		       iVec3f(fm.x - TICK_OFFSET, fm.y - (.5f * t3ax->fontSize()), fm.z));
      }
    }
  }
}

void GraphAxisBase::RenderAxis_Z(T3Axis* t3ax, bool ticks_only) {
  iVec3f fm;
  iVec3f to;

  // axis line itself
  to.z = axis_length;
  t3ax->addLine(fm, to);

  bool use_str_labels = false;

  if(!ticks_only) {
    // units legend
    if(units != 1.0) {
      fm.z = axis_length + UNIT_LEGEND_OFFSET;
      fm.y = -(.5f * GraphTableView::tick_size + TICK_OFFSET + t3ax->fontSize());
      fm.x = -(TICK_OFFSET + 2.0f * t3ax->fontSize());
      String label = "x " + String(units,"%.5g");
      t3ax->addLabel(label.chars(), fm, SoAsciiText::RIGHT);
    }
    if(!col_name.empty()) {
      SbRotation rot;
      rot.setValue(SbVec3f(0.0, 1.0f, 0.0f), .5f * taMath_float::pi);

      fm.z = .5f * axis_length;
      fm.y = -(.5f * GraphTableView::tick_size + TICK_OFFSET + t3ax->fontSize());
      fm.x = -(TICK_OFFSET + 2.5f * t3ax->fontSize());
      String label = col_name;
      taMisc::SpaceLabel(label);
      if(((GraphAxisView*)this)->row_num) {
	if(isString()) {
	  use_str_labels = true;
	}
	else {
	  label = "Row Number";
	}
      }
      t3ax->addLabelRot(label.chars(), fm, SoAsciiText::CENTER, rot);
    }
  }

  // ticks
  fm = 0.0f;
  to = 0.0f;
  fm.x = -(.5f * GraphTableView::tick_size);
  to.x =  (.5f * GraphTableView::tick_size);
  
  float y_lab_off = (.5f * GraphTableView::tick_size + TICK_OFFSET + t3ax->fontSize());

  DataCol* da = GetDAPtr();

  int i;
  float val;
  String label;
  for (i = 0, val = start_tick; i < act_n_ticks; val += tick_incr, ++i) {
    fm.z = DataToPlot(val);
    to.z = DataToPlot(val);
    t3ax->addLine(fm, to);
    if(!ticks_only) {
      float lab_val = val / units;
      if (fabsf(val / tick_incr) < range_zero_label_range)
	lab_val = 0.0f;		// the 0 can be screwy
      label = String(lab_val);
      if(use_str_labels && da) {
	int rnum = (int)lab_val;// lab_val is row number!
	if(rnum >= 0 && rnum < da->rows())
	  label = da->GetValAsString(rnum); 
      }
      if(label.nonempty()) {
	t3ax->addLabel(label.chars(),
		       iVec3f(fm.x - TICK_OFFSET, fm.y - y_lab_off, fm.z));
      }
    }
  }
}

//////////////////////////////////
//  GraphPlotView		//
//////////////////////////////////

void GraphPlotView::Initialize() {
  axis = Y;
  line_style = SOLID;
  point_style = CIRCLE;
  alt_y = false;
  eff_y_axis = NULL;
}

// void GraphPlotView::InitLinks() {
//   inherited::InitLinks();
// }

// void GraphPlotView::CutLinks() {
//   inherited::CutLinks();
// }

void GraphPlotView::CopyFromView(GraphPlotView* cp){
  CopyFromView_base(cp);	// get the base
  line_style = cp->line_style;
  point_style = cp->point_style;
  alt_y = cp->alt_y;
}

void GraphPlotView::UpdateOnFlag() {
  if(on) {
    if(!GetColPtr())
      on = false; // not actually on!
  }
}

void GraphPlotView::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();

  // just to make srue!
  if (line_style < (LineStyle)T3GraphLine::LineStyle_MIN)
    line_style = (LineStyle)T3GraphLine::LineStyle_MIN;
  else if (line_style > (LineStyle)T3GraphLine::LineStyle_MAX)
    line_style = (LineStyle)T3GraphLine::LineStyle_MAX;

  if (point_style < (PointStyle)T3GraphLine::MarkerStyle_MIN)
    point_style = (PointStyle)T3GraphLine::MarkerStyle_MIN;
  else if (point_style > (PointStyle)T3GraphLine::MarkerStyle_MAX)
    point_style = (PointStyle)T3GraphLine::MarkerStyle_MAX;
}

//////////////////////////////////
//  GraphAxisView		//
//////////////////////////////////

void GraphAxisView::Initialize() {
  row_num = false;
}

void GraphAxisView::CopyFromView(GraphAxisView* cp){
  CopyFromView_base(cp);	// get the base
  row_num = cp->row_num;
}

void GraphAxisView::UpdateOnFlag() {
  if(on) {
    if(!row_num && !GetColPtr())
      on = false; // not actually on!
  }
}

void GraphAxisView::ComputeRange() {
  if(!row_num) {
    inherited::ComputeRange();
    return;
  }
  // ROW_NUM
  GraphTableView* gv = GetGTV();
  SetRange_impl(gv->view_range.min, gv->view_range.max);
}

bool GraphAxisView::UpdateRange() {
  if(!row_num)
    return inherited::UpdateRange();
  // ROW_NUM
  GraphTableView* gv = GetGTV();
  return UpdateRange_impl(gv->view_range.min, gv->view_range.max);
}


//////////////////////////////////
//  GraphTableView		//
//////////////////////////////////

// Add a new GraphTableView object to the frame for the given DataTable.
GraphTableView* GraphTableView::New(DataTable* dt, T3DataViewFrame*& fr) {
  NewNetViewHelper newNetView(fr, dt, "table");
  if (!newNetView.isValid()) return NULL;

  GraphTableView* vw = new GraphTableView;
  fr->AddView(vw);
  vw->setDataTable(dt);
  // make sure we get it all setup!
  vw->BuildAll();

  newNetView.showFrame();
  return vw;
}

void GraphTableView::Initialize() {
  view_rows = 10000;

  x_axis.axis = GraphAxisBase::X;
  x_axis.color.name = taMisc::t3d_text_color;
  x_axis.color.UpdateAfterEdit_NoGui();	// needed to pick up color name
  z_axis.axis = GraphAxisBase::Z;
  z_axis.color.name = taMisc::t3d_text_color;
  z_axis.color.UpdateAfterEdit_NoGui();	// needed to pick up color name
  plot_1.color.name = taMisc::t3d_text_color;
  plot_1.point_style = GraphPlotView::CIRCLE;
  plot_1.color.UpdateAfterEdit_NoGui();	// needed to pick up color name
  plot_2.color.name = "red";
  plot_2.point_style = GraphPlotView::SQUARE;
  plot_2.color.UpdateAfterEdit_NoGui();
  plot_3.color.name = "blue";
  plot_3.point_style = GraphPlotView::DIAMOND;
  plot_3.color.UpdateAfterEdit_NoGui();
  plot_4.color.name = "green3";
  plot_4.point_style = GraphPlotView::TRIANGLE;
  plot_4.color.UpdateAfterEdit_NoGui();
  plot_5.color.name = "purple";
  plot_5.point_style = GraphPlotView::PLUS;
  plot_5.color.UpdateAfterEdit_NoGui();
  plot_6.color.name = "orange";
  plot_6.point_style = GraphPlotView::CROSS;
  plot_6.color.UpdateAfterEdit_NoGui();
  plot_7.color.name = "brown";
  plot_7.point_style = GraphPlotView::STAR;
  plot_7.color.UpdateAfterEdit_NoGui();
  plot_8.color.name = "chartreuse";
  plot_8.point_style = GraphPlotView::MINUS;
  plot_8.color.UpdateAfterEdit_NoGui();
  // todo: following obsolete: nuke
  alt_y_1 = false;
  alt_y_2 = false;
  alt_y_3 = false;
  alt_y_4 = false;
  alt_y_5 = false;

  graph_type = XY;
  plot_style = LINE;
  line_width = 2.0f;
  point_size = MEDIUM;
  point_spacing = 1;
  bar_space = .2f;
  color_mode = VALUE_COLOR;
  negative_draw = false;
  negative_draw_z = true;
  axis_font_size = .05f;
  label_font_size = .04f;
  label_spacing = -1;
  matrix_mode = SEP_GRAPHS;
  mat_layout = taMisc::BOT_ZERO;
  mat_odd_vert = true;
  scrolling_ = false;

  err_1.axis = GraphAxisBase::Y;
  err_2.axis = GraphAxisBase::Y;
  err_3.axis = GraphAxisBase::Y;
  err_4.axis = GraphAxisBase::Y;
  err_5.axis = GraphAxisBase::Y;
  err_6.axis = GraphAxisBase::Y;
  err_7.axis = GraphAxisBase::Y;
  err_8.axis = GraphAxisBase::Y;
  err_spacing = 1;
  err_bar_width = .02f;

  color_axis.axis = GraphAxisBase::Y;
  raster_axis.axis = GraphAxisBase::Y;

  thresh = .5f;
  thr_line_len = .48f;

  width	= 1.0f;
  depth = 1.0f;

  two_d_font = false;	// true -- this is causing mysterious so crash, disabled for now
  two_d_font_scale = 350.0f;

  last_sel_pt = 0.0f;

  colorscale.auto_scale = false;
  colorscale.min = -1.0f; colorscale.max = 1.0f;

  children.SetBaseType(&TA_GraphColView); // subclasses need to set this to their type!

  n_plots = 1;
  do_matrix_plot = false;
  t3_x_axis = NULL;
  t3_x_axis_top = NULL;
  t3_x_axis_far = NULL;
  t3_x_axis_far_top = NULL;
  t3_y_axis = NULL;
  t3_y_axis_rt = NULL;
  t3_y_axis_far = NULL;
  t3_y_axis_far_rt = NULL;
  t3_z_axis = NULL;
  t3_z_axis_rt = NULL;
  t3_z_axis_top = NULL;
  t3_z_axis_top_rt = NULL;
}

void GraphTableView::InitLinks() {
  inherited::InitLinks();
  taBase::Own(x_axis, this);
  taBase::Own(z_axis, this);
  taBase::Own(plot_1, this);
  taBase::Own(plot_2, this);
  taBase::Own(plot_3, this);
  taBase::Own(plot_4, this);
  taBase::Own(plot_5, this);
  taBase::Own(plot_6, this);
  taBase::Own(plot_7, this);
  taBase::Own(plot_8, this);
  taBase::Own(err_1, this);
  taBase::Own(err_2, this);
  taBase::Own(err_3, this);
  taBase::Own(err_4, this);
  taBase::Own(err_5, this);
  taBase::Own(err_6, this);
  taBase::Own(err_7, this);
  taBase::Own(err_8, this);
  taBase::Own(color_axis, this);
  taBase::Own(raster_axis, this);
  taBase::Own(colorscale, this);
  taBase::Own(last_sel_pt, this);

  taBase::Own(main_y_plots, this);
  taBase::Own(alt_y_plots, this);

  all_plots[0] = &plot_1;
  all_plots[1] = &plot_2;
  all_plots[2] = &plot_3;
  all_plots[3] = &plot_4;
  all_plots[4] = &plot_5;
  all_plots[5] = &plot_6;
  all_plots[6] = &plot_7;
  all_plots[7] = &plot_8;

  all_errs[0] = &err_1;
  all_errs[1] = &err_2;
  all_errs[2] = &err_3;
  all_errs[3] = &err_4;
  all_errs[4] = &err_5;
  all_errs[5] = &err_6;
  all_errs[6] = &err_7;
  all_errs[7] = &err_8;
}

void GraphTableView::CutLinks() {
  x_axis.CutLinks();
  z_axis.CutLinks();
  plot_1.CutLinks();
  plot_2.CutLinks();
  plot_3.CutLinks();
  plot_4.CutLinks();
  plot_5.CutLinks();
  plot_6.CutLinks();
  plot_7.CutLinks();
  plot_8.CutLinks();
  err_1.CutLinks();
  err_2.CutLinks();
  err_3.CutLinks();
  err_4.CutLinks();
  err_5.CutLinks();
  err_6.CutLinks();
  err_7.CutLinks();
  err_8.CutLinks();
  color_axis.CutLinks();
  raster_axis.CutLinks();
  colorscale.CutLinks();
  inherited::CutLinks();
}

void GraphTableView::CopyFromView(GraphTableView* cp) {
  graph_type = cp->graph_type;
  plot_style = cp->plot_style;
  negative_draw = cp->negative_draw;
  negative_draw_z = cp->negative_draw_z;
  line_width = cp->line_width;
  point_size = cp->point_size;
  point_spacing = cp->point_spacing;
  bar_space = cp->bar_space;
  label_spacing = cp->label_spacing;
  width = cp->width;
  depth = cp->depth;
  axis_font_size = cp->axis_font_size;
  label_font_size = cp->label_font_size;
  
  x_axis.CopyFromView(&(cp->x_axis));
  z_axis.CopyFromView(&(cp->z_axis));
  for(int i=0;i<max_plots;i++) {
    all_plots[i]->CopyFromView(cp->all_plots[i]);
    all_errs[i]->CopyFromView(cp->all_errs[i]);
  }

  err_spacing = cp->err_spacing;
  err_bar_width = cp->err_bar_width;

  color_mode = cp->color_mode;
  color_axis.CopyFromView(&(cp->color_axis));
  colorscale = cp->colorscale;

  raster_axis.CopyFromView(&(cp->raster_axis));
  thresh = cp->thresh;
  thr_line_len= cp->thr_line_len;
  matrix_mode = cp->matrix_mode;
  mat_layout = cp->mat_layout;
  mat_odd_vert = cp->mat_odd_vert;

  two_d_font = cp->two_d_font;
  two_d_font_scale = cp->two_d_font_scale;
  
  for(int i=0;i<children.size;i++) {
    GraphColView* cvs = (GraphColView*)colView(i);
    GraphColView* cpvs = (GraphColView*)cp->children.FindName(cvs->name);
    if(cpvs) {
      cvs->CopyFromView(cpvs);
    }
  }
  T3DataViewMain::CopyFromViewFrame(cp);
}

void GraphTableView::UpdateAfterEdit_impl(){
  inherited::UpdateAfterEdit_impl();

  // capture saved alt_y vals and reset in favor of new ones on the plot_x guys
  // todo: obsolete stuff should be removed after some point
  if(alt_y_1) { plot_1.alt_y = true; alt_y_1 = false; } 
  if(alt_y_2) { plot_2.alt_y = true; alt_y_1 = false; } 
  if(alt_y_3) { plot_3.alt_y = true; alt_y_1 = false; } 
  if(alt_y_4) { plot_4.alt_y = true; alt_y_1 = false; } 
  if(alt_y_5) { plot_5.alt_y = true; alt_y_1 = false; } 

  if(taMisc::is_loading) return;

  point_spacing = MAX(1, point_spacing);

  x_axis.axis_length = width;
  z_axis.axis_length = depth;

  x_axis.on = true;		// try to keep it on
  x_axis.UpdateOnFlag();
  z_axis.UpdateOnFlag();
  for(int i=0;i<max_plots;i++) {
    GraphPlotView* pl = all_plots[i];
    pl->UpdateOnFlag();
    GraphPlotView* epl = all_errs[i];
    epl->UpdateOnFlag();
  }
  color_axis.UpdateOnFlag();
  raster_axis.UpdateOnFlag();

  DataTable* dt = dataTable();
  bool no_cols = true;
  if(dt && dt->data.size > 0)
    no_cols = false;

  ////////////////////// update y axes in use, etc

  main_y_plots.Reset();
  alt_y_plots.Reset();
  do_matrix_plot = false;
  n_plots = 0;
  int first_mat = -1;
  for(int i=0;i<max_plots;i++) {
    GraphPlotView* pl = all_plots[i];
    if(!pl->on) continue;
    n_plots++;
    if(pl->alt_y) alt_y_plots.Add(i);
    else	  main_y_plots.Add(i);
    GraphColView* plc = all_plots[i]->GetColPtr();
    if(plc->dataCol()->is_matrix) {
      if(!do_matrix_plot) first_mat = i;
      do_matrix_plot = true;
    }
  }
  if(do_matrix_plot && n_plots > 1) {
    n_plots = 1;
    taMisc::Warning("GraphTableView -- a data column to be plotted is a matrix, so all other plots are being turned off (matrix can only be plotted by itself)");
    main_y_plots.Reset();
    alt_y_plots.Reset();
    for(int i=0;i<max_plots;i++) {
      GraphPlotView* pl = all_plots[i];
      if(!pl->on) continue;
      if(i == first_mat) continue;
      pl->on = false;		// turn off
      if(pl->alt_y) alt_y_plots.RemoveIdx(i);
      else	    main_y_plots.RemoveIdx(i);
    }
  }

  if(main_y_plots.size > 0) {
    GraphPlotView* pl = all_plots[main_y_plots[0]];
    if(pl->isString()) {
      int non_str = -1;
      for(int i=1; i< main_y_plots.size; i++) {
	if(!all_plots[main_y_plots[i]]->isString()) {
	  non_str = i;
	  break;
	}
      }

      if(non_str == -1) {
	taMisc::Warning("GraphTableView -- can't have only a string value(s) plotted on main Y axis -- must also include a numeric column to provide Y axis values -- turning off plot!");
	for(int i=0; i< main_y_plots.size; i++) {
	  all_plots[main_y_plots[i]]->on = false;
	}
	n_plots -= main_y_plots.size;
	main_y_plots.Reset();
      }
      else {
	main_y_plots.SwapIdx(0,non_str); // exchange
      }
    }
  }

  if(alt_y_plots.size > 0) {
    GraphPlotView* pl = all_plots[alt_y_plots[0]];
    if(pl->isString()) {	// oops.. not again!
      int non_str = -1;
      for(int i=1; i< alt_y_plots.size; i++) {
	if(!all_plots[alt_y_plots[i]]->isString()) {
	  non_str = i;
	  break;
	}
      }

      if(non_str == -1) {
	taMisc::Warning("GraphTableView -- can't have only a string value(s) plotted on alt Y axis -- must also include a numeric column to provide Y axis values -- turning off plot!");
	for(int i=0; i< alt_y_plots.size; i++) {
	  all_plots[alt_y_plots[i]]->on = false;
	}
	n_plots -= alt_y_plots.size;
	alt_y_plots.Reset();
      }
      else {
	alt_y_plots.SwapIdx(0,non_str); // exchange
      }
    }
  }

  if(n_plots > 0 && main_y_plots.size == 0) { // no first y's so must be alts -- switch them!
    main_y_plots = alt_y_plots;
    alt_y_plots.Reset();
  }

  mainy = NULL;
  if(main_y_plots.size > 0) mainy = all_plots[main_y_plots[0]];
  alty = NULL;
  if(alt_y_plots.size > 0) alty = all_plots[alt_y_plots[0]];

  ///////////////////////////////////////////////////////////
  // other axes

  if(!x_axis.on) {
    if(!no_cols) {
      taMisc::Warning("GraphTableView -- X axis is not on -- perhaps no valid col_name found for x_axis -- switching to row_num!");
      x_axis.row_num = true;
      x_axis.on = true;
    }
  }
  else {
    if(x_axis.isString()) {
      x_axis.row_num = true;	// must be row num!
    }
    if(!x_axis.row_num) {
      GraphColView* xa = x_axis.GetColPtr();
      if(xa->dataCol()->is_matrix) {
	taMisc::Warning("GraphTableView -- column", x_axis.col_name, "is a matrix -- cannot be used for an X axis -- resetting to default");
	FindDefaultXZAxes();
      }
    }
  }

  if(z_axis.isString()) {
    z_axis.row_num = true;	// must be row num!
  }

  if(graph_type == RASTER) {
    if((plot_style != THRESH_LINE) && (plot_style !=  THRESH_POINT))
      color_mode = VALUE_COLOR;
    raster_axis.on = true;
    raster_axis.UpdateOnFlag();
    if(!raster_axis.on) {
      if(!no_cols) {
	taMisc::Warning("GraphTableView -- graph_type = RASTER and no valid col_name found for raster_axis -- nothing will be plotted!");
      }
    }
    else {
      GraphColView* ra = raster_axis.GetColPtr();
      if(ra->dataCol()->is_matrix) {
	taMisc::Warning("GraphTableView -- column", raster_axis.col_name, "is a matrix -- cannot be used for raster_axis");
	raster_axis.col_name = "";
	raster_axis.on = false;
      }
    }
  }

  if(color_mode == COLOR_AXIS) {
    color_axis.on = true;
    color_axis.UpdateOnFlag();
    if(!color_axis.on) {
      if(!no_cols) {
	taMisc::Warning("GraphTableView -- color_mode = COLOR_AXIS and no valid col_name found for color_axis -- nothing will be plotted!");
      }
    }
    else {
      GraphColView* ca = color_axis.GetColPtr();
      if(ca->dataCol()->is_matrix) {
	taMisc::Warning("GraphTableView -- column", color_axis.col_name, "is a matrix -- cannot be used for color_axis");
	color_axis.col_name = "";
	color_axis.on = false;
      }
    }
  }
}

const iColor GraphTableView::bgColor(bool& ok) const {
  ok = true;
  return colorscale.background;
}

void GraphTableView::UpdateName() {
  DataTable* dt = dataTable();
  if (dt) {
    if (!name.contains(dt->name))
      SetName(dt->name + "_Graph");
  }
  else {
    if (name.empty())
      SetName("graph_no_table");
  }
}
  
const String GraphTableView::caption() const {
  String rval = inherited::caption();
  if(last_sel_col_nm.nonempty()) {
    rval += " pt: " + last_sel_col_nm + " = " + last_sel_pt.GetStr();
  }
  return rval;
}

void GraphTableView_MouseCB(void* userData, SoEventCallback* ecb) {
  GraphTableView* nv = (GraphTableView*)userData;
  T3DataViewFrame* fr = nv->GetFrame();
  SoMouseButtonEvent* mouseevent = (SoMouseButtonEvent*)ecb->getEvent();
  SoMouseButtonEvent::Button but = mouseevent->getButton();
  if(!SoMouseButtonEvent::isButtonReleaseEvent(mouseevent, but)) return; // only releases
  bool got_one = false;
  for(int i=0;i<fr->root_view.children.size;i++) {
    taDataView* dv = fr->root_view.children[i];
    if(dv->InheritsFrom(&TA_GraphTableView)) {
      GraphTableView* tgv = (GraphTableView*)dv;
      T3ExaminerViewer* viewer = tgv->GetViewer();
      SoRayPickAction rp( viewer->getViewportRegion());
      rp.setPoint(mouseevent->getPosition());
      rp.apply(viewer->quarter->getSoEventManager()->getSceneGraph()); // event mgr has full graph!

      SoPickedPoint* pp = rp.getPickedPoint(0);
      if(!pp) continue;
      SoNode* pobj = pp->getPath()->getNodeFromTail(2);
      if(!pobj) continue;
      //   cerr << "obj typ: " << pobj->getTypeId().getName() << endl;
      if(!pobj->isOfType(T3GraphLine::getClassTypeId())) {
	//     cerr << "not graph line!" << endl;
	continue;
      }
      GraphAxisBase* gab = static_cast<GraphAxisBase*>(((T3GraphLine*)pobj)->dataView());
      if(!gab) continue;
      if(!gab->InheritsFrom(&TA_GraphPlotView)) continue;
      GraphPlotView* gpv = (GraphPlotView*)gab;
      if(!gpv->eff_y_axis) continue;
      SbVec3f pt = pp->getObjectPoint(pobj); 
//       cerr << "got: " << pt[0] << " " << pt[1] << " " << pt[2] << endl;
      if(pt[1] == 0.0f && pt[2] == 0.0f) continue; // indicates an axis caption line!
      float xv = tgv->x_axis.range.Project(pt[0] / tgv->width);
      float yv = gpv->eff_y_axis->range.Project(pt[1]);
      float zv = 0.0f;
      if(tgv->z_axis.on)
	zv = tgv->z_axis.range.Project(pt[2] / tgv->depth);
      tgv->last_sel_pt.SetXYZ(xv, yv, zv);
      tgv->last_sel_col_nm = gpv->col_name;
      //      tgv->InitDisplay();
      tgv->UpdateDisplay();
      got_one = true;
      break;			// once you get one, stop!
    }
  }
  if(got_one)
    ecb->setHandled();
}

void GraphTableView::Render_pre() {
  bool show_drag = manip_ctrl_on;
  T3ExaminerViewer* vw = GetViewer();
  if(vw) {
    vw->syncViewerMode();
    if(!vw->interactionModeOn())
      show_drag = false;
  }

  setNode(new T3GraphViewNode(this, width, show_drag));

/*NOTES:
1. Render_pre is only done once, or on STRUCT changes
2. item should not take ownsership of a shared frame prop like bg color
3. we can set the frame bg color to the default for graphs

*/

  SoEventCallback* ecb = new SoEventCallback;
  ecb->addEventCallback(SoMouseButtonEvent::getClassTypeId(), GraphTableView_MouseCB, this);
  node_so()->addChild(ecb);

  inherited::Render_pre();
}

void GraphTableView::Render_impl() {
  inherited::Render_impl();
  T3GraphViewNode* node_so = this->node_so(); // cache
  if(!node_so || !dataTable())
    return;
  
  node_so->setWidth(width);	// does a render too -- ensure always up to date on width
  int orig_rows;
  CheckRowsChanged(orig_rows);	// don't do anything with this here, but just make sure m_rows is up to date
  MakeViewRangeValid();
  ComputeAxisRanges();
  SetScrollBars();
  RenderGraph();
  //  UpdatePanel();		// otherwise doesn't get updated without explicit click..
  // nein!  this is called in UpdateDisplay -- gets called 2x with this!
}

void GraphTableView::Render_post() {
  inherited::Render_post();
}

void GraphTableView::InitDisplay(bool init_panel) {
  MakeViewRangeValid();
  RemoveGraph();
  if(init_panel) {
    InitPanel();
  }
}

void GraphTableView::UpdateDisplay(bool update_panel) {
  int old_rows;
  int delta_rows = CheckRowsChanged(old_rows);

  if (!isVisible()) return;

  if(delta_rows > 0) {
    // if we were not at the very end, then don't scroll, but do update the panel
    if(update_panel && (view_range.max < old_rows-1) && (view_range.max > 0)) {
      UpdatePanel();
      return;
    }
    // scroll down to end of new data
    view_range.max = m_rows - 1; 
    view_range.min = view_range.max - view_rows + 1;
    view_range.min = MAX(0, view_range.min);
  }

  if (update_panel) UpdatePanel();
  Render_impl();
}

void GraphTableView::ComputeAxisRanges() {
  UpdateAfterEdit_impl();

  x_axis.ComputeRange();
  if(do_matrix_plot && matrix_mode == Z_INDEX && mainy) {
    DataCol* da_y = mainy->GetDAPtr();
    if(da_y && da_y->is_matrix) {
      z_axis.SetRange_impl(0.0f, da_y->cell_size());
    }
    else {
      z_axis.ComputeRange();
    }
  }
  else {
    z_axis.ComputeRange();
  }

  for(int i=0;i<max_plots;i++) {
    GraphPlotView* pl = all_plots[i];
    pl->ComputeRange();
  }

  if(mainy) {
    for(int i=1;i<main_y_plots.size;i++) {
      GraphPlotView* pl = all_plots[main_y_plots[i]];
      if(pl->isString()) continue;
      mainy->UpdateRange_impl(pl->data_range.min, pl->data_range.max);
    }
  }

  if(alty) {
    for(int i=1;i<alt_y_plots.size;i++) {
      GraphPlotView* pl = all_plots[alt_y_plots[i]];
      if(pl->isString()) continue;
      alty->UpdateRange_impl(pl->data_range.min, pl->data_range.max);
    }
  }

  if(color_mode == COLOR_AXIS)
    color_axis.ComputeRange();
  if(graph_type == RASTER)
    raster_axis.ComputeRange();
}

void GraphTableView_RowScrollCB(SoScrollBar* sb, int val, void* user_data) {
  GraphTableView* gtv = (GraphTableView*)user_data;
  gtv->scrolling_ = true;
  gtv->ViewRow_At(val);
  gtv->scrolling_ = false;
}

void GraphTableView::SetScrollBars() {
  if(scrolling_) return;		     // don't redo if currently doing!
  T3GraphViewNode* node_so = this->node_so(); // cache
  if(!node_so) return;

  SoScrollBar* rsb = node_so->RowScrollBar();
  //  rsb->setMinimum(0);
  //  rsb->setSingleStep(1);
  int mx = MAX((rows() - view_rows), 0);
  rsb->setMaximum(mx);
  int pg_step = MAX(view_rows, 1);
  rsb->setPageStep(pg_step);
  rsb->setValue(MIN(view_range.min, mx));
  rsb->setValueChangedCB(GraphTableView_RowScrollCB, this);
}

void GraphTableView::Clear_impl() {
  RemoveGraph();
  inherited::Clear_impl();
}

void GraphTableView::OnWindowBind_impl(iT3DataViewFrame* vw) {
  inherited::OnWindowBind_impl(vw);
  if (!m_lvp) {
    m_lvp = new iGraphTableView_Panel(this);
    vw->RegisterPanel(m_lvp);
  }
}

void GraphTableView::RemoveGraph(){
  T3GraphViewNode* node_so = this->node_so();
  if (!node_so) return;
  node_so->graphs()->removeAllChildren();
  node_so->y_axes()->removeAllChildren();
}

void GraphTableView::FindDefaultXZAxes() {
  // first, look for explicit flags
  bool set_x = false;
  z_axis.on = false;		// assume we're not going to find it
  for(int i=children.size-1;i>=0;i--) {
    GraphColView* cvs = (GraphColView*)colView(i);
    DataCol* da = cvs->dataCol();
    if(da->HasUserData("X_AXIS")) {
      x_axis.col_name = cvs->name;
      x_axis.InitFromUserData();
      x_axis.UpdateOnFlag();
      set_x = true;
    }
    if(da->HasUserData("Z_AXIS")) {
      z_axis.col_name = cvs->name;
      z_axis.on = true;
      z_axis.InitFromUserData();
      z_axis.UpdateOnFlag();
    }
  }
  if(set_x) return;

  // then, find X axis by getting *last* int col (first searching backwards), then Z is next
  for(int i=children.size-1;i>=0;i--) {
    GraphColView* cvs = (GraphColView*)colView(i);
    DataCol* da = cvs->dataCol();
    if(da->is_matrix || da->valType() != VT_INT) continue;
    if(set_x) {			// must be Z
      z_axis.col_name = cvs->name;
      z_axis.on = true;		// found one
      z_axis.InitFromUserData();
      z_axis.UpdateOnFlag();
      break;			// done
    }
    else {
      x_axis.col_name = cvs->name;
      x_axis.InitFromUserData();
      x_axis.UpdateOnFlag();
      set_x = true;
    }
  }
  if(!set_x) {			// didn't find it -- look for first numeric column, just for x
    for(int i=0;i<children.size;i++) {
      GraphColView* cvs = (GraphColView*)colView(i);
      DataCol* da = cvs->dataCol();
      if(da->is_matrix || !da->isNumeric()) continue;
      x_axis.col_name = cvs->name;
      x_axis.InitFromUserData();
      x_axis.UpdateOnFlag();
      set_x = true;
      break;
    }
  }
}

void GraphTableView::FindDefaultPlot1() {
  bool got_1 = false;
  for(int i=children.size-1;i>=0;i--) {
    GraphColView* cvs = (GraphColView*)colView(i);
    DataCol* da = cvs->dataCol();
    if(da->HasUserData("PLOT_1")) {
      plot_1.col_name = cvs->name;
      plot_1.InitFromUserData();
      plot_1.UpdateOnFlag();
      got_1 = true;
    }
  }
  if(got_1) return;

  // next, find first float/double and that is the plot_1
  for(int i=0;i<children.size;i++) {
    GraphColView* cvs = (GraphColView*)colView(i);
    DataCol* da = cvs->dataCol();
    if((da->valType() != VT_FLOAT) && (da->valType() != VT_DOUBLE)) continue;
    if(x_axis.col_name == cvs->name) continue; // don't make it same as X!
    plot_1.col_name = cvs->name;
    plot_1.InitFromUserData();
    plot_1.UpdateOnFlag();
    break;			// once you get one, that's it
  }
}


void GraphTableView::InitFromUserData() {
  FindDefaultXZAxes();
  FindDefaultPlot1();

  // next, find first float/double and that is the plot_1
  for(int i=0;i<children.size;i++) {
    GraphColView* cvs = (GraphColView*)colView(i);
    DataCol* da = cvs->dataCol();
    for(int i=1; i<max_plots; i++) {
      String pltst = "PLOT_" + String(i+1);
      if(da->HasUserData(pltst)) {
	all_plots[i]->col_name = cvs->name;  all_plots[i]->on = true;
	all_plots[i]->InitFromUserData();    all_plots[i]->UpdateOnFlag();
      }
    }
    for(int i=0; i<max_plots; i++) {
      String pltst = "ERR_" + String(i+1);
      if(da->HasUserData(pltst)) {
	all_errs[i]->col_name = cvs->name;  all_errs[i]->on = true;
	all_errs[i]->InitFromUserData();    all_errs[i]->UpdateOnFlag();
      }
    }
    if(da->HasUserData("COLOR_AXIS")) {
      color_axis.col_name = cvs->name;   color_axis.on = true;
      color_axis.InitFromUserData();     color_axis.UpdateOnFlag();
    }
    if(da->HasUserData("RASTER_AXIS")) {
      raster_axis.col_name = cvs->name;  raster_axis.on = true;
      raster_axis.InitFromUserData();    raster_axis.UpdateOnFlag();
    }
  }

  DataTable* dt = dataTable();
  if(dt->HasUserData("NO_Z_AXIS")) {
    z_axis.on = false;
  }
  String enum_tp_nm;
  if(dt->HasUserData("PLOT_STYLE")) {
    int pstv = GetEnumVal(dt->GetUserDataAsString("PLOT_STYLE"), enum_tp_nm);
    if(pstv >= 0)
      plot_style = (PlotStyle)pstv;
  }
  if(dt->HasUserData("GRAPH_TYPE")) {
    int pstv = GetEnumVal(dt->GetUserDataAsString("GRAPH_TYPE"), enum_tp_nm);
    if(pstv >= 0)
      graph_type = (GraphType)pstv;
  }
  if(dt->HasUserData("POINT_SIZE")) {
    int pstv = GetEnumVal(dt->GetUserDataAsString("POINT_SIZE"), enum_tp_nm);
    if(pstv >= 0)
      point_size = (PointSize)pstv;
  }
  if(dt->HasUserData("COLOR_MODE")) {
    int pstv = GetEnumVal(dt->GetUserDataAsString("COLOR_MODE"), enum_tp_nm);
    if(pstv >= 0)
      color_mode = (ColorMode)pstv;
  }
  if(dt->HasUserData("MATRIX_MODE")) {
    int pstv = GetEnumVal(dt->GetUserDataAsString("MATRIX_MODE"), enum_tp_nm);
    if(pstv >= 0)
      matrix_mode = (MatrixMode)pstv;
  }
  if(dt->HasUserData("LINE_WIDTH")) {
    line_width = dt->GetUserDataAsFloat("LINE_WIDTH");
  }
  if(dt->HasUserData("POINT_SPACING")) {
    point_spacing = dt->GetUserDataAsInt("POINT_SPACING");
  }
  if(dt->HasUserData("LABEL_SPACING")) {
    label_spacing = dt->GetUserDataAsInt("LABEL_SPACING");
  }
  if(dt->HasUserData("BAR_SPACE")) {
    bar_space = dt->GetUserDataAsFloat("BAR_SPACE");
  }
  if(dt->HasUserData("NEG_DRAW")) {
    negative_draw = dt->GetUserDataAsBool("NEG_DRAW");
  }
  if(dt->HasUserData("NEG_DRAW_Z")) {
    negative_draw_z = dt->GetUserDataAsBool("NEG_DRAW_Z");
  }
}

void GraphTableView::UpdateFromDataTable_this(bool first) {
  inherited::UpdateFromDataTable_this(first);
  if(!first) return;

  InitFromUserData();
}

///////////////////////////////////////////////////////////////
//	Actual Rendering of graph display

void GraphTableView::RenderGraph() {
//   cerr << "render graph" << endl;
  UpdateAfterEdit_impl();
  if(n_plots == 0 || !x_axis.on) return;

  RenderAxes();

  if(do_matrix_plot && mainy && mainy->GetDAPtr()->is_matrix) {
    if(matrix_mode == SEP_GRAPHS)
      RenderGraph_Matrix_Sep();
    else
      RenderGraph_Matrix_Zi();
  }
  else {
    RenderLegend();
    if(graph_type == BAR)
      RenderGraph_Bar();
    else
      RenderGraph_XY();
  }
}


void GraphTableView::RenderAxes() {
  T3GraphViewNode* node_so = this->node_so();
  if (!node_so) return;

  if(!x_axis.on || !mainy) return;

  SoSeparator* xax = node_so->x_axis();
  xax->removeAllChildren();
  SoSeparator* zax = node_so->z_axis();
  zax->removeAllChildren();
  SoSeparator* yax = node_so->y_axes();
  yax->removeAllChildren();

  t3_x_axis = new T3Axis((T3Axis::Axis)x_axis.axis, &x_axis, axis_font_size);
  t3_x_axis_top = new T3Axis((T3Axis::Axis)x_axis.axis, &x_axis, axis_font_size);

  xax->addChild(t3_x_axis);

  float ylen = plot_1.axis_length;

  SoTranslation* tr;
  // top
  tr = new SoTranslation();  xax->addChild(tr);
  tr->translation.setValue(0.0f, ylen, 0.0f);
  xax->addChild(t3_x_axis_top);

  x_axis.RenderAxis(t3_x_axis);
  x_axis.RenderAxis(t3_x_axis_top, 0, true); // ticks only

  if(z_axis.on) {
    t3_x_axis_far = new T3Axis((T3Axis::Axis)x_axis.axis, &x_axis, axis_font_size);
    t3_x_axis_far_top = new T3Axis((T3Axis::Axis)x_axis.axis, &x_axis, axis_font_size);

    // far_top
    tr = new SoTranslation();   xax->addChild(tr);
    tr->translation.setValue(0.0f, 0.0, -z_axis.axis_length);
    xax->addChild(t3_x_axis_far_top);
    // far
    tr = new SoTranslation();  xax->addChild(tr);
    tr->translation.setValue(0.0f, -ylen, 0.0f);
    xax->addChild(t3_x_axis_far);

    x_axis.RenderAxis(t3_x_axis_far_top, 0, true); // ticks only
    x_axis.RenderAxis(t3_x_axis_far, 0, true); // ticks only

    /////////////  Z
    SoSeparator* zax = node_so->z_axis();
    t3_z_axis = new T3Axis((T3Axis::Axis)z_axis.axis, &z_axis, axis_font_size);
    t3_z_axis_rt = new T3Axis((T3Axis::Axis)z_axis.axis, &z_axis, axis_font_size);
    t3_z_axis_top = new T3Axis((T3Axis::Axis)z_axis.axis, &z_axis, axis_font_size);
    t3_z_axis_top_rt = new T3Axis((T3Axis::Axis)z_axis.axis, &z_axis, axis_font_size);

    zax->addChild(t3_z_axis);
    z_axis.RenderAxis(t3_z_axis);

    // rt
    tr = new SoTranslation();   zax->addChild(tr);
    tr->translation.setValue(x_axis.axis_length, 0.0f, 0.0f);
    zax->addChild(t3_z_axis_rt);
    // top_rt
    tr = new SoTranslation();   zax->addChild(tr);
    tr->translation.setValue(0.0f, ylen, 0.0f);
    zax->addChild(t3_z_axis_top_rt);
    // top
    tr = new SoTranslation();   zax->addChild(tr);
    tr->translation.setValue(-x_axis.axis_length, 0.0f, 0.0f);
    zax->addChild(t3_z_axis_top);

    z_axis.RenderAxis(t3_z_axis_rt, 0, true); // ticks only
    z_axis.RenderAxis(t3_z_axis_top_rt, 0, true);
    z_axis.RenderAxis(t3_z_axis_top, 0, true);
  }
  else {
    t3_x_axis_far = NULL;
    t3_x_axis_far_top = NULL;
    t3_z_axis = NULL;
    t3_z_axis_rt = NULL;
    t3_z_axis_top = NULL;
    t3_z_axis_top_rt = NULL;
  }

  if(graph_type == RASTER) {
    t3_y_axis = new T3Axis((T3Axis::Axis)raster_axis.axis, &raster_axis, axis_font_size);
    raster_axis.RenderAxis(t3_y_axis); // raster axis is Y axis!
    yax->addChild(t3_y_axis);

  }
  else {
    t3_y_axis = new T3Axis((T3Axis::Axis)mainy->axis, mainy, axis_font_size);
    mainy->RenderAxis(t3_y_axis);
    yax->addChild(t3_y_axis);

    if(z_axis.on) {
      t3_y_axis_far = new T3Axis((T3Axis::Axis)mainy->axis, mainy, axis_font_size);
      mainy->RenderAxis(t3_y_axis_far, 0, true); // only ticks
      // far
      tr = new SoTranslation();   yax->addChild(tr);
      tr->translation.setValue(0.0f, 0.0f, -z_axis.axis_length);
      yax->addChild(t3_y_axis_far);
      tr = new SoTranslation();   yax->addChild(tr); // reset this guy for next..
      tr->translation.setValue(0.0f, 0.0f, z_axis.axis_length);
    }
    else {
      t3_y_axis_far = NULL;
    }

    if(alty) {
      t3_y_axis_rt = new T3Axis((T3Axis::Axis)alty->axis, alty, axis_font_size, 1); // second Y = 1
      alty->RenderAxis(t3_y_axis_rt, 1); // indicate second axis!
      tr = new SoTranslation();  yax->addChild(tr);
      tr->translation.setValue(x_axis.axis_length, 0.0f, 0.0f); // put on right hand side!
      yax->addChild(t3_y_axis_rt);		  

      if(z_axis.on) {
	t3_y_axis_far_rt = new T3Axis((T3Axis::Axis)alty->axis, alty, axis_font_size, 1);
	alty->RenderAxis(t3_y_axis_far_rt, 1, true); // only ticks
	tr = new SoTranslation();   yax->addChild(tr);
	tr->translation.setValue(0.0f, 0.0f, -z_axis.axis_length);
	yax->addChild(t3_y_axis_far_rt);
      }
      else {
	t3_y_axis_far_rt = NULL;
      }
    }
    else {
      // rt
      t3_y_axis_rt = new T3Axis((T3Axis::Axis)mainy->axis, mainy, axis_font_size);
      mainy->RenderAxis(t3_y_axis_rt, 0, true); // ticks
      tr = new SoTranslation();   yax->addChild(tr);
      tr->translation.setValue(x_axis.axis_length, 0.0f, 0.0f);
      yax->addChild(t3_y_axis_rt);

      if(z_axis.on) {
	t3_y_axis_far_rt = new T3Axis((T3Axis::Axis)mainy->axis, mainy, axis_font_size);
	mainy->RenderAxis(t3_y_axis_far_rt, 0, true); // only ticks
	tr = new SoTranslation();   yax->addChild(tr);
	tr->translation.setValue(0.0f, 0.0f, -z_axis.axis_length);
	yax->addChild(t3_y_axis_far_rt);
      }
      else {
	t3_y_axis_far_rt = NULL;
      }
    }
  }
}

void GraphTableView::RenderLegend_Ln(GraphPlotView& plv, T3GraphLine* t3gl) {
  t3gl->clear();
  t3gl->startBatch();
  t3gl->setLineStyle((T3GraphLine::LineStyle)plv.line_style, line_width);
  t3gl->setMarkerSize((T3GraphLine::MarkerSize)point_size);
  t3gl->setValueColorMode(false);
  t3gl->setDefaultColor((T3Color)(plv.color.color()));

  String label = plv.col_name; taMisc::SpaceLabel(label);

  iVec3f st;
  iVec3f ed;
  ed.x = 1.5f * label_font_size;
  t3gl->moveTo(st);
  t3gl->lineTo(ed);
  t3gl->textAt(iVec3f(ed.x + TICK_OFFSET,  ed.y - (.5f * label_font_size), ed.z),
	       label.chars());
  t3gl->finishBatch();
}

void GraphTableView::RenderLegend() {
  T3GraphViewNode* node_so = this->node_so();
  if (!node_so) return;

  float ylen = plot_1.axis_length;

  SoSeparator* leg = node_so->legend();
  leg->removeAllChildren();
  // move to top
  SoTranslation* tr;
  tr = new SoTranslation();  leg->addChild(tr);
  tr->translation.setValue(0.0f, ylen + 2.5f * axis_font_size, 0.0f);

  float over_amt = .33f * x_axis.axis_length;
  float dn_amt = -1.1f * axis_font_size;
  bool mv_dn = true;		// else over

  // keep in same order as plotting: main then alt
  for(int i=0;i<main_y_plots.size;i++) {
    GraphPlotView* pl = all_plots[main_y_plots[i]];
    T3GraphLine* ln = new T3GraphLine(pl, axis_font_size); leg->addChild(ln);
    RenderLegend_Ln(*pl, ln);
    tr = new SoTranslation();  leg->addChild(tr);
    if(mv_dn)    tr->translation.setValue(0.0f, dn_amt, 0.0f);
    else	 tr->translation.setValue(over_amt, -dn_amt, 0.0f);
    mv_dn = !mv_dn;		// flip
  }

  for(int i=0;i<alt_y_plots.size;i++) {
    GraphPlotView* pl = all_plots[alt_y_plots[i]];
    T3GraphLine* ln = new T3GraphLine(pl, axis_font_size); leg->addChild(ln);
    RenderLegend_Ln(*pl, ln);
    tr = new SoTranslation();  leg->addChild(tr);
    if(mv_dn)    tr->translation.setValue(0.0f, dn_amt, 0.0f);
    else	 tr->translation.setValue(over_amt, -dn_amt, 0.0f);
    mv_dn = !mv_dn;		// flip
  }
}

void GraphTableView::RenderGraph_XY() {
  T3GraphViewNode* node_so = this->node_so();
  if (!node_so) return;

  if(!mainy) return;

  DataCol* da_1 = mainy->GetDAPtr();
  if(!da_1) return;

  SoSeparator* graphs = node_so->graphs();
  graphs->removeAllChildren();

  SoSeparator* gr1 = new SoSeparator;
  graphs->addChild(gr1);

  float boxd = 0.0f;
  if(z_axis.on)
    boxd = depth;

  // each graph has a box and lines..
  SoLineBox3d* lbox = new SoLineBox3d(width, 1.0f, boxd, false); // not centered
  gr1->addChild(lbox);

  for(int i=0;i<main_y_plots.size;i++) {
    GraphPlotView* pl = all_plots[main_y_plots[i]];
    T3GraphLine* ln = new T3GraphLine(pl, label_font_size); gr1->addChild(ln);
    if(pl->isString()) {
      PlotData_String(*pl, *mainy, ln);
    }
    else {
      PlotData_XY(*pl, *all_errs[main_y_plots[i]], *mainy, ln);
    }
  }

  for(int i=0;i<alt_y_plots.size;i++) {
    GraphPlotView* pl = all_plots[alt_y_plots[i]];
    T3GraphLine* ln = new T3GraphLine(pl, label_font_size); gr1->addChild(ln);
    if(pl->isString()) {
      PlotData_String(*pl, *alty, ln);
    }
    else {
      PlotData_XY(*pl, *all_errs[alt_y_plots[i]], *alty, ln);
    }
  }
}

void GraphTableView::RenderGraph_Bar() {
  T3GraphViewNode* node_so = this->node_so();
  if (!node_so) return;

  if(n_plots <= 0) return;

  DataCol* da_1 = mainy->GetDAPtr();
  if(!da_1) return;

  SoSeparator* graphs = node_so->graphs();
  graphs->removeAllChildren();

  SoSeparator* gr1 = new SoSeparator;
  graphs->addChild(gr1);

  float boxd = 0.0f;
  if(z_axis.on)
    boxd = depth;

  int n_str = 0;
  for(int i=0;i<max_plots;i++) {
    GraphPlotView* pl = all_plots[i];
    if(pl->on && pl->isString()) n_str++;
  }

  bar_width = (1.0f - bar_space) / ((float)(n_plots - n_str));
  float bar_off = - .5f * (1.0f - bar_space);

  // each graph has a box and lines..
  SoLineBox3d* lbox = new SoLineBox3d(width, 1.0f, boxd, false); // not centered
  gr1->addChild(lbox);

  for(int i=0;i<main_y_plots.size;i++) {
    GraphPlotView* pl = all_plots[main_y_plots[i]];
    T3GraphLine* ln = new T3GraphLine(pl, label_font_size); gr1->addChild(ln);
    if(pl->isString()) {
      PlotData_String(*pl, *mainy, ln);
    }
    else {
      PlotData_Bar(*pl, *all_errs[main_y_plots[i]], *mainy, ln, bar_off);
      bar_off += bar_width;
    }
  }

  for(int i=0;i<alt_y_plots.size;i++) {
    GraphPlotView* pl = all_plots[alt_y_plots[i]];
    T3GraphLine* ln = new T3GraphLine(pl, label_font_size); gr1->addChild(ln);
    if(pl->isString()) {
      PlotData_String(*pl, *alty, ln);
    }
    else {
      PlotData_Bar(*pl, *all_errs[alt_y_plots[i]], *alty, ln, bar_off);
      bar_off += bar_width;
    }
  }
}

void GraphTableView::RenderGraph_Matrix_Zi() {
  T3GraphViewNode* node_so = this->node_so();
  if (!node_so) return;

  DataCol* da_1 = mainy->GetDAPtr();
  if(!da_1) return;

  SoSeparator* graphs = node_so->graphs();
  graphs->removeAllChildren();

  SoSeparator* gr1 = new SoSeparator;
  graphs->addChild(gr1);

  // each graph has a box and lines..
  SoLineBox3d* lbox = new SoLineBox3d(width, 1.0f, depth, false); // not centered
  gr1->addChild(lbox);

  for(int i=0;i<da_1->cell_size();i++) {
    T3GraphLine* ln = new T3GraphLine(mainy, label_font_size);
    gr1->addChild(ln);
    PlotData_XY(*mainy, *all_errs[main_y_plots[0]], *mainy, ln, i);
  }
}

void GraphTableView::RenderGraph_Matrix_Sep() {
  T3GraphViewNode* node_so = this->node_so();
  if (!node_so) return;

  DataCol* da_1 = mainy->GetDAPtr();
  if(!da_1) return;

  GraphPlotView* erry = all_errs[main_y_plots[0]];

  float boxd = .01f;
  if(z_axis.on)
    boxd = depth;

  SoSeparator* graphs = node_so->graphs();
  graphs->removeAllChildren();

  MatrixGeom& mgeom = da_1->cell_geom;

  int geom_x, geom_y;
  mgeom.Get2DGeomGui(geom_x, geom_y, mat_odd_vert, 1);
  float cl_x = 1.0f / (float)geom_x;	// how big each cell is
  float cl_y = 1.0f / (float)geom_y;
  float max_xy = MAX(cl_x, cl_y);

  TwoDCoord pos;
  int idx = 0;
  if(mgeom.dims() <= 2) {
    for(pos.y=0; pos.y<geom_y; pos.y++) {
      for(pos.x=0; pos.x<geom_x; pos.x++) { // right to left
	float xp = ((float)pos.x) * cl_x;
	float yp = ((float)pos.y) * cl_y;

	SoSeparator* gr = new SoSeparator;
	graphs->addChild(gr);

	SoTranslation* tr = new SoTranslation();
	gr->addChild(tr);
	tr->translation.setValue(xp, yp, 0.0f);

	// each graph has a box and lines..
	SoLineBox3d* lbox = new SoLineBox3d(width * cl_x, cl_y, boxd * max_xy, false); // not ctr
	gr->addChild(lbox);
	SoTransform* tx = new SoTransform();
	gr->addChild(tx);
	tx->scaleFactor.setValue(cl_x, cl_y, max_xy);
	T3GraphLine* ln = new T3GraphLine(mainy, label_font_size);
	gr->addChild(ln);
	if(mat_layout == taMisc::TOP_ZERO) {
	  if(mgeom.dims() == 1)
	    idx = MAX(geom_y-1-pos.y, pos.x);
	  else
	    idx = mgeom.IndexFmDims(pos.x, geom_y-1-pos.y);
	}
	else {
	  if(mgeom.dims() == 1)
	    idx = MAX(pos.y,pos.y);
	  else
	    idx = mgeom.IndexFmDims(pos.x, pos.y);
	}
	PlotData_XY(*mainy, *erry, *mainy, ln, idx);
      }
    }
  }
  else if(mgeom.dims() == 3) {
    int xmax = mgeom[0];
    int ymax = mgeom[1];
    int zmax = mgeom[2];
    for(int z=0; z<zmax; z++) {
      for(pos.y=0; pos.y<ymax; pos.y++) {
	for(pos.x=0; pos.x<xmax; pos.x++) {
	  TwoDCoord apos = pos;
	  if(mat_odd_vert)
	    apos.y += z * (ymax+1);
	  else
	    apos.x += z * (xmax+1);
	  float xp = ((float)apos.x) * cl_x;
	  float yp = ((float)apos.y) * cl_y;

	  SoSeparator* gr = new SoSeparator;
	  graphs->addChild(gr);

	  SoTranslation* tr = new SoTranslation();
	  gr->addChild(tr);
	  tr->translation.setValue(xp, yp, 0.0f);

	  // each graph has a box and lines..
	  SoLineBox3d* lbox = new SoLineBox3d(width * cl_x, cl_y, boxd * max_xy, false); // not ctr
	  gr->addChild(lbox);
	  SoTransform* tx = new SoTransform();
	  gr->addChild(tx);
	  tx->scaleFactor.setValue(cl_x, cl_y, max_xy);
	  T3GraphLine* ln = new T3GraphLine(mainy, label_font_size);
	  gr->addChild(ln);
	  if(mat_layout == taMisc::TOP_ZERO)
	    idx = mgeom.IndexFmDims(pos.x, ymax-1-pos.y, zmax-1-z);
	  else
	    idx = mgeom.IndexFmDims(pos.x, pos.y, z);
	  PlotData_XY(*mainy, *erry, *mainy, ln, idx);
	}
      }
    }
  }
  else if(mgeom.dims() == 4) {
    int xmax = mgeom[0];
    int ymax = mgeom[1];
    int xxmax = mgeom[2];
    int yymax = mgeom[3];
    TwoDCoord opos;
    for(opos.y=0; opos.y<yymax; opos.y++) {
      for(opos.x=0; opos.x<xxmax; opos.x++) {
	for(pos.y=0; pos.y<ymax; pos.y++) {
	  for(pos.x=0; pos.x<xmax; pos.x++) {
	    TwoDCoord apos = pos;
	    apos.x += opos.x * (xmax+1);
	    apos.y += opos.y * (ymax+1);
	    float xp = ((float)apos.x) * cl_x;
	    float yp = ((float)apos.y) * cl_y;

	    SoSeparator* gr = new SoSeparator;
	    graphs->addChild(gr);

	    SoTranslation* tr = new SoTranslation();
	    gr->addChild(tr);
	    tr->translation.setValue(xp, yp, 0.0f);

	    // each graph has a box and lines..
	    SoLineBox3d* lbox = new SoLineBox3d(width * cl_x, cl_y, boxd * max_xy, false); // not ctr
	    gr->addChild(lbox);
	    SoTransform* tx = new SoTransform();
	    gr->addChild(tx);
	    tx->scaleFactor.setValue(cl_x, cl_y, max_xy);
	    T3GraphLine* ln = new T3GraphLine(mainy, label_font_size);
	    gr->addChild(ln);
	    if(mat_layout == taMisc::TOP_ZERO)
	      idx = mgeom.IndexFmDims(pos.x, ymax-1-pos.y, opos.x, yymax-1-opos.y);
	    else
	      idx = mgeom.IndexFmDims(pos.x, pos.y, opos.x, opos.y);
	    PlotData_XY(*mainy, *erry, *mainy, ln, idx);
          }
	}
      }
    }
  }
}

const iColor GraphTableView::GetValueColor(GraphAxisBase* ax_clr, float val) {
  iColor clr;

  if (ax_clr->range.Range() == 0) {
    clr = colorscale.GetColor((int)((.5f * (float)(colorscale.chunks-1)) + .5f));
  } else if(val > ax_clr->range.max) {
    clr = colorscale.maxout.color();
  } else if(val < ax_clr->range.min) {
    clr = colorscale.minout.color();
  } else {
    int chnk = colorscale.chunks-1;
    float rval = ax_clr->range.Normalize(val);
    int idx = (int) ((rval * (float)chnk) + .5);
    idx = MAX(0, idx);
    idx = MIN(chnk, idx);
    clr = colorscale.GetColor(idx);
  }
  return clr;
}

void GraphTableView::PlotData_XY(GraphPlotView& plv, GraphPlotView& erv, GraphPlotView& yax, 
				 T3GraphLine* t3gl, int mat_cell) {
  t3gl->clear();

  DataCol* da_y = plv.GetDAPtr();
  if(!da_y) return;
  DataCol* da_x = x_axis.GetDAPtr();
  DataCol* da_z = z_axis.GetDAPtr();

  DataTable* dt = dataTable();

  // this should only happen if we have no data at all..
  if((view_range.min < 0) || (view_range.min >= dt->rows)) return;
  if((view_range.min < 0) || (view_range.min >= dt->rows)) return;

  plv.eff_y_axis = &yax;		// set this for point clicking!

  t3gl->startBatch();
  t3gl->setLineStyle((T3GraphLine::LineStyle)plv.line_style, line_width);
  t3gl->setMarkerSize((T3GraphLine::MarkerSize)point_size);

  DataCol* da_er = erv.GetDAPtr();

  GraphAxisBase* ax_clr = NULL;
  DataCol* da_clr = NULL;
  if(color_mode == FIXED_COLOR) {
    t3gl->setValueColorMode(false);
    t3gl->setDefaultColor((T3Color)(plv.color.color()));
  }
  else {
    t3gl->setValueColorMode(true);
    if(color_mode == VALUE_COLOR) {
      ax_clr = &yax;
      da_clr = plv.GetDAPtr();
    }
    else {
      ax_clr = &color_axis;
      da_clr = color_axis.GetDAPtr();
    }
    if(!da_clr) {		// fallback
      t3gl->setValueColorMode(false);
      t3gl->setDefaultColor((T3Color)(plv.color.color()));
    }
  }

  GraphAxisBase* ax_rst = NULL;
  DataCol* da_rst = NULL;
  if(graph_type == RASTER) {
    ax_rst = &raster_axis;
    da_rst = raster_axis.GetDAPtr();
  }

  bool matz = false;
  if((mat_cell >= 0) && (matrix_mode == Z_INDEX)) matz = true;

  float last_x;
  float last_z = 0.0f;
  if(z_axis.on) {
    if(x_axis.row_num)
      last_z = view_range.min;
    else
      last_z = da_z->GetValAsFloat(view_range.min);
  }
  if(x_axis.row_num)
    last_x = view_range.min;
  else
    last_x = da_x->GetValAsFloat(view_range.min);
  iVec3f dat;			// data point
  iVec3f plt;			// plot coords
  iVec3f th_st;			// start of thresholded line
  iVec3f th_ed;			// end of thresholded line
  bool first = true;
  for (int row = view_range.min; row <= view_range.max; row++) {
    iColor clr; // only used for color modes
    bool clr_ok = false;
    bool new_trace = false;
    bool new_trace_z = false;
    if(x_axis.row_num)
      dat.x = row;
    else
      dat.x = da_x->GetValAsFloat(row);
    if(dat.x < x_axis.range.min || dat.x > x_axis.range.max) continue;
    if(z_axis.on) {
      if(z_axis.row_num)
	dat.z = row;
      else if(da_z)
	dat.z = da_z->GetValAsFloat(row);
      if(dat.z < z_axis.range.min || dat.z > z_axis.range.max) continue;
    }
    if((mat_cell >= 0) && (matrix_mode == Z_INDEX)) {
      dat.z = mat_cell;
    }
    float yval; 
    if(mat_cell >= 0) {
      yval = da_y->GetValAsFloatM(row, mat_cell);
    }
    else {
      yval = da_y->GetValAsFloat(row);
    }
    if((graph_type == RASTER) && da_rst) {
      dat.y = da_rst->GetValAsFloat(row);
    }
    else {
      dat.y = yval;
    }

    plt.x = x_axis.DataToPlot(dat.x);
    if((graph_type == RASTER) && da_rst) {
      if(dat.y < ax_rst->range.min || dat.y > ax_rst->range.max) continue;
      plt.y = ax_rst->DataToPlot(dat.y);
    }
    else {
      if(dat.y < yax.range.min || dat.y > yax.range.max) continue;
      plt.y = yax.DataToPlot(dat.y);
    }
    if(z_axis.on || matz)
      plt.z = z_axis.DataToPlot(dat.z);

    if(dat.x < last_x)
      new_trace = true;
    if(z_axis.on && (dat.z < last_z))
      new_trace_z = true;


    if(da_clr) {
      clr_ok = true;
      if(color_mode == VALUE_COLOR) {
	clr = GetValueColor(ax_clr, yval);
      }
      else {
	clr = GetValueColor(ax_clr, da_clr->GetValAsFloat(row));
      }
    }

    // draw the line
    if(plot_style == LINE || plot_style == LINE_AND_POINTS) {
      if(first || (new_trace && !negative_draw) || (new_trace_z && !negative_draw_z))
      {	// just starting out
	if(clr_ok)
	  t3gl->moveTo(plt, (T3Color)(clr));
	else
	  t3gl->moveTo(plt);
      }
      else {
	if(clr_ok)
	  t3gl->lineTo(plt, (T3Color)(clr));
	else
	  t3gl->lineTo(plt);
      }
    }
    else if(plot_style == THRESH_LINE) {
      if(yval >= thresh) {
	th_st = plt;  th_st.x = x_axis.DataToPlot(dat.x - thr_line_len);
	th_ed = plt;  th_ed.x = x_axis.DataToPlot(dat.x + thr_line_len);
	if(clr_ok) {
	  t3gl->moveTo(th_st, (T3Color)(clr));
	  t3gl->lineTo(th_ed, (T3Color)(clr));
	}
	else {
	  t3gl->moveTo(th_st);
	  t3gl->moveTo(th_ed);
	}
      }
    }
    else if(plot_style == THRESH_POINT) {
      if(yval >= thresh) {
	if(clr_ok)
	  t3gl->markerAt(plt, (T3GraphLine::MarkerStyle)plv.point_style, (T3Color)(clr));
	else
	  t3gl->markerAt(plt, (T3GraphLine::MarkerStyle)plv.point_style);
      }
    }

    // render marker, if any
    if((plot_style == POINTS) || (plot_style == LINE_AND_POINTS)) {
      if(row % point_spacing == 0) {
	if(clr_ok)
	  t3gl->markerAt(plt, (T3GraphLine::MarkerStyle)plv.point_style, (T3Color)(clr));
	else
	  t3gl->markerAt(plt, (T3GraphLine::MarkerStyle)plv.point_style);
      }
    }

    if(erv.on && da_er && (row % err_spacing == 0)) {
      float err_dat;
      if(da_er->is_matrix && mat_cell >= 0) {
	err_dat = da_er->GetValAsFloatM(row, mat_cell);
      }
      else {
	err_dat = da_er->GetValAsFloat(row);
      }
      float err_plt = err_dat;
      if(yax.range.Range() > 0.0f) err_plt /= yax.range.Range();
      
      if(clr_ok)
	t3gl->errBar(plt, err_plt, err_bar_width, (T3Color)(clr));
      else
	t3gl->errBar(plt, err_plt, err_bar_width);
    }

    if((label_spacing > 0) && (row % label_spacing == 0)) {
      String str = String(dat.y); // todo: could spec format..
      t3gl->textAt(iVec3f(plt.x,  plt.y - (.5f * label_font_size), plt.z),
		   str.chars());
    }
    // post draw updates:
    first = false;
    last_x = dat.x;
    last_z = dat.z;
  }
  t3gl->finishBatch();
}

void GraphTableView::PlotData_Bar(GraphPlotView& plv, GraphPlotView& erv, GraphPlotView& yax, 
				  T3GraphLine* t3gl, float bar_off, int mat_cell) {
  t3gl->clear();

  DataCol* da_y = plv.GetDAPtr();
  if(!da_y) return;
  DataCol* da_x = x_axis.GetDAPtr();
  DataCol* da_z = z_axis.GetDAPtr();

  DataTable* dt = dataTable();

  // this should only happen if we have no data at all..
  if((view_range.min < 0) || (view_range.min >= dt->rows)) return;
  if((view_range.min < 0) || (view_range.min >= dt->rows)) return;

  plv.eff_y_axis = &yax;		// set this for point clicking!

  t3gl->startBatch();
  t3gl->setLineStyle((T3GraphLine::LineStyle)plv.line_style, line_width);
  t3gl->setMarkerSize((T3GraphLine::MarkerSize)point_size);

  DataCol* da_er = erv.GetDAPtr();

  GraphAxisBase* ax_clr = NULL;
  DataCol* da_clr = NULL;
  if(color_mode == FIXED_COLOR) {
    t3gl->setValueColorMode(false);
    t3gl->setDefaultColor((T3Color)(plv.color.color()));
  }
  else {
    t3gl->setValueColorMode(true);
    if(color_mode == VALUE_COLOR) {
      ax_clr = &yax;
      da_clr = plv.GetDAPtr();
    }
    else {
      ax_clr = &color_axis;
      da_clr = color_axis.GetDAPtr();
    }
    if(!da_clr) {		// fallback
      t3gl->setValueColorMode(false);
      t3gl->setDefaultColor((T3Color)(plv.color.color()));
    }
  }

  GraphAxisBase* ax_rst = NULL;
  DataCol* da_rst = NULL;
  if(graph_type == RASTER) {
    ax_rst = &raster_axis;
    da_rst = raster_axis.GetDAPtr();
  }

  bool matz = false;
  if((mat_cell >= 0) && (matrix_mode == Z_INDEX)) matz = true;

  float bar_wd_plt = bar_width * x_axis.axis_length / x_axis.range.Range();
  float bar_off_plt = bar_off * x_axis.axis_length / x_axis.range.Range();

  iVec3f dat;			// data point
  iVec3f plt;			// plot coords
  for (int row = view_range.min; row <= view_range.max; row++) {
    iColor clr; // only used for color modes
    bool clr_ok = false;
    if(x_axis.row_num)
      dat.x = row;
    else
      dat.x = da_x->GetValAsFloat(row);
    if(dat.x < x_axis.range.min || dat.x > x_axis.range.max) continue;
    if(z_axis.on) {
      if(z_axis.row_num)
	dat.z = row;
      else if(da_z)
	dat.z = da_z->GetValAsFloat(row);
      if(dat.z < z_axis.range.min || dat.z > z_axis.range.max) continue;
    }
    if((mat_cell >= 0) && (matrix_mode == Z_INDEX)) {
      dat.z = mat_cell;
    }
    if(mat_cell >= 0) {
      dat.y = da_y->GetValAsFloatM(row, mat_cell);
    }
    else {
      dat.y = da_y->GetValAsFloat(row);
    }
    if(dat.y < yax.range.min || dat.y > yax.range.max) continue;

    plt.x = x_axis.DataToPlot(dat.x);
    plt.y = yax.DataToPlot(dat.y);

    if(z_axis.on || matz)
      plt.z = z_axis.DataToPlot(dat.z);

    if(da_clr) {
      clr_ok = true;
      if(color_mode == VALUE_COLOR) {
	clr = GetValueColor(ax_clr, dat.y);
      }
      else {
	clr = GetValueColor(ax_clr, da_clr->GetValAsFloat(row));
      }
    }

    iVec3f pt;
    // draw the line
    if(clr_ok) {
      pt = plt;  pt.y = 0.0f;  pt.x += bar_off_plt;
      t3gl->moveTo(pt, (T3Color)(clr));
      pt.y = plt.y;
      t3gl->lineTo(pt, (T3Color)(clr));
      pt.x += bar_wd_plt;
      t3gl->lineTo(pt, (T3Color)(clr));
      pt.y = 0.0f;
      t3gl->lineTo(pt, (T3Color)(clr));
    }
    else {
      pt = plt;  pt.y = 0.0f;  pt.x += bar_off_plt;
      t3gl->moveTo(pt);
      pt.y = plt.y;
      t3gl->lineTo(pt);
      pt.x += bar_wd_plt;
      t3gl->lineTo(pt);
      pt.y = 0.0f;
      t3gl->lineTo(pt);
    }

    if(erv.on && da_er && (row % err_spacing == 0)) {
      float err_dat;
      if(da_er->is_matrix && mat_cell >= 0) {
	err_dat = da_er->GetValAsFloatM(row, mat_cell);
      }
      else {
	err_dat = da_er->GetValAsFloat(row);
      }
      float err_plt = err_dat;
      if(yax.range.Range() > 0.0f) err_plt /= yax.range.Range();

      pt = plt;  pt.x += bar_off_plt + bar_wd_plt * 0.5f;
      
      if(clr_ok)
	t3gl->errBar(pt, err_plt, err_bar_width, (T3Color)(clr));
      else
	t3gl->errBar(pt, err_plt, err_bar_width);
    }

    if((label_spacing > 0) && (row % label_spacing == 0)) {
      String str = String(dat.y); // todo: could spec format..
      t3gl->textAt(iVec3f(plt.x,  plt.y - (.5f * label_font_size), plt.z),
		   str.chars());
    }
  }
  t3gl->finishBatch();
}

void GraphTableView::PlotData_String(GraphPlotView& plv_str, GraphPlotView& plv_y, T3GraphLine* t3gl) {
  t3gl->clear();

  DataCol* da_y = plv_y.GetDAPtr();
  if(!da_y) return;
  DataCol* da_str = plv_str.GetDAPtr();
  if(!da_str) return;
  DataCol* da_x = x_axis.GetDAPtr();
  DataCol* da_z = z_axis.GetDAPtr();

  plv_str.eff_y_axis = &plv_y;		// set this for point clicking!

  // always plot in color assoc with plv_str, regardless of mode!
  t3gl->startBatch();
  t3gl->setValueColorMode(false);
  t3gl->setDefaultColor((T3Color)(plv_str.color.color()));

  iVec3f dat;			// data point
  iVec3f plt;			// plot coords
  for (int row = view_range.min; row <= view_range.max; row++) {
    if(x_axis.row_num)
      dat.x = row;
    else
      dat.x = da_x->GetValAsFloat(row);
    if(dat.x < x_axis.range.min || dat.x > x_axis.range.max) continue;
    if(z_axis.on) {
      if(z_axis.row_num)
	dat.z = row;
      else if(da_z)
	dat.z = da_z->GetValAsFloat(row);
      if(dat.z < z_axis.range.min || dat.z > z_axis.range.max) continue;
    }
    dat.y = da_y->GetValAsFloat(row);
    if(dat.y < plv_y.range.min || dat.y > plv_y.range.max) continue;

    plt.x = x_axis.DataToPlot(dat.x);
    plt.y = plv_y.DataToPlot(dat.y);
    if(z_axis.on)
      plt.z = z_axis.DataToPlot(dat.z);

    String str = da_str->GetValAsString(row);
    if(!str.empty()) {
      t3gl->textAt(iVec3f(plt.x,  plt.y - (.5f * label_font_size), plt.z),
		   str.chars());
    }
  }
  t3gl->finishBatch();
}

////////////////////////////////////////////////////////////////////////
// note on following: basically callbacks from view


void GraphTableView::SetColorSpec(ColorScaleSpec* color_spec) {
  colorscale.SetColorSpec(color_spec);
  UpdateDisplay(true);
}

void GraphTableView::setWidth(float wdth) {
  width = wdth;
  T3GraphViewNode* node_so = this->node_so();
  if (!node_so) return;
  node_so->setWidth(width);
  UpdateDisplay(true);
}

void GraphTableView::setScaleData(bool auto_scale_, float min_, float max_) {
  if ((colorscale.auto_scale == auto_scale_) && (colorscale.min == min_) && (colorscale.max == max_)) return;
  colorscale.auto_scale = auto_scale_;
  if(!colorscale.auto_scale)
    colorscale.SetMinMax(min_, max_);
  UpdateDisplay(true);
}



// callback for view transformer dragger
void T3GraphViewNode_DragFinishCB(void* userData, SoDragger* dragr) {
  SoTransformBoxDragger* dragger = (SoTransformBoxDragger*)dragr;
  T3GraphViewNode* vnd = (T3GraphViewNode*)userData;
  GraphTableView* nv = static_cast<GraphTableView*>(vnd->dataView());

  SbRotation cur_rot;
  cur_rot.setValue(SbVec3f(nv->main_xform.rotate.x, nv->main_xform.rotate.y, 
			   nv->main_xform.rotate.z), nv->main_xform.rotate.rot);

  SbVec3f trans = dragger->translation.getValue();
//   cerr << "trans: " << trans[0] << " " << trans[1] << " " << trans[2] << endl;
  cur_rot.multVec(trans, trans); // rotate translation by current rotation
  trans[0] *= nv->main_xform.scale.x;
  trans[1] *= nv->main_xform.scale.y;
  trans[2] *= nv->main_xform.scale.z;
  FloatTDCoord tr(trans[0], trans[1], trans[2]);
  nv->main_xform.translate += tr;

  const SbVec3f& scale = dragger->scaleFactor.getValue();
//   cerr << "scale: " << scale[0] << " " << scale[1] << " " << scale[2] << endl;
  FloatTDCoord sc(scale[0], scale[1], scale[2]);
  if(sc < .1f) sc = .1f;	// prevent scale from going to small too fast!!
  nv->main_xform.scale *= sc;

  SbVec3f axis;
  float angle;
  dragger->rotation.getValue(axis, angle);
//   cerr << "orient: " << axis[0] << " " << axis[1] << " " << axis[2] << " " << angle << endl;
  if(axis[0] != 0.0f || axis[1] != 0.0f || axis[2] != 1.0f || angle != 0.0f) {
    SbRotation rot;
    rot.setValue(SbVec3f(axis[0], axis[1], axis[2]), angle);
    SbRotation nw_rot = rot * cur_rot;
    nw_rot.getValue(axis, angle);
    nv->main_xform.rotate.SetXYZR(axis[0], axis[1], axis[2], angle);
  }

  vnd->txfm_shape()->scaleFactor.setValue(1.0f, 1.0f, 1.0f);
  vnd->txfm_shape()->rotation.setValue(SbVec3f(0.0f, 0.0f, 1.0f), 0.0f);

  float frmg = T3GraphViewNode::frame_margin;
  float frmg2 = 2.0f * frmg;
  float wdth = vnd->getWidth();

  vnd->txfm_shape()->translation.setValue(.5f * wdth, .5f * (1.0f + frmg2), 0.0f);

  dragger->translation.setValue(0.0f, 0.0f, 0.0f);
  dragger->rotation.setValue(SbVec3f(0.0f, 0.0f, 1.0f), 0.0f);
  dragger->scaleFactor.setValue(1.0f, 1.0f, 1.0f);

  nv->UpdateDisplay();
}

//////////////////////////
// iGraphTableView_Panel //
//////////////////////////

String iGraphTableView_Panel::panel_type() const {
  static String str("Graph Log");
  return str;
}

iGraphTableView_Panel::iGraphTableView_Panel(GraphTableView* tlv)
:inherited(tlv)
{
  int font_spec = taiMisc::fonMedium;

  ////////////////////////////////////////////////////////////////////
  layTopCtrls = new QHBoxLayout; layWidg->addLayout(layTopCtrls);

  chkDisplay = new QCheckBox("Disp", widg); chkDisplay->setObjectName("chkDisplay");
  chkDisplay->setToolTip("Whether to update the display when the underlying data changes");
  connect(chkDisplay, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
  layTopCtrls->addWidget(chkDisplay);
  layTopCtrls->addSpacing(taiM->hsep_c);

  chkManip = new QCheckBox("Manip", widg); chkDisplay->setObjectName("chkManip");
  chkManip->setToolTip("Whether to enable manipulation of the view object via a transformation box that supports position, scale and rotation manipulations");
  connect(chkManip, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
  layTopCtrls->addWidget(chkManip);
  layTopCtrls->addSpacing(taiM->hsep_c);

  // todo: fix tool tips on all of these..

  lblGraphType = taiM->NewLabel("Graph", widg, font_spec);
  lblGraphType->setToolTip("How to display the graph");
  layTopCtrls->addWidget(lblGraphType);
  cmbGraphType = dl.Add(new taiComboBox(true, TA_GraphTableView.sub_types.FindName("GraphType"),
				 this, NULL, widg, taiData::flgAutoApply));
  layTopCtrls->addWidget(cmbGraphType->GetRep());
  layTopCtrls->addSpacing(taiM->hsep_c);

  lblPlotStyle = taiM->NewLabel("Style", widg, font_spec);
  lblPlotStyle->setToolTip("How to plot the lines");
  layTopCtrls->addWidget(lblPlotStyle);
  cmbPlotStyle = dl.Add(new taiComboBox(true, TA_GraphTableView.sub_types.FindName("PlotStyle"),
				 this, NULL, widg, taiData::flgAutoApply));
  layTopCtrls->addWidget(cmbPlotStyle->GetRep());
  layTopCtrls->addSpacing(taiM->hsep_c);

  chkNegDraw =  new QCheckBox("Neg\nDraw", widg); chkNegDraw->setObjectName("chkNegDraw");
  chkNegDraw->setToolTip("Whether to draw a line when going in a negative direction (to the left), which may indicate a wrap-around to a new iteration of data");
  connect(chkNegDraw, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
  layTopCtrls->addWidget(chkNegDraw);
  layTopCtrls->addSpacing(taiM->hsep_c);

  chkNegDrawZ =  new QCheckBox("Neg\nDraw Z", widg); chkNegDrawZ->setObjectName("chkNegDrawZ");
  chkNegDrawZ->setToolTip("Whether to draw a line when going in a negative direction of Z (to the front), which may indicate a wrap-around to a new channel of data");
  connect(chkNegDrawZ, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
  layTopCtrls->addWidget(chkNegDrawZ);
  layTopCtrls->addSpacing(taiM->hsep_c);

  layTopCtrls->addStretch();
  butRefresh = new QPushButton("Refresh", widg);
  butRefresh->setFixedHeight(taiM->button_height(taiMisc::sizSmall));
  layTopCtrls->addWidget(butRefresh);
  connect(butRefresh, SIGNAL(pressed()), this, SLOT(butRefresh_pressed()) );

  ////////////////////////////////////////////////////////////////////
  layVals = new QHBoxLayout; layWidg->addLayout(layVals);

  lblRows = taiM->NewLabel("View\nRows", widg, font_spec);
  lblRows->setToolTip("Maximum number of rows to display (row height is scaled to fit).");
  layVals->addWidget(lblRows);
  fldRows = dl.Add(new taiIncrField(&TA_int, this, NULL, widg));
  layVals->addWidget(fldRows->GetRep());
  layVals->addSpacing(taiM->hsep_c);

  lblLineWidth = taiM->NewLabel("Line\nWidth", widg, font_spec);
  lblLineWidth->setToolTip("Width to draw lines with.");
  layVals->addWidget(lblLineWidth);
  fldLineWidth = dl.Add(new taiField(&TA_float, this, NULL, widg));
  layVals->addWidget(fldLineWidth->GetRep());
  layVals->addSpacing(taiM->hsep_c);

  lblPointSpacing = taiM->NewLabel("Pt\nSpc", widg, font_spec);
  lblPointSpacing->setToolTip("Spacing of points drawn relative to underlying data points.");
  layVals->addWidget(lblPointSpacing);
  fldPointSpacing = dl.Add(new taiField(&TA_float, this, NULL, widg));
  layVals->addWidget(fldPointSpacing->GetRep());
  layVals->addSpacing(taiM->hsep_c);

  lblLabelSpacing = taiM->NewLabel("Lbl\nSpc", widg, font_spec);
  lblLabelSpacing->setToolTip("Spacing of text labels of data point values. -1 means no text labels.");
  layVals->addWidget(lblLabelSpacing);
  fldLabelSpacing = dl.Add(new taiField(&TA_float, this, NULL, widg));
  layVals->addWidget(fldLabelSpacing->GetRep());
  layVals->addSpacing(taiM->hsep_c);

  lblWidth = taiM->NewLabel("Width", widg, font_spec);
  lblWidth->setToolTip("Width of graph display, in normalized units (default is 1.0 = same as height).");
  layVals->addWidget(lblWidth);
  fldWidth = dl.Add(new taiField(&TA_float, this, NULL, widg));
  layVals->addWidget(fldWidth->GetRep());
  layVals->addSpacing(taiM->hsep_c);

  lblDepth = taiM->NewLabel("Depth", widg, font_spec);
  lblDepth->setToolTip("Depth of graph display, in normalized units (default is 1.0 = same as height).");
  layVals->addWidget(lblDepth);
  fldDepth = dl.Add(new taiField(&TA_float, this, NULL, widg));
  layVals->addWidget(fldDepth->GetRep());
  layVals->addSpacing(taiM->hsep_c);

  layVals->addStretch();

  ////////////////////////////////////////////////////////////////////////////
  // 	Axes

  ////////////////////////////////////////////////////////////////////
  // X AXis
  layXAxis = new QHBoxLayout; layWidg->addLayout(layXAxis);

  int list_flags = taiData::flgNullOk | taiData::flgAutoApply | taiData::flgNoHelp;

  lblXAxis = taiM->NewLabel("X:", widg, font_spec);
  lblXAxis->setToolTip("Column of data to plot for the X Axis");
  layXAxis->addWidget(lblXAxis);
  lelXAxis = dl.Add(new taiListElsButton(&TA_T3DataView_List, this, NULL, widg, list_flags));
  layXAxis->addWidget(lelXAxis->GetRep());
  //  layVals->addSpacing(taiM->hsep_c);

  rncXAxis = new QCheckBox("Row\nNum", widg); rncXAxis->setObjectName("rncXAxis");
  rncXAxis->setToolTip("Use row number instead of column value for axis value");
  connect(rncXAxis, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
  layXAxis->addWidget(rncXAxis);
  layXAxis->addSpacing(taiM->hsep_c);

  pdtXAxis = dl.Add(taiPolyData::New(true, &TA_FixedMinMax, this, NULL, widg));
  layXAxis->addWidget(pdtXAxis->GetRep());

  layXAxis->addStretch();

  ////////////////////////////////////////////////////////////////////
  // Z AXis
  layZAxis = new QHBoxLayout; layWidg->addLayout(layZAxis);

  lblZAxis = taiM->NewLabel("Z:", widg, font_spec);
  lblZAxis->setToolTip("Column of data to plot for the Z Axis");
  layZAxis->addWidget(lblZAxis);
  lelZAxis = dl.Add(new taiListElsButton(&TA_T3DataView_List, this, NULL, widg, list_flags));
  layZAxis->addWidget(lelZAxis->GetRep());
  layZAxis->addSpacing(taiM->hsep_c);

  oncZAxis = new iCheckBox("On", widg);
  oncZAxis->setToolTip("Display a Z axis?");
  connect(oncZAxis, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
  layZAxis->addWidget(oncZAxis);
  layZAxis->addSpacing(taiM->hsep_c);

  rncZAxis = new QCheckBox("Row\nNum", widg); rncZAxis->setObjectName("rncZAxis");
  rncZAxis->setToolTip("Use row number instead of column value for axis value");
  connect(rncZAxis, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
  layZAxis->addWidget(rncZAxis);
  layZAxis->addSpacing(taiM->hsep_c);

  pdtZAxis = dl.Add(taiPolyData::New(true, &TA_FixedMinMax, this, NULL, widg));
  layZAxis->addWidget(pdtZAxis->GetRep());

  layZAxis->addStretch();

  ////////////////////////////////////////////////////////////////////
  // Y AXes

  for(int i=0;i<max_plots; i++) {
    layYAxis[i] = new QHBoxLayout; layWidg->addLayout(layYAxis[i]);

    String lbl = "Y" + String(i+1) + ":";
    lblYAxis[i] = taiM->NewLabel(lbl, widg, font_spec);
    lblYAxis[i]->setToolTip("Column of data to plot (optional)");
    layYAxis[i]->addWidget(lblYAxis[i]);
    lelYAxis[i] = dl.Add(new taiListElsButton(&TA_T3DataView_List, this, NULL, widg, list_flags));
    layYAxis[i]->addWidget(lelYAxis[i]->GetRep());
    layYAxis[i]->addSpacing(taiM->hsep_c);

    oncYAxis[i] = new iCheckBox("On", widg);
    oncYAxis[i]->setToolTip("Display this column's data or not?");
    connect(oncYAxis[i], SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
    layYAxis[i]->addWidget(oncYAxis[i]);
    layYAxis[i]->addSpacing(taiM->hsep_c);

    chkYAltY[i] =  new QCheckBox("Alt\nY", widg); chkYAltY[i]->setObjectName("chkYAltY");
    chkYAltY[i]->setToolTip("Whether to display values on an alternate Y axis for this column of data (otherwise it uses the main Y axis)");
    connect(chkYAltY[i], SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
    layYAxis[i]->addWidget(chkYAltY[i]);
    layYAxis[i]->addSpacing(taiM->hsep_c);

    pdtYAxis[i] = dl.Add(taiPolyData::New(true, &TA_FixedMinMax, this, NULL, widg));
    layYAxis[i]->addWidget(pdtYAxis[i]->GetRep());

    layYAxis[i]->addStretch();
  }

  ////////////////////////////////////////////////////////////////////
  // Error bars
  layErr[0] = new QHBoxLayout; layWidg->addLayout(layErr[0]);
  layErr[1] = new QHBoxLayout; layWidg->addLayout(layErr[1]);
  
  for(int i=0;i<max_plots; i++) {
    int rw = (i < 4) ? 0 : 1;
    String lbl = String(i+1) + " Err:";
    lblErr[i] = taiM->NewLabel(lbl, widg, font_spec);
    lblErr[i]->setToolTip("Column of for this column's error bar data");
    layErr[rw]->addWidget(lblErr[i]);
    layErr[rw]->addSpacing(taiM->hsep_c);

    lelErr[i] = dl.Add(new taiListElsButton(&TA_T3DataView_List, this, NULL, widg, list_flags));
    layErr[rw]->addWidget(lelErr[i]->GetRep());
    layErr[rw]->addSpacing(taiM->hsep_c);

    oncErr[i] = new iCheckBox("On", widg);
    oncErr[i]->setToolTip("Display error bars for this column's data?");
    connect(oncErr[i], SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
    layErr[rw]->addWidget(oncErr[i]);
    layErr[rw]->addSpacing(taiM->hsep_c);
  }

  layErr[0]->addStretch();
  layErr[1]->addStretch();

  ////////////////////////////////////////////////////////////////////////////
  // 	Colors

  layCAxis = new QHBoxLayout; layWidg->addLayout(layCAxis);
  lblColorMode = taiM->NewLabel("Color\nMode", widg, font_spec);
  lblColorMode->setToolTip("How to determine line color:\n VALUE_COLOR makes the color change as a function of the\n Y axis value, according to the colorscale pallete\n FIXED_COLOR uses fixed colors associated with each Y axis line\n (click on line/legend/axis and do View Properties in context menu to change)\n COLOR_AXIS uses a separate column of data to determine color value");
  layCAxis->addWidget(lblColorMode);
  cmbColorMode = dl.Add(new taiComboBox(true, TA_GraphTableView.sub_types.FindName("ColorMode"),
	this, NULL, widg, taiData::flgAutoApply));
  layCAxis->addWidget(cmbColorMode->GetRep());
  //  layColorScale->addSpacing(taiM->hsep_c);

  lblCAxis = taiM->NewLabel("Color\nAxis:", widg, font_spec);
  lblCAxis->setToolTip("Column of data for COLOR_AXIS color mode");
  layCAxis->addWidget(lblCAxis);
  lelCAxis = dl.Add(new taiListElsButton(&TA_T3DataView_List, this, NULL, widg, list_flags));
  layCAxis->addWidget(lelCAxis->GetRep());
  //  layVals->addSpacing(taiM->hsep_c);

  lblThresh = taiM->NewLabel("Thresh", widg, font_spec);
  lblThresh->setToolTip("Threshold for THRESH_LINE and THRESH_POINT styles -- only draw a line when value is over this threshold.");
  layCAxis->addWidget(lblThresh);
  fldThresh = dl.Add(new taiField(&TA_float, this, NULL, widg));
  layCAxis->addWidget(fldThresh->GetRep());
  layVals->addSpacing(taiM->hsep_c);

  // Err Spacing
  lblErrSpacing = taiM->NewLabel("Err\nSpc", widg, font_spec);
  lblErrSpacing->setToolTip("Spacing of error bars relative to data points.");
  layCAxis->addWidget(lblErrSpacing);
  fldErrSpacing = dl.Add(new taiField(&TA_float, this, NULL, widg));
  layCAxis->addWidget(fldErrSpacing->GetRep());

  layCAxis->addStretch();

  ////////////////////////////////////////////////////////////////////
  // second row: color bar + button
  layColorScale = new QHBoxLayout; layWidg->addLayout(layColorScale);
  cbar = new HCScaleBar(&tlv->colorscale, ScaleBar::RANGE, true, true, widg);
//  cbar->setMaximumWidth(30);
  connect(cbar, SIGNAL(scaleValueChanged()), this, SLOT(Changed()) );
  layColorScale->addWidget(cbar); // stretchfact=1 so it stretches to fill the space

  butSetColor = new QPushButton("Colors", widg);
  butSetColor->setToolTip("Select color pallette for color value plotting (also determines background color).");
  butSetColor->setFixedHeight(taiM->button_height(taiMisc::sizSmall));
  layColorScale->addWidget(butSetColor);
  connect(butSetColor, SIGNAL(pressed()), this, SLOT(butSetColor_pressed()) );

  ////////////////////////////////////////////////////////////////////
  // Raster Axis
  layRAxis = new QHBoxLayout; layWidg->addLayout(layRAxis);

  lblRAxis = taiM->NewLabel("Raster:", widg, font_spec);
  lblRAxis->setToolTip("Column of data for the Y axis in RASTER graphs");
  layRAxis->addWidget(lblRAxis);
  lelRAxis = dl.Add(new taiListElsButton(&TA_T3DataView_List, this, NULL, widg, list_flags));
  layRAxis->addWidget(lelRAxis->GetRep());
  //  layVals->addSpacing(taiM->hsep_c);

  pdtRAxis = dl.Add(taiPolyData::New(true, &TA_FixedMinMax, this, NULL, widg));
  layRAxis->addWidget(pdtRAxis->GetRep());

  layRAxis->addStretch();
  
  layWidg->addStretch();

  MakeButtons(layOuter);
}

iGraphTableView_Panel::~iGraphTableView_Panel() {
}

void iGraphTableView_Panel::InitPanel_impl() {
  // nothing structural here (could split out cols, but not worth it)
}

void iGraphTableView_Panel::UpdatePanel_impl() {
  inherited::UpdatePanel_impl();

  //   cerr << "panel update" << endl;

  GraphTableView* glv = this->glv(); //cache
  if (!glv) return; // probably destructing

  chkDisplay->setChecked(glv->display_on);
  chkManip->setChecked(glv->manip_ctrl_on);
  cmbGraphType->GetImage(glv->graph_type);
  cmbPlotStyle->GetImage(glv->plot_style);
  fldRows->GetImage((String)glv->view_rows);

  fldLineWidth->GetImage((String)glv->line_width);
  fldPointSpacing->GetImage((String)glv->point_spacing);
  fldLabelSpacing->GetImage((String)glv->label_spacing);
  chkNegDraw->setChecked(glv->negative_draw);
  chkNegDrawZ->setChecked(glv->negative_draw_z);
  fldWidth->GetImage((String)glv->width);
  fldDepth->GetImage((String)glv->depth);

  lelXAxis->GetImage(&(glv->children), glv->x_axis.GetColPtr());
  rncXAxis->setChecked(glv->x_axis.row_num);
  pdtXAxis->GetImage_(&(glv->x_axis.fixed_range));

  lelZAxis->GetImage(&(glv->children), glv->z_axis.GetColPtr());
  oncZAxis->setReadOnly(glv->z_axis.GetColPtr() == NULL);
  oncZAxis->setChecked(glv->z_axis.on);
  rncZAxis->setChecked(glv->z_axis.row_num);
  pdtZAxis->GetImage_(&(glv->z_axis.fixed_range));

  lelZAxis->SetFlag(taiData::flgReadOnly, !glv->z_axis.on);
  rncZAxis->setAttribute(Qt::WA_Disabled, !glv->z_axis.on);
  pdtZAxis->SetFlag(taiData::flgReadOnly, !glv->z_axis.on);

  for(int i=0;i<max_plots; i++) {
    lelYAxis[i]->GetImage(&(glv->children), glv->all_plots[i]->GetColPtr());
    oncYAxis[i]->setReadOnly(glv->all_plots[i]->GetColPtr() == NULL);
    oncYAxis[i]->setChecked(glv->all_plots[i]->on);
    pdtYAxis[i]->GetImage_(&(glv->all_plots[i]->fixed_range));
    lelYAxis[i]->SetFlag(taiData::flgReadOnly, !glv->all_plots[i]->on);
    pdtYAxis[i]->SetFlag(taiData::flgReadOnly, !glv->all_plots[i]->on);
    chkYAltY[i]->setChecked(glv->all_plots[i]->alt_y);
  }

  for(int i=0;i<max_plots; i++) {
    lelErr[i]->GetImage(&(glv->children), glv->all_errs[i]->GetColPtr());
    oncErr[i]->setReadOnly(glv->all_errs[i]->GetColPtr() == NULL);
    oncErr[i]->setChecked(glv->all_errs[i]->on);
  }

  fldErrSpacing->GetImage((String)glv->err_spacing);

  cmbColorMode->GetImage(glv->color_mode);
  lelCAxis->GetImage(&(glv->children), glv->color_axis.GetColPtr());
  lelCAxis->SetFlag(taiData::flgReadOnly, glv->color_mode != GraphTableView::COLOR_AXIS);

  fldThresh->GetImage((String)glv->thresh);

  cbar->UpdateScaleValues();

  lelRAxis->GetImage(&(glv->children), glv->raster_axis.GetColPtr());
  pdtRAxis->GetImage_(&(glv->raster_axis.fixed_range));

  lelRAxis->SetFlag(taiData::flgReadOnly, glv->graph_type != GraphTableView::RASTER);
  pdtRAxis->SetFlag(taiData::flgReadOnly, glv->graph_type != GraphTableView::RASTER);
}

void iGraphTableView_Panel::GetValue_impl() {
  GraphTableView* glv = this->glv(); //cache
  if (!glv) return;
  
  glv->display_on = chkDisplay->isChecked();
  glv->manip_ctrl_on = chkManip->isChecked();
  int i = 0;
  cmbGraphType->GetEnumValue(i); glv->graph_type = (GraphTableView::GraphType)i;
  cmbPlotStyle->GetEnumValue(i); glv->plot_style = (GraphTableView::PlotStyle)i;
  glv->view_rows = (int)fldRows->GetValue();
  glv->line_width = (float)fldLineWidth->GetValue();
  glv->point_spacing = (int)fldPointSpacing->GetValue();
  glv->point_spacing = MAX(1, glv->point_spacing);
  glv->label_spacing = (int)fldLabelSpacing->GetValue();
  glv->negative_draw = chkNegDraw->isChecked();
  glv->negative_draw_z = chkNegDrawZ->isChecked();
  glv->width = (float)fldWidth->GetValue();
  glv->depth = (float)fldDepth->GetValue();
  
  glv->setScaleData(false, cbar->min(), cbar->max());
  
  glv->x_axis.row_num = rncXAxis->isChecked();
  pdtXAxis->GetValue_(&(glv->x_axis.fixed_range));
  glv->x_axis.SetColPtr((GraphColView*)lelXAxis->GetValue());

  // if setting a col for 1st time, we automatically turn on (since it would be ro)
  GraphColView* tcol = (GraphColView*)lelZAxis->GetValue();
  if (tcol && !glv->z_axis.GetColPtr())
    oncZAxis->setChecked(true);
  glv->z_axis.on = oncZAxis->isChecked();
  pdtZAxis->GetValue_(&(glv->z_axis.fixed_range));
  glv->z_axis.row_num = rncZAxis->isChecked();
  glv->z_axis.SetColPtr(tcol);

  for(int i=0;i<max_plots; i++) {
    tcol = (GraphColView*)lelYAxis[i]->GetValue();
    if (tcol && !glv->all_plots[i]->GetColPtr())
      oncYAxis[i]->setChecked(true);
    glv->all_plots[i]->on = oncYAxis[i]->isChecked();
    pdtYAxis[i]->GetValue_(&(glv->all_plots[i]->fixed_range)); // this can change, so update
    glv->all_plots[i]->alt_y = chkYAltY[i]->isChecked();
    glv->all_plots[i]->SetColPtr(tcol);
  }  
  
  for(int i=0;i<max_plots; i++) {
    tcol = (GraphColView*)lelErr[i]->GetValue();
    if (tcol && !glv->all_errs[i]->GetColPtr())
      oncErr[i]->setChecked(true);
    glv->all_errs[i]->on = oncErr[i]->isChecked();
    glv->all_errs[i]->SetColPtr(tcol);
  }
  
  glv->err_spacing = (int)fldErrSpacing->GetValue();
  
  cmbColorMode->GetEnumValue(i); glv->color_mode = (GraphTableView::ColorMode)i;
  glv->color_axis.SetColPtr((GraphColView*)lelCAxis->GetValue());
  
  glv->thresh = (float)fldThresh->GetValue();
  
  glv->setScaleData(false, cbar->min(), cbar->max());
  
  glv->raster_axis.SetColPtr((GraphColView*)lelRAxis->GetValue());
  pdtRAxis->GetValue_(&(glv->raster_axis.fixed_range));
  
  glv->UpdateAfterEdit(); // so many guys require this, we just always do it
  glv->UpdateDisplay(false); // don't update us, because logic will do that anyway
}

void iGraphTableView_Panel::CopyFrom_impl() {
  GraphTableView* glv = this->glv(); //cache
  if (!glv) return;
  glv->CallFun("CopyFromView");
}


void iGraphTableView_Panel::butRefresh_pressed() {
  GraphTableView* glv = this->glv(); //cache
  if (updating || !glv) return;

  glv->InitDisplay();
  glv->UpdateDisplay();
}

void iGraphTableView_Panel::butClear_pressed() {
  GraphTableView* glv = this->glv(); //cache
  if (updating || !glv) return;

  glv->ClearData();
}

void iGraphTableView_Panel::butSetColor_pressed() {
  GraphTableView* glv = this->glv(); //cache
  if (updating || !glv) return;

  glv->CallFun("SetColorSpec");
}


/////////////////////////////////////////////////////////////////////////////////
//		Other GUI stuff

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
  // we create ours first, because it should be the default
  iDataTablePanel* dp = new iDataTablePanel(dl_);
  DataPanelCreated(dp);
  inherited::CreateDataPanel_impl(dl_);
}

//////////////////////////
//   iDataTableView	//
//////////////////////////

iDataTableView::iDataTableView(QWidget* parent)
:inherited(parent)
{
  setSelectionMode(QAbstractItemView::ContiguousSelection);
  gui_edit_op = false;

  // this is important for faster viewing:
  verticalHeader()->setResizeMode(QHeaderView::Interactive);
}
  
void iDataTableView::currentChanged(const QModelIndex& current, const QModelIndex& previous) {
  inherited::currentChanged(current, previous);
  emit sig_currentChanged(current);
}

void iDataTableView::dataChanged(const QModelIndex& topLeft,
    const QModelIndex & bottomRight)
{
  inherited::dataChanged(topLeft, bottomRight);
  emit sig_dataChanged(topLeft, bottomRight);
}

DataTable* iDataTableView::dataTable() const {
  DataTableModel* mod = qobject_cast<DataTableModel*>(model());
  if (mod) return mod->dataTable();
  else return NULL; 
}

void iDataTableView::EditAction(int ea) {
  DataTable* tab = this->dataTable(); // may not exist
  if (!tab || !selectionModel()) return;
  gui_edit_op = true;
  taiTabularDataMimeFactory* fact = taiTabularDataMimeFactory::instance();
  CellRange sel(selectionModel()->selectedIndexes());
  if (ea & taiClipData::EA_SRC_OPS) {
    fact->Table_EditActionS(tab, sel, ea);
  } else {// dest op
    taiMimeSource* ms = taiMimeSource::NewFromClipboard();
    fact->Table_EditActionD(tab, sel, ms, ea);
    delete ms;
  }
  gui_edit_op = false;
}

void iDataTableView::GetEditActionsEnabled(int& ea) {
  int allowed = 0;
  int forbidden = 0;
  DataTable* tab = this->dataTable(); // may not exist
  if (tab && selectionModel()) {
    taiTabularDataMimeFactory* fact = taiTabularDataMimeFactory::instance();
    CellRange sel(selectionModel()->selectedIndexes());
    taiMimeSource* ms = taiMimeSource::NewFromClipboard();
    fact->Table_QueryEditActions(tab, sel, ms, allowed, forbidden);
    delete ms;
  }
  ea = allowed & ~forbidden;
}

void iDataTableView::RowColOp_impl(int op_code, const CellRange& sel) {
  DataTable* tab = this->dataTable(); // may not exist
  if (!tab) return;
  taProject* proj = (taProject*)tab->GetOwner(&TA_taProject);

  gui_edit_op = true;
  if (op_code & OP_ROW) {
    // must have >=1 row selected to make sense
    if ((op_code & (OP_APPEND | OP_INSERT | OP_DUPLICATE | OP_DELETE))) {
      if (sel.height() < 1) goto bail;
      if (op_code & OP_APPEND) {
	if(proj) proj->undo_mgr.SaveUndo(tab, "AddRows", tab);
        tab->AddRows(sel.height());
      } else if (op_code & OP_INSERT) {
	if(proj) proj->undo_mgr.SaveUndo(tab, "Insertows", tab);
        tab->InsertRows(sel.row_fr, sel.height());
      } else if (op_code & OP_DUPLICATE) {
	if(proj) proj->undo_mgr.SaveUndo(tab, "DuplicateRows", tab);
        tab->DuplicateRows(sel.row_fr, sel.height());
      } else if (op_code & OP_DELETE) {
	if(taMisc::delete_prompts || !tab->HasDataFlag(DataTable::SAVE_ROWS)) {
	  if (taMisc::Choice("Are you sure you want to delete the selected rows?", "Yes", "Cancel") != 0) goto bail;
	}
	if(proj) proj->undo_mgr.SaveUndo(tab, "RemoveRows", tab);
        tab->RemoveRows(sel.row_fr, sel.height());
      }
    }
  } else if (op_code & OP_COL) { 
    // must have >=1 col selected to make sense
    if ((op_code & (OP_APPEND | OP_INSERT | OP_DELETE))) {
      if (sel.width() < 1) goto bail;
/*note: not supporting col ops here
      if (op_code & OP_APPEND) {
      } else 
      if (op_code & OP_INSERT) {
      } else */
      if (op_code & OP_DELETE) {
	if(taMisc::delete_prompts || !tab->HasDataFlag(DataTable::SAVE_ROWS)) {
	  if (taMisc::Choice("Are you sure you want to delete the selected columns?", "Yes", "Cancel") != 0) goto bail;
	}
        tab->StructUpdate(true);
	if(proj) proj->undo_mgr.SaveUndo(tab, "RemoveCols", tab);
        for (int col = sel.col_to; col >= sel.col_fr; --col) {
          tab->RemoveCol(col);
        }
        tab->StructUpdate(false);
      }
    }
  }
 bail:
  gui_edit_op = false;
} 

void iDataTableView::FillContextMenu_impl(ContextArea ca,
  taiMenu* menu, const CellRange& sel)
{
  inherited::FillContextMenu_impl(ca, menu, sel);
  DataTable* tab = this->dataTable(); // may not exist
  if (!tab) return;
  // only do col items if one selected only
  if ((ca == CA_COL_HDR) && (sel.width() == 1)) {
    DataCol* col = tab->GetColData(sel.col_fr, true);
    if (col) {
      taiDataLink* link = (taiDataLink*)col->GetDataLink();
      if (link) link->FillContextMenu(menu);
    }
  }
}

//////////////////////////
//    DataTableDelegate	//
//////////////////////////

DataTableDelegate::DataTableDelegate(DataTable* dt_) 
:inherited(NULL)
{
  dt = dt_;
}

DataTableDelegate::~DataTableDelegate() {
}


//////////////////////////
//    iDataTableEditor 	//
//////////////////////////

iDataTableEditor::iDataTableEditor(QWidget* parent) 
:inherited(parent)
{
  m_cell_par = NULL;
  layOuter = new QVBoxLayout(this);
  layOuter->setMargin(0); layOuter->setSpacing(0);
  splMain = new iSplitter(this);
  splMain->setOrientation(Qt::Vertical);
  layOuter->addWidget(splMain);
  tvTable = new iDataTableView();
  tvCell = new iMatrixEditor();
  splMain->addWidget(tvTable);
  splMain->addWidget(tvCell);

  tvTable->installEventFilter(this);
  tvCell->tv->installEventFilter(this);

  connect(tvTable, SIGNAL(sig_currentChanged(const QModelIndex&)),
    this, SLOT(tvTable_currentChanged(const QModelIndex&)));
  connect(tvTable, SIGNAL(sig_dataChanged(const QModelIndex&, const QModelIndex&)),
    this, SLOT(tvTable_dataChanged(const QModelIndex&, const QModelIndex&)) );

}

iDataTableEditor::~iDataTableEditor() {
  if (m_cell_par) {
    setCellMat(NULL, QModelIndex());
  }
}

void iDataTableEditor::ConfigView() {
  DataTable* dt = this->dt(); // cache
  if (!dt) return;
  bool show_cell = (bool)m_cell; // if we have a cell, show it..
  // false if not in mode, or no mat cols
//   if (true) { // in cell mode
//     for (int i = 0; i < dt->data.size; ++i) {
//       DataCol* dc = dt->data.FastEl(i);
//       if (dc->is_matrix) {
//         show_cell = true;
//         break;
//       }
//     }
//   }
  tvCell->setVisible(show_cell);
  // make sure orphan cell viewer goes away...
  if (dt->rows == 0) {
    setCellMat(NULL, QModelIndex());
  }
}


void iDataTableEditor::DataLinkDestroying(taDataLink* dl) {
  // note: following should always be true
  if (m_cell_par && (m_cell_par == dl->taData())) {
    // ok, probably the col is deleting, so unlink ourself now!
    setCellMat(NULL, QModelIndex());
    // WARNING: m_cell_par will now be NULL
  }
}

void iDataTableEditor::DataDataChanged(taDataLink* dl, int dcr, void* op1, void* op2) {
}

DataTableModel* iDataTableEditor::dtm() const {
  if (m_dt)
    return m_dt->GetTableModel();
  return NULL;
}

void iDataTableEditor::Refresh() {
  DataTableModel* dtm = this->dtm();
  if (dtm)
    dtm->refreshViews();
  if (m_cell)
    tvCell->Refresh();
}

void iDataTableEditor::setDataTable(DataTable* dt_) {
  if (dt_ == m_dt.ptr()) return;

  m_dt = dt_;
  
  DataTableModel* mod = dtm();

  if (mod) {
//nn    tv->setItemDelegate(new DataTableDelegate(dt_));
    tvTable->setModel(mod);
    connect(mod, SIGNAL(layoutChanged()),
	    this, SLOT(tvTable_layoutChanged()) );
  }
  
  ConfigView();
}

void iDataTableEditor::setCellMat(taMatrix* mat, const QModelIndex& index,
    bool pat_4d) 
{
  // unlink old parent
  if (m_cell_par) {
    m_cell_par->RemoveDataClient(this);
    m_cell_par = NULL;
  }
  // link new parent, if any
  if (mat) {
    m_cell_par = mat->slicePar();
    if (m_cell_par) { // should exist!!!
      m_cell_par->AddDataClient(this);
    }
  }

  m_cell = mat;
   
  m_cell_index = index;
  // actually set mat last, because gui immediately calls back
  tvCell->setMatrix(mat, pat_4d);
  if (mat) {
    MatrixTableModel* mat_model = mat->GetTableModel();
    mat_model->col_idx = index.column(); // ok if done repeatedly, is always the same
    // connect the magic signal that updates the table -- note that there is just
    // one of these, and it hangs around even when the cell isn't viewed
    connect(mat_model, SIGNAL(matDataChanged(int)),
	    dtm(), SLOT(matDataChanged(int)) );
  }
}

void iDataTableEditor::tvTable_layoutChanged() {
  if(!isVisible()) return;
  if(tvTable && !tvTable->gui_edit_op)
    tvTable->scrollToBottom();
  ConfigView();
//no-causes recursive invocation!  Refresh();
  if (m_cell) {
    MatrixTableModel* mat_model = m_cell->GetTableModel();
    mat_model->emit_layoutChanged(); // hacky but works
  }
}

void iDataTableEditor::tvTable_currentChanged(const QModelIndex& index) {
  DataTable* dt_ = dt(); // cache
  int colidx = index.column();
  DataCol* col = dt_->GetColData(colidx, true); // quiet
  if (col && col->is_matrix) {
    taMatrix* tcell = dt_->GetValAsMatrix(index.column(), index.row());
    if (tcell) {
      bool pat_4d = (col->HasColFlag(DataCol::PAT_4D) && tcell->dims() >= 4);
      setCellMat(tcell, index, pat_4d);

      MatrixTableModel* mat_model = tcell->GetTableModel();
      mat_model->setDimNames(&col->dim_names);
      tvCell->setVisible(true);
      return;
    }
  }
  tvCell->setVisible(false);
  setCellMat(NULL, QModelIndex());
}

void iDataTableEditor::tvTable_dataChanged(const QModelIndex& topLeft,
    const QModelIndex& bottomRight)
{
  if ((bool)m_cell && (m_cell_index.column() >= topLeft.column()) &&
    (m_cell_index.column() <= bottomRight.column()) &&
    (m_cell_index.row() >= topLeft.row()) &&
     (m_cell_index.row() <= bottomRight.row()) )
  {
    m_cell->DataChanged(DCR_ITEM_UPDATED, NULL, NULL); // easiest way
  }
}

void iDataTableEditor::UpdateSelectedItems_impl() {
  //note: not needed
}


bool iDataTableEditor::eventFilter(QObject* obj, QEvent* event) {
  if (event->type() != QEvent::KeyPress) {
    return inherited::eventFilter(obj, event);
  }

  if((bool)tvTable->m_window) {
    tvTable->m_window->FocusIsMiddlePanel();
  }

  QCoreApplication* app = QCoreApplication::instance();
  QKeyEvent* e = static_cast<QKeyEvent *>(event);
  bool ctrl_pressed = taiMisc::KeyEventCtrlPressed(e);
  if(ctrl_pressed) {
    if(e->key() == Qt::Key_T) {
      if(obj->inherits("iDataTableView")) {
	tvTable->clearExtSelection();
	if(m_cell) {
	  tvCell->tv->setFocus();
	  tvCell->tv->selectCurCell();
	}
      }
      else {
	tvCell->tv->clearExtSelection();
	tvTable->setFocus();
	tvTable->selectCurCell();
      }
      return true;
    }
  }
  return tvTable->eventFilter(obj, event); // this has all the other good emacs xlations
}

//////////////////////////
//    iDataTablePanel 	//
//////////////////////////

iDataTablePanel::iDataTablePanel(taiDataLink* dl_)
:inherited(dl_)
{
  dte = NULL;
  /* TODO: add view button(s) when we add Flat mode, and/or image or block editing
  QToolButton* but = new QToolButton;
  but->setMaximumHeight(taiM->button_height(taiMisc::sizSmall));
  but->setFont(taiM->buttonFont(taiMisc::sizSmall));
  but->setText("View...");
  AddMinibarWidget(but);
  connect(but, SIGNAL(clicked()), this, SLOT(mb_View()) );*/
}

iDataTablePanel::~iDataTablePanel() {
}

QWidget* iDataTablePanel::firstTabFocusWidget() {
  if(!dte) return NULL;
  return dte->tvTable;
}

void iDataTablePanel::DataChanged_impl(int dcr, void* op1_, void* op2_) {
  if(!isVisible() || !dte || !dte->isVisible()) return; // no update when hidden!
  inherited::DataChanged_impl(dcr, op1_, op2_);
  //NOTE: don't need to do anything because DataModel will handle it
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
    rval = ci->QueryEditActions_(sel_list);
    // certain things disallowed if more than one item selected
    if (sel_list.size > 1) {
      rval &= ~(taiClipData::EA_FORB_ON_MUL_SEL);
    }
  }
  return rval;
}

void iDataTablePanel::GetSelectedItems(ISelectable_PtrList& lst) {
//note: not applicable
}

void iDataTablePanel::GetWinState_impl() {
  inherited::GetWinState_impl();
  DataTable* dt = this->dt(); // cache
  if (!dt) return;
  QTableView* tv = dte->tvTable; // cache -- note: row# header size is separate
  // we store col widths as fraction of ctrl width
  float fwd = (float)tv->width();
  if (fwd <= 0.0f) return; // huh???
  for (int i = 0; i < dt->data.size; ++i) {
    DataCol* dc = dt->data.FastEl(i);
    int iwd = tv->columnWidth(i);
    if (iwd > 0) {
      float fcolwd = iwd / fwd;
      dc->SetUserData("view_panel_wd", fcolwd);
    }
  }
}

void iDataTablePanel::SetWinState_impl() {
  inherited::SetWinState_impl();
  DataTable* dt = this->dt(); // cache
  if (!dt) return;
  QTableView* tv = dte->tvTable; // cache -- note: row# header size is separate
  // we store col widths as fraction of ctrl width
  float fwd = (float)tv->width();
  if (fwd <= 0.0f) return; // huh???
  for (int i = 0; i < dt->data.size; ++i) {
    DataCol* dc = dt->data.FastEl(i);
    float fcolwd = dc->GetUserDataAsFloat("view_panel_wd");
    int iwd = (int)(fwd * fcolwd);
    if (iwd > 0) { // ==0 typically if not set in UD
      tv->setColumnWidth(i, iwd);
    }
  }
}

String iDataTablePanel::panel_type() const {
  static String str("Data Table");
  return str;
}

void iDataTablePanel::UpdatePanel_impl() {
  if(dte) {
    // bracket with gui_edit_op to prevent scroll to bottom on show events
    if(isShowUpdating())   dte->tvTable->gui_edit_op = true;
    dte->Refresh();
    if(isShowUpdating())   dte->tvTable->gui_edit_op = false;
  }
  inherited::UpdatePanel_impl();
}

void iDataTablePanel::Render_impl() {
  dte = new iDataTableEditor();
  setCentralWidget(dte);
  
  dte->setDataTable(dt());
  connect(dte->tvTable, SIGNAL(hasFocus(iTableView*)),
	  this, SLOT(tv_hasFocus(iTableView*)) );
  connect(dte->tvCell->tv, SIGNAL(hasFocus(iTableView*)),
	  this, SLOT(tv_hasFocus(iTableView*)) );

  iMainWindowViewer* vw = viewerWindow();
  if (vw) {
    dte->installEventFilter(vw);
    dte->tvTable->m_window = vw;
    dte->tvCell->tv->m_window = vw;
  }
}

void iDataTablePanel::tv_hasFocus(iTableView* sender) {
  iMainWindowViewer* vw = viewerWindow();
  if (vw) {
    vw->SetClipboardHandler(sender,
			    SLOT(GetEditActionsEnabled(int&)),
			    SLOT(EditAction(int)),
			    NULL,
			    SIGNAL(UpdateUi()) );
  }
}


//////////////////////////////////
//  taiTabularDataMimeFactory	//
//////////////////////////////////

/* Tabular Data semantics

  In general, we do not try to interpret the tabular data, since there
  are so many combinations, including from external sources -- we will
  make a "best effort" to paste data, by using the built-in string 
  converters.
  
  We can always (try) pasting data into a Matrix, since we can always
  interpret the data in a flattened way.
  
  
  
  Paste semantics
 
  Matrix: 
    * if 1 cell is selected, it is assumed that we want to paste
      "as much as possible"; 
    * if there is a selection, then we limit the pasting to that
      region
*/

const String taiTabularDataMimeFactory::tacss_matrixdesc("tacss/matrixdesc");
const String taiTabularDataMimeFactory::tacss_tabledesc("tacss/tabledesc");

void taiTabularDataMimeFactory::Initialize() { 
}

void taiTabularDataMimeFactory::Mat_Clear(taMatrix* mat,
    const CellRange& sel) const
{
  mat->DataUpdate(true);
  for (int row = sel.row_fr; row <= sel.row_to; ++row) {
    for (int col = sel.col_fr; col <= sel.col_to; ++col) {
      int idx = mat->FastElIndex2D(col, row);
      mat->SetFmVar_Flat(_nilVariant, idx);
    }
  }
  mat->DataUpdate(false);
}

void taiTabularDataMimeFactory::Mat_QueryEditActions(taMatrix* mat, 
  const CellRange& sel, taiMimeSource* ms,
  int& allowed, int& forbidden) const
{
  // ops that are never allowed on mats
  forbidden |= (taiClipData::EA_CUT | taiClipData::EA_DELETE);
  // forbidden on ro
  if (mat->isGuiReadOnly()) 
    forbidden |= taiClipData::EA_FORB_ON_DST_READONLY;
  // src ops
  if (sel.nonempty())
    allowed |= (taiClipData::EA_COPY | taiClipData::EA_CLEAR);
    
  if (!ms) return;
  // dst ops -- none allowed if no selection
  if (sel.empty()) {
    forbidden = taiClipData::EA_DST_OPS;
    return;
  }
  
  // Priority is: Mat, Table, Generic
  {taiMatrixDataMimeItem* mi = (taiMatrixDataMimeItem*)
     ms->GetMimeItem(&TA_taiMatrixDataMimeItem);
  if (mi) {
    allowed |= taiClipData::EA_PASTE;
    return;
  }}
  // Table
  {taiTableDataMimeItem* mi = (taiTableDataMimeItem*)
     ms->GetMimeItem(&TA_taiTableDataMimeItem);
  if (mi) {
    allowed |= taiClipData::EA_PASTE;
    return;
  }}
  // TSV
  {taiTsvMimeItem* mi = (taiTsvMimeItem*)
    ms->GetMimeItem(&TA_taiTsvMimeItem);
  if (mi) {
    allowed |= taiClipData::EA_PASTE;
  }}
}

void taiTabularDataMimeFactory::Mat_EditActionD(taMatrix* mat, 
  const CellRange& sel, taiMimeSource* ms, int ea) const
{
  int allowed = 0;
  int forbidden = 0;
  Mat_QueryEditActions(mat, sel, ms, allowed, forbidden);
  ea = ea & (allowed & ~forbidden);
  
  if (ea & taiClipData::EA_PASTE) {
    taProject* proj = dynamic_cast<taProject*>(mat->GetThisOrOwner(&TA_taProject));
    if(proj) {
      proj->undo_mgr.SaveUndo(mat, "Paste/Copy", mat);
    }
    
    CellRange sel2(sel);
    // if sel is a single cell, adjust to max
    if (sel2.single()) {
      sel2.col_to = mat->geom.dim(0) - 1;
      sel2.row_to = mat->rowCount() - 1;
    }
    // Priority is: Mat, Table, Generic
    {taiMatrixDataMimeItem* mi = (taiMatrixDataMimeItem*)
      ms->GetMimeItem(&TA_taiMatrixDataMimeItem);
    if (mi) {
      mi->WriteMatrix(mat, sel2);
      return;
    }}
    // Table
    {taiTableDataMimeItem* mi = (taiTableDataMimeItem*)
      ms->GetMimeItem(&TA_taiTableDataMimeItem);
    if (mi) {
      mi->WriteMatrix(mat, sel2);
      return;
    }}
    // TSV
    {taiTsvMimeItem* mi = (taiTsvMimeItem*)
      ms->GetMimeItem(&TA_taiTsvMimeItem);
    if (mi) {
      mi->WriteMatrix(mat, sel2);
    }}
  }
}

void taiTabularDataMimeFactory::Mat_EditActionS(taMatrix* mat, 
  const CellRange& sel, int ea) const
{
  int allowed = 0;
  int forbidden = 0;
  Mat_QueryEditActions(mat, sel, NULL, allowed, forbidden);
  ea = ea & (allowed & ~forbidden);
  
  if (ea & taiClipData::EA_COPY) {
    taiClipData* cd = Mat_GetClipData(mat,
      sel, taiClipData::EA_SRC_COPY, false);
    QApplication::clipboard()->setMimeData(cd); //cb takes ownership
  } else
  if (ea & taiClipData::EA_CLEAR) {
    taProject* proj = dynamic_cast<taProject*>(mat->GetThisOrOwner(&TA_taProject));
    if(proj) {
      proj->undo_mgr.SaveUndo(mat, "Clear", mat);
    }
    Mat_Clear(mat, sel);
  }
}

taiClipData* taiTabularDataMimeFactory::Mat_GetClipData(taMatrix* mat,
    const CellRange& sel, int src_edit_action, bool for_drag) const
{
  taiClipData* cd = new taiClipData(src_edit_action);
  AddMatDesc(cd, mat, sel);
  String str = mat->FlatRangeToTSV(sel);
  cd->setTextFromStr(str);
  return cd;
}

void taiTabularDataMimeFactory::AddMatDesc(QMimeData* md,
  taMatrix* mat, const CellRange& sel) const
{
  String str;
  AddDims(sel, str);
  md->setData(tacss_matrixdesc, StrToByteArray(str));
}

void taiTabularDataMimeFactory::AddDims(const CellRange& sel, String& str) const
{
  str = str + String(sel.width()) + ";" + String(sel.height()) + ";";
}

void taiTabularDataMimeFactory::Table_Clear(DataTable* tab,
    const CellRange& sel) const
{
  tab->DataUpdate(true);
  //note: it is easier and more efficient to clear in col-major order
  for (int col = sel.col_fr; col <= sel.col_to; ++col) {
    DataCol* da = tab->GetColData(col, true); // quiet
    if (!da) continue;
    // use bracketed data update, because these get sent to all slices, including gui
    da->AR()->DataUpdate(true);
    int cell_size = da->cell_size();
    for (int row = sel.row_fr; row <= sel.row_to; ++row) {
      for (int cell = 0; cell < cell_size; ++cell) {
        tab->SetValAsVarM(_nilVariant, col, row, cell);
      }
    }
    da->AR()->DataUpdate(false);
  }
  tab->DataUpdate(false);
}


void taiTabularDataMimeFactory::Table_QueryEditActions(DataTable* tab, 
  const CellRange& sel, taiMimeSource* ms,
  int& allowed, int& forbidden) const
{
  // ops that are never allowed on mats
  forbidden |= (taiClipData::EA_CUT | taiClipData::EA_DELETE);
  // forbidden on ro -- whole table
  if (isGuiReadOnly()) 
    forbidden |= taiClipData::EA_FORB_ON_DST_READONLY;
  // selected cols
  bool sel_ro = false; // we'll or in
  for (int col = sel.col_fr; (col <= sel.col_to) && !sel_ro; ++col) {
    DataCol* da = tab->GetColData(col);
    sel_ro = sel_ro || (da->col_flags & DataCol::READ_ONLY);
  }
  if (sel_ro) {
    forbidden |= taiClipData::EA_FORB_ON_DST_READONLY;
  }
  
  // src ops
  if (sel.nonempty())
    allowed |= (taiClipData::EA_COPY | taiClipData::EA_CLEAR);
    
  if (!ms) return;
  // dst ops -- none allowed if no selection
  if (sel.empty()) {
    forbidden = taiClipData::EA_DST_OPS;
    return;
  }
  
  // Priority is: Table, Matrix, Generic
  // Table
  {taiTableDataMimeItem* mi = (taiTableDataMimeItem*)
     ms->GetMimeItem(&TA_taiTableDataMimeItem);
  if (mi) {
    allowed |= taiClipData::EA_PASTE;
    return;
  }}
  // Matrx
  {taiMatrixDataMimeItem* mi = (taiMatrixDataMimeItem*)
     ms->GetMimeItem(&TA_taiMatrixDataMimeItem);
  if (mi) {
    allowed |= taiClipData::EA_PASTE;
    return;
  }}
  // TSV
  {taiTsvMimeItem* mi = (taiTsvMimeItem*)
    ms->GetMimeItem(&TA_taiTsvMimeItem);
  if (mi) {
    allowed |= taiClipData::EA_PASTE;
  }}
}

void taiTabularDataMimeFactory::Table_EditActionD(DataTable* tab, 
  const CellRange& sel, taiMimeSource* ms, int ea) const
{
  int allowed = 0;
  int forbidden = 0;
  Table_QueryEditActions(tab, sel, ms, allowed, forbidden);
  ea = ea & (allowed & ~forbidden);
  
  if (ea & taiClipData::EA_PASTE) {
    taProject* proj = dynamic_cast<taProject*>(tab->GetThisOrOwner(&TA_taProject));
    if(proj) {
      proj->undo_mgr.SaveUndo(tab, "Paste/Copy", tab);
    }

    CellRange sel2(sel);
    //NOTE: unlike matrix pastes, we do NOT adjust selection
    // (the Table item may adjust the selection to fit)
    
    // Priority is: Table, Matrix, Generic
    // Table
    {taiTableDataMimeItem* mi = (taiTableDataMimeItem*)
      ms->GetMimeItem(&TA_taiTableDataMimeItem);
    if (mi) {
      mi->WriteTable(tab, sel2);
      return;
    }}
    // Matrix
    {taiMatrixDataMimeItem* mi = (taiMatrixDataMimeItem*)
      ms->GetMimeItem(&TA_taiMatrixDataMimeItem);
    if (mi) {
      mi->WriteTable(tab, sel2);
      return;
    }}
    // TSV
    {taiTsvMimeItem* mi = (taiTsvMimeItem*)
      ms->GetMimeItem(&TA_taiTsvMimeItem);
    if (mi) {
      mi->WriteTable(tab, sel2);
    }}
  }
}

void taiTabularDataMimeFactory::Table_EditActionS(DataTable* tab, 
  const CellRange& sel, int ea) const
{
  int allowed = 0;
  int forbidden = 0;
  Table_QueryEditActions(tab, sel, NULL, allowed, forbidden);
  ea = ea & (allowed & ~forbidden);
  
  if (ea & taiClipData::EA_COPY) {
    taiClipData* cd = Table_GetClipData(tab,
      sel, taiClipData::EA_SRC_COPY, false);
    QApplication::clipboard()->setMimeData(cd); //cb takes ownership
    return;
  } else
  if (ea & taiClipData::EA_CLEAR) {
    taProject* proj = dynamic_cast<taProject*>(tab->GetThisOrOwner(&TA_taProject));
    if(proj) {
      proj->undo_mgr.SaveUndo(tab, "Clear", tab);
    }
    Table_Clear(tab, sel);
    return;
  }
}

taiClipData* taiTabularDataMimeFactory::Table_GetClipData(DataTable* tab,
    const CellRange& sel, int src_edit_action, bool for_drag) const
{
  taiClipData* cd = new taiClipData(src_edit_action);
  AddTableDesc(cd, tab, sel);
  String str = tab->RangeToTSV(sel);
  cd->setTextFromStr(str);
  return cd;
}

void taiTabularDataMimeFactory::AddTableDesc(QMimeData* md,
  DataTable* tab, const CellRange& sel) const
{
  STRING_BUF(str, 250);
  // add the flat dims
  int tot_col = 0; // total flat cols
  int max_cell_row = 0; // max flat rows per cell
  tab->GetFlatGeom(sel, tot_col, max_cell_row);
  str.cat(String(tot_col)).cat(';').cat(String(max_cell_row)).cat(";\n");
 
  // add the table dims
  AddDims(sel, str);
  str.cat('\n');
  
  // add the col descs
  for (int col = sel.col_fr; col <= sel.col_to; ++col) {
    DataCol* da = tab->GetColData(col, true); // quiet
    int x; int y;
    da->Get2DCellGeom(x, y); 
    str.cat(String(x)).cat(';').cat(String(y)).cat(';');
    str.cat(String(da->isImage())).cat(";\n");
  }
  md->setData(tacss_tabledesc, StrToByteArray(str));
}



//////////////////////////////////
//  taiTabularDataMimeItem 	//
//////////////////////////////////

/* TSV reading engine

the most common spreadsheet format is TSV -- note that
tabs and eols are separators, not terminators

value[<tab>value] 
[<eol>] value[<tab>value]

We need to be able to read, being able to skip cols,
knowing when we reach eols.

Parsing geoms:

  #rows = #eols - 1
  #cols = row0(#tabs - 1)
  


*/

taiTabularDataMimeItem::TsvSep taiTabularDataMimeItem::no_sep;

bool taiTabularDataMimeItem::ReadInt(String& arg, int& val) {
  String str;
  bool ok;
  str = arg.before(';'); // cols
  val = str.toInt(&ok);
  arg = arg.after(";");
  return ok;
}

bool taiTabularDataMimeItem::ExtractGeom(String& arg, iSize& val) {
  String str;
  bool ok = ReadInt(arg, val.w);
  if (!ok) goto end;
  ok = ReadInt(arg, val.h); 
  if (!ok) goto end;
  
end:
  return ok;
}

void taiTabularDataMimeItem::WriteMatrix(taMatrix* mat, const CellRange& sel) {
  // get the text/plain data and set it into a stream
  istringstream istr;
  QByteArray ba = data(taiMimeFactory::text_plain);
  istr.str(string(ba.data(), ba.size()));
  
  String val; // each cell val
  TsvSep sep; // sep after reading the cell val

  // client needs to have adjust paste region if necessary; we take it literally
  
  int row = sel.row_fr;
  mat->DataUpdate(true);
  while (row <= sel.row_to) {
    int col = sel.col_fr;
    while (col <= sel.col_to) {
      if (!ReadTsvValue(istr, val, sep)) goto done;
      int idx = mat->FastElIndex2D(col, row);
      mat->SetFmStr_Flat(val, idx);
      if (sep == TSV_EOF) goto done;
      // if we've run out of col data, skip to next row
      if (sep == TSV_EOL) goto next_row;
      ++col;
      
    }
    // if we haven't run out of col data yet, skip input data
    while (sep != TSV_EOL) {
      if (!ReadTsvValue(istr, val, sep)) goto done;
      if (sep == TSV_EOF) goto done;
    }
next_row:
    ++row;
  }
done:
  mat->DataUpdate(false);
  //NOTE: this is VERY hacky, but easiest way to update tables...
  DataCol* col = (DataCol*)mat->GetOwner(&TA_DataCol);
  if (col) {
    col->DataChanged(DCR_ITEM_UPDATED);
  }
}

void taiTabularDataMimeItem::WriteTable(DataTable* tab, const CellRange& sel) {
  // default does the generic -- Table source is more flexible
  WriteTable_Generic(tab, sel);
}

void taiTabularDataMimeItem::WriteTable_Generic(DataTable* tab, const CellRange& sel_) {
  // for generic-to-table copy, we apply data on a fully flattened basis
  // we paste in each direction (rows/cols) until we run out of data, or table
  istringstream istr;
  QByteArray ba = data(taiMimeFactory::text_plain);
  istr.str(string(ba.data(), ba.size()));
  
  String val; // each cell val
  TsvSep sep; // sep after reading the cell val
  
  // for generic source, for single-cell, we autoextend the range
  // to the entire extent of the table
  
  CellRange sel(sel_);
  if (sel_.single()) {
    sel.row_to = tab->rows - 1;
    sel.col_to = tab->cols() - 1;
  } 
  
  // we will use the first row pass to find the max cell rows
  int max_cell_rows = 1; // has to be at least 1 (for all scalars)
  tab->DataUpdate(true); 
  // we only iterate over the dst rows; we'll just stop reading data when done 
  // and/or break out if we run out of data
  for (int dst_row = sel.row_fr; dst_row <= sel.row_to; ++dst_row) {
    for (int cell_row = 0; cell_row < max_cell_rows; ++cell_row) {
      // we keep iterating while we have data and cols
      for (int dst_col = sel.col_fr; dst_col <= sel.col_to; ++dst_col) 
      {
        // here, we get the detailed cell geom for src and dst
        // just assume either could be empty, for robustness
        int dst_cell_cols = 0; //dummy
        int dst_cell_rows = 0;
        DataCol* da = tab->GetColData(dst_col, true); // quiet
        if (da) da->Get2DCellGeom(dst_cell_cols, dst_cell_rows); 
        //note: only need to do the following in the very first cell_row
        max_cell_rows = MAX(max_cell_rows, dst_cell_rows);
        // we break if we run out of data
        for (int cell_col = 0; cell_col < dst_cell_cols; ++cell_col) 
        {
          // we are always in source range here... 
          if (!ReadTsvValue(istr, val, sep)) goto done;
          
          // if we are in dst range then write the value
          if (/*row always valid*/ (cell_row < dst_cell_rows) &&
             (cell_col < dst_cell_cols))
          {
            int dst_cell = (cell_row * dst_cell_cols) + cell_col;
            tab->SetValAsStringM(val, dst_col, dst_row, dst_cell);
          }
          
          if (sep == TSV_EOF) goto done;
          // if we've run out of col data, skip to next row
          if (sep == TSV_EOL) {
            // if we've run out of data, set this as last col and break
            sel.col_to = dst_col;
            goto next_row;
          }
        }
        // if we're on the last col, we may have ran out of table -- read the rest of the row here, which will cause the loop
  // to be
        if (dst_col == sel.col_to) while (sep != TSV_EOL) {
          if (!ReadTsvValue(istr, val, sep)) goto done;
          if (sep == TSV_EOF) goto done;
        }
      } // dst tab cols
next_row: ;
    } // dst tab cell rows
  } // dst tab rows
done:
  tab->DataUpdate(false);
}

bool taiTabularDataMimeItem::ReadTsvValue(istringstream& strm, String& val,
  TsvSep& sep)
{
  // unless 1st read is EOF, we will succeed in reading something
  val = _nilString;
  int c = strm.peek();
  if (c == EOF) {
    sep = TSV_EOF;
    return false;
  }
  while (c != EOF) {
    if (c == '\t') {
      strm.get();
      sep = TSV_TAB;
      break;
    }
    if ((c == '\n') || (c == '\r')) {
      strm.get();
      sep = TSV_EOL;
      // Windows/DOS cr+lf
      if ((c == '\r') && (strm.peek() == '\n')) {
        strm.get();
      }
      // we ignore terminating eol
      if (strm.peek() == EOF) {
        strm.get();
        sep = TSV_EOF;
      }
      break;
    }
    
    val += (char)c; 
    strm.get();
    c = strm.peek();
    if (c == EOF) {
      strm.get();
      sep = TSV_EOF;
    }
  }
  return true;
}

//////////////////////////////////
//  taiMatrixDataMimeItem 	//
//////////////////////////////////

taiMimeItem* taiMatrixDataMimeItem::Extract(taiMimeSource* ms, 
    const String& subkey)
{
  if (!ms->hasFormat(taiTabularDataMimeFactory::tacss_matrixdesc)) 
    return NULL;
  taiMatrixDataMimeItem* rval = new taiMatrixDataMimeItem;
  rval->Constr(ms, subkey);
  return rval;
}

bool taiMatrixDataMimeItem::Constr_impl(const String&) {
  String arg;
  data(mimeData(), taiTabularDataMimeFactory::tacss_matrixdesc, arg);
  
  return ExtractGeom(arg, m_flat_geom);
}

void  taiMatrixDataMimeItem::DecodeData_impl() {
//note: maybe nothing!
}


//////////////////////////////////
//  taiTsvMimeItem	 	//
//////////////////////////////////

taiMimeItem* taiTsvMimeItem::Extract(taiMimeSource* ms, 
    const String& subkey)
{
  // note: virtually everything has text/plain, so we just grab and examine
  // note: tabs are separators, not terminators
  // note: some formats (ex Excel) use eol as a sep, others (ex OO.org) as a terminator
  QByteArray ba = ms->data(taiTabularDataMimeFactory::text_plain);
  if (ba.size() == 0) return NULL;
  // first, look at the first line -- we will pull tent cols from this, then 
  // double check against the remainder
  QTextStream ts(ba, QIODevice::ReadOnly); // ro stream
  QString str = ts.readLine();
  int cols = str.count('\t', Qt::CaseInsensitive) + 1;
  // now, if it is really a table, it should have total metrics consistent
  
  // see if it ends in an eol, adjust if necessary
  int tot_rows = 1; 
  if (ba.endsWith('\n')) // note: works for Unix or Windows
    tot_rows = 0;
  
  tot_rows = ba.count('\n') + tot_rows; // note: works for Unix or Windows
  int tot_tabs = ba.count('\t');
  if (((cols - 1) * tot_rows) != tot_tabs)
    return NULL;
  
  taiTsvMimeItem* rval = new taiTsvMimeItem;
  rval->Constr(ms, subkey); //note: doesn't do anything
  rval->m_flat_geom.set(cols, tot_rows);
  return rval;
}


void taiTsvMimeItem::Initialize() {
}


//////////////////////////////////
//  taiTableDataMimeItem	 	//
//////////////////////////////////

taiMimeItem* taiTableDataMimeItem::Extract(taiMimeSource* ms, 
    const String& subkey)
{
  if (!ms->hasFormat(taiTabularDataMimeFactory::tacss_tabledesc)) 
    return NULL;
  taiTableDataMimeItem* rval = new taiTableDataMimeItem;
  rval->Constr(ms, subkey);
  return rval;
}

void taiTableDataMimeItem::Initialize() {
  m_max_row_geom = 0;
}

bool taiTableDataMimeItem::Constr_impl(const String&) {
  String arg;
  data(mimeData(), taiTabularDataMimeFactory::tacss_tabledesc, arg);
 
  String str; // temp
  bool ok = ExtractGeom(arg, m_flat_geom);
  if (!ok) goto done;
  arg = arg.after('\n');
  ok = ExtractGeom(arg, m_tab_geom);
  if (!ok) goto done;
  arg = arg.after('\n');
   
  col_descs.Alloc(m_tab_geom.w);
  m_max_row_geom = 0;
  for (int col = 0; col < m_tab_geom.w; ++col) {
    taiTableColDesc& desc = col_descs[col];
    ok = ExtractGeom(arg, desc.flat_geom);
    if (!ok) goto done;
    str = arg.before(';');
    desc.is_image = str.toBool(); 
    arg = arg.after('\n');
    m_max_row_geom = MAX(m_max_row_geom, desc.flat_geom.h);
  }
   
done:
  return ok;
}

void taiTableDataMimeItem::DecodeData_impl() {
//note: maybe nothing!
}

void taiTableDataMimeItem::GetColGeom(int col, int& cols, int& rows) const {
  if ((col >= 0) && (col < col_descs.size)) {
    taiTableColDesc& desc = col_descs[col];
    cols = desc.flat_geom.w;
    rows = desc.flat_geom.h;
  } else {
    cols = 0;
    rows = 0;
  }
}

/* 
  A table supports two kinds of paste:
    * Paste -- starting at upper-left of select region, no new rows made
    * Paste Append -- appends all data to new rows; the first dst col is
      marked by the left selected col -- other than that, the sel region
      has no significance
  (Paste Append is especially convenient when selected from the Table
    object itself, rather than in the data grid.)
    
  For table-to-table copy, we assume the user wants the source col mappings
  to be preserved at the destination. This leads to three possibilities for
  each dimension in each cell:
    * the src and dst have the same dim
    * the src is bigger -- we will ignore the excess
    * the src is smaller -- we will nil out the underflow
  We should iterate, using a parallel set of indexes, one for src, one for
  dst; we will always need to do something for the max of these two, either
  skipping src data, or clearing out dst data; the only exception is row:
  we only need iterate the dst_rows, since we can just stop reading src
  data when we are finished.
  
  Since we placed the exact dims for each col on the clipboard, and we
  have a deterministic mapping of the flat data, we can therefore "dumbly"
  just read in the data, without checking for tabs/eols etc.
  
  
*/
void taiTableDataMimeItem::WriteTable(DataTable* tab, const CellRange& sel_) {
  // for table-to-table copy, we apply data on a table-cell basis
  // (not a fully flattened basis)
  istringstream istr;
  QByteArray ba = data(taiMimeFactory::text_plain);
  istr.str(string(ba.data(), ba.size()));
  
  String val; // each cell val
//nn  TsvSep sep; // sep after reading the cell val
  
  // if dest is a single cell, we will extend to size of src
  CellRange sel(sel_);
  if (sel_.single()) {
    sel.SetExtent(tabCols(), tabRows());
    sel.LimitRange(tab->rows - 1, tab->cols() - 1);
  } 
  // if dest is larger than src, we shrink to src
  else {
    sel.LimitExtent(tabCols(), tabRows());
  }
  
  // calculate the controlling params, for efficiency and clarity
  int src_rows = tabRows();
  int dst_max_cell_rows = tab->GetMaxCellRows(sel.col_fr, sel.col_to);
  int src_max_cell_rows = maxCellRows();
  int max_cell_rows = MAX(src_max_cell_rows, dst_max_cell_rows);
  // for cols, we only iterate over src, since we don't clear excess dst cols
  int src_cols = tabCols();
//nn  int dst_cols = sel.width();
  
  bool underflow = false;
  tab->DataUpdate(true); 
  // we only iterate over the dst rows; we'll just stop reading data when done 
  int dst_row; int src_row;
  for (dst_row = sel.row_fr, src_row = 0;
       dst_row <= sel.row_to;
       ++dst_row, ++src_row) 
  {
    // cell_row is same for src/dst (0-based), but we need to iterate the largest
    for (int cell_row = 0; cell_row < max_cell_rows; ++cell_row) {
      // for cols, we need to iterate over largest of src/dst
      int dst_col; int src_col;
      for (dst_col = sel.col_fr, src_col = 0;
          src_col < src_cols;
          ++dst_col, ++src_col) 
      {
        // here, we get the detailed cell geom for src and dst
        // just assume either could be empty, for robustness
        int dst_cell_cols = 0; 
        int dst_cell_rows = 0;
        DataCol* da = tab->GetColData(dst_col, true); // quiet
        if (da) da->Get2DCellGeom(dst_cell_cols, dst_cell_rows); 
        int src_cell_cols = 0;
        int src_cell_rows = 0;
        if (src_col < src_cols) {
          taiTableColDesc& tcd = col_descs[src_col];
          src_cell_cols = tcd.flat_geom.w;
          src_cell_rows = tcd.flat_geom.h;
        }
        int cell_cols = MAX(src_cell_cols, dst_cell_cols);
        for (int cell_col = 0; cell_col < cell_cols; ++cell_col) {
          // if we are in **flat** source range then read a value 
          // note that src values are always zero-based
          if ((src_row < src_rows) && (cell_row < src_max_cell_rows) &&
            (src_col < src_cols) && (cell_col < src_cell_cols))
          {
            underflow = underflow || (!ReadTsvValue(istr, val/*, sep*/));
          } else 
            val = _nilString;
          
          // if we are in dst range then write (the maybe nil) value
          // we only need to check upper bound, since we always started at lower
          if (/*row always valid*/ (cell_row < dst_cell_rows) &&
            (dst_col <= sel.col_to) && (cell_col < dst_cell_cols))
          {
            int dst_cell = (cell_row * dst_cell_cols) + cell_col;
            tab->SetValAsStringM(val, dst_col, dst_row, dst_cell);
          }
        }
      }
    }
  }
  tab->DataUpdate(false);
#ifdef DEBUG
  if (underflow)
    taMisc::Warning("Unexpected underflow of table data pasting table-to-table");
#endif
}


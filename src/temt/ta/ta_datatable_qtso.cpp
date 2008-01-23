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

#include "ilineedit.h"
#include "ispinbox.h"

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
#include <Q3ScrollView>
#include <QSplitter>
#include <QTableView>
#include <QTextStream>

//#include <Q3VBox>

#include <Inventor/SbLinear.h>
#include <Inventor/Qt/SoQt.h>
#include <Inventor/Qt/SoQtRenderArea.h>
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

#include <Inventor/Qt/viewers/SoQtExaminerViewer.h>

#include <limits.h>
#include <float.h>
//nn? #include <unistd.h>

#define DIST(x,y) sqrt((double) ((x * x) + (y*y)))

//////////////////////////////////
//   DataTableModel		//
//////////////////////////////////

DataTableModel::DataTableModel(DataTable* dt_, QWidget* gui_parent_) 
:inherited(NULL)
{
  m_dt = dt_;
  if (m_dt) {
    m_dt->AddDataClient(this);
  }
  gui_parent = gui_parent_;
}

DataTableModel::~DataTableModel() {
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
{
  //this is primarily for code-driven changes
  if (dcr <= DCR_ITEM_UPDATED_ND) {
    emit_layoutChanged(); // need to update layout for when rows added/removed
//nw    emit_dataChanged();
  }
  else if ((dcr == DCR_STRUCT_UPDATE_END)) { // for col insert/deletes
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

void DataTableModel::emit_layoutChanged() {
  emit layoutChanged();
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
  if (!m_dt || (role != Qt::DisplayRole))
    return QVariant();
  if (orientation == Qt::Horizontal) {
    DataCol* col = m_dt->GetColData(section, true); // quiet
    if (col)  
      return QString(col->GetDisplayName().chars());
    else 
      return QString();
  } else {
    return QString::number(section);
  }
}

bool DataTableModel::ignoreDataChanged() const {
  return (gui_parent && !gui_parent->isVisible());
}

void DataTableModel::refreshViews() {
  emit_layoutChanged();
/*  emit dataChanged(createIndex(0, 0), 
    createIndex(rowCount() - 1, columnCount() - 1));*/
}

int DataTableModel::rowCount(const QModelIndex& parent) const {
  return (m_dt) ? m_dt->rows : 0;
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
    emit dataChanged(index, index);
    col->DataChanged(DCR_ITEM_UPDATED); // for calc refresh
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
  name = cp.name;
  visible = cp.visible;
//   sticky = cp.sticky;
}

void DataColView::Destroy() {
  CutLinks();
}

bool DataColView::SetName(const String& value) { 
  name = value;  
  return true; 
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
  if (name != col->name) {
    name = col->name;
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

String DataTableView::GetLabel() const {
  DataTable* dt = dataTable(); 
  if(dt) return dt->GetName();
  return "(no table)";
}

String DataTableView::GetName() const {
  DataTable* dt = dataTable(); 
  if(dt) return dt->GetName();
  return "(no table)";
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

void DataTableView::DataStructUpdateEnd_impl() {
  inherited::DataStructUpdateEnd_impl();
  // default case is good enough: does full reconstruct
}

void DataTableView::DataUpdateView_impl() {
  if(!display_on) return;
  UpdateDisplay(true);
}

void DataTableView::DataUpdateAfterEdit_impl() {
  inherited::DataUpdateAfterEdit_impl();
  // this is actually fully redundant..
  //  UpdateDisplay(true);
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

void DataTableView::UpdateFromDataTable_this(bool first) {
//   DataTable* dt = dataTable();
//   if (first) {
// //     if (name != dt->name)
// //       name = dt->name;
//   }
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
    if (cols->FindEl(dcs->dataCol()) >= 0) {
      dcs->UpdateFromDataCol();
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
    int fm;
    bool first = false;
    if ((dcs = (DataColView*)children.FindName(dc->GetName(), fm))) {
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
  if(m_lvp && m_lvp->t3vs) {
    m_lvp->t3vs->setSceneGraph(NULL);	// remove existing scene graph
  }
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
  if (m_lvp && m_lvp->t3vs) {
    m_lvp->t3vs->setSceneGraph(node_so()); //TODO: change to node_so()->canvas()
    m_lvp->viewAll();
  }
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

/*obs
void DataTableView::View_F() {
  if(!taMisc::gui_active) return;
  int shft = (int)((float)view_rows * view_shift);
  if(shft < 1) shft = 1;
  view_range.max += shft;
  int log_max = MAX(log()->log_lines-1, log()->data_range.max);
  view_range.MaxLT(log_max); // keep it less than max
  view_range.min = view_range.max - view_rows +1; // always keep min and max valid
  view_range.MinGT(0);
  if(view_range.max > log()->data_range.max) {
    log()->Buffer_F();
  }
  view_range.MaxLT(log()->data_range.max); // redo now that data_range.max is final
  view_range.min = view_range.max - view_rows +1;
  view_range.MinGT(log()->data_range.min);
  UpdateFromBuffer();
}

void DataTableView::View_FSF() {
  if(!taMisc::gui_active) return;
  view_range.max += view_rows;
  int log_max = MAX(log()->log_lines-1, log()->data_range.max);
  view_range.MaxLT(log_max); // keep it less than max
  view_range.min = view_range.max - view_rows +1; // always keep min and max valid
  view_range.MinGT(0);
  if(view_range.max > log()->data_range.max) {
    log()->Buffer_F();
  }
  view_range.MaxLT(log()->data_range.max); // redo now that data_range.max is final
  view_range.min = view_range.max - view_rows +1;
  view_range.MinGT(log()->data_range.min);
  UpdateFromBuffer();
}

void DataTableView::View_FF() {
  if(!taMisc::gui_active) return;
  view_range.max = MAX(log()->log_lines-1, log()->data_range.max);
  view_range.min = view_range.max - view_rows +1; // always keep min and max valid
  view_range.MinGT(0);
  if(view_range.max > log()->data_range.max) {
    log()->Buffer_FF();
  }
  view_range.MaxLT(log()->data_range.max); // redo now that data_range.max is final
  view_range.min = view_range.max - view_rows +1;
  view_range.MinGT(log()->data_range.min);
  UpdateFromBuffer();
}

void DataTableView::View_R() {
  if(!taMisc::gui_active) return;
  int shft = (int)((float)view_rows * view_shift);
  if(shft < 1) shft = 1;
  view_range.min -= shft;
  view_range.MinGT(0);
  view_range.max = view_range.min + view_rows -1; // always keep min and max valid
  int log_max = MAX(log()->log_lines-1, log()->data_range.max);
  view_range.MaxLT(log_max); // keep it less than max
  if(view_range.min < log()->data_range.min) {
    log()->Buffer_R();
  }
  view_range.MinGT(log()->data_range.min); // redo now that data_range.min is final
  view_range.max = view_range.min + view_rows -1;
  view_range.MaxLT(log()->data_range.max);
  UpdateFromBuffer();
}

void DataTableView::View_FSR() {
  if(!taMisc::gui_active) return;
  view_range.min -= view_rows;
  view_range.MinGT(0);
  view_range.max = view_range.min + view_rows -1; // always keep min and max valid
  int log_max = MAX(log()->log_lines-1, log()->data_range.max);
  view_range.MaxLT(log_max); // keep it less than max
  if(view_range.min < log()->data_range.min) {
    log()->Buffer_R();
  }
  view_range.MinGT(log()->data_range.min); // redo now that data_range.min is final
  view_range.max = view_range.min + view_rows -1;
  view_range.MaxLT(log()->data_range.max);
  UpdateFromBuffer();
}

void DataTableView::View_FR() {
  if(!taMisc::gui_active) return;
  view_range.min = 0;
  view_range.max = view_range.min + view_rows -1; // always keep min and max valid
  view_range.MinGT(log()->data_range.min); // redo now that data_range.min is final
  view_range.max = view_range.min + view_rows -1;
  view_range.MaxLT(log()->data_range.max);
  UpdateFromBuffer();
}
*/


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

//////////////////////////////////
//  GridTableView		//
//////////////////////////////////

GridTableView* GridTableView::New(DataTable* dt, T3DataViewFrame*& fr) {
  if (!dt) return NULL;
  if (fr) {
    //note: even if fr specified, need to insure it is right proj for object
    if (!dt->SameScope(fr, &TA_taProject)) {
      taMisc::Error("The viewer you specified is not in the same Project as the table.");
      return NULL;
    }
    // check if already viewing this obj there, warn user
    // no, want to be able to graph and grid in same view..
//     T3DataView* dv = fr->FindRootViewOfData(dt);
//     if (dv) {
//       if (taMisc::Choice("This table is already shown in that frame -- would you like"
//           " to show it in a new frame?", "&Ok", "&Cancel") != 0) return NULL;
//       fr = NULL; // make a new one
//     }
  } 
  if (!fr) {
    fr = T3DataViewer::GetBlankOrNewT3DataViewFrame(dt);
  }
  if (!fr) return NULL; // unexpected...
  
  GridTableView* vw = new GridTableView;
  fr->AddView(vw);
  vw->setDataTable(dt);
  // make sure we get it all setup!
  vw->BuildAll();
  fr->Render();
  fr->ViewAll();
  fr->GetCameraPosOrient();
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

void GridTableView::UpdateAfterEdit_impl(){
  inherited::UpdateAfterEdit_impl();
  if (grid_margin < 0.0f) grid_margin = 0.0f;
  if (grid_line_size <  0.0f) grid_line_size =  0.0f;
}

String GridTableView::GetLabel() const {
  return inherited::GetLabel() + " Grid";
}

String GridTableView::GetName() const {
  return inherited::GetName() + " Grid";
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

void GridTableView::Render_pre() {
  bool show_drag = manip_ctrl_on;
  SoQtViewer* vw = GetViewer();
  if(vw)
    show_drag = !vw->isViewing();

  setNode(new T3GridViewNode(this, width, show_drag));

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
  
  if (!header_on) return; // normally shouldn't be called if off

  SoComplexity* cplx = new SoComplexity;
  cplx->value.setValue(taMisc::text_complexity);
  hdr->addChild(cplx);

  SoFont* fnt = new SoFont();
  fnt->name = "Arial";
  if(two_d_font)
    fnt->size.setValue(two_d_font_scale * font_scale);
  else
    fnt->size.setValue(font_scale);
  hdr->addChild(fnt);

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
    colnd->material()->diffuseColor.setValue(0.0f, 0.0f, 0.0f); // black text
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

  int col_idx = 0;
  float col_wd_lst = 0.0f; // width of last col
  // following metrics will be adjusted for mat font scale, when necessary
  float txt_base_adj = text_ht * t3Misc::char_base_fract;

  float row_ht = row_height - gr_mg_sz2;
  float mat_rot_rad = mat_rot / taMath_float::deg_per_rad;

  // render row_num cell, if on
  if (row_num_on) {
    col_wd_lst = col_widths[col_idx++];
    SoSeparator* row_sep = new SoSeparator;
    tr = new SoTranslation;
    row_sep->addChild(tr);
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

  // master font -- we only add a child font if different
  // doesn't seem to make much diff:
  SoComplexity* cplx = new SoComplexity;
  cplx->value.setValue(taMisc::text_complexity);
  body->addChild(cplx);

  SoFont* fnt = new SoFont();
  fnt->name = "Arial";
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

// callback for view transformer dragger
void T3GridViewNode_DragFinishCB(void* userData, SoDragger* dragr) {
  SoTransformBoxDragger* dragger = (SoTransformBoxDragger*)dragr;
  T3GridViewNode* vnd = (T3GridViewNode*)userData;
  GridTableView* nv = (GridTableView*)vnd->dataView();

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


//////////////////////////
//    iTableView_Panel //
//////////////////////////

iDataTableView_Panel::iDataTableView_Panel(DataTableView* lv)
:inherited(lv)
{
  t3vs = NULL; //these are created in  Constr_T3ViewspaceWidget
  m_ra = NULL;
  m_camera = NULL;
  
  widg = new QWidget();
//  widg->setFrameStyle( QFrame::GroupBoxPanel | QFrame::Sunken );
  layWidg = new QVBoxLayout(widg); //def margin/spacing=2  
  layWidg->setMargin(0); layWidg->setSpacing(2);
  setCentralWidget(widg);
}

iDataTableView_Panel::~iDataTableView_Panel() {
}

void iDataTableView_Panel::Constr_T3ViewspaceWidget(QWidget* widg) {
  t3vs = new iT3ViewspaceWidget(widg);
  t3vs->setSelMode(iT3ViewspaceWidget::SM_MULTI); // default

  m_ra = new SoQtRenderArea(t3vs);
  t3vs->setRenderArea(m_ra);

  SoSeparator* root = t3vs->root_so();
  m_camera = new SoPerspectiveCamera();
  root->addChild(m_camera);

  m_lm = new SoLightModel();
  m_lm->model = SoLightModel::BASE_COLOR;
  root->addChild(m_lm);

  viewAll();
}

void iDataTableView_Panel::viewAll() {
  if(!t3vs) return;
  m_camera->viewAll(t3vs->root_so(), ra()->getViewportRegion());
  m_camera->focalDistance.setValue(.1f); // zoom in!
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

  chkDisplay = new QCheckBox("Disp", widg); chkDisplay->setObjectName("chkDisplay");
  chkDisplay->setToolTip("Whether to update the display when the underlying data changes");
  connect(chkDisplay, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
  layTopCtrls->addWidget(chkDisplay);

  chkHeaders =  new QCheckBox("Hdrs", widg ); chkHeaders->setObjectName("chkHeaders");
  chkHeaders->setToolTip("Whether to display a top row of headers indicating the name of the columns");
  connect(chkHeaders, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
  layTopCtrls->addWidget(chkHeaders);

  chkRowNum =  new QCheckBox("Row\n#", widg); chkRowNum->setObjectName("chkRowNum");
  chkRowNum->setToolTip("Whether to display the row number as the first column");
  connect(chkRowNum, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
  layTopCtrls->addWidget(chkRowNum);

  chk2dFont =  new QCheckBox("2d\nFont", widg); chk2dFont->setObjectName("chk2dFont");
  chk2dFont->setToolTip("Whether to use a two-dimensional font that is easier to read but does not obey 3d transformations of the display");
  connect(chk2dFont, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
  layTopCtrls->addWidget(chk2dFont);

  lblFontScale = taiM->NewLabel("Font\nScale", widg, font_spec);
  lblFontScale->setToolTip("Scaling of the 2d font to make it roughly the same size as the 3d font -- adjust this to change the size of the 2d text (has no effect if 2d Font is not clicked");
  layTopCtrls->addWidget(lblFontScale);
  fldFontScale = new taiField(&TA_float, this, NULL, widg);
  layTopCtrls->addWidget(fldFontScale->GetRep());
//   layMatrix->addSpacing(taiM->hsep_c);

  layTopCtrls->addStretch();

  butRefresh = new QPushButton("Refresh", widg);
  butRefresh->setFixedHeight(taiM->button_height(taiMisc::sizSmall));
  layTopCtrls->addWidget(butRefresh);
  connect(butRefresh, SIGNAL(pressed()), this, SLOT(butRefresh_pressed()) );

  // not clear we want the user to be able to clear the table like this -- may just
  // think it is the display, not the actual underlying data..
//   butClear = new QPushButton("Clear", widg);
//   butClear->setFixedHeight(taiM->button_height(taiMisc::sizSmall));
//   layTopCtrls->addWidget(butClear);
//   connect(butClear, SIGNAL(pressed()), this, SLOT(butClear_pressed()) );

  layVals = new QHBoxLayout; layWidg->addLayout(layVals);

  lblRows = taiM->NewLabel("Rows", widg, font_spec);
  lblRows->setToolTip("Maximum number of rows to display (row height is scaled to fit).");
  layVals->addWidget(lblRows);
  fldRows = new taiIncrField(&TA_int, this, NULL, widg);
  layVals->addWidget(fldRows->GetRep());
//   layVals->addSpacing(taiM->hsep_c);

  lblCols = taiM->NewLabel("Cols", widg, font_spec);
  lblCols->setToolTip("Maximum number of columns to display (column widths are scaled to fit).");
  layVals->addWidget(lblCols);
  fldCols = new taiIncrField(&TA_int, this, NULL, widg);
  layVals->addWidget(fldCols->GetRep());
//   layVals->addSpacing(taiM->hsep_c);

  lblWidth = taiM->NewLabel("Width", widg, font_spec);
  lblWidth->setToolTip("Width of grid log display, in normalized units (default is 1.0 = same as height).");
  layVals->addWidget(lblWidth);
  fldWidth = new taiField(&TA_float, this, NULL, widg);
  layVals->addWidget(fldWidth->GetRep());
//   layVals->addSpacing(taiM->hsep_c);

  lblTxtMin = taiM->NewLabel("Min\nText", widg, font_spec);
  lblTxtMin->setToolTip("Minimum text size in 'view units' (size of entire display is 1.0) -- .02 is default -- increase to make small text more readable");
  layVals->addWidget(lblTxtMin);
  fldTxtMin = new taiField(&TA_float, this, NULL, widg);
  layVals->addWidget(fldTxtMin->GetRep());
//   layMatrix->addSpacing(taiM->hsep_c);

  lblTxtMax = taiM->NewLabel("Max\nText", widg, font_spec);
  lblTxtMax->setToolTip("Maximum text size in 'view units' (size of entire display is 1.0) -- .05 is default");
  layVals->addWidget(lblTxtMax);
  fldTxtMax = new taiField(&TA_float, this, NULL, widg);
  layVals->addWidget(fldTxtMax->GetRep());
//   layMatrix->addSpacing(taiM->hsep_c);

  layVals->addStretch();

  ////////////////////////////////////////////////////////////////////////////
  layMatrix = new QHBoxLayout; layWidg->addLayout(layMatrix);

  lblMatrix = taiM->NewLabel("Matrix\nDisplay", widg, font_spec);
  lblMatrix->setToolTip("This row contains parameters that control the display of matrix values (shown in a grid of colored blocks)");
  layMatrix->addWidget(lblMatrix);

  chkValText =  new QCheckBox("Val\nTxt", widg); chkValText->setObjectName( "chkValText");
  chkValText->setToolTip("Whether to display text of the matrix block values.");
  connect(chkValText, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
  layMatrix->addWidget(chkValText);

  lblTrans = taiM->NewLabel("Trans-\nparency", widg, font_spec);
  lblTrans->setToolTip("Maximum transparency of the grid blocks (0 = fully opaque, 1 = fully transparent)\nBlocks with smaller magnitude values are more transparent.");
  layMatrix->addWidget(lblTrans);
  fldTrans = new taiField(&TA_float, this, NULL, widg);
  layMatrix->addWidget(fldTrans->GetRep());
//   layMatrix->addSpacing(taiM->hsep_c);

  lblRot = taiM->NewLabel("Mat\nRot", widg, font_spec);
  lblRot->setToolTip("Rotation (in degrees) of the matrix in the Z axis, producing a denser stacking of patterns.");
  layMatrix->addWidget(lblRot);
  fldRot = new taiField(&TA_float, this, NULL, widg);
  layMatrix->addWidget(fldRot->GetRep());
//   layMatrix->addSpacing(taiM->hsep_c);

  lblBlockHeight = taiM->NewLabel("Blk\nHgt", widg, font_spec);
  lblBlockHeight->setToolTip("Maximum height of grid blocks (in Z dimension), as a proportion of their overall X-Y size.");
  layMatrix->addWidget(lblBlockHeight);
  fldBlockHeight = new taiField(&TA_float, this, NULL, widg);
  layMatrix->addWidget(fldBlockHeight->GetRep());
//   layMatrix->addSpacing(taiM->hsep_c);


  ////////////////////////////////////////////////////////////////////////////
  // 	Colorscale etc
  layColorScale = new QHBoxLayout; layWidg->addLayout(layColorScale);
  
  chkAutoScale = new QCheckBox("auto\nscale", widg);
  connect(chkAutoScale, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
  layColorScale->addWidget(chkAutoScale);

  cbar = new HCScaleBar(&tlv->colorscale, ScaleBar::RANGE, true, true, widg);
//  cbar->setMaximumWidth(30);
  connect(cbar, SIGNAL(scaleValueChanged()), this, SLOT(cbar_scaleValueChanged()) );
  layColorScale->addWidget(cbar); // stretchfact=1 so it stretches to fill the space

  butSetColor = new QPushButton("Colors", widg);
  butSetColor->setFixedHeight(taiM->button_height(taiMisc::sizSmall));
  layColorScale->addWidget(butSetColor);
  connect(butSetColor, SIGNAL(pressed()), this, SLOT(butSetColor_pressed()) );

  layWidg->addStretch();

  ////////////////////////////////////////////////////////////////////////////
  // 	viewspace guy

  // not used anymore!
//   layViewspace = new QHBoxLayout(layWidg);
//   Constr_T3ViewspaceWidget(widg);
//   layViewspace->addWidget(t3vs);

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
  
  glv->UpdateDisplay(false); // don't update us, because logic will do that anyway
}


void iGridTableView_Panel::UpdatePanel_impl() {
  inherited::UpdatePanel_impl();

  GridTableView* glv = this->glv(); //cache
  if (!glv) return;

  chkDisplay->setChecked(glv->display_on);
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

  // bg color
//   SoQtRenderArea* ra = t3vs->renderArea();
//   bool ok;
//   iColor bg = glv->bgColor(ok); // note: we know it is ok!
//   if (!ok) bg.setRgb(0.8f, 0.8f, 0.8f);
//   ra->setBackgroundColor(SbColor(bg.redf(), bg.greenf(), bg.bluef()));
}

void iGridTableView_Panel::butRefresh_pressed() {
  GridTableView* glv = this->glv(); //cache
  if (updating || !glv) return;

  glv->InitDisplay();
  glv->UpdateDisplay();
}

void iGridTableView_Panel::butClear_pressed() {
  GridTableView* glv = this->glv(); //cache
  if (updating || !glv) return;

  glv->ClearData();
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
  color.name = "black";
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

void GraphAxisBase::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  color.UpdateAfterEdit();
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
static const float range_min_limit = 1.0e-6f;
static const float range_zero_range = 5.0e-5f; // half-range for zero-range values

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
  float min = range.min;
  float max = range.max;
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
  float chk_max = max + (tick_incr / 100.0f); // give a little tolerance..
  float val;
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
      String label = col_name; taMisc::SpaceLabel(label);
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

  int i;
  float val;
  String label;
  for (i = 0, val = start_tick; i < act_n_ticks; val += tick_incr, ++i) {
    fm.x = DataToPlot(val);
    to.x = DataToPlot(val);
    t3ax->addLine(fm, to);
    if(!ticks_only) {
      float lab_val = val / units;
      if (fabs(lab_val) < .001) {
	if (fabs(lab_val) < .0001)
	  lab_val = 0.0f;		// the 0 can be screwy
	else lab_val = .001f;
      }
      label = String(lab_val);
      if(use_str_labels) {
	DataCol* da = GetDAPtr();
	if(da) {
	  int rnum = (int)lab_val;// lab_val is row number!
	  if((float)rnum == lab_val && rnum >= 0 && rnum < da->rows()) // only int and in range
	    label = da->GetValAsString(rnum);
	  else
	    label = "";		// empty it!
	}
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
      if (fabs(lab_val) < .001) {
	if (fabs(lab_val) < .0001)
	  lab_val = 0.0f;		// the 0 can be screwy
	else lab_val = .001f;
      }
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
      String label = col_name; taMisc::SpaceLabel(label);
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

  int i;
  float val;
  String label;
  for (i = 0, val = start_tick; i < act_n_ticks; val += tick_incr, ++i) {
    fm.z = DataToPlot(val);
    to.z = DataToPlot(val);
    t3ax->addLine(fm, to);
    if(!ticks_only) {
      float lab_val = val / units;
      if (fabs(lab_val) < .001) {
	if (fabs(lab_val) < .0001)
	  lab_val = 0.0f;		// the 0 can be screwy
	else lab_val = .001f;
      }
      label = String(lab_val);
      if(use_str_labels) {
	DataCol* da = GetDAPtr();
	if(da) {
	  int rnum = (int)lab_val;// lab_val is row number!
	  if(rnum >= 0 && rnum < da->rows())
	    label = da->GetValAsString(rnum); 
	}
      }
      t3ax->addLabel(label.chars(),
		     iVec3f(fm.x - TICK_OFFSET, fm.y - y_lab_off, fm.z));
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
  eff_y_axis = NULL;
}

// void GraphPlotView::InitLinks() {
//   inherited::InitLinks();
// }

// void GraphPlotView::CutLinks() {
//   inherited::CutLinks();
// }

void GraphPlotView::UpdateOnFlag() {
  if(on) {
    if(!GetColPtr()) on = false; // not actually on!
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

void GraphAxisView::UpdateOnFlag() {
  if(on) {
    if(!row_num && !GetColPtr()) on = false; // not actually on!
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

GraphTableView* GraphTableView::New(DataTable* dt, T3DataViewFrame*& fr) {
  if (!dt) return NULL;
  if (fr) {
    //note: even if fr specified, need to insure it is right proj for object
    if (!dt->SameScope(fr, &TA_taProject)) {
      taMisc::Error("The viewer you specified is not in the same Project as the table.");
      return NULL;
    }
    // check if already viewing this obj there, warn user
    // no, because we want to be able to graph and grid in same view!
//     T3DataView* dv = fr->FindRootViewOfData(dt);
//     if (dv) {
//       if (taMisc::Choice("This table is already shown in that frame -- would you like"
//           " to show it in a new frame?", "&Ok", "&Cancel") != 0) return NULL;
//       fr = NULL; // make a new one
//     }
  } 
  if (!fr) {
    fr = T3DataViewer::GetBlankOrNewT3DataViewFrame(dt);
  }
  if (!fr) return NULL; // unexpected...
  
  GraphTableView* vw = new GraphTableView;
  fr->AddView(vw);
  vw->setDataTable(dt);
  // make sure we get it all setup!
  vw->BuildAll();
  fr->Render();
  fr->ViewAll();
  fr->GetCameraPosOrient();
  return vw;
}

void GraphTableView::Initialize() {
  view_rows = 10000;

  x_axis.axis = GraphAxisBase::X;
  z_axis.axis = GraphAxisBase::Z;
  plot_1.color.name = "black";
  plot_1.point_style = GraphPlotView::CIRCLE;
  plot_1.color.UpdateAfterEdit();	// needed to pick up color name
  plot_2.color.name = "red";
  plot_2.point_style = GraphPlotView::SQUARE;
  plot_2.color.UpdateAfterEdit();
  plot_3.color.name = "blue";
  plot_3.point_style = GraphPlotView::DIAMOND;
  plot_3.color.UpdateAfterEdit();
  plot_4.color.name = "green3";
  plot_4.point_style = GraphPlotView::TRIANGLE;
  plot_4.color.UpdateAfterEdit();
  plot_5.color.name = "purple";
  plot_5.point_style = GraphPlotView::PLUS;
  plot_5.color.UpdateAfterEdit();
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
  taBase::Own(err_1, this);
  taBase::Own(err_2, this);
  taBase::Own(err_3, this);
  taBase::Own(err_4, this);
  taBase::Own(err_5, this);
  taBase::Own(color_axis, this);
  taBase::Own(raster_axis, this);
  taBase::Own(colorscale, this);
  taBase::Own(last_sel_pt, this);
}

void GraphTableView::CutLinks() {
  x_axis.CutLinks();
  z_axis.CutLinks();
  plot_1.CutLinks();
  plot_2.CutLinks();
  plot_3.CutLinks();
  plot_4.CutLinks();
  plot_5.CutLinks();
  err_1.CutLinks();
  err_2.CutLinks();
  err_3.CutLinks();
  err_4.CutLinks();
  err_5.CutLinks();
  color_axis.CutLinks();
  raster_axis.CutLinks();
  colorscale.CutLinks();
  inherited::CutLinks();
}

void GraphTableView::UpdateAfterEdit_impl(){
  inherited::UpdateAfterEdit_impl();

  if(taMisc::is_loading) return;

  x_axis.axis_length = width;
  z_axis.axis_length = depth;

  x_axis.on = true;		// try to keep it on
  x_axis.UpdateOnFlag();
  z_axis.UpdateOnFlag();
  plot_1.on = true;		// try to keep it on
  plot_1.UpdateOnFlag();
  plot_2.UpdateOnFlag();
  plot_3.UpdateOnFlag();
  plot_4.UpdateOnFlag();
  plot_5.UpdateOnFlag();
  err_1.UpdateOnFlag();
  err_2.UpdateOnFlag();
  err_3.UpdateOnFlag();
  err_4.UpdateOnFlag();
  err_5.UpdateOnFlag();
  color_axis.UpdateOnFlag();
  raster_axis.UpdateOnFlag();

  DataTable* dt = dataTable();
  bool no_cols = true;
  if(dt && dt->data.size > 0)
    no_cols = false;

  // don't bother: not generlizable and not supported
//   if(plot_2.on && !plot_1.on) { // move to pl1
//     plot_1.col_name = plot_2.col_name;
//     plot_2.col_name = "";
//     plot_1.UpdateOnFlag();
//     plot_2.UpdateOnFlag();
//   }

  if(!plot_1.on) {
    if(!no_cols) {
      taMisc::Warning("GraphTableView -- plot_1 is not on -- perhaps no valid col_name found for plot_1 -- nothing will be plotted!");
    }
  }
  else if(plot_1.isString()) {
    taMisc::Warning("GraphTableView -- plot_1 is a string -- must be a numeric value -- nothing will be plotted");
    plot_1.on = false;
  }
  else {
    GraphColView* pl1 = plot_1.GetColPtr();
    if(pl1->dataCol()->is_matrix && plot_2.on) {
      taMisc::Warning("GraphTableView -- plot_1 is a matrix, so all other plots are being turned off (matrix can only be plotted by itself)");
      plot_2.on = false; plot_3.on = false; plot_4.on = false; plot_5.on = false;
    }

    // always share axis if string
    if(plot_2.on && plot_2.isString()) alt_y_2 = false;
    
    if(!alt_y_2 || !plot_2.on) {
      if(alt_y_3 && plot_3.on) {
	taMisc::Warning("GraphTableView -- plot_3 cannot use alt axis because plot_2 alt axis is not set -- only plot_2 can define the alt axis");
	alt_y_3 = false;
      }
      if(alt_y_4 && plot_4.on) {
	taMisc::Warning("GraphTableView -- plot_4 cannot use alt axis because plot_2 alt axis is not set -- only plot_2 can define the alt axis");
	alt_y_4 = false;
      }
      if(alt_y_5 && plot_5.on) {
	taMisc::Warning("GraphTableView -- plot_5 cannot use alt axis because plot_2 alt axis is not set -- only plot_2 can define the alt axis");
	alt_y_5 = false; // no can do
      }
    }
  }

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

String GraphTableView::GetLabel() const {
  return inherited::GetLabel() + " Graph";
}

String GraphTableView::GetName() const {
  return inherited::GetName() + " Graph";
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
  bool got_one = false;
  for(int i=0;i<fr->root_view.children.size;i++) {
    taDataView* dv = fr->root_view.children[i];
    if(dv->InheritsFrom(&TA_GraphTableView)) {
      GraphTableView* tgv = (GraphTableView*)dv;
      SoQtViewer* viewer = tgv->GetViewer();
      SoMouseButtonEvent* mouseevent = (SoMouseButtonEvent*)ecb->getEvent();
      SoRayPickAction rp( viewer->getViewportRegion());
      rp.setPoint(mouseevent->getPosition());
      rp.apply(viewer->getSceneManager()->getSceneGraph());

      SoPickedPoint* pp = rp.getPickedPoint(0);
      if(!pp) continue;
      SoNode* pobj = pp->getPath()->getNodeFromTail(2);
      if(!pobj) continue;
      //   cerr << "obj typ: " << pobj->getTypeId().getName() << endl;
      if(!pobj->isOfType(T3GraphLine::getClassTypeId())) {
	//     cerr << "not graph line!" << endl;
	continue;
      }
      GraphAxisBase* gab = (GraphAxisBase*)((T3GraphLine*)pobj)->dataView();
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
    }
  }
  if(got_one)
    ecb->setHandled();
}

void GraphTableView::Render_pre() {
  bool show_drag = manip_ctrl_on;
  SoQtViewer* vw = GetViewer();
  if(vw)
    show_drag = !vw->isViewing();

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
  x_axis.ComputeRange();
  if(matrix_mode == Z_INDEX) {
    DataCol* da_y = plot_1.GetDAPtr();
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
  plot_1.ComputeRange();
  plot_2.ComputeRange();
  plot_3.ComputeRange();
  plot_4.ComputeRange();
  plot_5.ComputeRange();

  if(plot_2.on && plot_2.GetDAPtr() && !plot_2.isString()) {
    if(!alt_y_2)
      plot_1.UpdateRange_impl(plot_2.data_range.min, plot_2.data_range.max);
  }
  if(plot_3.on && plot_3.GetDAPtr() && !plot_3.isString()) {
    if(alt_y_3)
      plot_2.UpdateRange_impl(plot_3.data_range.min, plot_3.data_range.max);
    else
      plot_1.UpdateRange_impl(plot_3.data_range.min, plot_3.data_range.max);
  }
  if(plot_4.on && plot_4.GetDAPtr() && !plot_4.isString()) {
    if(alt_y_4)
      plot_2.UpdateRange_impl(plot_4.data_range.min, plot_4.data_range.max);
    else
      plot_1.UpdateRange_impl(plot_4.data_range.min, plot_4.data_range.max);
  }
  if(plot_5.on && plot_5.GetDAPtr() && !plot_5.isString()) {
    if(alt_y_5)
      plot_2.UpdateRange_impl(plot_5.data_range.min, plot_5.data_range.max);
    else
      plot_1.UpdateRange_impl(plot_5.data_range.min, plot_5.data_range.max);
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
    if(da->HasUserData("PLOT_2")) {
      plot_2.col_name = cvs->name;  plot_2.on = true;
      plot_2.InitFromUserData();    plot_2.UpdateOnFlag();
    }
    if(da->HasUserData("PLOT_3")) {
      plot_3.col_name = cvs->name; plot_3.on = true;
      plot_3.InitFromUserData();   plot_3.UpdateOnFlag();
    }
    if(da->HasUserData("PLOT_4")) {
      plot_4.col_name = cvs->name; plot_4.on = true;
      plot_4.InitFromUserData();   plot_4.UpdateOnFlag();
    }
    if(da->HasUserData("PLOT_5")) {
      plot_5.col_name = cvs->name; plot_5.on = true;
      plot_5.InitFromUserData();   plot_5.UpdateOnFlag();
    }
    if(da->HasUserData("ERR_1")) {
      err_1.col_name = cvs->name;  err_1.on = true;
      err_1.InitFromUserData();    err_1.UpdateOnFlag();
    }
    if(da->HasUserData("ERR_2")) {
      err_2.col_name = cvs->name;  err_2.on = true;
      err_2.InitFromUserData();    err_2.UpdateOnFlag();
    }
    if(da->HasUserData("ERR_3")) {
      err_3.col_name = cvs->name;  err_3.on = true;
      err_3.InitFromUserData();    err_3.UpdateOnFlag();
    }
    if(da->HasUserData("ERR_4")) {
      err_4.col_name = cvs->name;  err_4.on = true;
      err_4.InitFromUserData();    err_4.UpdateOnFlag();
    }
    if(da->HasUserData("ERR_5")) {
      err_5.col_name = cvs->name;  err_5.on = true;
      err_5.InitFromUserData();    err_5.UpdateOnFlag();
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
  if(dt->HasUserData("PLOT_STYLE")) {
    int pstv = GetEnumVal(dt->GetUserDataAsString("PLOT_STYLE"));
    if(pstv >= 0)
      plot_style = (PlotStyle)pstv;
  }
  if(dt->HasUserData("GRAPH_TYPE")) {
    int pstv = GetEnumVal(dt->GetUserDataAsString("GRAPH_TYPE"));
    if(pstv >= 0)
      graph_type = (GraphType)pstv;
  }
  if(dt->HasUserData("POINT_SIZE")) {
    int pstv = GetEnumVal(dt->GetUserDataAsString("POINT_SIZE"));
    if(pstv >= 0)
      point_size = (PointSize)pstv;
  }
  if(dt->HasUserData("COLOR_MODE")) {
    int pstv = GetEnumVal(dt->GetUserDataAsString("COLOR_MODE"));
    if(pstv >= 0)
      color_mode = (ColorMode)pstv;
  }
  if(dt->HasUserData("MATRIX_MODE")) {
    int pstv = GetEnumVal(dt->GetUserDataAsString("MATRIX_MODE"));
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
  if(!plot_1.on || !x_axis.on) return;
  n_plots = 1;
  if(plot_2.on && plot_2.GetDAPtr())
    n_plots++;
  if(plot_3.on && plot_3.GetDAPtr())
    n_plots++;
  if(plot_4.on && plot_4.GetDAPtr())
    n_plots++;
  if(plot_5.on && plot_5.GetDAPtr())
    n_plots++;

  RenderAxes();

  if(plot_1.GetDAPtr()->is_matrix) {
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

  SoSeparator* xax = node_so->x_axis();
  xax->removeAllChildren();
  SoSeparator* zax = node_so->z_axis();
  zax->removeAllChildren();
  SoSeparator* yax = node_so->y_axes();
  yax->removeAllChildren();


  t3_x_axis = new T3Axis((T3Axis::Axis)x_axis.axis, &x_axis, axis_font_size);
  t3_x_axis_top = new T3Axis((T3Axis::Axis)x_axis.axis, &x_axis, axis_font_size);

  xax->addChild(t3_x_axis);

  SoTranslation* tr;
  // top
  tr = new SoTranslation();  xax->addChild(tr);
  tr->translation.setValue(0.0f, plot_1.axis_length, 0.0f);
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
    tr->translation.setValue(0.0f, -plot_1.axis_length, 0.0f);
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
    tr->translation.setValue(0.0f, plot_1.axis_length, 0.0f);
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
    t3_y_axis = new T3Axis((T3Axis::Axis)plot_1.axis, &plot_1, axis_font_size);
    plot_1.RenderAxis(t3_y_axis);
    yax->addChild(t3_y_axis);

    if(z_axis.on) {
      t3_y_axis_far = new T3Axis((T3Axis::Axis)plot_1.axis, &plot_1, axis_font_size);
      plot_1.RenderAxis(t3_y_axis_far, 0, true); // only ticks
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

    if(plot_2.on && alt_y_2) {
      t3_y_axis_rt = new T3Axis((T3Axis::Axis)plot_2.axis, &plot_2, axis_font_size, 1); // second Y = 1
      plot_2.RenderAxis(t3_y_axis_rt, 1); // indicate second axis!
      tr = new SoTranslation();  yax->addChild(tr);
      tr->translation.setValue(x_axis.axis_length, 0.0f, 0.0f); // put on right hand side!
      yax->addChild(t3_y_axis_rt);		  

      if(z_axis.on) {
	t3_y_axis_far_rt = new T3Axis((T3Axis::Axis)plot_2.axis, &plot_2, axis_font_size, 1);
	plot_2.RenderAxis(t3_y_axis_far_rt, 1, true); // only ticks
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
      t3_y_axis_rt = new T3Axis((T3Axis::Axis)plot_1.axis, &plot_1, axis_font_size);
      plot_1.RenderAxis(t3_y_axis_rt, 0, true); // ticks
      tr = new SoTranslation();   yax->addChild(tr);
      tr->translation.setValue(x_axis.axis_length, 0.0f, 0.0f);
      yax->addChild(t3_y_axis_rt);

      if(z_axis.on) {
	t3_y_axis_far_rt = new T3Axis((T3Axis::Axis)plot_1.axis, &plot_1, axis_font_size);
	plot_1.RenderAxis(t3_y_axis_far_rt, 0, true); // only ticks
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
}

void GraphTableView::RenderLegend() {
  T3GraphViewNode* node_so = this->node_so();
  if (!node_so) return;

  SoSeparator* leg = node_so->legend();
  leg->removeAllChildren();
  // move to top
  SoTranslation* tr;
  tr = new SoTranslation();  leg->addChild(tr);
  tr->translation.setValue(0.0f, plot_1.axis_length + 2.5f * axis_font_size, 0.0f);

  float over_amt = .33f * x_axis.axis_length;
  float dn_amt = -1.1f * axis_font_size;
  bool mv_dn = true;		// else over

  if(plot_1.on) {
    T3GraphLine* ln = new T3GraphLine(&plot_1, axis_font_size); leg->addChild(ln);
    RenderLegend_Ln(plot_1, ln);
    tr = new SoTranslation();  leg->addChild(tr);
    if(mv_dn)    tr->translation.setValue(0.0f, dn_amt, 0.0f);
    else	 tr->translation.setValue(over_amt, -dn_amt, 0.0f);
    mv_dn = !mv_dn;		// flip
  }
  if(plot_2.on) {
    T3GraphLine* ln = new T3GraphLine(&plot_2, axis_font_size); leg->addChild(ln);
    RenderLegend_Ln(plot_2, ln);
    tr = new SoTranslation();  leg->addChild(tr);
    if(mv_dn)    tr->translation.setValue(0.0f, dn_amt, 0.0f);
    else	 tr->translation.setValue(over_amt, -dn_amt, 0.0f);
    mv_dn = !mv_dn;		// flip
  }
  if(plot_3.on) {
    T3GraphLine* ln = new T3GraphLine(&plot_3, axis_font_size); leg->addChild(ln);
    RenderLegend_Ln(plot_3, ln);
    tr = new SoTranslation();  leg->addChild(tr);
    if(mv_dn)    tr->translation.setValue(0.0f, dn_amt, 0.0f);
    else	 tr->translation.setValue(over_amt, -dn_amt, 0.0f);
    mv_dn = !mv_dn;		// flip
  }
  if(plot_4.on) {
    T3GraphLine* ln = new T3GraphLine(&plot_4, axis_font_size); leg->addChild(ln);
    RenderLegend_Ln(plot_4, ln);
    tr = new SoTranslation();  leg->addChild(tr);
    if(mv_dn)    tr->translation.setValue(0.0f, dn_amt, 0.0f);
    else	 tr->translation.setValue(over_amt, -dn_amt, 0.0f);
    mv_dn = !mv_dn;		// flip
  }
  if(plot_5.on) {
    T3GraphLine* ln = new T3GraphLine(&plot_5, axis_font_size); leg->addChild(ln);
    RenderLegend_Ln(plot_5, ln);
    tr = new SoTranslation();  leg->addChild(tr);
    if(mv_dn)    tr->translation.setValue(0.0f, dn_amt, 0.0f);
    else	 tr->translation.setValue(over_amt, -dn_amt, 0.0f);
    mv_dn = !mv_dn;		// flip
  }
}

void GraphTableView::RenderGraph_XY() {
  T3GraphViewNode* node_so = this->node_so();
  if (!node_so) return;

  DataCol* da_1 = plot_1.GetDAPtr();
  if(!da_1) return;

  SoSeparator* graphs = node_so->graphs();
  graphs->removeAllChildren();

  SoSeparator* gr1 = new SoSeparator;
  graphs->addChild(gr1);

  float boxd = 0.0f;
  if(z_axis.on)
    boxd = depth;

  // each graph has a box and lines..
  SoLineBox3d* lbox = new SoLineBox3d(width, 1.0f, boxd);
  gr1->addChild(lbox);
  T3GraphLine* ln = new T3GraphLine(&plot_1, label_font_size);
  gr1->addChild(ln);

  PlotData_XY(plot_1, err_1, plot_1, ln); // this guy always has to be on

  if(plot_2.on) {
    T3GraphLine* ln = new T3GraphLine(&plot_2, label_font_size); gr1->addChild(ln);
    if(plot_2.isString()) {
      PlotData_String(plot_2, plot_1, ln);
    }
    else {
      if(alt_y_2) PlotData_XY(plot_2, err_2, plot_2, ln);
      else        PlotData_XY(plot_2, err_2, plot_1, ln);
    }
  }
  if(plot_3.on) {
    T3GraphLine* ln = new T3GraphLine(&plot_3, label_font_size); gr1->addChild(ln);
    if(plot_3.isString()) {
      if(alt_y_3) PlotData_String(plot_3, plot_2, ln);
      else 	    PlotData_String(plot_3, plot_1, ln);
    }
    else {
      if(alt_y_3) PlotData_XY(plot_3, err_3, plot_2, ln);
      else        PlotData_XY(plot_3, err_3, plot_1, ln);
    }
  }
  if(plot_4.on) {
    T3GraphLine* ln = new T3GraphLine(&plot_4, label_font_size); gr1->addChild(ln);
    if(plot_4.isString()) {
      if(alt_y_4) PlotData_String(plot_4, plot_2, ln);
      else 	    PlotData_String(plot_4, plot_1, ln);
    }
    else {
      if(alt_y_4) PlotData_XY(plot_4, err_4, plot_2, ln);
      else        PlotData_XY(plot_4, err_4, plot_1, ln);
    }
  }
  if(plot_5.on) {
    T3GraphLine* ln = new T3GraphLine(&plot_5, label_font_size); gr1->addChild(ln);
    if(plot_5.isString()) {
      if(alt_y_5) PlotData_String(plot_5, plot_2, ln);
      else 	    PlotData_String(plot_5, plot_1, ln);
    }
    else {
      if(alt_y_5) PlotData_XY(plot_5, err_5, plot_2, ln);
      else        PlotData_XY(plot_5, err_5, plot_1, ln);
    }
  }
}

void GraphTableView::RenderGraph_Bar() {
  T3GraphViewNode* node_so = this->node_so();
  if (!node_so) return;

  if(n_plots <= 0) return;

  DataCol* da_1 = plot_1.GetDAPtr();
  if(!da_1) return;

  SoSeparator* graphs = node_so->graphs();
  graphs->removeAllChildren();

  SoSeparator* gr1 = new SoSeparator;
  graphs->addChild(gr1);

  float boxd = 0.0f;
  if(z_axis.on)
    boxd = depth;

  // each graph has a box and lines..
  SoLineBox3d* lbox = new SoLineBox3d(width, 1.0f, boxd);
  gr1->addChild(lbox);
  T3GraphLine* ln = new T3GraphLine(&plot_1, label_font_size);
  gr1->addChild(ln);

  int n_str = 0;
  if(plot_2.on && plot_2.isString()) n_str++;
  if(plot_3.on && plot_3.isString()) n_str++;
  if(plot_4.on && plot_4.isString()) n_str++;
  if(plot_5.on && plot_5.isString()) n_str++;

  bar_width = (1.0f - bar_space) / ((float)(n_plots - n_str));
  float bar_off = - .5f * (1.0f - bar_space);

  PlotData_Bar(plot_1, err_1, plot_1, ln, bar_off); // this guy always has to be on
  bar_off += bar_width;

  if(plot_2.on) {
    T3GraphLine* ln = new T3GraphLine(&plot_2, label_font_size); gr1->addChild(ln);
    if(plot_2.isString()) {
      PlotData_String(plot_2, plot_1, ln);
    }
    else {
      if(alt_y_2) PlotData_Bar(plot_2, err_2, plot_2, ln, bar_off);
      else        PlotData_Bar(plot_2, err_2, plot_1, ln, bar_off);
      bar_off += bar_width;
    }
  }
  if(plot_3.on) {
    T3GraphLine* ln = new T3GraphLine(&plot_3, label_font_size); gr1->addChild(ln);
    if(plot_3.isString()) {
      if(alt_y_3) PlotData_String(plot_3, plot_2, ln);
      else 	    PlotData_String(plot_3, plot_1, ln);
    }
    else {
      if(alt_y_3) PlotData_Bar(plot_3, err_3, plot_2, ln, bar_off);
      else        PlotData_Bar(plot_3, err_3, plot_1, ln, bar_off);
      bar_off += bar_width;
    }
  }
  if(plot_4.on) {
    T3GraphLine* ln = new T3GraphLine(&plot_4, label_font_size); gr1->addChild(ln);
    if(plot_4.isString()) {
      if(alt_y_4) PlotData_String(plot_4, plot_2, ln);
      else 	    PlotData_String(plot_4, plot_1, ln);
    }
    else {
      if(alt_y_4) PlotData_Bar(plot_4, err_4, plot_2, ln, bar_off);
      else        PlotData_Bar(plot_4, err_4, plot_1, ln, bar_off);
      bar_off += bar_width;
    }
  }
  if(plot_5.on) {
    T3GraphLine* ln = new T3GraphLine(&plot_5, label_font_size); gr1->addChild(ln);
    if(plot_5.isString()) {
      if(alt_y_5) PlotData_String(plot_5, plot_2, ln);
      else 	    PlotData_String(plot_5, plot_1, ln);
    }
    else {
      if(alt_y_5) PlotData_Bar(plot_5, err_5, plot_2, ln, bar_off);
      else        PlotData_Bar(plot_5, err_5, plot_1, ln, bar_off);
      bar_off += bar_width;
    }
  }
}

void GraphTableView::RenderGraph_Matrix_Zi() {
  T3GraphViewNode* node_so = this->node_so();
  if (!node_so) return;

  DataCol* da_1 = plot_1.GetDAPtr();
  if(!da_1) return;

  SoSeparator* graphs = node_so->graphs();
  graphs->removeAllChildren();

  SoSeparator* gr1 = new SoSeparator;
  graphs->addChild(gr1);

  // each graph has a box and lines..
  SoLineBox3d* lbox = new SoLineBox3d(width, 1.0f, depth);
  gr1->addChild(lbox);

  for(int i=0;i<da_1->cell_size();i++) {
    T3GraphLine* ln = new T3GraphLine(&plot_1, label_font_size);
    gr1->addChild(ln);
    PlotData_XY(plot_1, err_1, plot_1, ln, i);
  }
}

void GraphTableView::RenderGraph_Matrix_Sep() {
  T3GraphViewNode* node_so = this->node_so();
  if (!node_so) return;

  DataCol* da_1 = plot_1.GetDAPtr();
  if(!da_1) return;

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
  if(mgeom.size <= 2) {
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
	SoLineBox3d* lbox = new SoLineBox3d(width * cl_x, cl_y, boxd * max_xy);
	gr->addChild(lbox);
	SoTransform* tx = new SoTransform();
	gr->addChild(tx);
	tx->scaleFactor.setValue(cl_x, cl_y, max_xy);
	T3GraphLine* ln = new T3GraphLine(&plot_1, label_font_size);
	gr->addChild(ln);
	if(mat_layout == taMisc::TOP_ZERO)
	  idx = mgeom.IndexFmDims(pos.x, geom_y-1-pos.y);
	else
	  idx = mgeom.IndexFmDims(pos.x, pos.y);
	PlotData_XY(plot_1, err_1, plot_1, ln, idx);
      }
    }
  }
  else if(mgeom.size == 3) {
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
	  SoLineBox3d* lbox = new SoLineBox3d(width * cl_x, cl_y, boxd * max_xy);
	  gr->addChild(lbox);
	  SoTransform* tx = new SoTransform();
	  gr->addChild(tx);
	  tx->scaleFactor.setValue(cl_x, cl_y, max_xy);
	  T3GraphLine* ln = new T3GraphLine(&plot_1, label_font_size);
	  gr->addChild(ln);
	  if(mat_layout == taMisc::TOP_ZERO)
	    idx = mgeom.IndexFmDims(pos.x, ymax-1-pos.y, zmax-1-z);
	  else
	    idx = mgeom.IndexFmDims(pos.x, pos.y, z);
	  PlotData_XY(plot_1, err_1, plot_1, ln, idx);
	}
      }
    }
  }
  else if(mgeom.size == 4) {
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
	    SoLineBox3d* lbox = new SoLineBox3d(width * cl_x, cl_y, boxd * max_xy);
	    gr->addChild(lbox);
	    SoTransform* tx = new SoTransform();
	    gr->addChild(tx);
	    tx->scaleFactor.setValue(cl_x, cl_y, max_xy);
	    T3GraphLine* ln = new T3GraphLine(&plot_1, label_font_size);
	    gr->addChild(ln);
	    if(mat_layout == taMisc::TOP_ZERO)
	      idx = mgeom.IndexFmDims(pos.x, ymax-1-pos.y, opos.x, yymax-1-opos.y);
	    else
	      idx = mgeom.IndexFmDims(pos.x, pos.y, opos.x, opos.y);
	    PlotData_XY(plot_1, err_1, plot_1, ln, idx);
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
      if(first || (new_trace && !negative_draw)) {	// just starting out
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
  GraphTableView* nv = (GraphTableView*)vnd->dataView();

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

  // todo: fix tool tips on all of these..

  lblGraphType = taiM->NewLabel("Graph", widg, font_spec);
  lblGraphType->setToolTip("How to display the graph");
  layTopCtrls->addWidget(lblGraphType);
  cmbGraphType = new taiComboBox(true, TA_GraphTableView.sub_types.FindName("GraphType"),
				 this, NULL, widg, taiData::flgAutoApply);
  layTopCtrls->addWidget(cmbGraphType->GetRep());
  //  layTopCtrls->addSpacing(taiM->hsep_c);

  lblPlotStyle = taiM->NewLabel("Style", widg, font_spec);
  lblPlotStyle->setToolTip("How to plot the lines");
  layTopCtrls->addWidget(lblPlotStyle);
  cmbPlotStyle = new taiComboBox(true, TA_GraphTableView.sub_types.FindName("PlotStyle"),
				 this, NULL, widg, taiData::flgAutoApply);
  layTopCtrls->addWidget(cmbPlotStyle->GetRep());
  //  layTopCtrls->addSpacing(taiM->hsep_c);

  chkNegDraw =  new QCheckBox("Neg\nDraw", widg); chkNegDraw->setObjectName("chkNegDraw");
  chkNegDraw->setToolTip("Whether to draw a line when going in a negative direction (to the left), which may indicate a wrap-around to a new iteration of data");
  connect(chkNegDraw, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
  layTopCtrls->addWidget(chkNegDraw);

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
  fldRows = new taiIncrField(&TA_int, this, NULL, widg);
  layVals->addWidget(fldRows->GetRep());
//   layVals->addSpacing(taiM->hsep_c);

  lblLineWidth = taiM->NewLabel("Line\nWidth", widg, font_spec);
  lblLineWidth->setToolTip("Width to draw lines with.");
  layVals->addWidget(lblLineWidth);
  fldLineWidth = new taiField(&TA_float, this, NULL, widg);
  layVals->addWidget(fldLineWidth->GetRep());
  //  layVals->addSpacing(taiM->hsep_c);

  lblPointSpacing = taiM->NewLabel("Pt\nSpc", widg, font_spec);
  lblPointSpacing->setToolTip("Spacing of points drawn relative to underlying data points.");
  layVals->addWidget(lblPointSpacing);
  fldPointSpacing = new taiField(&TA_float, this, NULL, widg);
  layVals->addWidget(fldPointSpacing->GetRep());
  //  layVals->addSpacing(taiM->hsep_c);

  lblLabelSpacing = taiM->NewLabel("Lbl\nSpc", widg, font_spec);
  lblLabelSpacing->setToolTip("Spacing of text labels of data point values. -1 means no text labels.");
  layVals->addWidget(lblLabelSpacing);
  fldLabelSpacing = new taiField(&TA_float, this, NULL, widg);
  layVals->addWidget(fldLabelSpacing->GetRep());
  //  layVals->addSpacing(taiM->hsep_c);

  lblWidth = taiM->NewLabel("Width", widg, font_spec);
  lblWidth->setToolTip("Width of graph display, in normalized units (default is 1.0 = same as height).");
  layVals->addWidget(lblWidth);
  fldWidth = new taiField(&TA_float, this, NULL, widg);
  layVals->addWidget(fldWidth->GetRep());
  //  layVals->addSpacing(taiM->hsep_c);

  layVals->addStretch();

  ////////////////////////////////////////////////////////////////////////////
  // 	Axes

  ////////////////////////////////////////////////////////////////////
  // X AXis
  layXAxis = new QHBoxLayout; layWidg->addLayout(layXAxis);

  int list_flags = taiData::flgNullOk | taiData::flgAutoApply;

  lblXAxis = taiM->NewLabel("X:", widg, font_spec);
  lblXAxis->setToolTip("Column of data to plot for the X Axis");
  layXAxis->addWidget(lblXAxis);
  lelXAxis = new taiListElsButton(&TA_T3DataView_List, this, NULL, widg, list_flags);
  layXAxis->addWidget(lelXAxis->GetRep());
  //  layVals->addSpacing(taiM->hsep_c);

  rncXAxis = new QCheckBox("Row\nNum", widg); rncXAxis->setObjectName("rncXAxis");
  rncXAxis->setToolTip("Use row number instead of column value for axis value");
  connect(rncXAxis, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
  layXAxis->addWidget(rncXAxis);

  pdtXAxis = taiPolyData::New(true, &TA_FixedMinMax, this, NULL, widg);
  layXAxis->addWidget(pdtXAxis->GetRep());

  layXAxis->addStretch();

  ////////////////////////////////////////////////////////////////////
  // Z AXis
  layZAxis = new QHBoxLayout; layWidg->addLayout(layZAxis);

  lblZAxis = taiM->NewLabel("Z:", widg, font_spec);
  lblZAxis->setToolTip("Column of data to plot for the Z Axis");
  layZAxis->addWidget(lblZAxis);
  lelZAxis = new taiListElsButton(&TA_T3DataView_List, this, NULL, widg, list_flags);
  layZAxis->addWidget(lelZAxis->GetRep());
  //  layVals->addSpacing(taiM->hsep_c);

  oncZAxis = new iCheckBox("On", widg);
  oncZAxis->setToolTip("Display a Z axis?");
  connect(oncZAxis, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
  layZAxis->addWidget(oncZAxis);

  rncZAxis = new QCheckBox("Row\nNum", widg); rncZAxis->setObjectName("rncZAxis");
  rncZAxis->setToolTip("Use row number instead of column value for axis value");
  connect(rncZAxis, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
  layZAxis->addWidget(rncZAxis);

  pdtZAxis = taiPolyData::New(true, &TA_FixedMinMax, this, NULL, widg);
  layZAxis->addWidget(pdtZAxis->GetRep());

  layZAxis->addStretch();

  ////////////////////////////////////////////////////////////////////
  // 1 AXis
  lay1Axis = new QHBoxLayout; layWidg->addLayout(lay1Axis);

  lbl1Axis = taiM->NewLabel("Y1:", widg, font_spec);
  lbl1Axis->setToolTip("First column of data to plot");
  lay1Axis->addWidget(lbl1Axis);
  lel1Axis = new taiListElsButton(&TA_T3DataView_List, this, NULL, widg, list_flags);
  lay1Axis->addWidget(lel1Axis->GetRep());
  //  layVals->addSpacing(taiM->hsep_c);

  pdt1Axis = taiPolyData::New(true, &TA_FixedMinMax, this, NULL, widg);
  lay1Axis->addWidget(pdt1Axis->GetRep());

  lay1Axis->addStretch();

  ////////////////////////////////////////////////////////////////////
  // 2 AXis
  lay2Axis = new QHBoxLayout; layWidg->addLayout(lay2Axis);

  lbl2Axis = taiM->NewLabel("Y2:", widg, font_spec);
  lbl2Axis->setToolTip("Second column of data to plot (optional)");
  lay2Axis->addWidget(lbl2Axis);
  lel2Axis = new taiListElsButton(&TA_T3DataView_List, this, NULL, widg, list_flags);
  lay2Axis->addWidget(lel2Axis->GetRep());
  //  layVals->addSpacing(taiM->hsep_c);

  onc2Axis = new iCheckBox("On", widg);
  onc2Axis->setToolTip("Display a second column's worth of data?");
  connect(onc2Axis, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
  lay2Axis->addWidget(onc2Axis);

  chk2AltY =  new QCheckBox("Alt\nY", widg); chk2AltY->setObjectName("chk2AltY");
  chk2AltY->setToolTip("Whether to setup an alternate Y axis for this second column of data (otherwise it shares with the first plot's Y axis)");
  connect(chk2AltY, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
  //  layPlots->addWidget(chk2AltY);
  lay2Axis->addWidget(chk2AltY);

  pdt2Axis = taiPolyData::New(true, &TA_FixedMinMax, this, NULL, widg);
  lay2Axis->addWidget(pdt2Axis->GetRep());

  lay2Axis->addStretch();

  ////////////////////////////////////////////////////////////////////
  // 3 AXis
  lay3Axis = new QHBoxLayout; layWidg->addLayout(lay3Axis);

  lbl3Axis = taiM->NewLabel("Y3:", widg, font_spec);
  lbl3Axis->setToolTip("Second column of data to plot (optional)");
  lay3Axis->addWidget(lbl3Axis);
  lel3Axis = new taiListElsButton(&TA_T3DataView_List, this, NULL, widg, list_flags);
  lay3Axis->addWidget(lel3Axis->GetRep());
  //  layVals->addSpacing(taiM->hsep_c);

  onc3Axis = new iCheckBox("On", widg);
  onc3Axis->setToolTip("Display a second column's worth of data?");
  connect(onc3Axis, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
  lay3Axis->addWidget(onc3Axis);

  chk3AltY =  new QCheckBox("Alt\nY", widg); chk3AltY->setObjectName("chk3AltY");
  chk3AltY->setToolTip("Use the alternate (plot 2) or standard (plot 1) axis -- only plot 2 can create an alternate axis");
  connect(chk3AltY, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
  //  layPlots->addWidget(chk3AltY);
  lay3Axis->addWidget(chk3AltY);

  pdt3Axis = taiPolyData::New(true, &TA_FixedMinMax, this, NULL, widg);
  lay3Axis->addWidget(pdt3Axis->GetRep());

  lay3Axis->addStretch();

  ////////////////////////////////////////////////////////////////////
  // 4 AXis
  lay4Axis = new QHBoxLayout; layWidg->addLayout(lay4Axis);

  lbl4Axis = taiM->NewLabel("Y4:", widg, font_spec);
  lbl4Axis->setToolTip("Second column of data to plot (optional)");
  lay4Axis->addWidget(lbl4Axis);
  lel4Axis = new taiListElsButton(&TA_T3DataView_List, this, NULL, widg, list_flags);
  lay4Axis->addWidget(lel4Axis->GetRep());
  //  layVals->addSpacing(taiM->hsep_c);

  onc4Axis = new iCheckBox("On", widg);
  onc4Axis->setToolTip("Display a second column's worth of data?");
  connect(onc4Axis, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
  lay4Axis->addWidget(onc4Axis);

  chk4AltY =  new QCheckBox("Alt\nY", widg);
  chk4AltY->setToolTip("Use the alternate (plot 2) or standard (plot 1) axis -- only plot 2 can create an alternate axis");
  connect(chk4AltY, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
  //  layPlots->addWidget(chk4AltY);
  lay4Axis->addWidget(chk4AltY);

  pdt4Axis = taiPolyData::New(true, &TA_FixedMinMax, this, NULL, widg);
  lay4Axis->addWidget(pdt4Axis->GetRep());

  lay4Axis->addStretch();

  ////////////////////////////////////////////////////////////////////
  // 5 AXis
  lay5Axis = new QHBoxLayout; layWidg->addLayout(lay5Axis);

  lbl5Axis = taiM->NewLabel("Y5:", widg, font_spec);
  lbl5Axis->setToolTip("Second column of data to plot (optional)");
  lay5Axis->addWidget(lbl5Axis);
  lel5Axis = new taiListElsButton(&TA_T3DataView_List, this, NULL, widg, list_flags);
  lay5Axis->addWidget(lel5Axis->GetRep());
  //  layVals->addSpacing(taiM->hsep_c);

  onc5Axis = new iCheckBox("On", widg);
  onc5Axis->setToolTip("Display a second column's worth of data?");
  connect(onc5Axis, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
  lay5Axis->addWidget(onc5Axis);

  chk5AltY =  new QCheckBox("Alt\nY", widg); chk5AltY->setObjectName("chk5AltY");
  chk5AltY->setToolTip("Use the alternate (plot 2) or standard (plot 1) axis -- only plot 2 can create an alternate axis");
  connect(chk5AltY, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
  //  layPlots->addWidget(chk5AltY);
  lay5Axis->addWidget(chk5AltY);

  pdt5Axis = taiPolyData::New(true, &TA_FixedMinMax, this, NULL, widg);
  lay5Axis->addWidget(pdt5Axis->GetRep());

  lay5Axis->addStretch();

  ////////////////////////////////////////////////////////////////////
  // Error bars
  layErr1 = new QHBoxLayout; layWidg->addLayout(layErr1);
  
  // Err1
  lbl1Err = taiM->NewLabel("1 Err:", widg, font_spec);
  lbl1Err->setToolTip("Column of for the 1st column's error bar data");
  layErr1->addWidget(lbl1Err);
  lel1Err = new taiListElsButton(&TA_T3DataView_List, this, NULL, widg, list_flags);
  layErr1->addWidget(lel1Err->GetRep());
  onc1Err = new iCheckBox("On", widg);
  onc1Err->setToolTip("Display error bars for 1st column's data?");
  connect(onc1Err, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
  layErr1->addWidget(onc1Err);

  // Err2
  lbl2Err = taiM->NewLabel("2 Err:", widg, font_spec);
  lbl2Err->setToolTip("Column of for the 2nd column's error bar data");
  layErr1->addWidget(lbl2Err);
  lel2Err = new taiListElsButton(&TA_T3DataView_List, this, NULL, widg, list_flags);
  layErr1->addWidget(lel2Err->GetRep());
  onc2Err = new iCheckBox("On", widg);
  onc2Err->setToolTip("Display error bars for 2nd column's data?");
  connect(onc2Err, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
  layErr1->addWidget(onc2Err);

  // Err3
  lbl3Err = taiM->NewLabel("3 Err:", widg, font_spec);
  lbl3Err->setToolTip("Column of for the 3nd column's error bar data");
  layErr1->addWidget(lbl3Err);
  lel3Err = new taiListElsButton(&TA_T3DataView_List, this, NULL, widg, list_flags);
  layErr1->addWidget(lel3Err->GetRep());
  onc3Err = new iCheckBox("On", widg);
  onc3Err->setToolTip("Display error bars for 3nd column's data?");
  connect(onc3Err, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
  layErr1->addWidget(onc3Err);

  layErr1->addStretch();

  layErr2 = new QHBoxLayout; layWidg->addLayout(layErr2);
  // Err4
  lbl4Err = taiM->NewLabel("4 Err:", widg, font_spec);
  lbl4Err->setToolTip("Column of for the 4nd column's error bar data");
  layErr2->addWidget(lbl4Err);
  lel4Err = new taiListElsButton(&TA_T3DataView_List, this, NULL, widg, list_flags);
  layErr2->addWidget(lel4Err->GetRep());
  onc4Err = new iCheckBox("On", widg);
  onc4Err->setToolTip("Display error bars for 4nd column's data?");
  connect(onc4Err, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
  layErr2->addWidget(onc4Err);

  // Err5
  lbl5Err = taiM->NewLabel("5 Err:", widg, font_spec);
  lbl5Err->setToolTip("Column of for the 5nd column's error bar data");
  layErr2->addWidget(lbl5Err);
  lel5Err = new taiListElsButton(&TA_T3DataView_List, this, NULL, widg, list_flags);
  layErr2->addWidget(lel5Err->GetRep());
  onc5Err = new iCheckBox("On", widg);
  onc5Err->setToolTip("Display error bars for 5nd column's data?");
  connect(onc5Err, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
  layErr2->addWidget(onc5Err);

  // Err Spacing
  lblErrSpacing = taiM->NewLabel("Err\nSpc", widg, font_spec);
  lblErrSpacing->setToolTip("Spacing of error bars relative to data points.");
  layErr2->addWidget(lblErrSpacing);
  fldErrSpacing = new taiField(&TA_float, this, NULL, widg);
  layErr2->addWidget(fldErrSpacing->GetRep());
  //  layVals->addSpacing(taiM->hsep_c);

  layErr2->addStretch();

  ////////////////////////////////////////////////////////////////////////////
  // 	Colors

  layCAxis = new QHBoxLayout; layWidg->addLayout(layCAxis);
  lblColorMode = taiM->NewLabel("Color\nMode", widg, font_spec);
  lblColorMode->setToolTip("How to determine line color:\n VALUE_COLOR makes the color change as a function of the\n Y axis value, according to the colorscale pallete\n FIXED_COLOR uses fixed colors associated with each Y axis line\n (click on line/legend/axis and do View Properties in context menu to change)\n COLOR_AXIS uses a separate column of data to determine color value");
  layCAxis->addWidget(lblColorMode);
  cmbColorMode = new taiComboBox(true, TA_GraphTableView.sub_types.FindName("ColorMode"),
				 this, NULL, widg, taiData::flgAutoApply);
  layCAxis->addWidget(cmbColorMode->GetRep());
  //  layColorScale->addSpacing(taiM->hsep_c);

  lblCAxis = taiM->NewLabel("Color\nAxis:", widg, font_spec);
  lblCAxis->setToolTip("Column of data for COLOR_AXIS color mode");
  layCAxis->addWidget(lblCAxis);
  lelCAxis = new taiListElsButton(&TA_T3DataView_List, this, NULL, widg, list_flags);
  layCAxis->addWidget(lelCAxis->GetRep());
  //  layVals->addSpacing(taiM->hsep_c);

  lblThresh = taiM->NewLabel("Thresh", widg, font_spec);
  lblThresh->setToolTip("Threshold for THRESH_LINE and THRESH_POINT styles -- only draw a line when value is over this threshold.");
  layCAxis->addWidget(lblThresh);
  fldThresh = new taiField(&TA_float, this, NULL, widg);
  layCAxis->addWidget(fldThresh->GetRep());
  //  layVals->addSpacing(taiM->hsep_c);

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
  lelRAxis = new taiListElsButton(&TA_T3DataView_List, this, NULL, widg, list_flags);
  layRAxis->addWidget(lelRAxis->GetRep());
  //  layVals->addSpacing(taiM->hsep_c);

  pdtRAxis = taiPolyData::New(true, &TA_FixedMinMax, this, NULL, widg);
  layRAxis->addWidget(pdtRAxis->GetRep());

  layRAxis->addStretch();
  
  layWidg->addStretch();

  ////////////////////////////////////////////////////////////////////////////
  // 	viewspace guy

  // no room!
//   layViewspace = new QHBoxLayout(layWidg);
//   Constr_T3ViewspaceWidget(widg);
//   layViewspace->addWidget(t3vs);

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
//   DataTable* dt = glv->dataTable();

//  viewAll();

  chkDisplay->setChecked(glv->display_on);
  cmbGraphType->GetImage(glv->graph_type);
  cmbPlotStyle->GetImage(glv->plot_style);
  fldRows->GetImage((String)glv->view_rows);

  fldLineWidth->GetImage((String)glv->line_width);
  fldPointSpacing->GetImage((String)glv->point_spacing);
  fldLabelSpacing->GetImage((String)glv->label_spacing);
  chkNegDraw->setChecked(glv->negative_draw);
  fldWidth->GetImage((String)glv->width);

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
  lel1Axis->GetImage(&(glv->children), glv->plot_1.GetColPtr());
  pdt1Axis->GetImage_(&(glv->plot_1.fixed_range));

  lel2Axis->GetImage(&(glv->children), glv->plot_2.GetColPtr());
  onc2Axis->setReadOnly(glv->plot_2.GetColPtr() == NULL);
  onc2Axis->setChecked(glv->plot_2.on);
  pdt2Axis->GetImage_(&(glv->plot_2.fixed_range));
  lel2Axis->SetFlag(taiData::flgReadOnly, !glv->plot_2.on);
  pdt2Axis->SetFlag(taiData::flgReadOnly, !glv->plot_2.on);
  chk2AltY->setChecked(glv->alt_y_2);

  lel3Axis->GetImage(&(glv->children), glv->plot_3.GetColPtr());
  onc3Axis->setReadOnly(glv->plot_3.GetColPtr() == NULL);
  onc3Axis->setChecked(glv->plot_3.on);
  pdt3Axis->GetImage_(&(glv->plot_3.fixed_range));
  lel3Axis->SetFlag(taiData::flgReadOnly, !glv->plot_3.on);
  pdt3Axis->SetFlag(taiData::flgReadOnly, !glv->plot_3.on);
  chk3AltY->setChecked(glv->alt_y_3);

  lel4Axis->GetImage(&(glv->children), glv->plot_4.GetColPtr());
  onc4Axis->setReadOnly(glv->plot_4.GetColPtr() == NULL);
  onc4Axis->setChecked(glv->plot_4.on);
  pdt4Axis->GetImage_(&(glv->plot_4.fixed_range));
  lel4Axis->SetFlag(taiData::flgReadOnly, !glv->plot_4.on);
  pdt4Axis->SetFlag(taiData::flgReadOnly, !glv->plot_4.on);
  chk4AltY->setChecked(glv->alt_y_4);

  lel5Axis->GetImage(&(glv->children), glv->plot_5.GetColPtr());
  onc5Axis->setReadOnly(glv->plot_5.GetColPtr() == NULL);
  onc5Axis->setChecked(glv->plot_5.on);
  pdt5Axis->GetImage_(&(glv->plot_5.fixed_range));
  lel5Axis->SetFlag(taiData::flgReadOnly, !glv->plot_5.on);
  pdt5Axis->SetFlag(taiData::flgReadOnly, !glv->plot_5.on);
  chk5AltY->setChecked(glv->alt_y_5);

  lel1Err->GetImage(&(glv->children), glv->err_1.GetColPtr());
  onc1Err->setReadOnly(glv->err_1.GetColPtr() == NULL);
  onc1Err->setChecked(glv->err_1.on);
  lel2Err->GetImage(&(glv->children), glv->err_2.GetColPtr());
  onc2Err->setReadOnly(glv->err_2.GetColPtr() == NULL);
  onc2Err->setChecked(glv->err_2.on);
  lel3Err->GetImage(&(glv->children), glv->err_3.GetColPtr());
  onc3Err->setReadOnly(glv->err_3.GetColPtr() == NULL);
  onc3Err->setChecked(glv->err_3.on);
  lel4Err->GetImage(&(glv->children), glv->err_4.GetColPtr());
  onc4Err->setReadOnly(glv->err_4.GetColPtr() == NULL);
  onc4Err->setChecked(glv->err_4.on);
  lel5Err->GetImage(&(glv->children), glv->err_5.GetColPtr());
  onc5Err->setReadOnly(glv->err_5.GetColPtr() == NULL);
  onc5Err->setChecked(glv->err_5.on);
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
  int i = 0;
  cmbGraphType->GetEnumValue(i); glv->graph_type = (GraphTableView::GraphType)i;
  cmbPlotStyle->GetEnumValue(i); glv->plot_style = (GraphTableView::PlotStyle)i;
  glv->view_rows = (int)fldRows->GetValue();
  glv->line_width = (float)fldLineWidth->GetValue();
  glv->point_spacing = (int)fldPointSpacing->GetValue();
  glv->label_spacing = (int)fldLabelSpacing->GetValue();
  glv->negative_draw = chkNegDraw->isChecked();
  glv->width = (float)fldWidth->GetValue();
  
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

  pdt1Axis->GetValue_(&(glv->plot_1.fixed_range)); // this can change, so update
  glv->plot_1.SetColPtr((GraphColView*)lel1Axis->GetValue());
  
  tcol = (GraphColView*)lel2Axis->GetValue();
  if (tcol && !glv->plot_2.GetColPtr())
    onc2Axis->setChecked(true);
  glv->plot_2.on = onc2Axis->isChecked();
  pdt2Axis->GetValue_(&(glv->plot_2.fixed_range)); // this can change, so update
  glv->alt_y_2 = chk2AltY->isChecked();
  glv->plot_2.SetColPtr(tcol);
  
  tcol = (GraphColView*)lel3Axis->GetValue();
  if (tcol && !glv->plot_3.GetColPtr())
    onc3Axis->setChecked(true);
  glv->plot_3.on = onc3Axis->isChecked();
  pdt3Axis->GetValue_(&(glv->plot_3.fixed_range)); // this can change, so update
  glv->alt_y_3 = chk3AltY->isChecked();
  glv->plot_3.SetColPtr(tcol);
  
  tcol = (GraphColView*)lel4Axis->GetValue();
  if (tcol && !glv->plot_4.GetColPtr())
    onc4Axis->setChecked(true);
  glv->plot_4.on = onc4Axis->isChecked();
  pdt4Axis->GetValue_(&(glv->plot_4.fixed_range)); // this can change, so update
  glv->alt_y_4 = chk4AltY->isChecked();
  glv->plot_4.SetColPtr(tcol);
  
  tcol = (GraphColView*)lel5Axis->GetValue();
  if (tcol && !glv->plot_5.GetColPtr())
    onc5Axis->setChecked(true);
  glv->plot_5.on = onc5Axis->isChecked();
  pdt5Axis->GetValue_(&(glv->plot_5.fixed_range)); // this can change, so update
  glv->alt_y_5 = chk5AltY->isChecked();
  glv->plot_5.SetColPtr(tcol);
  
  tcol = (GraphColView*)lel1Err->GetValue();
  if (tcol && !glv->err_1.GetColPtr())
    onc1Err->setChecked(true);
  glv->err_1.on = onc1Err->isChecked();
  glv->err_1.SetColPtr(tcol);
  
  tcol = (GraphColView*)lel2Err->GetValue();
  if (tcol && !glv->err_2.GetColPtr())
    onc2Err->setChecked(true);
  glv->err_2.on = onc2Err->isChecked();
  glv->err_2.SetColPtr(tcol);
  
  tcol = (GraphColView*)lel3Err->GetValue();
  if (tcol && !glv->err_3.GetColPtr())
    onc3Err->setChecked(true);
  glv->err_3.on = onc3Err->isChecked();
  glv->err_3.SetColPtr(tcol);
  
  tcol = (GraphColView*)lel4Err->GetValue();
  if (tcol && !glv->err_4.GetColPtr())
    onc4Err->setChecked(true);
  glv->err_4.on = onc4Err->isChecked();
  glv->err_4.SetColPtr(tcol);
  
  tcol = (GraphColView*)lel5Err->GetValue();
  if (tcol && !glv->err_5.GetColPtr())
    onc5Err->setChecked(true);
  glv->err_5.on = onc5Err->isChecked();
  glv->err_5.SetColPtr(tcol);
  
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
  if (!tab) return;
  taiTabularDataMimeFactory* fact = taiTabularDataMimeFactory::instance();
  CellRange sel(selectionModel()->selectedIndexes());
  if (ea & taiClipData::EA_SRC_OPS) {
    fact->Table_EditActionS(tab, sel, ea);
  } else {// dest op
    taiMimeSource* ms = taiMimeSource::NewFromClipboard();
    fact->Table_EditActionD(tab, sel, ms, ea);
    delete ms;
  }
}

void iDataTableView::GetEditActionsEnabled(int& ea) {
  int allowed = 0;
  int forbidden = 0;
  DataTable* tab = this->dataTable(); // may not exist
  if (tab) {
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
  if (op_code & OP_ROW) {
    // must have >=1 row selected to make sense
    if ((op_code & (OP_APPEND | OP_INSERT | OP_DELETE))) {
      if (sel.height() < 1) return;
      if (op_code & OP_APPEND) {
        tab->AddRows(sel.height());
      } else if (op_code & OP_INSERT) {
        tab->InsertRows(sel.row_fr, sel.height());
      } else if (op_code & OP_DELETE) {
        if (taMisc::Choice("Are you sure you want to delete the selected rows? (this operation cannot be undone!)", "Yes", "Cancel") != 0) return;
        tab->RemoveRows(sel.row_fr, sel.height());
      }
    }
  } else if (op_code & OP_COL) { 
    // must have >=1 col selected to make sense
    if ((op_code & (OP_APPEND | OP_INSERT | OP_DELETE))) {
      if (sel.width() < 1) return;
/*note: not supporting col ops here
      if (op_code & OP_APPEND) {
      } else 
      if (op_code & OP_INSERT) {
      } else */
      if (op_code & OP_DELETE) {
        if (taMisc::Choice("Are you sure you want to delete the selected columns? (this operation cannot be undone!)", "Yes", "Cancel") != 0) return;
        tab->StructUpdate(true);
        for (int col = sel.col_to; col >= sel.col_fr; --col) {
          tab->RemoveCol(col);
        }
        tab->StructUpdate(false);
      }
    }
  }
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
  m_model = NULL;
  m_cell_par = NULL;
  layOuter = new QVBoxLayout(this);
  layOuter->setMargin(0); layOuter->setSpacing(0);
  splMain = new QSplitter(this);
  splMain->setOrientation(Qt::Vertical);
  layOuter->addWidget(splMain);
  tvTable = new iDataTableView();
  tvCell = new iMatrixEditor();
  splMain->addWidget(tvTable);
  splMain->addWidget(tvCell);
  
  connect(tvTable, SIGNAL(sig_currentChanged(const QModelIndex&)),
    this, SLOT(tvTable_currentChanged(const QModelIndex&)));
  connect(tvTable, SIGNAL(sig_dataChanged(const QModelIndex&, const QModelIndex&)),
    this, SLOT(tvTable_dataChanged(const QModelIndex&, const QModelIndex&)) );

}

iDataTableEditor::~iDataTableEditor() {
  if (m_model) { 
    delete m_model;
    m_model = NULL;
  }
  if (m_cell_par) {
    setCellMat(NULL, QModelIndex());
  }
}

void iDataTableEditor::ConfigView() {
  DataTable* dt = this->dt(); // cache
  if (!dt) return;
  bool show_cell = false; // false if not in mode, or no mat cols
  if (true) { // in cell mode
    for (int i = 0; i < dt->data.size; ++i) {
      DataCol* dc = dt->data.FastEl(i);
      if (dc->is_matrix) {
        show_cell = true;
        break;
      }
    }
  }
  tvCell->setVisible(show_cell);
  
}

void iDataTableEditor::DataLinkDestroying(taDataLink* dl) {
  // note: following should always be true
  if (m_cell_par && (m_cell_par == dl->taData())) {
    // ok, probably the col is deleting, so unlink ourself now!
    setCellMat(NULL, QModelIndex());
    // WARNING: m_cell_par will now be NULL
  }
}

void iDataTableEditor::Refresh() {
  if (m_model)
    m_model->refreshViews();
}

void iDataTableEditor::setDataTable(DataTable* dt_) {
  if (dt_ == m_dt.ptr()) return;
  if (m_model) { // shouldn't really happen, we only assign one table
    delete m_model;
    m_model = NULL;
  }
  if (dt_) {
//nn    tv->setItemDelegate(new DataTableDelegate(dt_));
    m_model = new DataTableModel(dt_, this);
    tvTable->setModel(m_model);
    connect(m_model, SIGNAL(layoutChanged()),
      this, SLOT(tvTable_layoutChanged()) );
  }
  m_dt = dt_;
  ConfigView();
}

void iDataTableEditor::setCellMat(taMatrix* mat, const QModelIndex& index) {
  tvCell->setMatrix(mat);
  m_cell = mat; 
  m_cell_index = index;
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
}

void iDataTableEditor::tvTable_layoutChanged()
{
  ConfigView();
}

void iDataTableEditor::tvTable_currentChanged(const QModelIndex& index) {
  DataTable* dt_ = dt(); // cache
  int colidx = index.column();
  DataCol* col = dt_->GetColData(colidx, true); // quiet
  if (col && col->is_matrix) {
    taMatrix* tcell = dt_->GetValAsMatrix(index.column(), index.row());
    if (tcell) {
      setCellMat(tcell, index);
      return;
    }
  }
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
  if (dte)
    dte->Refresh();
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
}

void iDataTablePanel::tv_hasFocus(iTableView* sender) {
  iMainWindowViewer* vw = viewerWindow();
  if (vw)
    vw->SetClipboardHandler(sender,
    SLOT(GetEditActionsEnabled(int&)),
    SLOT(EditAction(int)),
    NULL,
    SIGNAL(UpdateUi()) );
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
    CellRange sel2(sel);
    // if sel is a single cell, adjust to max
    if (sel2.single()) {
      sel2.col_to = mat->geom.SafeEl(0) - 1;
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


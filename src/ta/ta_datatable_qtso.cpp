// Carnegie Mellon University, Princeton University.
// Copyright, 1995-2005, Regents of the University of Colorado,
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

#include <Inventor/Qt/viewers/SoQtExaminerViewer.h>

#include <limits.h>
#include <float.h>
//nn? #include <unistd.h>

#define DIST(x,y) sqrt((double) ((x * x) + (y*y)))

void DataTable::ShowInViewer(T3DataViewFrame* fr) {
  GridTableView* vw = GridTableView::New(this, fr);
  if (!vw) return;
  vw->BuildAll();
  fr->Render();
}


//////////////////////////
//  DataColView	//
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
  sticky = false;
  data_base = &TA_DataArray_impl;
}

void DataColView::Copy_(const DataColView& cp) {
  name = cp.name;
  visible = cp.visible;
  sticky = cp.sticky;
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

void DataColView::setDataCol(DataArray_impl* value, bool first_time) {
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
  DataArray_impl* col = dataCol();
  if (name != col->name) {
    name = col->name;
  }
  // only copy display options first time, since user may override in view
  if (first) {
    if (col->GetUserData(DataArray_impl::udkey_hidden).toBool())
      visible = false;
  }
}

bool DataColView::isVisible() const {
  return (visible && (bool)m_data);
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

  //TODO: new ones should offset pos in viewers so they don't overlap
  table_pos.SetXYZ(1.0f, 0.0f, 0.0f);
  table_scale = 1.0f;

  updating = 0;
  m_lvp = NULL;
  m_rows = 0;

  children.SetBaseType(&TA_DataColView); // subclasses need to set this to their type!
}

void DataTableView::InitLinks() {
  inherited::InitLinks();
  taBase::Own(view_range, this);
  taBase::Own(table_pos, this);
  taBase::Own(table_scale, this);
  taBase::Own(table_orient, this);
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
  table_pos = cp.table_pos;
  table_scale = cp.table_scale;
  table_orient = cp.table_orient;
}

void DataTableView::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  //note: UAE calls setDirty, which is where we do most of the rejigging
}

const String DataTableView::caption() const {
  DataTable* dt = dataTable(); 
  String rval;
  if (dt) {
    rval = dt->GetDisplayName();
  } else rval = "(no table)";
  //TODO: maybe we should also qualify with this view's designator
  // since we can have many views of the same table
  return rval;
}

void DataTableView::ClearData() {
//TODO: the data reset should actually call us back anyway
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
  return (taMisc::gui_active && display_on && isMapped());
}

void DataTableView::MakeViewRangeValid() {
  int rows = this->rows();
  if (view_range.min >= rows) {
    view_range.min = MAX(0, (rows - view_rows - 1));
  }
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
  UpdateDisplay(true);
}

void DataTableView::DataUpdateAfterEdit_impl() {
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
  UpdateFromDataTable_this(first);
  UpdateFromDataTable_child(first);
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
  DataArray_impl* dc = NULL;
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
      if (dcs->sticky) {
        dcs->setDataCol(NULL); //keep him, but unbind from any col
      } else {
        children.RemoveIdx(i);
      }
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
  if (!m_node_so.ptr()) return; // shouldn't happen
  InitPanel();
  if(m_lvp && m_lvp->t3vs) {
    m_lvp->t3vs->setSceneGraph(NULL);	// remove existing scene graph
  }
  inherited::Render_pre();
}

void DataTableView::Render_impl() {
  // set origin: in allocated area
  FloatTransform* ft = transform(true);
  ft->translate = table_pos;
  ft->rotate = table_orient;
  ft->scale = table_scale;

  inherited::Render_impl();

  T3Node* node_so = this->node_so(); // cache
  if(!node_so) return;
  SoFont* font = node_so->captionFont(true);
  float font_size = 0.4f;
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


//////////////////////////////////
//  GridColView		//
//////////////////////////////////

void GridColView::Initialize(){
  text_width = 8;
  scale_on = true;
  mat_layout = BOT_ZERO; // typical default for data patterns
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

void GridColView::UpdateFromDataCol_impl(bool first){
  inherited::UpdateFromDataCol_impl(first);
  DataArray_impl* dc = dataCol(); //note: exists, because we were called
  if (first) {
    // just get the display width, don't worry about maxwidth
    text_width = dc->displayWidth();
  
    if (dc->isMatrix() && dc->isNumeric()) {
      mat_image = dc->GetUserData("IMAGE").toBool();
    }
  }
}

String GridColView::GetDisplayName() const {
  DataArray_impl* dc = dataCol(); //note: exists, because we were called
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
  DataArray_impl* dc = dataCol(); // cache
  col_width = 0.0f;
  row_height = 0.0f;
  if (!dc) return;

  if(dc->isMatrix()) {
    int raw_width = 1;
    int raw_height = 1;
    dc->Get2DCellGeom(raw_width, raw_height, mat_odd_vert);
    // just linear in block size between range
    col_width = par->mat_size_range.Clip(raw_width);
    row_height = par->mat_size_range.Clip(raw_height);
  }
  else {
    row_height = 1.0f;		// always just one char high
    col_width = text_width;
  }
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
    T3DataView* dv = fr->FindRootViewOfData(dt);
    if (dv) {
      if (taMisc::Choice("This table is already shown in that frame -- would you like"
          " to show it in a new frame?", "&Ok", "&Cancel") != 0) return NULL;
      fr = NULL; // make a new one
    }
  } 
  if (!fr) {
    fr = T3DataViewer::GetBlankOrNewT3DataViewFrame(dt);
  }
  if (!fr) return NULL; // unexpected...
  
  GridTableView* vw = new GridTableView;
  fr->AddView(vw);
  vw->setDataTable(dt);
  return vw;
}

void GridTableView::Initialize() {
  view_rows = 10;
  col_n = 5;

  width	= 1.0f;
  grid_on = true;
  header_on = true;
  row_num_on = true;
  two_d_font = false;		// true -- this is causing mysterious so crash, disabled for now
  two_d_font_scale = 350.0f;
  mat_val_text = false;

  grid_margin = 0.01f;
  grid_line_size = 0.005f;
  row_num_width = 4;

  mat_block_spc = 0.1f;
  mat_block_height = 0.2f;
  mat_rot = 0.0f;
  mat_trans = 0.6f;

  mat_size_range.min = 4;
  mat_size_range.max = 16;
  max_text_sz = .05f;

  scale.auto_scale = false;
  scale.min = -1.0f; scale.max = 1.0f;

  row_height = 0.1f; // non-zero dummy value
  head_height = .1f;
  font_scale = .1f;

  children.SetBaseType(&TA_GridColView); // subclasses need to set this to their type!
}

void GridTableView::InitLinks() {
  inherited::InitLinks();
  taBase::Own(mat_size_range, this);
  taBase::Own(col_range, this);
  taBase::Own(scale, this);
  taBase::Own(vis_cols, this);
  taBase::Own(col_widths_raw, this);
  taBase::Own(col_widths, this);
}

void GridTableView::CutLinks() {
  scale.CutLinks();
  col_range.CutLinks();
  inherited::CutLinks();
}

void GridTableView::Copy_(const GridTableView& cp) {
  col_n = cp.col_n;

  col_range = cp.col_range;
  grid_on = cp.grid_on;
  header_on = cp.header_on;
  row_num_on = cp.row_num_on;
  two_d_font = cp.two_d_font;
  two_d_font_scale = cp.two_d_font_scale;
  scale = cp.scale;

  grid_margin = cp.grid_margin;
  grid_line_size = cp.grid_line_size;

  mat_size_range = cp.mat_size_range;
  mat_val_text = cp.mat_val_text;
  mat_block_spc = cp.mat_block_spc;
  mat_block_height = cp.mat_block_height;
  mat_trans = cp.mat_trans;
  mat_rot = cp.mat_rot;
}

void GridTableView::UpdateAfterEdit_impl(){
  inherited::UpdateAfterEdit_impl();
  if (grid_margin < 0.0f) grid_margin = 0.0f;
  if (grid_line_size <  0.0f) grid_line_size =  0.0f;
}

void GridTableView::Render_pre() {
  m_node_so = new T3GridViewNode(this, width, manip_ctrl_on);

  T3DataViewFrame* frame = GET_MY_OWNER(T3DataViewFrame);
  if(!frame) return;
  SoQtViewer* viewer = frame->widget()->ra();
  viewer->setBackgroundColor(SbColor(scale.background.redf(), scale.background.greenf(), 
				     scale.background.bluef()));
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
  RenderGrid();
  RenderHeader();
  RenderLines();
}

void GridTableView::Render_post() {
  inherited::Render_post();
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
    if ((view_range.max < old_rows-1) && (view_range.max > 0)) {
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
    if(font_scale > max_text_sz && has_mat) {
      redo = true;		// don't do more than once
      float red_rat = max_text_sz / font_scale;
      // we have a matrix and font size is too big: rescale it and then recompute
      font_scale = max_text_sz;
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

  // need another iteration now that we know the true font scale!
  if(header_on) {
    head_height = font_scale;
    float per_raw = (1.0f - head_height) / (tot_ht_raw - 1.0f);
    row_height = row_height_raw * per_raw;
  }
}

void GridTableView::GetScaleRange() {
  if(!scale.auto_scale) {
    scale.FixRangeZero();
    return;
  }
  bool got_one = false;
  MinMax sc_rg;
  for(int col = col_range.min; col<=col_range.max; ++col) {
    GridColView* cvs = (GridColView*)colVis(col);
    if(!cvs || !cvs->scale_on)
      continue;
    DataArray_impl* da = cvs->dataCol();
    if(!da->isNumeric() || !da->is_matrix) continue;
    da->GetMinMaxScale(sc_rg);
    if(!got_one)
      scale.SetMinMax(sc_rg.min, sc_rg.max);
    else
      scale.UpdateMinMax(sc_rg.min, sc_rg.max);
    got_one = true;
  }
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
    vw->viewerWindow()->AddPanelNewTab(lvp());
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
  SoSeparator* hdr = node_so->header();
  hdr->removeAllChildren();
  if (!header_on) return; // normally shouldn't be called if off

  // doesn't seem to make much diff:
  SoComplexity* cplx = new SoComplexity;
  cplx->value.setValue(.2f);
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
    T3GridColViewNode* colnd = new T3GridColViewNode(cvs);
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
    rectmat->diffuseColor.setValue(0.0f, 1.0f, 1.0f);
    rectmat->transparency.setValue(0.8f);
    colnd->topSeparator()->addChild(rectmat);
    SoCube* rect = new SoCube;
    rect->width = col_wd_lst - gr_mg_sz2;
    rect->height = head_height;
    rect->depth = gr_mg_sz;
    colnd->topSeparator()->addChild(rect);
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
    DataArray_impl* dc = cvs->dataCol();

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
	SoTransform* tr = new SoTransform(); // todo: add this to image object! get rid of extra
	img->addChild(tr);
	// scale the image according to pixel metrics
	// note: image defaults to 1 geom unit width, so we scale by our act width
	tr->scaleFactor.setValue(col_wd, row_ht, 1.0f);
	// center the shape in the middle of the cell
	tr->translation.setValue(col_wd_lst * .5f, -(row_height *.5f), 0.0f);
	SoImageEx* img_so = new SoImageEx;
	img->addChild(img_so);

	taMatrix* cell_mat =  dc->GetValAsMatrix(act_idx);
	taBase::Ref(cell_mat);
	bool top_zero = (cvs->mat_layout == GridColView::TOP_ZERO);
	img_so->setImage(*cell_mat, top_zero);
	taBase::UnRef(cell_mat);
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
	taBase::Ref(cell_mat);
	SoMatrixGrid* sogr = new SoMatrixGrid
	  (cell_mat, cvs->mat_odd_vert, &scale, (SoMatrixGrid::MatrixLayout)cvs->mat_layout, 
	   mat_val_text);
	sogr->spacing = mat_block_spc;
	sogr->block_height = mat_block_height;
	sogr->trans_max = mat_trans;
	sogr->render();
	taBase::UnRef(cell_mat);
	grsep->addChild(sogr);
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
// note on following: basically callbacks from view, so don't update panel

void GridTableView::SetColorSpec(ColorScaleSpec* color_spec) {
  scale.SetColorSpec(color_spec);
  UpdateDisplay(false);
}

void GridTableView::setGrid(bool value) {
  if (grid_on == value) return;
  grid_on = value;
  UpdateDisplay(false);
}

void GridTableView::setHeader(bool value) {
  if (header_on == value) return;
  header_on = value;
  UpdateDisplay(false);
}

void GridTableView::setRowNum(bool value) {
  if(row_num_on == value) return;
  row_num_on = value;
  UpdateDisplay(false);
}

void GridTableView::set2dFont(bool value) {
  if (two_d_font == value) return;
  two_d_font = value;
  UpdateDisplay(false);
}

void GridTableView::setValText(bool value) {
  if (mat_val_text == value) return;
  mat_val_text = value;
  UpdateDisplay(false);
}

void GridTableView::setWidth(float wdth) {
  width = wdth;
  T3GridViewNode* node_so = this->node_so();
  if (!node_so) return;
  node_so->setWidth(wdth);
  UpdateDisplay(false);
}

void GridTableView::setRows(int value) {
  if(view_rows == value) return;
  view_rows = value;
  UpdateDisplay(false);
}

void GridTableView::setCols(int value) {
  if(col_n == value) return;
  col_n = value;
  UpdateDisplay(false);
}

void GridTableView::setMatTrans(float value) {
  if(mat_trans == value) return;
  mat_trans = value;
  UpdateDisplay(false);
}

void GridTableView::setMatRot(float value) {
  if(mat_rot == value) return;
  mat_rot = value;
  UpdateDisplay(false);
}

void GridTableView::setAutoScale(bool value) {
  if(scale.auto_scale == value) return;
  scale.auto_scale = value;
  UpdateDisplay(false);
}
  
void GridTableView::setScaleData(bool auto_scale_, float min_, float max_) {
  if ((scale.auto_scale == auto_scale_) && (scale.min == min_) && (scale.max == max_)) return;
  scale.auto_scale = auto_scale_;
  if(!scale.auto_scale)
    scale.SetMinMax(min_, max_);
  UpdateDisplay(false);
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
  GridTableView* nv = (GridTableView*)vnd->dataView;

  SbRotation cur_rot;
  cur_rot.setValue(SbVec3f(nv->table_orient.x, nv->table_orient.y, 
			   nv->table_orient.z), nv->table_orient.rot);

  SbVec3f trans = dragger->translation.getValue();
//   cerr << "trans: " << trans[0] << " " << trans[1] << " " << trans[2] << endl;
  cur_rot.multVec(trans, trans); // rotate translation by current rotation
  trans[0] *= nv->table_scale.x;
  trans[1] *= nv->table_scale.y;
  trans[2] *= nv->table_scale.z;
  FloatTDCoord tr(T3GridViewNode::drag_size * trans[0],
		  T3GridViewNode::drag_size * trans[1],
		  T3GridViewNode::drag_size * trans[2]);
  nv->table_pos += tr;

  const SbVec3f& scale = dragger->scaleFactor.getValue();
//   cerr << "scale: " << scale[0] << " " << scale[1] << " " << scale[2] << endl;
  FloatTDCoord sc(scale[0], scale[1], scale[2]);
  nv->table_scale *= sc;

  SbVec3f axis;
  float angle;
  dragger->rotation.getValue(axis, angle);
//   cerr << "orient: " << axis[0] << " " << axis[1] << " " << axis[2] << " " << angle << endl;
  if(axis[0] != 0.0f || axis[1] != 0.0f || axis[2] != 1.0f || angle != 0.0f) {
    SbRotation rot;
    rot.setValue(SbVec3f(axis[0], axis[1], axis[2]), angle);
    SbRotation nw_rot = rot * cur_rot;
    nw_rot.getValue(axis, angle);
    nv->table_orient.SetXYZR(axis[0], axis[1], axis[2], angle);
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
  m_ra->setBackgroundColor(SbColor(0.8f, 0.8f, 0.8f));
}

void iDataTableView_Panel::InitPanel() {
  if (updating) return;
  ++updating;
  InitPanel_impl();
  UpdatePanel_impl();
  --updating;
}

void iDataTableView_Panel::UpdatePanel() {
  if (updating) return;
  ++updating;
  UpdatePanel_impl();
  --updating;
}

void iDataTableView_Panel::viewAll() {
  m_camera->viewAll(t3vs->root_so(), ra()->getViewportRegion());
  m_camera->focalDistance.setValue(.1); // zoom in!
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

  widg = new QWidget();
  layOuter = new QVBoxLayout(widg);

  layTopCtrls = new QHBoxLayout(layOuter);

  chkDisplay = new QCheckBox("Disp", widg, "chkDisplay");
  chkDisplay->setToolTip("Whether to update the display when the underlying data changes");
  connect(chkDisplay, SIGNAL(toggled(bool)), this, SLOT(chkDisplay_toggled(bool)) );
  layTopCtrls->addWidget(chkDisplay);

  chkHeaders =  new QCheckBox("Hdrs", widg, "chkHeaders");
  chkDisplay->setToolTip("Whether to display a top row of headers indicating the name of the columns");
  connect(chkHeaders, SIGNAL(toggled(bool)), this, SLOT(chkHeaders_toggled(bool)) );
  layTopCtrls->addWidget(chkHeaders);

  chkRowNum =  new QCheckBox("Row\n#", widg, "chkRowNum");
  chkDisplay->setToolTip("Whether to display the row number as the first column");
  connect(chkRowNum, SIGNAL(toggled(bool)), this, SLOT(chkRowNum_toggled(bool)) );
  layTopCtrls->addWidget(chkRowNum);

  chk2dFont =  new QCheckBox("2d\nFont", widg, "chk2dFont");
  chkDisplay->setToolTip("Whether to use a two-dimensional font that is easier to read but does not obey 3d transformations of the display");
  connect(chk2dFont, SIGNAL(toggled(bool)), this, SLOT(chk2dFont_toggled(bool)) );
  layTopCtrls->addWidget(chk2dFont);

  chkValText =  new QCheckBox("Val\nTxt", widg, "chkValText");
  chkDisplay->setToolTip("Whether to display text of the matrix block values.");
  connect(chkValText, SIGNAL(toggled(bool)), this, SLOT(chkValText_toggled(bool)) );
  layTopCtrls->addWidget(chkValText);

  layTopCtrls->addStretch();

  butSetColor = new QPushButton("Colors", widg);
  butSetColor->setFixedHeight(taiM->button_height(taiMisc::sizSmall));
  layTopCtrls->addWidget(butSetColor);
  connect(butSetColor, SIGNAL(pressed()), this, SLOT(butSetColor_pressed()) );

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

  layVals = new QHBoxLayout(layOuter);

  lblRows = taiM->NewLabel("Rows", widg, font_spec);
  lblRows->setToolTip("Maximum number of rows to display (row height is scaled to fit).");
  layVals->addWidget(lblRows);
  fldRows = new taiIncrField(&TA_int, NULL, NULL, widg);
  layVals->addWidget(fldRows->GetRep());
  layVals->addSpacing(taiM->hsep_c);
  connect(fldRows->rep(), SIGNAL(selectionChanged()), this, SLOT(fldRows_textChanged()) );

  lblCols = taiM->NewLabel("Cols", widg, font_spec);
  lblCols->setToolTip("Maximum number of columns to display (column widths are scaled to fit).");
  layVals->addWidget(lblCols);
  fldCols = new taiIncrField(&TA_int, NULL, NULL, widg);
  layVals->addWidget(fldCols->GetRep());
  layVals->addSpacing(taiM->hsep_c);
  connect(fldCols->rep(), SIGNAL(selectionChanged()), this, SLOT(fldCols_textChanged()) );

  lblWidth = taiM->NewLabel("Width", widg, font_spec);
  lblWidth->setToolTip("Width of grid log display, in normalized units (default is 1.0 = same as height).");
  layVals->addWidget(lblWidth);
  fldWidth = new taiField(&TA_float, NULL, NULL, widg);
  layVals->addWidget(fldWidth->GetRep());
  layVals->addSpacing(taiM->hsep_c);
  connect(fldWidth->rep(), SIGNAL(editingFinished()), this, SLOT(fldWidth_textChanged()) );

  lblTrans = taiM->NewLabel("Trans-\nparency", widg, font_spec);
  lblTrans->setToolTip("Maximum transparency of the grid blocks (0 = fully opaque, 1 = fully transparent)\nBlocks with smaller magnitude values are more transparent.");
  layVals->addWidget(lblTrans);
  fldTrans = new taiField(&TA_float, NULL, NULL, widg);
  layVals->addWidget(fldTrans->GetRep());
  layVals->addSpacing(taiM->hsep_c);
  connect(fldTrans->rep(), SIGNAL(editingFinished()), this, SLOT(fldTrans_textChanged()) );

  lblRot = taiM->NewLabel("Mat\nRot", widg, font_spec);
  lblRot->setToolTip("Rotation (in degrees) of the matrix in the Z axis, producing a denser stacking of patterns.");
  layVals->addWidget(lblRot);
  fldRot = new taiField(&TA_float, NULL, NULL, widg);
  layVals->addWidget(fldRot->GetRep());
  layVals->addSpacing(taiM->hsep_c);
  connect(fldRot->rep(), SIGNAL(editingFinished()), this, SLOT(fldRot_textChanged()) );

  layVals->addStretch();

  ////////////////////////////////////////////////////////////////////////////
  // 	Colorscale etc
  layColorScale = new QHBoxLayout(layOuter);
  
  chkAutoScale = new QCheckBox("auto scale", widg);
  connect(chkAutoScale, SIGNAL(toggled(bool)), this, SLOT(chkAutoScale_toggled(bool)) );
  layColorScale->addWidget(chkAutoScale);
  layVals->addSpacing(taiM->hsep_c);

  cbar = new HCScaleBar(&tlv->scale, ScaleBar::RANGE, true, true, widg);
//  cbar->setMaximumWidth(30);
  connect(cbar, SIGNAL(scaleValueChanged()), this, SLOT(cbar_scaleValueChanged()) );
  layColorScale->addWidget(cbar); // stretchfact=1 so it stretches to fill the space

  ////////////////////////////////////////////////////////////////////////////
  // 	viewspace guy

  layViewspace = new QHBoxLayout(layOuter);
  Constr_T3ViewspaceWidget(widg);
  layViewspace->addWidget(t3vs);

  setCentralWidget(widg);
}

iGridTableView_Panel::~iGridTableView_Panel() {
}

void iGridTableView_Panel::InitPanel_impl() {
  // nothing structural here (could split out cols, but not worth it)
}

void iGridTableView_Panel::UpdatePanel_impl() {
  inherited::UpdatePanel_impl();

  GridTableView* glv = this->glv(); //cache
//   DataTable* dt = glv->dataTable();

  viewAll();

  chkDisplay->setChecked(glv->display_on);
  chkHeaders->setChecked(glv->header_on);
  chkRowNum->setChecked(glv->row_num_on);
  chk2dFont->setChecked(glv->two_d_font);
  chkValText->setChecked(glv->mat_val_text);

  fldWidth->GetImage((String)glv->width);
  fldRows->GetImage((String)glv->view_rows);
  fldCols->GetImage((String)glv->col_n);
  fldTrans->GetImage((String)glv->mat_trans);
  fldRot->GetImage((String)glv->mat_rot);

  cbar->UpdateScaleValues();
  chkAutoScale->setChecked(glv->scale.auto_scale);

  // only show col slider if necessary
//   if(glv->col_n >= dt->cols()) {
//     t3vs->setHasHorScrollBar(false);
//   } else {
  // always show the column scrollbar!
  QScrollBar* sb = t3vs->horScrollBar(); // no autocreate
  if (!sb) {
    sb = t3vs->horScrollBar(true);
    connect(sb, SIGNAL(valueChanged(int)), this, SLOT(horScrBar_valueChanged(int)) );
    sb->setTracking(true);
  }
  sb->setMinValue(0);
  sb->setMaxValue(glv->vis_cols.size - glv->col_n);
  sb->setPageStep(glv->col_n);
  sb->setSingleStep(1);
  sb->setValue(glv->col_range.min);
//   }

  // only show row slider if necessary
  if (glv->view_rows >= glv->rows()) {
    t3vs->setHasVerScrollBar(false);
  } else {
    QScrollBar* sb = t3vs->verScrollBar(); // no autocreate
    if (!sb) {
      sb = t3vs->verScrollBar(true);
      connect(sb, SIGNAL(valueChanged(int)), this, SLOT(verScrBar_valueChanged(int)) );
      sb->setTracking(true);
      sb->setMinValue(0);
    }
    //note: the max val is set so that the last page is a full page (i.e., can't scroll past end)
    int mx = MAX((glv->rows() - glv->view_rows), 0);
    sb->setMaxValue(mx);
    //page step size based on viewable to total lines
    int pg_step = MAX(glv->view_rows, 1);
    sb->setPageStep(pg_step);
    sb->setSingleStep(1);
    sb->setValue(MIN(glv->view_range.min, mx));
  }
}

void iGridTableView_Panel::horScrBar_valueChanged(int value) {
  GridTableView* glv = this->glv(); //cache
  if (updating || !glv) return;
  glv->ViewCol_At(value);
}

void iGridTableView_Panel::verScrBar_valueChanged(int value) {
  GridTableView* glv = this->glv(); //cache
  if (updating || !glv) return;
  glv->ViewRow_At(value);
}

void iGridTableView_Panel::chkDisplay_toggled(bool on) {
  GridTableView* glv = this->glv(); //cache
  if (updating || !glv) return;
  glv->setDisplay(on);
}

void iGridTableView_Panel::chkHeaders_toggled(bool on) {
  GridTableView* glv = this->glv(); //cache
  if (updating || !glv) return;
  glv->setHeader(on);
}

void iGridTableView_Panel::chkRowNum_toggled(bool on) {
  GridTableView* glv = this->glv(); //cache
  if (updating || !glv) return;
  glv->setRowNum(on);
}

void iGridTableView_Panel::chk2dFont_toggled(bool on) {
  GridTableView* glv = this->glv(); //cache
  if (updating || !glv) return;
  glv->set2dFont(on);
}

void iGridTableView_Panel::chkValText_toggled(bool on) {
  GridTableView* glv = this->glv(); //cache
  if (updating || !glv) return;
  glv->setValText(on);
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

void iGridTableView_Panel::fldWidth_textChanged() {
  GridTableView* glv = this->glv(); //cache
  if (updating || !glv) return;

  glv->setWidth((float)fldWidth->GetValue());
}

void iGridTableView_Panel::fldRows_textChanged() {
  GridTableView* glv = this->glv(); //cache
  if (updating || !glv) return;

  glv->setRows((int)fldRows->GetValue());
}

void iGridTableView_Panel::fldCols_textChanged() {
  GridTableView* glv = this->glv(); //cache
  if (updating || !glv) return;

  glv->setCols((int)fldCols->GetValue());
}

void iGridTableView_Panel::fldTrans_textChanged() {
  GridTableView* glv = this->glv(); //cache
  if (updating || !glv) return;

  glv->setMatTrans((float)fldTrans->GetValue());
}

void iGridTableView_Panel::fldRot_textChanged() {
  GridTableView* glv = this->glv(); //cache
  if (updating || !glv) return;

  glv->setMatRot((float)fldRot->GetValue());
}

void iGridTableView_Panel::cbar_scaleValueChanged() {
  GridTableView* glv = this->glv(); //cache
  if (updating || !glv) return;

  //note: user changed value, so must no longer be autoscale
  ++updating;
  chkAutoScale->setChecked(false); //note: raises signal on widget! (grr...)
  --updating;

  glv->setScaleData(false, cbar->min(), cbar->max());
}

void iGridTableView_Panel::chkAutoScale_toggled(bool on) {
  GridTableView* glv = this->glv(); //cache
  if (updating || !glv) return;
  glv->setScaleData(on, cbar->min(), cbar->max());
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
  layOuter = new QVBoxLayout(this);
  splMain = new QSplitter(this);
  splMain->setOrientation(Qt::Vertical);
  layOuter->addWidget(splMain);
  tvTable = new iDataTableView();
  tvCell = new iMatrixTableView();
  splMain->addWidget(tvTable);
  splMain->addWidget(tvCell);
  
  connect(tvTable, SIGNAL(sig_currentChanged(const QModelIndex&)),
    this, SLOT(tvTable_currentChanged(const QModelIndex&)));
  connect(tvTable, SIGNAL(sig_dataChanged(const QModelIndex&, const QModelIndex&)),
    this, SLOT(tvTable_dataChanged(const QModelIndex&, const QModelIndex&)) );

}

iDataTableEditor::~iDataTableEditor() {
}

void iDataTableEditor::Refresh() {
  DataTableModel* mod = dynamic_cast<DataTableModel*>(tvTable->model());
  //note: this will refresh all views, not just this one
  if (mod)
    mod->refreshViews();
}

void iDataTableEditor::setDataTable(DataTable* dt_) {
  if (dt_ == m_dt) return;
  if (dt_) {
//nn    tv->setItemDelegate(new DataTableDelegate(dt_));
    tvTable->setModel(dt_->GetDataModel());
  }
  m_dt = dt_;
}

void iDataTableEditor::tvTable_currentChanged(const QModelIndex& index) {
  DataTable* dt_ = dt(); // cache
  DataArray_impl* col = dt_->GetColData(index.column());
  // note: we return from following if, otherwise fall through to do the contra
  if (col && col->is_matrix) {
    m_cell = dt_->GetValAsMatrix(index.column(), index.row());
    m_cell_index = index;
    //TODO: the ref above will prevent the col from growing, and also
    // screw up things like deleting the col -- we need to be able to "unlock"
    // this defacto lock on the col!!!!
    if (m_cell.ptr()) {
      tvCell->setModel(m_cell->GetDataModel());
      return;
    } 
  } else {  
    tvCell->setModel(NULL);
    m_cell = NULL; // good to at least release this asap!!!
    m_cell_index = QModelIndex(); // empty is invalid
  }

}

void iDataTableEditor::tvTable_dataChanged(const QModelIndex& topLeft,
    const QModelIndex& bottomRight)
{
  if (m_cell && (m_cell_index.column() >= topLeft.column()) &&
    (m_cell_index.column() <= bottomRight.column()) &&
    (m_cell_index.row() >= topLeft.row()) &&
     (m_cell_index.row() <= bottomRight.row()) )
  {
    m_cell->DataChanged(DCR_ITEM_UPDATED, NULL, NULL); // easiest way
  }
}

void iDataTableEditor::UpdateSelectedItems_impl() {
//TODO
}

//////////////////////////
//    iDataTablePanel 	//
//////////////////////////

iDataTablePanel::iDataTablePanel(taiDataLink* dl_)
:inherited(dl_)
{
  dte = NULL;
/*  list->setSelectionMode(QListView::Extended);
  list->setShowSortIndicator(true);
  // set up number of col_n, based on link
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
    rval = ci->QueryEditActions_(sel_list);
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

void iDataTablePanel::Refresh_impl() {
  if (dte)
    dte->Refresh();
  inherited::Refresh_impl();
}

void iDataTablePanel::Render_impl() {
  dte = new iDataTableEditor();
  setCentralWidget(dte);
  
  dte->setDataTable(dt());
  connect(dte->tvTable, SIGNAL(hasFocus(iTableView*)),
    this, SLOT(tv_hasFocus(iTableView*)) );
  connect(dte->tvCell, SIGNAL(hasFocus(iTableView*)),
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

void taiTabularDataMimeFactory::Mat_QueryEditActions(taMatrix* mat, 
  const CellRange& sel, taiMimeSource* ms,
  int& allowed, int& forbidden) const
{
  // ops that are never allowed on mats
  forbidden |= (taiClipData::EA_CUT | taiClipData::EA_DELETE);
  // src ops
  if (sel.nonempty())
    allowed |= taiClipData::EA_COPY;
    
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


void taiTabularDataMimeFactory::Table_QueryEditActions(DataTable* tab, 
  const CellRange& sel, taiMimeSource* ms,
  int& allowed, int& forbidden) const
{
  // ops that are never allowed on mats
  forbidden |= (taiClipData::EA_CUT | taiClipData::EA_DELETE);
  // src ops
  if (sel.nonempty())
    allowed |= taiClipData::EA_COPY;
    
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
    DataArray_impl* da = tab->GetColData(col);
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

void taiTabularDataMimeItem::WriteTable_Generic(DataTable* tab, const CellRange& sel) {
  // TODO
//TEMP
taMisc::Error("This is not implemented yet.");
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
//TODO: note: maybe nothing!
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
//TODO: note: maybe nothing!
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
  } 
  // if dest is larger than src, we shrink to src
  else {
    sel.Limit(tabCols(), tabRows());
  }
  
  // calculate the controlling params, for efficiency and clarity
  int src_rows = tabRows();
  int dst_max_cell_rows = tab->GetMaxCellRows(sel.col_fr, sel.col_to);
  int src_max_cell_rows = maxCellRows();
  int max_cell_rows = MAX(src_max_cell_rows, dst_max_cell_rows);
  // for cols, we only iterate over src, since we don't clear excess dst cols
  int src_cols = tabCols();
  int dst_cols = sel.width();
  
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
        DataArray_impl* da = tab->GetColData(dst_col);
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


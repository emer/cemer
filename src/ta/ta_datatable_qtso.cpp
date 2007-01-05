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



#include "ta_datatable_qtso.h"

// stuff to implement graphical view of datatable
#include "igeometry.h"

#include "ta_qtgroup.h"
#include "ta_qtclipdata.h"

#include "ta_datatable_so.h"

#include <QButtonGroup>
#include <QCheckBox>
#include <qclipboard.h>
#include <qimage.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qpalette.h>
#include <qpixmap.h>
#include <QPushButton>
#include <Q3ScrollView>
#include <QSplitter>
#include <QTableView>

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

#include <limits.h>
#include <float.h>
//nn? #include <unistd.h>

#define DIST(x,y) sqrt((double) ((x * x) + (y*y)))

void DataTable::ShowInViewer(T3DataViewFrame* fr) {
  GridTableView* vw = GridTableView::New(this, fr);
  if (!vw) return;
//  vw->BuildAll();
  fr->Render();
}


//////////////////////////
//  TableView		//
//////////////////////////


void TableView::setDataTable(DataTable* dt) {
  viewSpecBase()->setDataTable(dt);
}

void TableView::Initialize() {
  updating = 0;
  data_base = &TA_DataTableViewSpec;  //superceded
//obs  view_bufsz = 100;
  view_bufsz = 0; //note: set by actual class based on screen
//obs  view_shift = .2f;
  view_range.min = 0;
  view_range.max = -1;
  display_on = true;
  m_lvp = NULL;
  geom.SetXYZ(4, 3, 1);
  //TODO: new ones should offset pos in viewers so they don't overlap
  pos.SetXYZ(0-geom.x, 0, 0);
  frame_inset = 0.05f;
  m_rows = 0;
}

void TableView::InitLinks() {
  inherited::InitLinks();
  taBase::Own(view_range, this);
  taBase::Own(pos,this);
  taBase::Own(geom,this);
}

void TableView::CutLinks() {
  geom.CutLinks();
  pos.CutLinks();
  view_range.CutLinks();
  if (m_lvp) {
    m_lvp = NULL;
  }
  inherited::CutLinks();
}

void TableView::Copy_(const TableView& cp) {
  view_bufsz = cp.view_bufsz;
  view_range = cp.view_range;
  pos = cp.pos;
  pos.x++; pos.y++; pos.z--; // outset it from last one
  geom = cp.geom;
  frame_inset = cp.frame_inset;
  UpdateStage();
}

void TableView::UpdateStage() {
  stage.setValue(
    frame_inset, frame_inset, -(float)(geom.z),
    geom.x - (2 * frame_inset),
    geom.y - (2 * frame_inset),
    (float)0.0f
  );
}

void TableView::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  UpdateStage();
  //note: UAE calls setDirty, which is where we do most of the rejigging
}

const String TableView::caption() const {
  DataTable* dt = dataTable(); 
  String rval;
  if (dt) {
    rval = dt->GetDisplayName();
  } else rval = "(no table)";
  //TODO: maybe we should also qualify with this view's designator
  // since we can have many views of the same table
  return rval;
}

void TableView::ClearData() {
//TODO: the data reset should actually call us back anyway
  m_rows = 0;
  if (dataTable()) {
    dataTable()->ResetData();
  }
  ClearViewRange();
  InitDisplay();
}

void TableView::ClearViewRange() {
  view_range.min = 0;
  view_range.max = -1; // gets adjusted later
}

void TableView::DataChanged_DataTable(int dcr, void* op1, void* op2) {
// all changes to the data table (either data or struct) get monitored here
//note: we must ALWAYS make sure m_rows accords with the actual data
  DataTable* dt = dataTable(); // note: be very conservative...
  if (!dt || (dcr == DCR_ITEM_DELETING)) {
    m_rows = 0; // duh!
    UpdateView(); 
    return;
  }
  
  int delta_rows = dt->rows - m_rows;
  m_rows = dt->rows;
  
  if (!isVisible()) return;
  if ((dcr == DCR_UPDATE_VIEWS) || (dcr == DCR_DATA_UPDATE_END)) {
    if (delta_rows > 0) {
      DataChange_NewRows(delta_rows);
      return;
    } else { // if not appending rows, treat as misc update
      DataChange_Other();
      return;
    }
  } else if (dcr == DCR_STRUCT_UPDATE_END) {
    DataChange_StructUpdate();
    return;
  }
  // we don't respond to any other kinds of updates
}

void TableView::DataChange_StructUpdate() {
  UpdateView();
}
  
void TableView::DataChange_NewRows(int) {
//note: overridden in Grid and Graph
  UpdateView();
}
  
void TableView::DataChange_Other() {
//TEMP, or overridden in Grid
  UpdateView();
}

void TableView::InitDisplay(){
  UpdateStage();
  DataTable* data_table = dataTable();
  if (data_table)
    m_rows = data_table->rows;
  else m_rows = 0;
  InitDisplay_impl();
}

void TableView::InitView() {
  ClearViewRange();
  UpdateView();
}

void TableView::InitViewSpec() {
  viewSpecBase()->Render();
}

void TableView::InitPanel() {
  if (m_lvp)
    m_lvp->InitPanel();
}

bool TableView::isVisible() const {
  return (taMisc::gui_active && display_on && isMapped());
}

void TableView::MakeViewRangeValid() {
  int rows = this->rows();
  if (view_range.min >= rows) {
    view_range.min = MAX(0, (rows - 1));
  }
  view_range.max = view_range.min + view_bufsz - 1; // always keep min and max valid
  view_range.MaxLT(rows - 1); // keep it less than max
}

void TableView::Render_pre() {
  if (!m_node_so.ptr()) return; // shouldn't happen

  // release any previous incarnation of graph in panel
  if (m_lvp && m_lvp->t3vs) {
    m_lvp->t3vs->setSceneGraph(NULL);
  }
  T3Node* node_so = this->node_so(); //cache
//TODO: maybe we shouldn't set the color here...
  const iColor* col = GetEditColorInherit();
  if (col) {
    col->copyTo(node_so->material()->diffuseColor);
  }

  inherited::Render_pre();
  InitDisplay(); // for first time
}

void TableView::Render_impl() {
  // set origin: in allocated area
  FloatTransform* ft = transform(true);
  ft->translate.SetXYZ(pos.x, (pos.y + 0.5f), -pos.z);
  inherited::Render_impl();

  T3Node* node_so = this->node_so(); // cache
  SoFont* font = node_so->captionFont(true);
  float font_size = 0.4f;
  font->size.setValue(font_size); // is in same units as geometry units of network
  node_so->setCaption(caption().chars());
  //NOTE: will need to move caption appropriately in subclass
}

void TableView::Render_post() {
  inherited::Render_post();
  if (m_lvp && m_lvp->t3vs) {
    m_lvp->t3vs->setSceneGraph(node_so()); //TODO: change to node_so()->canvas()
    m_lvp->viewAll();
  }
}

void TableView::Reset_impl() {
  view_range.max = -1;
  view_range.min = 0;
  inherited::Reset_impl();
}

void TableView::setDirty(bool value) {
  inherited::setDirty(value);
  if (value) UpdateView();
}

void TableView::setDisplay(bool value) {
  if (display_on == value) return;
  display_on = value;
  UpdateAfterEdit(); // does the whole kahuna
}

void TableView::UpdatePanel() {
  if (m_lvp)
    m_lvp->UpdatePanel();
}

void TableView::UpdateView() {
  if (updating) return;
  if (!isVisible() || !display_on) return;
  //note: when display goes back on, we do the full kahuna
  ++updating;
  InitViewSpec();
  InitDisplay();
  MakeViewRangeValid();
  ViewRangeChanged();
  --updating;
}

void TableView::ViewRangeChanged() {
  if (!isVisible())
     return;
  ViewRangeChanged_impl();
  UpdatePanel();
}

void TableView::View_At(int start) {
  if (!taMisc::gui_active) return;
  int rows = this->rows();
  if (start >= rows)
    start = rows - 1;
  if (start < 0)
    start = 0;
  view_range.min = start;
  MakeViewRangeValid();
  ViewRangeChanged();
}


/*obs
void TableView::View_F() {
  if(!taMisc::gui_active) return;
  int shft = (int)((float)view_bufsz * view_shift);
  if(shft < 1) shft = 1;
  view_range.max += shft;
  int log_max = MAX(log()->log_lines-1, log()->data_range.max);
  view_range.MaxLT(log_max); // keep it less than max
  view_range.min = view_range.max - view_bufsz +1; // always keep min and max valid
  view_range.MinGT(0);
  if(view_range.max > log()->data_range.max) {
    log()->Buffer_F();
  }
  view_range.MaxLT(log()->data_range.max); // redo now that data_range.max is final
  view_range.min = view_range.max - view_bufsz +1;
  view_range.MinGT(log()->data_range.min);
  UpdateFromBuffer();
}

void TableView::View_FSF() {
  if(!taMisc::gui_active) return;
  view_range.max += view_bufsz;
  int log_max = MAX(log()->log_lines-1, log()->data_range.max);
  view_range.MaxLT(log_max); // keep it less than max
  view_range.min = view_range.max - view_bufsz +1; // always keep min and max valid
  view_range.MinGT(0);
  if(view_range.max > log()->data_range.max) {
    log()->Buffer_F();
  }
  view_range.MaxLT(log()->data_range.max); // redo now that data_range.max is final
  view_range.min = view_range.max - view_bufsz +1;
  view_range.MinGT(log()->data_range.min);
  UpdateFromBuffer();
}

void TableView::View_FF() {
  if(!taMisc::gui_active) return;
  view_range.max = MAX(log()->log_lines-1, log()->data_range.max);
  view_range.min = view_range.max - view_bufsz +1; // always keep min and max valid
  view_range.MinGT(0);
  if(view_range.max > log()->data_range.max) {
    log()->Buffer_FF();
  }
  view_range.MaxLT(log()->data_range.max); // redo now that data_range.max is final
  view_range.min = view_range.max - view_bufsz +1;
  view_range.MinGT(log()->data_range.min);
  UpdateFromBuffer();
}

void TableView::View_R() {
  if(!taMisc::gui_active) return;
  int shft = (int)((float)view_bufsz * view_shift);
  if(shft < 1) shft = 1;
  view_range.min -= shft;
  view_range.MinGT(0);
  view_range.max = view_range.min + view_bufsz -1; // always keep min and max valid
  int log_max = MAX(log()->log_lines-1, log()->data_range.max);
  view_range.MaxLT(log_max); // keep it less than max
  if(view_range.min < log()->data_range.min) {
    log()->Buffer_R();
  }
  view_range.MinGT(log()->data_range.min); // redo now that data_range.min is final
  view_range.max = view_range.min + view_bufsz -1;
  view_range.MaxLT(log()->data_range.max);
  UpdateFromBuffer();
}

void TableView::View_FSR() {
  if(!taMisc::gui_active) return;
  view_range.min -= view_bufsz;
  view_range.MinGT(0);
  view_range.max = view_range.min + view_bufsz -1; // always keep min and max valid
  int log_max = MAX(log()->log_lines-1, log()->data_range.max);
  view_range.MaxLT(log_max); // keep it less than max
  if(view_range.min < log()->data_range.min) {
    log()->Buffer_R();
  }
  view_range.MinGT(log()->data_range.min); // redo now that data_range.min is final
  view_range.max = view_range.min + view_bufsz -1;
  view_range.MaxLT(log()->data_range.max);
  UpdateFromBuffer();
}

void TableView::View_FR() {
  if(!taMisc::gui_active) return;
  view_range.min = 0;
  view_range.max = view_range.min + view_bufsz -1; // always keep min and max valid
  int log_max = MAX(log()->log_lines-1, log()->data_range.max);
  view_range.MaxLT(log_max); // keep it less than max
  if(view_range.min < log()->data_range.min) {
    log()->Buffer_FR();
  }
  view_range.MinGT(log()->data_range.min); // redo now that data_range.min is final
  view_range.max = view_range.min + view_bufsz -1;
  view_range.MaxLT(log()->data_range.max);
  UpdateFromBuffer();
}
*/


//////////////////////////
// GridTableView	//
//////////////////////////

float GridTableViewSpec::gridLineSize() const {
  return grid_line_pts * t3Misc::geoms_per_pt;
} 
float GridTableViewSpec::gridMarginSize() const {
  return grid_margin_pts * t3Misc::geoms_per_pt;
}
float GridTableViewSpec::matBlockSize() const {
  return mat_block_pts * t3Misc::geoms_per_pt;
}
float GridTableViewSpec::matBorderSize() const {
  return mat_border_pts * t3Misc::geoms_per_pt;
} 
float GridTableViewSpec::matSepSize() const {
  return mat_sep_pts * t3Misc::geoms_per_pt;
}
float GridTableViewSpec::pixelSize() const {
  return pixel_pts * t3Misc::geoms_per_pt;
}
float GridTableViewSpec::textHeight() const {
  return font.pointSize * t3Misc::char_ht_to_wd_pts * t3Misc::geoms_per_pt;
}

GridTableView* GridTableView::New(DataTable* dt, T3DataViewFrame*& fr)
{
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
  geom.SetXYZ(12, 9, 1);
  data_base = &TA_GridTableViewSpec;  //supercedes base
  col_bufsz = 0;
  head_ht = head_ht_ex = row_num_wd = row_num_wd_ex = 0.0f;
  row_height = 0.1f; // non-zero dummy value
  tot_col_widths = 0.00001f; //avoid /0
  
  grid_on = true;
  header_on = true;
  row_num_on = true;
  auto_scale = false;
  scale_range.min = -1.0f;
  scale_range.max = 1.0f;
  view_bufsz = 3;
//obs  view_shift = .2f;
}

void GridTableView::InitLinks() {
  inherited::InitLinks();
  taBase::Own(col_range, this);
  taBase::Own(scale, this);
  taBase::Own(scale_range,this);
  taBase::Own(actual_range,this);
  taBase::Own(view_spec, this);
  SetData(&view_spec);
}

void GridTableView::CutLinks() {
  SetData(NULL);
  view_spec.CutLinks();
  actual_range.CutLinks();
  scale_range.CutLinks();
  scale.CutLinks();
  col_range.CutLinks();
  inherited::CutLinks();
}

void GridTableView::Copy_(const GridTableView& cp) {
  col_bufsz = cp.col_bufsz;
  col_range = cp.col_range;
  grid_on = cp.grid_on;
  header_on = cp.header_on;
  row_num_on = cp.row_num_on;
  auto_scale = cp.auto_scale;
  scale_range = cp.scale_range;
  scale = cp.scale;
  actual_range = cp.actual_range;
  view_spec = cp.view_spec;
  //metrics calced
}

void GridTableView::AllBlockText_impl(bool on) {
  GridTableViewSpec* vs = viewSpec();
  int i;
  for (i=0; i< vs->col_specs.size; ++i) {
    GridColViewSpec* da = (GridColViewSpec*)vs->col_specs.FastEl(i);
    if (on) {
      if (da->display_style == GridColViewSpec::BLOCK)
        da->display_style = GridColViewSpec::TEXT_AND_BLOCK;
    } else { //off
      if (da->display_style == GridColViewSpec::TEXT_AND_BLOCK)
        da->display_style = GridColViewSpec::BLOCK;
    }
  }
  InitDisplay();
}

void GridTableView::CalcColMetrics() {
  //note: col_range.min is usually set to a visible col
  GridTableViewSpec* tvs = viewSpec();
  float gr_mg_sz = view_spec.gridMarginSize();
  float gr_ln_sz = (grid_on) ? view_spec.gridLineSize() : 0.0f;
  float wd_tot = 0.0f;
  float act_wd = stage.width() - row_num_wd_ex;
  // note: we display at least one col, even if it spills over size
  int last_col = col_range.min;
  for (int col = col_range.min; col < tvs->col_specs.size; ++col) { 
    GridColViewSpec* cvs = tvs->colSpec(col);
    if (!cvs->isVisible()) continue;
    wd_tot += colWidth(col) + (2 * gr_mg_sz) + gr_ln_sz;
    if (wd_tot > act_wd) break;
    last_col = col;
  }
  col_range.max = last_col;
}

void GridTableView::CalcViewMetrics() {
  GridTableViewSpec* tvs = viewSpec();
  tvs->Render(); // updates all the view stuff
  float gr_mg_sz = tvs->gridMarginSize();
  float gr_ln_sz = (grid_on) ? view_spec.gridLineSize() : 0.0f;
  // header height (if on) -- note that font height is ~ 12/8 times width
  if (header_on) {
    head_ht = (tvs->textHeight());
    head_ht_ex = head_ht + (2 * gr_mg_sz);
  } else head_ht = head_ht_ex = 0.0f;
  // row number column width (if on)
  if (row_num_on) {
    int row_chars = 5; // 5 digs should be enough -- NOTE: this could be a param
    row_num_wd = (row_chars * tvs->font.pointSize * t3Misc::geoms_per_pt);
    row_num_wd_ex = row_num_wd + (2 * gr_mg_sz) + gr_ln_sz ;
    tot_col_widths = row_num_wd_ex;
  } else {
    row_num_wd = row_num_wd_ex = 0.0f;
    tot_col_widths = 0.00001f; // avoid /0
  }
  // num vis cols, tot col width (for scrollbars), row height
  col_bufsz = 0;
  row_height = 0.00001f;
  for (int col = 0; col < tvs->col_specs.size; ++col) {
    GridColViewSpec* cvs = tvs->colSpec(col);
    if (!cvs->isVisible()) continue;
    ++col_bufsz;
    tot_col_widths += cvs->col_width;
    row_height = MAX(row_height, cvs->row_height);
  }
  // add grid lines/margins
  tot_col_widths +=  ((2 * gr_mg_sz) * col_bufsz) + (gr_ln_sz * (col_bufsz - 1));
  // calculate visible rows based on row_height and our geom
  view_bufsz = MAX((int)((stage.height() - head_ht_ex) / 
    (row_height + (2 * gr_mg_sz))), 1);
  
  CalcColMetrics();
  MakeViewRangeValid();
}

void GridTableView::ClearViewRange() {
  col_range.min = 0;
  inherited::ClearViewRange();
}


void GridTableView::Clear_impl() {
//  if (!taMisc::gui_active ) return;
  T3GridViewNode* node_so = this->node_so();
  if (node_so) {
    node_so->grid()->removeAllChildren();
    node_so->header()->removeAllChildren();
    node_so->body()->removeAllChildren();
  }
  inherited::Clear_impl();
}

void GridTableView::ColorBar_execute() {
  auto_scale = false;
/*TODO  scale_range.min = editor->cbar->min;
  scale_range.max = editor->cbar->max;
  editor->auto_scale = auto_scale;
  editor->scale_range = scale_range;
  if(auto_sc_but != NULL) {
    auto_sc_but->setDown(auto_scale);
  }
  InitDisplay(); */
}

float GridTableView::colWidth(int idx) const {
  GridColViewSpec* cs = view_spec.colSpec(idx);
  if (cs) return cs->col_width;
  else return 0.0f;
}

void GridTableView::DataChange_NewRows(int rows_added) {
  int old_rows = m_rows - rows_added;
  // if we were not at the very end, then don't scroll, but do update the panel
  if ((view_range.max < old_rows-1) && (view_range.max > 0)) {
    if (m_lvp)
      m_lvp->UpdatePanel();
    return;
  }
  // scroll down to end of new data
  view_range.max = m_rows - 1; 
  view_range.min = view_range.max - view_bufsz + 1;
  view_range.min = MAX(0, view_range.min);
  ViewRangeChanged();
}

void GridTableView::InitDisplay_impl() {
  if (!header_on) RemoveHeader();
  CalcViewMetrics();
  view_range.max = MIN(view_bufsz, (rows() - 1));
  scale.SetMinMax(scale_range.min, scale_range.max);
  InitPanel();
  RenderGrid();
  RenderHeader();
}

void GridTableView::MakeViewRangeValid() {
  inherited::MakeViewRangeValid();
  //TODO: adjust col.min to make sure it is valid
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

void GridTableView::RenderGrid() {
  T3GridViewNode* node_so = this->node_so();
  if (!node_so) return;
  SoGroup* grid = node_so->grid();
  grid->removeAllChildren(); // should have been done
  if (!grid_on) return;
  
  GridTableViewSpec* tvs = viewSpec();

  float gr_mg_sz = tvs->gridMarginSize();
  float gr_ln_sz = tvs->gridLineSize();
  // vertical lines
  SoSeparator* vert = new SoSeparator;
  // note: VERY unlikely to not need at least one line, so always make it
  SoCube* ln = new SoCube;
  ln->ref(); // our unref later will delete it if we never use it
  ln->width = gr_ln_sz; 
  ln->depth = gr_ln_sz; //note: give it depth to try to avoid disappearing lines issue
  ln->height = stage.height();
  // move y and inset by 1/2 depth of the line we make
  SoTranslation* tr = new SoTranslation();
  vert->addChild(tr);
  float y_offs =  (stage.height() * 0.5f);
  tr->translation.setValue(0.0f, -y_offs, -(ln->depth.getValue() * 0.5f));
  // row nums
  if (row_num_on) {
    tr = new SoTranslation();
    vert->addChild(tr);
    float x_offs = row_num_wd + (2 * gr_mg_sz) + (gr_ln_sz / 2);
    tr->translation.setValue(x_offs, 0.0f, 0.0f);
    vert->addChild(ln);
  }
  int idx = -1;
  for (int col = col_range.min; col <= col_range.max; ++col) {
    GridColViewSpec* cvs = tvs->colSpec(col);
    if (!cvs || !cvs->isVisible()) continue;
    ++idx;
    //note: we always put a sep after the last col, because it is usually
    // followed by blank space
    tr = new SoTranslation();
    vert->addChild(tr);
    float x_offs = colWidth(col) + (2 * gr_mg_sz) + (gr_ln_sz / 2);
    if ((idx > 0) || row_num_on) x_offs += (gr_ln_sz / 2);
    tr->translation.setValue(x_offs, 0.0f, 0.0f);
    vert->addChild(ln);
  }
  grid->addChild(vert);
  ln->unref(); // deleted if not used
}

void GridTableView::RenderHeader() {
  T3GridViewNode* node_so = this->node_so();
  if (!node_so) return;
  SoSeparator* hdr = node_so->header();
  hdr->removeAllChildren();
  if (!header_on) return; // normally shouldn't be called if off

  GridTableViewSpec* tvs = viewSpec();
  SoFont* fnt = new SoFont();
  FontSpec fs(tvs->font);
  fs.setBold(true);
  fs.copyTo(fnt);
  hdr->addChild(fnt);

  float gr_mg_sz = tvs->gridMarginSize();
  float gr_ln_sz = (grid_on) ? view_spec.gridLineSize() : 0.0f;
  // margin and baseline adj
  SoTranslation* tr = new SoTranslation();
  hdr->addChild(tr);
  float base_adj = (head_ht * t3Misc::char_base_fract);
  tr->translation.setValue(gr_mg_sz,
      - (gr_mg_sz + head_ht - base_adj), 0.0f);
  
  float col_wd_lst = 0.0f; // width of last col
  // render row_num col, if on
  if (row_num_on) {
    SoAsciiText* txt = new SoAsciiText();
    hdr->addChild(txt);
    txt->string.setValue(" #");
    col_wd_lst = row_num_wd;
  }
  int idx = -1;
  for (int col = col_range.min; col <= col_range.max; ++col) {
    GridColViewSpec* cvs = tvs->colSpec(col);
    if (!cvs || !cvs->isVisible()) continue;
    ++idx;
    if (col_wd_lst > 0.0f) { 
      tr = new SoTranslation();
      hdr->addChild(tr);
      tr->translation.setValue(
        col_wd_lst + (2 * gr_mg_sz) + gr_ln_sz, 0.0f, 0.0f);
    }
    SoAsciiText* txt = new SoAsciiText();
    hdr->addChild(txt);
    int max_chars = (int)((colWidth(col) * t3Misc::pts_per_geom) / fs.pointSize);
    txt->string.setValue(cvs->GetDisplayName().elidedTo(max_chars).chars());
    col_wd_lst = colWidth(col);
  }
}

void GridTableView::RenderLine(int view_idx, int data_row) {
  T3GridViewNode* node_so = this->node_so();
  if (!node_so) return;
  GridTableViewSpec* tvs = viewSpec();
  float gr_mg_sz = view_spec.gridMarginSize();
  float gr_ln_sz = (grid_on) ? view_spec.gridLineSize() : 0.0f;
  // origin is top-left of body area
  // make line container
  SoSeparator* ln = new SoSeparator();

  DataTable* dt = dataTable(); //cache
  String el;
  // place row and first margin here, simplifies everything...
  SoTranslation* tr = new SoTranslation();
  ln->addChild(tr);
  // 1st tr places origin for the row
  tr->translation.setValue(gr_mg_sz,
      -(gr_mg_sz + ((row_height + (2 * gr_mg_sz)) * view_idx)),
      0.0f);
  float col_wd_lst = 0.0f; // width of last col
  // following metrics will be adjusted for mat font scale, when necessary
  float txt_base_adj = tvs->font.pointSize * t3Misc::geoms_per_pt *
      t3Misc::char_base_fract;
  float text_ht = tvs->font.pointSize * t3Misc::char_ht_to_wd_pts *
        t3Misc::geoms_per_pt;

  // render row_num cell, if on
  if (row_num_on) {
    SoSeparator* row_sep = new SoSeparator;
    tr = new SoTranslation;
    row_sep->addChild(tr);
    float text_height = tvs->textHeight();
    float y_offs = ((row_height - text_height) * 0.5f) +
      text_height - txt_base_adj;
    el = String(data_row);
    tr->translation.setValue(row_num_wd, -y_offs, 0.0f);
    SoAsciiText* txt = new SoAsciiText();
    txt->justification = SoAsciiText::RIGHT;
    row_sep->addChild(txt);
    txt->string.setValue(el.chars());
    col_wd_lst = row_num_wd;
    ln->addChild(row_sep);
  }
  int idx = -1;
  for (int col = col_range.min; col <= col_range.max; col++) {
    GridColViewSpec* cvs = tvs->colSpec(col);
    if (!cvs || !cvs->isVisible()) continue;
    ++idx;
    
    DataArray_impl* dc = cvs->dataCol();
    //calculate the actual col row index, for the case of a jagged data table
    int act_idx; // <0 for null
    dt->idx(data_row, dc->rows(), act_idx);
    
    // translate the col to proper location
    if (col_wd_lst > 0.0f) { 
      tr = new SoTranslation();
      ln->addChild(tr);
      tr->translation.setValue(
        col_wd_lst + (2 * gr_mg_sz) + gr_ln_sz, 0.0f, 0.0f);
    }
    float col_width = colWidth(col); // cache for convenience
    
    // following metrics will be adjusted for mat font scale, if applicable
    txt_base_adj = tvs->font.pointSize * t3Misc::geoms_per_pt *
	t3Misc::char_base_fract;
    text_ht = tvs->font.pointSize * t3Misc::char_ht_to_wd_pts *
          t3Misc::geoms_per_pt;
    // cache 2d-equivalent geom info, and render according to col style
    iVec2i cg; // 1x1 for scalar -- have to init here so we can jump to cont
    dc->Get2DCellGeom(cg); 
    // null columns just get single "n/a" regardless of geom etc.
    if (act_idx < 0) {
      SoSeparator* nul_sep = new SoSeparator;
      ln->addChild(nul_sep);
      tr = new SoTranslation;
      nul_sep->addChild(tr);
      SoAsciiText* txt = new SoAsciiText();
      nul_sep->addChild(txt);
      txt->justification = SoAsciiText::CENTER;
      txt->string.setValue("n/a");
      // center the n/a in the cell
      float x_offs = col_width / 2;
      float y_offs = ((row_height - text_ht) / 2) +
         text_ht - txt_base_adj;
      tr->translation.setValue(x_offs, -y_offs, 0.0f);
      col_wd_lst = col_width;
      continue;
    }
    float bsz = tvs->matBlockSize(); 
    float bbsz = tvs->matBorderSize();
    // following calced if used 
    float blk_area_wd = 0.0f;
    float blk_area_ht = 0.0f; 

    // todo: this should all be done with a single unitary trianglestrip guy
    // as in the netview blocks mode
    if (cvs->display_style & GridColViewSpec::BLOCK_MASK) {
      blk_area_wd = (bsz * cg.x) + (bbsz * (cg.x - 1));
      blk_area_ht = (bsz * cg.y) + (bbsz * (cg.y - 1)); 
      SoSeparator* blk = new SoSeparator;
      ln->addChild(blk);
      
      // vertically center (may be smaller than total)
      tr = new SoTranslation;
      float y_offs = (row_height - blk_area_ht) * 0.5f;
      tr->translation.setValue(0.0f, -y_offs, 0.0f); 
      blk->addChild(tr);
        
      T3Color col;
      iVec2i c; // index coords
      //note: accessor uses a linear cell value; 
      // for TOP_ZERO, this can just iterate from zero
      // for BOT_ZERO we need to massage it
      int cell = 0; // accessor just uses a linear cell value
      SoRect* cu = new SoRect; // all can share!!!
      cu->ref();
      cu->setDimensions(bsz, bsz);
      bool is_image = dc->isImage(); // use true color for images
      int comps = (is_image) ? dc->imageComponents() : 1; //only used for images
      for (c.y = 0; c.y < cg.y; ++c.y) {
        if (cvs->mat_layout == GridColViewSpec::BOT_ZERO)
          cell = cg.x * (cg.y - c.y - 1);
        SoSeparator* blk_ln = new SoSeparator;
        blk->addChild(blk_ln);
        // add line trans 
        tr = new SoTranslation();
        blk_ln->addChild(tr);
        tr->translation.setValue(
          bbsz + (bsz / 2), // offset border plus cube center
          - (bbsz + ((bsz + bbsz)* c.y) + (bsz / 2)), 0.0f);
        for (c.x = 0; c.x < cg.x; ++c.x) {
          // add block trans
          if (c.x > 0) {
            tr = new SoTranslation();
            blk_ln->addChild(tr);
            tr->translation.setValue((bsz + bbsz), 0.0f, 0.0f);
          }
          SoBaseColor* bc = new SoBaseColor;
          blk_ln->addChild(bc);
          float val = dc->GetValAsFloatM(act_idx, cell);
          if (is_image) {
            if (comps > 2) { // color of some kind
              // rgb val correct for topzero, inverted for bot zero (g always right)
              int rgb_mod = (c.y / (cg.y / comps)) % comps; //r=0,g=1,b=2
              if (rgb_mod == 1)
                col.setValue(0, val, 0);
              else  if ((rgb_mod >= 0) && (rgb_mod <= 2)) {
                if (cvs->mat_layout == GridColViewSpec::BOT_ZERO) {
                  if (rgb_mod == 0) //aka blue
                    col.setValue(0, 0, val);
                  else // 2, aka red
                    col.setValue(val, 0, 0);
                } else {
                  col.setValue(0,0,0);
                  col.rgb[rgb_mod] = val;
                }
              } else // unknown component (ex alpha)
                col.setValue(val, val, val);
            } else // b&w
              col.setValue(val, val, val);
          } else { // just use color scale
            //NOTE: following a bit dangerous, but should never be null
            col = *(scale.GetColor(val));
          }
          bc->rgb = (SbColor)col;
          blk_ln->addChild(cu);
          ++cell;
        }
      }
      cu->unref(); 
    }
    if (cvs->display_style == GridColViewSpec::TEXT_AND_BLOCK) {
      tr = new SoTranslation;
      tr->translation.setValue(blk_area_wd +tvs->matSepSize(), 0.0f, 0.0f); 
      ln->addChild(tr);
    }
    if (cvs->display_style & GridColViewSpec::TEXT_MASK) {
      SoSeparator* txt_sep = new SoSeparator;
      ln->addChild(txt_sep);
      // text origin is bottom left (Left) or bottom right (Right)
      // and we want to center vertically when  height is greater than txt height
      SoAsciiText::Justification just = SoAsciiText::LEFT;
      float x_offs = 0.0f; // default for left
      if (!dc->isMatrix()) {
        tr = new SoTranslation;
        txt_sep->addChild(tr);
        float y_offs = ((row_height - cvs->row_height) / 2) +
          cvs->row_height - txt_base_adj;
        if (dc->isNumeric()) {
          just = SoAsciiText::RIGHT;
          x_offs = col_width;
          el = Variant::formatNumber(dc->GetValAsVar(act_idx),
            cvs->num_prec);
        } else {
          int max_chars = (int)((col_width * t3Misc::pts_per_geom) / tvs->font.pointSize);
          el = dc->GetValAsString(act_idx).elidedTo(max_chars);
        }
        tr->translation.setValue(x_offs, -y_offs, 0.0f);
        SoAsciiText* txt = new SoAsciiText();
        txt_sep->addChild(txt);
        txt->justification = just;
        txt->string.setValue(el.chars());
      } else { // matrix
        float text_wd = tvs->font.pointSize * cvs->text_width *
          t3Misc::geoms_per_pt;
        // text usually smaller for mats
        if (tvs->mat_font_scale != 1.0f) {
          SoFont* fnt = new SoFont();
          FontSpec fs(tvs->font);
          fs.pointSize *= tvs->mat_font_scale;
          text_ht *= tvs->mat_font_scale;
          text_wd *= tvs->mat_font_scale;
          txt_base_adj *=  tvs->mat_font_scale;
          fs.copyTo(fnt);
          txt_sep->addChild(fnt);
        }
        // do the base height, and justification translations (once)
        float y_offs = -txt_base_adj;
        tr = new SoTranslation;
        txt_sep->addChild(tr);
        if (dc->isNumeric()) {
          just = SoAsciiText::RIGHT;
          x_offs = text_wd;
        }
        tr->translation.setValue(x_offs, -y_offs, 0.0f);
        //note: accessor uses a linear cell value; 
        // for TOP_ZERO, this can just iterate from zero
        // for BOT_ZERO we need to massage it
        int cell = 0; // accessor just uses a linear cell value
        iVec2i c;
        for (c.y = 0; c.y < cg.y; ++c.y) {
          if (cvs->mat_layout == GridColViewSpec::BOT_ZERO)
            cell = cg.x * (cg.y - c.y - 1);
          SoSeparator* txt_ln = new SoSeparator;
          txt_sep->addChild(txt_ln);
          for (c.x = 0; c.x < cg.x; ++c.x) {
            // add line and/or block trans
            tr = new SoTranslation();
            txt_ln->addChild(tr);
            float x_offs = 0.0f;
            float y_offs = 0.0f;
            if (c.x == 0) {
              if (c.y > 0)
                y_offs = (text_ht * c.y) + (bbsz * (c.y - 1));
            } else { // >0
              x_offs = text_wd + bbsz;
            }
            tr->translation.setValue(x_offs, -y_offs, 0.0f);
            if (dc->isNumeric()) {
              el = Variant::formatNumber(dc->GetValAsVarM(
                act_idx, cell), cvs->num_prec);
            } else {
              el = dc->GetValAsStringM(act_idx, cell).
                elidedTo(cvs->text_width);
            }
            SoAsciiText* txt = new SoAsciiText();
            txt->justification = just;
            txt->string.setValue(el.chars());
            txt_ln->addChild(txt);
            ++cell;
          } //c.x
        } //c.y
      } //matrix text
    } //text
    
    if (cvs->display_style == GridColViewSpec::IMAGE) {
      //note: y will be #comps*image.y, but this doesn't affect us here
      if ((cg.x == 0) || (cg.y == 0)) continue; // something wrong!
      SoSeparator* img = new SoSeparator;
      ln->addChild(img);
      SoTransform* tr = new SoTransform();
      img->addChild(tr);
      // scale the image according to pixel metrics
      //note: image defaults to 1 geom unit width, so we scale by our act width
      float pxsz =  tvs->pixelSize();
      float scale_fact = pxsz * cg.x; // note: NOT row_height
      tr->scaleFactor.setValue(scale_fact, scale_fact, 1.0f);
      // center the shape in the middle of the cell
      tr->translation.setValue((col_width / 2), -(row_height / 2), 0.0f);
      SoImageEx* img_so = new SoImageEx;
      img->addChild(img_so);

      taMatrix* cell_mat =  dc->GetValAsMatrix(act_idx);
      taBase::Ref(cell_mat);
      bool top_zero = (cvs->mat_layout == GridColViewSpec::TOP_ZERO);
      img_so->setImage(*cell_mat, top_zero);
      taBase::UnRef(cell_mat);
    }
    col_wd_lst = col_width; 
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
  FontSpec& fs = viewSpec()->font;
  fs.copyTo(fnt);
  body->addChild(fnt);
  if (header_on) {
    SoTranslation* tr = new SoTranslation;
    body->addChild(tr);
    tr->translation.setValue(0.0f, -head_ht_ex, 0.0f);
  }

  // this is the index (in view_range units) of the first view line of data in the buffer
  int view_idx = 0;
  for (int data_row = view_range.min; data_row <= view_range.max; ++data_row) {
    RenderLine(view_idx, data_row);
    ++view_idx;
  }
}

void GridTableView::Render_pre() {
  m_node_so = new T3GridViewNode(this);

  inherited::Render_pre();
}

void GridTableView::Render_impl() {
  inherited::Render_impl();
  T3GridViewNode* node_so = this->node_so(); // cache
  node_so->setGeom(geom.x, geom.y, frame_inset);
  SoFont* font = node_so->captionFont(true);
  node_so->transformCaption(iVec3f(0.0f, -((float)font->size.getValue()), 0.0f)); //move caption below the frame
}

void GridTableView::Render_post() {
  RenderGrid();
  RenderHeader();
  ViewRangeChanged(); //TODO: should this really be here???
  inherited::Render_post();
}

void GridTableView::Reset_impl() {
  col_range.min = 0;
  inherited::Reset_impl();
}

void GridTableView::setAutoScale(bool value) {
  if (auto_scale == value) return;
  //TODO:
}
  
void GridTableView::setGrid(bool value) {
  if (grid_on == value) return;
  grid_on = value;
  RenderGrid(); // or remove
  ViewRangeChanged();
}

void GridTableView::setHeader(bool value) {
  if (header_on == value) return;
  header_on = value;
  RenderHeader(); // or remove
  ViewRangeChanged();
}

void GridTableView::SetBlockSizes(float block_sz, float border_sz) {
  view_spec.mat_block_pts = block_sz;
  view_spec.mat_border_pts = border_sz;
  UpdateAfterEdit();
}

void GridTableView::SetViewFontSize(int point_size) {
  view_spec.font.SetFontSize(point_size);
  UpdateAfterEdit();
}

void GridTableView::ViewRangeChanged_impl() {
  RemoveLines();
  RenderLines();
}

void GridTableView::VScroll(bool left) {
  if (left) {
    ViewC_At(col_range.min - 1);
  } else {
    ViewC_At(col_range.min + 1);
  }
}

void GridTableView::ViewC_At(int start) {
  if (start < 0) start = 0;
  if (start >= view_spec.col_specs.size) start = view_spec.col_specs.size - 1;
  if (col_range.min == start) return;//TODO: may be prob if cols go vis/invis
  RemoveLines();
  RemoveHeader(); // noop if off
  RemoveGrid();
  col_range.min = start;
  CalcColMetrics();
  MakeViewRangeValid();
  if (grid_on) RenderGrid();
  if (header_on) RenderHeader();
  RenderLines();
}

void  GridTableView::ViewC_VisibleAt(int ord_idx) {
  if (ord_idx < 0) ord_idx = 0;
  if (ord_idx >= col_bufsz) ord_idx = col_bufsz - 1;
  // find the ord_idx'th visible col index
  int col;
  for (col = 0; col < view_spec.col_specs.size; ++col) {
    DataColViewSpec* dcs = view_spec.colSpec(col);
    if (!dcs->isVisible()) continue;
    if (--ord_idx < 0) break;
  }
  ViewC_At(col);
}

//////////////////////////
//    iTableView_Panel //
//////////////////////////

iTableView_Panel::iTableView_Panel(TableView* lv)
:inherited(lv)
{
  init(false);
}

iTableView_Panel::iTableView_Panel(bool is_grid_log, TableView* lv)
:inherited(lv)
{
  init(is_grid_log);
}

//TEMP -- will be pixmaps
const char* but_strs[] = {"|<","<<","<",">",">>",">|","Update","Init","Clear"};

void iTableView_Panel::init(bool is_grid_log)
{
  t3vs = NULL; //these are created in  Constr_T3ViewspaceWidget
  m_ra = NULL;
  m_camera = NULL;
  
  widg = new QWidget();
  layOuter = new QVBoxLayout(widg);

  layTopCtrls = new QHBoxLayout(layOuter);

// //   layDispCheck = new QHBoxLayout(layTopCtrls);
  chkDisplay = new QCheckBox("Display", widg, "chkDisplay");
  layTopCtrls->addWidget(chkDisplay);

  if (is_grid_log) {
    chkAuto =  new QCheckBox("Auto", widg, "chkAuto");
    layTopCtrls->addWidget(chkAuto);
    chkHeaders =  new QCheckBox("Hdrs", widg, "chkHeaders");
    layTopCtrls->addWidget(chkHeaders);
  } else {
    chkAuto = NULL;
    chkHeaders = NULL;
  }
  layTopCtrls->addStretch();


  layVcrButtons = new QHBoxLayout(layTopCtrls);
  bgpTopButtons = new QButtonGroup(widg); // NOTE: not a widget
  bgpTopButtons->setExclusive(false); // not applicable
  int but_ht = taiM->button_height(taiMisc::sizSmall);
  for (int i = BUT_BEG_ID; i <= BUT_END_ID; ++i) {
    QPushButton* pb = new QPushButton(widg);
    pb->setText(but_strs[i]);
    pb->setMaximumHeight(but_ht);
    pb->setMaximumWidth(20);
    layVcrButtons->addWidget(pb);
    bgpTopButtons->addButton(pb, i);
  }
  layTopCtrls->addStretch();

  layInitButtons = new QHBoxLayout(layTopCtrls);
  for (int i = BUT_UPDATE; i <= BUT_CLEAR; ++i) {
    QPushButton* pb = new QPushButton(widg);
    pb->setText(but_strs[i]);
    pb->setMaximumHeight(but_ht);
    layInitButtons->addWidget(pb);
    bgpTopButtons->addButton(pb, i);
  }

  layContents = new QHBoxLayout(layOuter);

  setCentralWidget(widg);
  connect(chkDisplay, SIGNAL(toggled(bool)), this, SLOT(chkDisplay_toggled(bool)) );
  connect(bgpTopButtons, SIGNAL(buttonClicked(int)), this, SLOT(buttonClicked(int)) );
}

iTableView_Panel::~iTableView_Panel() {
  delete bgpTopButtons; bgpTopButtons = NULL;

}

void iTableView_Panel::buttonClicked(int id) {
  if (updating) return;
  TableView* lv;
  if (!(lv = this->lv())) return;

  switch (id) {
  case BUT_BEG_ID:
    break;
  case BUT_FREV_ID:
    break;
  case BUT_REV_ID:
    break;
  case BUT_FWD_ID:
    break;
  case BUT_FFWD_ID:
    break;
  case BUT_END_ID:
    break;

  case BUT_UPDATE:
    lv->UpdateView();
    break;
  case BUT_INIT:
    lv->InitView();
    break;
  case BUT_CLEAR:
    lv->ClearData();
    break;
  }
}

void iTableView_Panel::chkDisplay_toggled(bool on) {
  if (updating) return;
  TableView* lv;
  if (!(lv = this->lv())) return;

  lv->setDisplay(on);
}

void iTableView_Panel::Constr_T3ViewspaceWidget() {
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

  m_camera->viewAll(root, m_ra->getViewportRegion());
  m_ra->setBackgroundColor(SbColor(0.5f, 0.5f, 0.5f));


  layContents->addWidget(t3vs);
}

void iTableView_Panel::InitPanel() {
  if (updating) return;
  ++updating;
  InitPanel_impl();
  UpdatePanel_impl();
  --updating;
}

void iTableView_Panel::UpdatePanel() {
  if (updating) return;
  ++updating;
  UpdatePanel_impl();
  --updating;
}

void iTableView_Panel::UpdatePanel_impl() {
  TableView* lv = this->lv(); // cache
  chkDisplay->setChecked(lv->display_on);
}

void iTableView_Panel::viewAll() {
  m_camera->viewAll(t3vs->root_so(), ra()->getViewportRegion());
}


//////////////////////////
// iGridTableView_Panel //
//////////////////////////

iGridTableView_Panel::iGridTableView_Panel(GridTableView* tlv)
:inherited(true, tlv)
{
  Constr_T3ViewspaceWidget();

  connect(chkAuto, SIGNAL(toggled(bool)), this, SLOT(chkAuto_toggled(bool)) );
  connect(chkHeaders, SIGNAL(toggled(bool)), this, SLOT(chkHeaders_toggled(bool)) );
}

iGridTableView_Panel::~iGridTableView_Panel() {
}


void iGridTableView_Panel::InitPanel_impl() {
  inherited::InitPanel_impl();
  GridTableView* lv = this->glv(); //cache
  // only show col slider if necessary
  if (lv->tot_col_widths <= lv->stage.width() ) {
    t3vs->setHasHorScrollBar(false);
  } else {
    QScrollBar* sb = t3vs->horScrollBar(); // no autocreate
    if (!sb) {
      sb = t3vs->horScrollBar(true);
      connect(sb, SIGNAL(valueChanged(int)), this, SLOT(horScrBar_valueChanged(int)) );
      sb->setTracking(true);
    }
    sb->setMinValue(0);
    sb->setMaxValue(lv->col_bufsz - 1);
/*obs    //we make a crude estimate of page step based on ratio of act width to tot width
    int pg_step = (int)(((lv->geom.x / lv->tot_col_widths) * lv->col_bufsz) + 0.5f);
    if (pg_step == 0) pg_step = 1; */
    // for cols, only convenient page step is 1 because Qt forces the little arrows
    // to also be the same step size as clicking in the blank area
    int pg_step = 1;
    sb->setSteps(1, pg_step);
  }
}

void iGridTableView_Panel::UpdatePanel_impl() {
  inherited::UpdatePanel_impl();
  GridTableView* lv = this->glv(); //cache
  chkHeaders->setChecked(lv->header_on);
  chkAuto->setChecked(lv->auto_scale);
  // only show row slider if necessary
  if (lv->view_bufsz >= lv->rows()) {
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
    int mx = MAX((lv->rows() - lv->view_bufsz), 0);
    sb->setMaxValue(mx);
    //page step size based on viewable to total lines
    int pg_step = MAX(lv->view_bufsz, 1);
    sb->setSteps(1, pg_step);
    sb->setValue(MIN(lv->view_range.min, mx));
  }
  // col pos
  QScrollBar* sb = t3vs->horScrollBar(); // no autocreate
  if (sb) {
    sb->setValue(lv->col_range.min);
  }

}

void iGridTableView_Panel::horScrBar_valueChanged(int value) {
  GridTableView* glv = this->glv(); //cache
  if (updating || !glv) return;
  glv->ViewC_VisibleAt(value);
}

void iGridTableView_Panel::verScrBar_valueChanged(int value) {
  GridTableView* glv = this->glv(); //cache
  if (updating || !glv) return;
  glv->View_At(value);
}

void iGridTableView_Panel::chkAuto_toggled(bool on) {
  GridTableView* glv = this->glv(); //cache
  if (updating || !glv) return;
  glv->setAutoScale(on);
}

void iGridTableView_Panel::chkHeaders_toggled(bool on) {
  GridTableView* glv = this->glv(); //cache
  if (updating || !glv) return;
  glv->setHeader(on);
}



String iGridTableView_Panel::panel_type() const {
  static String str("Grid Log");
  return str;
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
  emit currentChanged(current);
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
  tvCell = new QTableView();
  splMain->addWidget(tvTable);
  splMain->addWidget(tvCell);
  
  connect(tvTable, SIGNAL(currentChanged(const QModelIndex&)),
    this, SLOT(tvTable_currentChanged(const QModelIndex&)));
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
  if (!col) return;
  // note: we return from following if, otherwise fall through to do the contra
  if (col->is_matrix) {
    m_cell = dt_->GetValAsMatrix(index.column(), index.row());
    //TODO: the ref above will prevent the col from growing, and also
    // screw up things like deleting the col -- we need to be able to "unlock"
    // this defacto lock on the col!!!!
    if (m_cell.ptr()) {
      tvCell->setModel(m_cell->GetDataModel());
      return;
    } 
  }   
  tvCell->setModel(NULL);
  m_cell = NULL; // good to at least release this asap!!!

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
  // set up number of col_bufsz, based on link
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

void iDataTablePanel::Refresh_impl() {
  if (dte)
    dte->Refresh();
  inherited::Refresh_impl();
}

void iDataTablePanel::Render_impl() {
  dte = new iDataTableEditor();
  setCentralWidget(dte);
  
  dte->setDataTable(dt());
}


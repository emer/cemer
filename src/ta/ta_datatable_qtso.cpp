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
//obs  view_rows = 100;
  view_rows = 3; //note: set by actual class based on screen
//obs  view_shift = .2f;
  view_range.min = 0;
  view_range.max = -1;
  display_on = true;
  m_lvp = NULL;
  //TODO: new ones should offset pos in viewers so they don't overlap
  pos.SetXYZ(1.0f, 0.0f, 0.0f);
  frame_inset = 0.005f;
  m_rows = 0;
}

void TableView::InitLinks() {
  inherited::InitLinks();
  taBase::Own(view_range, this);
  taBase::Own(pos,this);
}

void TableView::CutLinks() {
  pos.CutLinks();
  view_range.CutLinks();
  if (m_lvp) {
    m_lvp = NULL;
  }
  inherited::CutLinks();
}

void TableView::Copy_(const TableView& cp) {
  view_rows = cp.view_rows;
  view_range = cp.view_range;
  pos = cp.pos;
  pos.x++; 
  frame_inset = cp.frame_inset;
}

void TableView::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
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
    view_range.min = MAX(0, (rows - view_rows - 1));
  }
  view_range.max = view_range.min + view_rows - 1; // always keep min and max valid
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

void TableView::View_FSF() {
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

void TableView::View_FF() {
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

void TableView::View_R() {
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

void TableView::View_FSR() {
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

void TableView::View_FR() {
  if(!taMisc::gui_active) return;
  view_range.min = 0;
  view_range.max = view_range.min + view_rows -1; // always keep min and max valid
  int log_max = MAX(log()->log_lines-1, log()->data_range.max);
  view_range.MaxLT(log_max); // keep it less than max
  if(view_range.min < log()->data_range.min) {
    log()->Buffer_FR();
  }
  view_range.MinGT(log()->data_range.min); // redo now that data_range.min is final
  view_range.max = view_range.min + view_rows -1;
  view_range.MaxLT(log()->data_range.max);
  UpdateFromBuffer();
}
*/


//////////////////////////
// GridTableView	//
//////////////////////////

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
  data_base = &TA_GridTableViewSpec;  //supercedes base
  col_n = 5;
  row_height = 0.1f; // non-zero dummy value
  head_height = .1f;
  font_scale = .1f;
  
  grid_on = true;
  header_on = true;
  row_num_on = true;
  auto_scale = false;
  scale_range.min = -1.0f;
  scale_range.max = 1.0f;
  view_rows = 3;
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
  col_n = cp.col_n;
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

void GridTableView::CalcViewMetrics() {
  // compute column widths: first generate requests based on requested col_width (computed
  // in Render above, see ta_datatable.cpp, Render_impl
  // then normalize; units are in characters
  GridTableViewSpec* tvs = viewSpec();

  float tot_wd_raw = 0.0f;
  col_widths_raw.Reset();
  if(row_num_on) {
    col_widths_raw.Add(5.0f);
    tot_wd_raw += 5.0f;
  }

  row_height_raw = 1.0f;
  for(int col = col_range.min; col<=col_range.max; ++col) {
    GridColViewSpec* cvs = tvs->colSpec(col);
    if(!cvs->isVisible()) continue;
    col_widths_raw.Add(cvs->col_width);
    tot_wd_raw += cvs->col_width;
    row_height_raw = MAX(row_height_raw, cvs->row_height);
  }

  if(tot_wd_raw == 0.0f) return;	// something wrong

  // now normalize
  col_widths.SetSize(col_widths_raw.size);
  for(int i=0;i<col_widths_raw.size;i++) {
    col_widths[i] = col_widths_raw[i] / tot_wd_raw;
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
  float row_font_scale = row_height; // todo: account for margins..
  
  font_scale = MIN(col_font_scale, row_font_scale);
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
  view_range.min = view_range.max - view_rows + 1;
  view_range.min = MAX(0, view_range.min);
  ViewRangeChanged();
}

void GridTableView::InitDisplay_impl() {
  if (!header_on) RemoveHeader();
  GridTableViewSpec* tvs = viewSpec();
  tvs->Render(); // updates all the view stuff
  MakeViewRangeValid();
  CalcViewMetrics();
  scale.SetMinMax(scale_range.min, scale_range.max);
  InitPanel();
  RenderGrid();
  RenderHeader();
}

void GridTableView::MakeViewRangeValid() {
  inherited::MakeViewRangeValid();
  DataTable* data_table = dataTable();
  int cols = data_table->cols();
  if (col_range.min >= cols) {
    col_range.min = MAX(0, (cols - col_n - 1));
  }
  // ensure thate col_range.min is a visible column!
  GridTableViewSpec* tvs = viewSpec();
  GridColViewSpec* min_cvs = tvs->colSpec(col_range.min);
  while(!min_cvs->isVisible() && col_range.min > 0) {
    col_range.min--; min_cvs = tvs->colSpec(col_range.min);
  }
  while(!min_cvs->isVisible() && col_range.min <= cols-2) {
    col_range.min++; min_cvs = tvs->colSpec(col_range.min);
  }
  int act_n = 0;
  int col;
  for(col = col_range.min; col<cols; ++col) {
    GridColViewSpec* cvs = tvs->colSpec(col);
    if(!cvs->isVisible())
      continue;
    act_n++;
    if(act_n >= col_n)
      break;
  }
  col_range.max = col-1;
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

  return;			// todo: not doing yet!!

  /*  
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
  */
}

void GridTableView::RenderHeader() {
  T3GridViewNode* node_so = this->node_so();
  if (!node_so) return;
  SoSeparator* hdr = node_so->header();
  hdr->removeAllChildren();
  if (!header_on) return; // normally shouldn't be called if off

  GridTableViewSpec* tvs = viewSpec();
  SoFont* fnt = new SoFont();
  fnt->name = "Arial";
  fnt->size.setValue(font_scale);
  hdr->addChild(fnt);

  float gr_mg_sz = view_spec.grid_margin * font_scale;
  float gr_mg_sz2 = 2.0f * gr_mg_sz;
  float gr_ln_sz = (grid_on) ? view_spec.grid_line_size * font_scale : 0.0f;
  // margin and baseline adj
  SoTranslation* tr = new SoTranslation();
  hdr->addChild(tr);
  //  float base_adj = (head_height * t3Misc::char_base_fract);
  float base_adj = 0.0f;
  tr->translation.setValue(gr_mg_sz, - (gr_mg_sz + head_height - base_adj), 0.0f);

  int col_idx = 0;
  float col_wd_lst = 0.0f; // width of last col
  // render row_num col, if on
  if (row_num_on) {
    SoAsciiText* txt = new SoAsciiText();
    hdr->addChild(txt);
    txt->string.setValue(" #");
    col_wd_lst = col_widths[col_idx++];
  }
  for (int col = col_range.min; col <= col_range.max; ++col) {
    GridColViewSpec* cvs = tvs->colSpec(col);
    if (!cvs || !cvs->isVisible()) continue;
    if (col_wd_lst > 0.0f) { 
      tr = new SoTranslation();
      hdr->addChild(tr);
      tr->translation.setValue(col_wd_lst + gr_mg_sz, 0.0f, 0.0f);
    }
    col_wd_lst = col_widths[col_idx];
    SoAsciiText* txt = new SoAsciiText();
    hdr->addChild(txt);
    int max_chars = (int)(t3Misc::char_ht_to_wd_pts * col_wd_lst / font_scale) + 1;
    txt->string.setValue(cvs->GetDisplayName().elidedTo(max_chars).chars());
    col_idx++;
  }
}

void GridTableView::RenderLine(int view_idx, int data_row) {
  T3GridViewNode* node_so = this->node_so();
  if (!node_so) return;
  GridTableViewSpec* tvs = viewSpec();
  float gr_mg_sz = view_spec.grid_margin * font_scale;
  float gr_mg_sz2 = 2.0f * gr_mg_sz;
  float gr_ln_sz = (grid_on) ? view_spec.grid_line_size * font_scale : 0.0f;
  // origin is top-left of body area
  // make line container
  SoSeparator* ln = new SoSeparator();

  DataTable* dt = dataTable(); //cache
  String el;
  // place row and first margin here, simplifies everything...
  SoTranslation* tr = new SoTranslation();
  ln->addChild(tr);
  // 1st tr places origin for the row
  tr->translation.setValue(gr_mg_sz, -(gr_mg_sz + row_height * (float)view_idx), 0.0f);

  float text_ht = font_scale;

  int col_idx = 0;
  float col_wd_lst = 0.0f; // width of last col
  // following metrics will be adjusted for mat font scale, when necessary
//   float txt_base_adj = tvs->font.pointSize * t3Misc::geoms_per_pt *
//       t3Misc::char_base_fract;
//   float text_ht = tvs->font.pointSize * t3Misc::char_ht_to_wd_pts *
//         t3Misc::geoms_per_pt;
  float txt_base_adj = 0.0f;

  // render row_num cell, if on
  if (row_num_on) {
    SoSeparator* row_sep = new SoSeparator;
    col_wd_lst = col_widths[col_idx];
    tr = new SoTranslation;
    row_sep->addChild(tr);
    float y_offs = ((row_height - text_ht) * 0.5f) + text_ht - txt_base_adj;
    el = String(data_row);
    tr->translation.setValue(col_wd_lst - gr_mg_sz, -y_offs, 0.0f);
    SoAsciiText* txt = new SoAsciiText();
    txt->justification = SoAsciiText::RIGHT;
    row_sep->addChild(txt);
    txt->string.setValue(el.chars());
    ln->addChild(row_sep);
    col_idx++;
  }
  for (int col = col_range.min; col <= col_range.max; col++) {
    GridColViewSpec* cvs = tvs->colSpec(col);
    if (!cvs || !cvs->isVisible()) continue;
    DataArray_impl* dc = cvs->dataCol();

    //calculate the actual col row index, for the case of a jagged data table
    int act_idx; // <0 for null
    dt->idx(data_row, dc->rows(), act_idx);
    
    // translate the col to proper location
    if (col_wd_lst > 0.0f) { 
      tr = new SoTranslation();
      ln->addChild(tr);
      tr->translation.setValue(col_wd_lst + gr_mg_sz, 0.0f, 0.0f);
    }
    col_wd_lst = col_widths[col_idx++]; // index is now +1 already..

    float col_wd = col_wd_lst - gr_mg_sz2;
    float row_ht = row_height - gr_mg_sz2;
    
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
      float x_offs = .5f * col_wd_lst;
      float y_offs = (.5f * (row_height - text_ht)) + text_ht - txt_base_adj;
      tr->translation.setValue(x_offs, -y_offs, 0.0f);
      continue;
    }

    if (cvs->display_style & GridColViewSpec::BLOCK_MASK) {
      taMatrix* cell_mat =  dc->GetValAsMatrix(act_idx);
      taBase::Ref(cell_mat);
      SoMatrixGrid* sogr = new SoMatrixGrid
	(cell_mat, &scale, (SoMatrixGrid::MatrixLayout)cvs->mat_layout, 
	 cvs->display_style & GridColViewSpec::TEXT_MASK);
      taBase::UnRef(cell_mat);
      ln->addChild(sogr);
      sogr->transform()->scaleFactor.setValue(col_wd, row_ht, 1.0f);
    }

    if(cvs->display_style & GridColViewSpec::TEXT_MASK && !dc->isMatrix()) {
      SoSeparator* txt_sep = new SoSeparator;
      ln->addChild(txt_sep);
      // text origin is bottom left (Left) or bottom right (Right)
      // and we want to center vertically when  height is greater than txt height
      SoAsciiText::Justification just = SoAsciiText::LEFT;
      float x_offs = 0.0f; // default for left
      tr = new SoTranslation;
      txt_sep->addChild(tr);
      float y_offs = ((row_height - text_ht) *.5f) +
	text_ht - txt_base_adj;
      if (dc->isNumeric()) {
	just = SoAsciiText::RIGHT;
	x_offs = col_wd;
	el = Variant::formatNumber(dc->GetValAsVar(act_idx),
				   cvs->num_prec);
      } else {
	int max_chars = (int)(t3Misc::char_ht_to_wd_pts * col_wd / font_scale) + 1;
	el = dc->GetValAsString(act_idx).elidedTo(max_chars);
      }
      tr->translation.setValue(x_offs, -y_offs, 0.0f);
      SoAsciiText* txt = new SoAsciiText();
      txt_sep->addChild(txt);
      txt->justification = just;
      txt->string.setValue(el.chars());
    } //text

    if (cvs->display_style == GridColViewSpec::IMAGE) {
      SoSeparator* img = new SoSeparator;
      ln->addChild(img);
      SoTransform* tr = new SoTransform(); // todo: add this to image object! get rid of extra
      img->addChild(tr);
      // scale the image according to pixel metrics
      //note: image defaults to 1 geom unit width, so we scale by our act width
      tr->scaleFactor.setValue(col_wd, row_ht, 1.0f);
      // center the shape in the middle of the cell
      tr->translation.setValue((col_wd * .5f), -(row_height *.5f), 0.0f);
      SoImageEx* img_so = new SoImageEx;
      img->addChild(img_so);

      taMatrix* cell_mat =  dc->GetValAsMatrix(act_idx);
      taBase::Ref(cell_mat);
      bool top_zero = (cvs->mat_layout == GridColViewSpec::TOP_ZERO);
      img_so->setImage(*cell_mat, top_zero);
      taBase::UnRef(cell_mat);
    }
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

void GridTableView::Render_pre() {
  m_node_so = new T3GridViewNode(this);

  inherited::Render_pre();
}

void GridTableView::Render_impl() {
  inherited::Render_impl();
  T3GridViewNode* node_so = this->node_so(); // cache
  node_so->render();
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
  MakeViewRangeValid();
  CalcViewMetrics();
  if (grid_on) RenderGrid();
  if (header_on) RenderHeader();
  RenderLines();
}

// todo: could probably nuke this -- done by MakeViewRangeValid now
void  GridTableView::ViewC_VisibleAt(int ord_idx) {
  DataTable* dt = dataTable();
  if (ord_idx < 0) ord_idx = 0;
  if (ord_idx >= dt->cols()-col_n) ord_idx = dt->cols()-col_n;
  ViewC_At(ord_idx);
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
  DataTable* dt = lv->dataTable();
  // only show col slider if necessary
  if(lv->col_n >= dt->cols()) {
    t3vs->setHasHorScrollBar(false);
  } else {
    QScrollBar* sb = t3vs->horScrollBar(); // no autocreate
    if (!sb) {
      sb = t3vs->horScrollBar(true);
      connect(sb, SIGNAL(valueChanged(int)), this, SLOT(horScrBar_valueChanged(int)) );
      sb->setTracking(true);
    }
    sb->setMinValue(0);
    sb->setMaxValue(dt->cols()-lv->col_n);
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
  if (lv->view_rows >= lv->rows()) {
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
    int mx = MAX((lv->rows() - lv->view_rows), 0);
    sb->setMaxValue(mx);
    //page step size based on viewable to total lines
    int pg_step = MAX(lv->view_rows, 1);
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


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
#include <Inventor/draggers/SoTransformBoxDragger.h>

#include <Inventor/Qt/viewers/SoQtExaminerViewer.h>

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
  view_rows = 3; //note: set by actual class based on screen
//obs  view_shift = .2f;
  view_range.min = 0;
  view_range.max = -1;
  display_on = true;
  m_lvp = NULL;
  //TODO: new ones should offset pos in viewers so they don't overlap
  table_pos.SetXYZ(1.0f, 0.0f, 0.0f);
  table_scale = 1.0f;
  m_rows = 0;
}

void TableView::InitLinks() {
  inherited::InitLinks();
  taBase::Own(view_range, this);
  taBase::Own(table_pos, this);
  taBase::Own(table_scale, this);
  taBase::Own(table_orient, this);
}

void TableView::CutLinks() {
  view_range.CutLinks();
  if (m_lvp) {
    m_lvp = NULL;
  }
  inherited::CutLinks();
}

void TableView::Copy_(const TableView& cp) {
  view_rows = cp.view_rows;
  view_range = cp.view_range;
  table_pos = cp.table_pos;
  table_scale = cp.table_scale;
  table_orient = cp.table_orient;
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
  UpdateDisplay();
}

void TableView::ClearViewRange() {
  view_range.min = 0;
  view_range.max = -1; // gets adjusted later
}

void TableView::DataDataChanged(taDataLink* dl, int dcr, void* op1, void* op2) {
  // tableview is registered as a datalink client of the datatablespec that it
  // actually owns.  this creates bad situations, when the datatable updates the
  // specs and they update us, and then we depend on them...  so, just don't do
  // anything with our data updates!
}

void TableView::DataChanged_DataTable(int dcr, void* op1, void* op2) {
  // all changes to the data table (either data or struct) get monitored here
  //note: we must ALWAYS make sure m_rows accords with the actual data
  DataTable* dt = dataTable(); // note: be very conservative...
  if (!dt || (dcr == DCR_ITEM_DELETING)) {
    m_rows = 0; // duh!
    InitDisplay(); 
    return;
  }
  
  int delta_rows = dt->rows - m_rows;
  m_rows = dt->rows;
  
  if (!isVisible()) return;

  if(delta_rows > 0) {
    DataChange_NewRows(delta_rows);
  }
  else if (dcr == DCR_STRUCT_UPDATE_END) {
    DataChange_StructUpdate();
  }
  else {
    DataChange_Other();		// always update..
  }
}

void TableView::DataChange_StructUpdate() {
  InitDisplay();
  UpdateDisplay();
}
  
void TableView::DataChange_NewRows(int) {
  // note: overridden in Grid and Graph
  UpdateDisplay();
}
  
void TableView::DataChange_Other() {
  // TEMP, or overridden in Grid
  InitDisplay();
  UpdateDisplay();
}

int TableView::CheckRowsChanged() {
  DataTable* data_table = dataTable();
  int rval = 0;
  if(!data_table) {
    rval = -m_rows;
    m_rows = 0;
    return rval;
  }
  rval = data_table->rows - m_rows;
  m_rows = data_table->rows;
  return rval;
}

void TableView::InitViewSpec() {
  viewSpecBase()->Render();
}

void TableView::InitPanel() {
  if (m_lvp)
    m_lvp->InitPanel();
}

void TableView::UpdatePanel() {
  if (m_lvp)
    m_lvp->UpdatePanel();
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
  InitPanel();
  if(m_lvp && m_lvp->t3vs) {
    m_lvp->t3vs->setSceneGraph(NULL);	// remove existing scene graph
  }
  inherited::Render_pre();
}

void TableView::Render_impl() {
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

void TableView::Render_post() {
  inherited::Render_post();
  if (m_lvp && m_lvp->t3vs) {
    m_lvp->t3vs->setSceneGraph(node_so()); //TODO: change to node_so()->canvas()
    m_lvp->viewAll();
  }
}

void TableView::Reset_impl() {
//   ClearViewRange();
  inherited::Reset_impl();
}

void TableView::setDirty(bool value) {
  inherited::setDirty(value);
  // this is a bad idea: if the view specs themselves are being updated,
  // it can be for many reasons including rebuilding from original data
  // so we should not do anything here..
  //  if (value) UpdateDisplay();
}

void TableView::setDisplay(bool value) {
  if (display_on == value) return;
  display_on = value;
  //  UpdateAfterEdit(); // does the whole kahuna
  UpdateDisplay(false);		// 
}

void TableView::ViewRow_At(int start) {
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

  width	= 1.0f;
  grid_on = true;
  header_on = true;
  row_num_on = true;
  view_rows = 3;
//obs  view_shift = .2f;
}

void GridTableView::InitLinks() {
  inherited::InitLinks();
  taBase::Own(col_range, this);
  taBase::Own(scale, this);
  taBase::Own(view_spec, this);
  SetData(&view_spec);
}

void GridTableView::CutLinks() {
  SetData(NULL);
  view_spec.CutLinks();
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
  scale = cp.scale;
  view_spec = cp.view_spec;
  //metrics calced
}

void GridTableView::Render_pre() {
  m_node_so = new T3GridViewNode(this, width);
  InitViewSpec();

  T3DataViewFrame* frame = GET_MY_OWNER(T3DataViewFrame);
  if(!frame) return;
  SoQtViewer* viewer = frame->widget()->ra();
  viewer->setBackgroundColor(SbColor(scale.background.redf(), scale.background.greenf(), 
				     scale.background.bluef()));
  inherited::Render_pre();
}

void GridTableView::Render_impl() {
  inherited::Render_impl();
  T3GridViewNode* node_so = this->node_so(); // cache
  if(!node_so) return;
  node_so->setWidth(width);	// does a render too -- ensure always up to date on width
  CheckRowsChanged();		// not sure what to do with this but get m_rows anyway
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
  InitViewSpec();
  RemoveLines();
  RemoveHeader();
  RemoveGrid();
  CheckRowsChanged();		// just get the m_rows value
  MakeViewRangeValid();

  if(init_panel) {
    InitPanel();
  }
}

void GridTableView::UpdateDisplay(bool update_panel) {
  if(!node_so()) return;
  if (update_panel) UpdatePanel();
  Render_impl();
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
  float row_font_scale = row_height; // todo: account for margins?  nah..
  
  font_scale = MIN(col_font_scale, row_font_scale);
}

void GridTableView::GetScaleRange() {
  if(!scale.auto_scale) return;
  GridTableViewSpec* tvs = viewSpec();
  MinMax sc_rg;
  tvs->GetMinMaxScale(sc_rg, true);
  scale.SetMinMax(sc_rg.min, sc_rg.max);
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
  UpdateDisplay();
}

void GridTableView::MakeViewRangeValid() {
  inherited::MakeViewRangeValid();
  DataTable* data_table = dataTable();
  int cols = data_table->cols();
  if (col_range.min >= cols) {
    col_range.min = MAX(0, (cols - col_n - 1));
  }
  if(cols == 0) {
    col_range.max = -1;
    return;
  }
  // ensure thate col_range.min is a visible column!
  GridTableViewSpec* tvs = viewSpec();
  GridColViewSpec* min_cvs = tvs->colSpec(col_range.min);
  while((!min_cvs || !min_cvs->isVisible()) && col_range.min > 0) {
    col_range.min--; min_cvs = tvs->colSpec(col_range.min);
  }
  while((!min_cvs || !min_cvs->isVisible()) && col_range.min <= cols-2) {
    col_range.min++; min_cvs = tvs->colSpec(col_range.min);
  }
  int act_n = 0;
  int col;
  for(col = col_range.min; col<cols; ++col) {
    GridColViewSpec* cvs = tvs->colSpec(col);
    if(!cvs || !cvs->isVisible())
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
  //  float gr_mg_sz2 = 2.0f * gr_mg_sz;
  float gr_ln_sz = (grid_on) ? view_spec.grid_line_size * font_scale : 0.0f;
  // margin and baseline adj
  SoTranslation* tr = new SoTranslation();
  hdr->addChild(tr);
  float base_adj = (head_height * t3Misc::char_base_fract);
  //  float base_adj = 0.0f;
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

// todo: support renderValues() for grid??

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
  float txt_base_adj = text_ht * t3Misc::char_base_fract;
//   float txt_base_adj = 0.0f;

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

    if(dc->is_matrix) {
      if(cvs->mat_image) {
	// todo: change image to be separator transform guy
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
      else {
	taMatrix* cell_mat =  dc->GetValAsMatrix(act_idx);
	taBase::Ref(cell_mat);
	SoMatrixGrid* sogr = new SoMatrixGrid
	  (cell_mat, cvs->mat_odd_vert, &scale, (SoMatrixGrid::MatrixLayout)cvs->mat_layout, 
	   tvs->mat_val_text);
	sogr->spacing = tvs->mat_block_spc;
	sogr->block_height = tvs->mat_block_height;
	sogr->trans_max = tvs->mat_trans;
	sogr->render();
	taBase::UnRef(cell_mat);
	ln->addChild(sogr);
	sogr->transform()->scaleFactor.setValue(col_wd, row_ht, 1.0f);
      }
    }
    else {			// scalar: always text
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
	el = Variant::formatNumber(dc->GetValAsVar(act_idx),6); // 6 = precision
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

/////////////////
// note on following: basically callbacks from view, so don't update panel

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
  if (start >= view_spec.col_specs.size) start = view_spec.col_specs.size - 1;
  col_range.min = start;
  UpdateDisplay();
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

  vnd->txfm_shape()->translation.setValue(.5f * (wdth + frmg), .5f * (1.0f + frmg2), 0.0f);

  dragger->translation.setValue(0.0f, 0.0f, 0.0f);
  dragger->rotation.setValue(SbVec3f(0.0f, 0.0f, 1.0f), 0.0f);
  dragger->scaleFactor.setValue(1.0f, 1.0f, 1.0f);

  nv->UpdateDisplay();
}


//////////////////////////
//    iTableView_Panel //
//////////////////////////

iTableView_Panel::iTableView_Panel(TableView* lv)
:inherited(lv)
{
  t3vs = NULL; //these are created in  Constr_T3ViewspaceWidget
  m_ra = NULL;
  m_camera = NULL;
}

iTableView_Panel::~iTableView_Panel() {
}

void iTableView_Panel::Constr_T3ViewspaceWidget(QWidget* widg) {
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

  m_camera->viewAll(root, m_ra->getViewportRegion(), 2.0);
  m_ra->setBackgroundColor(SbColor(0.8f, 0.8f, 0.8f));
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

void iTableView_Panel::viewAll() {
  m_camera->viewAll(t3vs->root_so(), ra()->getViewportRegion(), 2.0); // reduce slack!
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

  chkDisplay = new QCheckBox("Display", widg, "chkDisplay");
  connect(chkDisplay, SIGNAL(toggled(bool)), this, SLOT(chkDisplay_toggled(bool)) );
  layTopCtrls->addWidget(chkDisplay);

  chkHeaders =  new QCheckBox("Hdrs", widg, "chkHeaders");
  connect(chkHeaders, SIGNAL(toggled(bool)), this, SLOT(chkHeaders_toggled(bool)) );
  layTopCtrls->addWidget(chkHeaders);
  layTopCtrls->addStretch();

  butRefresh = new QPushButton("Refresh", widg);
  butRefresh->setFixedHeight(taiM->button_height(taiMisc::sizSmall));
  layTopCtrls->addWidget(butRefresh);
  connect(butRefresh, SIGNAL(pressed()), this, SLOT(butRefresh_pressed()) );

  butClear = new QPushButton("Clear", widg);
  butClear->setFixedHeight(taiM->button_height(taiMisc::sizSmall));
  layTopCtrls->addWidget(butClear);
  connect(butClear, SIGNAL(pressed()), this, SLOT(butClear_pressed()) );

  layVals = new QHBoxLayout(layOuter);

  lblWidth = taiM->NewLabel("Width", widg, font_spec);
  lblWidth->setToolTip("Width of grid log display, in normalized units (default is 1.0 = same as height).");
  layVals->addWidget(lblWidth);

  fldWidth = new taiField(&TA_float, NULL, NULL, widg);
  layVals->addWidget(fldWidth->GetRep());
  layVals->addSpacing(taiM->hsep_c);
  connect(fldWidth->rep(), SIGNAL(editingFinished()), this, SLOT(fldWidth_textChanged()) );

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
  DataTable* dt = glv->dataTable();

  viewAll();

  chkDisplay->setChecked(glv->display_on);
  chkHeaders->setChecked(glv->header_on);

  fldWidth->GetImage((String)glv->width);
  fldRows->GetImage((String)glv->view_rows);
  fldCols->GetImage((String)glv->col_n);

  cbar->UpdateScaleValues();
  chkAutoScale->setChecked(glv->scale.auto_scale);

  // only show col slider if necessary
  if(glv->col_n >= dt->cols()) {
    t3vs->setHasHorScrollBar(false);
  } else {
    QScrollBar* sb = t3vs->horScrollBar(); // no autocreate
    if (!sb) {
      sb = t3vs->horScrollBar(true);
      connect(sb, SIGNAL(valueChanged(int)), this, SLOT(horScrBar_valueChanged(int)) );
      sb->setTracking(true);
    }
    sb->setMinValue(0);
    sb->setMaxValue(dt->cols()-glv->col_n+1);
    int pg_step = 1;
    sb->setSteps(1, pg_step);
    sb->setValue(glv->col_range.min);
  }

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
    sb->setSteps(1, pg_step);
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

void iGridTableView_Panel::fldWidth_textChanged() {
  GridTableView* glv = this->glv(); //cache
  if (updating || !glv) return;

  glv->setWidth((float)fldWidth->GetValue());
  glv->UpdateDisplay(false);
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
}

//////////////////////////////////
//  taiTabularDataMimeFactory	//
//////////////////////////////////

const String taiTabularDataMimeFactory::tacss_matrixdesc("tacss/matrixdesc");
const String taiTabularDataMimeFactory::tacss_tabledesc("tacss/tabledesc");

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
  // TODO: dst ops
}

void taiTabularDataMimeFactory::Mat_EditAction(taMatrix* mat, 
  const CellRange& sel, taiMimeSource* ms, int ea) const
{
  int allowed = 0;
  int forbidden = 0;
  Mat_QueryEditActions(mat, sel, ms, allowed, forbidden);
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

/*
void taiTabularDataMimeFactory::AddSingleObject(QMimeData* md, taBase* obj) {
  if (!obj) return;
  QString str;
  InitHeader(1, str);
  AddHeaderDesc(obj, str);
  md->setData(tacss_objectdesc, StrToByteArray(str));
  AddObjectData(md, obj, 0);
}

void taiTabularDataMimeFactory::AddMultiObjects(QMimeData* md,
   taPtrList_impl* obj_list)
{
  if (!obj_list) return;
  QString str;
  InitHeader(obj_list->size, str);
  // note: prob not necessary, but we iterate twice so header precedes data
  for (int i = 0; i < obj_list->size; ++i) {
    taBase* obj = (taBase*)obj_list->FastEl_(i); // better damn well be a taBase list!!!
    AddHeaderDesc(obj, str);
  }
  md->setData(tacss_objectdesc, StrToByteArray(str));
  for (int i = 0; i < obj_list->size; ++i) {
    taBase* obj = (taBase*)obj_list->FastEl_(i); // better damn well be a taBase list!!!
    AddObjectData(md, obj, i);
  }
}

void taiTabularDataMimeFactory::AddObjectData(QMimeData* md, taBase* obj, int idx) {
    ostringstream ost;
    obj->Save_strm(ost);
    QString str = tacss_objectdata + ";index=" + QString::number(idx);
    md->setData(str, QByteArray(ost.str().c_str()));
}


void taiTabularDataMimeFactory::AddHeaderDesc(taBase* obj, QString& str) {
  //note: can't use Path_Long because path parsing routines don't handle names in paths
  str = str + obj->GetTypeDef()->name.toQString() + ";" +
        obj->GetPath().toQString() + ";\n"; // we put ; at end to facilitate parsing, and for }
}

void taiTabularDataMimeFactory::InitHeader(int cnt, QString& str) {
  str = QString::number(cnt) + ";\n";
}

*/

//////////////////////////////////
//  taiMatDataMimeItem 		//
//////////////////////////////////
/*
taiMatDataMimeItem::taiMatDataMimeItem(int data_type_)
{
  m_data_type = data_type_;
}

bool taiMatDataMimeItem::isMatrix() const {
  return (m_data_type == taiMimeSource::ST_MATRIX_DATA);
}
  
bool taiMatDataMimeItem::isTable() const {
  return (m_data_type == taiMimeSource::ST_TABLE_DATA);
}
  
void taiMatDataMimeItem::GetFormats_impl(QStringList& list, int) const {
  if (isMatrix())
    list.append(taiClipData::tacss_matrixdesc);
  else if (isTable())
    list.append(taiClipData::tacss_tabledesc);
}



//////////////////////////////////
//  taiRcvMatDataMimeItem 	//
//////////////////////////////////

taiRcvMatDataMimeItem::taiRcvMatDataMimeItem(int data_type_)
:inherited(data_type_)
{
  m_cols = 0;
  m_rows = 0;
  m_max_row = 1;
  m_geoms.SetBaseType(&TA_MatrixGeom);
}

void taiRcvMatDataMimeItem::DecodeMatrixDesc(String& arg) {
  // just do it all blind, because supposed to be in correct format
  String tmp = arg.before(';');
  m_cols = tmp.toInt();
  arg = arg.after(';');
  tmp = arg.before(';');
  m_rows = tmp.toInt();
  arg = arg.after(';'); 
}

void taiRcvMatDataMimeItem::DecodeTableDesc(String& arg) {
  DecodeMatrixDesc(arg);
  String tmp;
  for (int i = 0; i < m_cols; ++i) {
    tmp = arg.before(';');
    int col_cols = tmp.toInt();
    arg = arg.after(';');
    tmp = arg.before(';');
    int col_rows = tmp.toInt();
    m_max_row = MAX(m_max_row, col_rows);
    arg = arg.after(';'); 
    MatrixGeom* geom = new MatrixGeom(2, col_cols, col_rows);
    m_geoms.Add(geom);
  }
}

void taiRcvMatDataMimeItem::GetColGeom(int col, int& cols, int& rows) const {
  if (m_data_type == taiMimeSource::ST_MATRIX_DATA) {
    cols = 1;  rows = 1;
  } else {
    MatrixGeom* geom = (MatrixGeom*)m_geoms.SafeEl(col);
    if (geom) {
      cols = geom->SafeEl(0);
      rows = geom->SafeEl(1);
    } else { // bad call!
      cols = 0;  rows = 0;
    }
  }
}

*/

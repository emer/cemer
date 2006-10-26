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

#include "datatable_so.h"

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

//////////////////////////
//  TableView		//
//////////////////////////


// called virtually, after construct
void TableView::InitNew(DataTable* dt, T3DataViewFrame* fr) {
  fr->AddView(this);
  viewSpecBase()->BuildFromDataTable(dt);
  //??
  if (fr->isMapped())
    fr->Render();
}


void TableView::Initialize() {
  data_base = &TA_DataTable; 
//obs  view_bufsz = 100;
  view_bufsz = 0; //note: set by actual class based on screen
//obs  view_shift = .2f;
  view_range.min = 0;
  view_range.max = -1;
  display_toggle = true;
  m_lvp = NULL;
  geom.SetXYZ(4, 1, 3);
  //TODO: new ones should offset pos in viewers so they don't overlap
  pos.SetXYZ(0-geom.x, 0, 0);
  frame_inset = 0.05f;
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
  pos.x++; pos.y--; pos.z++; // outset it from last one
  geom = cp.geom;
  frame_inset = cp.frame_inset;
}

void TableView::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if (!taMisc::gui_active) return;
  InitData();
  SetToggle(display_toggle);
  InitDisplay();
}

int TableView::data_range_max_() const {
  DataTable* data_table =  dataTable();
  if (data_table) return data_table->rows - 1;
  else return 0;
}

void TableView::ClearData() {
  if (dataTable()) {
    dataTable()->ResetData();
  }
  view_range.min = 0;
  view_range.max = -1;
  InitDisplay();
}

void TableView::InitDisplay(){
  // save the display names
/*obs  int mxsz = MIN(viewspec->leaves, log()->display_labels.size);
  int i;
  for(i=0;i<mxsz;i++) {
    DA_ViewSpec* dvs = (DA_ViewSpec*)viewspec->Leaf(i);
    if(dvs->display_name != DA_ViewSpec::CleanName(dvs->name)) {
      log()->display_labels[i] = dvs->display_name;
    }
    else {
      log()->display_labels[i] = "";
    }
  } */
  InitDisplayParams();
}

void TableView::InitData() {
  DataTable* data_table =  dataTable();
  if (!data_table) return;
  viewSpecBase()->BuildFromDataTable(data_table, true);
}

void TableView::InitPanel() {
  if (m_lvp)
    m_lvp->InitPanel();
}

void TableView::NewHead() {
  DataTable* data_table = dataTable();
  if (data_table) {
    viewSpecBase()->BuildFromDataTable(data_table, true);
  }
  //  UpdateDispLabels();
}

void TableView::Render_pre() {
  if (!m_node_so.ptr()) return; // shouldn't happen

  // release any previous incarnation of graph in panel
  if (m_lvp && m_lvp->t3vs) {
    m_lvp->t3vs->setSceneGraph(NULL);
  }

  T3TableViewNode* node_so = this->node_so(); //cache
  node_so->setShowFrame(true);
  node_so->frame()->inset = frame_inset;
  const iColor* col = GetEditColorInherit();
  if (col) {
    col->copyTo(node_so->material()->diffuseColor);
  }

  inherited::Render_pre();
  InitDisplayParams();
}

void TableView::Render_impl() {
  // set origin: in allocated area
  FloatTransform* ft = transform(true);
  ft->translate.SetXYZ(pos.x, (pos.z + 0.5f), -pos.y);

  T3TableViewNode* node_so = this->node_so(); // cache
  node_so->setGeom(geom.x, geom.y, geom.z);
  SoFont* font = node_so->captionFont(true);
  float font_size = 0.4f;
  font->size.setValue(font_size); // is in same units as geometry units of network
  node_so->setCaption(dataTable()->GetName().chars());
  node_so->transformCaption(iVec3f(0.0f, -font_size, 0.0f)); //move caption below the frame

  inherited::Render_impl();
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

int TableView::rows() const {
//TODO: optimize this into an inline by keeping our own copy of rows
  DataTable* data_table =  dataTable();
  if (data_table) return data_table->rows;
  else return 0;
}

void TableView::SetToggle(bool value){
  display_toggle = value;
//TODO: update display  if(disp_tog_but) {
//    disp_tog_but->setDown(value);
//  }
}

void TableView::ToggleDisplay() {
  display_toggle = !display_toggle;
//TODO:  if(disp_tog_but != NULL) {
//    disp_tog_but->setDown(display_toggle);
//  }
}

void TableView::UpdateFromBuffer() {
  if (!display_toggle)
    return;
  UpdateFromBuffer_impl();
  if (m_lvp)
    m_lvp->BufferUpdated();
}

void TableView::UpdateFromBuffer_impl() {
}

void TableView::UpdateDisplay(TAPtr){
//TODO:fixup  inherited::UpdateDisplay();
}

void TableView::View_At(int start) {
  if (!taMisc::gui_active) return;
  int rows = this->rows();
  if (start >= rows)
    start = rows - 1;
  if (start < 0)
    start = 0;

  view_range.min = start;
  view_range.max = view_range.min + view_bufsz -1; // always keep min and max valid
  view_range.MaxLT(rows - 1); // keep it less than max
  UpdateFromBuffer();
}

void TableView::View_FF() {
  if(!taMisc::gui_active) return;
  int rows = this->rows();
  View_At(rows - view_bufsz);
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

GridTableView* GridTableView::NewGridTableView(DataTable* dt,
    T3DataViewFrame* fr)
{
  if (!dt) return NULL;
  GridTableView* rval = new GridTableView;
  rval->InitNew(dt, fr);
  return rval;
}

void GridTableView::Initialize() {
  col_bufsz = 0;
  row_height = 0.1f; // non-zero dummy value
  tot_col_widths = 0.00001f; //avoid /0
  
//from GTV
  block_size = 8;
  block_border_size = 1;
  header_on = true;
  auto_scale = false;
  scale_range.min = -1.0f;
  scale_range.max = 1.0f;
  view_bufsz = 3;
//obs  view_shift = .2f;

//TODO  editor = NULL;
  head_tog_but = NULL;
  auto_sc_but = NULL;
//REDO  SetAdapter(new GridTableViewAdapter(this));

}

void GridTableView::InitLinks() {
  inherited::InitLinks();
  taBase::Own(col_range, this);
  taBase::Own(view_font,this);
  taBase::Own(scale_range,this);
  taBase::Own(actual_range,this);
  taBase::Own(view_spec, this);
}

void GridTableView::CutLinks() {
  view_spec.CutLinks();
  actual_range.CutLinks();
  scale_range.CutLinks();
  view_font.CutLinks();
  colorspec = NULL; // unlink
  col_range.CutLinks();
  inherited::CutLinks();
}

void GridTableView::Copy_(const GridTableView& cp) {
  col_bufsz = cp.col_bufsz;
  col_range = cp.col_range;
  
  block_size = cp.block_size;
  block_border_size = cp.block_border_size;
  header_on = cp.header_on;
  auto_scale = cp.auto_scale;
  scale_range = cp.scale_range;
  colorspec = cp.colorspec;
  view_font = cp.view_font;
  actual_range = cp.actual_range;
  view_spec = cp.view_spec;
  
}

void GridTableView::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  InitDisplay();
  AdjustColView();
}

void GridTableView::AdjustColView() {
  col_range.max = col_range.min - 1;
  if (col_bufsz == 0) return;
  float wd_tot = 0.0f;
  while ((col_range.max < (col_bufsz - 1)) && 
   ((wd_tot += colWidth(col_range.max + 1)) <= (float)geom.x))
    col_range.max++;
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

void GridTableView::Clear_impl() {
//  if (!taMisc::gui_active ) return;
  view_range.max = -1;
  view_range.min = 0;
  T3GridTableViewNode* node_so = this->node_so();
  if (node_so) {
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

void GridTableView::InitDisplayParams() {
  InitHead();
  float tmp = (header_on) ? geom.z : geom.z - head_height;
  view_bufsz = MAX((int)(geom.z / tmp), 1);
  InitPanel();
}

void GridTableView::InitHead() {
  GridTableViewSpec* tvs = viewSpec();
  head_height = tvs->font.pointSize / t3Misc::pts_per_so_unit;
  // col widths, number of col_bufsz, row height
  col_bufsz = 0;
  row_height = 0.0f;
  tot_col_widths = 0.00001f; // avoid /0
  taListItr itr;
  GridColViewSpec* vs;
  FOR_ITR_EL(GridColViewSpec, vs, tvs->col_specs., itr) {
    if ((!vs->visible) || (!vs->dataCol())) continue;
    ++col_bufsz;
    tot_col_widths += vs->col_width;
    row_height = MAX(row_height, vs->row_height);
  }
  AdjustColView();
}

void GridTableView::NewData() {
  if (!isMapped() || !display_toggle) return;
  inherited::NewData();

  // if we are not at the very end, then don't scroll, but do update the panel
  if ((view_range.max < data_range_max_()-1) && (view_range.max > 0)) {
    if (m_lvp)
      m_lvp->BufferUpdated();
    return;
  }

  if ((view_range.max - view_range.min + 1) >= view_bufsz) {
    view_range.min++;
  }
  view_range.max++; //must update before calling AddLine
  UpdateFromBuffer();
}

void GridTableView::NewHead() {
  if(!isMapped() || !display_toggle) return;
  inherited::NewHead();
  T3GridTableViewNode* node_so = this->node_so();
  if (node_so) {
    node_so->header()->removeAllChildren();
  }
  InitHead();
  RenderHead();
}

void GridTableView::OnWindowBind_impl(iT3DataViewFrame* vw) {
  inherited::OnWindowBind_impl(vw);
  if (!m_lvp) {
    m_lvp = new iGridTableView_Panel(this);
    vw->viewerWindow()->AddPanelNewTab(lvp());
  }
}

void GridTableView::RemoveLine(int index){
  T3GridTableViewNode* node_so = this->node_so();
  if (!node_so) return;
  if (index == -1) {
    node_so->body()->removeAllChildren();
  } else {
    if (index <= (node_so->body()->getNumChildren() - 1))
      node_so->body()->removeChild(index);
  }
}

void GridTableView::RenderHead() {
  //note: only called when node exists
  T3GridTableViewNode* node_so = this->node_so();
  GridTableViewSpec* tvs = viewSpec();
  // make header
  SoSeparator* hdr = new SoSeparator();
  SoFont* fnt = new SoFont();
  FontSpec fs(tvs->font);
  fs.setBold(true);
  fs.copyTo(fnt);
  hdr->addChild(fnt);

  taListItr itr;
  int col = 0;
  GridColViewSpec* vs;
  FOR_ITR_EL(GridColViewSpec, vs, tvs->col_specs., itr) {
    if ((!vs->visible) || (!vs->dataCol())) continue;
    if (col < col_range.min) { ++col; continue;}
    if (col > col_range.max) break;
    SoTranslation* tr = new SoTranslation();
    hdr->addChild(tr);
    if (col == col_range.min) { // first trnsl positions row
      tr->translation.setValue(0.0f, geom.z - row_height, 0.0f);
    } else {
      tr->translation.setValue(colWidth(col - 1), 0.0f, 0.0f);
    }
    SoAsciiText* txt = new SoAsciiText();
    hdr->addChild(txt);
    txt->string.setValue(vs->display_name.chars());
    ++col;
  }
  node_so->header()->addChild(hdr);
}

void GridTableView::Render_pre() {
  m_node_so = new T3GridTableViewNode(this);

  inherited::Render_pre();
}

void GridTableView::Render_post() {
  if (header_on) RenderHead();
  UpdateFromBuffer();
  inherited::Render_post();
}

void GridTableView::Reset_impl() {
  col_range.min = 0;
  inherited::Reset_impl();
}

void GridTableView::SetBlockSizes(int block_sz, int border_sz) {
  block_size = block_sz;
  block_border_size = border_sz;
  UpdateAfterEdit();
}

void GridTableView::SetColorSpec(ColorScaleSpec* colors) {
  colorspec = colors;
  UpdateAfterEdit();
}

void GridTableView::SetViewFontSize(int point_size) {
  view_font.SetFontSize(point_size);
  UpdateAfterEdit();
}

void GridTableView::ToggleAutoScale() {
  auto_scale = !auto_scale;
//TODO  if(editor != NULL) editor->auto_scale = auto_scale;
  if (auto_sc_but) {
    auto_sc_but->setDown(auto_scale);
    InitDisplay();
  }
}

void GridTableView::ToggleHeader() {
  header_on = !header_on;
//TODO  if(editor != NULL) editor->header_on = header_on;
  if (head_tog_but) {
    head_tog_but->setDown(header_on);
    InitDisplay();
  }
}

void GridTableView::SetBlockFill(
  GridColViewSpec::BlockColor color,
  GridColViewSpec::BlockFill fill)
{
//TODO: iterate, setting for all applicable col_bufsz
  InitDisplay();
}


void GridTableView::UpdateFromBuffer_impl(){
  // this updates the data area
  T3GridTableViewNode* node_so = this->node_so();
  if (!node_so) return;
  node_so->body()->removeAllChildren();

  // master font -- we only add a child font if different
  SoFont* fnt = new SoFont();
  FontSpec& fs = viewSpec()->font;
  fs.copyTo(fnt);
  node_so->body()->addChild(fnt);

  // this is the index (in view_range units) of the first view line of data in the buffer
  int row = 0;
  for (int ln = view_range.min; ln <= view_range.max; ln++) {
    UpdateFromBuffer_AddLine(row, ln);
    ++row;
  }
}

void GridTableView::UpdateFromBuffer_AddLine(int row, int buff_idx){
  T3GridTableViewNode* node_so = this->node_so();
  if (!node_so) return;
  GridTableViewSpec* tvs = viewSpec();

  // make line container
  SoSeparator* ln = new SoSeparator();

  taListItr itr;
  int col = 0;
  GridColViewSpec* vs;
  DataTable* dt = dataTable(); //cache
  String el;
  FOR_ITR_EL(GridColViewSpec, vs, tvs->col_specs., itr) {
    DataArray_impl* data_array = vs->dataCol();
    if ((!vs->visible) || (!data_array)) continue;
    if (col < col_range.min) {++col; continue;}
    if (col > col_range.max) break;
    SoTranslation* tr = new SoTranslation();
    ln->addChild(tr);
    if (col == col_range.min) { // translation is relative to 0,0
      tr->translation.setValue(0.0f, geom.z - (row_height * (row + 2)), 0.0f);
    } else {
      tr->translation.setValue(colWidth(col - 1), 0.0f, 0.0f);
    }
    if (vs->display_style & GridColViewSpec::TEXT_MASK) {
    //TODO: add a font, if col font differs from main font
      int act_idx;
      //calculate the actual row index, for the case of a jagged data tables
      if (dt->idx(buff_idx, data_array->rows(), act_idx)) {
        el = data_array->GetValAsString(act_idx);
      } else {
        el = "n/a";
      }
      SoAsciiText* txt = new SoAsciiText();
      ln->addChild(txt);
      txt->string.setValue(el.chars());
    }
    //TODO: grid stuff
    ++col;
  }
  node_so->body()->addChild(ln);
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
  if (start >= col_bufsz) start = col_bufsz - 1;
  if (col_range.min == start) return;
  col_range.min = start;
  AdjustColView();
  InitDisplay();
  Render();
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
  
  QWidget* widg = new QWidget();
  layOuter = new QVBoxLayout(widg);

  layTopCtrls = new QHBoxLayout(layOuter);

// //   layDispCheck = new QHBoxLayout(layTopCtrls);
  chkDisplay = new QCheckBox("Display", this, "chkDisplay");
  layTopCtrls->addWidget(chkDisplay);

  if (is_grid_log) {
    chkAuto =  new QCheckBox("Auto", this, "chkAuto");
    layTopCtrls->addWidget(chkAuto);
    chkHeaders =  new QCheckBox("Hdrs", this, "chkHeaders");
    layTopCtrls->addWidget(chkHeaders);
  } else {
    chkAuto = NULL;
    chkHeaders = NULL;
  }
  layTopCtrls->addStretch();


  layVcrButtons = new QHBoxLayout(layTopCtrls);
  bgpTopButtons = new QButtonGroup(this); // NOTE: not a widget
  bgpTopButtons->setExclusive(false); // not applicable
  int but_ht = taiM->button_height(taiMisc::sizSmall);
  for (int i = BUT_BEG_ID; i <= BUT_END_ID; ++i) {
    QPushButton* pb = new QPushButton(this);
    pb->setText(but_strs[i]);
    pb->setMaximumHeight(but_ht);
    pb->setMaximumWidth(20);
    layVcrButtons->addWidget(pb);
    bgpTopButtons->addButton(pb, i);
  }
  layTopCtrls->addStretch();

  layInitButtons = new QHBoxLayout(layTopCtrls);
  for (int i = BUT_UPDATE; i <= BUT_CLEAR; ++i) {
    QPushButton* pb = new QPushButton(this);
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
    break;
  case BUT_INIT:
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

  lv->display_toggle = on;
}

void iTableView_Panel::Constr_T3ViewspaceWidget() {
  t3vs = new iT3ViewspaceWidget(this);
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


void iTableView_Panel::DataChanged_impl(int dcr, void* op1_, void* op2_) {
  inherited::DataChanged_impl(dcr, op1_, op2_);
}
/*TODO
int iLogDataPanel::EditAction(int ea) {
  int rval = 0;

  ISelectable_PtrList sel_list;
  GetSelectedItems(sel_list);
  ISelectable* ci = sel_list.SafeEl(0);
  if (ci)  {
    rval = ci->EditAction_(sel_list, ea);
  }
  return rval;
}

int iLogDataPanel::GetEditActions() {
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
} */

void iTableView_Panel::GetImage_impl() {
  TableView* lv = this->lv(); // cache
  chkDisplay->setChecked(lv->display_toggle);
}

/* void iLogDataPanel::GetSelectedItems(ISelectable_PtrList& lst) {
  QListViewItemIterator it(list, QListViewItemIterator::Selected);
  while (it.current()) {
    lst.Add((taiListDataNode*)it.current());
    ++it;
  }
}

void iLogDataPanel::list_contextMenuRequested(QListViewItem* item, const QPoint & pos, int col ) {
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

void iLogDataPanel::list_selectionChanged() {
  viewer_win()->UpdateUi();
} */

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
  ++updating;
  // only show col slider if necessary
  if (lv->tot_col_widths < (float)lv->geom.x) {
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
    //we make a crude estimate of page step based on ratio of act width to tot width
    int pg_step = (int)(((lv->geom.x / lv->tot_col_widths) * lv->col_bufsz) + 0.5f);
    if (pg_step == 0) pg_step = 1;
    sb->setSteps(1, pg_step);
  }
  //TODO: BufferUpdated()???
  --updating;
}

void iGridTableView_Panel::BufferUpdated() {
  ++updating;
  inherited::BufferUpdated();
  GridTableView* lv = this->glv(); //cache
  // only show row slider if necessary
  if (lv->view_bufsz >= lv->rows()) {
    t3vs->setHasVerScrollBar(false);
  } else {
    QScrollBar* sb = t3vs->verScrollBar(); // no autocreate
    if (!sb) {
      sb = t3vs->verScrollBar(true);
      connect(sb, SIGNAL(valueChanged(int)), this, SLOT(verScrBar_valueChanged(int)) );
      sb->setTracking(true);
    }
    sb->setMinValue(0);
    //note: the max val is set so that the last page is a full page (i.e., can't scroll past end)
    int mx = MAX((lv->rows() - lv->view_bufsz), 0);
    sb->setMaxValue(mx);
    //page step size based on viewable to total lines
    int pg_step = MAX(lv->view_bufsz, 1);
    sb->setSteps(1, pg_step);
    sb->setValue(MIN(lv->view_range.min, mx));
  }
  --updating;
}

void iGridTableView_Panel::horScrBar_valueChanged(int value) {
  GridTableView* glv = this->glv(); //cache
  if (updating || !glv) return;
  glv->ViewC_At(value);
}

void iGridTableView_Panel::verScrBar_valueChanged(int value) {
  GridTableView* glv = this->glv(); //cache
  if (updating || !glv) return;
  glv->View_At(value);
}

void iGridTableView_Panel::chkAuto_toggled(bool on) {
}
void iGridTableView_Panel::chkHeaders_toggled(bool on) {
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
  inherited::CreateDataPanel_impl(dl_);
  iDataTablePanel* dp = new iDataTablePanel(dl_);
  DataPanelCreated(dp);
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
  dte = new iDataTableEditor();
  setCentralWidget(dte);
  
  dte->setDataTable(dt());
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


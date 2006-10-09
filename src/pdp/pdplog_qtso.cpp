// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/PDP++
//
//   TA/PDP++ is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   TA/PDP++ is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.



// stuff to implement pdplog_view graphics

#include "ta_qt.h"

#include "datagraph_qtso.h"

//#include "pdp_qtso.h"
#include "pdplog_qtso.h"
#include "netstru_so.h" // TEMP
//#include "pdpshell.h"

#include "icolor.h"

#include <QButtonGroup>
#include <qcheckbox.h>
#include <qclipboard.h>
#include <qimage.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qpixmap.h>
#include <QPushButton>
#include <Q3ScrollView>
#include <Q3VBox>

#include <Inventor/SbLinear.h>
#include <Inventor/Qt/SoQt.h>
#include <Inventor/Qt/SoQtRenderArea.h>
#include <Inventor/fields/SoMFString.h>
#include <Inventor/nodes/SoAsciiText.h>
#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoCone.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoCylinder.h>
#include <Inventor/nodes/SoDirectionalLight.h>
#include <Inventor/nodes/SoFont.h>
#include <Inventor/nodes/SoLightModel.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/nodes/SoSelection.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoTranslation.h>

#include <time.h>		// for animate

//////////////////////////
//  TableView		//
//////////////////////////

// TEMP: conversion properties
int TableView::virt_lines_() const {
  DataTable* dt = data();
  if (dt) return dt->rows;
  else return 0;
}
int TableView::data_range_min_() const {
  return 0;
}
int TableView::data_range_max_() const {
  DataTable* dt = data();
  if (dt) return dt->rows - 1;
  else return 0;
}
// END conversion properties

void TableView::Initialize() {
  data_base = &TA_DataTable;
//obs  view_bufsz = 100;
  view_bufsz = 0; //note: set by actual class based on screen
//obs  view_shift = .2f;
  view_range.min = 0;
  view_range.max = -1;
  viewspec = NULL;
  display_toggle = true;
  m_lvp = NULL;
  geom.SetXYZ(4, 1, 3);
  //TODO: new ones should offset pos in viewers so they don't overlap
  pos.SetXYZ(0-geom.x, 0, 0);
  frame_inset = 0.05f;
  own_data = false;

  SetAdapter(new TableViewAdapter(this));
}

// called virtually, after construct
void TableView::InitNew(DataTable* dt, T3DataViewFrame* fr) {
  SetData(dt);
  fr->AddView(this);
  //??
  if (fr->isMapped())
    fr->Render();
}


void TableView::InitLinks() {
  inherited::InitLinks();
  taBase::Own(view_range, this);
  taBase::Own(viewspecs,this);
  taBase::Own(pos,this);
  taBase::Own(geom,this);

  if (!taMisc::is_loading) {
    CreateViewSpec();
    viewspec->BuildFromDataTable(data());
  }
}

void TableView::CutLinks() {
  taBase::DelPointer((TAPtr*)&viewspec);
  geom.CutLinks();
  pos.CutLinks();
  viewspecs.CutLinks();
  view_range.CutLinks();
  if (m_lvp) {
    m_lvp = NULL;
  }
  // if we own the data, have to delete it now because inherited will disconnect us
  if (own_data && data()) {
    delete data(); // will null our pointer
  }
  inherited::CutLinks();
}

void TableView::Copy_(const TableView& cp) {
  view_bufsz = cp.view_bufsz;
//obs  view_shift = cp.view_shift;
  view_range = cp.view_range;
  viewspecs = cp.viewspecs;
  pos = cp.pos;
  pos.x++; pos.y--; pos.z++; // outset it from last one
  geom = cp.geom;
  frame_inset = cp.frame_inset;
  //TODO: we will need to fixup the ref to the data
  own_data = cp.own_data;
}

void TableView::UpdateAfterEdit() {
  inherited::UpdateAfterEdit();
  if (own_data && data()) UpdateOwnedData();
  if(!taMisc::gui_active) return;
  SetToggle(display_toggle);
  InitDisplay();
}

void TableView::UpdateOwnedData() {
  DataTable* dt = data();
  if (!dt) return;
  dt->SetName(GetName()); // mirror our name
}

void TableView::ClearData() {
  if (data()) {
    data()->ResetData();
  }
  view_range.min = 0;
  view_range.max = -1;
  InitDisplay();
}

TypeDef* TableView::DT_ViewSpecType() {
  return &TA_DT_ViewSpec;
}

TypeDef* TableView::DA_ViewSpecType() {
  return &TA_DA_ViewSpec;
}

/*void TableView::CloseWindow() {
  if (!isMapped()) return;
  inherited::CloseWindow();
} */

void TableView::CreateViewSpec() {
  if (viewspecs.size == 0) {
    viewspecs.New(1, DT_ViewSpecType());
    taBase::DelPointer((TAPtr*)&viewspec); // this must be old if it exists
  }
  if (viewspec == NULL) {
    taBase::SetPointer((TAPtr*)&viewspec, viewspecs.FastEl(0));
    viewspec->SetBaseType(DA_ViewSpecType());
  }
}

void TableView::EditViewSpec(taBase* spec){
  if(spec==NULL) return;
  spec->Edit();
}

QWidget* TableView::GetPrintData() {
return NULL; //temp //TODO  return win_box;
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

void TableView::InitPanel() {
  if (m_lvp)
    m_lvp->InitPanel();
}

void TableView::NewHead() {
  if(data() && (viewspec != NULL)) {
    if (!viewspec->BuildFromDataTable()) {
      viewspec->ReBuildFromDataTable();	// make sure it rebuilds for sure!!
    }
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
  node_so->setCaption(data()->GetName().chars());
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

void TableView::SetVisibility(taBase* spec, bool vis) {
  if(spec==NULL) return;
  if(spec->InheritsFrom(&TA_DA_ViewSpec)) {
    DA_ViewSpec* davs = (DA_ViewSpec*)spec;
    davs->visible = vis;
    davs->UpdateAfterEdit();
    davs->UpdateView();
  }
  else if(spec->InheritsFrom(&TA_DT_ViewSpec)) {
    DT_ViewSpec* dtvs = (DT_ViewSpec*)spec;
    dtvs->SetVisibility(vis);
  }
}

void TableView::SetLogging(taBase* spec, bool log_data, bool also_chg_vis) {
  if(spec==NULL) return;
  if(spec->InheritsFrom(&TA_DA_ViewSpec)) {
    DA_ViewSpec* davs = (DA_ViewSpec*)spec;
    if(davs->data_array == NULL) return;
    davs->data_array->save_to_file = log_data;
    if(also_chg_vis) {
      davs->visible = log_data;
      davs->UpdateAfterEdit();
      davs->UpdateView();
    }
  }
  else if(spec->InheritsFrom(&TA_DT_ViewSpec)) {
    DT_ViewSpec* dtvs = (DT_ViewSpec*)spec;
    if(dtvs->data_table == NULL) return;
    dtvs->data_table->SetSaveToFile(log_data);
    if(also_chg_vis)
      dtvs->SetVisibility(log_data);
  }
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

/*nn void TableView::UpdateDispLabels() {
  if((log() == NULL) || (viewspec == NULL)) return;
  // restore the display names
  int mxsz = MIN(viewspec->leaves, log()->display_labels.size);
  int i;
  for(i=0;i<mxsz;i++) {
    DA_ViewSpec* dvs = (DA_ViewSpec*)viewspec->Leaf(i);
    String dnm = log()->display_labels[i];
    if(!dnm.empty())
      dvs->display_name = dnm;
  }
}*/

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
  if(!taMisc::gui_active) return;
  int virt_lines = virt_lines_();
  if (start >= virt_lines)
    start = virt_lines - 1;
  if (start < 0)
    start = 0;

  view_range.min = start;
  view_range.max = view_range.min + view_bufsz -1; // always keep min and max valid
  int log_max = MAX(virt_lines - 1, data_range_max_());
  view_range.MaxLT(log_max); // keep it less than max
//obs  log()->Buffer_SeekForView(view_range);
  view_range.MinGT(data_range_min_()); // redo now that data_range.min is final
  view_range.max = view_range.min + view_bufsz -1;
  view_range.MaxLT(data_range_max_());
  UpdateFromBuffer();
}

void TableView::View_FF() {
  if(!taMisc::gui_active) return;
  int virt_lines = virt_lines_();
  View_At(virt_lines - view_bufsz);
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

//////////////////////////////////
// 	Analysis Routines 	//
//////////////////////////////////

void TableView::LogUpdateAfterEdit() {
  if (data() && viewspec && (viewspec->leaves != data()->cols()))
    NewHead();
  //TODO: following is legacy, and may be obsolete
//      NotifyAllUpdaters();	// make sure logs are getting it from  us..
}
/*TODO 
void TableView::DistMatrixGrid(taBase* data, taBase* labels, GridLog* disp_log,
			     float_RArray::DistMetric metric, bool norm, float tol) {
  if(!data->InheritsFrom(&TA_DT_ViewSpec)){
    taMisc::Error("DistMatrixGrid: use of single data of data not supported -- please select a vector (group) of data");
    return;
  }
  if(!labels->InheritsFrom(&TA_DA_ViewSpec)){
    taMisc::Error("DistMatrixGrid: labels must be a single column in the log, not a group");
    return;
  }
  Environment* env = pdpMisc::GetNewEnv(GET_MY_OWNER(ProjectBase), &TA_Environment);
  if(env == NULL) return;
  CopyToEnv(data, labels, env);
  env->DistMatrixGrid(disp_log, 0, metric, norm, tol);
  tabMisc::Close_Obj(env);
}

void TableView::ClusterPlot(taBase* data, taBase* labels, GraphLog* disp_log,
			  float_RArray::DistMetric metric, bool norm, float tol) {
  if(!data->InheritsFrom(&TA_DT_ViewSpec)) {
    taMisc::Error("ClusterPlot: use of single data of data not supported -- please select a vector (group) of data");
    return;
  }
  if(!labels->InheritsFrom(&TA_DA_ViewSpec)) {
    taMisc::Error("ClusterPlot: labels must be a single column in the log, not a group");
    return;
  }
  Environment* env = pdpMisc::GetNewEnv(GET_MY_OWNER(ProjectBase), &TA_Environment);
  if(env == NULL) return;
  CopyToEnv(data, labels, env);
  env->ClusterPlot(disp_log, 0, metric, norm, tol);
  tabMisc::Close_Obj(env);
}

void TableView::CorrelMatrixGrid(taBase* data, taBase* labels, GridLog* disp_log) {
  if(!data->InheritsFrom(&TA_DT_ViewSpec)) {
    taMisc::Error("CorrelMatrixGrid: use of single data of data not supported -- please select a vector (group) of data");
    return;
  }
  if(!labels->InheritsFrom(&TA_DA_ViewSpec)) {
    taMisc::Error("CorrelMatrixGrid: labels must be a single column in the log, not a group");
    return;
  }
  Environment* env = pdpMisc::GetNewEnv(GET_MY_OWNER(ProjectBase), &TA_Environment);
  if(env == NULL) return;
  CopyToEnv(data, labels, env);
  env->CorrelMatrixGrid(disp_log, 0);
  tabMisc::Close_Obj(env);
}

void TableView::PCAEigenGrid(taBase* data, taBase* labels, GridLog* disp_log) {
  if(!data->InheritsFrom(&TA_DT_ViewSpec)){
    taMisc::Error("PCAEigenGrid: use of single data of data not supported -- please select a vector (group) of data");
    return;
  }
  if(!labels->InheritsFrom(&TA_DA_ViewSpec)){
    taMisc::Error("PCAEigenGrid: labels must be a single column in the log, not a group");
    return;
  }
  Environment* env = pdpMisc::GetNewEnv(GET_MY_OWNER(ProjectBase), &TA_Environment);
  if(env == NULL) return;
  CopyToEnv(data, labels, env);
  env->PCAEigenGrid(disp_log, 0);
  tabMisc::Close_Obj(env);
}

void TableView::PCAPrjnPlot(taBase* data, taBase* labels, GraphLog* disp_log, int x_axis_component, int y_axis_component) {
  if(!data->InheritsFrom(&TA_DT_ViewSpec)){
    taMisc::Error("PCAPrjnPlot: use of single data of data not supported -- please select a vector (group) of data");
    return;
  }
  if(!labels->InheritsFrom(&TA_DA_ViewSpec)){
    taMisc::Error("PCAPrjnPlot: labels must be a single column in the log, not a group");
    return;
  }
  Environment* env = pdpMisc::GetNewEnv(GET_MY_OWNER(ProjectBase), &TA_Environment);
  if(env == NULL) return;
  CopyToEnv(data, labels, env);
  env->PCAPrjnPlot(disp_log, 0, x_axis_component, y_axis_component);
  tabMisc::Close_Obj(env);
}

void TableView::MDSPrjnPlot(taBase* data, taBase* labels, GraphLog* disp_log, int x_axis_component, int y_axis_component) {
  if(!data->InheritsFrom(&TA_DT_ViewSpec)){
    taMisc::Error("MDSPrjnPlot: use of single data of data not supported -- please select a vector (group) of data");
    return;
  }
  if(!labels->InheritsFrom(&TA_DA_ViewSpec)){
    taMisc::Error("MDSPrjnPlot: labels must be a single column in the log, not a group");
    return;
  }
  Environment* env = pdpMisc::GetNewEnv(GET_MY_OWNER(ProjectBase), &TA_Environment);
  if(env == NULL) return;
  CopyToEnv(data, labels, env);
  env->MDSPrjnPlot(disp_log, 0, x_axis_component, y_axis_component);
  tabMisc::Close_Obj(env);
}
*/
//////////////////////////
// 	GridTableViewBase	//
//////////////////////////

void GridTableViewBase::Initialize() {
  cols = 0;
  row_height = 0.1f; // non-zero dummy value
  tot_col_widths = 0.00001f; //avoid /0
}

void GridTableViewBase::InitLinks() {
  inherited::InitLinks();
  taBase::Own(col_range, this);
}

void GridTableViewBase::CutLinks() {
  col_range.CutLinks();
  inherited::CutLinks();
}

void GridTableViewBase::Copy_(const GridTableViewBase& cp) {
  cols = cp.cols;
  col_range = cp.col_range;
  col_widths = cp.col_widths;
}

void GridTableViewBase::UpdateAfterEdit() {
  AdjustColView();
  inherited::UpdateAfterEdit();
}

void GridTableViewBase::Clear_impl() {
//  if (!taMisc::gui_active ) return;
  view_range.max = -1;
  view_range.min = 0;
  T3GridTableViewBaseNode* node_so = this->node_so();
  if (node_so) {
    node_so->header()->removeAllChildren();
    node_so->body()->removeAllChildren();
  }
  inherited::Clear_impl();
}

void GridTableViewBase::NewData() {
  if (!isMapped() || !display_toggle) return;

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

void GridTableViewBase::NewHead() {
  if(!isMapped() || !display_toggle) return;
  inherited::NewHead();
  T3GridTableViewBaseNode* node_so = this->node_so();
  if (node_so) {
    node_so->header()->removeAllChildren();
  }
  InitHead();
  RenderHead();
}

void GridTableViewBase::Render_pre() {
  m_node_so = new T3GridTableViewBaseNode(this);

  inherited::Render_pre();
}

void GridTableViewBase::Render_post() {
  RenderHead();
  UpdateFromBuffer();
  inherited::Render_post();
}

void GridTableViewBase::Reset_impl() {
  col_range.min = 0;
  inherited::Reset_impl();
}

void GridTableViewBase::VScroll(bool left) {
  if (left) {
    ViewC_At(col_range.min - 1);
  } else {
    ViewC_At(col_range.min + 1);
  }
}

void GridTableViewBase::ViewC_At(int start) {
  if (start < 0) start = 0;
  if (start >= cols) start = cols - 1;
  if (col_range.min == start) return;
  col_range.min = start;
  AdjustColView();
  InitDisplay();
  Render();
}


//////////////////////////
// 	TextTableView	//
//////////////////////////

void TextTableView::Initialize() {
}

void TextTableView::InitLinks() {
  inherited::InitLinks();
}

void TextTableView::CutLinks() {
  inherited::CutLinks();
}

void TextTableView::UpdateAfterEdit() {
  TableView::UpdateAfterEdit();
  InitDisplay();
}

void TextTableView::AdjustColView() {
  col_range.max = col_range.min - 1;
  if (cols == 0) return;
  float wd_tot = 0.0f;
  while ((col_range.max < (cols - 1)) && ((wd_tot += col_widths[col_range.max + 1]) <= (float)geom.x))
    col_range.max++;
}

TypeDef* TextTableView::DT_ViewSpecType() {
  return &TA_DT_ViewSpec;
}

TypeDef* TextTableView::DA_ViewSpecType() {
  return &TA_DA_TextViewSpec;
}

QWidget* TextTableView::GetPrintData(){
return NULL; /*TODO  if(vbox == NULL) return TableView::GetPrintData();
  return patch; */
}

void TextTableView::InitDisplay() {
  if (!(isMapped() && taMisc::gui_active)) return;
  TableView::InitDisplay(); //clears
/*nn  T3GridTableViewBaseNode* node_so = this->node_so();
  node_so->body()->removeAllChildren();

  NewHead();

  UpdateDisplay(); */
}

void TextTableView::InitDisplayParams() {
  DT_ViewSpec* tvs = viewSpec();
  // row height, and number of rows
  row_height = tvs->def_font.pointSize / t3Misc::pts_per_so_unit;
  view_bufsz = (int)(geom.z / row_height) - 1; // 1 for header

  InitHead();
  InitPanel();
}

void TextTableView::InitHead() {
  DT_ViewSpec* tvs = viewSpec();
  // col widths, and number of cols
  cols = 0;
  col_widths.Reset();
  taLeafItr i;
  DA_TextViewSpec* vs;
  tot_col_widths = 0.00001f; // avoid /0
  FOR_ITR_EL(DA_TextViewSpec, vs, viewspec->, i) {
    if((vs->visible == false) || (vs->data_array == NULL)) continue;
    ++cols;
    // col width will depend on font size, and number of tabs in col (8 chars / tab)
    float col_wd = MIN(
      (tvs->def_font.pointSize * vs->width * 8)  / t3Misc::char_pts_per_so_unit,
      (float)geom.x
    );
    tot_col_widths += col_wd;
    col_widths.Add(col_wd);
  }
  AdjustColView();
}

/*void TextTableView::NewData() {
  if(!isMapped() || !display_toggle) return;

  if((view_range.max < log()->data_range.max-1) && (view_range.max > 0))
    return;			// i'm not viewing the full file here..

  if ((view_range.max - view_range.min + 1) >= view_bufsz) {
    view_range.min++;
    RemoveLine(0);
  }
  view_range.max++; //must update before calling AddLine
  AddLine();
  UpdateDisplay();
} */

void TextTableView::OnWindowBind_impl(iT3DataViewFrame* vw) {
  inherited::OnWindowBind_impl(vw);
  if (!m_lvp) {
    m_lvp = new iTextTableView_Panel(this);
    vw->viewerWindow()->AddPanelNewTab(m_lvp);
  }
}

void TextTableView::RenderHead() {
  //note: only called when node exists
  T3GridTableViewBaseNode* node_so = this->node_so();
  DT_ViewSpec* tvs = viewSpec();
  // make header
  SoSeparator* hdr = new SoSeparator();
  SoFont* fnt = new SoFont();
  FontSpec fs = tvs->def_font;
  fs.setBold(true);
  fs.copyTo(fnt);
  hdr->addChild(fnt);

  taLeafItr i;
  int col = 0;
  DA_TextViewSpec* vs;
  FOR_ITR_EL(DA_TextViewSpec, vs, viewspec->, i) {
    if((vs->visible == false) || (vs->data_array == NULL)) continue;
    if (col < col_range.min) { ++col; continue;}
    if (col > col_range.max) break;
    SoTranslation* tr = new SoTranslation();
    hdr->addChild(tr);
    if (col == col_range.min) { // first trnsl positions row
      tr->translation.setValue(0.0f, geom.z - row_height, 0.0f);
    } else {
      tr->translation.setValue(col_widths[col - 1], 0.0f, 0.0f);
    }
    SoAsciiText* txt = new SoAsciiText();
    hdr->addChild(txt);
    txt->string.setValue(vs->display_name.chars());
    ++col;
  }
  node_so->header()->addChild(hdr);

}

/*obs
int TextTableView::ShowLess(int newsize) {
  if((newsize <= 0) || (newsize == view_bufsz)) return 1;
  view_bufsz = MAX(newsize, 1);	// never show zero!
  view_range.max = view_range.min + view_bufsz -1;
  view_range.MaxLT(log()->data_range.max); // double check
  view_range.min = view_range.max - view_bufsz +1;
  view_range.MinGT(log()->data_range.min);
  // this crashes because its called within a draw..
  //  FixSize();
  return 0;
}

int TextTableView::ShowMore(int newsize) {
  if((newsize <= 0) || (newsize == view_bufsz)) return 1;
  int oldsize = (view_range.max - view_range.min +1);
  view_bufsz = MAX(newsize, 1);
  view_range.max = view_range.min + view_bufsz -1;
  view_range.MaxLT(log()->data_range.max); // double check
  view_range.min = view_range.max - view_bufsz +1;
  view_range.MinGT(log()->data_range.min);
  if(oldsize != (view_range.max - view_range.min + 1)) {
    // this crashes because its called within a draw..
    //    FixSize();
    //    return 0;
    return 0;
  }
  else {
    return 1;
  }
} */

void TextTableView::RemoveLine(int index){
  T3GridTableViewBaseNode* node_so = this->node_so();
  if (!node_so) return;
  if (index == -1) {
    node_so->body()->removeAllChildren();
  } else {
    if (index <= (node_so->body()->getNumChildren() - 1))
      node_so->body()->removeChild(index);
  }
}

void TextTableView::UpdateDisplay(TAPtr) {
/*TODO   if(!taMisc::gui_active) return;
  if(patch == NULL) return;
  patch->reallocate();
  patch->redraw();
  if(anim.capture) CaptureAnimImg(); */
}

void TextTableView::UpdateFromBuffer_impl(){
  // this updates the data area
  if(!taMisc::gui_active || !display_toggle) return;
  T3GridTableViewBaseNode* node_so = this->node_so();
  if (!node_so) return;
  node_so->body()->removeAllChildren();

  // we only need 1 font for all rows
  SoFont* fnt = new SoFont();
  FontSpec& fs = viewSpec()->def_font;
  fs.copyTo(fnt);
  node_so->body()->addChild(fnt);

  // this is the index (in view_range units) of the first view line of data in the buffer
  int row = 0;
  int buff_offset = data_range_min_();
  for (int ln = view_range.min; ln <= view_range.max; ln++) {
    UpdateFromBuffer_AddLine(row, ln - buff_offset);
    ++row;
  }
}

void TextTableView::UpdateFromBuffer_AddLine(int row, int buff_idx){
  T3GridTableViewBaseNode* node_so = this->node_so();
  if (!node_so) return;

  // make line container
  SoSeparator* ln = new SoSeparator();

  taLeafItr i;
  int col = 0;
  DA_TextViewSpec* vs;
  DataTable* dt = data(); //cache
  String el;
  FOR_ITR_EL(DA_TextViewSpec, vs, viewspec->, i) {
    if((vs->visible == false) || (!vs->data_array)) continue;
    if (col < col_range.min) {++col; continue;}
    if (col > col_range.max) break;
    SoTranslation* tr = new SoTranslation();
    ln->addChild(tr);
    if (col == col_range.min) { // translation is relative to 0,0
      tr->translation.setValue(0.0f, geom.z - (row_height * (row + 2)), 0.0f);
    } else {
      tr->translation.setValue(col_widths[col - 1], 0.0f, 0.0f);
    }
//TODO: we really shouldn't be doing this low level stuff ourself...
    taMatrix* mat = vs->data_array->AR();
    int act_idx;
    //calculate the actual row index, for the case of a jagged data tables
    if ((mat != NULL) && dt->idx(buff_idx, mat->frames(), act_idx)) {
      el = vs->ValAsString(act_idx);
    } else {
      el = "n/a";
    }
    SoAsciiText* txt = new SoAsciiText();
    ln->addChild(txt);
    txt->string.setValue(el.chars());
    ++col;
  }
  node_so->body()->addChild(ln);
}


//////////////////////////
//    GridTableView	//
//////////////////////////

void GridTableView::Initialize() {
  fill_type = DT_GridViewSpec::COLOR;
  block_size = 8;
  block_border_size = 1;
  header_on = true;
  auto_scale = false;
  scale_range.min = -1.0f;
  scale_range.max = 1.0f;
  view_bufsz = 3;
//obs  view_shift = .2f;
  colorspec = NULL;

//TODO  editor = NULL;
  head_tog_but = NULL;
  auto_sc_but = NULL;
  SetAdapter(new GridTableViewAdapter(this));
}

void GridTableView::InitLinks(){
  inherited::InitLinks();

//done  CreateViewSpec();
  // set top level view spec to not use gp_name
  ((DT_GridViewSpec *) viewspec)->use_gp_name = false;

  taBase::Own(view_font,this);
  taBase::Own(scale_range,this);
  taBase::Own(actual_range,this);
}

void GridTableView::CutLinks() {
  view_font.CutLinks();
  taBase::DelPointer((TAPtr*)&colorspec);
  inherited::CutLinks();
//TODO  if(editor != NULL) { delete editor; editor = NULL; }
}

void GridTableView::Destroy() {
  CutLinks();
}

void GridTableView::Copy_(const GridTableView& cp) {
  fill_type = cp.fill_type;
  block_size = cp.block_size;
  block_border_size = cp.block_border_size;
  header_on = cp.header_on;
  auto_scale = cp.auto_scale;
  scale_range = cp.scale_range;
  taBase::SetPointer((TAPtr*)&colorspec, cp.colorspec);
  view_font = cp.view_font;
  actual_range = cp.actual_range;
}

void GridTableView::UpdateAfterEdit(){
  TableView::UpdateAfterEdit();
  if (!taMisc::gui_active) return;
  if (taMisc::is_loading) return;

  bool init = false;
/*TODO  if((colorspec != NULL) && (editor->scale->spec != colorspec)) {
    taBase::SetPointer((TAPtr*)&(editor->scale->spec), colorspec);
    editor->scale->MapColors();
    editor->dtg->background(editor->scale->Get_Background());
    init = true;
  }
  if(editor->fill_type != fill_type) {
    editor->fill_type = fill_type;    init = true;
  }
  if(editor->block_size != block_size) {
    editor->block_size = block_size;    init = true;
  }
  if(editor->block_border_size != block_border_size) {
    editor->block_border_size = block_border_size;    init = true;
  }
  if(editor->header_on != header_on) {
    editor->header_on = header_on;    init = true;
  }
  if(editor->auto_scale != auto_scale) {
    editor->auto_scale = auto_scale;    init = true;
  }
  if(editor->view_font.pattern != view_font.pattern) {
    editor->view_font = view_font;    init = true;
  }
  if(editor->scale_range != scale_range) {
    editor->scale_range = scale_range;    init = true;
  } */

  if(init) InitDisplay();
  else UpdateDisplay();
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


TypeDef* GridTableView::DT_ViewSpecType() {
  return &TA_DT_GridViewSpec;
}

TypeDef* GridTableView::DA_ViewSpecType() {
  return &TA_DA_GridViewSpec;
}

void GridTableView::NewHead() {
  TableView::NewHead();
  if(!isMapped() || !display_toggle) return;
//    if((editor != NULL) && (editor->dtvsp != NULL))
//      editor->dtvsp->UpdateLayout();
  InitDisplay();
}

void GridTableView::NewData() {
  if(!isMapped() || !display_toggle ) return;
  if((view_range.max < data_range_max_()-1) && (view_range.max > 0))
    return;			// not viewing the whole range, don't update
  view_range.max++;
  view_range.MaxLT(data_range_max_()); // double check
  view_range.MinGT(view_range.max - view_bufsz + 1);
  view_range.WithinRange(data_range_min_(), data_range_max_()); // triple check

  actual_range.min = view_range.min - data_range_min_();
  actual_range.max = view_range.max - data_range_min_();
/*TODO  editor->view_range = actual_range;
  editor->AddOneLine();
  view_bufsz = editor->disp_lines;
  actual_range = editor->view_range;
  view_range.min = actual_range.min + log()->data_range.min;
  view_range.max = actual_range.max + log()->data_range.min; */
}

void GridTableView::InitDisplay(){
  if(!taMisc::gui_active) return;
  if (taMisc::is_loading) return;
  TableView::InitDisplay();
  actual_range.min = view_range.min - data_range_min_();
  actual_range.max = view_range.max - data_range_min_();
/*TODO  editor->view_range = actual_range;
  editor->InitDisplay();
  view_bufsz = editor->disp_lines;
  actual_range = editor->view_range;
  view_range.min = actual_range.min + log()->data_range.min;
  view_range.max = actual_range.max + log()->data_range.min;
  auto_scale = editor->auto_scale;
  scale_range = editor->scale_range;
  if(auto_sc_but != NULL) {
    auto_sc_but->setDown(auto_scale);
  } */
}

void GridTableView::UpdateDisplay(TAPtr){
  if(!taMisc::gui_active) return;
/*TODO  editor->UpdateDisplay();
  auto_scale = editor->auto_scale;
  scale_range = editor->scale_range;
  if(auto_sc_but != NULL) {
    auto_sc_but->setDown(auto_scale);
  }
  if(anim.capture) CaptureAnimImg(); */
}

void GridTableView::UpdateFromBuffer_impl() {
  inherited::UpdateFromBuffer_impl();
  InitDisplay();
}

void GridTableView::Clear_impl(){
  view_range.max = -1;
  view_range.min = 0;
  InitDisplay();
}

void GridTableView::GetBodyRep(){
/*  if(!taMisc::gui_active) return;
  if(body != NULL) return;

  editor = new DTEditor(&(log()->data), (DT_GridViewSpec*)viewspec, (ivWindow*) window);
  actual_range.min = view_range.min - log()->data_range.min;
  actual_range.max = view_range.max - log()->data_range.min;

  editor->fill_type = fill_type;
  editor->block_size = block_size;
  editor->block_border_size = block_border_size;
  editor->header_on = header_on;
  editor->auto_scale = auto_scale;
  editor->view_font = view_font;
  editor->scale_range = scale_range;
  editor->view_range = actual_range;

  editor->Init();

//TODO:  connect(editor->cbar, SIGNAL(TODO:SIGNALNAME), this->adapter, SLOT(ColorBar_execute()));
  const iColor* bgclr = GetEditColorInherit();
  if(bgclr == NULL) bgclr = wkit->background();

  QHBoxLayout* log_buttons = new ivBackground(GetLogButtons(), bgclr);

  body = layout->vbox
	  (log_buttons,
	   layout->flexible
	   (layout->natural
	    (wkit->inset_frame(editor->GetLook()),200,150),fil,150));
  ivResource::ref(body);
  if(bgclr != NULL) {
    wkit->pop_style();
  } */
}

void GridTableView::OnWindowBind_impl(iT3DataViewFrame* vw) {
  inherited::OnWindowBind_impl(vw);
  if (!m_lvp) {
    m_lvp = new iGridTableView_Panel(this);
    vw->viewerWindow()->AddPanelNewTab(lvp());
  }
}

void GridTableView::ToggleHeader() {
  header_on = !header_on;
//TODO  if(editor != NULL) editor->header_on = header_on;
  if(head_tog_but != NULL) {
    head_tog_but->setDown(header_on);
    InitDisplay();
  }
}

void GridTableView::ToggleAutoScale() {
  auto_scale = !auto_scale;
//TODO  if(editor != NULL) editor->auto_scale = auto_scale;
  if(auto_sc_but != NULL) {
    auto_sc_but->setDown(auto_scale);
    InitDisplay();
  }
}

QWidget* GridTableView::GetPrintData() {
/*TODO  if((editor != NULL) && (editor->print_patch != NULL))
    return editor->print_patch; */
  return TableView::GetPrintData();
}

void GridTableView::SetColorSpec(ColorScaleSpec* colors) {
  taBase::SetPointer((TAPtr*)&colorspec, colors);
  UpdateAfterEdit();
}

void GridTableView::SetBlockFill(DT_GridViewSpec::BlockFill b){
  fill_type = b;
//TODO  editor->fill_type = b;
  InitDisplay();
}

void GridTableView::SetBlockSizes(int block_sz, int border_sz) {
  block_size = block_sz;
  block_border_size = border_sz;
  UpdateAfterEdit();
}

void GridTableView::UpdateGridLayout(DT_GridViewSpec::MatrixLayout ml){
//TODO   editor->dtvsp->UpdateLayout(ml);
   InitDisplay(); // gets new headerbox and grids
}

void GridTableView::SetViewFontSize(int point_size) {
  view_font.SetFontSize(point_size);
  UpdateAfterEdit();
}

void GridTableView::AllBlockTextOn() {
  DT_GridViewSpec* vs = (DT_GridViewSpec*)viewspec;
  int i;
  for(i=0;i<vs->size;i++) {
    DA_GridViewSpec* da = (DA_GridViewSpec*)vs->FastEl(i);
    if(da->display_style == DA_GridViewSpec::BLOCK)
      da->display_style = DA_GridViewSpec::TEXT_AND_BLOCK;
  }
  int j;
  FOR_ITR_GP(DT_GridViewSpec, vs, viewspec->, j) {
    if(vs->use_gp_name) {
      if(vs->display_style == DA_GridViewSpec::BLOCK)
	vs->display_style = DA_GridViewSpec::TEXT_AND_BLOCK;
    }
    else {
      for(i=0;i<vs->size;i++) {
	DA_GridViewSpec* da = (DA_GridViewSpec*)vs->FastEl(i);
	if(da->display_style == DA_GridViewSpec::BLOCK)
	  da->display_style = DA_GridViewSpec::TEXT_AND_BLOCK;
      }
    }
  }
  InitDisplay();
}

void GridTableView::AllBlockTextOff() {
  DT_GridViewSpec* vs = (DT_GridViewSpec*)viewspec;
  int i;
  for(i=0;i<vs->size;i++) {
    DA_GridViewSpec* da = (DA_GridViewSpec*)vs->FastEl(i);
    if(da->display_style == DA_GridViewSpec::TEXT_AND_BLOCK)
      da->display_style = DA_GridViewSpec::BLOCK;
  }
  int j;
  FOR_ITR_GP(DT_GridViewSpec, vs, viewspec->, j) {
    if(vs->use_gp_name) {
      if(vs->display_style == DA_GridViewSpec::TEXT_AND_BLOCK)
	vs->display_style = DA_GridViewSpec::BLOCK;
    }
    else {
      for(i=0;i<vs->size;i++) {
	DA_GridViewSpec* da = (DA_GridViewSpec*)vs->FastEl(i);
	if(da->display_style == DA_GridViewSpec::TEXT_AND_BLOCK)
	  da->display_style = DA_GridViewSpec::BLOCK;
      }
    }
  }
  InitDisplay();
}
//////////////////////////
//    GraphTableView	//
//////////////////////////

GraphTableView* GraphTableView::NewGraphTableView(DataTable* dt,
    T3DataViewFrame* fr)
{
  if (!dt) return NULL;
  GraphTableView* rval = new GraphTableView;
  rval->InitNew(dt, fr);
  return rval;
}

/*TODO nn??
void GraphTableViewLabel::GetMasterViewer() {
  GraphTableView* glv = GET_MY_OWNER(GraphTableView);
  if((glv != NULL) && (glv->editor != NULL)) {
    GraphEditor* ge = glv->editor;
    if(ge->first_graph != NULL) {
      viewer = ge->first_graph->viewer;
      master = ge->first_graph->graphg;
    }
  } 
}*/

void GraphTableView::Initialize() {
  x_axis_index = 0;
//  graph = new Graph();
//  labels.SetBaseType(&TA_GraphTableViewLabel); // make sure to use this type..
  view_range.max = 10000;
  view_bufsz = 10000;
  colorspec = NULL;
  separate_graphs = false;
  graph_layout.x = 1; graph_layout.y = 10;
  anim_stop = false;
  graphs = NULL;
}

TypeDef* GraphTableView::DT_ViewSpecType() {
  return &TA_GraphSpec;
}

TypeDef* GraphTableView::DA_ViewSpecType() {
  return &TA_GraphColSpec;
}

void GraphTableView::InitLinks() {
//  taBase::Own(graph, this);
//  taBase::Own(labels,this);
  taBase::Own(actual_range,this);
  inherited::InitLinks();
  UpdateViewRange();
}

void GraphTableView::CutLinks() {
//  labels.CutLinks();
  taBase::DelPointer((TAPtr*)&colorspec);
//  if (graph) graph.CutLinks();
  inherited::CutLinks();
}

void GraphTableView::Destroy() {
  CutLinks();
/*  if (graph) {
    delete graph;
    graph = NULL;
  } */
}

void GraphTableView::Copy_(const GraphTableView& cp) {
  x_axis_index = cp.x_axis_index;
//  labels = cp.labels;
  actual_range = cp.actual_range;
  taBase::SetPointer((TAPtr*)&colorspec, cp.colorspec);
  separate_graphs = cp.separate_graphs;
  graph_layout = cp.graph_layout;
}

void GraphTableView::UpdateAfterEdit() {
  TableView::UpdateAfterEdit();
//  labels.SetBaseType(&TA_GraphTableViewLabel); // make sure to use this type..
/*TODO  if((editor == NULL) || (log() == NULL)) return;
  bool init = false;
  if((colorspec != NULL) && (editor->scale->spec != colorspec)) {
    taBase::SetPointer((TAPtr*)&(editor->scale->spec), colorspec);
    editor->scale->MapColors();
    //    editor->dtg->background(editor->scale->Get_Background());
    init = true;
  }
  if(editor->separate_graphs != separate_graphs) {
    editor->separate_graphs = separate_graphs;    init = true;
  }
  if(editor->graph_layout != graph_layout) {
    editor->graph_layout = graph_layout;    init = true;
  }
  x_axis_index = MIN(log()->data.size-1, x_axis_index);
  x_axis_index = MAX(0, x_axis_index);
  UpdateViewRange();
  if(editor->x_axis_index != x_axis_index) {
    editor->SetXAxis(x_axis_index);
  }
  if(init)
    InitDisplay();
  else
    UpdateDisplay(); */
}

void GraphTableView::BuildAll() {
  Reset();
  if (!graphs) {
    graphs = GraphViews::New(viewSpec());
    children.Add(graphs);
    graphs->BuildAll();
  }
}

void GraphTableView::ChildAdding(T3DataView* child) {
  inherited::ChildAdding(child);
  TypeDef* typ = child->GetTypeDef();
  if (typ->InheritsFrom(&TA_GraphViews)) {
     graphs = (GraphViews*)child;
  }
}

void GraphTableView::ChildRemoving(T3DataView* child) {
  if (child == graphs) {graphs = NULL; goto done;}
done:
  inherited::ChildRemoving(child);
}

void GraphTableView::Clear_impl() {
  inherited::Clear_impl();
/*obs  view_range.max = -1;
  view_range.min = 0; // collapse the view range..
  InitDisplay(); */
}

void GraphTableView::InitColors(){
//TODO  if(editor!=NULL) editor->InitColors();
}

void GLVSetXAxis(TAPtr tap, int idx){
  GraphTableView* glv = (GraphTableView *) tap;
  if(glv->x_axis_index != idx){
    glv->x_axis_index = idx;
    glv->DataChanged(DCR_ITEM_UPDATED); //obs tabMisc::NotifyEdits(glv);
  }
}

void GraphTableView::GetBodyRep() {
  if(!taMisc::gui_active) return;

/*TODO  editor = new GraphEditor((TAPtr) this, &(log()->data),
			   (GraphSpec*) viewspec, (ivWindow*) window);
  editor->Init();
  editor->SetOwnerXAxis = GLVSetXAxis;

  const iColor* bgclr = GetEditColorInherit();
  if(bgclr == NULL) bgclr = wkit->background();

  ivGlyph* ed_bod = editor->GetLook();
  QHBoxLayout* log_buttons = GetLogButtons();
  body = layout->vbox(new ivBackground(log_buttons, bgclr), ed_bod);
  ivResource::ref(body); */
}

QWidget* GraphTableView::GetPrintData(){
return NULL; /*TODO  if(editor == NULL) return TableView::GetPrintData();
  for(int i=0;i<editor->graphs.size;i++) {
    GraphGraph* gg = (GraphGraph*)editor->graphs[i];
    gg->viewer->ClearCachedAllocation(); // recompute size for printing
  }
  return editor->boxpatch; */
}

void GraphTableView::InitDisplay() {
  TableView::InitDisplay();
  if (graphs) {
    graphs->ReInit(); // clears out lines and axes
  }
  UpdateViewRange();
/*TODO  if(editor == NULL) return;
  editor->last_pt_offset = 0;
  editor->InitDisplay(); */
}

void GraphTableView::Log_Clear() {
  if(!isMapped() || !display_toggle || (graphs == NULL)) return;
  view_range = 0;
  UpdateDisplay();
}

void GraphTableView::NewData() {
  if(!isMapped() || !display_toggle || (graphs == NULL)) return;
  bool not_at_last = false;
  if ((view_range.max < data_range_max_()-1) && (view_range.max > 0)) {
    // not viewing most recent data point..
    not_at_last = true;
    if((data_range_max_() - view_range.min) > view_bufsz) // beyond current display -- don't view
      return;
  }
  view_range.max++;
  view_range.MaxLT(data_range_max_()); // double check
  view_range.MinGT(view_range.max - view_bufsz + 1);
  view_range.WithinRange(data_range_min_(), data_range_max_()); // triple check

//TODO: fix this nonsense!!!!!!!!!!!!!!!!!!!!!!!!!!!! UVR sets actual_range!!! don't change it again!!!
  UpdateViewRange();
  actual_range.max++;
/*TODO: fix this nonsense... nothing should set the graph_spec's view_range directly
  graphs->graph_spec()->view_range = actual_range;
*/
  if (not_at_last) {
    UpdateDisplay();
  } else if (actual_range.Range() >= view_bufsz) {
/*TODO: shift view    int shft = (int)((float)view_bufsz * view_shift);
    if(shft < 1) shft = 1;
    view_range.min += shft;
    actual_range.min += shft;
    */
    UpdateDisplay();
  } else {
    graphs->AddLastPoint();
  }
}

void GraphTableView::NewHead() {
  TableView::NewHead();
  if(!isMapped() || !display_toggle) return;
  // for new header (or data reset) just rebuild all graphs
  if (graphs) {
    graphs->BuildAll();
  }
}

T3GraphTableViewNode* GraphTableView::node_so() const {
  return (T3GraphTableViewNode*)m_node_so.ptr();
}


void GraphTableView::OnWindowBind_impl(iT3DataViewFrame* vw) {
  inherited::OnWindowBind_impl(vw);
  if (!m_lvp) {
    m_lvp = new iGraphTableView_Panel(this);
    vw->viewerWindow()->AddPanelNewTab(m_lvp);
  }
}

void GraphTableView::Render_impl() {
//nn  GraphSpec* gvs = viewSpec(); // cache
  // set origin: in allocated area
  if (graphs) {
    graphs->geom.x = geom.x - (2 * frame_inset); //note: PDP coords
    graphs->geom.y = geom.y;
    graphs->geom.z = geom.z - (2 * frame_inset);
    graphs->transform(true)->translate.x = frame_inset; //note: Inventor coords
    graphs->transform()->translate.y = frame_inset;
  }

  inherited::Render_impl();
}

void GraphTableView::Render_pre() {
  m_node_so = new T3GraphTableViewNode(this);

  inherited::Render_pre();
}

int GraphTableView::SetXAxis(const String& nm) {
  int dx = data()->data.FindLeaf(nm); //TODO: replace with the portable DataTable i/f
  if (dx >= 0) {
    x_axis_index = dx;
    UpdateAfterEdit();
  }
  return x_axis_index;
}

void GraphTableView::UpdateFromBuffer_impl() {
  inherited::UpdateFromBuffer_impl();
  UpdateDisplay();
}

void GraphTableView::UpdateDisplay(TAPtr) {
  UpdateViewRange();
  if (graphs) {
    //TODO: maybe don't need to always do complete redraw???
    graphs->Redraw();
  }
/*TODO  if(editor != NULL) editor->UpdateDisplay();
  if(anim.capture) CaptureAnimImg(); */
}

void GraphTableView::UpdateViewRange() {
  actual_range.min = view_range.min - data_range_min_();
  actual_range.max = view_range.max - data_range_min_();
  if (viewSpec()) {
    viewSpec()->view_range = actual_range;
//TODO: fix up this whacky nonsense!!!!!!
//TEMP
viewSpec()->view_range.min = 0;
viewSpec()->view_range.max = (data()) ? -1 + data()->rows : 0;


    viewSpec()->view_bufsz = view_bufsz;
  }
}

void GraphTableView::Animate(int msec_per_point) {
/*TODO  if(editor == NULL) return;
  anim_stop = false;
  int old_bufsz = editor->view_bufsz;
  editor->view_bufsz = 0;
  editor->InitDisplay();	// init with nothing shown!
  taiMisc::RunPending();
  editor->view_bufsz = old_bufsz;
  struct timespec ts;
  ts.tv_sec = msec_per_point / 1000;
  ts.tv_nsec = (msec_per_point % 1000) * 1000000;
  int mx_cnt = MIN(old_bufsz, log()->data.MaxLength());
  cerr << "running: " << mx_cnt << " cycles: " << endl;
  for(int i=mx_cnt-1; i>=0; i--) {
    cerr << "plotting point: " << mx_cnt - i -1<< endl;
    editor->view_bufsz = mx_cnt - i -1;
    editor->last_pt_offset = i;
    editor->last_row_cnt = editor->x_axis_ar->ar->size - 1; // fake last point
    editor->AddLastPoint();
    taiMisc::RunPending();
    if(anim.capture) CaptureAnimImg();
    if(anim_stop) break;
    nanosleep(&ts, NULL);
  }
  anim_stop = false;
  editor->last_pt_offset = 0;	// just make sure */
}

void GraphTableView::StopAnimate() {
  anim_stop = true;
}

void GraphTableView::SetColorSpec(ColorScaleSpec* colors) {
  taBase::SetPointer((TAPtr*)&colorspec, colors);
  UpdateAfterEdit();
}

void GraphTableView::SetBackground(RGBA background){
  GraphSpec* dtvsp = (GraphSpec*)viewspec;
  dtvsp->background = background;
  dtvsp->UpdateAfterEdit();
}

void GraphTableView::UpdateLineFeatures() {
  GraphSpec* dtvsp = (GraphSpec*)viewspec;
  dtvsp->UpdateLineFeatures();
  InitDisplay();
}

void GraphTableView::SetLineFeatures(GraphSpec::ColorType	color_type,
				   GraphSpec::SequenceType sequence1,
				   GraphSpec::SequenceType sequence2,
				   GraphSpec::SequenceType sequence3,
				   bool visible_only) {
  GraphSpec* dtvsp = (GraphSpec*)viewspec;

  if(sequence3 == sequence1) sequence3 = GraphSpec::NONE;
  if(sequence2 == sequence1) sequence2 = GraphSpec::NONE;
  else if(sequence3 == sequence2) sequence3 = GraphSpec::NONE;

  dtvsp->color_type = color_type;
  dtvsp->sequence_1 = sequence1;
  dtvsp->sequence_2 = sequence2;
  dtvsp->sequence_3 = sequence3;
  dtvsp->UpdateLineFeatures(visible_only);
  InitDisplay();
}

void GraphTableView::SetLineWidths(float line_width) {
  GraphSpec* dtvsp = (GraphSpec*)viewspec;
  dtvsp->SetLineWidths(line_width);
}

void GraphTableView::SetLineType(GraphColSpec::LineType line_type) {
  GraphSpec* dtvsp = (GraphSpec*)viewspec;
  dtvsp->SetLineType(line_type);
}

void GraphTableView::ShareAxisAfter(taBase* spec) {
  if(spec==NULL) return;
  if(!spec->InheritsFrom(&TA_GraphColSpec)) {
    taMisc::Error("Must specify a valid GraphViewSpec variable as the axis variable");
    return;
  }
  GraphColSpec* var = (GraphColSpec*) spec;
  GraphSpec* own_gp = (GraphSpec*)var->owner;
  bool not_yet = true;
  int i;
  for(i=0;i<own_gp->size;i++) {
    GraphColSpec* davs = (GraphColSpec*)own_gp->FastEl(i);
    if(davs == var) {
      not_yet = false;
      continue;
    }
    if(!davs->visible || not_yet) continue;
    taBase::SetPointer((TAPtr*)&(davs->axis_spec), var);
  }
  InitDisplay();
}

void GraphTableView::ShareAxes() {
  GraphSpec* dtvsp = (GraphSpec*)viewspec;
  dtvsp->ShareAxes();
}

void GraphTableView::SeparateAxes() {
  GraphSpec* dtvsp = (GraphSpec*)viewspec;
  dtvsp->SeparateAxes();
}

void GraphTableView::SeparateGraphs(int x_layout, int y_layout) {
  separate_graphs = true;
  graph_layout.x = x_layout;
  graph_layout.y = y_layout;
  UpdateAfterEdit();
}

void GraphTableView::OneGraph() {
  separate_graphs = false;
  UpdateAfterEdit();
}

void GraphTableView::StackTraces() {
  GraphSpec* dtvsp = (GraphSpec*)viewspec;
  dtvsp->StackTraces();
}

void GraphTableView::UnStackTraces() {
  GraphSpec* dtvsp = (GraphSpec*)viewspec;
  dtvsp->UnStackTraces();
}

void GraphTableView::StackSharedAxes() {
  GraphSpec* dtvsp = (GraphSpec*)viewspec;
  dtvsp->StackSharedAxes();
}

void GraphTableView::UnStackSharedAxes() {
  GraphSpec* dtvsp = (GraphSpec*)viewspec;
  dtvsp->UnStackSharedAxes();
}

void GraphTableView::SpikeRaster(float thresh) {
  GraphSpec* dtvsp = (GraphSpec*)viewspec;
  dtvsp->graph_type = GraphSpec::STACK_LINES;
  taLeafItr i;
  GraphColSpec* davs;
  FOR_ITR_EL(GraphColSpec, davs, dtvsp->, i) {
    davs->line_type = GraphColSpec::THRESH_POINTS;
    davs->negative_draw = false;
    davs->thresh = thresh;
    davs->line_width = 2.0;
  }
  YAxisSpec* daxs;
  for (int j = 0; j < dtvsp->y_axes.size; ++j) {
    daxs = dtvsp->y_axes.FastEl(j);
    daxs->no_vertical = true;
//    daxs->vertical = YAxisSpec::NO_VERTICAL;
//    daxs->shared_y = YAxisSpec::STACK_LINES;
  }
  InitDisplay();
}

void GraphTableView::ColorRaster() {
  GraphSpec* dtvsp = (GraphSpec*)viewspec;
  dtvsp->graph_type = GraphSpec::STACK_LINES;
  taLeafItr i;
  GraphColSpec* davs;
  FOR_ITR_EL(GraphColSpec, davs, dtvsp->, i) {
    davs->line_type = GraphColSpec::VALUE_COLORS;
    davs->negative_draw = false;
    davs->line_width = 2.0;
  }
  YAxisSpec* daxs;
  for (int j = 0; j < dtvsp->y_axes.size; ++j) {
    daxs = dtvsp->y_axes.FastEl(j);
    daxs->no_vertical = true;
//    daxs->vertical = YAxisSpec::NO_VERTICAL;
//    daxs->shared_y = YAxisSpec::STACK_LINES;
  }
  InitDisplay();
}

void GraphTableView::StandardLines() {
  GraphSpec* dtvsp = (GraphSpec*)viewspec;
  YAxisSpec* daxs;
  for (int j = 0; j < dtvsp->y_axes.size; ++j) {
    daxs = dtvsp->y_axes.FastEl(j);
    daxs->no_vertical = false;
//    daxs->vertical = YAxisSpec::FULL_VERTICAL;
  }
  UpdateLineFeatures();
}


//////////////////////////
//   TableView_ViewType 	//
//////////////////////////

/*int TableView_ViewType::BidForView(TypeDef* td) {
  if (td->InheritsFrom(&TA_TableView))
    return (inherited::BidForView(td) +1);
  return 0;
} */

/*taiDataLink* TableViewType::CreateDataLink_impl(taBase* data_) {
  return new tabListDataLink((taList_impl*)data_);
} */



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
/*TODO  list = new iLDPListView(this, "list");
  layOuter->addWidget(list);
  list->setSelectionMode(QListView::Extended);
  list->setShowSortIndicator(true);
  // set up number of cols, based on link
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
// iGridTableViewBase_Panel //
//////////////////////////

iGridTableViewBase_Panel::iGridTableViewBase_Panel(GridTableViewBase* tlv)
:inherited(tlv)
{
}

iGridTableViewBase_Panel::~iGridTableViewBase_Panel() {
}

void iGridTableViewBase_Panel::InitPanel_impl() {
  inherited::InitPanel_impl();
  GridTableViewBase* lv = this->lv(); //cache
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
    sb->setMaxValue(lv->cols - 1);
    //we make a crude estimate of page step based on ratio of act width to tot width
    int pg_step = (int)(((lv->geom.x / lv->tot_col_widths) * lv->cols) + 0.5f);
    if (pg_step == 0) pg_step = 1;
    sb->setSteps(1, pg_step);
  }
  //TODO: BufferUpdated()???
  --updating;
}

void iGridTableViewBase_Panel::BufferUpdated() {
  ++updating;
  inherited::BufferUpdated();
  GridTableViewBase* lv = this->lv(); //cache
  // only show row slider if necessary
  if (lv->view_bufsz >= lv->virt_lines_()) {
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
    int mx = MAX((lv->virt_lines_() - lv->view_bufsz), 0);
    sb->setMaxValue(mx);
    //page step size based on viewable to total lines
    int pg_step = MAX(lv->view_bufsz, 1);
    sb->setSteps(1, pg_step);
    sb->setValue(MIN(lv->view_range.min, mx));
  }
  --updating;
}

void iGridTableViewBase_Panel::horScrBar_valueChanged(int value) {
  GridTableViewBase* lv = this->lv(); //cache
  if (updating || !lv) return;
  lv->ViewC_At(value);
}

void iGridTableViewBase_Panel::verScrBar_valueChanged(int value) {
  GridTableViewBase* lv = this->lv(); //cache
  if (updating || !lv) return;
  lv->View_At(value);
}

//////////////////////////
// iTextTableView_Panel //
//////////////////////////

iTextTableView_Panel::iTextTableView_Panel(TextTableView* tlv)
:inherited(tlv)
{
  Constr_T3ViewspaceWidget();
/*TODO  list = new iLDPListView(this, "list");
  layOuter->addWidget(list);
  list->setSelectionMode(QListView::Extended);
  list->setShowSortIndicator(true);
  // set up number of cols, based on link
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

iTextTableView_Panel::~iTextTableView_Panel() {
}

void iTextTableView_Panel::InitPanel_impl() {
  inherited::InitPanel_impl();
//  TextTableView* lv = this->lv(); //cache
  // only show col slider if necessary
}

String iTextTableView_Panel::panel_type() const {
  static String str("Text Log");
  return str;
}


//////////////////////////
// iGridTableView_Panel //
//////////////////////////

iGridTableView_Panel::iGridTableView_Panel(GridTableView* glv)
:inherited(true, glv)
{

  connect(chkAuto, SIGNAL(toggled(bool)), this, SLOT(chkAuto_toggled(bool)) );
  connect(chkHeaders, SIGNAL(toggled(bool)), this, SLOT(chkHeaders_toggled(bool)) );

/*TODO  list = new iLDPListView(this, "list");
  layOuter->addWidget(list);
  list->setSelectionMode(QListView::Extended);
  list->setShowSortIndicator(true);
  // set up number of cols, based on link
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

iGridTableView_Panel::~iGridTableView_Panel() {
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
// iGraphTableView_Panel //
//////////////////////////

iGraphTableView_Panel::iGraphTableView_Panel(GraphTableView* glv)
:inherited(glv)
{
  //TODO: add in below when there is anything else other than graphbutts
//  layToolGraphButtons = new QVBoxLayout(layContents);
//  QLayout*		  layToolButtons;// layout containing tool buttons
//  layGraphButtons = new QVBoxLayout(layToolGraphButtons)  ;// layout containing tool buttons
  layGraphButtons = new QVBoxLayout(layContents)  ;// layout containing tool buttons
  lblGraphButtons = new QLabel("Data Column Buttons", this);
  layGraphButtons->addWidget(lblGraphButtons);
  gbs = new iGraphButtons(this);
  layGraphButtons->addWidget(gbs, 1);



  Constr_T3ViewspaceWidget();

/*TODO  list = new iLDPListView(this, "list");
  layOuter->addWidget(list);
  list->setSelectionMode(QListView::Extended);
  list->setShowSortIndicator(true);
  // set up number of cols, based on link
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

iGraphTableView_Panel::~iGraphTableView_Panel() {
}


void iGraphTableView_Panel::GetImage_impl() {
  inherited::GetImage_impl();
  gbs->setGraph(glv()->viewSpec(), true);
}

void iGraphTableView_Panel::InitPanel_impl() {
  inherited::InitPanel_impl();
  gbs->setGraph(glv()->viewSpec(), true);
}

String iGraphTableView_Panel::panel_type() const {
  static String str("Graph Log");
  return str;
}




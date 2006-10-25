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


//////////////////////////
// 	TextTableView	//
//////////////////////////

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

void TextTableView::UpdateDisplay(TAPtr) {
/*TODO   if(!taMisc::gui_active) return;
  if(patch == NULL) return;
  patch->reallocate();
  patch->redraw();
  if(anim.capture) CaptureAnimImg(); */
}


//////////////////////////
//    GridTableView	//
//////////////////////////



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

void GridTableView::Clear_impl(){
  view_range.max = -1;
  view_range.min = 0;
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
  int dx = dataTable()->data.FindLeaf(nm); //TODO: replace with the portable DataTable i/f
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
viewSpec()->view_range.max = (dataTable()) ? -1 + dataTable()->rows : 0;


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




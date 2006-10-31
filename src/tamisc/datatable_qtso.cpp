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
#include "igeometry.h"

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
  if (fr->isMapped())
    fr->Render();
}


void TableView::Initialize() {
  data_base = &TA_DataTableViewSpec;  //superceded
//obs  view_bufsz = 100;
  view_bufsz = 0; //note: set by actual class based on screen
//obs  view_shift = .2f;
  view_range.min = 0;
  view_range.max = -1;
  display_on = true;
  m_lvp = NULL;
  geom.SetXYZ(4, 1, 3);
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
  pos.x++; pos.y--; pos.z++; // outset it from last one
  geom = cp.geom;
  frame_inset = cp.frame_inset;
}

void TableView::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if (!isVisible()) return;
  InitViewSpec();
  InitDisplay();
  ViewRangeChanged();
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
  view_range.max = -1;
}

void TableView::DataChanged_DataTable(int dcr, void* op1, void* op2) {
  if (dcr == DCR_ITEM_DELETING) {
    UpdateAfterEdit(); // easiest
    return;
  }
  
  DataTable* dt = dataTable(); // note: valid, otherwise we couldn't get this notify
  if (dcr == DCR_UPDATE_VIEWS) {
    if (dt->rows > m_rows) {
      DataChange_NewRows();
      return;
    } else { // if not appending rows, treat as struct update
      DataChange_StructUpdate();
      return;
    }
  } else if (dcr == DCR_STRUCT_UPDATE_END) {
    DataChange_StructUpdate();
    return;
  } else if (dcr == DCR_DATA_UPDATE_END) {
    DataChange_Other();
    return;
  }
  // we don't respond to any other kinds of updates
}

void TableView::DataChange_StructUpdate() {
  if (!isVisible()) return;
  InitViewSpec();
  ClearViewRange();
  InitDisplay();
  ViewRangeChanged();
}
  
void TableView::DataChange_NewRows() {
//TEMP, or overridden in Grid
DataChange_StructUpdate();
}
  
void TableView::DataChange_Other() {
//TEMP, or overridden in Grid
DataChange_StructUpdate();
}

void TableView::InitDisplay(){
  DataTable* data_table = dataTable();
  if (data_table)
    m_rows = data_table->rows;
  else m_rows = 0;
  InitDisplay_impl();
}

void TableView::InitViewSpec() {
  DataTable* data_table = dataTable();
  viewSpecBase()->BuildFromDataTable(data_table, true);
}

void TableView::InitPanel() {
  if (m_lvp)
    m_lvp->InitPanel();
}

bool TableView::isVisible() const {
  return (taMisc::gui_active && display_on && isMapped());
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
  InitDisplay(); // for first time
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

void TableView::setDisplay(bool value) {
  if (display_on == value) return;
  display_on = value;
  // if going off, just leave display as is, but if on, reinit
  if (display_on) {
    UpdateAfterEdit(); // does the whole kahuna
  }
}

void TableView::ViewRangeChanged() {
  if (!isVisible())
     return;
  ViewRangeChanged_impl();
  if (m_lvp)
    m_lvp->BufferUpdated();
}

void TableView::UpdatePanel() {
  if (m_lvp)
    m_lvp->UpdatePanel();
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
  ViewRangeChanged();
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
  geom.SetXYZ(12, 1, 9);
  data_base = &TA_GridTableViewSpec;  //supercedes base
  col_bufsz = 0;
  row_height = 0.1f; // non-zero dummy value
  tot_col_widths = 0.00001f; //avoid /0
  
//from GTV
  header_on = true;
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
  header_on = cp.header_on;
  auto_scale = cp.auto_scale;
  scale_range = cp.scale_range;
  scale = cp.scale;
  actual_range = cp.actual_range;
  view_spec = cp.view_spec;
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
  if (col_bufsz == 0) {
    col_range.max = col_range.min - 1;
    return;
  } else 
    // note: we display at least one col, even if it spills over size
    col_range.max = col_range.min;
  
  float wd_tot = 0.0f;
  while ((col_range.max < (col_bufsz - 1)) && 
   ((wd_tot += colWidth(col_range.max)) <= (float)geom.x))//TODO: frame_inset???
    col_range.max++;
}

void GridTableView::CalcViewMetrics() {
  GridTableViewSpec* tvs = viewSpec();
  // header height (if on)
  if (header_on)
    head_height = tvs->font.pointSize / t3Misc::pts_per_so_unit;
  else head_height = 0.0f;
  
  // num vis cols, tot col width (for scrollbars), row height
  col_bufsz = 0;
  tot_col_widths = 0.00001f; // avoid /0
  row_height = 0.00001f;
  taListItr itr;
  GridColViewSpec* vs;
  FOR_ITR_EL(GridColViewSpec, vs, tvs->col_specs., itr) {
    if ((!vs->visible) || (!vs->dataCol())) continue;
    ++col_bufsz;
    tot_col_widths += vs->col_width;
    row_height = MAX(row_height, vs->row_height);
  }
  // calculate visible rows based on row_height and our geom
  view_bufsz = MAX((int)((geom.z - head_height - (2 * frame_inset)) / row_height), 1);
  
  CalcColMetrics();
}

void GridTableView::ClearViewRange() {
  col_range.min = 0;
  inherited::ClearViewRange();
}


void GridTableView::Clear_impl() {
//  if (!taMisc::gui_active ) return;
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

void GridTableView::InitDisplay_impl() {
  if (!header_on) RemoveHeader();
  CalcViewMetrics();
  view_range.max = MIN(view_bufsz, (rows() - 1));
  scale.SetMinMax(scale_range.min, scale_range.max);
  InitPanel();
  RenderHeader();
}
/*TODO: this is basis of DataChange_NewRows
void GridTableView::DataRowsAdded() {
int m_rows = rows();
  // if we are not at the very end, then don't scroll, but do update the panel
  if ((view_range.max < rows()-1) && (view_range.max > 0)) {
    if (m_lvp)
      m_lvp->BufferUpdated();
    return;
  }

  if ((view_range.max - view_range.min + 1) >= view_bufsz) {
    view_range.min++;
  }
  view_range.max++; //must update before calling AddLine
  ViewRangeChanged();
}*/

void GridTableView::OnWindowBind_impl(iT3DataViewFrame* vw) {
  inherited::OnWindowBind_impl(vw);
  if (!m_lvp) {
    m_lvp = new iGridTableView_Panel(this);
    vw->viewerWindow()->AddPanelNewTab(lvp());
  }
}

void GridTableView::RemoveHeader() {
  T3GridTableViewNode* node_so = this->node_so();
  if (node_so) {
    node_so->header()->removeAllChildren();
  }
}

void GridTableView::RemoveLine(int idx){
  T3GridTableViewNode* node_so = this->node_so();
  if (!node_so) return;
  if (idx < 0) {
    node_so->body()->removeAllChildren();
  } else {
    if (idx <= (node_so->body()->getNumChildren() - 1))
      node_so->body()->removeChild(idx);
  }
}

void GridTableView::RenderHeader() {
  if (!header_on) return; // normally shouldn't be called if off
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
  int col = -1;
  GridColViewSpec* vs;
  FOR_ITR_EL(GridColViewSpec, vs, tvs->col_specs., itr) {
    ++col;
    if ((!vs->visible) || (!vs->dataCol())) continue;
    if (col < col_range.min) continue;
    if (col > col_range.max) break;
    SoTranslation* tr = new SoTranslation();
    hdr->addChild(tr);
    if (col == col_range.min) { // first trnsl positions row
      tr->translation.setValue(0.0f, geom.z - head_height, 0.0f);
    } else {
      tr->translation.setValue(colWidth(col - 1), 0.0f, 0.0f);
    }
    SoAsciiText* txt = new SoAsciiText();
    hdr->addChild(txt);
    txt->string.setValue(vs->GetDisplayName().chars());
  }
  node_so->header()->addChild(hdr);
}

void GridTableView::RenderLine(int view_idx, int data_row) {
  T3GridTableViewNode* node_so = this->node_so();
  if (!node_so) return;
  GridTableViewSpec* tvs = viewSpec();

  // make line container
  SoSeparator* ln = new SoSeparator();

  taListItr itr;
  int col = -1;
  GridColViewSpec* vs;
  DataTable* dt = dataTable(); //cache
  String el;
  FOR_ITR_EL(GridColViewSpec, vs, tvs->col_specs., itr) {
    col++; // lets us continue at any point
    DataArray_impl* data_array = vs->dataCol();
    if ((!vs->visible) || (!data_array)) continue;
    if (col < col_range.min) continue;
    if (col > col_range.max) break;
    
    //calculate the actual col row index, for the case of a jagged data table
    int act_idx; // <0 for null
    dt->idx(data_row, data_array->rows(), act_idx);
    
    // translate the row and col to proper location
    // new origin will be top-left of row
    SoTranslation* tr = new SoTranslation();
    ln->addChild(tr);
    // 1st tr places origin for the row
    if (col == col_range.min) { // translation is relative to 0,0
      tr->translation.setValue(
        0.0f, 
        geom.z - head_height - frame_inset - (row_height * view_idx),
        0.0f);
    } else {
      tr->translation.setValue(colWidth(col - 1), 0.0f, 0.0f);
    }
    float col_width = colWidth(col); // cache for convenience
    
    // cache 2d-equivalent geom info, and render according to col style
    iVec2i cg;
    data_array->Get2DCellGeom(cg); 
    if (vs->display_style & GridColViewSpec::TEXT_MASK) {
      //TODO: mat cells
    //TODO: add a font, if col font differs from main font
      SoSeparator* txt_sep = new SoSeparator;
      ln->addChild(txt_sep);
      tr = new SoTranslation;
      txt_sep->addChild(tr);
      // text origin is bottom left, and we want to center when 
      // height is greater than txt height
      float offs = ((row_height - vs->row_height) / 2) + vs->row_height;
      tr->translation.setValue(0.0f, -offs, 0.0f);
      if (act_idx >= 0) {
        el = data_array->GetValAsString(act_idx);
      } else {
        el = "n/a";
      }
      SoAsciiText* txt = new SoAsciiText();
      txt_sep->addChild(txt);
      txt->string.setValue(el.chars());
    }
    // if val is NULL, just skip rendering remaining col types
    if (act_idx < 0) continue;
    
    if (vs->display_style & GridColViewSpec::TEXT_AND_BLOCK) {
      //TODO: space
    }
    if (vs->display_style & GridColViewSpec::BLOCK_MASK) {
      float bsz = vs->blockSize(); 
      float bbsz = vs->blockBorderSize(); 
      SoSeparator* blk = new SoSeparator;
      ln->addChild(blk);
      // if using border, create it first
      
      T3Color col;
      iVec2i c; // index coords
      //note: accessor uses a linear cell value; 
      // for TOP_ZERO, this can just iterate from zero
      // for BOT_ZERO we need to massage it
      int cell = 0; // accessor just uses a linear cell value
      for (c.y = 0; c.y < cg.y; ++c.y) {
        if (vs->layout == GridColViewSpec::BOT_ZERO)
          cell = cg.x * (cg.y - c.y - 1);
        SoSeparator* blk_ln = new SoSeparator;
        blk->addChild(blk_ln);
        // add line trans 
        SoTranslation* tr = new SoTranslation();
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
          float val = data_array->GetValAsFloatM(act_idx, cell);
          //NOTE: following a bit dangerous, but should never be null
          col = *(scale.GetColor(val));
          bc->rgb = (SbColor)col;
//TEMP
//val = cell / float(cg.x * cg.y);
//bc->rgb.setValue(val, val, val);
          SoCube* cu = new SoCube;
          cu->width = bsz;
          cu->height = bsz;
          cu->depth = 0.0f;
          blk_ln->addChild(cu);
          ++cell;
        }
      }        
    }
    if (vs->display_style & GridColViewSpec::IMAGE) {
      if ((cg.x == 0) || (cg.y == 0)) continue; // something wrong!
      SoSeparator* img = new SoSeparator;
      ln->addChild(img);
      SoTransform* tr = new SoTransform();
      img->addChild(tr);
      // scale the image according to pixel metrics
      //note: image defaults to 1 geom unit height, so we scale by our act height
      float pxsz =  vs->pixelSize();
      float scale_fact = pxsz * cg.y; // note: NOT row_height
      tr->scaleFactor.setValue(scale_fact, scale_fact, 1.0f);
      // center the shape in the middle of the cell
      tr->translation.setValue((col_width / 2), -(row_height / 2), 0.0f);
      SoImageEx* img_so = new SoImageEx;
      img->addChild(img_so);
      // just center it to simplify our translation
//      img->vertAlignment = SoImageEx::HALF;
//      img->horAlignment = SoImageEx::CENTER;
      //note: we must unref the mat
      taMatrix* cell_mat =  data_array->GetValAsMatrix(act_idx);
      taBase::Ref(cell_mat);
      img_so->setImage(*cell_mat);
      taBase::UnRef(cell_mat);
    }

  }
  node_so->body()->addChild(ln);
}

void GridTableView::RenderLines(){
  // this updates the data area
  T3GridTableViewNode* node_so = this->node_so();
  if (!node_so) return;
  node_so->body()->removeAllChildren(); //should already have been done

  // master font -- we only add a child font if different
  SoFont* fnt = new SoFont();
  FontSpec& fs = viewSpec()->font;
  fs.copyTo(fnt);
  node_so->body()->addChild(fnt);

  // this is the index (in view_range units) of the first view line of data in the buffer
  int view_idx = 0;
  for (int data_row = view_range.min; data_row <= view_range.max; ++data_row) {
    RenderLine(view_idx, data_row);
    ++view_idx;
  }
}

void GridTableView::Render_pre() {
  m_node_so = new T3GridTableViewNode(this);

  inherited::Render_pre();
}

void GridTableView::Render_post() {
  if (header_on) RenderHeader();
  ViewRangeChanged(); //TODO: should this really be here???
  inherited::Render_post();
}

void GridTableView::Reset_impl() {
  col_range.min = 0;
  inherited::Reset_impl();
}

void GridTableView::SetBlockSizes(float block_sz, float border_sz) {
  view_spec.block_size = block_sz;
  view_spec.block_border_size = border_sz;
  UpdateAfterEdit();
}

void GridTableView::SetViewFontSize(int point_size) {
  view_spec.font.SetFontSize(point_size);
  UpdateAfterEdit();
}

void GridTableView::ToggleAutoScale() {
  auto_scale = !auto_scale;
//TODO  update, and update panel
}

void GridTableView::ToggleHeader() {
  header_on = !header_on;
//TODO  update, and update panel
}

void GridTableView::SetBlockFill(
  GridColViewSpec::BlockColor color,
  GridColViewSpec::BlockFill fill)
{
//TODO: iterate, setting for all applicable col_bufsz
  InitDisplay();
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
  RemoveLines();
  RemoveHeader(); // noop if off
  if (start < 0) start = 0;
  if (start >= col_bufsz) start = col_bufsz - 1;
  if (col_range.min == start) return;
  col_range.min = start;
  CalcColMetrics();
  if (header_on) RenderHeader();
  RenderLines();
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
  ++updating;
  chkDisplay->setChecked(lv->display_on);
  --updating;
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
/*obs    //we make a crude estimate of page step based on ratio of act width to tot width
    int pg_step = (int)(((lv->geom.x / lv->tot_col_widths) * lv->col_bufsz) + 0.5f);
    if (pg_step == 0) pg_step = 1; */
    // for cols, only convenient page step is 1 because Qt forces the little arrows
    // to also be the same step size as clicking in the blank area
    int pg_step = 1;
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

void iGridTableView_Panel::GetImage_impl() {
  inherited::GetImage_impl();
  GridTableView* lv = this->glv(); // cache
  ++updating;
  chkHeaders->setChecked(lv->header_on);
  chkAuto->setChecked(lv->auto_scale);
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
  GridTableView* glv = this->glv(); //cache
  if (updating || !glv) return;
  if (glv->auto_scale == on) return;
  glv->auto_scale = on;
  glv->UpdateAfterEdit();
}

void iGridTableView_Panel::chkHeaders_toggled(bool on) {
  GridTableView* glv = this->glv(); //cache
  if (updating || !glv) return;
  if (glv->header_on == on) return;
  glv->header_on = on;
  glv->UpdateAfterEdit();
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


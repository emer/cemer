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



// datatable.cc

#include "datatable.h"


#ifdef TA_GUI
#  include "datatable_qtso.h"
#endif

#include <limits.h>
#include <float.h>
#include <ctype.h>

//////////////////////////////////
// 	DA Grid View Specs	//
//////////////////////////////////

void GridColViewSpec::Initialize(){
  display_style = TEXT; // updated later in build
  text_width = 16;
  num_prec = 5;
  mat_layout = BOT_ZERO; // typical default for data patterns
  scale_on = true;
  col_width = 0.0f;
  row_height = 0.0f;
}

void GridColViewSpec::Copy_(const GridColViewSpec& cp){
  display_style = cp.display_style;
  text_width = cp.text_width;
  num_prec = cp.num_prec;
  mat_layout = cp.mat_layout;
  scale_on = cp.scale_on;
  // others recalced
}

void GridColViewSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if (text_width < 2) text_width = 2; // smallest practical
  if (num_prec < 2) num_prec = 2;
}

void GridColViewSpec::UpdateFromDataCol_impl(bool first){
  inherited::UpdateFromDataCol_impl(first);
  DataArray_impl* dc = dataCol(); //note: exists, because we were called
  if (first) {
    // just get the display width, don't worry about maxwidth
    text_width = dc->displayWidth();
  
    if (dc->isMatrix() && dc->isNumeric()) {
      if (dc->GetUserData("IMAGE").toBool()) {
        display_style = IMAGE;
        mat_layout = BOT_ZERO;
      } else {
        display_style = BLOCK;
        mat_layout = BOT_ZERO;
      }
    } else /*obs  if (dc->GetUserData("TEXT").toBool() ||
      dc->GetUserData(DataArray_impl::udkey_narrow).toBool() ||
      dc->InheritsFrom(TA_String_Data)
    )*/ {
      display_style = TEXT;
    }
  }
}

void GridColViewSpec::DataColUnlinked() {
  col_width = 0.0f;
  row_height = 0.0f;
}

void GridColViewSpec::Render_impl() {
  inherited::Render_impl(); // prob nothing
  //NOTE: we just calc everything in points, then adjust at the end
  // cache some params
  GridTableViewSpec* par = parent();
  DataArray_impl* dc = dataCol(); // cache
  col_width = 0.0f;
  row_height = 0.0f;
  if (!dc) return;
  
  float blk_pts = par->mat_block_pts; 
  float brd_pts = par->mat_border_pts; 
  float fnt_pts = par->font.pointSize;
  // first stab at col width is the normal size text, for scalar width
  float min_col_wd = fnt_pts * text_width;
  if (dc->isMatrix()) // shrink font for mats
    fnt_pts *=  par->mat_font_scale; 
  
  // get 2d equivalent cell geom values
  iVec2i cg;
  dc->Get2DCellGeom(cg); //note: 1x1 for scalar
  float row_ht = 0.0f;
  float col_wd = 0.0f;
  float tmp; // to avoid multi-calcs in min/max 
  if (display_style & BLOCK_MASK) {
    col_wd += (blk_pts * cg.x) + (brd_pts * (cg.x - 1));
    tmp =  (blk_pts * cg.y) + (brd_pts * (cg.y - 1));
    row_ht = MAX(row_ht, tmp);
  }
  if (display_style == TEXT_AND_BLOCK) {
    col_wd += par->mat_sep_pts;
  }
  if (display_style & TEXT_MASK) {
    col_wd += (fnt_pts * text_width * cg.x) + (brd_pts * (cg.x - 1));
    // row height, and number of rows -- ht ~ 12/8 x wd
    tmp = (fnt_pts * cg.y * t3Misc::char_ht_to_wd_pts) +
      (brd_pts * (cg.y - 1));
    row_ht = MAX(row_ht, tmp);
  }
  if (display_style == IMAGE) {
    //note: cg.y will be #comps*image.y (ex image.y for b&w, 3*image.y for rgb)
    float px_pts = par->pixel_pts; 
    tmp = px_pts * cg.x;
    col_wd = MAX(col_wd, tmp);
    row_ht += (px_pts * cg.y) / dc->imageComponents(); // need to comp
  }
  col_wd = MAX(col_wd, min_col_wd);
  // change to geoms
  col_width = col_wd * t3Misc::geoms_per_pt;
  row_height = row_ht * t3Misc::geoms_per_pt;
}


//////////////////////////////////
//  GridTableViewSpec		//
//////////////////////////////////

void GridTableViewSpec::Initialize() {
  col_specs.SetBaseType(&TA_GridColViewSpec);
  grid_margin_pts = 4.0f;
  grid_line_pts = 3.0f;
  SetMatSizeModel_impl(SMALL_BLOCKS);
}

void GridTableViewSpec::Destroy() {
}

void GridTableViewSpec::Copy_(const GridTableViewSpec& cp) {
  grid_margin_pts = cp.grid_margin_pts;
  grid_line_pts = cp.grid_line_pts;
  mat_size_model = cp.mat_size_model;
  mat_block_pts = cp.mat_block_pts;
  mat_border_pts = cp.mat_border_pts;
  mat_sep_pts = cp.mat_sep_pts;
  mat_font_scale = cp.mat_font_scale;
  pixel_pts = cp.pixel_pts;
}

void GridTableViewSpec::UpdateAfterEdit_impl(){
  inherited::UpdateAfterEdit_impl();
  // just blindly enforce all minimums
  if (grid_margin_pts < 0.0f) grid_margin_pts = 0.0f;
  if (grid_line_pts <  0.1f) grid_line_pts =  0.1f;
  if (mat_block_pts < 0.1f) mat_block_pts = 0.1f;
  if (mat_border_pts < 0.0f) mat_border_pts = 0.0f;
  if (mat_sep_pts < 0.0f) mat_sep_pts = 0.0f;
  if (mat_font_scale < 0.1f) mat_font_scale = 0.1f;
  if (pixel_pts < 0.1f) pixel_pts = 0.1f;
  // now, unconditionally apply any model
  SetMatSizeModel_impl(mat_size_model);
}

void GridTableViewSpec::DataDataChanged_impl(int dcr, void* op1, void* op2) {
  inherited::DataDataChanged_impl(dcr, op1, op2);
  TableView* tv = GET_MY_OWNER(TableView);
  if (!tv) return;
  tv->DataChanged_DataTable(dcr, op1, op2);
}

void GridTableViewSpec::DataDestroying() {
  TableView* tv = GET_MY_OWNER(TableView);
  if (tv) {
    tv->DataChanged_DataTable(DCR_ITEM_DELETING, NULL, NULL);
  }
  inherited::DataDestroying();
}

void GridTableViewSpec::GetMinMaxScale(MinMax& mm, bool first) {
/*TODO NOTE: this "frst" business is BROKEN -- can't use global static this way!!!
  static bool frst;
  if(first)
    frst = true; */
  int i;
  for (i=0;i< col_specs.size; i++){
    GridColViewSpec* vs = (GridColViewSpec *) col_specs.FastEl(i);
    if (!(vs->visible && vs->scale_on &&
      (vs->display_style & GridColViewSpec::BLOCK_MASK)))
      continue;
    if (!vs->dataCol()->isNumeric()) continue; // shouldn't happen!
/*BROKEN    if(frst) {
      frst = false;
      mm.max = ar->range.max;
      mm.min = ar->range.min;
    }
    else {
      mm.UpdateRange(ar->range);
    } */
  }
//TODO: need to add ranging to datatable
//TEMP
  mm.min = -1.0f;
  mm.max = 1.0f;
}

void GridTableViewSpec::SetMatSizeModel_impl(MatSizeModel mm){
  mat_size_model = mm;
  switch (mm) {
  case CUSTOM_METRICS: break;
  case SMALL_BLOCKS:
    mat_block_pts = 2.0f;
    mat_border_pts = 0.5f;
    mat_sep_pts = 2.0f;
    mat_font_scale = 0.5f;
    pixel_pts = 0.5f;
    break;
  case MEDIUM_BLOCKS:
    mat_block_pts = 8.0f;
    mat_border_pts = 2.0f;
    mat_sep_pts = 4.0f;
    mat_font_scale = 0.75f;
    pixel_pts = 2.0f;
    break;
  case LARGE_BLOCKS:
    mat_block_pts = 16.0f;
    mat_border_pts = 4.0f;
    mat_sep_pts = 6.0f;
    mat_font_scale = 0.8f;
    pixel_pts = 4.0f;
    break;
  }
}

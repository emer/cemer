// Copyright, 1995-2013, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
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

#include "GridColView.h"
#include <DataCol>
#include <GridTableView>
#include <T3GridColViewNode>


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

void GridColView::CopyFromView(GridColView* cp){
  Copy_(*cp);                   // get per above
  visible = cp->visible;                // from inh
}

void GridColView::Destroy() {
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
  else if(dc->isString()) {
    if(dc->HasUserData("WIDTH"))
      text_width = dc->GetUserData("WIDTH").toInt();
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
    row_height = 1.0f;          // always just one char high
    col_width = text_width;
  }
}

T3GridColViewNode* GridColView::MakeGridColViewNode() {
//NOTE: assumes that Clear has previously been called, so doesn't check for exist
  T3GridColViewNode* colnd = new T3GridColViewNode(this);
  setNode(colnd);
  return colnd;
}

void GridColView::SetTextWidth(int text_wdth) {
  text_width = text_wdth;
  GridTableView* par = parent();
  if(par)
    par->Render();
}



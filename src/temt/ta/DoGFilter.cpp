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

#include "DoGFilter.h"
#include <taMath_float>
#include <taProject>
#include <DataTable>



void DoGFilter::Initialize() {
  filter_width = 4;
  filter_size = filter_width * 2 + 1;
  on_sigma = 1.0f;
  off_sigma = 2.0f;
  spacing = 1;
  circle_edge = true;
  on_filter.SetGeom(2, filter_size, filter_size);
  off_filter.SetGeom(2, filter_size, filter_size);
  net_filter.SetGeom(2, filter_size, filter_size);
}

void DoGFilter::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  filter_size = filter_width * 2 + 1;
  UpdateFilter();
}

void DoGFilter::RenderFilter(float_Matrix& on_flt, float_Matrix& off_flt,
                                 float_Matrix& net_flt) {
  on_flt.SetGeom(2, filter_size, filter_size);
  off_flt.SetGeom(2, filter_size, filter_size);
  net_flt.SetGeom(2, filter_size, filter_size);
  int x,y;
  for(y=-filter_width; y<=filter_width; y++) {
    for(x=-filter_width; x<=filter_width; x++) {
      float dist = taMath_float::hypot(x, y);
      float ong = 0.0f;
      float offg = 0.0f;
      if(!circle_edge || (dist <= filter_width)) { // only set values inside of filter radius
        ong = taMath_float::gauss_den_sig(dist, on_sigma);
        offg = taMath_float::gauss_den_sig(dist, off_sigma);
      }
      on_flt.Set(ong, x+filter_width, y+filter_width);
      off_flt.Set(offg, x+filter_width, y+filter_width);
    }
  }

  taMath_float::vec_norm_sum(&on_flt); // make sure sums to 1.0
  taMath_float::vec_norm_sum(&off_flt); // make sure sums to 1.0

  for(int i=0;i<on_flt.size;i++) {
    float net = on_flt.FastEl_Flat(i) - off_flt.FastEl_Flat(i);
    net_flt.FastEl_Flat(i) = net;
  }
  //  taMath_float::vec_norm_abs_max(&net_flt); // max norm = 1
}

void DoGFilter::UpdateFilter() {
  RenderFilter(on_filter, off_filter, net_filter);
}

void DoGFilter::GraphFilter(DataTable* graph_data) {
  taProject* proj = GET_MY_OWNER(taProject);
  if(!graph_data) {
    graph_data = proj->GetNewAnalysisDataTable(name + "_DoG_GraphFilter", true);
  }
  graph_data->StructUpdate(true);
  graph_data->Reset();
  int idx;
  DataCol* xda = graph_data->FindMakeColName("X", idx, VT_FLOAT);
  DataCol* zda = graph_data->FindMakeColName("Z", idx, VT_FLOAT);
  DataCol* valda = graph_data->FindMakeColName("Y", idx, VT_FLOAT);

  xda->SetUserData("X_AXIS", true);
  zda->SetUserData("Z_AXIS", true);
  valda->SetUserData("PLOT_1", true);

  float_Matrix* mat = &net_filter;
  int x,z;
  for(z=-filter_width; z<=filter_width; z++) {
    for(x=-filter_width; x<=filter_width; x++) {
      float val = mat->FastEl2d(x+filter_width, z+filter_width);
      graph_data->AddBlankRow();
      xda->SetValAsFloat(x, -1);
      zda->SetValAsFloat(z, -1);
      valda->SetValAsFloat(val, -1);
    }
  }
  graph_data->StructUpdate(false);
  graph_data->FindMakeGraphView();
}

void DoGFilter::GridFilter(DataTable* graph_data, bool reset) {
  taProject* proj = GET_MY_OWNER(taProject);
  if(!graph_data) {
    graph_data = proj->GetNewAnalysisDataTable(name + "_DoG_GridFilter", true);
  }
  graph_data->StructUpdate(true);
  if(reset)
    graph_data->Reset();
  int idx;
  DataCol* nmda = graph_data->FindMakeColName("Name", idx, VT_STRING);
  DataCol* matda = graph_data->FindMakeColName("Filter", idx, VT_FLOAT, 2, filter_size, filter_size);

  float maxv = taMath_float::vec_max(&on_filter, idx);

  graph_data->SetUserData("N_ROWS", 3);
  graph_data->SetUserData("SCALE_MIN", -maxv);
  graph_data->SetUserData("SCALE_MAX", maxv);
  graph_data->SetUserData("BLOCK_HEIGHT", 0.0f);

  for(int i=0;i<3;i++) {
    float_Matrix* mat;
    graph_data->AddBlankRow();
    if(i==0) {
      nmda->SetValAsString("On", -1);
      mat = &on_filter;
    }
    else if(i==1) {
      nmda->SetValAsString("Off", -1);
      mat = &off_filter;
    }
    else {
      nmda->SetValAsString("Net", -1);
      mat = &net_filter;
    }
    matda->SetValAsMatrix(mat, -1);
  }
  graph_data->StructUpdate(false);
  graph_data->FindMakeGridView();
}


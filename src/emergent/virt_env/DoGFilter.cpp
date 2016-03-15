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

TA_BASEFUNS_CTORS_DEFN(DoGFilter);


void DoGFilter::Initialize() {
  on = true;
  half_size = 8;
  size = half_size * 2 + 1;
  on_sigma = 8.0f;
  off_sigma = 8.0f;
  spacing = 8.0f;
  gain = 8.0f;
  on_gain = 1.0f;
  circle_edge = true;
  on_filter.SetGeom(2, size, size);
  off_filter.SetGeom(2, size, size);
  net_filter.SetGeom(2, size, size);
}

void DoGFilter::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  size = half_size * 2 + 1;
  UpdateFilter();
}

void DoGFilter::RenderFilter(float_Matrix& on_flt, float_Matrix& off_flt,
                                 float_Matrix& net_flt) {
  on_flt.SetGeom(2, size, size);
  off_flt.SetGeom(2, size, size);
  net_flt.SetGeom(2, size, size);
  int x,y;
  for(y=-half_size; y<=half_size; y++) {
    for(x=-half_size; x<=half_size; x++) {
      float dist = taMath_float::hypot(x, y);
      float ong = 0.0f;
      float offg = 0.0f;
      if(!circle_edge || (dist <= half_size)) { // only set values inside of filter radius
        ong = taMath_float::gauss_den_sig(dist, on_sigma);
        offg = taMath_float::gauss_den_sig(dist, off_sigma);
      }
      on_flt.Set(ong, x+half_size, y+half_size);
      off_flt.Set(offg, x+half_size, y+half_size);
    }
  }

  taMath_float::vec_norm_sum(&on_flt); // make sure sums to 1.0
  taMath_float::vec_norm_sum(&off_flt); // make sure sums to 1.0

  float pos_sum = 0.0f;
  float neg_sum = 0.0f;
  for(int i=0;i<on_flt.size;i++) {
    float net = on_flt.FastEl_Flat(i) - off_flt.FastEl_Flat(i);
    net_flt.FastEl_Flat(i) = net;
    if(net > 0.0f) pos_sum += net;
    else           neg_sum += net;
  }

  // actually what matters most is balance on the NET guy!!
  if(pos_sum != 0.0f && neg_sum != 0.0f) {
    float pos_norm = 1.0f / pos_sum;
    float neg_norm = -1.0f / neg_sum;
    for(int i=0;i<on_flt.size;i++) {
      float& val = net_flt.FastEl_Flat(i);
      if(val > 0.0f)          { val *= pos_norm; }
      else if(val < 0.0f)     { val *= neg_norm; }
    }
  }
}

void DoGFilter::UpdateFilter() {
  RenderFilter(on_filter, off_filter, net_filter);
}

void DoGFilter::GraphFilter(DataTable* graph_data) {
  taProject* proj = GetMyProj();
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

  
  float_Matrix* mat;
  if(on_sigma == off_sigma)
    mat = &on_filter;
  else
    mat = &net_filter;
  
  int x,z;
  for(z=-half_size; z<=half_size; z++) {
    for(x=-half_size; x<=half_size; x++) {
      float val = mat->FastEl2d(x+half_size, z+half_size);
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
  taProject* proj = GetMyProj();
  if(!graph_data) {
    graph_data = proj->GetNewAnalysisDataTable(name + "_DoG_GridFilter", true);
  }
  graph_data->StructUpdate(true);
  if(reset)
    graph_data->Reset();
  int idx;
  DataCol* nmda = graph_data->FindMakeColName("Name", idx, VT_STRING);
  DataCol* matda = graph_data->FindMakeColName("Filter", idx, VT_FLOAT, 2, size, size);

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


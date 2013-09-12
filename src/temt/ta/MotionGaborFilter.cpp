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

#include "MotionGaborFilter.h"
#include <taProject>
#include <taMath_float>

#include <math.h>


void MotionGaborFilter::Initialize() {
/*
  x_size = 2;
  y_size = 2;
  ctr_x = .5f * x_size;
  ctr_y = .5f * y_size;
  angle = 0;
  phase = 0;
  freq = 1;
  length = 8;
  width = 4;
  amp = 1;
  filter.SetGeom(2, x_size, y_size);
*/
  x_size = 4;
  y_size = 4;
  t_size = 3;
  ctr_x = .5f * x_size;
  ctr_y = .5f * y_size;
  ctr_t = .5f * t_size;

  spat_angle = 0;               // angle of sine wave in 2-d space (in radians)
  time_angle = 1;               // angle of sine wave in 2-d time (in radians)

  phase = 0;            // phase of sine wave wrt the center of the gaussian (radians)
  freq = 1;             // frequency of the sine wave
  freq_t = 1;           // frequency of the sine wave

  width = 4;            // width of the gaussian in the wave direction
  length = 8;           // width of the gaussian in the wave direction
  width_t = 6;          // width of the gaussian in the wave direction

  amp = 1;              // amplitude (maximum value)
  filter.SetGeom(3, x_size, y_size, t_size);
}

float MotionGaborFilter::Eval(float x, float y, float t) {
  // normalize into normal coords, where sin goes along x axis
  float cx = x - ctr_x;
  float cy = y - ctr_y;
  float ct = time_angle*(t - ctr_t);


  float r = sqrtf(cx*cx + cy*cy);
  float thet = atan2(cy, cx);
  float totang = thet - spat_angle;
  float n_x = r * cos(totang);
  float n_y = r * sin(totang);

  float rval = 0;
  if(use_3d_gabors) {
    rval = amp * cos(phase + freq * n_y + freq_t*ct) *
      exp(-.5f * ((n_y * n_y) / (width * width) + (n_x * n_x) / (length * length) + (ct*ct)/(width_t*width_t) ));
  }
  else {
    n_y += (t - ctr_t) * freq_t;
    rval = amp * cos(phase + freq * n_y) *
      exp(-.5f * ((n_y * n_y) / (width * width) + (n_x * n_x) / (length * length)));
  }

  return rval;
}

void MotionGaborFilter::RenderFilter(float_Matrix& flt) {
  flt.SetGeom(3, x_size, y_size, t_size);
  for(int y=0;y<y_size;y++) {
    for(int x=0;x<x_size;x++) {
       for(int t = 0; t < t_size; t++) {
                   float val = Eval(x, y, t);
         flt.Set(val, x, y, t);
       }
    }
  }
}

void MotionGaborFilter::UpdateFilter() {
  RenderFilter(filter);
}

float MotionGaborFilter::GetParam(MotionGaborParam param) {
  switch(param) {
  case CTR_X:   return ctr_x;
  case CTR_Y:   return ctr_y;
  case CTR_T:   return ctr_t;
  case TIME_ANGLE:      return spat_angle;
  case SPAT_ANGLE:      return time_angle;
  case PHASE:   return phase;
  case FREQ:    return freq;
  case FREQ_T:  return freq_t;
  case LENGTH:  return length;
  case WIDTH:   return width;
  case WIDTH_T: return width_t;
  case AMP:     return amp;
  }
  return 0.0f;
}


void MotionGaborFilter::GraphFilter(DataTable* graph_data) {
  UpdateFilter();
  taProject* proj = GET_MY_OWNER(taProject);
  if(!graph_data) {
    graph_data = proj->GetNewAnalysisDataTable(name + "_MotionGabor_GraphFilter", true);
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

  int x,z,t;
  for(t=0; t<t_size; t++) {
    for(z=0; z<y_size; z++) {
      for(x=0; x<x_size; x++) {
        float val = filter.FastEl3d(x,z,t);
        graph_data->AddBlankRow();
        xda->SetValAsFloat(x, -1);
        zda->SetValAsFloat(z, -1);
        valda->SetValAsFloat(val, -1);
      }
    }
  }
  graph_data->StructUpdate(false);
  graph_data->FindMakeGraphView();
}

void MotionGaborFilter::GridFilter(DataTable* graph_data, bool reset) {
  UpdateFilter();
  taProject* proj = GET_MY_OWNER(taProject);
  if(!graph_data) {
    graph_data = proj->GetNewAnalysisDataTable(name + "_MotionGabor_GridFilter", true);
  }
  graph_data->StructUpdate(true);
  if(reset)
    graph_data->Reset();
  int idx;
  DataCol* matda = graph_data->FindMakeColName("Filter", idx, VT_FLOAT, 3, x_size, y_size, t_size);

  float maxv = taMath_float::vec_abs_max(&filter, idx);

  graph_data->SetUserData("N_ROWS", 1);
  graph_data->SetUserData("SCALE_MIN", -maxv);
  graph_data->SetUserData("SCALE_MAX", maxv);
  graph_data->SetUserData("BLOCK_HEIGHT", 0.0f);

  graph_data->AddBlankRow();
  matda->SetValAsMatrix(&filter, -1);

  graph_data->StructUpdate(false);
  graph_data->FindMakeGridView();
}

String& MotionGaborFilter::PrintParams(String& strm) {
  strm << "ctr: " << ctr_x << ", " << ctr_y << ", angle: " << spat_angle
       << ", phase: " << phase << ", freq: " << freq
       << ", length: " << length << ", width: " << width
       << ", amp: " << amp
       << "\n";
  return strm;
}


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

#include "VisRegionSpecBase.h"
#include <CircMatrix>
#include <taMath_float>
#include <DataTable>

#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(VisRegionSpecBase);

TA_BASEFUNS_CTORS_DEFN(VisRegionSizes);

TA_BASEFUNS_CTORS_DEFN(VisAdaptation);

TA_BASEFUNS_CTORS_DEFN(VisRegionParams);


// for thread function calling:
typedef void (VisRegionSpecBase::*VisRegionMethod)(int, int);

void VisRegionParams::Initialize() {
  ocularity = MONOCULAR;
  region = FOVEA;
  res = HI_RES;
  color = MONOCHROME;
  edge_mode= WRAP;
  renorm_thr = 1.0e-5f;
}

void VisRegionSizes::Initialize() {
  retina_size = 144;
  border = 0;
  input_size = 144;
}

void VisRegionSizes::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  input_size = (retina_size - 2 * border);
}

void VisAdaptation::Initialize() {
  on = false;
  up_dt = 0.3f;
  dn_dt = 0.1f;
}

// for thread function calling:
typedef void (VisRegionSpecBase::*VisRegionBaseMethod)(int, int);

void VisRegionSpecBase::Initialize() {
  save_mode = FIRST_ROW;
  image_save = (DataSave)(SAVE_DATA | ONLY_GUI);
  motion_frames = 0;

  cur_img_r = NULL;
  cur_img_l = NULL;
  cur_img = NULL;
  cur_in = NULL;
  cur_out = NULL;
  cur_in2 = NULL;
  cur_in_r = NULL;
  cur_out_r = NULL;
  cur_in_l = NULL;
  cur_out_l = NULL;
  cur_adapt = NULL;
  cur_circ = NULL;
  rgb_img = false;
  wrap = false;
  cur_mot_only = false;
}

void VisRegionSpecBase::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  input_size.UpdateAfterEdit_NoGui();
  motion_frames = MAX(motion_frames, 0);
  UpdateGeom();
}

void VisRegionSpecBase::UpdateGeom() {
  // note: override in derived classes..
}

bool VisRegionSpecBase::Init() {
  bool rval = InitFilters();
  rval &= InitOutMatrix();
  rval &= InitDataTable();
  return rval;
}

bool VisRegionSpecBase::NeedsInit() {
  // no way to know in base -- override!
  return false;
}

bool VisRegionSpecBase::InitFilters() {
  // note: override in derived classes..
  return true;
}

bool VisRegionSpecBase::InitOutMatrix() {
  // note: override in derived classes, but call base just in case..
  if(input_adapt.on) {
    cur_img_r_adapt.SetGeom(2, input_size.retina_size.x, input_size.retina_size.y);
    if(region.ocularity == VisRegionParams::BINOCULAR) {
      cur_img_l_adapt.SetGeom(2, input_size.retina_size.x, input_size.retina_size.y);
    }
    else {
      cur_img_l_adapt.SetGeom(1,1);
    }
  }
  else {
    cur_img_r_adapt.SetGeom(1,1);
    cur_img_l_adapt.SetGeom(1,1);
  }
  return true;
}

////////////////////////////////////////////////////////////////////
//      DoGRegion       Filtering

bool VisRegionSpecBase::FilterImage(float_Matrix* right_eye_image, float_Matrix* left_eye_image,
                                    bool motion_only) {
  // this is not typically overwritten -- just all the checks -- see _impl
  if(TestWarning(NeedsInit(),
                 "FilterImage", "not properly initialized to current geom -- running Init now")) {
    Init();
  }

  if(TestError(!right_eye_image, "FilterImage", "right_eye_image is NULL -- must pass image"))
    return false;

  if(TestError((right_eye_image->dim(0) != input_size.retina_size.x) ||
               (right_eye_image->dim(1) != input_size.retina_size.y),
               "FilterImage", "right_eye_image is not appropriate size -- must be same as retina_size!"))
    return false;

  if(region.ocularity == VisRegionParams::BINOCULAR) {
    if(TestError(!left_eye_image, "FilterImage", "left_eye_image is NULL -- must pass image"))
      return false;

    if(TestError((left_eye_image->dim(0) != input_size.retina_size.x) ||
                 (left_eye_image->dim(1) != input_size.retina_size.y),
                 "FilterImage", "left_eye_image is not appropriate size -- must be same as retina_size!"))
      return false;

    cur_img_l = left_eye_image;
  }

  cur_mot_only = motion_only;
  cur_img_r = right_eye_image;

  IncrTime();
  bool rval = FilterImage_impl(motion_only);

  cur_img_r = NULL;
  cur_img_l = NULL;

  return rval;
}

void VisRegionSpecBase::IncrTime() {
  // note: override in derived classes..
}

bool VisRegionSpecBase::FilterImage_impl(bool motion_only) {
  if(!data_table || save_mode == NONE_SAVE) // bail now
    return false;

  if(save_mode == FIRST_ROW) {
    data_table->EnforceRows(1);
    data_table->WriteItem(0);
    data_table->ReadItem(0);
  }
  else {
    data_table->AddBlankRow();
  }

  if(input_adapt.on) {
    int n_run = input_size.retina_size.Product();

    threads.n_threads = MIN(n_run, taMisc::thread_defaults.n_threads); // keep in range..
    threads.min_units = 1;
    threads.nibble_chunk = 1;     // small chunks

    cur_in = cur_img_r;
    cur_adapt = &cur_img_r_adapt;
    ThreadImgProcCall ip_call((ThreadImgProcMethod)(VisRegionBaseMethod)
                              &VisRegionSpecBase::InputAdapt_thread);
    threads.Run(&ip_call, n_run);
    if(region.ocularity == VisRegionParams::BINOCULAR) {
      cur_in = cur_img_l;
      cur_adapt = &cur_img_l_adapt;
      ThreadImgProcCall ip_call((ThreadImgProcMethod)(VisRegionBaseMethod)
                                &VisRegionSpecBase::InputAdapt_thread);
      threads.Run(&ip_call, n_run);
    }
  }

  if(image_save & SAVE_DATA && !(!taMisc::gui_active && image_save & ONLY_GUI)) {
    ImageToTable(data_table, cur_img_r, cur_img_l);
  }
  return true;
}

void VisRegionSpecBase::ResetAdapt() {
  if(input_adapt.on) {
    for(int i=0; i<cur_img_r_adapt.size; i++) {
      cur_img_r_adapt.FastEl_Flat(i) = 0.0f;
    }
    if(region.ocularity == VisRegionParams::BINOCULAR) {
      for(int i=0; i<cur_img_l_adapt.size; i++) {
        cur_img_l_adapt.FastEl_Flat(i) = 0.0f;
      }
    }
  }
}

void VisRegionSpecBase::InputAdapt_thread(int img_idx, int thread_no) {
  taVector2i sc;                 // simple coords
  sc.SetFmIndex(img_idx, input_size.retina_size.x);
  float& ret_in = cur_in->FastEl2d(sc.x, sc.y);
  float orig_in = ret_in;
  float& adpt = cur_adapt->FastEl2d(sc.x, sc.y);
  ret_in -= adpt;
  if(ret_in < 0.0f) ret_in = 0.0f; // can't go any lower
  adpt += input_adapt.up_dt * orig_in - input_adapt.dn_dt * adpt;
}

bool VisRegionSpecBase::ColorRGBtoCMYK(float_Matrix& img) {
  taVector2i img_size(img.dim(0), img.dim(1));

  cur_img_grey.SetGeom(2, img_size.x, img_size.y);
  cur_img_rc.SetGeom(2, img_size.x, img_size.y);
  cur_img_gm.SetGeom(2, img_size.x, img_size.y);
  cur_img_by.SetGeom(2, img_size.x, img_size.y);

  for(int yi = 0; yi < img_size.y; yi++) {
    for(int xi = 0; xi < img_size.y; xi++) {
      float r_val = img.FastEl3d(xi, yi, 0);
      float g_val = img.FastEl3d(xi, yi, 1);
      float b_val = img.FastEl3d(xi, yi, 2);

      float grey = 0.33333f * (r_val + g_val + b_val);
      float r_v_c = r_val - 0.5f * (g_val + b_val);
      float g_v_m = g_val - 0.5f * (r_val + b_val);
      float b_v_y = b_val - 0.5f * (r_val + g_val);

      cur_img_grey.FastEl2d(xi, yi) = grey;
      cur_img_rc.FastEl2d(xi, yi) = r_v_c;
      cur_img_gm.FastEl2d(xi, yi) = g_v_m;
      cur_img_by.FastEl2d(xi, yi) = b_v_y;
    }
  }
  return true;
}

float_Matrix* VisRegionSpecBase::GetImageForChan(ColorChannel cchan) {
  switch(cchan) {
  case LUMINANCE:
    return &cur_img_grey;
  case RED_CYAN:
    return &cur_img_rc;
  case GREEN_MAGENTA:
    return &cur_img_gm;
  case BLUE_YELLOW:
    return &cur_img_by;
  }
  return NULL;
}

bool VisRegionSpecBase::RenormOutput(RenormMode mode, float_Matrix* mat) {
  bool rval = false;
  int idx;
  float max_val = taMath_float::vec_max(mat, idx);
  if(max_val > region.renorm_thr) {
    rval = true;
    if(mode == LIN_RENORM) {
      taMath_float::vec_mult_scalar(mat, 1.0f / max_val);
    }
    else if(mode == LOG_RENORM) {
      float rescale = 1.0f / logf(1.0f + max_val);
      for(int j=0;j<mat->size;j++) {
        float& vl = mat->FastEl_Flat(j);
        vl = logf(1.0f + vl) * rescale;
      }
    }
  }
  return rval;
}


////////////////////////////////////////////////////////////////////
//      DoGRegion       Data Table Output

bool VisRegionSpecBase::InitDataTable() {
  if(!data_table) {
    return false;
  }
  int idx;
  if(image_save & SAVE_DATA) {
    DataCol* col;
    String sufx = "_r";

    if(region.color == VisRegionParams::COLOR)
      col = data_table->FindMakeColName(name + "_image" + sufx, idx, DataTable::VT_FLOAT, 3,
                                        input_size.retina_size.x, input_size.retina_size.y, 3);
    else
      col = data_table->FindMakeColName(name + "_image" + sufx, idx, DataTable::VT_FLOAT, 2,
                                        input_size.retina_size.x, input_size.retina_size.y);
    col->SetUserData("IMAGE", true);

    if(region.ocularity == VisRegionParams::BINOCULAR) {
      sufx = "_l";
      if(region.color == VisRegionParams::COLOR)
        col = data_table->FindMakeColName(name + "_image" + sufx, idx, DataTable::VT_FLOAT, 3,
                                          input_size.retina_size.x, input_size.retina_size.y, 3);
      else
        col = data_table->FindMakeColName(name + "_image" + sufx, idx, DataTable::VT_FLOAT, 2,
                                          input_size.retina_size.x, input_size.retina_size.y);
      col->SetUserData("IMAGE", true);
    }
  }

  return true;
}

bool VisRegionSpecBase::ImageToTable(DataTable* dtab, float_Matrix* right_eye_image,
                                 float_Matrix* left_eye_image) {
  ImageToTable_impl(dtab, right_eye_image, "_r");
  if(region.ocularity == VisRegionParams::BINOCULAR)
    ImageToTable_impl(dtab, left_eye_image, "_l");
  return true;
}

bool VisRegionSpecBase::ImageToTable_impl(DataTable* dtab, float_Matrix* img,
                                      const String& col_sufx) {
  DataCol* col;
  int idx;
  if(region.color == VisRegionParams::COLOR)
    col = data_table->FindMakeColName(name + "_image" + col_sufx, idx, DataTable::VT_FLOAT, 3,
                                      input_size.retina_size.x, input_size.retina_size.y, 3);
  else
    col = data_table->FindMakeColName(name + "_image" + col_sufx, idx, DataTable::VT_FLOAT, 2,
                                      input_size.retina_size.x, input_size.retina_size.y);

  float_MatrixPtr ret_img; ret_img = (float_Matrix*)col->GetValAsMatrix(-1);
  ret_img->CopyFrom(img);
  return true;
}

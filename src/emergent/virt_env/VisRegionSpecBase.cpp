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
#include <ColorSpace>

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
    if(region.color == VisRegionParams::COLOR) {
      cur_img_r_adapt.SetGeom(3, input_size.retina_size.x, input_size.retina_size.y, 3);
    }
    else {
      cur_img_r_adapt.SetGeom(2, input_size.retina_size.x, input_size.retina_size.y);
    }
    if(region.ocularity == VisRegionParams::BINOCULAR) {
      if(region.color == VisRegionParams::COLOR) {
        cur_img_l_adapt.SetGeom(3, input_size.retina_size.x, input_size.retina_size.y, 3);
      }
      else {
        cur_img_l_adapt.SetGeom(2, input_size.retina_size.x, input_size.retina_size.y);
      }
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

  threads.InitAll();
  
  if(save_mode == FIRST_ROW) {
    data_table->EnforceRows(1);
    data_table->WriteItem(0);
    data_table->ReadItem(0);
  }
  else {
    data_table->AddBlankRow();
  }

  if(input_adapt.on) {
    cur_img = cur_img_r;
    cur_adapt = &cur_img_r_adapt;

    IMG_THREAD_CALL(VisRegionSpecBase::InputAdapt_thread);

    if(region.ocularity == VisRegionParams::BINOCULAR) {
      cur_img = cur_img_l;
      cur_adapt = &cur_img_l_adapt;

      IMG_THREAD_CALL(VisRegionSpecBase::InputAdapt_thread);
    }
  }

  return true;
}

void VisRegionSpecBase::ResetAdapt() {
  if(input_adapt.on) {
    cur_img_r_adapt.InitVals(0.0f);
    if(region.ocularity == VisRegionParams::BINOCULAR) {
      cur_img_l_adapt.InitVals(0.0f);
    }
  }
}

void VisRegionSpecBase::InputAdapt_thread(int thr_no) {
  taVector2i st;
  taVector2i ed;
  GetThread2DGeom(thr_no, input_size.retina_size, st, ed);

  taVector2i oc;         // current coord -- output space
  taVector2i ic;         // input coord
  for(oc.y = st.y; oc.y < ed.y; oc.y++) {
    for(oc.x = st.x; oc.x < ed.x; oc.x++) {
      if(cur_img->dims() == 3) {
        for(int c = 0; c<3; c++) {
          float& ret_in = cur_img->FastEl3d(oc.x, oc.y, c);
          float orig_in = ret_in;
          float& adpt = cur_adapt->FastEl3d(oc.x, oc.y, c);
          ret_in -= adpt;
          if(ret_in < 0.0f) ret_in = 0.0f; // can't go any lower
          adpt += input_adapt.up_dt * orig_in - input_adapt.dn_dt * adpt;
        }
      }
      else {
        float& ret_in = cur_img->FastEl2d(oc.x, oc.y);
        float orig_in = ret_in;
        float& adpt = cur_adapt->FastEl2d(oc.x, oc.y);
        ret_in -= adpt;
        if(ret_in < 0.0f) ret_in = 0.0f; // can't go any lower
        adpt += input_adapt.up_dt * orig_in - input_adapt.dn_dt * adpt;
      }
    }
  }
#if 0
  taVector2i sc;                 // simple coords
  sc.SetFmIndex(img_idx, input_size.retina_size.x);
  if(cur_img->dims() == 3) {
    for(int c = 0; c<3; c++) {
      float& ret_in = cur_img->FastEl3d(sc.x, sc.y, c);
      float orig_in = ret_in;
      float& adpt = cur_adapt->FastEl3d(sc.x, sc.y, c);
      ret_in -= adpt;
      if(ret_in < 0.0f) ret_in = 0.0f; // can't go any lower
      adpt += input_adapt.up_dt * orig_in - input_adapt.dn_dt * adpt;
    }
  }
  else {
    float& ret_in = cur_img->FastEl2d(sc.x, sc.y);
    float orig_in = ret_in;
    float& adpt = cur_adapt->FastEl2d(sc.x, sc.y);
    ret_in -= adpt;
    if(ret_in < 0.0f) ret_in = 0.0f; // can't go any lower
    adpt += input_adapt.up_dt * orig_in - input_adapt.dn_dt * adpt;
  }
#endif
}

bool VisRegionSpecBase::PrecomputeColor(float_Matrix* img) {
  cur_img = img;
  taVector2i img_size(img->dim(0), img->dim(1));

  ColorSpace::sRGBtoOpponentsImg(cur_img_opp, *cur_img);

  cur_img_L_c = cur_img_opp.GetFrameSlice(ColorSpace::L_C);
  cur_img_M_c = cur_img_opp.GetFrameSlice(ColorSpace::M_C);
  cur_img_S_c = cur_img_opp.GetFrameSlice(ColorSpace::S_C);
  cur_img_LM_c = cur_img_opp.GetFrameSlice(ColorSpace::LM_C);
  cur_img_LvM = cur_img_opp.GetFrameSlice(ColorSpace::LvM_C);
  cur_img_SvLM = cur_img_opp.GetFrameSlice(ColorSpace::SvLM_C);
  cur_img_grey = cur_img_opp.GetFrameSlice(ColorSpace::GREY);
  
  return true;
}

float_Matrix* VisRegionSpecBase::GetImageForChan(ColorChannel cchan) {
  switch(cchan) {
  case GREY:
    return cur_img_grey.ptr();
  case RED_GREEN:
    return cur_img_LvM.ptr();
  case BLUE_YELLOW:
    return cur_img_SvLM.ptr();
  case RED:
    return cur_img_L_c.ptr();
  case GREEN:
    return cur_img_M_c.ptr();
  case BLUE:
    return cur_img_S_c.ptr();
  case YELLOW:
    return cur_img_LM_c.ptr();
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
    else if(max_val > 1.0f && mode == THR_LIN_RENORM) {
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
  if(OutSaveOk(image_save)) {
    ImageToTable(data_table, NULL, NULL, true); // fmt only
  }

  return true;
}

bool VisRegionSpecBase::ImageToTable(DataTable* dtab, float_Matrix* right_eye_image,
                                     float_Matrix* left_eye_image, bool fmt_only) {
  ImageToTable_impl(dtab, right_eye_image, "_r", fmt_only);
  if(region.ocularity == VisRegionParams::BINOCULAR)
    ImageToTable_impl(dtab, left_eye_image, "_l", fmt_only);
  return true;
}

bool VisRegionSpecBase::ImageToTable_impl(DataTable* dtab, float_Matrix* img,
                                          const String& col_sufx, bool fmt_only) {
  DataCol* col;
  int idx;
  if(region.color == VisRegionParams::COLOR) {
    col = data_table->FindMakeColName
      (name + "_image" + col_sufx, idx, DataTable::VT_FLOAT, 3,
       input_size.retina_size.x, input_size.retina_size.y, 3);
  }
  else {
    col = data_table->FindMakeColName
      (name + "_image" + col_sufx, idx, DataTable::VT_FLOAT, 2,
       input_size.retina_size.x, input_size.retina_size.y);
  }

  col->SetUserData("IMAGE", true);
  if(!fmt_only) {
    float_MatrixPtr ret_img; ret_img = (float_Matrix*)col->GetValAsMatrix(-1);
    ret_img->CopyFrom(img);
  }

  if(region.color == VisRegionParams::COLOR && (image_save & SEP_MATRIX) &&
     (image_save & SAVE_DEBUG)) {

    col = data_table->FindMakeColName
      (name + "_image_rd" + col_sufx, idx, DataTable::VT_FLOAT, 2,
       input_size.retina_size.x, input_size.retina_size.y);
    if(!fmt_only) {
      float_MatrixPtr ret_img; ret_img = (float_Matrix*)col->GetValAsMatrix(-1);
      ret_img->CopyFrom(cur_img_L_c.ptr());
    }

    col = data_table->FindMakeColName
      (name + "_image_gn" + col_sufx, idx, DataTable::VT_FLOAT, 2,
       input_size.retina_size.x, input_size.retina_size.y);
    if(!fmt_only) {
      float_MatrixPtr ret_img; ret_img = (float_Matrix*)col->GetValAsMatrix(-1);
      ret_img->CopyFrom(cur_img_M_c.ptr());
    }

    col = data_table->FindMakeColName
      (name + "_image_bl" + col_sufx, idx, DataTable::VT_FLOAT, 2,
       input_size.retina_size.x, input_size.retina_size.y);
    if(!fmt_only) {
      float_MatrixPtr ret_img; ret_img = (float_Matrix*)col->GetValAsMatrix(-1);
      ret_img->CopyFrom(cur_img_S_c.ptr());
    }

    col = data_table->FindMakeColName
      (name + "_image_yl" + col_sufx, idx, DataTable::VT_FLOAT, 2,
       input_size.retina_size.x, input_size.retina_size.y);
    if(!fmt_only) {
      float_MatrixPtr ret_img; ret_img = (float_Matrix*)col->GetValAsMatrix(-1);
      ret_img->CopyFrom(cur_img_LM_c.ptr());
    }

    col = data_table->FindMakeColName
      (name + "_image_grey" + col_sufx, idx, DataTable::VT_FLOAT, 2,
       input_size.retina_size.x, input_size.retina_size.y);
    if(!fmt_only) {
      float_MatrixPtr ret_img; ret_img = (float_Matrix*)col->GetValAsMatrix(-1);
      ret_img->CopyFrom(cur_img_grey.ptr());
    }

    col = data_table->FindMakeColName
      (name + "_image_r_g" + col_sufx, idx, DataTable::VT_FLOAT, 2,
       input_size.retina_size.x, input_size.retina_size.y);
    if(!fmt_only) {
      float_MatrixPtr ret_img; ret_img = (float_Matrix*)col->GetValAsMatrix(-1);
      ret_img->CopyFrom(cur_img_LvM.ptr());
    }

    col = data_table->FindMakeColName
      (name + "_image_b_y" + col_sufx, idx, DataTable::VT_FLOAT, 2,
       input_size.retina_size.x, input_size.retina_size.y);
    if(!fmt_only) {
      float_MatrixPtr ret_img; ret_img = (float_Matrix*)col->GetValAsMatrix(-1);
      ret_img->CopyFrom(cur_img_SvLM.ptr());
    }
  }

  return true;
}


bool VisRegionSpecBase::FourDimMatrixToTable(DataTable* dtab, float_Matrix* out,
                                        const String& col_nm, bool fmt_only) {
  DataCol* col;
  int idx;
  col = data_table->FindMakeColName
    (name + col_nm, idx, DataTable::VT_FLOAT, 4,
     out->dim(0), out->dim(1), out->dim(2), out->dim(3));
  if(!fmt_only) {
    float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
    dout->CopyFrom(out);
  }
  return true;
}

bool VisRegionSpecBase::OutSaveOk(DataSave save_flags) {
  if(!(save_flags & SAVE_DATA)) return false;
  if(!(save_flags & ONLY_GUI)) return true;
  return taMisc::gui_active;
}

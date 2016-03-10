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

#include "DoGRegionSpec.h"
#include <taProject>
#include <DataTable>

#include <ta_vector_ops.h>
#include <taMath_float>
#include <taMisc>


TA_BASEFUNS_CTORS_DEFN(GradFilter);
TA_BASEFUNS_CTORS_DEFN(DoGRegionSpec);


void GradFilter::Initialize() {
  on = true;
  size = 16;
  spacing = 8;
  n_angles = 4;
  gain = 2.0f;
  circle_edge = true;
  filter.SetGeom(3, size, size, n_angles);
}

void GradFilter::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  UpdateFilters();
}

void GradFilter::RenderFilters(float_Matrix& fltrs) {
  fltrs.SetGeom(3, size, size, n_angles);

  float ctr = (float)(size-1) / 2.0f;
  float ang_inc = taMath_float::pi / (float)n_angles;

  float circ_radius = (float)(size) / 2.0f;

  for(int ang = 0; ang < n_angles; ang++) {
    float angf = -(float)ang * ang_inc;

    float pos_sum = 0.0f;
    float neg_sum = 0.0f;
    for(int x = 0; x < size; x++) {
      for(int y = 0; y < size; y++) {
        float xf = (float)x - ctr;
        float yf = (float)y - ctr;

        float dist = taMath_float::hypot(xf, yf);
        float val = 0.0f;
        if(!(circle_edge && (dist > circ_radius))) {
          // float nx = xf * cosf(angf) - yf * sinf(angf);
          float ny = yf * cosf(angf) + xf * sinf(angf);
          val = ny;             // simple linear gradient in ny
          if(val > 0.0f)        { pos_sum += val; }
          else if(val < 0.0f)   { neg_sum += val; }
        }
        fltrs.FastEl3d(x, y, ang) = val;
      }
    }
    // renorm each half
    float pos_norm = 1.0f / pos_sum;
    float neg_norm = -1.0f / neg_sum;
    for(int x = 0; x < size; x++) {
      for(int y = 0; y < size; y++) {
        float& val = fltrs.FastEl3d(x, y, ang);
        if(val > 0.0f)          { val *= pos_norm; }
        else if(val < 0.0f)     { val *= neg_norm; }
      }
    }
  }
}

void GradFilter::UpdateFilters() {
  RenderFilters(filter);
}

void GradFilter::GridFilters(DataTable* graph_data, bool reset) {
  UpdateFilters();

  String name;
  if(owner) name = owner->GetName();

  taProject* proj = GetMyProj();
  if(!graph_data) {
    graph_data = proj->GetNewAnalysisDataTable(name + "_Grad_GridFilters", true);
  }
  graph_data->StructUpdate(true);
  if(reset)
    graph_data->ResetData();
  int idx;
  DataCol* nmda = graph_data->FindMakeColName("Name", idx, VT_STRING);
//   nmda->SetUserData("WIDTH", 10);
  DataCol* matda = graph_data->FindMakeColName("Filter", idx, VT_FLOAT, 2, size, size);

  float maxv = taMath_float::vec_abs_max(&filter, idx);

  graph_data->SetUserData("N_ROWS", 4);
  graph_data->SetUserData("SCALE_MIN", -maxv);
  graph_data->SetUserData("SCALE_MAX", maxv);
  graph_data->SetUserData("BLOCK_HEIGHT", 0.0f);

  int ang_inc = 180 / n_angles;

  for(int ang=0; ang<n_angles; ang++) {
    graph_data->AddBlankRow();
    float_MatrixPtr frm; frm = (float_Matrix*)filter.GetFrameSlice(ang);
    matda->SetValAsMatrix(frm, -1);
    nmda->SetValAsString("Angle: " + String(ang * ang_inc), -1);
  }

  graph_data->StructUpdate(false);
  graph_data->FindMakeGridView();
}

// for thread function calling:
typedef void (DoGRegionSpec::*DoGRegionMethod)(int, int);

void DoGRegionSpec::Initialize() {
  dog_specs.on = true;
  dog_specs.half_size = 4;
  dog_specs.on_sigma = 1;
  dog_specs.off_sigma = 2;
  dog_specs.circle_edge = true;
  dog_renorm = LOG_RENORM;
  dog_save = SAVE_DATA;
  dog_feat_geom.x = 1;
  dog_feat_geom.y = 2;
  dog_feat_geom.n = 2;
  dog_img_geom = 144;

  grad_specs.on = false;
  grad_renorm = LOG_RENORM;
  grad_save = SAVE_DATA;

  kwta.mode = V1KwtaSpec::OFF;
  kwta.gi = 2.0f;
  kwta.lay_gi = 1.5f;
  kwta.gp_k = 1;
  kwta.gp_g = 0.02f;
}

void DoGRegionSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  dog_specs.name = name;
  dog_specs.UpdateAfterEdit_NoGui();
  grad_specs.UpdateAfterEdit_NoGui();
  kwta.UpdateAfterEdit_NoGui();
}

void DoGRegionSpec::UpdateGeom() {
  inherited::UpdateGeom();

  // 0 1 2 3 4 5 6 7 8  retina_size = 9
  // b b .   .   . b b  border = 2, spacing = 2: input_size = 5, output_size = 3

  // wrap mode:
  // 0 1 2 3 4 5 6 7  retina_size = 8
  // .   .   .   .    border = 0, spacing = 2; input_size = 8, output_size = 4

  if(region.edge_mode == VisRegionParams::WRAP) {
    dog_img_geom = input_size.input_size / dog_specs.spacing;
  }
  else {
    dog_img_geom = ((input_size.input_size - 1) / dog_specs.spacing) + 1;
  }
  if(region.edge_mode == VisRegionParams::WRAP) {
    grad_img_geom = input_size.input_size / grad_specs.spacing;
  }
  else {
    grad_img_geom = ((input_size.input_size - 1) / grad_specs.spacing) + 1;
  }
  if(region.color == VisRegionParams::COLOR) {
    dog_feat_geom.SetXYN(4,2,8);
  }
  else {
    dog_feat_geom.SetXYN(1,2,2);
  }
  grad_feat_geom.SetXYN(grad_specs.n_angles, 2, 2 * grad_specs.n_angles);

  combined_out = false;
  if(dog_img_geom == grad_img_geom &&
     dog_specs.on && grad_specs.on && !(dog_save & SEP_MATRIX) &&
     !(grad_save & SEP_MATRIX) && grad_specs.n_angles == 4) {
    combined_out = true;
    if(region.color == VisRegionParams::COLOR) {
      cmb_feat_geom.SetXYN(4,4,16); // assuming 4 angles..
    }
    else {
      cmb_feat_geom.SetXYN(5,2,10); // color on / off are 1st 2
    }
  }
}

String DoGRegionSpec::GetDoGFiltName(int flt_no) {
  String nm;
  if(region.color == VisRegionParams::COLOR) {
    switch(flt_no) {
    case 0: nm = "on"; break;
    case 1: nm = "rvc"; break;
    case 2: nm = "gvm"; break;
    case 3: nm = "bvy"; break;
    case 4: nm = "off"; break;
    case 5: nm = "cvr"; break;
    case 6: nm = "mvg"; break;
    case 7: nm = "yvb"; break;
    }
  }
  else {
    if(flt_no == 0) nm = "on";
    else            nm = "off";
  }
  return nm;
}

bool DoGRegionSpec::InitFilters() {
  inherited::InitFilters();
  UpdateGeom();
  dog_specs.UpdateFilter();
  grad_specs.UpdateFilters();
  return true;
}

bool DoGRegionSpec::InitOutMatrix() {
  inherited::InitOutMatrix();

  if(dog_specs.on) {
    dog_out_r.SetGeom(4, dog_feat_geom.x, dog_feat_geom.y, dog_img_geom.x, dog_img_geom.y);
    if(region.ocularity == VisRegionParams::BINOCULAR)
      dog_out_l.SetGeom(4, dog_feat_geom.x, dog_feat_geom.y, dog_img_geom.x,
                        dog_img_geom.y);
    else
      dog_out_l.SetGeom(1,1);     // free memory
  }
  else {
    dog_out_r.SetGeom(1,1);     // free memory
    dog_out_l.SetGeom(1,1);     // free memory
  }
    
  if(grad_specs.on) {
    grad_out_r.SetGeom(4, grad_feat_geom.x, grad_feat_geom.y, grad_img_geom.x,
                       grad_img_geom.y);
    if(region.ocularity == VisRegionParams::BINOCULAR)
      grad_out_l.SetGeom(4, grad_feat_geom.x, grad_feat_geom.y, grad_img_geom.x,
                         grad_img_geom.y);
    else
      grad_out_l.SetGeom(1,1);     // free memory
  }
  else {
    grad_out_r.SetGeom(1,1);     // free memory
    grad_out_l.SetGeom(1,1);     // free memory
  }

  if(combined_out) {
    cmb_out_r.SetGeom(4, cmb_feat_geom.x, cmb_feat_geom.y, grad_img_geom.x,
                       grad_img_geom.y);
    if(region.ocularity == VisRegionParams::BINOCULAR)
      cmb_out_l.SetGeom(4, cmb_feat_geom.x, cmb_feat_geom.y, grad_img_geom.x,
                         grad_img_geom.y);
    else
      cmb_out_l.SetGeom(1,1);     // free memory

    if(kwta.On()) {
      kwta_out_r.SetGeomN(cmb_out_r.geom);
      kwta_out_l.SetGeomN(cmb_out_l.geom);
    }
  }
  else {
    cmb_out_r.SetGeom(1,1);     // free memory
    cmb_out_l.SetGeom(1,1);     // free memory

    if(kwta.On()) {
      if(grad_specs.on) {
        kwta_out_r.SetGeomN(grad_out_r.geom);
        kwta_out_l.SetGeomN(grad_out_l.geom);
      }
      else if(dog_specs.on) {
        kwta_out_r.SetGeomN(dog_out_r.geom);
        kwta_out_l.SetGeomN(dog_out_l.geom);
      }
    }
  }

  return true;
}

bool DoGRegionSpec::NeedsInit() {
  if(dog_specs.on) { 
    if((dog_out_r.dims() < 4) ||
       (dog_out_r.dim(0) * dog_out_r.dim(1) != dog_feat_geom.n) ||
       (dog_out_r.dim(2) != dog_img_geom.x) ||
       (dog_out_r.dim(3) != dog_img_geom.y))
      return true;

    if(region.ocularity == VisRegionParams::BINOCULAR) {
      if((dog_out_l.dims() < 4) ||
         (dog_out_l.dim(0) * dog_out_l.dim(1) != dog_feat_geom.n) ||
         (dog_out_l.dim(2) != dog_img_geom.x) ||
         (dog_out_l.dim(3) != dog_img_geom.y))
        return true;
    }
  }
  if(grad_specs.on) { 
    if((grad_out_r.dims() < 4) ||
       (grad_out_r.dim(0) * grad_out_r.dim(1) != grad_feat_geom.n) ||
       (grad_out_r.dim(2) != grad_img_geom.x) ||
       (grad_out_r.dim(3) != grad_img_geom.y))
      return true;

    if(region.ocularity == VisRegionParams::BINOCULAR) {
      if((grad_out_l.dims() < 4) ||
         (grad_out_l.dim(0) * grad_out_l.dim(1) != grad_feat_geom.n) ||
         (grad_out_l.dim(2) != grad_img_geom.x) ||
         (grad_out_l.dim(3) != grad_img_geom.y))
        return true;
    }
  }
  return false;
}

////////////////////////////////////////////////////////////////////
//      DoGRegion       Filtering

bool DoGRegionSpec::FilterImage_impl(bool motion_only) {
  inherited::FilterImage_impl(motion_only);

  bool rval = true;
  if(dog_specs.on) {
    DoGFilterImage(cur_img_r, &dog_out_r);
    if(region.ocularity == VisRegionParams::BINOCULAR) {
      DoGFilterImage(cur_img_l, &dog_out_l);
    }
  }

  if(grad_specs.on) {
    GradFilterImage(cur_img_r, &grad_out_r);
    if(region.ocularity == VisRegionParams::BINOCULAR) {
      GradFilterImage(cur_img_l, &grad_out_l);
    }
  }

  if(combined_out) {
    CombineOuts(&cmb_out_r, &dog_out_r, &grad_out_r);
    if(kwta.On()) {
      kwta.Compute_Inhib(cmb_out_r, kwta_out_r, kwta_gci);
      memcpy(cmb_out_r.el, kwta_out_r.el, cmb_out_r.size * sizeof(float));
    }
    if(region.ocularity == VisRegionParams::BINOCULAR) {
      CombineOuts(&cmb_out_l, &dog_out_l, &grad_out_l);
      if(kwta.On()) {
        kwta.Compute_Inhib(cmb_out_l, kwta_out_l, kwta_gci);
        memcpy(cmb_out_l.el, kwta_out_l.el, cmb_out_l.size * sizeof(float));
      }
    }
  }
  else if(kwta.On()) {
    if(grad_specs.on) {
      kwta.Compute_Inhib(grad_out_r, kwta_out_r, kwta_gci);
      memcpy(grad_out_r.el, kwta_out_r.el, grad_out_r.size * sizeof(float));
      if(region.ocularity == VisRegionParams::BINOCULAR) {
        kwta.Compute_Inhib(grad_out_l, kwta_out_l, kwta_gci);
        memcpy(grad_out_l.el, kwta_out_l.el, grad_out_l.size * sizeof(float));
      }
    }
    else if(dog_specs.on) {
      kwta.Compute_Inhib(dog_out_r, kwta_out_r, kwta_gci);
      memcpy(dog_out_r.el, kwta_out_r.el, dog_out_r.size * sizeof(float));
      if(region.ocularity == VisRegionParams::BINOCULAR) {
        kwta.Compute_Inhib(dog_out_l, kwta_out_l, kwta_gci);
        memcpy(dog_out_l.el, kwta_out_l.el, dog_out_l.size * sizeof(float));
      }
    }      
  }
  
  if(!data_table || save_mode == NONE_SAVE) // bail now
    return rval;

  if(dog_save & SAVE_DATA && !(!taMisc::gui_active && dog_save & ONLY_GUI)) {
    DoGOutputToTable(data_table);
  }
  if(grad_save & SAVE_DATA && !(!taMisc::gui_active && grad_save & ONLY_GUI)) {
    GradOutputToTable(data_table);
  }
  if(combined_out) {
    CombinedOutputToTable(data_table);
  }

  return rval;
}

bool DoGRegionSpec::DoGFilterImage(float_Matrix* image, float_Matrix* out) {
  cur_img = image;
  cur_out = out;
  rgb_img = (cur_img->dims() == 3);
  wrap = (region.edge_mode == VisRegionParams::WRAP);

  if(rgb_img) {
    ColorRGBtoCMYK(*cur_img);   // precompute!
  }

  IMG_THREAD_CALL(DoGRegionSpec::DoGFilterImage_thread);

  // renormalize -- todo: could thread this perhaps, but chunk size would have to be larger probably
  if(dog_renorm != NO_RENORM) {
    RenormOutput(dog_renorm, out);
  }

  return true;
}

void DoGRegionSpec::DoGFilterImage_thread(int thr_no) {
  taVector2i st;
  taVector2i ed;
  GetThread2DGeom(thr_no, dog_img_geom, st, ed);

  const float* flt = (const float*)dog_specs.net_filter.data();
  int   flt_wd = dog_specs.half_size; // half-size
  int   flt_wdf = dog_specs.size;     // full-size
  int   flt_vecw = flt_wdf / 4;
  flt_vecw *= 4;

  taVector2i st_ne = (flt_wd - input_size.border); // no edge
  taVector2i ed_ne = dog_img_geom - (flt_wd - input_size.border);

  taVector2i oc;         // current coord -- output space
  taVector2i ic;         // input coord

  float_Matrix* dog_img = cur_img;

  for(oc.y = st.y; oc.y < ed.y; oc.y++) {
    bool y_ne = (oc.y >= st_ne.y && oc.y < ed_ne.y); // y no edge
    for(oc.x = st.x; oc.x < ed.x; oc.x++) {
      bool ne = y_ne && (oc.x >= st_ne.x && oc.x < ed_ne.x); // no edge

      taVector2i icc = input_size.border + dog_specs.spacing * oc; // image coords center

      // y = on/off, x = color channel
      for(int chan = 0; chan < dog_feat_geom.x; chan++) {
        ColorChannel cchan = (ColorChannel)chan;
        if(rgb_img) {
          dog_img = GetImageForChan(cchan);
        }

        float cnv_sum = 0.0f;               // convolution sum
        if(chan == 0 || rgb_img) {          // only rgb images if chan > 0
          int fi = 0;
          for(int yf = -flt_wd; yf <= flt_wd; yf++) {
            if(ne) {
              int img_st = dog_img->FastElIndex2d(icc.x - flt_wd, icc.y + yf);
#ifdef TA_VEC_USE
              int xf;
              for(xf = 0; xf < flt_vecw; xf+= 4, fi+=4) {
                Vec4f ivals;  ivals.load(dog_img->el + img_st + xf);
                Vec4f fvals;  fvals.load(flt + fi);
                Vec4f prod = ivals * fvals;
                cnv_sum += horizontal_add(prod);
              }
              for(; xf < flt_wdf; xf++, fi++) { // get the residuals
                cnv_sum += dog_img->FastEl_Flat(img_st + xf) * flt[fi];
              }
#else              
              for(int xf = 0; xf < flt_wdf; xf++, fi++) {
                cnv_sum += dog_img->FastEl_Flat(img_st + xf) * flt[fi];
              }
#endif              
            }
            else {
              for(int xf = -flt_wd; xf <= flt_wd; xf++, fi++) {
                ic.y = icc.y + yf;
                ic.x = icc.x + xf;
                if(ic.WrapClip(wrap, input_size.retina_size)) {
                  if(region.edge_mode == VisRegionParams::CLIP) continue;
                }
                cnv_sum += dog_img->FastEl2d(ic.x, ic.y) * flt[fi];
              }
            }
          }
        }
        cnv_sum *= dog_specs.gain;
        
        if(cnv_sum >= 0.0f) {
          cur_out->FastEl4d(chan, 0, oc.x, oc.y) = cnv_sum; // feat x = 0 = on
          cur_out->FastEl4d(chan, 1, oc.x, oc.y) = 0.0f;      // feat x = 1 = off
        }
        else {
          cur_out->FastEl4d(chan, 0, oc.x, oc.y) = 0.0f;      // feat x = 0 = on
          cur_out->FastEl4d(chan, 1, oc.x, oc.y) = -cnv_sum; // feat x = 1 = off
        }
      }
    }
  }
}

bool DoGRegionSpec::GradFilterImage(float_Matrix* image, float_Matrix* out) {
  cur_img = image;
  cur_out = out;
  rgb_img = (cur_img->dims() == 3);
  wrap = (region.edge_mode == VisRegionParams::WRAP);

  if(rgb_img) {
    ColorRGBtoCMYK(*cur_img);   // precompute!
  }

  IMG_THREAD_CALL(DoGRegionSpec::GradFilterImage_thread);

  // renormalize -- todo: could thread this perhaps, but chunk size would have to be larger probably
  if(grad_renorm != NO_RENORM) {
    RenormOutput(grad_renorm, out);
  }

  return true;
}

void DoGRegionSpec::GradFilterImage_thread(int thr_no) {
  taVector2i st;
  taVector2i ed;
  GetThread2DGeom(thr_no, dog_img_geom, st, ed);

  int flt_wdf = grad_specs.size; // full-width
  int flt_wd;                          // half-width
  if(flt_wdf % 2 == 0)
    flt_wd = flt_wdf / 2;
  else
    flt_wd = (flt_wdf-1) / 2;

  int flt_vecw = flt_wdf / 4;
  flt_vecw *= 4;

  taVector2i in_off = input_size.border - flt_wd;
  
  taVector2i st_ne = -in_off; // skip over start..
  taVector2i ed_ne = grad_img_geom + in_off;
  ed_ne -= flt_wdf;

  // taMisc::Info("st:", st.GetStr(), "ed:", ed.GetStr(),
  //              "st_ne:", st_ne.GetStr(), "ed_ne:", ed_ne.GetStr());
  
  taVector2i oc;         // current coord -- output space
  taVector2i ic;         // input coord

  float_Matrix* v1s_img = cur_img;

  for(oc.y = st.y; oc.y < ed.y; oc.y++) {
    bool y_ne = (oc.y >= st_ne.y && oc.y < ed_ne.y); // y no edge
    for(oc.x = st.x; oc.x < ed.x; oc.x++) {
      bool ne = y_ne && (oc.x >= st_ne.x && oc.x < ed_ne.x); // no edge

      taVector2i icc = in_off + grad_specs.spacing * oc; // image coords center

      ColorChannel cchan = (ColorChannel)0;
      if(rgb_img) {
        v1s_img = GetImageForChan(cchan);
      }

      for(int ang = 0; ang < grad_specs.n_angles; ang++) {
        const float* flt = (const float*)grad_specs.filter.el +
          grad_specs.filter.FastElIndex3d(0, 0, ang);
        
        float cnv_sum = 0.0f;               // convolution sum
        int fi = 0;
        for(int yf = 0; yf < flt_wdf; yf++) {
          if(ne) {
            int img_st = v1s_img->FastElIndex2d(icc.x, icc.y + yf);
#ifdef TA_VEC_USE
            int xf;
            for(xf = 0; xf < flt_vecw; xf+= 4, fi+=4) {
              Vec4f ivals;  ivals.load(v1s_img->el + img_st + xf);
              Vec4f fvals;  fvals.load(flt + fi);
              Vec4f prod = ivals * fvals;
              cnv_sum += horizontal_add(prod);
            }
            for(; xf < flt_wdf; xf++, fi++) { // get the residuals
              cnv_sum += v1s_img->FastEl_Flat(img_st + xf) * flt[fi];
            }
#else              
            for(int xf = 0; xf < flt_wdf; xf++, fi++) {
              cnv_sum += v1s_img->FastEl_Flat(img_st + xf) * flt[fi];
            }
#endif              
          }
          else {
            for(int xf = 0; xf < flt_wdf; xf++, fi++) {
              ic.y = icc.y + yf;
              ic.x = icc.x + xf;
              if(ic.WrapClip(wrap, input_size.retina_size)) {
                if(region.edge_mode == VisRegionParams::CLIP) continue;
              }
              cnv_sum += v1s_img->FastEl2d(ic.x, ic.y) * flt[fi];
            }
          }
        }
        cnv_sum *= grad_specs.gain;
        if(cnv_sum >= 0.0f) {
          cur_out->FastEl4d(ang, 0, oc.x, oc.y) = cnv_sum; // on-polarity
          cur_out->FastEl4d(ang, 1, oc.x, oc.y) = 0.0f;
        }
        else {
          cur_out->FastEl4d(ang, 0, oc.x, oc.y) = 0.0f;
          cur_out->FastEl4d(ang, 1, oc.x, oc.y) = -cnv_sum; // off-polarity
        }
      }
    }
  }
}


bool DoGRegionSpec::CombineOuts(float_Matrix* cmb, float_Matrix* dog_out,
                                float_Matrix* grad_out) {
  if(region.color == VisRegionParams::COLOR) {
    for(int oy=0; oy<dog_img_geom.y; oy++) {
      for(int ox=0; ox<dog_img_geom.x; ox++) {
        for(int fy=0; fy<dog_feat_geom.y; fy++) {
          for(int fx=0; fx<dog_feat_geom.x; fx++) {
            cmb->FastEl4d(fx, fy, ox, oy) = dog_out->FastEl4d(fx, fy, ox, oy);
            cmb->FastEl4d(fx, fy, ox, oy) = dog_out->FastEl4d(fx, fy, ox, oy);
            
            cmb->FastEl4d(fx, fy+2, ox, oy) = grad_out->FastEl4d(fx, fy, ox, oy);
            cmb->FastEl4d(fx, fy+2, ox, oy) = grad_out->FastEl4d(fx, fy, ox, oy);
          }
        }
      }
    }
  }
  else {
    for(int oy=0; oy<dog_img_geom.y; oy++) {
      for(int ox=0; ox<dog_img_geom.x; ox++) {
        for(int fy=0; fy<dog_feat_geom.y; fy++) {
          for(int fx=0; fx<dog_feat_geom.x; fx++) {
            cmb->FastEl4d(fx, fy, ox, oy) = dog_out->FastEl4d(fx, fy, ox, oy);
            cmb->FastEl4d(fx, fy, ox, oy) = dog_out->FastEl4d(fx, fy, ox, oy);
          }
        }
        for(int fy=0; fy<grad_feat_geom.y; fy++) {
          for(int fx=0; fx<grad_feat_geom.x; fx++) {
            cmb->FastEl4d(fx+1, fy, ox, oy) = grad_out->FastEl4d(fx, fy, ox, oy);
            cmb->FastEl4d(fx+1, fy, ox, oy) = grad_out->FastEl4d(fx, fy, ox, oy);
          }
        }
      }
    }
  }

  return true;
}


////////////////////////////////////////////////////////////////////
//      DoGRegion       Data Table Output

bool DoGRegionSpec::InitDataTable() {
  inherited::InitDataTable();

  int idx;
  if(dog_save & SAVE_DATA) {
    DoGOutputToTable(data_table, true); // format only
  }
  if(grad_save & SAVE_DATA) {
    GradOutputToTable(data_table, true); // format only
  }
  if(combined_out) {
    CombinedOutputToTable(data_table, true); // format only
  }
  return true;
}

bool DoGRegionSpec::DoGOutputToTable(DataTable* dtab, bool fmt_only) {
  DoGOutputToTable_impl(dtab, &dog_out_r, "_r", fmt_only);
  if(region.ocularity == VisRegionParams::BINOCULAR)
    DoGOutputToTable_impl(dtab, &dog_out_l, "_l", fmt_only);
  return true;
}

bool DoGRegionSpec::DoGOutputToTable_impl(DataTable* dtab, float_Matrix* out,
                                          const String& col_sufx, bool fmt_only) {
  DataCol* col;
  int idx;
  if(dog_save & SEP_MATRIX) {
    taVector2i dc;               // dog coords
    taVector2i fc;               // feature coords
    for(int i=0;i<dog_feat_geom.n;i++) {
      fc.SetFmIndex(i, dog_feat_geom.x);
      String nm = name + "_" + GetDoGFiltName(i) + "_dog";
      col = data_table->FindMakeColName(nm+ col_sufx, idx, DataTable::VT_FLOAT, 2,
                                  dog_img_geom.x, dog_img_geom.y);
      if(!fmt_only) {
        float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
        for(dc.y = 0; dc.y < dog_img_geom.y; dc.y++) {
          for(dc.x = 0; dc.x < dog_img_geom.x; dc.x++) {
            float val = out->FastEl4d(fc.x, fc.y, dc.x, dc.y);
            dout->FastEl2d(dc.x, dc.y) = val;
          }
        }
      }
    }
  }
  else {
    col = data_table->FindMakeColName(name + "_dog" + col_sufx, idx,
                                      DataTable::VT_FLOAT, 4,
                     dog_feat_geom.x, dog_feat_geom.y, dog_img_geom.x, dog_img_geom.y);
    if(!fmt_only) {
      float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
      dout->CopyFrom(out);
    }
  }
  return true;
}

bool DoGRegionSpec::GradOutputToTable(DataTable* dtab, bool fmt_only) {
  GradOutputToTable_impl(dtab, &grad_out_r, "_r", fmt_only);
  if(region.ocularity == VisRegionParams::BINOCULAR)
    GradOutputToTable_impl(dtab, &grad_out_l, "_l", fmt_only);
  return true;
}

bool DoGRegionSpec::GradOutputToTable_impl(DataTable* dtab, float_Matrix* out,
                                          const String& col_sufx, bool fmt_only) {
  DataCol* col;
  int idx;
  col = data_table->FindMakeColName(name + "_grad" + col_sufx, idx,
                                    DataTable::VT_FLOAT, 4,
                  grad_feat_geom.x, grad_feat_geom.y, grad_img_geom.x, grad_img_geom.y);
  if(!fmt_only) {
    float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
    dout->CopyFrom(out);
  }
  return true;
}

bool DoGRegionSpec::CombinedOutputToTable(DataTable* dtab, bool fmt_only) {
  CombinedOutputToTable_impl(dtab, &cmb_out_r, "_r", fmt_only);
  if(region.ocularity == VisRegionParams::BINOCULAR)
    CombinedOutputToTable_impl(dtab, &cmb_out_l, "_l", fmt_only);
  return true;
}

bool DoGRegionSpec::CombinedOutputToTable_impl(DataTable* dtab, float_Matrix* out,
                                          const String& col_sufx, bool fmt_only) {
  DataCol* col;
  int idx;
  col = data_table->FindMakeColName(name + "_cmb" + col_sufx, idx,
                                    DataTable::VT_FLOAT, 4,
                        cmb_feat_geom.x, cmb_feat_geom.y, dog_img_geom.x, dog_img_geom.y);
  if(!fmt_only) {
    float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
    dout->CopyFrom(out);
  }
  return true;
}



void DoGRegionSpec::GraphDoGFilter(DataTable* graph_data) {
  dog_specs.GraphFilter(graph_data);
}

void DoGRegionSpec::GridDoGFilter(DataTable* graph_data) {
  dog_specs.GridFilter(graph_data);
}

void DoGRegionSpec::GridGradFilter(DataTable* graph_data) {
  grad_specs.GridFilters(graph_data);
}

void DoGRegionSpec::PlotSpacing(DataTable* graph_data, bool reset) {
  taProject* proj = GetMyProj();
  if(!graph_data) {
    graph_data = proj->GetNewAnalysisDataTable(name + "_DoG_PlotSpacing", true);
  }
  graph_data->StructUpdate(true);
  if(reset)
    graph_data->Reset();
  int idx;
  DataCol* nmda = graph_data->FindMakeColName("Name", idx, VT_STRING);
  DataCol* matda = graph_data->FindMakeColName("Spacing", idx, VT_FLOAT, 2,
                                              input_size.retina_size.x, input_size.retina_size.y);
  graph_data->SetUserData("N_ROWS", 1);
  graph_data->SetUserData("BLOCK_HEIGHT", 0.0f);
  graph_data->SetUserData("BLOCK_SPACE", 20.0f);
  graph_data->SetUserData("WIDTH", 1.0f + (float)input_size.retina_size.x / (float)input_size.retina_size.y);

  graph_data->AddBlankRow();
  nmda->SetValAsString("DoG", -1);
  float_MatrixPtr mat; mat = (float_Matrix*)matda->GetValAsMatrix(-1);
  if(mat) {
    taVector2i ic;
    int x,y;
    for(y=input_size.border.y; y<= input_size.retina_size.y-input_size.border.y; y+= dog_specs.spacing) {
      for(x=input_size.border.x; x<= input_size.retina_size.x-input_size.border.x; x+=dog_specs.spacing) {
        ic.y = y; ic.x = x;
        ic.WrapHalf(input_size.retina_size);      mat->FastEl2d(ic.x,ic.y) = 1.0f;
      }
    }
  }

  graph_data->StructUpdate(false);
  graph_data->FindMakeGridView();
}

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


TA_BASEFUNS_CTORS_DEFN(DoGRegionSpec);

// for thread function calling:
typedef void (DoGRegionSpec::*DoGRegionMethod)(int, int);

void DoGRegionSpec::Initialize() {
  dog_specs.on = true;
  dog_specs.half_size = 4;
  dog_specs.on_sigma = 1;
  dog_specs.off_sigma = 2;
  dog_specs.circle_edge = true;
  dog_specs_2.on = false;
  dog_specs_3.on = false;

  dog_color_only = true;
  dog_renorm = LOG_RENORM;
  dog_save = SAVE_DATA;
  dog_feat_geom.x = 1;
  dog_feat_geom.y = 2;
  dog_feat_geom.n = 2;
  dog_img_geom = 144;

  dog_kwta.on = false;
  dog_kwta.gi = 2.0f;
  dog_kwta.lay_gi = 1.5f;

  n_colors = 1;
  n_dogs = 1;
}

void DoGRegionSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  dog_specs.on = true;          // at least one must always be selected!
  dog_specs.name = name;
  dog_specs.UpdateAfterEdit_NoGui();
  dog_specs_2.name = name;
  dog_specs_2.UpdateAfterEdit_NoGui();
  dog_specs_3.name = name;
  dog_specs_3.UpdateAfterEdit_NoGui();
  dog_kwta.UpdateAfterEdit_NoGui();
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

  n_colors = 1;
  if(region.color == VisRegionParams::COLOR) {
    if(dog_color_only)
      n_colors = 2;
    else
      n_colors = 3;
  }
  else {
    n_colors = 1;
  }

  n_dogs = 1;
  if(dog_specs_2.on) {
    n_dogs++;
  }
  if(dog_specs_3.on) {
    n_dogs++;
  }

  dog_feat_geom.x = n_dogs * n_colors;
  dog_feat_geom.y = 2;
  dog_feat_geom.UpdateNfmXY();
}

String DoGRegionSpec::GetDoGFiltName(int flt_no) {
  String nm;
  if(region.color == VisRegionParams::COLOR) {
    if(dog_color_only) {
      switch(flt_no) {
      case 0: nm = "r_g"; break;
      case 1: nm = "b_y"; break;
      case 2: nm = "g_r"; break;
      case 3: nm = "y_b"; break;
      }
    }
    else {
      switch(flt_no) {
      case 0: nm = "on"; break;
      case 1: nm = "r_g"; break;
      case 2: nm = "b_y"; break;
      case 3: nm = "off"; break;
      case 4: nm = "g_r"; break;
      case 5: nm = "y_b"; break;
      }
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
  dog_specs_2.UpdateFilter();
  dog_specs_3.UpdateFilter();
  return true;
}

bool DoGRegionSpec::InitOutMatrix() {
  inherited::InitOutMatrix();

  dog_out_r.SetGeom(4, dog_feat_geom.x, dog_feat_geom.y, dog_img_geom.x, dog_img_geom.y);
  if(region.ocularity == VisRegionParams::BINOCULAR)
    dog_out_l.SetGeomN(dog_out_r.geom);
  else
    dog_out_l.SetGeom(1,1);     // free memory
    
  if(dog_kwta.On()) {
    dog_raw_r.SetGeomN(dog_out_r.geom);
    dog_raw_l.SetGeomN(dog_out_l.geom);
  }
  else {
    dog_raw_r.SetGeom(1,1);
    dog_raw_l.SetGeom(1,1);
  }

  return true;
}

bool DoGRegionSpec::NeedsInit() {
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
  return false;
}

////////////////////////////////////////////////////////////////////
//      DoGRegion       Filtering

bool DoGRegionSpec::FilterImage_impl(bool motion_only) {
  inherited::FilterImage_impl(motion_only);

  if(dog_kwta.On()) {
    cur_out_r = &dog_raw_r;
    cur_out_l = &dog_raw_l;
  }
  else {
    cur_out_r = &dog_out_r;
    cur_out_l = &dog_out_l;
  }
  
  bool rval = true;
  DoGFilterImage(cur_img_r, cur_out_r);
  if(region.ocularity == VisRegionParams::BINOCULAR) {
    DoGFilterImage(cur_img_l, cur_out_l);
  }

  if(dog_kwta.On()) {
    dog_kwta.Compute_Inhib(dog_raw_r, dog_out_r, kwta_gci);
    if(region.ocularity == VisRegionParams::BINOCULAR) {
      dog_kwta.Compute_Inhib(dog_raw_l, dog_out_l, kwta_gci);
    }
  }
  
  if(!data_table || save_mode == NONE_SAVE) // bail now
    return rval;

  if(dog_save & OutSaveOk(dog_save)) {
    DoGOutputToTable(data_table);
  }
  
  return rval;
}

bool DoGRegionSpec::DoGFilterImage(float_Matrix* image, float_Matrix* out) {
  cur_img = image;
  cur_out = out;
  rgb_img = (cur_img->dims() == 3);
  wrap = (region.edge_mode == VisRegionParams::WRAP);

  if(rgb_img) {
    PrecomputeColor(cur_img, true);   // need rgb separate!
  }

  cur_dog_filter = &dog_specs;
  cur_dog_off = 0;
  IMG_THREAD_CALL(DoGRegionSpec::DoGFilterImage_thread);

  if(dog_specs_2.on) {
    cur_dog_filter = &dog_specs_2;
    cur_dog_off += n_colors;
    IMG_THREAD_CALL(DoGRegionSpec::DoGFilterImage_thread);
  }

  if(dog_specs_3.on) {
    cur_dog_filter = &dog_specs_3;
    cur_dog_off += n_colors;
    IMG_THREAD_CALL(DoGRegionSpec::DoGFilterImage_thread);
  }

  if(dog_renorm != NO_RENORM) {
    RenormOutput(dog_renorm, out);
  }

  return true;
}

void DoGRegionSpec::DoGFilterImage_thread(int thr_no) {
  taVector2i st;
  taVector2i ed;
  GetThread2DGeom(thr_no, dog_img_geom, st, ed);

  const float* net_flt = (const float*)cur_dog_filter->net_filter.data();
  const float* on_flt = (const float*)cur_dog_filter->on_filter.data();
  const float* off_flt = (const float*)cur_dog_filter->off_filter.data();
  int   flt_wd = cur_dog_filter->half_size; // half-size
  int   flt_wdf = cur_dog_filter->size;     // full-size
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

      taVector2i icc = input_size.border + cur_dog_filter->spacing * oc; // image coords center

      // y = on/off, x = color channel
      for(int chan = 0; chan < n_colors; chan++) {
        float cnv_sum = 0.0f;

        if(chan == 0 && !(dog_color_only && rgb_img)) { // monochrome
          ColorChannel cchan = (ColorChannel)chan;
          if(rgb_img) {
            dog_img = GetImageForChan(cchan);
          }
          int fi = 0;
          for(int yf = -flt_wd; yf <= flt_wd; yf++) {
            if(ne) {
              int img_st = dog_img->FastElIndex2d(icc.x - flt_wd, icc.y + yf);
#ifdef TA_VEC_USE
              int xf;
              for(xf = 0; xf < flt_vecw; xf+= 4, fi+=4) {
                Vec4f ivals;  ivals.load(dog_img->el + img_st + xf);
                Vec4f fvals;  fvals.load(net_flt + fi);
                Vec4f prod = ivals * fvals;
                cnv_sum += horizontal_add(prod);
              }
              for(; xf < flt_wdf; xf++, fi++) { // get the residuals
                cnv_sum += dog_img->FastEl_Flat(img_st + xf) * net_flt[fi];
              }
#else              
              for(int xf = 0; xf < flt_wdf; xf++, fi++) {
                cnv_sum += dog_img->FastEl_Flat(img_st + xf) * net_flt[fi];
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
                cnv_sum += dog_img->FastEl2d(ic.x, ic.y) * net_flt[fi];
              }
            }
          }
        } // monochrome
        else {
          // color
          int eff_chan = chan;
          if(dog_color_only) eff_chan++;
          float_Matrix* img_on;
          float_Matrix* img_off;
          if(eff_chan == 1) { // red vs. green
            img_on = GetImageForChan(RED);
            img_off = GetImageForChan(GREEN);
          }
          else {
            img_on = GetImageForChan(BLUE);
            img_off = GetImageForChan(YELLOW);
          }
          float on_sum = 0.0f;
          float off_sum = 0.0f;
          int fi = 0;
          for(int yf = -flt_wd; yf <= flt_wd; yf++) {
            if(ne) {
              int img_st = img_on->FastElIndex2d(icc.x - flt_wd, icc.y + yf);
#ifdef TA_VEC_USE
              int xf;
              for(xf = 0; xf < flt_vecw; xf+= 4, fi+=4) {
                Vec4f ivals_on;  ivals_on.load(img_on->el + img_st + xf);
                Vec4f fvals_on;  fvals_on.load(on_flt + fi);
                Vec4f ivals_off;  ivals_off.load(img_off->el + img_st + xf);
                Vec4f fvals_off;  fvals_off.load(off_flt + fi);
                Vec4f prod_on = ivals_on * fvals_on;
                Vec4f prod_off = ivals_off * fvals_off;
                on_sum += horizontal_add(prod_on);
                off_sum += horizontal_add(prod_off);
              }
              for(; xf < flt_wdf; xf++, fi++) { // get the residuals
                on_sum += img_on->FastEl_Flat(img_st + xf) * on_flt[fi];
                off_sum += img_off->FastEl_Flat(img_st + xf) * off_flt[fi];
              }
#else              
              for(int xf = 0; xf < flt_wdf; xf++, fi++) {
                on_sum += img_on->FastEl_Flat(img_st + xf) * on_flt[fi];
                off_sum += img_off->FastEl_Flat(img_st + xf) * off_flt[fi];
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
                on_sum += img_on->FastEl2d(ic.x, ic.y) * on_flt[fi];
                off_sum += img_off->FastEl2d(ic.x, ic.y) * off_flt[fi];
              }
            }
          }
          cnv_sum = cur_dog_filter->on_gain * on_sum - off_sum;
          // color
        }
        cnv_sum *= cur_dog_filter->gain;
        
        if(cnv_sum >= 0.0f) {
          cur_out->FastEl4d(cur_dog_off + chan, 0, oc.x, oc.y) = cnv_sum; // feat x = 0 = on
          cur_out->FastEl4d(cur_dog_off + chan, 1, oc.x, oc.y) = 0.0f;      // feat x = 1 = off
        }
        else {
          cur_out->FastEl4d(cur_dog_off + chan, 0, oc.x, oc.y) = 0.0f;      // feat x = 0 = on
          cur_out->FastEl4d(cur_dog_off + chan, 1, oc.x, oc.y) = -cnv_sum; // feat x = 1 = off
        }
      }
    }
  }
}

////////////////////////////////////////////////////////////////////
//      DoGRegion       Data Table Output

bool DoGRegionSpec::InitDataTable() {
  inherited::InitDataTable();

  int idx;
  if(dog_save & SAVE_DATA) {
    DoGOutputToTable(data_table, true); // format only
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

void DoGRegionSpec::GraphDoGFilter(DataTable* graph_data, int n_filter) {
  switch(n_filter) {
  case 1:
    dog_specs.GraphFilter(graph_data);
    break;
  case 2:
    dog_specs_2.GraphFilter(graph_data);
    break;
  case 3:
    dog_specs_3.GraphFilter(graph_data);
    break;
  }
}

void DoGRegionSpec::GridDoGFilter(DataTable* graph_data, int n_filter) {
  switch(n_filter) {
  case 1:
    dog_specs.GridFilter(graph_data);
    break;
  case 2:
    dog_specs_2.GridFilter(graph_data);
    break;
  case 3:
    dog_specs_3.GridFilter(graph_data);
    break;
  }    
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

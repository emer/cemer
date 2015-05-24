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

#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(DoGRegionSpec);


// for thread function calling:
typedef void (DoGRegionSpec::*DoGRegionMethod)(int, int);

void DoGRegionSpec::Initialize() {
  dog_specs.filter_width = 4;
  dog_specs.on_sigma = 1;
  dog_specs.off_sigma = 2;
  dog_specs.circle_edge = true;
  dog_renorm = LOG_RENORM;
  dog_save = SAVE_DATA;
  dog_feat_geom.x = 2;
  dog_feat_geom.y = 3;
  dog_feat_geom.n = 6;
  dog_img_geom = 144;
}

void DoGRegionSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  dog_specs.name = name;
  dog_specs.UpdateAfterEdit_NoGui();
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
  if(region.color == VisRegionParams::COLOR) {
    dog_feat_geom.SetXYN(2,4,8);
  }
  else {
    dog_feat_geom.SetXYN(2,1,2);
  }
}

String DoGRegionSpec::GetDoGFiltName(int flt_no) {
  String nm;
  switch(flt_no) {
  case 0: nm = "on"; break;
  case 1: nm = "off"; break;
  case 2: nm = "rvc"; break;
  case 3: nm = "cvr"; break;
  case 4: nm = "gvm"; break;
  case 5: nm = "mvg"; break;
  case 6: nm = "bvy"; break;
  case 7: nm = "yvb"; break;
  }
  return nm;
}

bool DoGRegionSpec::InitFilters() {
  inherited::InitFilters();
  UpdateGeom();
  dog_specs.UpdateFilter();
  return true;
}

bool DoGRegionSpec::InitOutMatrix() {
  inherited::InitOutMatrix();

  dog_out_r.SetGeom(4, dog_feat_geom.x, dog_feat_geom.y, dog_img_geom.x, dog_img_geom.y);
  if(region.ocularity == VisRegionParams::BINOCULAR)
    dog_out_l.SetGeom(4, dog_feat_geom.x, dog_feat_geom.y, dog_img_geom.x, dog_img_geom.y);
  else
    dog_out_l.SetGeom(1,1);     // free memory

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

  bool rval = DoGFilterImage(cur_img_r, &dog_out_r);
  if(rval && region.ocularity == VisRegionParams::BINOCULAR) {
    rval &= DoGFilterImage(cur_img_l, &dog_out_l);
  }

  if(!data_table || save_mode == NONE_SAVE) // bail now
    return rval;

  if(dog_save & SAVE_DATA && !(!taMisc::gui_active && dog_save & ONLY_GUI)) {
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
  int   flt_wd = dog_specs.filter_width; // half-width
  int   flt_wdf = 2 * flt_wd + 1;        // full-width
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

      // x = on/off, y = color channel
      for(int chan = 0; chan < dog_feat_geom.y; chan++) {
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
        if(cnv_sum >= 0.0f) {
          cur_out->FastEl4d(0, chan, oc.x, oc.y) = cnv_sum; // feat x = 0 = on
          cur_out->FastEl4d(1, chan, oc.x, oc.y) = 0.0f;      // feat x = 1 = off
        }
        else {
          cur_out->FastEl4d(0, chan, oc.x, oc.y) = 0.0f;      // feat x = 0 = on
          cur_out->FastEl4d(1, chan, oc.x, oc.y) = -cnv_sum; // feat x = 1 = off
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
    if(dog_save & SEP_MATRIX) {
      for(int i=0;i<dog_feat_geom.n;i++) {
        String nm = name + "_" + GetDoGFiltName(i) + "_dog";
        data_table->FindMakeColName(nm+ "_r", idx, DataTable::VT_FLOAT, 2,
                                    dog_img_geom.x, dog_img_geom.y);
        if(region.ocularity == VisRegionParams::BINOCULAR) {
          data_table->FindMakeColName(nm+ "_l", idx, DataTable::VT_FLOAT, 2,
                                      dog_img_geom.x, dog_img_geom.y);
        }
      }
    }
    else {
      data_table->FindMakeColName(name + "_dog_r", idx, DataTable::VT_FLOAT, 4,
                                  dog_feat_geom.x, dog_feat_geom.y, dog_img_geom.x, dog_img_geom.y);
      if(region.ocularity == VisRegionParams::BINOCULAR) {
        data_table->FindMakeColName(name + "_dog_l", idx, DataTable::VT_FLOAT, 4,
                                    dog_feat_geom.x, dog_feat_geom.y, dog_img_geom.x, dog_img_geom.y);
      }
    }
  }
  return true;
}

bool DoGRegionSpec::DoGOutputToTable(DataTable* dtab) {
  DoGOutputToTable_impl(dtab, &dog_out_r, "_r");
  if(region.ocularity == VisRegionParams::BINOCULAR)
    DoGOutputToTable_impl(dtab, &dog_out_l, "_l");
  return true;
}

bool DoGRegionSpec::DoGOutputToTable_impl(DataTable* dtab, float_Matrix* out, const String& col_sufx) {
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
      float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
      for(dc.y = 0; dc.y < dog_img_geom.y; dc.y++) {
        for(dc.x = 0; dc.x < dog_img_geom.x; dc.x++) {
          float val = out->FastEl4d(fc.x, fc.y, dc.x, dc.y);
          dout->FastEl2d(dc.x, dc.y) = val;
        }
      }
    }
  }
  else {
    col = data_table->FindMakeColName(name + "_dog" + col_sufx, idx, DataTable::VT_FLOAT, 4,
                                      dog_feat_geom.x, dog_feat_geom.y, dog_img_geom.x, dog_img_geom.y);
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

void DoGRegionSpec::PlotSpacing(DataTable* graph_data, bool reset) {
  taProject* proj = GET_MY_OWNER(taProject);
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

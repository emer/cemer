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

#include "RetinaProc.h"
#include <taVector2f>
#include <taImage>


void RetinaProc::Initialize() {
  edge_mode = taImageProc::WRAP;
  fade_width = -1;
}

void RetinaProc::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
}

void RetinaProc::CheckChildConfig_impl(bool quiet, bool& rval) {
  inherited::CheckChildConfig_impl(quiet, rval);
  regions.CheckConfig(quiet, rval);
}

///////////////////////////////////////////////////////////////////////
//              Basic impl routines

bool RetinaProc::Init() {
  if(regions.size == 0) return false;
  for(int ri=0; ri < regions.size; ri++) {
    VisRegionSpecBase* reg = regions[ri];
    reg->Init();
  }
  return true;
}

bool RetinaProc::TransformImageData_impl(float_Matrix& eye_image,
                                         float_Matrix& xform_image,
                                         float move_x, float move_y,
                                         float scale, float rotate)
{
  if(regions.size == 0) return false;
  if(TestError(scale == 0.0f, "TransformImageData_impl",
               "scale is 0 -- indicates bad parameters probably"))
    return false;

  VisRegionSpecBase* reg = regions[0]; // take params from first

  float ctr_x = .5f + .5 * move_x;
  float ctr_y = .5f + .5 * move_y;

  if(TestError(eye_image.dims() < 2, "TransformImageData_impl",
               "eye input image input must be at least 2 dimensional"))
    return false;

  // NOTE: the edge_mode of WRAP causes tiling effects for downscaling images -- switch to BORDER otherwise
  taImageProc::EdgeMode eff_em = edge_mode;
  if(edge_mode == taImageProc::WRAP) {
    taVector2i img_size(eye_image.dim(0), eye_image.dim(1));
    taVector2i scaled_size;
    scaled_size.x = 2 + (int)(scale * (img_size.x-2)); // keep border in there
    scaled_size.y = 2 + (int)(scale * (img_size.y-2));
    if(scaled_size.x < reg->input_size.retina_size.x || scaled_size.y < reg->input_size.retina_size.y) {
      eff_em = taImageProc::BORDER;
    }
  }

  taImageProc::SampleImageWindow_float(xform_image, eye_image, reg->input_size.retina_size.x,
				       reg->input_size.retina_size.y,
				       ctr_x, ctr_y, rotate, scale, eff_em);

  if(edge_mode == taImageProc::BORDER) taImageProc::RenderBorder_float(xform_image);
  if(edge_mode == taImageProc::BORDER && fade_width > 0) {
    taImageProc::FadeEdgesToBorder_float(xform_image, fade_width);
  }
  return true;
}

bool RetinaProc::LookAtImageData_impl(float_Matrix& eye_image,
                                      float_Matrix& xform_image,
                                      VisRegionParams::Region region,
                                      float box_ll_x, float box_ll_y,
                                      float box_ur_x, float box_ur_y,
                                      float move_x, float move_y,
                                      float scale, float rotate)
{
  // todo: add error messages on all these..
  if(regions.size == 0) return false;
  VisRegionSpecBase* trg_reg = regions.FindRetinalRegion(region);
  if(!trg_reg) return false;

  if(TestError(scale == 0.0f, "LookAtImageData_impl",
               "scale is 0 -- indicates bad parameters probably"))
    return false;

  // translation: find the middle of the box
  taVector2f obj_ctr((float) (0.5 * (float) (box_ll_x + box_ur_x)),
                       (float) (0.5 * (float) (box_ll_y + box_ur_y)));
  // convert into center-relative coords:
  taVector2f obj_ctr_off = 2.0f * (obj_ctr - 0.5f);

  move_x -= obj_ctr_off.x;
  move_y -= obj_ctr_off.y;

  // now, scale the thing to fit in trg_reg->input_size
  taVector2i img_size(eye_image.dim(0), eye_image.dim(1));

  // height and width in pixels of box:
  float pix_x = (box_ur_x - box_ll_x) * img_size.x;
  float pix_y = (box_ur_y - box_ll_y) * img_size.y;

  // scale to fit within input size of filter or retina
  float sc_x = (float)trg_reg->input_size.input_size.x / pix_x;
  float sc_y = (float)trg_reg->input_size.input_size.y / pix_y;

  float fov_sc = MIN(sc_x, sc_y);
  scale *= fov_sc;
  if(scale > 100.0f)
    scale = 100.0f;
  if(scale < .01f)
    scale = .01f;

  bool rval = TransformImageData_impl(eye_image, xform_image, move_x, move_y, scale, rotate);
  return rval;
}

bool RetinaProc::FilterImageData(bool motion_only) {
  if(regions.size == 0) return false;
  for(int ri=0; ri < regions.size; ri++) {
    VisRegionSpecBase* reg = regions[ri];
    reg->FilterImage(&xform_image_r, &xform_image_l, motion_only);
  }
  return true;
}

//////////////////////////////////////////////////////
//              Various front-ends

bool RetinaProc::TransformImageData(float_Matrix* right_eye_image,
                                    float_Matrix* left_eye_image,
                                    float move_x, float move_y,
                                    float scale, float rotate)
{
  if(regions.size == 0) return false;
  if(right_eye_image) {
    raw_image_r.CopyFrom(right_eye_image);
    TransformImageData_impl(raw_image_r, xform_image_r, move_x, move_y, scale, rotate);
  }
  if(left_eye_image) {
    raw_image_l.CopyFrom(left_eye_image);
    TransformImageData_impl(raw_image_l, xform_image_l, move_x, move_y, scale, rotate);
  }
  return true;
}

bool RetinaProc::LookAtImageData(float_Matrix* right_eye_image,
                                 float_Matrix* left_eye_image,
                                 VisRegionParams::Region region,
                                 float box_ll_x, float box_ll_y,
                                 float box_ur_x, float box_ur_y,
                                 float move_x, float move_y,
                                 float scale, float rotate)
{
  if(regions.size == 0) return false;
  if(right_eye_image) {
    raw_image_r.CopyFrom(right_eye_image);
    LookAtImageData_impl(raw_image_r, xform_image_r, region, box_ll_x, box_ll_y,
                         box_ur_x, box_ur_y, move_x, move_y, scale, rotate);
  }
  if(left_eye_image) {
    raw_image_l.CopyFrom(left_eye_image);
    LookAtImageData_impl(raw_image_l, xform_image_l, region, box_ll_x, box_ll_y,
                         box_ur_x, box_ur_y, move_x, move_y, scale, rotate);
  }
  return true;
}

bool RetinaProc::ConvertImageToMatrix(float_Matrix& img_data, taImage* img,
                                      VisRegionParams::Color color) {
  if(color == VisRegionParams::COLOR) {
    img->ImageToMatrix_rgb(img_data);
  }
  else {
    img->ImageToMatrix_grey(img_data);
  }
  return true;
}

bool RetinaProc::TransformImage(taImage* right_eye_image, taImage* left_eye_image,
                                float move_x, float move_y, float scale, float rotate)
{
  if(regions.size == 0) return false;
  VisRegionSpecBase* reg = regions[0]; // take params from first
  if(right_eye_image) {
    ConvertImageToMatrix(raw_image_r, right_eye_image, reg->region.color);
    TransformImageData_impl(raw_image_r, xform_image_r, move_x, move_y, scale, rotate);
  }
  if(left_eye_image) {
    ConvertImageToMatrix(raw_image_l, left_eye_image, reg->region.color);
    TransformImageData_impl(raw_image_l, xform_image_l, move_x, move_y, scale, rotate);
  }
  return true;
}

bool RetinaProc::TransformImageName(const String& right_eye_img_fname,
                                    const String& left_eye_img_fname,
                                    float move_x, float move_y,
                                    float scale, float rotate)
{
  if(regions.size == 0) return false;
  taImage img_r;
  if(!img_r.LoadImage(right_eye_img_fname)) return false;
  img_r.name = right_eye_img_fname;             // explicitly name it
  if(left_eye_img_fname.nonempty()) {
    taImage img_l;
    if(!img_l.LoadImage(left_eye_img_fname)) return false;
    img_l.name = left_eye_img_fname;            // explicitly name it
    return TransformImage(&img_r, &img_l, move_x, move_y, scale, rotate);
  }
  else {
    return TransformImage(&img_r, NULL, move_x, move_y, scale, rotate);
  }
}

///////////// Look At

bool RetinaProc::LookAtImage(taImage* right_eye_image,
                             taImage* left_eye_image,
                             VisRegionParams::Region region,
                             float box_ll_x, float box_ll_y,
                             float box_ur_x, float box_ur_y,
                             float move_x, float move_y,
                             float scale, float rotate)
{
  if(regions.size == 0) return false;
  VisRegionSpecBase* reg = regions[0]; // take params from first
  if(right_eye_image) {
    ConvertImageToMatrix(raw_image_r, right_eye_image, reg->region.color);
    LookAtImageData_impl(raw_image_r, xform_image_r, region, box_ll_x, box_ll_y,
                         box_ur_x, box_ur_y, move_x, move_y, scale, rotate);
  }
  if(left_eye_image) {
    ConvertImageToMatrix(raw_image_l, left_eye_image, reg->region.color);
    LookAtImageData_impl(raw_image_l, xform_image_l, region, box_ll_x, box_ll_y,
                         box_ur_x, box_ur_y, move_x, move_y, scale, rotate);
  }
  return true;
}

bool RetinaProc::LookAtImageName(const String& right_eye_img_fname,
                                 const String& left_eye_img_fname,
                                 VisRegionParams::Region region,
                                 float box_ll_x, float box_ll_y,
                                 float box_ur_x, float box_ur_y,
                                 float move_x, float move_y,
                                 float scale, float rotate)
{
  if(regions.size == 0) return false;
  taImage img_r;
  if(!img_r.LoadImage(right_eye_img_fname)) return false;
  img_r.name = right_eye_img_fname;             // explicitly name it
  if(left_eye_img_fname.nonempty()) {
    taImage img_l;
    if(!img_l.LoadImage(left_eye_img_fname)) return false;
    img_l.name = left_eye_img_fname;            // explicitly name it
    return LookAtImage(&img_r, &img_l, region, box_ll_x, box_ll_y,
                         box_ur_x, box_ur_y, move_x, move_y, scale, rotate);
  }
  else {
    return LookAtImage(&img_r, NULL, region, box_ll_x, box_ll_y,
                         box_ur_x, box_ur_y, move_x, move_y, scale, rotate);
  }
}

///////////////////////////////////////////////////////////////////////
// Misc other processing operations

bool RetinaProc::AttendRegion(DataTable* dt, VisRegionParams::Region region) {
//   VisRegionSpecBase* fov_spec = regions.FindRetinalRegion(region);
//   if(!fov_spec) return false;

//   float fov_x_pct = (float)fov_spec->spacing.input_size.x / (float)input_size.retina_size.x;
//   float fov_y_pct = (float)fov_spec->spacing.input_size.y / (float)input_size.retina_size.y;
//   float fov_pct = taMath_float::max(fov_x_pct, fov_y_pct);

//   int idx;
//   for(int i=0;i<regions.size;i++) {
//     VisRegionSpecBase* sp = regions[i];
//     if(sp->spacing.region <= region) continue; // don't filter this region -- only ones above it!
//     DataCol* da_on = dt->FindMakeColName(sp->name + "_on", idx, DataTable::VT_FLOAT, 2,
//                                              sp->spacing.output_size.x, sp->spacing.output_size.y);
//     DataCol* da_off = dt->FindMakeColName(sp->name + "_off", idx, DataTable::VT_FLOAT,
//                                               2, sp->spacing.output_size.x, sp->spacing.output_size.y);

//     float_MatrixPtr on_mat; on_mat = (float_Matrix*)da_on->GetValAsMatrix(-1);
//     float_MatrixPtr off_mat; off_mat = (float_Matrix*)da_off->GetValAsMatrix(-1);
//     taImageProc::AttentionFilter(*on_mat, fov_pct);
//     taImageProc::AttentionFilter(*off_mat, fov_pct);
//   }
  return true;
}

void DoGRetinaProc::Initialize() {
  regions.SetDefaultElType(&TA_DoGRegionSpec);
}

void V1RetinaProc::Initialize() {
  regions.SetDefaultElType(&TA_V1RegionSpec);
}



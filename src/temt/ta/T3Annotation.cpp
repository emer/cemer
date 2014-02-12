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

#include "T3Annotation.h"
#include <T3DataViewMain>
#include <taMath_float>

#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(T3Annotation);

void T3Annotation::Initialize() {
  type = LINE;
  arrow = NO_ARROW;
  rend_type = (AnnoteType)-1;
  size = 1.0f;
  line_width = 1.0f;
  arrow_size = .02f;
  text = "Select, Context Menu to Edit";
  justification = LEFT;
  font_size = .05f;
  fill = false;
  color.Set(0.0f, 0.0f, 0.0f);
  fill_color.Set(1.0f, 1.0f, 1.0f);
  xform_size = true;
  scale = 1.0f;
}

void T3Annotation::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(type == TEXT || type == OBJECT) {
    xform_size = false;
  }
  UpdateGeom();
  if(taMisc::gui_active && rend_type >= 0 && type != rend_type) {
    UpdateDisplay();
  }
}

void T3Annotation::UpdateDisplay() {
  T3DataViewMain* t3m = GET_MY_OWNER(T3DataViewMain);
  if(t3m) {
    t3m->ReBuildAll();
  }
}

void T3Annotation::UpdateGeom() {
  if(!xform_size || type == TEXT || type == OBJECT) return;

  if(rot.x != 0.0f || rot.y != 0.0f || rot.z != 1.0f || rot.rot != 0.0f) {
    rot.RotateVec(size);
    rot.SetXYZR(0.0f, 0.0f, 1.0f, 0.0f);
  }

  size *= scale;
  scale = 1.0f;
}

void T3Annotation::SetColor(const String& clr) {
  color.setColorName(clr);
  UpdateAfterEdit();
}

void T3Annotation::SetFillColor(bool fill, const String& clr) {
  fill = fill;
  fill_color.setColorName(clr);
  UpdateAfterEdit();
}

void T3Annotation::SetLineWidth(float ln_width) {
  line_width = ln_width;
  UpdateAfterEdit();
}

void T3Annotation::SetArrowSize(float arrow_sz) {
  arrow_size = arrow_sz;
  UpdateAfterEdit();
}

void T3Annotation::SetFontSize(float font_sz) {
  font_size = font_sz;
  UpdateAfterEdit();
}

void T3Annotation::SetJustification(TextJust just) {
  justification = just;
  UpdateAfterEdit();
}

void T3Annotation::Translate(float mv_x, float mv_y, float mv_z) {
  pos.x += mv_x;
  pos.y += mv_y;
  pos.z += mv_z;
  UpdateAfterEdit();
}

void T3Annotation::Scale(float sc) {
  scale *= sc;
  UpdateAfterEdit();
}

void T3Annotation::SetSize(float sz_x, float sz_y, float sz_z) {
  size.x = sz_x;
  size.y = sz_y;
  size.z = sz_z;
  UpdateAfterEdit();
}

void T3Annotation::AddSize(float sz_x, float sz_y, float sz_z) {
  size.x += sz_x;
  size.y += sz_y;
  size.z += sz_z;
  UpdateAfterEdit();
}

void T3Annotation::RotateAroundZ(float rotate_deg) {
  rot.SetXYZ(0.0f, 0.0f, 1.0f);
  rot.rot += taMath_float::deg_to_rad(rotate_deg);
  UpdateAfterEdit();
}

void T3Annotation::SetLine(float pos_x, float pos_y, float pos_z,
                           float size_x, float size_y, float size_z,
                           float ln_width , const String& clr) {
  type = LINE;
  arrow = NO_ARROW;
  pos.SetXYZ(pos_x, pos_y, pos_z);
  size.SetXYZ(size_x, size_y, size_z);
  line_width = ln_width;
  xform_size = true;
  SetColor(clr);
}

void T3Annotation::SetStartArrow(float pos_x, float pos_y, float pos_z,
                            float size_x, float size_y, float size_z,
                            float ln_width , const String& clr,
                            float arrow_sz) {
  type = LINE;
  arrow = START_ARROW;
  pos.SetXYZ(pos_x, pos_y, pos_z);
  size.SetXYZ(size_x, size_y, size_z);
  line_width = ln_width;
  xform_size = true;
  arrow_size = arrow_sz;
  SetColor(clr);
}

void T3Annotation::SetEndArrow(float pos_x, float pos_y, float pos_z,
                               float size_x, float size_y, float size_z,
                               float ln_width , const String& clr,
                               float arrow_sz) {
  type = LINE;
  arrow = END_ARROW;
  pos.SetXYZ(pos_x, pos_y, pos_z);
  size.SetXYZ(size_x, size_y, size_z);
  line_width = ln_width;
  xform_size = true;
  arrow_size = arrow_sz;
  SetColor(clr);
}

void T3Annotation::SetDoubleArrow(float pos_x, float pos_y, float pos_z,
                                  float size_x, float size_y, float size_z,
                                  float ln_width , const String& clr,
                                  float arrow_sz) {
  type = LINE;
  arrow = BOTH_ARROWS;
  pos.SetXYZ(pos_x, pos_y, pos_z);
  size.SetXYZ(size_x, size_y, size_z);
  line_width = ln_width;
  xform_size = true;
  arrow_size = arrow_sz;
  SetColor(clr);
}

void T3Annotation::SetRectangle(float pos_x, float pos_y, float pos_z,
                                float size_x, float size_y, float size_z,
                                float ln_width, const String& clr,
                                bool fil, const String& fill_clr) {
  type = RECTANGLE;
  pos.SetXYZ(pos_x, pos_y, pos_z);
  size.SetXYZ(size_x, size_y, size_z);
  line_width = ln_width;
  xform_size = false;
  fill = fil;
  fill_color.setColorName(fill_clr);
  SetColor(clr);
}

void T3Annotation::SetEllipse(float pos_x, float pos_y, float pos_z,
                              float size_x, float size_y, float size_z,
                              float ln_width , const String& clr,
                              bool fil, const String& fill_clr) {
  type = ELLIPSE;
  pos.SetXYZ(pos_x, pos_y, pos_z);
  size.SetXYZ(size_x, size_y, size_z);
  xform_size = false;
  line_width = ln_width;
  fill = fil;
  fill_color.setColorName(fill_clr);
  SetColor(clr);
}

void T3Annotation::SetCircle(float ctr_x, float ctr_y, float ctr_z,
                             float radius,
                             float ln_width , const String& clr,
                             bool fil, const String& fill_clr) {
  type = ELLIPSE;
  pos.SetXYZ(ctr_x - radius, ctr_y - radius, ctr_z);
  size.SetXYZ(2.0f * radius, 2.0f * radius, 0.0f);
  xform_size = false;
  line_width = ln_width;
  fill = fil;
  fill_color.setColorName(fill_clr);
  SetColor(clr);
}

void T3Annotation::SetText(const String& txt,
                           float pos_x, float pos_y, float pos_z,
                           float font_sz, TextJust just, const String& clr) {
  type = TEXT;
  text = txt;
  font_size = font_sz;
  justification = just;
  pos.SetXYZ(pos_x, pos_y, pos_z);
  xform_size = false;
  SetColor(clr);
}

void T3Annotation::SetObject(const String& obj_file_path,
                             float pos_x, float pos_y, float pos_z,
                             const String& clr) {
  type = OBJECT;
  obj_fname = obj_file_path;
  pos.SetXYZ(pos_x, pos_y, pos_z);
  xform_size = false;
  SetColor(clr);
}

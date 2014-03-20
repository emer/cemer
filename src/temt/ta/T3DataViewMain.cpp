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

#include "T3DataViewMain.h"
#include <T3Panel>
#include <T3ExaminerViewer>
#include <T3AnnotationView>
#include <T3Annotation>
#include <RGBA>

#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(T3DataViewMain);

void T3DataViewMain::Initialize() {
}

void T3DataViewMain::InitLinks() {
  inherited::InitLinks();
  taBase::Own(main_xform, this);
  taBase::Own(annotations, this);
  taBase::Own(annote_children, this);
}

void T3DataViewMain::CutLinks() {
  annote_children.CutLinks();
  inherited::CutLinks();
}

void T3DataViewMain::Copy_(const T3DataViewMain& cp) {
  main_xform = cp.main_xform;
  annotations = cp.annotations;
  annote_children = cp.annote_children;
}


void T3DataViewMain::CopyFromViewFrame(T3DataViewPar* cp) {
  T3Panel* vwr = GetFrame();
  if(!vwr) return;
  T3Panel* cp_vwr = cp->GetFrame();
  if(!cp_vwr || vwr == cp_vwr) return;

  vwr->CopyFromViewPanel(cp_vwr);
}

void T3DataViewMain::setInteractionModeOn(bool on_off, bool re_render) {
  T3ExaminerViewer* vw = GetViewer();
  if(vw) {
    vw->setInteractionModeOn(on_off, re_render);
  }
}

void T3DataViewMain::CloseChild(taDataView* child) {
  child->Reset();
  children.RemoveEl(child);
  annote_children.RemoveEl(child);
}

void T3DataViewMain::DoActionChildren_impl(DataViewAction acts) {
  if (acts & CONSTR_MASK) {
    inherited::DoActionChildren_impl(acts);
    annote_children.DoAction(acts);
  }
  else {
    annote_children.DoAction(acts);
    inherited::DoActionChildren_impl(acts);
  }
}

void T3DataViewMain::OnWindowBind(iT3Panel* vw) {
  inherited::OnWindowBind(vw);
  for (int i = 0; i < annote_children.size; ++i) {
    T3DataView* item = annote_children.FastEl(i);
    item->OnWindowBind(vw);
  }
}

void T3DataViewMain::ReInit_impl() {
  for (int i = annote_children.size - 1; i >= 0; --i) {
    T3DataView* item = annote_children.FastEl(i);
    item->ReInit();
  }
  inherited::ReInit_impl();
}

void T3DataViewMain::BuildAnnotations() {
  annote_children.Reset();
  for(int i=0; i<annotations.size; i++) {
    T3Annotation* obj = annotations.FastEl(i);
    T3AnnotationView* ov = new T3AnnotationView();
    ov->SetAnno(obj);
    annote_children.Add(ov);
  }
}

void T3DataViewMain::ReBuildAll() {
  Reset();
  BuildAll();
  Render();
}

void T3DataViewMain::SaveImageSVG(const String& svg_fnm) {
  taMisc::Error("Sorry, SVG output not (yet) supported for this type of object");
}

T3Annotation* T3DataViewMain::AnnoteLine(bool data_units, float pos_x, float pos_y, float pos_z,
                                         float size_x, float size_y, float size_z,
                                         float line_width, const String& color) {
  T3Annotation* obj = (T3Annotation*)annotations.New(1);
  obj->name = String("line_") + String(annotations.size-1);
  taVector3f pos(pos_x, pos_y, pos_z);
  taVector3f size(size_x, size_y, size_z);
  if(data_units)
    DataUnitsXForm(pos, size);
  obj->SetLine(pos.x, pos.y, pos.z, size.x, size.y, size.z, line_width, color);
  ReBuildAll();
  return obj;
}

T3Annotation* T3DataViewMain::AnnoteStartArrow(bool data_units, float pos_x, float pos_y, float pos_z,
                                          float size_x, float size_y, float size_z,
                                          float line_width, const String& color,
                                          float arrow_size) {
  T3Annotation* obj = (T3Annotation*)annotations.New(1);
  obj->name = String("arrow_") + String(annotations.size-1);
  taVector3f pos(pos_x, pos_y, pos_z);
  taVector3f size(size_x, size_y, size_z);
  if(data_units)
    DataUnitsXForm(pos, size);
  obj->SetStartArrow(pos.x, pos.y, pos.z, size.x, size.y, size.z, line_width, color, arrow_size);
  ReBuildAll();
  return obj;
}

T3Annotation* T3DataViewMain::AnnoteEndArrow(bool data_units, float pos_x, float pos_y, float pos_z,
                                          float size_x, float size_y, float size_z,
                                          float line_width, const String& color,
                                          float arrow_size) {
  T3Annotation* obj = (T3Annotation*)annotations.New(1);
  obj->name = String("arrow_") + String(annotations.size-1);
  taVector3f pos(pos_x, pos_y, pos_z);
  taVector3f size(size_x, size_y, size_z);
  if(data_units)
    DataUnitsXForm(pos, size);
  obj->SetEndArrow(pos.x, pos.y, pos.z, size.x, size.y, size.z, line_width, color, arrow_size);
  ReBuildAll();
  return obj;
}

T3Annotation* T3DataViewMain::AnnoteDoubleArrow(bool data_units, float pos_x, float pos_y, float pos_z,
                                                float size_x, float size_y, float size_z,
                                                float line_width, const String& color,
                                                float arrow_size) {
  T3Annotation* obj = (T3Annotation*)annotations.New(1);
  obj->name = String("double_arrow_") + String(annotations.size-1);
  taVector3f pos(pos_x, pos_y, pos_z);
  taVector3f size(size_x, size_y, size_z);
  if(data_units)
    DataUnitsXForm(pos, size);
  obj->SetDoubleArrow(pos.x, pos.y, pos.z, size.x, size.y, size.z, line_width, color, arrow_size);
  ReBuildAll();
  return obj;
}

T3Annotation* T3DataViewMain::AnnoteRectangle(bool data_units, float pos_x, float pos_y, float pos_z,
                                              float size_x, float size_y, float size_z,
                                              float line_width, const String& color,
                                              bool fil, const String& fill_clr) {
  T3Annotation* obj = (T3Annotation*)annotations.New(1);
  obj->name = String("rectangle_") + String(annotations.size-1);
  taVector3f pos(pos_x, pos_y, pos_z);
  taVector3f size(size_x, size_y, size_z);
  if(data_units)
    DataUnitsXForm(pos, size);
  obj->SetRectangle(pos.x, pos.y, pos.z, size.x, size.y, size.z, line_width, color, fil, fill_clr);
  ReBuildAll();
  return obj;
}

T3Annotation* T3DataViewMain::AnnoteEllipse(bool data_units, float pos_x, float pos_y, float pos_z,
                                            float size_x, float size_y, float size_z,
                                            float line_width, const String& color,
                                            bool fil, const String& fill_clr) {
  T3Annotation* obj = (T3Annotation*)annotations.New(1);
  obj->name = String("ellipse_") + String(annotations.size-1);
  taVector3f pos(pos_x, pos_y, pos_z);
  taVector3f size(size_x, size_y, size_z);
  if(data_units)
    DataUnitsXForm(pos, size);
  obj->SetEllipse(pos.x, pos.y, pos.z, size.x, size.y, size.z, line_width, color, fil, fill_clr);
  ReBuildAll();
  return obj;
}

T3Annotation* T3DataViewMain::AnnoteCircle(bool data_units, float ctr_x, float ctr_y, float ctr_z,
                                           float radius,
                                           float line_width, const String& color,
                                           bool fil, const String& fill_clr) {
  T3Annotation* obj = (T3Annotation*)annotations.New(1);
  obj->name = String("circle_") + String(annotations.size-1);
  taVector3f pos(ctr_x, ctr_y, ctr_z);
  taVector3f size(radius, 0.0f, 0.0f);
  if(data_units)
    DataUnitsXForm(pos, size);
  obj->SetCircle(pos.x, pos.y, pos.z, size.x, line_width, color, fil, fill_clr);
  ReBuildAll();
  return obj;
}

T3Annotation* T3DataViewMain::AnnoteText(bool data_units, const String& text,
                                         float pos_x, float pos_y, float pos_z,
                                         float font_size, TextJust just,
                                         const String& color) {
  T3Annotation* obj = (T3Annotation*)annotations.New(1);
  obj->name = String("text_") + String(annotations.size-1);
  taVector3f pos(pos_x, pos_y, pos_z);
  taVector3f size;
  if(data_units)
    DataUnitsXForm(pos, size);
  obj->SetText(text, pos.x, pos.y, pos.z, font_size, (T3Annotation::TextJust)just, color);
  ReBuildAll();
  return obj;
}

T3Annotation* T3DataViewMain::AnnoteObject(bool data_units, const String& obj_file_path,
                                           float pos_x, float pos_y, float pos_z,
                                           const String& color) {
  T3Annotation* obj = (T3Annotation*)annotations.New(1);
  obj->name = String("object_") + String(annotations.size-1);
  taVector3f pos(pos_x, pos_y, pos_z);
  taVector3f size;
  if(data_units)
    DataUnitsXForm(pos, size);
  obj->SetObject(obj_file_path, pos.x, pos.y, pos.z, color);
  ReBuildAll();
  return obj;
}

void T3DataViewMain::AnnoteClearAll() {
  annotations.Reset();
}

/////////////////////////////////
//      SVG Rendering help

String T3DataViewMain::SvgHeader(float width, float height,
                                 float pix_width, float pix_height) {
  String rval;
  rval << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
       << "<svg\n"
       << "  xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\"\n"
       << "  width=\"" << 400.0f * width << "px\"\n"
       << "  height=\"" << 400.0f * height << "px\"\n"
       << "  viewBox=\"0 0 " << 1000.0f * width << " " << 1000.0f * height << "\">\n";
  return rval;
}

String T3DataViewMain::SvgFooter() {
  return "</svg>\n";
}

String T3DataViewMain::SvgCoords(float x, float y) {
  return String(1000.0f * x) + "," + String(1000.0f - (1000.0f * y)) + " ";
}

String T3DataViewMain::SvgCoordsXY(float x, float y) {
  String rval;
  rval  << "x=\"" << 1000.0f * x << "\" y=\""
        << 1000.0f - 1000.0f * y << "\"";
  return rval;
}

String T3DataViewMain::SvgPath(const RGBA& color, float line_width) {
  String rval;
  rval << "<path fill=\"none\" stroke=\"#" << color.ToHexString()
       << "\" stroke-width=\"" << line_width << "\"\n"
       << "  d=\"";
  return rval;
}

String T3DataViewMain::SvgPathEnd() {
  return "\"\n />\n";
}

String T3DataViewMain::SvgGroup() {
  return "\n<g>\n";
}

String T3DataViewMain::SvgGroupEnd() {
  return "\n</g>\n";
}

String T3DataViewMain::SvgGroupTranslate(float x, float y) {
  String rval; 
  rval << "\n<g transform=\"translate(" << String(1000.0f * x) << ","
       << String(1000.0f * y) + ")\">\n";
  return rval;
}

String T3DataViewMain::SvgText(const String& str, float x, float y, const RGBA& color,
                               float font_size, TextJust just, 
                               bool vertical, const String& font) {
  String rval;
  String anch;
  switch (just) {
  case LEFT:
    anch = "start";
    break;
  case CENTER:
    anch = "middle";
    break;
  case RIGHT:
    anch = "end";
    break;
  }
  rval << "\n<text " << SvgCoordsXY(x,y)
       << " font-family=\"" << font
       << "\" font-size=\"" << 1000.0f * font_size
       << "\" fill=\"#" << color.ToHexString();
  if(vertical) {
    rval << "\" writing-mode=\"tb";
  }
  rval << "\" text-anchor=\"" << anch << "\">\n"
       << str << "\n</text>\n";
  return rval;
}

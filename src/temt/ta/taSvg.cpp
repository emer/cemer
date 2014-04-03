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

#include "taSvg.h"
#include <iColor>
#include <T3ExaminerViewer>

#include <taMisc>

#include <Inventor/nodes/SoCamera.h>
#include <Inventor/SbViewVolume.h>
#include <Inventor/SbViewportRegion.h>

TA_BASEFUNS_CTORS_DEFN(taSvg);

taSvgPtr taSvg::cur_inst;

void taSvg::Initialize() {
  view_vol = NULL;
}

void taSvg::Destroy() {
  if(view_vol) {
    delete view_vol;
    view_vol = NULL;
  }
}

String taSvg::Header(T3ExaminerViewer* vw,
                     float pix_width, float pix_height) {

  cur_inst = new taSvg;
  cur_inst->view_vol = new SbViewVolume();
  SbViewportRegion rvp;
  *(cur_inst->view_vol) = vw->getViewerCamera()->getViewVolume(vw->getViewportRegion(),
                                                               rvp);

  float width = cur_inst->view_vol->getWidth();
  float height = cur_inst->view_vol->getHeight();

  taMisc::Info("width: ", String(width), "height: ", String(height));

  width = height = 1.0f;

  String rval;
  rval << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
       << "<svg\n"
       << "  xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\"\n"
       << "  width=\"" << pix_width * width << "px\"\n"
       << "  height=\"" << pix_height * height << "px\"\n"
       << "  viewBox=\"0 0 " << 1000.0f * width << " " << 1000.0f * height << "\">\n";
  return rval;
}

bool taSvg::CheckInst() {
  if(!cur_inst) {
    taMisc::Error("taSvg does not have a current inst -- this is a programmer error in the svg rendering code -- please submit bug report");
    return false;
  }
  if(!cur_inst->view_vol) {
    taMisc::Error("taSvg does not have a current view volume -- this is a programmer error in the svg rendering code -- please submit bug report");
    return false;
  }
  return true;
}

String taSvg::Footer() {
  if(!CheckInst()) return _nilString;
  cur_inst = NULL;              // deletes it
  return "</svg>\n";
}

String taSvg::Coords(float x, float y, float z) {
  if(!CheckInst()) return _nilString;
  SbVec3f src(x+1.0f,y,-z); // needed to center properly for some reason..
  //  SbVec3f src(x,y,z);
  SbVec3f dst;
  cur_inst->view_vol->projectToScreen(src, dst);
  // note: z = depth plane here -- could use for sorting by depth..
  return String(1000.0f * dst[0]) + "," + String(1000.0f - (1000.0f * dst[1])) + " ";
}

String taSvg::CoordsXY(float x, float y, float z) {
  if(!CheckInst()) return _nilString;
  SbVec3f src(x+1.0f,y,-z); // needed to center properly for some reason..
  //  SbVec3f src(x,y,z);
  SbVec3f dst;
  cur_inst->view_vol->projectToScreen(src, dst);
  // note: z = depth plane here -- could use for sorting by depth..
  String rval;
  rval  << "x=\"" << 1000.0f * dst[0] << "\" y=\""
        << 1000.0f - 1000.0f * dst[1] << "\"";
  return rval;
}

String taSvg::Path(const iColor& color, float line_width) {
  if(!CheckInst()) return _nilString;
  String rval;
  rval << "<path fill=\"none\" stroke=\"#" << color.toString()
       << "\" stroke-width=\"" << line_width << "\"\n"
       << "  d=\"";
  return rval;
}

String taSvg::PathEnd() {
  if(!CheckInst()) return _nilString;
  return "\"\n />\n";
}

String taSvg::Group() {
  if(!CheckInst()) return _nilString;
  return "\n<g>\n";
}

String taSvg::GroupEnd() {
  if(!CheckInst()) return _nilString;
  return "\n</g>\n";
}

String taSvg::GroupTranslate(float x, float y) {
  if(!CheckInst()) return _nilString;
  String rval; 
  rval << "\n<g transform=\"translate(" << String(1000.0f * x) << ","
       << String(1000.0f * y) + ")\">\n";
  return rval;
}

String taSvg::Text(const String& str, float x, float y, float z, const iColor& color,
                   float font_size, TextJust just, 
                   bool vertical, const String& font) {
  if(!CheckInst()) return _nilString;
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
  rval << "\n<text " << CoordsXY(x,y,z)
       << " font-family=\"" << font
       << "\" font-size=\"" << 1000.0f * font_size
       << "\" fill=\"#" << color.toString();
  if(vertical) {
    rval << "\" writing-mode=\"tb";
  }
  rval << "\" text-anchor=\"" << anch << "\">\n"
       << str << "\n</text>\n";
  return rval;
}

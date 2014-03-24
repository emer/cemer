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

TA_BASEFUNS_CTORS_DEFN(taSvg);


String taSvg::Header(float width, float height,
                                 float pix_width, float pix_height) {
  String rval;
  rval << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
       << "<svg\n"
       << "  xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\"\n"
       << "  width=\"" << pix_width * width << "px\"\n"
       << "  height=\"" << pix_height * height << "px\"\n"
       << "  viewBox=\"0 0 " << 1000.0f * width << " " << 1000.0f * height << "\">\n";
  return rval;
}

String taSvg::Footer() {
  return "</svg>\n";
}

String taSvg::Coords(float x, float y) {
  return String(1000.0f * x) + "," + String(1000.0f - (1000.0f * y)) + " ";
}

String taSvg::CoordsXY(float x, float y) {
  String rval;
  rval  << "x=\"" << 1000.0f * x << "\" y=\""
        << 1000.0f - 1000.0f * y << "\"";
  return rval;
}

String taSvg::Path(const iColor& color, float line_width) {
  String rval;
  rval << "<path fill=\"none\" stroke=\"#" << color.toString()
       << "\" stroke-width=\"" << line_width << "\"\n"
       << "  d=\"";
  return rval;
}

String taSvg::PathEnd() {
  return "\"\n />\n";
}

String taSvg::Group() {
  return "\n<g>\n";
}

String taSvg::GroupEnd() {
  return "\n</g>\n";
}

String taSvg::GroupTranslate(float x, float y) {
  String rval; 
  rval << "\n<g transform=\"translate(" << String(1000.0f * x) << ","
       << String(1000.0f * y) + ")\">\n";
  return rval;
}

String taSvg::Text(const String& str, float x, float y, const iColor& color,
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
  rval << "\n<text " << CoordsXY(x,y)
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

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

#ifndef taSvg_h
#define taSvg_h 1

// parent includes:
#include <taNBase>

// member includes:

// declare all other types mentioned but not required to include:
class iColor; // 

taTypeDef_Of(taSvg);

class TA_API taSvg : public taNBase {
  // support for SVG -- scalable vector graphics format (rendering, etc)
INHERITED(taNBase)
public:
  enum TextJust {               // justification for text
    LEFT,                       // left justified text
    CENTER,                     // center justified text
    RIGHT,                      // right justified text
  };

  static String  Header(float width = 1.0, float height = 1.0,
                        float pix_width = 400.0, float pix_height = 400.0);
  // #IGNORE the full svg header to start an svg image -- pixel width and height are then multiplied by the normalized width, height so you don't need to do that manually
  static String  Footer();
  // #IGNORE svg footer
  static String  Coords(float x, float y);
  // #IGNORE x,y assuming normalized x,y values multiplied by 1000
  static String  CoordsXY(float x, float y);
  // #IGNORE x="x" y="y" assuming normalized x,y values multiplied by 1000
  static String  Path(const iColor& color, float line_width = 2.0);
  // #IGNORE <path> start, leaving d=" hanging..
  static String  PathEnd();
  // #IGNORE end the path
  static String  Group();
  // #IGNORE just a basic group <g>
  static String  GroupEnd();
  // #IGNORE end a group
  static String  GroupTranslate(float tr_x, float tr_y);
  // #IGNORE a group with translation transform 
  static String  Text(const String& str, float x, float y, const iColor& color,
                      float font_size = 0.05, TextJust just = LEFT, 
                      bool vertical = false, const String& font = "Arial");
  // #IGNORE <text> with given properties -- font size is normalized -- multiplied x1000

  TA_SIMPLE_BASEFUNS(taSvg);
private:
  void Initialize()  { };
  void Destroy()     { };
};

#endif // taSvg_h

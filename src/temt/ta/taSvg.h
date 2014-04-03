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
#include <taSmartRefT>
#include <taSmartPtrT>

// member includes:
#include <taVector3f>
#include <iVec3f>

// declare all other types mentioned but not required to include:
class iColor; // 
class SbViewVolume; //
class T3ExaminerViewer; //

taTypeDef_Of(taSvg);

class taSvg; //
TA_SMART_PTRS(TA_API, taSvg); // taSvgRef


class TA_API taSvg : public taNBase {
  // support for SVG -- scalable vector graphics format (rendering, etc)
INHERITED(taNBase)
public:
  enum TextJust {               // justification for text
    LEFT,                       // left justified text
    CENTER,                     // center justified text
    RIGHT,                      // right justified text
  };

  static taSvgPtr cur_inst;       // current instance of svg for rendering
  SbViewVolume*   view_vol;       // #IGNORE captures camera view projection data

  static String  Header(T3ExaminerViewer* viewer, float pix_width = 400.0,
                        float pix_height = 400.0);
  // #IGNORE initializes the svg rendering information based on the given camera, along with the full svg header to start an svg image -- pixel width and height are then multiplied by the normalized width, height so you don't need to do that manually
  static bool    CheckInst();
  // #CAT_Svg check if the instance is present and properly initialized
  static String  Footer();
  // #CAT_Svg svg footer
  static String  Coords(const taVector3f& vec)
  { return Coords(vec.x, vec.y, vec.z); }
  // #IGNORE output screen x,y coords from given 3D point
  static String  Coords(const iVec3f& vec)
  { return Coords(vec.x, vec.y, vec.z); }
  // #IGNORE output screen x,y coords from given 3D point
  static String  Coords(float x, float y, float z);
  // #CAT_Svg output screen x,y coords from given 3D point
  static String  CoordsXY(const taVector3f& vec)
  { return CoordsXY(vec.x, vec.y, vec.z); }
  // #IGNORE output screen x="x" y="y" from given 3D point
  static String  CoordsXY(const iVec3f& vec)
  { return CoordsXY(vec.x, vec.y, vec.z); }
  // #IGNORE output screen x="x" y="y" from given 3D point
  static String  CoordsXY(float x, float y, float z);
  // #CAT_Svg output screen x="x" y="y" from given 3D point
  static String  Path(const iColor& color, float line_width = 2.0);
  // #CAT_Svg <path> start, leaving d=" hanging -- to be filled in with Coords calls
  static String  PathEnd();
  // #CAT_Svg end the path
  static String  Group();
  // #CAT_Svg just a basic group <g>
  static String  GroupEnd();
  // #CAT_Svg end a group
  static String  GroupTranslate(float tr_x, float tr_y);
  // #CAT_Svg a group with translation transform 
  static String  Text(const String& str, const taVector3f& vec, const iColor& color,
                      float font_size = 0.05, TextJust just = LEFT, 
                      bool vertical = false, const String& font = "Arial")
  { return Text(str, vec.x, vec.y, vec.z, color, font_size, just, vertical, font); }
  // #IGNORE <text> with given properties -- font size is in normalized units
  static String  Text(const String& str, const iVec3f& vec, const iColor& color,
                      float font_size = 0.05, TextJust just = LEFT, 
                      bool vertical = false, const String& font = "Arial")
  { return Text(str, vec.x, vec.y, vec.z, color, font_size, just, vertical, font); }
  // #IGNORE <text> with given properties -- font size is in normalized units
  static String  Text(const String& str, float x, float y, float z, const iColor& color,
                      float font_size = 0.05, TextJust just = LEFT, 
                      bool vertical = false, const String& font = "Arial");
  // #CAT_Svg <text> with given properties -- font size is in normalized units

  TA_SIMPLE_BASEFUNS(taSvg);
private:
  void Initialize();
  void Destroy();
};

#endif // taSvg_h

// Copyright 2013-2018, Regents of the University of Colorado,
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

#ifndef iFont_h
#define iFont_h 1

#include "ta_def.h"
#include <taString>

#ifdef TA_GUI
class QFont; // 
#endif

#ifdef TA_QT3D

#else // TA_QT3D
class SoFont; // #IGNORE
#endif // TA_QT3D

taTypeDef_Of(iFont);

class TA_API iFont {
  // #NO_TOKENS represents font information
public:
  enum Attributes { // #BITS
    Bold                = 0x01,
    Italic              = 0x02,
    Underline           = 0x04
  };

  String        fontName; // family name of the font, such as 'Helvetica' or 'Arial'
  Attributes    attribs; // #BITS font modifiers
  float         pointSize; // point size of the font -- will be aproximated in 3D
  int           stretch; // #EXPERT #DEF_100 the degree of stretch, less is narrower

  bool          bold() const {return  (attribs && Bold);}
  void          setBold(bool value);
  bool          italic() const {return  (attribs && Italic);}
  void          setItalic(bool value);
  void          setPointSize(float value);
  void          setPointSize(int value);
  void          setStretch(int value);
  bool          underline() const {return (attribs && Underline);}
  void          setUnderline(bool value);

  void          copy(const iFont& src);

  bool          equals(const iFont& b);
  
  iFont& operator=(const iFont& val); // must use value copy

  iFont();
  iFont(const iFont& val); // must use value copy
  ~iFont();

#ifdef TA_GUI
  void          copyFrom(const QFont& src); //#IGNORE
  void          copyTo(QFont& dst) const; //#IGNORE
  void          setRawName(const char* raw_name);
  iFont(const char* raw_name); //Iv-compatible constructor TODO: this s/b nuked!
  iFont(const QFont& val);
  iFont& operator=(const QFont& val);
  operator QFont() const;
#endif

#ifdef TA_QT3D

#else // TA_QT3D
  void          copyTo(SoFont* dst, float pts_per_unit = 36.0f) const; //#IGNORE
#endif // TA_QT3D
protected:
  static String defName; // #IGNORE
  void          init(); // #IGNORE
};

#endif // iFont_h

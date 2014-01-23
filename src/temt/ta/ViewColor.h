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

#ifndef ViewColor_h
#define ViewColor_h 1

// parent includes:
#include <taNBase>

// member includes:
#include <RGBA>

// declare all other types mentioned but not required to include:


taTypeDef_Of(ViewColor);

class TA_API ViewColor : public taNBase {
  // ##INLINE ##NO_TOKENS ##CAT_Display view color specification -- name lookup of color highlighting for view display
INHERITED(taNBase)
public:
  bool          use_fg;                 // use a special foreground color
  RGBA          fg_color;               // #CONDEDIT_ON_use_fg:true foreground color
  bool          use_bg;                 // use a special background color
  RGBA          bg_color;               // #CONDEDIT_ON_use_fg:true background color
  String        desc;                   // description of this view color item

  String       GetDesc() const CPP11_OVERRIDE { return desc; }

  void  InitLinks();
  TA_BASEFUNS(ViewColor);
private:
  SIMPLE_COPY(ViewColor);
  void  Initialize();
  void  Destroy();
};


#endif // ViewColor_h

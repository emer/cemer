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

#ifndef ViewBackground_h
#define ViewBackground_h 1

// parent includes:
#include <taNBase>

// member includes:
#ifndef __MAKETA__
  #include <QBrush>
#else
  class QBrush; // #IGNORE
#endif

// declare all other types mentioned but not required to include:

taTypeDef_Of(ViewBackground);

class TA_API ViewBackground : public taNBase {
  // ##INLINE ##NO_TOKENS ##CAT_Display view background pattern specification -- name lookup of background pattern for view display
INHERITED(taNBase)
public:
  Qt::BrushStyle         style;  // the pattern for the background
  String                 desc;   // description of this view color item

  String       GetDesc() const override { return desc; }

  void  InitLinks();
  TA_BASEFUNS(ViewBackground);
private:
  SIMPLE_COPY(ViewBackground);
  void Initialize();
  void Destroy();
};

#endif // ViewBackground_h

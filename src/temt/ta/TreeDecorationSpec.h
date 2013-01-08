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

#ifndef TreeDecorationSpec_h
#define TreeDecorationSpec_h 1

// parent includes:
#include <taNBase>

// member includes:
#include <RGBA>

// declare all other types mentioned but not required to include:


class TA_API TreeDecorationSpec: public taNBase {
  // provides the color and other details for the named type of entry, ex "comment"
INHERITED(taNBase)
public:

  RGBA                  text_color;

//  void        UpdateAfterEdit();
  void  InitLinks();
  void  CutLinks();
  TA_BASEFUNS(TreeDecorationSpec) //
private:
  void  Copy_(const TreeDecorationSpec& cp);
  void Initialize();
  void Destroy();
};

#endif // TreeDecorationSpec_h

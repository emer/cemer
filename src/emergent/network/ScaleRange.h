// Copyright, 1995-2013, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of Emergent
//
//   Emergent is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   Emergent is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.

#ifndef ScaleRange_h
#define ScaleRange_h 1

// parent includes:
#include "network_def.h"
#include <taNBase>

// member includes:

// declare all other types mentioned but not required to include:
class ColorScale; //


TypeDef_Of(ScaleRange);

class EMERGENT_API ScaleRange : public taOBase {
  // ##NO_TOKENS ##CAT_Display saves scale ranges for different variables viewed in netview
INHERITED(taOBase)
public:
  String        var_name;               // #AKA_name name of variable -- not name of object
  bool          auto_scale;
  float         min;
  float         max;

  override bool FindCheck(const String& nm) const
  { return (var_name == nm); }

  void          SetFromScale(ColorScale& cs);
  void          SetFromScaleRange(ColorScale& cs);

  TA_BASEFUNS(ScaleRange);
protected:
  override void         UpdateAfterEdit_impl();

private:
  void          Copy_(const ScaleRange &cp)
    {auto_scale = cp.auto_scale; min = cp.min; max = cp.max; }
  void          Initialize()    { auto_scale = false; min = 0.0f; max = 0.0f;}
  void          Destroy()       { }
};

#endif // ScaleRange_h

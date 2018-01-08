// Copyright 2017-2018, Regents of the University of Colorado,
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

#ifndef PatchRec_Group_h
#define PatchRec_Group_h 1

// parent includes:
#include <taGroup>

// member includes:

// declare all other types mentioned but not required to include:
class PatchRec; //

taTypeDef_Of(PatchRec);
taTypeDef_Of(PatchRec_Group);

class TA_API PatchRec_Group : public taGroup<PatchRec> {
  // ##CAT_Patch group of patch records -- allows patches to be applied based on differences
INHERITED(taGroup<PatchRec>)
public:
  String       path;            // our root path for all the patches that apply under us
  
  String       GetTypeDecoKey() const override { return "ControlPanel"; }

  TA_SIMPLE_BASEFUNS(PatchRec_Group);
private:
  void Initialize()  { SetBaseType(&TA_PatchRec); }
  void Destroy()     { };
};

#endif // PatchRec_Group_h

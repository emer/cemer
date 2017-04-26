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

#ifndef Patch_Group_h
#define Patch_Group_h 1

// parent includes:
#include <taGroup>

// member includes:

// declare all other types mentioned but not required to include:
class Patch; //

taTypeDef_Of(Patch);
taTypeDef_Of(Patch_Group);

class TA_API Patch_Group : public taGroup<Patch> {
  // ##CAT_Patch ##EXPAND_DEF_1 a group of patches, where each patch is a set of edits to change a project based on changes (diffs) generated from other projects
INHERITED(taGroup<Patch>)
public:

  virtual void  LoadPatch();
  // #BUTTON load a patch from local file system
  virtual void  FindPatches();
  // #BUTTON browse for patches on web  
  
  virtual Patch*  NewPatch();
  // get a new patch record

  String        GetTypeDecoKey() const override { return "ControlPanel"; }

  TA_BASEFUNS(Patch_Group);
private:
  NOCOPY(Patch_Group);
  void Initialize();
  void Destroy()     { };
};

#endif // Patch_Group_h

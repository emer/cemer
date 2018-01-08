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

#ifndef PatchLib_h
#define PatchLib_h 1

// parent includes:
#include <ObjLibrary>

// member includes:

// declare all other types mentioned but not required to include:
class Patch; //
class Patch_Group; //

taTypeDef_Of(PatchLib);

class TA_API PatchLib : public ObjLibrary {
  // A Library of Patch's available in various locations
INHERITED(ObjLibrary)
public:

  virtual Patch*  NewPatch(Patch_Group* new_owner, ObjLibEl* lib_el);
  // create a new patch in given group, based on lib element
  virtual Patch* NewPatchFmName(Patch_Group* new_owner, const String& patch_nm);
  // create a new patch in given group, (lookup by name) -- used by web interface to auto-load a patch
  virtual bool UpdatePatch(Patch* pat, ObjLibEl* lib_el);
  // load patch at lib element into patch

  void  SetWikiInfoToObj(taBase* obj, const String& wiki_name) override;
  void  GetWikiInfoFromObj
    (taBase* obj, String*& tags, String*& desc, taProjVersion*& version, String*& author,
     String*& email, String*& pub_cite) override;
  
  TA_BASEFUNS_NOCOPY(PatchLib);
private:
  void Initialize();
  void Destroy()     { };
};

#endif // PatchLib_h

// Co2018ght 2017-2017, Regents of the University of Colorado,
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
#include <PatchLib>

// declare all other types mentioned but not required to include:
class Patch; //

taTypeDef_Of(Patch);
taTypeDef_Of(Patch_Group);

class TA_API Patch_Group : public taGroup<Patch> {
  // ##CAT_Patch a group of patches, where each patch is a set of edits to change a project based on changes (diffs) generated from other projects
INHERITED(taGroup<Patch>)
public:
  static PatchLib patch_lib; // #TREE_HIDDEN #HIDDEN #NO_SAVE library of available patches

  virtual void  LoadPatch();
  // #BUTTON #CAT_Patch load a patch from local file system
  
  virtual void    BuildPatchLib();
  // #CAT_IGNORE build the patch library -- find all the patches -- called just-in-time when needed
  
  virtual Patch*  AddFromPatchLib(ObjLibEl* patch_lib_item);
  // #BUTTON #MENU_CONTEXT #FROM_LIST_patch_lib.library #NO_SAVE_ARG_VAL #PRE_CALL_BuildPatchLib #CAT_PatchLib create a new patch from a library of existing patches
  virtual Patch*  AddFromPatchLibByName(const String& patch_nm);
  // #PRE_CALL_BuildPatchLib #CAT_PatchLib create a new patch from a library of existing patches -- used by web interface to auto-load a patch
  virtual void    BrowsePatchLib(PatchLib::LibLocs location = PatchLib::WEB_APP_LIB);
  // #BUTTON #MENU_CONTEXT #CAT_PatchLib browse given patch library location using web browser (or file browser for local files -- less useful) -- USER_LIB: user's personal library -- located in app user dir (~/lib/emergent or ~/Library/Emergent patch_lib), SYSTEM_LIB: local system library, installed with software, in /usr/share/Emergent/patch_lib, WEB_APP_LIB: web-based application-specific library (e.g., emergent, WEB_SCI_LIB: web-based scientifically oriented library (e.g., CCN), WEB_USER_LIB: web-based user's library (e.g., from lab wiki)

  virtual Patch*  MergePatches(Patch* patch1, Patch* patch2);
  // #BUTTON #NO_SCOPE #CAT_Patch merge two patch records computed against the same source object -- this will create a new combined patch here with  any apparent conflicts affecting the same target object flagged, and duplicates removed -- once the conflicts are manually resolved (remove one or the other) then the resulting patch can be applied to the base project to combine two versions
  
  virtual Patch*  NewPatch();
  // #CAT_Patch get a new patch record

  String        GetTypeDecoKey() const override { return "ControlPanel"; }

  TA_BASEFUNS(Patch_Group);
private:
  NOCOPY(Patch_Group);
  void Initialize();
  void Destroy()     { };
};

#endif // Patch_Group_h

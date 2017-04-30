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

#ifndef Patch_h
#define Patch_h 1

// parent includes:
#include <taNBase>

// member includes:
#include <PatchRec_Group>

// declare all other types mentioned but not required to include:

taTypeDef_Of(Patch);

class TA_API Patch : public taNBase {
  // ##CAT_Patch ##EXPAND_DEF_1 #STEM_BASE ##EXT_patch a patch is a set of edits to change a project based on changes (diffs) generated from other projects
INHERITED(taNBase)
public:
  static String cur_subgp;      // #IGNORE new subgroup for creating new patch records -- if this is non-empty then NewPatchRec methods will create records in this subgroup
 
  String        desc;           // #EDIT_DIALOG description of what this patch does
  String        author;
  // the patch author -- typically based on the project author from where the patch was generated, and defaults to the one saved in emergent preferences
  String        email;
  // email address for comments, questions about the project -- typically the author's email address -- by default the one saved in emergent preferences
  String        date;
  // date timestamp for when this patch was generated

  PatchRec_Group patch_recs;    // patch records -- all the individual edits that make up this patch

  virtual bool   ApplyPatch(taProject* proj);
  // #BUTTON apply this patch to the selected project


  ////////////////////////////////////////
  // Patch generation API

  virtual PatchRec*  NewPatchRec_impl(taBase* obj, const String& val,
                                      const String& subgp = _nilString);
  // #IGNORE
  
  virtual PatchRec*  NewPatchRec_Assign(taBase* obj, const String& val);
  // #IGNORE 

  virtual PatchRec*  NewPatchRec_Replace(taBase* obj, const String& val);
  // #IGNORE 

  virtual PatchRec*  NewPatchRec_Insert
    (taBase* add_obj, taBase* own_obj, taBase* bef_obj, taBase* aft_obj, const String& val);
  // #IGNORE 

  virtual PatchRec*  NewPatchRec_Delete(taBase* obj, const String& val);
  // #IGNORE 
  
  String             GetTypeDecoKey() const override { return "ControlPanel"; }
  
  TA_SIMPLE_BASEFUNS(Patch);
protected:
  String             last_before_path; // for an insert, the last full before path
  String             last_obj_added_path; // for an insert, the last object added for the last_before path -- becomes the new before...
    
private:
  void Initialize()  { };
  void Destroy()     { };
};

#endif // Patch_h

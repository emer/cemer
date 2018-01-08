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

#ifndef Patch_h
#define Patch_h 1

// parent includes:
#include <taNBase>

// member includes:
#include <PatchRec_Group>
#include <PatchLib>

// declare all other types mentioned but not required to include:
class ObjDiff; //

taTypeDef_Of(Patch);

class TA_API Patch : public taNBase {
  // ##CAT_Patch #STEM_BASE ##EXT_patch a patch is a set of edits to change a project based on changes (diffs) generated from other projects
INHERITED(taNBase)
public:
  static String         cur_subgp;      // #IGNORE new subgroup for creating new patch records -- if this is non-empty then NewPatchRec methods will create records in this subgroup
  static PatchLib*      patch_lib; // #TREE_HIDDEN #NO_SAVE #NO_FIND #HIDDEN library of available programs
 
  String        tags;
  // #EDIT_DIALOG list of comma separated tags, initial letter capitalized, that indicate the applicability of this patch -- should be listed in hierarchical order, with most important/general tags first, as this is how they will be sorted in the patch library -- use Load from Patch Lib to see existing tags in use -- best to re-use where possible
  String        desc;
  // #EDIT_DIALOG description of what this patch does (used for searching to find patches -- be thorough!)
  String        author;
  // the patch author -- typically based on the project author from where the patch was generated, and defaults to the one saved in emergent preferences
  String        email;
  // email address for comments, questions about the project -- typically the author's email address -- by default the one saved in emergent preferences
  String        date;
  // date timestamp for when this patch was generated
  String        from_name;
  // name of the object on which changes were being applied, to transform into the to_name object
  String        to_name;
  // name of the object that was the source of changes, to transform the from_name object into this one

  PatchRec_Group patch_recs;    // patch records -- all the individual edits that make up this patch

  virtual bool          ApplyPatch(taProject* proj);
  // #BUTTON apply this patch to the selected project

  virtual void          SavePatch();
  // #BUTTON #CAT_Patch save patch locally to a local file
  
  virtual void          BuildPatchLib();
  // #CAT_IGNORE initialize the patch library -- find all the patches -- called just-in-time when needed
  virtual void          SaveToPatchLib(PatchLib::LibLocs location = PatchLib::USER_LIB);
  // #BUTTON #MENU_CONTEXT #CAT_PatchLib save the patch to given patch library location -- file name = object name -- be sure to add good desc comments -- USER_LIB: user's personal library -- located in app user dir (~/lib/emergent or ~/Library/Emergent patch_lib), SYSTEM_LIB: local system library, installed with software, in /usr/share/Emergent/patch_lib, WEB_APP_LIB: web-based application-specific library (e.g., emergent, WEB_SCI_LIB: web-based scientifically oriented library (e.g., CCN), WEB_USER_LIB: web-based user's library (e.g., from lab wiki)
  virtual void          UpdateFromPatchLib(ObjLibEl* patch_lib_item);
  // #MENU #MENU_ON_Object #MENU_CONTEXT #FROM_LIST_patch_lib.library #ARG_VAL_FM_FUN #PRE_CALL_BuildPatchLib #CAT_PatchLib (re)load the patch from the selected patch library element

  virtual void          RemoveOffs();
  // #BUTTON #CAT_Patch #CONFIRM remove all the patch records that are currently turned off -- useful for cleaning up a merged or edited patch in preparation for sharing

  ////////////////////////////////////////
  // Patch generation API

  virtual PatchRec*  NewRec_impl(const String& subgp = _nilString);
  // #IGNORE
  virtual PatchRec*  NewRec_AssignMbr
    (ObjDiff* diff, bool a_or_b, taBase* trg_indep_obj, taBase* trg_mbr_obj, MemberDef* md, const String& val);
  // #IGNORE 
  virtual PatchRec*  NewRec_AssignObj(ObjDiff* diff, bool a_or_b, taBase* trg_obj, taBase* src_obj);
  // #IGNORE 
  virtual PatchRec*  NewRec_Replace(ObjDiff* diff, bool a_or_b, taList_impl* own_obj, taBase* trg_obj, taBase* src_obj);
  // #IGNORE 
  virtual PatchRec*  NewRec_Delete(ObjDiff* diff, bool a_or_b, taBase* obj);
  // #IGNORE 
  virtual PatchRec*  NewRec_Insert
    (ObjDiff* diff, bool a_or_b, taList_impl* own_obj, taBase* add_obj, taBase* aft_obj, taBase* bef_obj);
  // #IGNORE 

  ////////////////////////////////////////
  // Patch merging API

  virtual PatchRec*  FindClosestRec(PatchRec* prec);
  // #IGNORE find the patch rec here that has the closest match to given one, based on obj_path_names and other factors
  
  String             GetTypeDecoKey() const override { return "ControlPanel"; }
  
  TA_SIMPLE_BASEFUNS(Patch);
protected:
  String             last_insert_own_path; // for an insert, the last before owner path
  int                last_before_idx; // for an insert, the last before index
  int                last_obj_added_idx; // for an insert, the last object added for the last_before path -- becomes the new before...
    
private:
  void Initialize();
  void Destroy()     { };
};

#endif // Patch_h

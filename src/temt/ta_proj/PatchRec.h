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

#ifndef PatchRec_h
#define PatchRec_h 1

// parent includes:
#include <taOBase>

// member includes:

// declare all other types mentioned but not required to include:
class taProject; //
class taList_impl; //

taTypeDef_Of(PatchRec);

class TA_API PatchRec : public taOBase {
  // ##CAT_Patch #STEM_BASE one patch record -- performs one specific action (assign, insert, delete)
INHERITED(taOBase)
public:
  enum PatchActions { // patch actions to perform
    NO_APPLY,         // #COLOR_grey choose this to disable application of this patch element
    ASSIGN,           // #COLOR_purple assign object at given path to given value
    REPLACE,          // #COLOR_darkblue replace object at given path with new one of a different type from saved value
    INSERT,           // #COLOR_green insert new object between path and path_after to given value
    DELETE,           // #COLOR_red delete object at given path -- should hopefully match value -- prompt if not
  };

  PatchActions  action;         // action that this patch record performs
  bool          failed;         // #READ_ONLY #SHOW #NO_SAVE did the last attempt to apply this patch fail?
  String        obj_path_names; // #EDIT_DIALOG project-relative path to object using names -- must be a taBase object -- for INSERT it is the path to the OWNER list to insert a new object in
  String        obj_path_idx;   // #EDIT_DIALOG project-relative path to object using indexes -- must be a taBase object -- for INSERT it is the path to the OWNER list to insert a new object in
  String        obj_type;       // type of object to be found at given obj_path -- for double-checking path finding
  String        mbr_path;       // path to a member of object, if relevant (for ASSIGN only)
  int           targ_idx;       // #CONDSHOW_ON_action:INSERT,REPLACE insert / replace item at this index in the owner list (owner is obj_path)
  String        replace_name;   // #CONDSHOW_ON_action:REPLACE name of item to replace
  String        insert_after;   // #CONDSHOW_ON_action:INSERT insert new item after item with this name in owner list (owner is obj_path) -- blank means first item
  String        insert_before;  // #CONDSHOW_ON_action:INSERT insert new item before item with this name in owner list (owner is obj_path) -- blank means last item
  String        new_obj_type;   // for REPLACE and INSERT, the type name of the new object to create 
  String        value;          // #EDIT_DIALOG string encoded value of object for assign and insert -- also has info about the object that should be deleted, to provide a match

  virtual bool ApplyPatch(taProject* proj);
  // #BUTTON apply this patch record to given project -- can also apply at level of entire patch

  virtual bool ApplyPatch_Assign(taProject* proj);
  // #IGNORE
  virtual bool ApplyPatch_Replace(taProject* proj);
  // #IGNORE
  virtual bool ApplyPatch_Insert(taProject* proj);
  // #IGNORE
    virtual int ApplyPatch_Insert_GetIdx(taList_impl* own);
    // #IGNORE
  virtual bool ApplyPatch_Delete(taProject* proj);
  // #IGNORE

  virtual taBase* FindPathRobust(taProject* proj);
  // find the object on the path robustly
  virtual taList_impl* FindPathRobust_List(taProject* proj);
  // find the object on the path robustly -- must be a taList_impl obj
  virtual taBase* CheckObjType(taProject* proj, taBase* obj, const String& path_used);
  // check that object is of the right type -- if not, do something..

  virtual bool  NewRec_impl(taBase* obj, const String& val);
  // #IGNORE
  virtual bool  NewRec_AssignMbr(taBase* trg_indep_obj, taBase* trg_mbr_obj, MemberDef* md, const String& val);
  // #IGNORE 
  virtual bool  NewRec_AssignObj(taBase* trg_obj, taBase* src_obj);
  // #IGNORE 
  virtual bool  NewRec_Replace(taList_impl* own_obj, taBase* trg_obj, taBase* src_obj);
  // #IGNORE 
  virtual bool  NewRec_Delete(taBase* obj);
  // #IGNORE 
  virtual bool  NewRec_Insert
    (taList_impl* own_obj, taBase* add_obj, taBase* aft_obj, taBase* bef_obj);
  // #IGNORE 
  
  String        GetDisplayName() const override;
  String        GetTypeDecoKey() const override { return "ControlPanel"; }
  int           GetEnabled() const override;
  int           GetSpecialState() const override;
  
  TA_SIMPLE_BASEFUNS(PatchRec);
private:
  void Initialize();
  void Destroy()     { };
};

#endif // PatchRec_h

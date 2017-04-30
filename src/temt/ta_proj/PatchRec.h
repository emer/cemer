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
  String        path_before;    // #CONDSHOW_ON_action:INSERT #EDIT_DIALOG for INSERT, OWNER-relative path for object BEFORE an insertion using names (NULL if first item in list)
  String        path_after;     // #CONSDHOW_ON_action:INSERT #EDIT_DIALOG for INSERT OWNER-relative path for object AFTER the one to be inserted (NULL = end of list)
  String        new_obj_type;   // for REPLACE and INSERT, the type name of the new object to create 
  String        value;          // #EDIT_DIALOG string encoded value of object for assign and insert -- also has info about the object that should be deleted, to provide a match

  virtual bool ApplyPatch(taProject* proj);
  // #BUTTON apply this patch record to given project -- can also apply at level of entire patch

  virtual bool ApplyPatch_assign(taProject* proj);
  // #IGNORE
  virtual bool ApplyPatch_replace(taProject* proj);
  // #IGNORE
  virtual bool ApplyPatch_insert(taProject* proj);
  // #IGNORE
  virtual bool ApplyPatch_delete(taProject* proj);
  // #IGNORE

  virtual taBase* FindPathRobust(taProject* proj);
  // find the object on the path robustly
  virtual taBase* CheckObjType(taProject* proj, taBase* obj, const String& path_used);
  // check that object is of the right type -- if not, do something..

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

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
    ASSIGN,           // assign object at given path to given value
    REPLACE,          // replace object at given path with new one of a different type from saved value
    INSERT,           // insert new object between path and path_after to given value
    DELETE,           // delete object at given path -- should hopefully match value -- prompt if not
  };

  PatchActions  action;         // action that this patch record performs
  String        obj_path_names; // project-relative path to object using names -- must be a taBase object -- for INSERT it is the path to the object AFTER the one to be inserted (NULL = end of list)
  String        obj_path_idx;   // project-relative path to object using indexes -- must be a taBase object -- for INSERT it is the path to the object AFTER the one to be inserted (NULL = end of list)
  String        obj_type;       // type of object to be found at given obj_path -- for double-checking path finding
  String        mbr_path;       // path to a member of object, if relevant (for ASSIGN only)
  String        path_before_names; // project-relative path for object after an insertion using names (NULL if first item in list)
  String        new_obj_type;   // for REPLACE and INSERT, the type name of the new object to create 
  String        value;          // string encoded value of object for assign and insert -- also has info about the object that should be deleted, to provide a match

  virtual bool ApplyPatch(taProject* proj);
  // apply the patch

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
  
  TA_SIMPLE_BASEFUNS(PatchRec);
private:
  void Initialize();
  void Destroy()     { };
};

#endif // PatchRec_h

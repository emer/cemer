// Copyright 2013-2018, Regents of the University of Colorado,
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

#ifndef MemberProgEl_h
#define MemberProgEl_h 1

// parent includes:
#include <ProgEl>

// member includes:
#include <ProgVar>

// declare all other types mentioned but not required to include:
class TypeDef; // 
class MemberDef; // 


taTypeDef_Of(MemberProgEl);

class TA_API MemberProgEl: public ProgEl { 
  // #VIRT_BASE base class for dealing with members of objects
INHERITED(ProgEl)
public:
  ProgVarRef            obj;
  // #ITEM_FILTER_ObjProgVarFilter program variable that points to the object with the method to call
  TypeDef*              obj_type;
  // #HIDDEN #NO_SAVE object lookup type -- set to be the last element on the path that is suitable for lookup
  String                path;
  // path to the member -- can just be member name (use Ctrl+L or member_lookup to lookup and enter here) -- you can also enter in multiple sub-path elements for object members that themselves have members
  MemberDef*            member_lookup;
  // #TYPE_ON_obj_type #NULL_OK #NO_SAVE #NO_EDIT #NO_UPDATE_POINTER lookup a member name -- after you choose, it will copy the name into the path and reset this lookup to NULL
  
  virtual bool          GetTypeFromPath();
  // get obj_type from current path (also gives warnings about bad paths unless quiet = true)
  static TypeDef*       GetObjTypeFromPath
    (const String& path, TypeDef* base_type, taBase* base_obj, taBase*& path_obj);
  // return an object type from path -- if path goes all the way to a non-taBase object, then it gets the parent of that object (recursively) -- sets the object at the end of the found path in path_obj if it is available
  
  static bool           UAEInProgram
    (const String& path, TypeDef* base_type, taBase* base_obj);
  // determine if the base type or any element of the subsequent path from there has the UAE_IN_PROGRAM class flag set -- use for setting update_after flag where needed
  
  void  Help() override;

  PROGEL_SIMPLE_BASEFUNS(MemberProgEl);
protected:
  void          UpdateAfterEdit_impl() override;
  void  CheckThisConfig_impl(bool quiet, bool& rval) override;

private:
  void  Initialize();
  void  Destroy()       {CutLinks();}
}; 

#endif // MemberProgEl_h

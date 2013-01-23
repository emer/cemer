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

#ifndef MemberProgEl_h
#define MemberProgEl_h 1

// parent includes:
#include <ProgEl>

// member includes:
#include <ProgVarRef>

// declare all other types mentioned but not required to include:
class TypeDef; // 
class MemberDef; // 


TypeDef_Of(MemberProgEl);

class TA_API MemberProgEl: public ProgEl { 
  // #VIRT_BASE base class for dealing with members of objects
INHERITED(ProgEl)
public:
  ProgVarRef		obj;
  // #ITEM_FILTER_ObjProgVarFilter program variable that points to the object with the method to call
  TypeDef*		obj_type;
  // #NO_SHOW #NO_SAVE object lookup type -- set to be the last element on the path that is suitable for lookup
  String		path;
  // path to the member -- can just be member name (use Ctrl+L or member_lookup to lookup and enter here) -- you can also enter in multiple sub-path elements for object members that themselves have members
  MemberDef*		member_lookup;
  // #TYPE_ON_obj_type #NULL_OK #NO_SAVE #NO_EDIT #NO_UPDATE_POINTER lookup a member name -- after you choose, it will copy the name into the path and reset this lookup to NULL
  
  virtual bool		GetTypeFromPath(bool quiet = false);
  // get obj_type from current path (also gives warnings about bad paths unless quiet = true)

  override String StringFieldLookupFun(const String& cur_txt, int cur_pos,
				       const String& mbr_name, int& new_pos);
  override void 	Help();

  PROGEL_SIMPLE_BASEFUNS(MemberProgEl);
protected:
  override void		UpdateAfterEdit_impl();
  override void 	CheckThisConfig_impl(bool quiet, bool& rval);

private:
  void	Initialize();
  void	Destroy()	{CutLinks();}
}; 

#endif // MemberProgEl_h

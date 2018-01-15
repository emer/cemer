// Copyright 2013-2018, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of Emergent
//
//   Emergent is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   Emergent is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.

#ifndef BaseSpec_Group_h
#define BaseSpec_Group_h 1

// parent includes:
#include "network_def.h"
#include <taGroup>

// member includes:

// declare all other types mentioned but not required to include:
class BaseSpec; //

eTypeDef_Of(BaseSpec_Group);

class E_API BaseSpec_Group: public taGroup<BaseSpec> {
  // ##EXT_spec ##CAT_Spec ##SCOPE_Network group of specs
INHERITED(taGroup<BaseSpec>)
public:
  static bool nw_itm_def_arg;   // #IGNORE default arg val for FindMake..


  virtual BaseSpec*     FindSpecType(TypeDef* td);
  // find a spec of eactly given type, searching in the children too
  virtual BaseSpec*     FindSpecInherits(TypeDef* td, taBase* for_obj = NULL);
  // find a spec that inherits from given type, searching in the children too

  virtual BaseSpec*     FindSpecTypeNotMe(TypeDef* td, BaseSpec* not_me);
  // find a spec of eactly given type, searching in the children too, but skip not_me
  virtual BaseSpec*     FindSpecInheritsNotMe(TypeDef* td, BaseSpec* not_me, taBase* for_obj = NULL);
  // find a spec that inherits from given type, searching in the children too

  virtual BaseSpec*     FindSpecName(const String& nm);
  // find a spec with given name, also searches in the children of each spec

  virtual BaseSpec*     FindMakeSpec(const String& nm, TypeDef* td, bool& nw_itm = nw_itm_def_arg, const String& alt_nm = NULLStr);
  // find a given spec and if not found, make it  (if nm is not found and alt_nm != NULL, it is searched for)

  virtual bool          RemoveSpec(const String& nm, TypeDef* td);
  // find a given spec and remove it

  virtual BaseSpec*     FindParent();
  // #MENU #USE_RVAL #MENU_ON_Actions Find the parent spec of this one

  String        GetTypeDecoKey() const override;

  virtual void  SetParam(TypeDef* spec_type, const String& param_path, const String& value);
  // #BUTTON #DYN1 #TYPE_ON_el_base Set parameter at given path to given value, for all the specs inheriting from given type within this group, and, via the spec inheritance mechanism, all the child specs underneath these specs -- also sets the unique flag for each spec in this group

  virtual void  Defaults();
  // #BUTTON #CONFIRM #CAT_ObjectMgmt for all specs in this group: restore specs to their default original parameter values, for parameters that have a strong default value -- WARNING: you will lose any unique parameters for anything that has a strong default value
  
  virtual void  UpdateAllSpecs();
  // update all of the specs within this group
  virtual void  ResetAllSpecIdxs();
  // #IGNORE reset all the state spec_idx spec indexes to -1 -- called prior to building so only used specs have a valid spec_idx

  void RestorePanels();
  // if the panel was pinned when the program was saved redisplay it on project open

  void  Initialize();
  void  Destroy()               { }
  TA_BASEFUNS_NOCOPY(BaseSpec_Group);
  
protected:
  void*         El_CopyN_(void* to, void* fm) override; // wrap in an update bracket

};

TA_SMART_PTRS(E_API, BaseSpec_Group)

#endif // BaseSpec_Group_h

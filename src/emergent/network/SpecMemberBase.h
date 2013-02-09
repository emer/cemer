// Copyright, 1995-2013, Regents of the University of Colorado,
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

#ifndef SpecMemberBase_h
#define SpecMemberBase_h 1

// parent includes:
#include "network_def.h"
#include <taOBase>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(SpecMemberBase);

class E_API SpecMemberBase : public taOBase {
  // ##EXT_spec ##SCOPE_Network ##CAT_Spec base type for members of spec objects -- defines interface for default values
friend class BaseSpec;
INHERITED(taOBase)
public:

  virtual void	Defaults();
  // #BUTTON #CONFIRM #CAT_ObjectMgmt restore specs to their default original parameter values, for parameters that have a strong default value -- WARNING: you will lose any unique parameters for anything that has a strong default value
  // note: typically do NOT redefine basic Defaults function -- see SPEC_DEFAULTS comment above

  override void		MemberUpdateAfterEdit(MemberDef* md, bool edit_dialog = false);

  TA_BASEFUNS_NOCOPY(SpecMemberBase);
protected:
  virtual void	Defaults_impl() { };
  // #IGNORE this is base of impl class -- derived cases should call inherited then do their thing (usu just Defaults_init) -- very much like UAE_impl -- note that Defaults cals Defaults_Members
  virtual void	DefaultsMembers();
  // #IGNORE just a simple loop to call defaults on all the SpecMemberBase and BaseSpec member objects in a class

private:
  void 	Initialize() { };
  void	Destroy()    { };
  void	Defaults_init() { };
  // #IGNORE put basic default params init stuff in here -- can check other params that might have been set -- Initialize can call this + do other housekeeping inits, or this can call Initialize if the two are sufficiently synonymous
};

#endif // SpecMemberBase_h

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

#ifndef BaseSubSpec_h
#define BaseSubSpec_h 1

// parent includes:
#include "network_def.h"
#include <taNBase>

// member includes:

// declare all other types mentioned but not required to include:
class BaseSpec; //

TypeDef_Of(BaseSubSpec);

class EMERGENT_API BaseSubSpec : public taNBase {
  // ##EXT_spec ##MEMB_IN_GPMENU ##CAT_Spec specification class for sub-objects of specs
INHERITED(taNBase)
public:
  String_Array unique; // #HIDDEN string list of unique members
  virtual BaseSubSpec*	FindParent();
  // #MENU #USE_RVAL #MENU_ON_Actions Find the corresponding parent subspec of this one
  virtual BaseSpec*	FindParentBaseSpec();
  // #MENU #USE_RVAL Find the parent spec of this one

  virtual void	SetUnique(int memb_no, bool on); // set inherit bit
  virtual void	SetUnique(const char* memb_nm, bool on); // set inherit bit
  virtual bool	GetUnique(int memb_no);	     	// check inherit bit
  virtual bool	GetUnique(const char* memb_nm);	// check inherit bit

  virtual void	UpdateMember(BaseSubSpec* from, int memb_no);
  // copy member from given sub spec if not unique
  virtual void	UpdateSpec();
  // update from parent sub spec, if one exists

  void	InitLinks();
  TA_BASEFUNS(BaseSubSpec);
protected:
  void	UpdateAfterEdit_impl();
private:
  void 	Copy_(const BaseSubSpec& cp);
  void 	Initialize();
  void	Destroy();
};

#endif // BaseSubSpec_h

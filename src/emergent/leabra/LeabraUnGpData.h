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

#ifndef LeabraUnGpData_h
#define LeabraUnGpData_h 1

// parent includes:
#include <taOBase>
#include <LeabraInhib>

// member includes:
#include <LeabraSort>
#include <AvgMaxVals>

// declare all other types mentioned but not required to include:
class taBase; // 
class TypeDef; // 
class taMatrix; // 
class taBaseItr; // 
class MemberDef; // 
class taBase_PtrList; // 
class taObjDiff_List; // 
class taDoc; // 
class SelectEdit; // 
class MethodDef; // 
class UserDataItem_List; // 
class UserDataItemBase; // 
class UserDataItem; // 
class LeabraLayerSpec; // 

eTypeDef_Of(LeabraUnGpData);

class E_API LeabraUnGpData : public taOBase, public LeabraInhib {
  // #STEM_BASE ##CAT_Leabra data to maintain for independent unit groups of competing units within a single layer -- storing separately allows unit groups to be virtual (virt_groups flag on layer)
INHERITED(taOBase)
public:

  String 	GetTypeDecoKey() const override { return "Unit"; }

  virtual void		Init_State();
  // initialize state -- called during InitWeights -- mainly for derived classes

  void	InitLinks();
  void	Copy_(const LeabraUnGpData& cp);
  TA_BASEFUNS(LeabraUnGpData);
private:
  void	Initialize();
  void	Destroy()		{ };
};

#endif // LeabraUnGpData_h

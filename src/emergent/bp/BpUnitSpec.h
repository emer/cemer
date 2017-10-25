// Copyright 2017, Regents of the University of Colorado,
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

#ifndef BpUnitSpec_h
#define BpUnitSpec_h 1

// parent includes:
#include <UnitSpec>

// member includes:

// full standalone C++ implementation State code
#include <BpNetworkState_cpp>
#include <BpUnitState_cpp>
#include <BpUnitSpec_cpp>

#include <State_main>

// declare all other types mentioned but not required to include:

eTypeDef_Of(BpUnitState_cpp);
eTypeDef_Of(NLXX1ActSpec);
eTypeDef_Of(GaussActSpec);

#include <BpUnitSpec_mbrs>

eTypeDef_Of(BpUnitSpec);

class E_API BpUnitSpec : public UnitSpec {
  // Backpropagation version of unit spec
INHERITED(UnitSpec)
public:

#include <BpUnitSpec_core>

  virtual void	GraphActFun(DataTable* graph_data, float min = -5.0, float max = 5.0);
  // #BUTTON #NULL_OK graph the activation function along with derivative (NULL = new graph log)

  void 	(*err_fun)(BpUnitSpec* spec, BpUnitState_cpp* u);
  // #LIST_BpUnit_Error #OBSOLETE #HIDDEN #READ_ONLY #NO_SAVE replaced by enum -- this points to the error fun, set appropriately
  
  TA_BASEFUNS(BpUnitSpec);
  SIMPLE_COPY(BpUnitSpec);
  void InitLinks() override;
protected:
  SPEC_DEFAULTS;
  void UpdateAfterEdit_impl() override;
private:
  void Initialize();
  void Destroy() { };
};

// These are now OBSOLETE -- remove sometime (as of 8/2016, version 8.0)

// #REG_FUN
E_API void Bp_Squared_Error(BpUnitSpec* spec, BpUnitState_cpp* u)
// #LIST_BpUnit_Error #OBSOLETE -- replaced with enum -- Squared error function for bp
     ;				// term here so scanner picks up comment
// #REG_FUN
E_API void Bp_CrossEnt_Error(BpUnitSpec* spec, BpUnitState_cpp* u)
// #LIST_BpUnit_Error #OBSOLETE -- replaced with enum -- Cross entropy error function for bp
     ;				// term here so scanner picks up comment

#endif // BpUnitSpec_h

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

#ifndef BpUnit_h
#define BpUnit_h 1

// parent includes:
#include <Unit>

// member includes:
#include <BpUnitState_cpp>

#include <State_main>

// declare all other types mentioned but not required to include:

eTypeDef_Of(BpUnit);

class E_API BpUnit : public Unit {
  // #STEM_BASE ##CAT_Bp standard feed-forward Bp unit
INHERITED(Unit)
public:
  inline UnitState_cpp::ExtFlags ext_flag() { return GetUnitState()->ext_flag; }
  // #CAT_UnitVar external input flags -- determines whether the unit is receiving an external input (EXT), target (TARG), or comparison value (COMP)
  inline float& targ()  { return GetUnitState()->targ; }
  // #VIEW_HOT #CAT_UnitVar target value: drives learning to produce this activation value
  inline float& ext()   { return GetUnitState()->ext; }
  // #VIEW_HOT #CAT_UnitVar external input: drives activation of unit from outside influences (e.g., sensory input)
  inline float& act()   { return GetUnitState()->act; }
  // #VIEW_HOT #CAT_UnitVar activation value -- what the unit communicates to others
  inline float& net()   { return GetUnitState()->net; }
  // #VIEW_HOT #CAT_UnitVar net input value -- what the unit receives from others  (typically sum of sending activations times the weights)
  inline float& bias_wt() { return GetUnitState()->bias_wt; }
  // #VIEW_HOT #CAT_UnitVar bias weight value -- the bias weight acts like a connection from a unit that is always active with a constant value of 1 -- reflects intrinsic excitability from a biological perspective
  inline float& bias_dwt() { return GetUnitState()->bias_dwt; }
  // #VIEW_HOT #CAT_UnitVar change in bias weight value as computed by a learning mechanism

  inline float& bias_pdw()
  { return ((BpUnitState_cpp*)GetUnitState())->bias_pdw; }
  // #VIEW_HOT #CAT_UnitVar previous bias weight change
  inline float&	err()
  { return ((BpUnitState_cpp*)GetUnitState())->err; }
  // #VIEW_HOT #CAT_UnitVar error value -- this is E for target units, not dEdA
  inline float& dEdA()
  { return ((BpUnitState_cpp*)GetUnitState())->dEdA; }
  // #VIEW_HOT #CAT_UnitVar derivative of error wrt activation
  inline float& dEdNet()
  { return ((BpUnitState_cpp*)GetUnitState())->dEdNet; }
  // #VIEW_HOT #CAT_UnitVar derivative of error wrt net input
  inline float& misc1()
  { return ((BpUnitState_cpp*)GetUnitState())->misc1; }
  // #VIEW_HOT #CAT_UnitVar miscellaneous computational value -- used for exp(netin) in SOFTMAX case, and to hold the index of the most active unit among input connections for MAX_POOL (cast to int)

  TA_BASEFUNS_NOCOPY(BpUnit);
private:
  void 	Initialize()            { };
  void 	Destroy()		{ };
};

#endif // BpUnit_h

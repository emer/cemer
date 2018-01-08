// Co2018ght 2013-2017, Regents of the University of Colorado,
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

#ifndef UnitSpec_h
#define UnitSpec_h 1

#include <State_main>

// parent includes:
#include <BaseSpec>
#include <SpecPtr>

// member includes:
#include <MinMaxRange>
#include <ConSpec>

// full standalone C++ implementation State code
#include <NetworkState_cpp>
#include <ConSpec_cpp>

#include <State_main>

// declare all other types mentioned but not required to include:
class Layer; //
class Network; //

eTypeDef_Of(UnitSpec);

class E_API UnitSpec: public BaseSpec {
  // ##CAT_Spec Generic Unit Specification
INHERITED(BaseSpec)
public:

  ConSpec_SPtr  bias_spec;
  // #CAT_Structure con spec that controls the bias connection on the unit

#include <UnitSpec_core>
  
  ////////////////////////////////////////////////////////////////////////////////
  //    The following are misc functionality not required for primary computing

  inline ConSpec* GetMainBiasSpec() const {  return bias_spec;  }
  // #CAT_State get the main bias spec (not cpp)
  virtual TypeDef* MinBiasSpecType() const { return &TA_ConSpec; }
  // #IGNORE overload in subclasses to ensure that bias spec is at least of a given type
  virtual void  CheckBiasSpec();
  // #IGNORE check that our bias spec is of appropriate type, emit error if not
  
  virtual bool  CheckConfig_Unit(Layer* lay, bool quiet=false);
  // #CAT_ObjectMgmt check for for misc configuration settings required by different algorithms

  String        GetTypeDecoKey() const override { return "UnitSpec"; }
  String        GetToolbarName() const override { return "unit spec"; }

  void  UpdateStateSpecs() override;
  void  ResetAllSpecIdxs() override;
  
  void  InitLinks() override;
  void  CutLinks() override;
  void  Copy_(const UnitSpec& cp);
  TA_BASEFUNS(UnitSpec);
protected:
  SPEC_DEFAULTS;
  void         UpdateAfterEdit_impl() override;
  void         CheckThisConfig_impl(bool quiet, bool& ok) override;
  bool         CheckType_impl(TypeDef* td) override;
  bool         CheckObjectType_impl(taBase* obj) override;
private:
  void  Initialize();
  void  Destroy()               { };
  void  Defaults_init()         { };
};

SPECPTR_OF(UnitSpec);

#endif // UnitSpec_h

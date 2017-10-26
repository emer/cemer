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

#ifndef ConSpec_h
#define ConSpec_h 1

// parent includes:
#include <BaseSpec>
#include <SpecMemberBase>
#include <SpecPtr>

// member includes:
#include <Connection>
#include <Random>

// full standalone C++ implementation State code
#include <NetworkState_cpp>
#include <ConState_cpp>
#include <UnitState_cpp>
#include <LayerState_cpp>
#include <ConSpec_cpp>

#include <State_main>
eTypeDef_Of(WeightLimits);

// conspec member classes
#include <ConSpec_mbrs>

#include <State_main>

// declare all other types mentioned but not required to include:
class Network; //
class Projection; //

// the ConSpec has 2 versions of every function: one to go through the group
// and the other to apply to a single connection.
// The C_ denotes the con one which is non-virtual so that it is inlined
// this requires the group level one to be re-defined whenever the con
// level one is.  This is necessary given the speed penalty for a virtual
// function call at the connection level.

// The exception to the non-virtual rule is the C_Init functions!

eTypeDef_Of(ConSpec);

class E_API ConSpec: public BaseSpec {
  // ##CAT_Spec Connection specs: for processing over a set of connections all from the same projection -- all ConState functions should be called on one that owns the connections
INHERITED(BaseSpec)
public:

  // contains shared elements
#include <ConSpec_core>
  
  ////////////////////////////////////////////////////////////////////////////////
  //    The following are misc functionality not required for primary computing

  virtual void          GetPrjnName(Projection& prjn, String& nm) { };
  // add anything special for this type of connection to the projection name -- default name (FM_from) is provided as a nm value, which this function can modify in any way

  virtual bool          CheckConfig_RecvCons(Projection* prjn, bool quiet=false);
  // check for for misc configuration settings required by different algorithms

  virtual  void         Init_Weights_Net();
  // #BUTTON #CAT_Learning initializes all weights in the network

  String          GetTypeDecoKey() const override { return "ConSpec"; }
  String          GetToolbarName() const override { return "con spec"; }

  void  UpdateStateSpecs() override;
  
  void  InitLinks() override;
  void  CutLinks() override;
  void  Copy_(const ConSpec& cp);
  TA_BASEFUNS(ConSpec);
protected:
  SPEC_DEFAULTS;
  bool  CheckType_impl(TypeDef* td) override;
  bool  CheckObjectType_impl(taBase* obj) override; // don't do checking on 1st con group in units
  void  UpdateAfterEdit_impl() override;
private:
  void  Initialize();
  void  Destroy()               { CutLinks(); }
  void  Defaults_init()         { };
};

SPECPTR_OF(ConSpec);

#endif // ConSpec_h

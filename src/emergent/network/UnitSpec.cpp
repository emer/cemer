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

#include "UnitSpec.h"
#include <Network>
#include <Layer>

#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(UnitSpec);
TA_BASEFUNS_CTORS_LITE_DEFN(UnitSpec_SPtr);
SMARTREF_OF_CPP(UnitSpec);

eTypeDef_Of(UnitState_cpp);

void UnitSpec::Initialize() {
  Initialize_core();
  min_obj_type = &TA_UnitState_cpp;
  min_user_type = &TA_Layer;
}

void UnitSpec::InitLinks() {
  inherited::InitLinks();
  children.SetBaseType(&TA_UnitSpec); // allow all of this general spec type to be created under here
  children.el_typ = GetTypeDef(); // but make the default to be me!
  taBase::Own(act_range, this);
  taBase::Own(bias_spec, this);
  // don't do this if loading -- creates specs in all the wrong places..
  // specs that own specs have this problem
  if(!taMisc::is_loading) {
    Network* net = (Network *) GET_MY_OWNER(Network);
    if(net && !net->HasBaseFlag(COPYING))
      bias_spec.SetDefaultSpec(this);
  }
}

void UnitSpec::CutLinks() {
  bias_spec.CutLinks();
  inherited::CutLinks();
}

void UnitSpec::Copy_(const UnitSpec& cp) {
  act_range = cp.act_range;
  bias_spec = cp.bias_spec;
  sse_tol = cp.sse_tol;
}

void UnitSpec::CheckBiasSpec() {
  ConSpec* bs = GetMainBiasSpec();
  if(!bs) return;
  TypeDef* min_bs = MinBiasSpecType();
  TestError(!bs->InheritsFrom(min_bs), "CheckBiasSpec",
            "Unit spec does not have a bias_spec of the appropriate minimum type:",
            min_bs->name, "bias_spec name:", bs->name, "type:", bs->GetTypeDef()->name);
}

bool UnitSpec::CheckConfig_Unit(Layer* lay, bool quiet) {
  return true;
}

void UnitSpec::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
}

bool UnitSpec::CheckType_impl(TypeDef* td) {
  // other specs are allowed to own any kind of other spec,
  // and layers and projections also contain specs..
  if (td->InheritsFrom(TA_Layer))
    return true;
  return inherited::CheckType_impl(td);
}

bool UnitSpec::CheckObjectType_impl(taBase* obj) {
  // other specs are allowed to own any kind of other spec,
  // and layers and projections also contain specs..
  if (obj->InheritsFrom(TA_Layer))
    return true;
  return inherited::CheckObjectType_impl(obj);
}

void UnitSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  act_range.UpdateAfterEdit_NoGui();
}

void UnitSpec::UpdateStateSpecs() {
  Network* net = GET_MY_OWNER(Network);
  if(!net || !net->IsBuiltIntact() || spec_idx < 0) return;
  CopyToState(net->net_state->unit_specs[spec_idx], net->net_state->GetStateSuffix());
#ifdef CUDA_COMPILE
  CopyToState(net->cuda_state->unit_specs[spec_idx], net->cuda_state->GetStateSuffix());
#endif
}

void UnitSpec::ResetAllSpecIdxs() {
  spec_idx = -1;
  inherited::ResetAllSpecIdxs(); // calls on children
}

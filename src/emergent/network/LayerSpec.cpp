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

#include "LayerSpec.h"

#include <Network>
#include <Layer>

TA_BASEFUNS_CTORS_DEFN(LayerSpec);

void LayerSpec::Initialize() {
  min_user_type = &TA_Layer;

  Initialize_core();
}

void LayerSpec::InitLinks() {
  BaseSpec::InitLinks();
  children.SetBaseType(&TA_LayerSpec); // allow all of this general spec type to be created under here
  children.el_typ = GetTypeDef(); // but make the default to be me!
}

void LayerSpec::CutLinks() {
  BaseSpec::CutLinks();
}

void LayerSpec::UpdateStateSpecs() {
  Network* net = GET_MY_OWNER(Network);
  if(!net || !net->IsBuiltIntact() || spec_idx < 0) return;
  CopyToState(net->net_state->layer_specs[spec_idx], net->net_state->GetStateSuffix());
#ifdef CUDA_COMPILE
  CopyToState(net->cuda_state->layer_specs[spec_idx], net->cuda_state->GetStateSuffix());
#endif
}

void LayerSpec::ResetAllSpecIdxs() {
  spec_idx = -1;
  inherited::ResetAllSpecIdxs(); // calls on children
}

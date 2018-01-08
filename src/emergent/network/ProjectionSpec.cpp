// Copyright 2013-2017, Regents of the University of Colorado,
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

#include "ProjectionSpec.h"

#include <ConSpec_cpp>
#include <Projection>
#include <Layer>

#include <State_main>

#include <Network>

TA_BASEFUNS_CTORS_DEFN(RenormInitWtsSpec);
TA_BASEFUNS_CTORS_DEFN(ProjectionSpec);
TA_BASEFUNS_CTORS_LITE_DEFN(ProjectionSpec_SPtr);
SMARTREF_OF_CPP(ProjectionSpec);

void ProjectionSpec::Initialize() {
  min_obj_type = &TA_Projection;
  min_user_type = &TA_Projection;

  Initialize_core_base();
}

void ProjectionSpec::InitLinks() {
  BaseSpec::InitLinks();
  children.SetBaseType(&TA_ProjectionSpec); // allow all of this general spec type to be created under here
  taBase::Own(renorm_wts, this);
  children.el_typ = GetTypeDef(); // but make the default to be me!
}

void ProjectionSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();

  if(set_scale) {
    Network* net = GET_MY_OWNER(Network);
    if(TestWarning(net && !net->InheritsFromName("LeabraNetwork"),
                   "Connect_impl", "set_scale can only be used with Leabra networks -- turning off")) {
      set_scale = false;
    }
  }
}

bool ProjectionSpec::CheckConnect(Projection* prjn, bool quiet) {
  if(prjn->off) return true;
  bool rval;
  if(prjn->CheckError(!prjn->from, quiet, rval, "from is null -- must set from!")) {
    return false;
  }
  if(prjn->from->lesioned()) return true;
  if(prjn->CheckError(!prjn->con_spec.spec, quiet, rval, "has null con_spec")) {
    return false;
  }
  if(prjn->CheckError(!prjn->con_spec->CheckObjectType(prjn), quiet, rval,
                "does not have correct spec/object type")) {
    return false;
  }
  return true;
}

void ProjectionSpec::UpdateStateSpecs() {
  Network* net = GET_MY_OWNER(Network);
  if(!net || !net->IsBuiltIntact() || spec_idx < 0) return;
  CopyToState(net->net_state->prjn_specs[spec_idx], net->net_state->GetStateSuffix());
#ifdef CUDA_COMPILE
  CopyToState(net->cuda_state->prjn_specs[spec_idx], net->cuda_state->GetStateSuffix());
#endif
}

void ProjectionSpec::ResetAllSpecIdxs() {
  spec_idx = -1;
  inherited::ResetAllSpecIdxs(); // calls on children
}

// impl
#include "ProjectionSpec_core.cpp"


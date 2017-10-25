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

#include "LeabraLayer.h"
#include <LeabraNetwork>
#include <LeabraUnitSpec>

eTypeDef_Of(TwoDValLayerSpec);

TA_BASEFUNS_CTORS_DEFN(LeabraLayer);
SMARTREF_OF_CPP(LeabraLayer);

void LeabraLayer::Initialize() {
  spec.SetBaseType(&TA_LeabraLayerSpec);
  projections.SetBaseType(&TA_LeabraPrjn);
  send_prjns.SetBaseType(&TA_LeabraPrjn);
  unit_spec.SetBaseType(&TA_LeabraUnitSpec);

  Initialize_lay_core();
  
#ifdef DMEM_COMPILE
  dmem_agg_sum.agg_op = MPI_SUM;
#endif
}

void LeabraLayer::InitLinks() {
  inherited::InitLinks();
  taBase::Own(spec, this);

  taBase::Own(margin, this);
  taBase::Own(avg_netin, this);
  taBase::Own(avg_netin_sum, this);

#ifdef DMEM_COMPILE
  taBase::Own(dmem_agg_sum, this);
  DMem_InitAggs();
#endif
  spec.SetDefaultSpec(this);

  AutoNameMyMembers();
}

void LeabraLayer::CutLinks() {
  inherited::CutLinks();
  spec.CutLinks();
}

void LeabraLayer::CheckSpecs() {
  spec.CheckSpec();
  inherited::CheckSpecs();
}

void LeabraLayer::CheckThisConfig_impl(bool quiet, bool& rval) {
  if(lesioned()) return;
  inherited::CheckThisConfig_impl(quiet, rval);

  if(!spec->CheckConfig_Layer(this, quiet)) {
    rval = false;
  }
}

bool LeabraLayer::SetLayerSpec(LayerSpec* sp) {
  if(sp == NULL)        return false;
  if(sp->CheckObjectType(this))
    spec.SetSpec((LeabraLayerSpec*)sp);
  else
    return false;
  return true;
}


#ifdef DMEM_COMPILE
void LeabraLayer::DMem_InitAggs() {
  dmem_agg_sum.ScanMembers(GetTypeDef(), (void*)this);
  dmem_agg_sum.CompileVars();
}
void LeabraLayer::DMem_ComputeAggs(MPI_Comm comm) {
  dmem_agg_sum.AggVar(comm, MPI_SUM);
}
#endif

bool LeabraLayer::TwoDValMode() {
  LeabraLayerSpec* ls = (LeabraLayerSpec*)GetMainLayerSpec();
  if(!ls) return false;
  // return (ls->InheritsFrom(&TA_TwoDValLayerSpec));
  return false;
}

void LeabraLayer::ApplyInputData_2d(NETWORK_STATE* net, taMatrix* data, ExtFlags ext_flags,
                                    Random* ran, const taVector2i& offs, bool na_by_range) {
  if(TwoDValMode()) {
    // todo: needs lgpd
    // only no unit_group supported!
    if(TestError(unit_groups, "ApplyInputData_2d",
                 "input data must be 4d for layers with unit_groups: outer 2 are group dims, inner 2 are x,y vals and n_vals")) {
      return;
    }
    if(TestError(data->dim(0) < 2, "ApplyInputData_2d",
                 "input data must have at inner dimension of at least 2 for x,y vals")) {
      return;
    }
    // todo: this just overwrites if dim(1) > 1 - not really sure what the point is!?
    for(int d_y = 0; d_y < data->dim(1); d_y++) {
      // if(ext_flags & EXT) {
      //   twod.ext_x = data->SafeElAsVar(0, d_y);
      //   twod.ext_y = data->SafeElAsVar(1, d_y);
      // }
      // else {                    // targ
      //   twod.targ_x = data->SafeElAsVar(0, d_y);
      //   twod.targ_y = data->SafeElAsVar(1, d_y);
      // }
    }
  }
  else {
    inherited::ApplyInputData_2d(net, data, ext_flags, ran, offs, na_by_range);
  }
}

void LeabraLayer::ApplyInputData_Flat4d(NETWORK_STATE* net, taMatrix* data, ExtFlags ext_flags,
                                        Random* ran, const taVector2i& offs, bool na_by_range) {
  if(TwoDValMode()) {
    // outer-loop is data-group (groups of x-y data items)
    if(TestError(!unit_groups, "ApplyInputData_Flat4d",
                 "input data must be 2d for layers without unit_groups: x,y vals and n_vals")) {
      return;
    }
    for(int dg_y = 0; dg_y < data->dim(3); dg_y++) {
      for(int dg_x = 0; dg_x < data->dim(2); dg_x++) {

        for(int d_y = 0; d_y < data->dim(1); d_y++) {
          int u_y = offs.y + dg_y * data->dim(1) + d_y; // multiply out data indicies
          for(int d_x = 0; d_x < data->dim(0); d_x++) {
            int u_x = offs.x + dg_x * data->dim(0) + d_x; // multiply out data indicies
            // todo: I don't get what is going on here!?
            // Unit* un = UnitAtCoord(u_x, u_y);
            // if(un) {
            //   float val = data->SafeElAsVar(d_x, d_y, dg_x, dg_y).toFloat();
            //   un->ApplyInputData(val, (UnitState_cpp::ExtFlags)ext_flags, ran, na_by_range);
            // }
          }
        }
      }
    }
  }
  else {
    inherited::ApplyInputData_Flat4d(net, data, ext_flags, ran, offs, na_by_range);
  }
}

void LeabraLayer::ApplyInputData_Gp4d(NETWORK_STATE* net, taMatrix* data, ExtFlags ext_flags,
                                      Random* ran, bool na_by_range) {
  if(TwoDValMode()) {
    // outer-loop is data-group (groups of x-y data items)
    for(int dg_y = 0; dg_y < data->dim(3); dg_y++) {
      for(int dg_x = 0; dg_x < data->dim(2); dg_x++) {

        for(int d_y = 0; d_y < data->dim(1); d_y++) {
          int val_idx = d_y;
          // todo: access unit group state, put this there.
          // if(ext_flags & EXT) {
          //   twod.ext_x = data->SafeElAsVar(0, d_y, dg_x, dg_y);
          //   twod.ext_y = data->SafeElAsVar(1, d_y, dg_x, dg_y);
          // }
          // else {                    // targ
          //   twod.targ_x = data->SafeElAsVar(0, d_y, dg_x, dg_y);
          //   twod.targ_y = data->SafeElAsVar(1, d_y, dg_x, dg_y);
          // }
        }
      }
    }
  }
  else {
    inherited::ApplyInputData_Gp4d(net, data, ext_flags, ran, na_by_range);
  }
}

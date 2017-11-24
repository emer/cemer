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

#include "LeabraLayerSpec.h"
#include <LeabraNetwork>
#include <LeabraUnitSpec>
#include <LeabraConSpec>
#include <MemberDef>

#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(LeabraInhibSpec);
TA_BASEFUNS_CTORS_DEFN(LayerAvgActSpec);
TA_BASEFUNS_CTORS_DEFN(LeabraAdaptInhib);
TA_BASEFUNS_CTORS_DEFN(LeabraActMargin);
TA_BASEFUNS_CTORS_DEFN(LeabraInhibMisc);
TA_BASEFUNS_CTORS_DEFN(LeabraClampSpec);
TA_BASEFUNS_CTORS_DEFN(LayerDecaySpec);
TA_BASEFUNS_CTORS_DEFN(LeabraDelInhib);
TA_BASEFUNS_CTORS_DEFN(LeabraCosDiffMod);
TA_BASEFUNS_CTORS_DEFN(LeabraLayStats);

TA_BASEFUNS_CTORS_DEFN(LeabraLayerSpec);

TA_BASEFUNS_CTORS_LITE_DEFN(LeabraLayerSpec_SPtr);
SMARTREF_OF_CPP(LeabraLayerSpec);

eTypeDef_Of(MarkerConSpec);

#include <LeabraUnitSpec_cpp>
#include <LeabraConSpec_cpp>
#include <LeabraLayerSpec_cpp>

#include <State_main>

#include "LeabraLayerSpec_core.cpp"


//////////////////////////////////////////////////////////


void LeabraLayerSpec::Initialize() {
  min_obj_type = &TA_LeabraLayer;
  Defaults_init();
  Initialize_core();
}

void LeabraLayerSpec::Defaults_init() {
}

void LeabraLayerSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();

  lay_inhib.UpdateAfterEdit_NoGui();
  unit_gp_inhib.UpdateAfterEdit_NoGui();
  lay_gp_inhib.UpdateAfterEdit_NoGui();
  avg_act.UpdateAfterEdit_NoGui();
  cos_diff.UpdateAfterEdit_NoGui();
}

void LeabraLayerSpec::InitLinks() {
  inherited::InitLinks();
  InitLinks_taAuto(&TA_LeabraLayerSpec);
  children.SetBaseType(&TA_LeabraLayerSpec);
  children.el_typ = GetTypeDef(); // but make the default to be me!
}

bool LeabraLayerSpec::CheckConfig_Layer(Layer* ly, bool quiet) {
  LeabraLayer* lay = (LeabraLayer*)ly;
  bool rval = true;

  LeabraNetwork* net = (LeabraNetwork*)lay->own_net;

  if(lay->CheckError(!lay->projections.el_base->InheritsFrom(&TA_LeabraPrjn), quiet, rval,
                "does not have LeabraPrjn projection base type!",
                "project must be updated and projections remade"))
    return false;

  return rval;
}

void LeabraLayerSpec::HelpConfig() {
  String help = "LeabraLayerSpec Configuration:\n\
The layer spec sets the inhibitory parameters for each layer, along with \
the expected initial average activation per layer, which is used for \
netinput rescaling.  Therefore, you must have a different layer spec \
with an appropriate activity level for layers that have different activity levels.";
  taMisc::Confirm(help);
}

LeabraLayer* LeabraLayerSpec::FindLayerFmSpec(LeabraLayer* lay, int& prjn_idx, TypeDef* layer_spec) {
  LeabraLayer* rval = NULL;
  prjn_idx = -1;
  for(int pi=0; pi < lay->projections.size; pi++) {
    Projection* prjn = lay->projections[pi];
    LeabraLayer* fmlay = (LeabraLayer*)prjn->from.ptr();
    if(fmlay->spec.SPtr()->InheritsFrom(layer_spec)) {  // inherits - not excact match!
      prjn_idx = prjn->recv_idx;
      rval = fmlay;
      break;
    }
  }
  return rval;
}

LeabraLayer* LeabraLayerSpec::FindLayerFmSpecExact(LeabraLayer* lay, int& prjn_idx, TypeDef* layer_spec) {
  LeabraLayer* rval = NULL;
  prjn_idx = -1;
  for(int pi=0; pi < lay->projections.size; pi++) {
    Projection* prjn = lay->projections[pi];
    LeabraLayer* fmlay = (LeabraLayer*)prjn->from.ptr();
    if(fmlay->spec.SPtr()->GetTypeDef() == layer_spec) {        // not inherits - excact match!
      prjn_idx = prjn->recv_idx;
      rval = fmlay;
      break;
    }
  }
  return rval;
}

LeabraLayer* LeabraLayerSpec::FindLayerFmSpecNet(Network* net, TypeDef* layer_spec) {
  FOREACH_ELEM_IN_GROUP(LeabraLayer, lay, net->layers) {
    if(lay->spec.SPtr()->InheritsFrom(layer_spec)) {    // inherits - not excact match!
      return lay;
    }
  }
  return NULL;
}


///////////////////////////////////////////////////////////////////////
//      General Init functions


///////////////////////////////////////////////////////////////////////
//      TrialInit -- at start of trial


///////////////////////////////////////////////////////////////////////
//      Cycle Stats

void LeabraLayerSpec::Compute_OutputName_ugp(LeabraLayer* lay, LeabraNetwork* net, int gpidx) {
  LEABRA_UNGP_STATE* gpd = lay->GetUnGpState(net->net_state, gpidx);
  
  String* onm;
  if(lay->unit_groups)
    onm = &(lay->gp_output_names.FastEl_Flat(gpidx));
  else
    onm = &(lay->output_name);

  if(gpd->acts_eq.max_i < 0) {
    *onm = "n/a";
    return;
  }
  UnitState_cpp* u = net->GetUnitState(gpd->acts_eq.max_i); // max_i = full flat idx
  if(!u || u->lesioned()) {
    *onm = "n/a";
    return;
  }
  String un_nm = lay->GetUnitName(u);
  // for target/output layers, if we set something, set network name!
  if(un_nm.empty()) return;
  *onm = un_nm;       // if it is something..

  if(lay->unit_groups) {        // also aggregate the layer name
    if(lay->output_name.nonempty())
      lay->output_name += "_";
    lay->output_name += un_nm;
  }

  if((lay->layer_type != Layer::OUTPUT) && (lay->layer_type != Layer::TARGET)) return;
  if(net->output_name.nonempty())
    net->output_name += "_";
  net->output_name += un_nm;
}

void LeabraLayerSpec::Compute_OutputName(LeabraLayer* lay, LeabraNetwork* net) {
  NetworkState_cpp* net_state = net->net_state;
  LeabraLayerState_cpp* lst = (LeabraLayerState_cpp*)net_state->GetLayerState(lay->layer_idx);
  if(lay->unit_groups) {
    lay->output_name = "";
    for(int gpidx=0; gpidx < lay->n_ungps; gpidx++) {
      Compute_OutputName_ugp(lay, net, gpidx);
    }
  }
  else {
    Compute_OutputName_ugp(lay, net, -1);
  }
}

///////////////////////////////////////////////////////////////////////
//      Trial-level Stats



void LeabraLayerSpec::LayerAvgAct(DataTable* report_table) {
  LeabraNetwork* net = GET_MY_OWNER(LeabraNetwork);
  if(!net) return;
  net->LayerAvgAct(report_table, this);
}



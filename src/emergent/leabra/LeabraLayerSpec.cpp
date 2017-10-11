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

#include <State_main>

#include "LeabraLayerSpec_core.cpp"

//////////////////////////////////////////////////////////


void LeabraLayerSpec::Initialize() {
  min_obj_type = &TA_LeabraLayer;
  Defaults_init();
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
  FOREACH_ELEM_IN_GROUP(Projection, p, lay->projections) {
    LeabraLayer* fmlay = (LeabraLayer*)p->from.ptr();
    if(fmlay->spec.SPtr()->InheritsFrom(layer_spec)) {  // inherits - not excact match!
      prjn_idx = p->recv_idx;
      rval = fmlay;
      break;
    }
  }
  return rval;
}

LeabraLayer* LeabraLayerSpec::FindLayerFmSpecExact(LeabraLayer* lay, int& prjn_idx, TypeDef* layer_spec) {
  LeabraLayer* rval = NULL;
  prjn_idx = -1;
  FOREACH_ELEM_IN_GROUP(Projection, p, lay->projections) {
    LeabraLayer* fmlay = (LeabraLayer*)p->from.ptr();
    if(fmlay->spec.SPtr()->GetTypeDef() == layer_spec) {        // not inherits - excact match!
      prjn_idx = p->recv_idx;
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


void LeabraLayer::CheckInhibCons(LeabraNetwork* net) {
  FOREACH_ELEM_IN_GROUP(LeabraPrjn, p, projections) {
    p->CheckInhibCons(net);
  }
}

void LeabraLayerSpec::DecayState(LeabraLayer* lay, LeabraNetwork* net, float decay_val) {
  FOREACH_ELEM_IN_GROUP(LeabraUnit, u, lay->units) {
    if(u->lesioned()) continue;
    ((LeabraUnitSpec*)u->GetUnitSpec())->DecayState
      ((LeabraUnitState_cpp*)u->GetUnitState(), (LeabraNetworkState_cpp*)net->net_state,
       u->thread_no, decay_val);
  }
}


///////////////////////////////////////////////////////////////////////
//      TrialInit -- at start of trial

void LeabraLayerSpec::Trial_Init_Specs(LeabraLayer* lay, LeabraNetwork* net) {
  if(lay->unit_spec.SPtr()) {
    ((LeabraUnitSpec*)lay->unit_spec.SPtr())->Trial_Init_Specs(net);
  }

  FOREACH_ELEM_IN_GROUP(LeabraPrjn, p, lay->projections) {
    if(p->NotActive()) continue;
    p->Trial_Init_Specs(net);
  }

  if(lay_gp_inhib.on) {
    net->net_misc.lay_gp_inhib = true;
  }
  if(decay.trial > 0.0f) {
    net->net_misc.trial_decay = true;
  }
}


///////////////////////////////////////////////////////////////////////
//      QuarterInit -- at start of settling

float LeabraLayerSpec::Compute_AvgExt(LeabraLayer* lay, LeabraNetwork* net) {
  // todo: could do this in a state / thread compatible way..
  float avg_ext = 0.0f;
  int avg_n = 0;
  FOREACH_ELEM_IN_GROUP(LeabraUnit, u, lay->units) {
    if(u->lesioned()) continue;
    if(lay->HasExtFlag(UnitState_cpp::TARG)) { // targ comes first b/c not copied to ext at this point yet!
      avg_ext += u->targ();
    }
    else if(lay->HasExtFlag(UnitState_cpp::EXT)) {
      avg_ext += u->ext();
    }
    avg_n++;
  }
  if(avg_n > 0) {
    avg_ext /= (float)avg_n;
  }
  return avg_ext;
}


///////////////////////////////////////////////////////////////////////
//      Cycle Stats

void LeabraLayerSpec::Compute_OutputName_ugp
(LeabraLayer* lay, Layer::AccessMode acc_md, int gpidx, LeabraUnGpState_cpp* gpd, LeabraNetwork* net) {
  String* onm;
  if(lay->unit_groups)
    onm = &(lay->gp_output_names.FastEl_Flat(gpidx));
  else
    onm = &(lay->output_name);

  if(gpd->acts_eq.max_i < 0) {
    *onm = "n/a";
    return;
  }
  LeabraUnit* u = (LeabraUnit*)net->UnFmIdx(gpd->acts_eq.max_i); // max_i = flat now
  if(!u || u->lesioned()) {
    *onm = "n/a";
    return;
  }
  // for target/output layers, if we set something, set network name!
  if(u->name.empty()) return;
  *onm = u->name;       // if it is something..

  if(lay->unit_groups) {        // also aggregate the layer name
    if(lay->output_name.nonempty())
      lay->output_name += "_";
    lay->output_name += u->name;
  }

  if((lay->layer_type != Layer::OUTPUT) && (lay->layer_type != Layer::TARGET)) return;
  if(net->output_name.nonempty())
    net->output_name += "_";
  net->output_name += u->name;
}

void LeabraLayerSpec::Compute_OutputName(LeabraLayer* lay, LeabraNetwork* net) {
  NetworkState_cpp* net_state = net->net_state;
  LeabraLayerState_cpp* lst = (LeabraLayerState_cpp*)net_state->GetLayerState(lay->layer_idx);
  if(lay->unit_groups) {
    lay->output_name = "";
    for(int g=0; g < lay->n_ungps; g++) {
      LeabraUnGpState_cpp* gpd = (LeabraUnGpState_cpp*)lst->GetUnGpState(net_state, g);
      Compute_OutputName_ugp(lay, Layer::ACC_GP, g, gpd, net);
    }
  }
  else {
    LeabraUnGpState_cpp* lgpd = (LeabraUnGpState_cpp*)lst->GetLayUnGpState(net_state);
    Compute_OutputName_ugp(lay, Layer::ACC_LAY, 0, lgpd, net);
  }
}

///////////////////////////////////////////////////////////////////////
//      Trial-level Stats



void LeabraLayerSpec::LayerAvgAct(DataTable* report_table) {
  LeabraNetwork* net = GET_MY_OWNER(LeabraNetwork);
  if(!net) return;
  net->LayerAvgAct(report_table, this);
}


// todo: move to state

void LeabraLayerSpec::Compute_AbsRelNetin(LeabraLayer* lay, LeabraNetwork* net) {
  NetworkState_cpp* net_state = net->net_state;
  
  LeabraLayerState_cpp* lst = (LeabraLayerState_cpp*)net_state->GetLayerState(lay->layer_idx);
  LeabraUnGpState_cpp* lgpd = (LeabraUnGpState_cpp*)lst->GetLayUnGpState(net_state);

  if(lgpd->netin.max < 0.01f) return; // not getting enough activation to count!

  // layer is automatic
  lay->avg_netin_sum.avg += lgpd->netin_m.avg;
  lay->avg_netin_sum.max += lgpd->netin_m.max;
  lay->avg_netin_n++;

  // but projection level is not
  if(net->NetinPerPrjn() || net->rel_netin.ComputeNow(net->epoch, net->trial)) {
    float sum_net = 0.0f;
    for(int i=0;i<lay->projections.size;i++) {
      LeabraPrjn* prjn = (LeabraPrjn*)lay->projections[i];
      if(prjn->NotActive()) continue;
      LeabraConSpec* cs = (LeabraConSpec*)prjn->GetConSpec();
      prjn->netin_avg = 0.0f;
      int netin_avg_n = 0;
      FOREACH_ELEM_IN_GROUP(LeabraUnit, u, lay->units) {
        if(u->lesioned()) continue;
        LeabraUnitSpec* us = (LeabraUnitSpec*)u->GetUnitSpec();
        if(u->act_eq() < us->opt_thresh.send) continue; // ignore if not above sending thr
        LeabraConState_cpp* cg = (LeabraConState_cpp*)u->RecvConStatePrjn(prjn);
        if(!cg) continue;
        float netin;
        if(net->NetinPerPrjn()) {
          netin = cg->net_raw;
        }
        else {
          netin = cs->Compute_Netin(cg, net_state, u->thread_no);
          // otherwise have to compute it
        }
        cg->net = netin;
        prjn->netin_avg += netin;
        netin_avg_n++;
      }
      if(netin_avg_n > 0)
        prjn->netin_avg /= (float)netin_avg_n;
      sum_net += prjn->netin_avg;
    }

    for(int i=0;i<lay->projections.size;i++) {
      LeabraPrjn* prjn = (LeabraPrjn*)lay->projections[i];
      if(prjn->NotActive()) continue;
      if(sum_net > 0.0f)
        prjn->netin_rel = prjn->netin_avg / sum_net;
      // increment epoch-level
      prjn->avg_netin_avg_sum += prjn->netin_avg;
      prjn->avg_netin_rel_sum += prjn->netin_rel;
      prjn->avg_netin_n++;
    }
  }
}

void LeabraLayerSpec::Compute_AvgAbsRelNetin(LeabraLayer* lay, LeabraNetwork* net) {
#ifdef DMEM_COMPILE
  lay->DMem_ComputeAggs(net->dmem_trl_comm.comm);
#endif
  if(lay->avg_netin_n > 0) {
    lay->avg_netin.avg = lay->avg_netin_sum.avg / (float)lay->avg_netin_n;
    lay->avg_netin.max = lay->avg_netin_sum.max / (float)lay->avg_netin_n;
  }
  lay->avg_netin_sum.avg = 0.0f;
  lay->avg_netin_sum.max = 0.0f;
  lay->avg_netin_n = 0;
  for(int i=0;i<lay->projections.size;i++) {
    LeabraPrjn* prjn = (LeabraPrjn*)lay->projections[i];
    if(prjn->NotActive()) continue;
#ifdef DMEM_COMPILE
    prjn->DMem_ComputeAggs(net->dmem_trl_comm.comm);
#endif
    if(prjn->avg_netin_n > 0) {
      prjn->avg_netin_avg = prjn->avg_netin_avg_sum / (float)prjn->avg_netin_n;
      prjn->avg_netin_rel = prjn->avg_netin_rel_sum / (float)prjn->avg_netin_n;
    }
    prjn->avg_netin_n = 0;
    prjn->avg_netin_avg_sum = 0.0f;
    prjn->avg_netin_rel_sum = 0.0f;
  }
}

void LeabraLayerSpec::ClearDeepActs(LeabraLayer* lay, LeabraNetwork* net) {
  LeabraNetworkState_cpp* net_state = (LeabraNetworkState_cpp*)net->net_state;
  FOREACH_ELEM_IN_GROUP(LeabraUnit, u, lay->units) {
    if(u->lesioned()) continue;
    ((LeabraUnitSpec*)u->GetUnitSpec())->ClearDeepActs
      ((LeabraUnitState_cpp*)u->GetUnitState(), net_state, u->thread_no);
  }
}


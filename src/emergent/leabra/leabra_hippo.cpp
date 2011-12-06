// Copyright, 1995-2007, Regents of the University of Colorado,
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

#include "leabra_hippo.h"

///////////////////////////////////////////////////////////////////////////////////////
//              Special Hippocampal Quadphase Layerspecs

void HippoQuadLayerSpec::Initialize() {
  auto_m_cycles = 20;
}

void HippoQuadLayerSpec::Defaults_init() {
}

void HippoQuadLayerSpec::RecordActM2(LeabraLayer* lay, LeabraNetwork* net) {
  FOREACH_ELEM_IN_GROUP(LeabraUnit, u, lay->units) {
    u->act_m2 = u->act_nd;      // record the minus phase before overwriting it..
  }

  // record stats for act_m2
  AvgMaxVals& vals = lay->acts_m2;
  static ta_memb_ptr mb_off = 0;
  if(mb_off == 0) {
    TypeDef* td = &TA_LeabraUnit; int net_base_off = 0;
    TypeDef::FindMemberPathStatic(td, net_base_off, mb_off, "act_m2");
  }
  if(lay->unit_groups) {
    vals.InitVals();
    int nunits = lay->UnitAccess_NUnits(Layer::ACC_GP);
    for(int g=0; g < lay->gp_geom.n; g++) {
      LeabraUnGpData* gpd = lay->ungp_data.FastEl(g);
      Compute_AvgMaxVals_ugp(lay, Layer::ACC_GP, g, gpd->acts_m2, mb_off);
      vals.UpdtFmAvgMax(gpd->acts_m2, nunits, g);
    }
    vals.CalcAvg(lay->units.leaves);
  }
  else {
    Compute_AvgMaxVals_ugp(lay, Layer::ACC_LAY, 0, vals, mb_off);
  }
}

void HippoQuadLayerSpec::Compute_AutoEncStats(LeabraLayer* lay, LeabraNetwork* net) {
  LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.SPtr();
  float norm_err = 0.0f;
  float sse_err = 0.0f;
  FOREACH_ELEM_IN_GROUP(LeabraUnit, u, lay->units) {
    u->act_dif2 = u->act_eq - u->act_m2;
    float sse = u->act_dif2;
    if(fabsf(sse) < us->sse_tol)
      sse = 0.0f;
    sse *= sse;
    sse_err += sse;

    if(net->on_errs) {
      if(u->act_m2 > 0.5f && u->act_eq < 0.5f) norm_err += 1.0f;
    }
    if(net->off_errs) {
      if(u->act_m2 < 0.5f && u->act_eq > 0.5f) norm_err += 1.0f;
    }
  }
  int ntot = 0;
  if(net->on_errs && net->off_errs)
    ntot = 2 * lay->kwta.k;
  else
    ntot = lay->kwta.k;
  if(ntot > 0)
    norm_err = norm_err / (float)ntot;
  if(norm_err > 1.0f)
    norm_err = 1.0f;
  lay->SetUserData("enc_sse", sse_err);
  lay->SetUserData("enc_norm_err", norm_err);
}

void ECinLayerSpec::Compute_CycleStats(LeabraLayer* lay, LeabraNetwork* net) {
  if(net->ct_cycle == auto_m_cycles)
    RecordActM2(lay,net);
  inherited::Compute_CycleStats(lay, net);
}


void ECoutLayerSpec::Initialize() {
}

bool ECoutLayerSpec::CheckConfig_Layer(Layer* ly, bool quiet) {
  LeabraLayer* lay = (LeabraLayer*)ly;
  if(!inherited::CheckConfig_Layer(lay, quiet)) return false;

  bool rval = true;

  LeabraNetwork* net = (LeabraNetwork*)lay->own_net;
  if(net) {
    if(TestWarning(net->min_cycles < auto_m_cycles + 20, "CheckConfig",
                   "ECoutLayerSpec: setting network min_cycles to be auto_m_cycles + 20 to ensure minimum amount of time to settle")) {
      net->min_cycles = auto_m_cycles + 20;
    }
    if(TestWarning((net->learn_rule >= LeabraNetwork::CTLEABRA_CAL) &&
                   (net->ct_time.minus < auto_m_cycles + 20), "CheckConfig",
                   "ECoutLayerSpec: setting network ct_time.minus to be auto_m_cycles + 20 to ensure minimum amount of time to settle")) {
      net->ct_time.minus = auto_m_cycles + 20;
    }
  }

  int in_prjn_idx;
  LeabraLayer* in_lay = FindLayerFmSpec(lay, in_prjn_idx, &TA_ECinLayerSpec);
  if(lay->CheckError(!in_lay, quiet, rval,
                "no projection from ECin Layer found: must recv from layer with ECinLayerSpec!")) {
    return false;
  }
  if(in_lay->unit_groups) {
    if(lay->CheckError(in_lay->gp_geom.n != lay->gp_geom.n, quiet, rval,
                       "ECout Layer unit groups must = ECinLayer unit groups, copiped from IN Layer; Please do a Build of network")) {
      lay->unit_groups = true;
      lay->gp_geom.n = in_lay->gp_geom.n;
    }
  }
  if(lay->CheckError(in_lay->un_geom.n != lay->un_geom.n, quiet, rval,
                "ECout Layer units must = ECinLayer units, copied from IN Layer; Please do a Build of network")) {
    lay->un_geom = in_lay->un_geom;
  }

  return true;
}

void ECoutLayerSpec::ClampFromECin(LeabraLayer* lay, LeabraNetwork* net) {
  int in_prjn_idx;
  LeabraLayer* in_lay = FindLayerFmSpec(lay, in_prjn_idx, &TA_ECinLayerSpec);
  if(!in_lay) return;
  LeabraUnitSpec* rus = (LeabraUnitSpec*)lay->GetUnitSpec();
  int nunits = MIN(in_lay->units.leaves, lay->units.leaves);
  for(int i=0;i<nunits;i++) {
    LeabraUnit* ru = (LeabraUnit*)lay->units.Leaf(i);
    LeabraUnit* inu = (LeabraUnit*)in_lay->units.Leaf(i);
    float inval = inu->act_eq;
    if(clamp.max_plus) {
      float min_max = MAX(lay->acts_m.max, lay->acts_m2.max); // consider auto enc max too -- esp if doing pretraining on encoder only, this is important
      float clmp = min_max + clamp.plus;
      clmp = MAX(clmp, clamp.min_clamp);
      inval *= clmp;            // downscale
    }
    ru->act = rus->clamp_range.Clip(inval);
    ru->act_eq = ru->act_nd = ru->act;
    ru->da = 0.0f;              // I'm fully settled!
    ru->AddToActBuf(rus->syn_delay);
  }
}

void ECoutLayerSpec::Compute_CycleStats(LeabraLayer* lay, LeabraNetwork* net) {
  if(net->ct_cycle == auto_m_cycles)
    RecordActM2(lay,net);
  if(net->phase == LeabraNetwork::PLUS_PHASE) {
    ClampFromECin(lay, net);
    if(net->cycle <= 1)
      Compute_AutoEncStats(lay, net);
  }
  inherited::Compute_CycleStats(lay, net);
}

/////////////////////////////////////////////
//              CA1

void CA1LayerSpec::Initialize() {
  recall_decay = 1.0f;
  use_test_mode = true;
}

bool CA1LayerSpec::CheckConfig_Layer(Layer* ly, bool quiet) {
  LeabraLayer* lay = (LeabraLayer*)ly;
  if(!inherited::CheckConfig_Layer(lay, quiet)) return false;

  bool rval = true;

  int in_prjn_idx;
  LeabraLayer* in_lay = FindLayerFmSpec(lay, in_prjn_idx, &TA_CA3LayerSpec);
  if(lay->CheckError(!in_lay, quiet, rval,
                "no projection from CA3 Layer found: must recv from layer with CA3LayerSpec!")) {
    return false;
  }
  in_lay = FindLayerFmSpec(lay, in_prjn_idx, &TA_ECinLayerSpec);
  if(lay->CheckError(!in_lay, quiet, rval,
                "no projection from EC_in Layer found: must recv from layer with ECinLayerSpec!")) {
    return false;
  }

  return true;
}

void CA1LayerSpec::ModulateCA3Prjn(LeabraLayer* lay, LeabraNetwork* net, bool ca3_on) {
  int ca3_prjn_idx;
  LeabraLayer* ca3_lay = FindLayerFmSpec(lay, ca3_prjn_idx, &TA_CA3LayerSpec);
  if(!ca3_lay) return;
  LeabraUnitSpec* rus = (LeabraUnitSpec*)lay->GetUnitSpec();

  FOREACH_ELEM_IN_GROUP(LeabraUnit, u, lay->units) {
    if(ca3_on) {
      u->Compute_NetinScale(net,0);
    }
    else {
      LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.SafeEl(ca3_prjn_idx);
      if(!recv_gp) continue;
      if(!ca3_on)
        recv_gp->scale_eff = 0.0f;
    }
  }
}

void CA1LayerSpec::ModulateECinPrjn(LeabraLayer* lay, LeabraNetwork* net, bool ecin_on) {
  int ecin_prjn_idx;
  LeabraLayer* ecin_lay = FindLayerFmSpec(lay, ecin_prjn_idx, &TA_ECinLayerSpec);
  if(!ecin_lay) return;
  LeabraUnitSpec* rus = (LeabraUnitSpec*)lay->GetUnitSpec();

  FOREACH_ELEM_IN_GROUP(LeabraUnit, u, lay->units) {
    if(ecin_on) {
      u->Compute_NetinScale(net,0);
    }
    else {
      LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.SafeEl(ecin_prjn_idx);
      if(!recv_gp) continue;
      if(!ecin_on)
        recv_gp->scale_eff = 0.0f;
    }
  }
}

void CA1LayerSpec::FinalizePrjnMods(LeabraLayer* lay, LeabraNetwork* net) {
  net->Compute_NetinScale_Senders(); // update senders!
  net->DecayState(0.0f);               // need to re-send activations -- this triggers resend
}

void CA1LayerSpec::Settle_Init_Layer(LeabraLayer* lay, LeabraNetwork* net) {
  // always off at start of minus and plus phase
  ModulateCA3Prjn(lay, net, false); // turn off ca3 in minus phase until further notice
  if(net->phase == LeabraNetwork::PLUS_PHASE)
    lay->DecayState(net, recall_decay); // decay at start of plus phase too
  inherited::Settle_Init_Layer(lay, net);
}

void CA1LayerSpec::Compute_CycleStats(LeabraLayer* lay, LeabraNetwork* net) {
  if(net->ct_cycle == auto_m_cycles)
    RecordActM2(lay,net);
  if(net->ct_cycle == auto_m_cycles+1) {
    if(!(use_test_mode && net->train_mode == Network::TEST))
      lay->DecayState(net, recall_decay); // specifically CA1 activations at recall
    ModulateCA3Prjn(lay, net, true);    // turn on ca3 -- calls netinscale
    if(!(use_test_mode && net->train_mode == Network::TEST))
      ModulateECinPrjn(lay, net, false); // turn off ecin -- must be after ca3 to specifically turn off
    FinalizePrjnMods(lay, net);        // make 'em stick
  }
  inherited::Compute_CycleStats(lay, net);
}

///////////////////////////////////////////////////////////////
//   SubiculumLayerSpec

void SubiculumNoveltySpec::Initialize() {
  max_norm_err = 0.2f;
  min_lrate = 0.01f;

  nov_rescale = 1.0f / max_norm_err;
  lrate_factor = (1.0f - min_lrate);
}

void SubiculumNoveltySpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  nov_rescale = 1.0f / max_norm_err;
  lrate_factor = (1.0f - min_lrate);
}

void SubiculumLayerSpec::Initialize() {
  lrate_mod_con_spec.SetBaseType(&TA_LeabraConSpec);
}

bool SubiculumLayerSpec::CheckConfig_Layer(Layer* ly, bool quiet) {
  LeabraLayer* lay = (LeabraLayer*)ly;
  if(!inherited::CheckConfig_Layer(lay, quiet)) return false;

  bool rval = true;

  if(lay->CheckError(lay->projections.size < 2, quiet, rval,
        "must receive from at least 2 other layers -- one pair of EC_in / EC_out layers!")) {
    return false;
  }
  if(lay->CheckError(lay->projections.size % 2 != 0, quiet, rval,
        "must receive from an even number of other layers -- sequential matched pairs of EC_in / EC_out layers!")) {
    return false;
  }

  for(int pi=0; pi<lay->projections.size; pi+=2) {
    Projection* pin = lay->projections.FastEl(pi);
    Projection* pout = lay->projections.FastEl(pi+1);
    LeabraLayer* lin = (LeabraLayer*)pin->from.ptr();
    LeabraLayer* lout = (LeabraLayer*)pout->from.ptr();
    LeabraLayerSpec* lsin = (LeabraLayerSpec*)lin->GetLayerSpec();
    LeabraLayerSpec* lsout = (LeabraLayerSpec*)lout->GetLayerSpec();
    if(lay->CheckError(!lsin->InheritsFrom(&TA_ECinLayerSpec), quiet, rval,
                       "projection number:", String(pi), "should have been ECin, instead was:",
                       lsin->name, "type:", lsin->GetTypeDef()->name)) {
      return false;
    }
    if(lay->CheckError(!lsout->InheritsFrom(&TA_ECoutLayerSpec), quiet, rval,
               "projection number:", String(pi+1), "should have been ECout, instead was:",
               lsout->name, "type:", lsout->GetTypeDef()->name)) {
      return false;
    }
    bool geom_eq = (lin->un_geom == lout->un_geom) && (lin->gp_geom == lout->gp_geom);
    if(lay->CheckError(!geom_eq, quiet, rval,
               "projection numbers starting at:", String(pi), "do not have the same geometry, ECin layer:",
                       lin->name, "ECout layer:", lout->name)) {
      return false;
    }
  }

  LeabraConSpec* cs = (LeabraConSpec*)lrate_mod_con_spec.SPtr();
  if(lay->CheckError(!cs, quiet, rval,
                     "lrate_mod_con_spec is NULL")) {
      return false;
  }
  if(cs->lrate_sched.size > 0) {
    TestWarning(true, "CheckConfig", "configuring lrate_sched on lrate_mod_con_spec to be empty:"
                , cs->name);
    cs->lrate_sched.SetSize(0);
    cs->UpdateAfterEdit();      // update subs
  }


  return true;
}

float SubiculumLayerSpec::Compute_ECNormErr_ugp(LeabraLayer* lin, LeabraLayer* lout,
                                           Layer::AccessMode acc_md, int gpidx,
                                           LeabraNetwork* net) {
  int nunits = lin->UnitAccess_NUnits(acc_md);
  float nerr = 0.0f;
  for(int i=0; i<nunits; i++) {
    LeabraUnit* uin = (LeabraUnit*)lin->UnitAccess(acc_md, i, gpidx);
    LeabraUnit* uout = (LeabraUnit*)lout->UnitAccess(acc_md, i, gpidx);
    float outval = uout->act_eq;
    if(net->phase == LeabraNetwork::PLUS_PHASE) outval = uout->act_m; // use minus
    if(net->on_errs) { if(outval > 0.5f && uin->act_eq < 0.5f) nerr += 1.0f; }
    if(net->off_errs) { if(outval < 0.5f && uin->act_eq > 0.5f) nerr += 1.0f; }
  }
  return nerr;
}

void SubiculumLayerSpec::Compute_ECNovelty(LeabraLayer* lay, LeabraNetwork* net) {
  float nerr = 0.0f;
  int ntot = 0;
  for(int pi=0; pi<lay->projections.size; pi+=2) {
    Projection* pin = lay->projections.FastEl(pi);
    Projection* pout = lay->projections.FastEl(pi+1);
    LeabraLayer* lin = (LeabraLayer*)pin->from.ptr();
    LeabraLayer* lout = (LeabraLayer*)pout->from.ptr();
    LeabraLayerSpec* lsin = (LeabraLayerSpec*)lin->GetLayerSpec();
    LeabraLayerSpec* lsout = (LeabraLayerSpec*)lout->GetLayerSpec();

    if((lsin->inhib_group != ENTIRE_LAYER) && lin->unit_groups) {
      for(int g=0; g < lay->gp_geom.n; g++) {
        LeabraUnGpData* gpdin = lin->ungp_data.FastEl(g);
        nerr += Compute_ECNormErr_ugp(lin, lout, Layer::ACC_GP, g, net);
        if(net->on_errs && net->off_errs)
          ntot += 2 * gpdin->kwta.k;
        else
          ntot += gpdin->kwta.k;
      }
    }
    else {
      nerr += Compute_ECNormErr_ugp(lin, lout, Layer::ACC_LAY, 0, net);
      if(net->on_errs && net->off_errs)
        ntot += 2 * lin->kwta.k;
      else
        ntot += lin->kwta.k;
    }
  }

  // store norm_err on us too..
  lay->norm_err = nerr / (float)ntot;
  if(lay->norm_err > 1.0f) lay->norm_err = 1.0f;

  float nov = novelty.ComputeNovelty(lay->norm_err);
  float lrate = novelty.ComputeLrate(nov);
  lay->SetUserData("novelty", nov);
  lay->SetUserData("lrate", lrate);

  LeabraConSpec* cs = (LeabraConSpec*)lrate_mod_con_spec.SPtr();
  if(cs) {
    cs->lrate_sched.default_val = lrate;
    cs->UpdateAfterEdit_NoGui();        // get into cur_lrate with lrate schedule etc
  }

  // clamp novelty value on our layer
  UNIT_GP_ITR
    (lay,
     LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, 0, gpidx);
     u->ext = nov;
     ClampValue_ugp(lay, acc_md, gpidx, net);
     Compute_ExtToAct_ugp(lay, acc_md, gpidx, net);
     );
}


void SubiculumLayerSpec::Compute_CycleStats(LeabraLayer* lay, LeabraNetwork* net) {
  Compute_ECNovelty(lay, net);
  inherited::Compute_CycleStats(lay, net);
}

///////////////////////////////////////////////////////////////
//   HippoEncoderConSpec

void HippoEncoderConSpec::Initialize() {
  SetUnique("lmix", true);
  lmix.hebb = 0.0f;
  lmix.err = 1.0f;
//   lmix.err_sb = false;

  SetUnique("wt_limits", true);
  wt_limits.sym = false;

  SetUnique("wt_sig", true);
  wt_sig.gain = 1.0f;  wt_sig.off = 1.0f;

//   SetUnique("xcalm", true);
//   xcalm.use_sb = false;
}

void HippoEncoderConSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  // these are enforced absolutely because the code does not use them:
  lmix.hebb = 0.0f;
  lmix.err = 1.0f;
}

void XCalCHLConSpec::Initialize() {
  use_chl = true;
}

///////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////
//              Wizard          //
//////////////////////////////////

bool LeabraWizard::Hippo(LeabraNetwork* net) {
  if(!net) {
    LeabraProject* proj = GET_MY_OWNER(LeabraProject);
    net = (LeabraNetwork*)proj->GetNewNetwork();
    if(TestError(!net, "Hippo", "network is NULL and could not make a new one -- aborting!"))
      return false;
    if(!StdNetwork()) return false;
  }

  String msg = "Configuring Quad Phase Hippocampus:\n\n\
 You will have to configure inputs/outputs to/from the EC layers after the configuration:\n\n";

  taMisc::Confirm(msg);

  net->RemoveUnits();

  //////////////////////////////////////////////////////////////////////////////////
  // make layers

  bool new_lay;
  Layer_Group* hip_laygp = net->FindMakeLayerGroup("Hippocampus");
  LeabraLayer* ecin = (LeabraLayer*)hip_laygp->FindMakeLayer("EC_in", NULL, new_lay);
  LeabraLayer* ecout = (LeabraLayer*)hip_laygp->FindMakeLayer("EC_out", NULL, new_lay);
  LeabraLayer* dg = (LeabraLayer*)hip_laygp->FindMakeLayer("DG", NULL, new_lay);
  LeabraLayer* ca3 = (LeabraLayer*)hip_laygp->FindMakeLayer("CA3", NULL, new_lay);
  LeabraLayer* ca1 = (LeabraLayer*)hip_laygp->FindMakeLayer("CA1", NULL, new_lay);
  LeabraLayer* subic = (LeabraLayer*)hip_laygp->FindMakeLayer("Subiculum", NULL, new_lay);

  //////////////////////////////////////////////////////////////////////////////////
  // make specs

  BaseSpec_Group* hipspec = net->FindMakeSpecGp("HippoSpecs");

  // unit specs
  LeabraUnitSpec* hip_units = (LeabraUnitSpec*)hipspec->FindMakeSpec("HippoUnitSpec", &TA_LeabraUnitSpec);
//   LeabraUnitSpec* dg_units = (LeabraUnitSpec*)hip_units->FindMakeChild("DGUnits", &TA_LeabraUnitSpec);
//   LeabraUnitSpec* ecout_units = (LeabraUnitSpec*)units->FindMakeSpec("ECOutUnits", &TA_LeabraUnitSpec);

  HippoEncoderConSpec* ecca1_cons = (HippoEncoderConSpec*)hipspec->FindMakeSpec("EC_CA1ConSpecs", &TA_HippoEncoderConSpec);
  HippoEncoderConSpec* ecin_ca1_cons = (HippoEncoderConSpec*)ecca1_cons->FindMakeChild("EC_in_CA1", &TA_HippoEncoderConSpec);
  HippoEncoderConSpec* ca1_ecout_cons = (HippoEncoderConSpec*)ecca1_cons->FindMakeChild("CA1_EC_out", &TA_HippoEncoderConSpec);
  HippoEncoderConSpec* ecout_ca1_cons = (HippoEncoderConSpec*)ecca1_cons->FindMakeChild("EC_out_CA1", &TA_HippoEncoderConSpec);
  HippoEncoderConSpec* ecin_ecout_cons = (HippoEncoderConSpec*)ecca1_cons->FindMakeChild("EC_in_EC_out", &TA_HippoEncoderConSpec);
  HippoEncoderConSpec* ecout_ecin_cons = (HippoEncoderConSpec*)ecca1_cons->FindMakeChild("EC_out_EC_in", &TA_HippoEncoderConSpec);
  HippoEncoderConSpec* in_ecin_cons = (HippoEncoderConSpec*)ecca1_cons->FindMakeChild("Input_EC_in", &TA_HippoEncoderConSpec);
  HippoEncoderConSpec* ecout_out_cons = (HippoEncoderConSpec*)ecca1_cons->FindMakeChild("EC_out_Output", &TA_HippoEncoderConSpec);

  // connection specs
  XCalCHLConSpec* hip_cons = (XCalCHLConSpec*)hipspec->FindMakeSpec("HippoConSpecs", &TA_XCalCHLConSpec);
  LeabraBiasSpec* hip_bias = (LeabraBiasSpec*)hip_cons->FindMakeChild("HippoBiasSpec", &TA_LeabraBiasSpec);
  XCalCHLConSpec* ppath_cons = (XCalCHLConSpec*)hip_cons->FindMakeChild("PerfPath", &TA_XCalCHLConSpec);
  XCalCHLConSpec* mossy_cons = (XCalCHLConSpec*)hip_cons->FindMakeChild("Mossy", &TA_XCalCHLConSpec);
  XCalCHLConSpec* ca3ca3_cons = (XCalCHLConSpec*)hip_cons->FindMakeChild("CA3_CA3", &TA_XCalCHLConSpec);
  XCalCHLConSpec* ca3ca1_cons = (XCalCHLConSpec*)hip_cons->FindMakeChild("CA3_CA1", &TA_XCalCHLConSpec);

  // layer specs
  HippoQuadLayerSpec* hip_laysp = (HippoQuadLayerSpec*)hipspec->FindMakeSpec("HippoLayerSpec", &TA_HippoQuadLayerSpec);
  ECoutLayerSpec* ecout_laysp = (ECoutLayerSpec*)hip_laysp->FindMakeChild("EC_out", &TA_ECoutLayerSpec);
  ECinLayerSpec* ecin_laysp = (ECinLayerSpec*)ecout_laysp->FindMakeChild("EC_in", &TA_ECinLayerSpec);
  LeabraLayerSpec* dg_laysp = (LeabraLayerSpec*)hip_laysp->FindMakeChild("DG", &TA_LeabraLayerSpec);
  CA3LayerSpec* ca3_laysp = (CA3LayerSpec*)hip_laysp->FindMakeChild("CA3", &TA_CA3LayerSpec);
  CA1LayerSpec* ca1_laysp = (CA1LayerSpec*)hip_laysp->FindMakeChild("CA1", &TA_CA1LayerSpec);
  SubiculumLayerSpec* subic_laysp = (SubiculumLayerSpec*)hipspec->FindMakeSpec("Subiculum", &TA_SubiculumLayerSpec);

  // prjn specs
  BaseSpec_Group* prjns = (BaseSpec_Group*)hipspec->FindMakeGpName("HippoPrjns");

  ProjectionSpec* fullprjn = (ProjectionSpec*)prjns->FindMakeSpec("FullPrjn", &TA_FullPrjnSpec);
  ProjectionSpec* onetoone = (ProjectionSpec*)prjns->FindMakeSpec("OneToOne", &TA_OneToOnePrjnSpec);
  ProjectionSpec* gponetoone = (ProjectionSpec*)prjns->FindMakeSpec("GpOneToOne", &TA_GpOneToOnePrjnSpec);
  UniformRndPrjnSpec* ppath_prjn = (UniformRndPrjnSpec*)prjns->FindMakeSpec("RandomPerfPath", &TA_UniformRndPrjnSpec);
  UniformRndPrjnSpec* mossy_prjn = (UniformRndPrjnSpec*)prjns->FindMakeSpec("UniformRndMossy", &TA_UniformRndPrjnSpec);


  //////////////////////////////////////////////////////////////////////////////////
  // apply specs to objects

  // set bias specs for unit specs
  hip_units->bias_spec.SetSpec(hip_bias);

  ecin->SetLayerSpec(ecin_laysp);
  ecout->SetLayerSpec(ecout_laysp);
  dg->SetLayerSpec(dg_laysp);
  ca3->SetLayerSpec(ca3_laysp);
  ca1->SetLayerSpec(ca1_laysp);
  subic->SetLayerSpec(subic_laysp);

  ecin->SetUnitSpec(hip_units);
  ecout->SetUnitSpec(hip_units);
  dg->SetUnitSpec(hip_units);
  ca3->SetUnitSpec(hip_units);
  ca1->SetUnitSpec(hip_units);
  subic->SetUnitSpec(hip_units);

  //////////////////////////////////////////////////////////////////////////////////
  // make projections

  // FindMakePrjn(Layer* recv, Layer* send,

//   net->FindMakePrjn(snc, pvi, onetoone, marker_cons);
//   net->FindMakePrjn(snc, lve, onetoone, marker_cons);
//   net->FindMakePrjn(snc, lvi, onetoone, marker_cons);
//   net->FindMakePrjn(snc, pvr, onetoone, marker_cons);
//   net->FindMakePrjn(snc, nv,  onetoone, marker_cons);
//   net->FindMakePrjn(snc, patch,  onetoone, marker_cons);

//   // patch has same basic connectivity as lve
//   net->FindMakePrjn(patch, pvr, onetoone, marker_cons);

//   if(patch_new) {
//     for(i=0;i<input_lays.size;i++) {
//       Layer* il = (Layer*)input_lays[i];
//       net->FindMakePrjn(patch, il, fullprjn, lve_cons);
//     }
//   }

//   net->RemovePrjn(matrix_m, vta); // no more vta prjn!
//   net->FindMakePrjn(matrix_m, snc, gponetoone, marker_cons);

//   net->FindMakePrjn(snrthal_m, matrix_m, gponetoone, marker_cons);
//   net->FindMakePrjn(pfc_m, snrthal_m, gponetoone, marker_cons);
//   net->FindMakePrjn(matrix_m, snrthal_m, gponetoone, marker_cons);
//   net->FindMakePrjn(matrix_m, patch, gponetoone, marker_cons); // for noise

//   if(out_gate) {
//     net->RemovePrjn(matrix_o, snc); // transiently made that so nuke it if there
//     net->FindMakePrjn(matrix_o, vta, fullprjn, marker_cons);
//     // output gets from vta, not snc

//     net->FindMakePrjn(snrthal_o, matrix_o, gponetoone, marker_cons);
//     net->FindMakePrjn(pfc_o, snrthal_o, gponetoone, marker_cons);
//     net->FindMakePrjn(matrix_o, snrthal_o, gponetoone, marker_cons);

//     net->FindMakePrjn(pfc_o, pfc_m, onetoone, marker_cons);

// //     net->FindMakePrjn(matrix_m, pfc_m, gponetoone, mfmpfc_cons);
// //     net->FindMakePrjn(matrix_o, pfc_m, gponetoone, mofmpfc_cons);

//     net->FindMakeSelfPrjn(pfc_m, pfc_selfps, pfc_self);
//     //  net->FindMakeSelfPrjn(pfc_m, intra_pfcps, intra_pfc);

//     // this part in particular doesn't make sense for pfc_o only..
//     // critics need up reflect updating!
//     net->FindMakePrjn(pvr, pfc_m, fullprjn, pvr_cons);
//     net->FindMakePrjn(pvi, pfc_m, fullprjn, pvi_cons);
//     net->FindMakePrjn(lve, pfc_m, pfc_lv_prjn, lve_cons);
//     net->FindMakePrjn(lvi, pfc_m, pfc_lv_prjn, lvi_cons);
//     net->FindMakePrjn(nv,  pfc_m, fullprjn, nv_cons);

//     net->FindMakePrjn(patch, pfc_m, gponetoone, lve_cons);
//   }
//   else {                        // !out_gate
// //     net->FindMakePrjn(matrix_m, pfc_m, gponetoone, mfmpfc_cons);

//     net->FindMakeSelfPrjn(pfc_m, pfc_selfps, pfc_self);
//     //  net->FindMakeSelfPrjn(pfc, intra_pfcps, intra_pfc);

//     net->FindMakePrjn(pvr, pfc_m, fullprjn, pvr_cons);
//     net->FindMakePrjn(pvi, pfc_m, fullprjn, pvi_cons);
//     net->FindMakePrjn(lve, pfc_m, pfc_lv_prjn, lve_cons);
//     net->FindMakePrjn(lvi, pfc_m, pfc_lv_prjn, lvi_cons);
//     net->FindMakePrjn(nv,  pfc_m, fullprjn, nv_cons);

//     net->FindMakePrjn(patch, pfc_m, gponetoone, lve_cons);
//   }

//   for(i=0;i<input_lays.size;i++) {
//     Layer* il = (Layer*)input_lays[i];
//     if(pfc_m_new) {
//       if(pfc_learns)
//         net->FindMakePrjn(pfc_m, il, fullprjn, topfc_cons);
//       else
//         net->FindMakePrjn(pfc_m, il, input_pfc, topfc_cons);
//     }
//     if(matrix_m_new)
//       net->FindMakePrjn(matrix_m, il, fullprjn, matrix_cons);
//     if(matrix_o_new)
//       net->FindMakePrjn(matrix_o, il, fullprjn, matrixo_cons);
//   }
//   for(i=0;i<hidden_lays.size;i++) {
//     Layer* hl = (Layer*)hidden_lays[i];
//     if(out_gate) {
//       net->FindMakePrjn(hl, pfc_o, fullprjn, fmpfcout_cons);
//       net->FindMakePrjn(hl, pfc_m, fullprjn, fmpfcmnt_cons);
//     }
//     else {
//       net->FindMakePrjn(hl, pfc_m, fullprjn, learn_cons);
//     }
//   }
//   if(pfc_m_new && pfc_learns) {
//     for(i=0;i<output_lays.size;i++) {
//       Layer* ol = (Layer*)output_lays[i];
//       net->FindMakePrjn(pfc_m, ol, fullprjn, topfc_cons);
//     }
//   }

//   //////////////////////////////////////////////////////////////////////////////////
//   // set positions & geometries

//   int n_lv_u;           // number of pvlv-type units
//   if(lvesp->scalar.rep == ScalarValSpec::LOCALIST)
//     n_lv_u = 4;
//   else if(lvesp->scalar.rep == ScalarValSpec::GAUSSIAN)
//     n_lv_u = 12;
//   else
//     n_lv_u = 21;

//   lay_set_geom(lve, 1); // patch has the per-stripe lve guys basically
//   lay_set_geom(lvi, 1);
//   lvesp->SetUnique("inhib_group", true);
//   lvesp->SetUnique("gp_kwta", true);
//   lvisp->SetUnique("inhib_group", false);
//   lvisp->SetUnique("gp_kwta", false);
//   lvesp->inhib_group = LeabraLayerSpec::UNIT_GROUPS;
//   lvesp->gp_kwta.k_from = KWTASpec::USE_K;
//   lvesp->gp_kwta.k = 1;

//   if(patch_new) {
//     patch->pos.SetXYZ(vta->pos.x+3, 0, 0);
//   }
//   if(snc_new) {
//     snc->pos.SetXYZ(vta->pos.x+3 + n_lv_u, 4, 0);
//   }

//   if(patch->un_geom.n != n_lv_u) { patch->un_geom.n = n_lv_u; patch->un_geom.x = n_lv_u; patch->un_geom.y = 1; }

//   lay_set_geom(patch, n_stripes);
//   lay_set_geom(snc, n_stripes, 1); // one unit

//   if(pfc_m_new) {
//     pfc_m->pos.SetXYZ(mx_z2 + 1, 0, 2);
//     if(!pfc_learns && (input_lays.size > 0)) {
//       Layer* il = (Layer*)input_lays[0];
//       pfc_m->un_geom = il->un_geom;
//     }
//     else {
//       pfc_m->un_geom.n = 30; pfc_m->un_geom.x = 5; pfc_m->un_geom.y = 6;
//     }
//   }
//   lay_set_geom(pfc_m, n_stripes);

//   if(matrix_m_new) {
//     matrix_m->pos.SetXYZ(mx_z1+1, 0, 1);
//     matrix_m->un_geom.n = 28; matrix_m->un_geom.x = 4; matrix_m->un_geom.y = 7;
//   }
//   lay_set_geom(matrix_m, n_stripes);

//   if(snrthal_m_new) {
//     snrthal_m->pos.SetXYZ(patch->pos.x + (patch->un_geom.x +1) * patch->gp_geom.x +1, 0, 0);
//   }
//   lay_set_geom(snrthal_m, n_stripes, 1);

//   // this is here, to allow it to get disp_geom for laying out the pfc and matrix guys!
//   Hippo_SetNStripes(net, n_stripes);

//   if(out_gate) {
//     if(pfc_o_new) {
//       pfc_o->pos.z = pfc_m->pos.z; pfc_o->pos.y = pfc_m->pos.y;
//       pfc_o->pos.x = pfc_m->pos.x + pfc_m->disp_geom.x + 2;
//       if(!pfc_learns && (input_lays.size > 0)) {
//         Layer* il = (Layer*)input_lays[0];
//         pfc_o->un_geom = il->un_geom;
//       }
//       else {
//         pfc_o->un_geom = pfc_m->un_geom;
//       }
//     }
//     lay_set_geom(pfc_o, n_stripes);

//     if(matrix_o_new) {
//       matrix_o->pos.z = matrix_m->pos.z; matrix_o->pos.y = matrix_m->pos.y;
//       matrix_o->pos.x = matrix_m->pos.x + matrix_m->disp_geom.x + 2;
//       matrix_o->un_geom.n = 28; matrix_o->un_geom.x = 4; matrix_o->un_geom.y = 7;
//     }
//     lay_set_geom(matrix_o, n_stripes);

//     snrthal_o->un_geom.n = 1;
//     if(snrthal_o_new) {
//       snrthal_o->pos.SetXYZ(snrthal_m->pos.x + (snrthal_m->gp_geom.x*2)+1, 0, 0);
//     }
//     lay_set_geom(snrthal_o, n_stripes);
//   }

//   if(new_bg_laygp) {
//     bg_laygp->pos.z = 0;
//   }
//   if(new_pfc_laygp) {
//     pfc_laygp->pos.z = 2;
//   }

//   //////////////////////////////////////////////////////////////////////////////////
//   // build and check

//   Hippo_SetNStripes(net, n_stripes);
//   Hippo_Defaults(net, pfc_learns); // sets all default params and gets selectedits

//   net->LayerPos_Cleanup();

//   // move back!
//   if(new_bg_laygp || new_pfc_laygp) {
//     bg_laygp->pos.z = 0;
//     pfc_laygp->pos.z = 2;
//     net->RebuildAllViews();     // trigger update
//   }

//   taMisc::CheckConfigStart(false, false);

//   bool ok = patchsp->CheckConfig_Layer(patch, false);
//   ok &= sncsp->CheckConfig_Layer(snc, false);
//   ok &= pfcmsp->CheckConfig_Layer(pfc_m, false);
//   ok &= matrixsp->CheckConfig_Layer(matrix_m, false);
//   ok &= snrthalsp->CheckConfig_Layer(snrthal_m, false);
//   if(out_gate) {
//     ok &= pfcosp->CheckConfig_Layer(pfc_o, false);
//     ok &= matrixosp->CheckConfig_Layer(matrix_o, false);
//     ok &= snrthalosp->CheckConfig_Layer(snrthal_o, false);
//   }

//   taMisc::CheckConfigEnd(ok);

//   if(!ok) {
//     msg =
//       "BG/PFC: An error in the configuration has occurred (it should be the last message\
//  you received prior to this one).  The network will not run until this is fixed.\
//  In addition, the configuration process may not be complete, so you should run this\
//  function again after you have corrected the source of the error.";
//   }
//   else {
//     msg =
//     "BG/PFC configuration is now complete.  Do not forget the one remaining thing\
//  you need to do manually:\n\n" + man_msg;
//   }
//   taMisc::Confirm(msg);

//   for(int j=0;j<net->specs.leaves;j++) {
//     BaseSpec* sp = (BaseSpec*)net->specs.Leaf(j);
//     sp->UpdateAfterEdit();
//   }

//   LeabraProject* proj = GET_MY_OWNER(LeabraProject);
//   if(proj) {
//     proj->undo_mgr.SaveUndo(net, "Wizard::Hippo -- actually saves network specifically");
//   }
  return true;
}


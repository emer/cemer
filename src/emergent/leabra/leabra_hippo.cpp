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
  auto_m_cycles = 30;
  Defaults_init();
}

void HippoQuadLayerSpec::Defaults_init() {
  inhib.type = LeabraInhibSpec::KWTA_AVG_INHIB;
  inhib.kwta_pt = 0.7f;
}

void HippoQuadLayerSpec::RecordActM2(LeabraLayer* lay, LeabraNetwork* net) {
  FOREACH_ELEM_IN_GROUP(LeabraUnit, u, lay->units) {
    if(u->lesioned()) continue;
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
    if(u->lesioned()) continue;
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
  Defaults_init();
}

void ECoutLayerSpec::Defaults_init() {
  SetUnique("inhib_group", true);
  inhib_group = UNIT_GROUPS;
  SetUnique("inhib", true);
  inhib.type = LeabraInhibSpec::KWTA_INHIB;
  inhib.kwta_pt = 0.25f;
  SetUnique("clamp", true);
  clamp.max_plus = true;
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
  if(!in_lay) return true;		// just hope it works out
  // if(lay->CheckError(!in_lay, quiet, rval,
  //               "no projection from ECin Layer found: must recv from layer with ECinLayerSpec!")) {
  //   return false;
  // }
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
    if(u->lesioned()) continue;
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
    if(u->lesioned()) continue;
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
  Defaults_init();
}

void SubiculumLayerSpec::Defaults_init() {
  scalar.rep = ScalarValSpec::GAUSSIAN;
  unit_range.min = -0.5f;
  unit_range.max = 1.5f;
  inhib.kwta_pt = 0.5f;
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
    if(uin->lesioned()) continue;
    LeabraUnit* uout = (LeabraUnit*)lout->UnitAccess(acc_md, i, gpidx);
    if(uout->lesioned()) continue;
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

  // clamp novelty value on our layer
  UNIT_GP_ITR
    (lay,
     LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, 0, gpidx);
     u->ext = nov;
     ClampValue_ugp(lay, acc_md, gpidx, net);
     Compute_ExtToAct_ugp(lay, acc_md, gpidx, net);
     );
}

void SubiculumLayerSpec::Compute_SetLrate(LeabraLayer* lay, LeabraNetwork* net) {
  Compute_ECNovelty(lay, net);
  float nov = novelty.ComputeNovelty(lay->norm_err);
  float lrate = novelty.ComputeLrate(nov);
  LeabraConSpec* cs = (LeabraConSpec*)lrate_mod_con_spec.SPtr();
  if(cs) {
    cs->lrate_sched.default_val = lrate;
    cs->UpdateAfterEdit_NoGui();        // propagate to children
    cs->SetCurLrate(net, net->epoch);	// actually make it take NOW to affect cur_lrate
    FOREACH_ELEM_IN_GROUP(LeabraConSpec, lc, cs->children) {
      lc->lrate_sched.default_val = lrate;
      lc->UpdateAfterEdit_NoGui();        // propagate to children
      lc->SetCurLrate(net, net->epoch);	// actually make it take NOW to affect cur_lrate
    }
  }
}

void SubiculumLayerSpec::Compute_CycleStats(LeabraLayer* lay, LeabraNetwork* net) {
  Compute_ECNovelty(lay, net);
  inherited::Compute_CycleStats(lay, net);
}

void SubiculumLayerSpec::PostSettle(LeabraLayer* lay, LeabraNetwork* net) {
  inherited::PostSettle(lay, net);
  if(net->phase == LeabraNetwork::PLUS_PHASE) {
    Compute_SetLrate(lay, net);
  }
}

///////////////////////////////////////////////////////////////
//   HippoEncoderConSpec

void HippoEncoderConSpec::Initialize() {
  SetUnique("lmix", true);
  lmix.hebb = 0.005f;		// works better with tiny bit of hebb apparently..
  lmix.err = 1.0f;
//   lmix.err_sb = false;

  // SetUnique("wt_limits", true);
  // wt_limits.sym = false;

  SetUnique("wt_sig", true);
  wt_sig.gain = 6.0f;  wt_sig.off = 1.0f; // 1.0f seems to work better actually

  SetUnique("savg_cor", true);
  savg_cor.cor = 1.0f;

//   SetUnique("xcalm", true);
//   xcalm.use_sb = false;
}

void HippoEncoderConSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  // these are enforced absolutely because the code does not use them:
  // actually it does so turning this off!
  // lmix.hebb = 0.0f;
  // lmix.err = 1.0f;
}

void XCalCHLConSpec::Initialize() {
  use_chl = true;
}

///////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////
//              Wizard          //
//////////////////////////////////

bool LeabraWizard::Hippo(LeabraNetwork* net, int n_ec_slots) {
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
  LeabraConSpec* in_ecin_cons = (LeabraConSpec*)ecca1_cons->FindMakeChild("Input_EC_in", &TA_LeabraConSpec);
  LeabraConSpec* ecout_out_cons = (LeabraConSpec*)ecca1_cons->FindMakeChild("EC_out_Output", &TA_LeabraConSpec);
  LeabraConSpec* tosubic_cons = (LeabraConSpec*)ecca1_cons->FindMakeChild("ToSubic", &TA_LeabraConSpec);

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

  net->FindMakePrjn(ecin, ecout, onetoone, ecout_ecin_cons);

//   net->FindMakePrjn(ecout, ecin, onetoone, ecin_ecout_cons);
  net->FindMakePrjn(ecout, ca1, gponetoone, ca1_ecout_cons);

  net->FindMakePrjn(dg, ecin, ppath_prjn, ppath_cons);

  net->FindMakePrjn(ca3, ecin, ppath_prjn, ppath_cons);
  net->FindMakePrjn(ca3, dg, mossy_prjn, mossy_cons);
  net->FindMakePrjn(ca3, ca3, fullprjn, ca3ca3_cons);

  net->FindMakePrjn(ca1, ecin, gponetoone, ecin_ca1_cons);
  net->FindMakePrjn(ca1, ecout, gponetoone, ecout_ca1_cons);
  net->FindMakePrjn(ca1, ca3, fullprjn, ca3ca1_cons);

  net->FindMakePrjn(subic, ecin, onetoone, tosubic_cons);
  net->FindMakePrjn(subic, ecout, onetoone, tosubic_cons);

  //////////////////////////////////////////////////////////////////////////////////
  // set positions & geometries

  ecin->unit_groups = true;
  ecin->SetNUnitGroups(n_ec_slots);
  ecin->SetNUnits(49);

  ecout->unit_groups = true;
  ecout->SetNUnitGroups(n_ec_slots);
  ecout->SetNUnits(49);

  ca1->unit_groups = true;
  ca1->SetNUnitGroups(n_ec_slots);
  ca1->SetNUnits(100);

  dg->SetNUnits(1000);

  ca3->SetNUnits(225);

  subic->SetNUnits(12);
  subic->un_geom.x = 12;
  subic->un_geom.y = 1;

  hip_laygp->pos.SetXYZ(0, 0, 1);
  ecin->pos.SetXYZ(0, 0, 0);
  ecout->pos.SetXYZ(35, 0, 0);
  subic->pos.SetXYZ(70, 0, 0);
  dg->pos.SetXYZ(0, 0, 1);
  ca3->pos.SetXYZ(0, 0, 2);
  ca1->pos.SetXYZ(35, 0, 2);

  //////////////////////////////////////////////////////////////////////////////////
  // params

  // EC_CA1ConSpecs, wt_sig.gain = 6, off 1.25, cor = 1 (not .4)

  // EC_in_CA1, abs = 2
  ecin_ca1_cons->SetUnique("wt_scale", true);
  ecin_ca1_cons->wt_scale.abs = 2.0f;

  // CA1_EC_out, abs = 4
  ca1_ecout_cons->SetUnique("wt_scale", true);
  ca1_ecout_cons->wt_scale.abs = 4.0f;

  // EC_in_EC_out mean/var = .9, .01, rel = 0, lrate = 0
  ecin_ecout_cons->SetUnique("wt_scale", true);
  ecin_ecout_cons->wt_scale.rel = 0.0f;
  ecin_ecout_cons->SetUnique("lrate", true);
  ecin_ecout_cons->lrate = 0.0f;
  ecin_ecout_cons->SetUnique("rnd", true);
  ecin_ecout_cons->rnd.mean = 0.9f;
  ecin_ecout_cons->rnd.var = 0.01f;

  ecout_ecin_cons->SetUnique("lrate", true);
  ecout_ecin_cons->lrate = 0.0f;
  ecout_ecin_cons->SetUnique("wt_scale", true);
  ecout_ecin_cons->wt_scale.rel = 0.5f;
  ecout_ecin_cons->SetUnique("rnd", true);
  ecout_ecin_cons->rnd.mean = 0.5f;
  ecout_ecin_cons->rnd.var = 0.01f;

  // HippoConSpecs, lrate = .2, hebb = 0.05
  hip_cons->SetUnique("lrate", true);
  hip_cons->lrate = 0.2f;
  hip_cons->SetUnique("lmix", true);
  hip_cons->lmix.hebb = 0.05f;
  
  // mossy mean = 0.9 var = 0.01, rel = 8 , lrate = 0
  mossy_cons->SetUnique("rnd", true);
  mossy_cons->rnd.mean = 0.9f;
  mossy_cons->rnd.var = 0.01f;
  mossy_cons->SetUnique("wt_scale", true);
  mossy_cons->wt_scale.rel = 8.0f;
  mossy_cons->SetUnique("lrate", true);
  mossy_cons->lrate = 0.0f;
  mossy_cons->SetUnique("savg_cor", true);
  mossy_cons->savg_cor.cor = 1.0f;
  
  // ca3_ca3 rel = 2, 
  ca3ca3_cons->SetUnique("wt_scale", true);
  ca3ca3_cons->wt_scale.rel = 2.0f;
  ca3ca3_cons->SetUnique("savg_cor", true);
  ca3ca3_cons->savg_cor.cor = 1.0f;

  // ca3_ca1 lrate = 0.05
  ca3ca1_cons->SetUnique("lrate", true);
  ca3ca1_cons->lrate = 0.05f;
  ca3ca1_cons->SetUnique("lmix", true);
  ca3ca1_cons->lmix.hebb = 0.005f;

  // sparse hippocampal layers!

  dg_laysp->SetUnique("kwta", true);
  dg_laysp->kwta.pct = 0.01f;

  ca3_laysp->SetUnique("kwta", true);
  ca3_laysp->kwta.pct = 0.02f;

  ca1_laysp->SetUnique("inhib_group", true);
  ca1_laysp->inhib_group = LeabraLayerSpec::UNIT_GROUPS;
  ca1_laysp->SetUnique("gp_kwta", true);
  ca1_laysp->gp_kwta.pct = 0.1f;
 

  subic_laysp->lrate_mod_con_spec.SetSpec(ca3ca1_cons);

  // todo; lrate schedule!
  
  //////////////////////////////////////////////////////////////////////////////////
  // build and check

  net->Build();
  net->LayerPos_Cleanup();

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

  for(int j=0;j<net->specs.leaves;j++) {
    BaseSpec* sp = (BaseSpec*)net->specs.Leaf(j);
    sp->UpdateAfterEdit();
  }

  LeabraProject* proj = GET_MY_OWNER(LeabraProject);
  if(proj) {
    proj->undo_mgr.SaveUndo(net, "Wizard::Hippo -- actually saves network specifically");
  }
  return true;
}


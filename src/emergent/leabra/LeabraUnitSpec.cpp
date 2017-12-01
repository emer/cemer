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

#include "LeabraUnitSpec.h"
#include <LeabraNetwork>
#include <LeabraBiasSpec>
#include <LeabraExtraConSpecs>

#include <taProject>
#include <taMath_double>
#include <DataTable>
#include <CircBufferIndex>
#include <FunLookup>

#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(LeabraActFunSpec);
TA_BASEFUNS_CTORS_DEFN(LeabraActMiscSpec);
TA_BASEFUNS_CTORS_DEFN(SpikeFunSpec);
TA_BASEFUNS_CTORS_DEFN(SpikeMiscSpec);
TA_BASEFUNS_CTORS_DEFN(OptThreshSpec);
TA_BASEFUNS_CTORS_DEFN(LeabraInitSpec);
TA_BASEFUNS_CTORS_DEFN(LeabraDtSpec);
TA_BASEFUNS_CTORS_DEFN(LeabraActAvgSpec);
TA_BASEFUNS_CTORS_DEFN(LeabraAvgLSpec);
TA_BASEFUNS_CTORS_DEFN(LeabraAvgL2Spec);
TA_BASEFUNS_CTORS_DEFN(LeabraChannels);
TA_BASEFUNS_CTORS_DEFN(KNaAdaptSpec);
TA_BASEFUNS_CTORS_DEFN(ShortPlastSpec);
TA_BASEFUNS_CTORS_DEFN(SynDelaySpec);
TA_BASEFUNS_CTORS_DEFN(DeepSpec);
TA_BASEFUNS_CTORS_DEFN(TRCSpec);
TA_BASEFUNS_CTORS_DEFN(DaModSpec);
TA_BASEFUNS_CTORS_DEFN(LeabraNoiseSpec);

TA_BASEFUNS_CTORS_DEFN(LeabraUnitSpec);

SMARTREF_OF_CPP(LeabraUnitSpec);

#include <LeabraLayerSpec_cpp>
//#include <LeabraConSpec_cpp>
#include <LeabraExtraConSpecs_cpp> // includes base

#include <State_main>

#include "LeabraUnitSpec_core.cpp"

/* TODO:
void LeabraAvgLSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  taVersion v804(8, 0, 4);
  if(taMisc::is_loading && taMisc::loading_version < v804) {
    if(gain == 1.5f || gain == 2.5f) {
      gain = 2.5f;
    }
    else {
      TestWarning(true, "UAE",
                  "avg_l computation has been updated to a running-average formulation that works much better overall -- previous max value was a nonstandard:", String(gain),
                  "(default was 1.5 before, 2.5 now) -- updating to 2.5 now but you should experiment and find the best current value for this param!");
      gain = 2.5f;
    }
    if(lrn_max == 0.05f && lrn_min == 0.005f) {
      lrn_max = 0.5f;
      lrn_min = 0.0001f;
    }
    else {
      TestWarning(true, "UAE",
                  "avg_l computation has been updated to a running-average formulation that works much better overall -- previous lrn_max and/or lrn_min value(s) were nonstandard:",
                  String(lrn_max), String(lrn_min),
                  "new defaults are 0.5, 0.0001 -- recommend trying them!");
    }
  }
}
*/

void LeabraUnitSpec::Initialize() {
  min_obj_type = &TA_LeabraUnitState_cpp;
  bias_spec.SetBaseType(&TA_LeabraBiasSpec);

  Defaults_init();
  Initialize_core();
}

void LeabraUnitSpec::Defaults_init() {
}

void LeabraUnitSpec::InitLinks() {
  //  bias_spec.type = &TA_LeabraBiasSpec;
  inherited::InitLinks();
  InitLinks_taAuto(&TA_LeabraUnitSpec);
  children.SetBaseType(&TA_LeabraUnitSpec);
  children.el_typ = GetTypeDef(); // but make the default to be me!
}

void LeabraUnitSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();

  if(noise_type.type == LeabraNoiseSpec::VM_NOISE && act_fun != SPIKE) {
    taMisc::Warning("Cannot use noise_type = VM_NOISE with rate-code (non-spiking) activation function -- changing noise_type to NETIN_NOISE");
    noise_type.type = LeabraNoiseSpec::NETIN_NOISE;
  }

  if(spike_misc.ex)
    spike_misc.eff_spk_thr = spike_misc.spk_thr;
  else
    spike_misc.eff_spk_thr = act.thr;

  act_misc.UpdateAfterEdit_NoGui();
  spike.UpdateAfterEdit_NoGui();
  spike_misc.UpdateAfterEdit_NoGui();
  clamp_range.UpdateAfterEdit_NoGui();
  vm_range.UpdateAfterEdit_NoGui();
  dt.UpdateAfterEdit_NoGui();
  act_avg.UpdateAfterEdit_NoGui();
  kna_adapt.UpdateAfterEdit_NoGui();
  stp.UpdateAfterEdit_NoGui();
  deep.UpdateAfterEdit_NoGui();

  e_rev_sub_thr.e = e_rev.e - act.thr;
  e_rev_sub_thr.l = e_rev.l - act.thr;
  e_rev_sub_thr.i = e_rev.i - act.thr;
  thr_sub_e_rev_i = (act.thr - e_rev.i);
  thr_sub_e_rev_e = (act.thr - e_rev.e);

    // if(deep.on && deep_raw_qtr == QNULL) { // doesn't make sense to not have any deep raw..
  //   deep_raw_qtr = Q4;
  // }
}

void LeabraUnitSpec::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  LeabraNetwork* net = GET_MY_OWNER(LeabraNetwork);
  if(net) {
    // if(dt.integ != 1000.0f * net->times.time_inc) {
    //   taMisc::Warning("unit time integration constant dt.integ of:", (String)dt.integ,
    //                   "does not match network phases.time_inc increment of:",
    //                   (String)net->times.time_inc, "time_inc should be 0.001 * dt.integ");
    // }
  }
}

bool LeabraUnitSpec::CheckConfig_Unit(Layer* lay, bool quiet) {
  if(!inherited::CheckConfig_Unit(lay, quiet)) return false;

  bool rval = true;

  return rval;
}

void LeabraUnitSpec::CreateNXX1Fun(LeabraActFunSpec& act_spec, FunLookup& nxx1_fl, 
                                   FunLookup& noise_fl) {
  // first create the gaussian noise convolver
  nxx1_fl.x_range.min = -.1f;
  nxx1_fl.x_range.max = 1.0f;
  nxx1_fl.res = .001f;       // needs same fine res to get the noise transitions
  nxx1_fl.UpdateAfterEdit_NoGui();

  float ns_rng = 3.0f * act_spec.nvar;       // range factor based on noise level -- 3 sd
  ns_rng = MAX(ns_rng, nxx1_fl.res);
  nxx1_fl.x_range.min = -ns_rng;

  noise_fl.x_range.min = -ns_rng;
  noise_fl.x_range.max = ns_rng;
  noise_fl.res = nxx1_fl.res;
  noise_fl.UpdateAfterEdit_NoGui();

  noise_fl.AllocForRange();
  int i;
  float eff_nvar = fmaxf(act_spec.nvar, 1.0e-6f); // just too lazy to do proper conditional for 0..
  float var = eff_nvar * eff_nvar;
  for(i=0; i < noise_fl.size; i++) {
    float x = noise_fl.Xval(i);
    noise_fl[i] = expf(-((x * x) / var));
  }

  // normalize it
  float sum = 0.0f;
  for(i=0; i < noise_fl.size; i++)
    sum += noise_fl[i];
  for(i=0; i < noise_fl.size; i++)
    noise_fl[i] /= sum;

  // then create the initial function
  FunLookup fun;
  fun.x_range.min = nxx1_fl.x_range.min + noise_fl.x_range.min;
  fun.x_range.max = nxx1_fl.x_range.max + noise_fl.x_range.max;
  fun.res = nxx1_fl.res;
  fun.UpdateAfterEdit_NoGui();
  fun.AllocForRange();

  for(i=0; i<fun.size; i++) {
    float x = fun.Xval(i);
    float val = 0.0f;
    if(x > 0.0f)
      val = (act_spec.gain * x) / ((act_spec.gain * x) + 1.0f);
    fun[i] = val;
  }

  nxx1_fl.Convolve(fun, noise_fl); // does alloc
}


///////////////////////////////////////////////////////////////////////
//      TrialInit functions


//////////////////////////////////////////
//       Misc Functions                 //
//////////////////////////////////////////

void LeabraUnitSpec::BioParams(float norm_sec, float norm_volt, float volt_off, float norm_amp,
          float C_pF, float gbar_l_nS, float gbar_e_nS, float gbar_i_nS,
          float erev_l_mV, float erev_e_mV, float erev_i_mV,
          float act_thr_mV, float spk_thr_mV, float exp_slope_mV)
{
  // derived units
  float norm_siemens = norm_amp / norm_volt;
  float norm_farad = (norm_sec * norm_amp) / norm_volt;

  dt.vm_tau = ((C_pF * 1.0e-12f) / norm_farad);

  g_bar.l = (gbar_l_nS * 1.0e-9f) / norm_siemens;
  g_bar.e = (gbar_e_nS * 1.0e-9f) / norm_siemens;
  g_bar.i = (gbar_i_nS * 1.0e-9f) / norm_siemens;

  e_rev.l = ((erev_l_mV * 1.0e-3f) - volt_off) / norm_volt;
  e_rev.e = ((erev_e_mV * 1.0e-3f) - volt_off) / norm_volt;
  e_rev.i = ((erev_i_mV * 1.0e-3f) - volt_off) / norm_volt;

  act.thr = ((act_thr_mV* 1.0e-3f) - volt_off) / norm_volt;
  spike_misc.spk_thr = ((spk_thr_mV * 1.0e-3f) - volt_off) / norm_volt;
  spike_misc.exp_slope = ((exp_slope_mV * 1.0e-3f)) / norm_volt; // no off!
  spike_misc.vm_r = e_rev.l;                                     // go back to leak

  // adapt.tau = (adapt_tau_ms * 1.0e-3f) / norm_sec;
  // adapt.vm_gain = (adapt_vm_gain_nS * 1.0e-9f) / norm_siemens;
  // adapt.spike_gain = (adapt_spk_gain_nA * 1.0e-9f) / norm_amp;

  init.v_m = 0.4f;
  vm_range.min = 0.0f;
  vm_range.max = 2.0f;

  if(act_fun != SPIKE) {
    dt.vm_tau = 3.3f;
    act.gain = 100;
  }

  UpdateAfterEdit();
}

void LeabraUnitSpec::GraphVmFun(DataTable* graph_data, float g_i, float min, float max, float incr) {
  taProject* proj = GetMyProj();
  if(!graph_data) {
    graph_data = proj->GetNewAnalysisDataTable(name + "_VmFun", true);
  }
  int idx;
  graph_data->StructUpdate(true);
  graph_data->ResetData();
  DataCol* nt = graph_data->FindMakeColName("Net", idx, VT_FLOAT);
  DataCol* vm = graph_data->FindMakeColName("Vm", idx, VT_FLOAT);
  
  incr = fmaxf(0.001f, incr);	// must be pos

  for(float x = min; x <= max; x += incr) {
    float y = ((g_bar.e * x * e_rev.e) + (g_bar.i * g_i * e_rev.i) + (g_bar.l * e_rev.l)) /
      ((g_bar.e * x) + (g_bar.i * g_i) + g_bar.l);
    graph_data->AddBlankRow();
    nt->SetValAsFloat(x, -1);
    vm->SetValAsFloat(y, -1);
  }
  graph_data->StructUpdate(false);
  graph_data->FindMakeGraphView();
}

void LeabraUnitSpec::GraphActFmNetFun(DataTable* graph_data, float g_i, float min, float max,
                                      float incr, float g_e_thr, float lin_gain) {
  taProject* proj = GetMyProj();
  if(!graph_data) {
    graph_data = proj->GetNewAnalysisDataTable(name + "_ActFmNetFun", true);
  }
  
  FunLookup        nxx1_fun;
  FunLookup        noise_conv;

  nxx1_fun.OwnTempObj();
  noise_conv.OwnTempObj();

  CreateNXX1Fun(act, nxx1_fun, noise_conv);
  int idx;
  graph_data->StructUpdate(true);
  graph_data->ResetData();
  DataCol* nt = graph_data->FindMakeColName("Net", idx, VT_FLOAT);
  DataCol* av = graph_data->FindMakeColName("Act", idx, VT_FLOAT);
  DataCol* lin = graph_data->FindMakeColName("Linear", idx, VT_FLOAT);
  DataCol* cnv = graph_data->FindMakeColName("Convolved", idx, VT_FLOAT);

  av->SetUserData("PLOT_1", true);
  av->SetUserData("MIN", 0.0f);
  av->SetUserData("MAX", 1.0f);

  lin->SetUserData("PLOT_2", true);

//  LeabraNetwork* net = GET_MY_OWNER(LeabraNetwork);

  incr = fmaxf(0.001f, incr);	// must be pos
  float x;
  for(x = min; x <= max; x += incr) {
    float aval;
    aval = Compute_ActFun_Rate_fun(x - g_e_thr);
    float val_sub_thr = x - g_e_thr;
    float ln = val_sub_thr;
    if(ln < 0.0f) ln = 0.0f;
    ln *= lin_gain;
    if(ln > 1.0f) ln = 1.0f;
    graph_data->AddBlankRow();
    nt->SetValAsFloat(x, -1);
    av->SetValAsFloat(aval, -1);
    lin->SetValAsFloat(ln, -1);

    float cnval = 0.0f;
    if(val_sub_thr >= nxx1_fun.x_range.max) {
      float tmp = val_sub_thr * act.gain;
      cnval = tmp / (tmp + 1.0f);
    }
    else if(val_sub_thr > nxx1_fun.x_range.min) {
      cnval = nxx1_fun.Eval(val_sub_thr);
    }
    cnv->SetValAsFloat(cnval, -1);
  }
  graph_data->StructUpdate(false);
  graph_data->FindMakeGraphView();
}

void LeabraUnitSpec::TimeExp(int mode, int nreps) {

  const char* modes[6] = {"double sum", "double exp", "double exp_fast",
                          "float sum", "float exp", "float exp_fast"};

  TimeUsedHR tu;
  tu.StartTimer(true);          // start, w/reset
  double dsum = 0.0;
  float  fsum = 0.0f;
  switch (mode) {
  case 0: {
    for(int i=0;i<nreps; i++) {
      dsum += Random::UniformMinMax(-50.0, 50.0) - 0.99 * dsum;
    }
    break;
  }
  case 1: {
    for(int i=0;i<nreps; i++) {
      dsum += taMath_double::exp(Random::UniformMinMax(-50.0, 50.0)) - 0.99 * dsum;
    }
    break;
  }
  case 2: {
    for(int i=0;i<nreps; i++) {
      dsum += taMath_double::exp_fast(Random::UniformMinMax(-50.0, 50.0)) - 0.99 * dsum;
    }
    break;
  }
  case 3: {
    for(int i=0;i<nreps; i++) {
      fsum += Random::UniformMinMax(-50.0, 50.0) - 0.99f * fsum;
    }
    break;
  }
  case 4: {
    for(int i=0;i<nreps; i++) {
      fsum += taMath_float::exp(Random::UniformMinMax(-50.0, 50.0)) - 0.99f * fsum;
    }
    break;
  }
  case 5: {
    for(int i=0;i<nreps; i++) {
      fsum += taMath_float::exp_fast(Random::UniformMinMax(-50.0, 50.0)) - 0.99f * fsum;
    }
    break;
  }
  }
  tu.EndTimer();
  String msg;
  msg << "mode: " << mode << " " << modes[mode] << " seconds used: " << tu.s_used
      << " dsum: " << dsum << " fsum: " << fsum;
  taMisc::Info(msg);
}

void LeabraUnitSpec::GraphSpikeAlphaFun(DataTable* graph_data, bool force_alpha) {
  taProject* proj = GetMyProj();
  if(!graph_data) {
    graph_data = proj->GetNewAnalysisDataTable(name + "_SpikeAlphFun", true);
  }
  int idx;
  graph_data->StructUpdate(true);
  graph_data->ResetData();
  DataCol* t = graph_data->FindMakeColName("time_fm_spike", idx, VT_FLOAT);
  DataCol* g = graph_data->FindMakeColName("conductance", idx, VT_FLOAT);

  float tmax = fmaxf(spike.window, 2.0f);

  float sumg = 0.0f;
  float x;
  if(!force_alpha && spike.rise == 0.0f && spike.decay > 0.0f) {
    float net = 0.0f;
    float input;
    tmax = 20.0f;
    for(x = 0.0f; x <= tmax; x += 1.0f) {
      if(x < spike.window)
        input = spike.gg_decay / (float)spike.window;
      else
        input = 0.0f;
      net = net + input - net / spike.decay;
      graph_data->AddBlankRow();
      t->SetValAsFloat(x, -1);
      g->SetValAsFloat(net, -1);
      sumg += net;
    }
  }
  else {
    for(x = 0.0f; x <= tmax; x += 1.0f) {
      float y = spike.ComputeAlpha(x);
      graph_data->AddBlankRow();
      t->SetValAsFloat(x, -1);
      g->SetValAsFloat(y, -1);
      sumg += y;
    }
  }
  graph_data->AddBlankRow();
  t->SetValAsFloat(x, -1);
  g->SetValAsFloat(0.0f, -1);
  graph_data->AddBlankRow();
  t->SetValAsFloat(x+1.0f, -1);
  g->SetValAsFloat(sumg, -1);
  graph_data->StructUpdate(false);
  graph_data->FindMakeGraphView();
}

// void LeabraUnitSpec::GraphSLNoiseAdaptFun(DataTable* graph_data, float incr) {
//   taProject* proj = GetMyProj();
//   if(!graph_data) {
//     graph_data = proj->GetNewAnalysisDataTable(name + "_SLNoiseAdaptFun", true);
//   }
//   int idx;
//   graph_data->StructUpdate(true);
//   graph_data->ResetData();
//   DataCol* lt = graph_data->FindMakeColName("LongTerm", idx, VT_FLOAT);
//   lt->SetUserData("X_AXIS", true);
//   DataCol* st = graph_data->FindMakeColName("ShortTerm", idx, VT_FLOAT);
//   st->SetUserData("Z_AXIS", true);
//   DataCol* nv = graph_data->FindMakeColName("NoiseVarMult", idx, VT_FLOAT);
//   nv->SetUserData("PLOT_1", true);
//   nv->SetUserData("MIN", 0.0f);
//   nv->SetUserData("MAX", 1.0f);

//   LeabraNetwork* net = GET_MY_OWNER(LeabraNetwork);

//   for(float ltv = 0.0f; ltv <= 1.0f; ltv += incr) {
//     for(float stv = 0.0f; stv <= 1.0f; stv += incr) {
//       float nvar = noise_adapt.SLNoiseFun(stv, ltv);
//       graph_data->AddBlankRow();
//       lt->SetValAsFloat(ltv, -1);
//       st->SetValAsFloat(stv, -1);
//       nv->SetValAsFloat(nvar, -1);
//     }
//   }
//   taDataAnal::Matrix3DGraph(graph_data, "LongTerm", "ShortTerm");
//   graph_data->SetUserData("NEG_DRAW", false);
//   graph_data->SetUserData("NEG_DRAW_Z", false);
//   graph_data->StructUpdate(false);
//   graph_data->FindMakeGraphView();
// }



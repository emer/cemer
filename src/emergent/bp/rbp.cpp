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



// rbp.cc

#include "rbp.h"
#include "netstru_extra.h"

// todo: complete new RBP netstru hierarchy required
// move rbp functions over to Network from corresponding 3.2 processes

//////////////////////////
//  	CircBuffer	//
//////////////////////////

void float_CircBuffer::Initialize() {
  st_idx = 0;
  length = 0;
}

void float_CircBuffer::Copy_(const float_CircBuffer& cp) {
  st_idx = cp.st_idx;
  length = cp.length;
}

void float_CircBuffer::Add(const float& item) {
  if((st_idx == 0) && (length >= size)) { // must be building up the list, so add it
    float_Array::Add(item);
    length++;
    return;
  }
  Set(CircIdx(length++), item);	// set it to the element at the end
}

void float_CircBuffer::Reset() {
  float_Array::Reset();
  st_idx = 0;
  length = 0;
}


//////////////////////////
//  	UnitSpec	//
//////////////////////////

void RBpUnitSpec::Initialize() {
  min_obj_type = &TA_RBpUnit;
  dt = 1.0f;
  time_avg = NET_INPUT;
  soft_clamp = false;
  soft_clamp_gain = 1.0f;
  teacher_force = false;
  store_states = true;
  bias_spec.SetBaseType(&TA_RBpConSpec);
  initial_act.type = Random::UNIFORM;
  initial_act.mean = .5f;
  initial_act.var = 0.0f;
  updt_clamped_wts = false;
  fast_hard_clamp_net = false;
}

void RBpUnitSpec::Init_Acts(Unit* u) {
  BpUnitSpec::Init_Acts(u);
  RBpUnit* ru = (RBpUnit*)u;
  ru->act = initial_act.Gen();
  // set initial net value too..
  ru->net = sig.Inverse(SigmoidSpec::Clip(act_range.Normalize(ru->act)));
  ru->prv_act = ru->act;
  ru->da = 0;
  ru->ddE = 0;
  ru->prv_ext_flag = Unit::NO_EXTERNAL;
  ru->prv_ext = 0.0f;
  ru->prv_targ = 0.0f;
  ru->act_raw = 0.0f;
  ru->prv_net = 0.0f;
  ru->prv_dEdA = 0.0f;
  ru->prv_dEdNet = 0.0f;
  ru->clmp_net = 0.0f;
  ru->ext_flags.Reset();
  ru->targs.Reset();
  ru->exts.Reset();
  ru->acts.Reset();
}

void RBpUnitSpec::UpdateAfterEdit_impl() {
  BpUnitSpec::UpdateAfterEdit_impl();
  if((dt < 0.0f) || (dt > 1.0f))
    taMisc::Error("Warning: dt must be in the [0..1] range!");
}

void RBpUnitSpec::ResetStored(RBpUnit* u) {
  u->ext_flags.Reset();
  u->targs.Reset();
  u->exts.Reset();
  u->acts.Reset();
}

void RBpUnitSpec::Compute_ClampExt(RBpUnit* u) {
  if(!soft_clamp && (u->ext_flag & Unit::EXT)) {
    u->act = u->act_raw = u->ext;
    u->net = sig.Inverse(SigmoidSpec::Clip(act_range.Normalize(u->act)));
  }
}

void RBpUnitSpec::Compute_HardClampNet(RBpUnit* ru) {
  ru->clmp_net = 0.0f;
  if(!fast_hard_clamp_net) return;
  for(int g=0; g<ru->recv.size; g++) {
    RecvCons* recv_gp = (RecvCons*)ru->recv.FastEl(g);
    Layer* fmlay = recv_gp->prjn->from;
    if(fmlay->lesioned() || !(fmlay->ext_flag & Unit::EXT))
      continue;		// don't get from the non-clamped layers!
    ru->clmp_net += recv_gp->Compute_Netin(ru);
  }
  if(ru->bias.cons.size)
    ru->clmp_net += ru->bias.Cn(0)->wt;
}

void RBpUnitSpec::Compute_Netin(Unit* u) {
  RBpUnit* ru = (RBpUnit*)u;
  ru->prv_net = ru->net; // save current net as previous
  if(fast_hard_clamp_net) {
    ru->net = ru->clmp_net;
    for(int g=0; g<u->recv.size; g++) {
      RecvCons* recv_gp = (RecvCons*)u->recv.FastEl(g);
      Layer* fmlay = recv_gp->prjn->from;
      if(fmlay->lesioned() || (fmlay->ext_flag & Unit::EXT))
	continue;		// don't get from the clamped layers
      u->net += recv_gp->Compute_Netin(u);
    }
  }
  else {
    BpUnitSpec::Compute_Netin(u);
  }
}

void RBpUnitSpec::Compute_Act_impl(RBpUnit* u) {
  if(time_avg == ACTIVATION) {
    u->act_raw = act_range.Project(sig.Eval(u->net));
    u->da = u->act_raw - u->prv_act;
    u->act = u->prv_act + dt * u->da;
  }
  else {
    u->da = u->net - u->prv_net;
    u->net = u->prv_net + dt * u->da;
    u->act = u->act_raw = act_range.Project(sig.Eval(u->net));
  }
}

void RBpUnitSpec::Compute_Act(Unit* u) {
  RBpUnit* rbu = (RBpUnit*)u;
  rbu->prv_act = rbu->act;	// save current act as previous
  if(u->ext_flag & Unit::EXT) {
    if(soft_clamp) {
      rbu->net += soft_clamp_gain * rbu->ext;
      Compute_Act_impl(rbu);
    }
    else {
      rbu->act = rbu->act_raw = rbu->ext;
      rbu->net = sig.Inverse(SigmoidSpec::Clip(act_range.Normalize(rbu->act)));
    }
  }
  else
    Compute_Act_impl(rbu);

  if(store_states)
    rbu->StoreState();
}

void RBpUnitSpec::Compute_dEdA(BpUnit* u) {
  RBpUnit* rbu = (RBpUnit*)u;
  rbu->prv_dEdA = rbu->dEdA;	// save current value
  BpUnitSpec::Compute_dEdA(u);	// get new dEdA
}

void RBpUnitSpec::Compute_dEdNet(BpUnit* u) {
  RBpUnit* rbu = (RBpUnit*)u;

  if(time_avg == ACTIVATION) {
    rbu->ddE = rbu->dEdA - rbu->prv_dEdA;
    rbu->dEdA = rbu->prv_dEdA + dt * rbu->ddE;	// update current
  }
  if(teacher_force && (u->ext_flag & Unit::TARG))
    rbu->prv_act = u->targ;	// force the teacher value for back propagation..

  rbu->prv_dEdNet = rbu->dEdNet; // save current value
  if((u->ext_flag & Unit::EXT) && !soft_clamp) {
    rbu->dEdNet = rbu->ddE = 0.0f; // no err for clamped units..
    return;
  }

  BpUnitSpec::Compute_dEdNet(u); // get new dEdNet

  if(time_avg == NET_INPUT) {
    rbu->ddE = rbu->dEdNet - rbu->prv_dEdNet;
    rbu->dEdNet = rbu->prv_dEdNet + dt * rbu->ddE; // update current
  }
}

void RBpUnitSpec::Compute_dWt(Unit* u) {
  if((u->ext_flag & Unit::EXT) && !soft_clamp && !updt_clamped_wts)  return; // don't compute dwts for clamped units
  UnitSpec::Compute_dWt(u);
  ((BpConSpec*)bias_spec.SPtr())->B_Compute_dWt((BpCon*)u->bias.Cn(0), (BpUnit*)u);
}

void RBpUnitSpec::Compute_Weights(Unit* u) {
  if((u->ext_flag & Unit::EXT) && !soft_clamp && !updt_clamped_wts) return; // don't update for clamped units
  UnitSpec::Compute_Weights(u);
  ((BpConSpec*)bias_spec.SPtr())->B_Compute_Weights((BpCon*)u->bias.Cn(0), (BpUnit*)u);
}


//////////////////////////
//  	Unit		//
//////////////////////////

void RBpUnit::Initialize() {
  prv_ext_flag = Unit::NO_EXTERNAL;
  prv_targ = 0.0f;
  prv_ext = 0.0f;
  prv_act = 0.0f;
  act_raw = 0.0f;
  prv_net = 0.0f;
  prv_dEdA = 0.0f;
  prv_dEdNet = 0.0f;
  clmp_net = 0.0f;
}

void RBpUnit::InitLinks() {
  BpUnit::InitLinks();
  taBase::Own(ext_flags, this);
  taBase::Own(targs, this);
  taBase::Own(exts, this);
  taBase::Own(acts, this);
}

void RBpUnit::Copy_(const RBpUnit& cp) {
  da = cp.da;
  ddE = cp.ddE;
  prv_ext_flag = cp.prv_ext_flag;
  prv_targ = cp.prv_targ;
  prv_ext = cp.prv_ext;
  prv_act = cp.prv_act;
  act_raw = cp.act_raw;
  prv_net = cp.prv_net;
  prv_dEdA = cp.prv_dEdA;
  prv_dEdNet = cp.prv_dEdNet;

  ext_flags = cp.ext_flags;
  targs = cp.targs;
  exts  = cp.exts;
  acts = cp.acts;
}

void RBpUnit::Init_InputData() {
  prv_ext_flag = ext_flag;
  prv_targ = targ;
  prv_ext = ext;
  BpUnit::Init_InputData();
}

void RBpUnit::StoreState() {
  ext_flags.Add((float)ext_flag);	// store values in arrays
  targs.Add(targ);
  exts.Add(ext);
  acts.Add(act);
}

bool RBpUnit::StepBack(int tick) {
  int tck_idx = ext_flags.CircIdx(tick);
  if(ext_flags.size <= tck_idx)  // not enough depth here..
    return false;
  ext_flag = prv_ext_flag;
  targ = prv_targ;
  ext = prv_ext;
  act = prv_act;

  prv_ext_flag = (ExtType)(int)ext_flags.FastEl(tck_idx); //NOTE: float->int->ExtType this is REALLY not good!
  prv_targ = targs.FastEl(tck_idx);
  prv_ext = exts.FastEl(tck_idx);
  prv_act = acts.FastEl(tck_idx);
  return true;
}

bool RBpUnit::RestoreState(int tick) {
  int tck_idx = ext_flags.CircIdx(tick);
  if(ext_flags.size <= tck_idx)  // not enough depth here..
    return false;
  ext_flag = (ExtType)(int)ext_flags.FastEl(tck_idx); //NOTE: float->int->ExtType this is REALLY not good!
  targ = targs.FastEl(tck_idx);
  ext = exts.FastEl(tck_idx);
  act = acts.FastEl(tck_idx);

  tck_idx = ext_flags.CircIdx(tick-1); // get previous data too
  if(ext_flags.size <= tck_idx)  // not enough depth here..
    return false;
  prv_ext_flag = (ExtType)(int)ext_flags.FastEl(tck_idx); //NOTE: float->int->ExtType this is REALLY not good!
  prv_targ = targs.FastEl(tck_idx);
  prv_ext = exts.FastEl(tck_idx);
  prv_act = acts.FastEl(tck_idx);
  return true;
}


void RBpUnit::ShiftBuffers(int ticks) {
  ext_flags.ShiftLeft(ticks);
  targs.ShiftLeft(ticks);
  exts.ShiftLeft(ticks);
  acts.ShiftLeft(ticks);
}


//////////////////////////
// 	RBpContextSpec	//
//////////////////////////

void RBpContextSpec::Initialize() {
  hysteresis = .3f;
  hysteresis_c = .7f;
  variable = "act";
  unit_flags = Unit::NO_EXTERNAL;
}

void RBpContextSpec::UpdateAfterEdit_impl() {
  RBpUnitSpec::UpdateAfterEdit_impl();
  hysteresis_c = 1.0f - hysteresis;
  var_md = TA_RBpUnit.members.FindName(variable);
  if(var_md == NULL)
    taMisc::Error("RBpContextSpec: could not find variable:",variable,"in RBpUnit type");
}

/*obs bool RBpContextSpec::CheckConfig(Unit* un, Layer* lay, TrialProcess* tp) {
  if(!RBpUnitSpec::CheckConfig(un, lay, tp)) return false;
  if(var_md == NULL) {
    taMisc::Error("RBpContextSpec: could not find variable:",variable,"in RBpUnit type");
    return false;
  }
  RecvCons* recv_gp = (RecvCons*)un->recv.SafeGp(0); // first group
  if(recv_gp == NULL) {
    taMisc::Error("RBpContextSpec: expecting one one-to-one projection from layer",
		   "did not find con group");
    return false;
  }
  Unit* hu = (Unit*)recv_gp->Un(0);
  if(hu == NULL) {
    taMisc::Error("RBpContextSpec: expecting one one-to-one projection from layer",
		   "did not find unit");
    return false;
  }
  return true;
} */

void RBpContextSpec::CopyContext(RBpUnit* u) {
  // todo: checkconfig should test for this!
  RecvCons* recv_gp = (RecvCons*)u->recv.SafeEl(0); // first group
  Unit* hu = (Unit*)recv_gp->Un(0);
  float* varptr = (float*)var_md->GetOff((void*)u);
  *varptr = hysteresis_c * hu->act + hysteresis * (*varptr);
  u->SetExtFlag(unit_flags);
}

void RBpContextSpec::Compute_Act(Unit* u) {
  if(store_states)
    ((RBpUnit*)u)->StoreState();
}

//////////////////////////////////////////
//	Additional Unit Types		//
//////////////////////////////////////////


//////////////////
//    Noisy	//
//////////////////

void NoisyRBpUnitSpec::Initialize() {
  noise.type = Random::GAUSSIAN;
  noise.var = .1f;
  sqrt_dt = sqrtf(dt);
}

void NoisyRBpUnitSpec::UpdateAfterEdit_impl() {
  RBpUnitSpec::UpdateAfterEdit_impl();
  sqrt_dt = sqrtf(dt);
}

void NoisyRBpUnitSpec::Compute_Act_impl(RBpUnit* u) {
  if(time_avg == ACTIVATION) {
    u->act_raw =
      act_range.Project(SigmoidSpec::Clip(sig.Eval(u->net) + noise.Gen()*sqrt_dt));
    u->da = u->act_raw - u->prv_act;
    u->act = u->prv_act + dt * u->da;
  }
  else {
    u->da = u->net - u->prv_net;
    u->net = u->prv_net + dt * u->da;
    u->act = u->act_raw =
      act_range.Project(SigmoidSpec::Clip(sig.Eval(u->net) + noise.Gen()*sqrt_dt));
  }
}


//////////////////////////////////
// 	Bp Network Wizard	//
//////////////////////////////////

void BpWizard::Initialize() {
}

void BpWizard::StdProgs() {
  StdProgs_impl("BpAll_Std");
}

void BpWizard::SRNContext(Network* net) {
  if(net == NULL) {
    taMisc::Error("SRNContext: must have basic constructed network first");
    return;
  }
  OneToOnePrjnSpec* otop = (OneToOnePrjnSpec*)net->FindMakeSpec("CtxtPrjn", &TA_OneToOnePrjnSpec);
  BpContextSpec* ctxts = (BpContextSpec*)net->FindMakeSpec("CtxtUnits", &TA_BpContextSpec);

  if((otop == NULL) || (ctxts == NULL)) {
    return;
  }

  Layer* hidden = net->FindLayer("Hidden");
  Layer* ctxt = net->FindMakeLayer("Context");

  if((hidden == NULL) || (ctxt == NULL)) return;

  ctxt->SetUnitSpec(ctxts);
  ctxt->un_geom = hidden->un_geom;

  net->layers.MoveAfter(hidden, ctxt);
  net->FindMakePrjn(ctxt, hidden, otop); // one-to-one into the ctxt layer
  net->FindMakePrjn(hidden, ctxt); 	 // std prjn back into the hidden from context
  net->Build();
  net->Connect();
}

/* obs bool BpWizard::ToTimeEvents(Environment* env) {
  if(env == NULL) {
    taMisc::Error("ToTimeEvents: requires non-NULL env");
    return false;
  }
  bool changed = false;
  if(env->events.gp.size == 0) {
    TimeEvent_MGroup* eg = (TimeEvent_MGroup*)env->events.gp.New(1, &TA_TimeEvent_MGroup);
    while(env->events.size > 0) {
      eg->Transfer(env->events[0]);
    }
    int nc = eg->ReplaceType(&TA_Event, &TA_TimeEvent);
    tabMisc::WaitProc();
    if(nc > 0) changed = true;
  }
  else {
    int gpi;
    for(gpi=env->events.gp.size-1;gpi>=0;gpi--) {
      Event_MGroup* eg = (Event_MGroup*)env->events.gp[gpi];
      int nc = eg->ReplaceType(&TA_Event, &TA_TimeEvent);
      if(nc > 0) changed = true;
      tabMisc::WaitProc();
      if(!eg->InheritsFrom(&TA_TimeEvent_MGroup)) {
	env->events.gp.ChangeType(gpi, &TA_TimeEvent_MGroup);
	changed = true;
      }
      tabMisc::WaitProc();
    }
  }
  if(!env->InheritsFrom(&TA_TimeEnvironment)) {
    env->ChangeMyType(&TA_TimeEnvironment);
    changed = true;
    tabMisc::WaitProc();
  }
  return changed;
}

void BpWizard::ToRBPEvents(Environment* env, int targ_time) {
  if(env == NULL) {
    taMisc::Error("ToRBPEvents: requires non-NULL env");
    return;
  }
  if(ToTimeEvents(env)) {
    taMisc::Error("ToRBPEvents: Events,Groups,Env were not TimeEvent,_MGroup,Enviro -- they were changed.  Now you need to re-run this function again to setup input/target sequences!");
    return;
  }

  EventSpec* es = (EventSpec*)env->event_specs.SafeEl(0);
  if(es == NULL) {
    taMisc::Error("ToRBPEvents: requires at least one event_spec.");
    return;
  }
  if(es->children.size == 0)
    es->children.EnforceSize(1);
  EventSpec* targ_es = (EventSpec*)es->children[0];
  es->name = "InputOnly";
  targ_es->name = "InputTarget";
  int pi;
  for(pi=0;pi<es->patterns.size;pi++) {
    PatternSpec* pp = (PatternSpec*)es->patterns[pi];
    PatternSpec* cp = (PatternSpec*)targ_es->patterns[pi];
    if(pp->type == PatternSpec::TARGET) {
      cp->SetUnique("type", true);
      cp->type = PatternSpec::TARGET;
      pp->type = PatternSpec::INACTIVE;
    }
  }

  int gpi;
  for(gpi=env->events.gp.size-1;gpi>=0;gpi--) {
    TimeEvent_MGroup* eg = (TimeEvent_MGroup*)env->events.gp[gpi];
    int evi;
    for(evi=eg->size-1;evi>=0;evi--) {
      TimeEvent* ev = (TimeEvent*)eg->FastEl(evi);
      eg->DuplicateEl(ev);
      eg->Move(eg->size-1, evi+1);
      ev->SetSpec(es);
      TimeEvent* te = (TimeEvent*)eg->FastEl(evi+1);
      te->SetSpec(targ_es);
    }
  }

  for(gpi=env->events.gp.size-1;gpi>=0;gpi--) {
    TimeEvent_MGroup* eg = (TimeEvent_MGroup*)env->events.gp[gpi];
    int time = 0;
    int evi;
    for(evi=0;evi<eg->size;evi+=2) {
      TimeEvent* ev = (TimeEvent*)eg->FastEl(evi);
      TimeEvent* te = (TimeEvent*)eg->FastEl(evi+1);
      ev->time = time;
      te->time = time + targ_time;
      time += targ_time + 1;
    }
    eg->end_time = time - (targ_time + 1);
  }
} */



/* -*- C++ -*- */
/*=============================================================================
//									      //
// This file is part of the PDP++ software package.			      //
//									      //
// Copyright (C) 1995 Randall C. O'Reilly, Chadley K. Dawson, 		      //
//		      James L. McClelland, and Carnegie Mellon University     //
//     									      //
// Permission to use, copy, and modify this software and its documentation    //
// for any purpose other than distribution-for-profit is hereby granted	      //
// without fee, provided that the above copyright notice and this permission  //
// notice appear in all copies of the software and related documentation.     //
//									      //
// Permission to distribute the software or modified or extended versions     //
// thereof on a not-for-profit basis is explicitly granted, under the above   //
// conditions. 	HOWEVER, THE RIGHT TO DISTRIBUTE THE SOFTWARE OR MODIFIED OR  //
// EXTENDED VERSIONS THEREOF FOR PROFIT IS *NOT* GRANTED EXCEPT BY PRIOR      //
// ARRANGEMENT AND WRITTEN CONSENT OF THE COPYRIGHT HOLDERS.                  //
// 									      //
// Note that the taString class, which is derived from the GNU String class,  //
// is Copyright (C) 1988 Free Software Foundation, written by Doug Lea, and   //
// is covered by the GNU General Public License, see ta_string.h.             //
// The iv_graphic library and some iv_misc classes were derived from the      //
// InterViews morpher example and other InterViews code, which is             //
// Copyright (C) 1987, 1988, 1989, 1990, 1991 Stanford University             //
// Copyright (C) 1991 Silicon Graphics, Inc.				      //
//									      //
// THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,         //
// EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY 	      //
// WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  	      //
// 									      //
// IN NO EVENT SHALL CARNEGIE MELLON UNIVERSITY BE LIABLE FOR ANY SPECIAL,    //
// INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND, OR ANY DAMAGES  //
// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER OR NOT     //
// ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF LIABILITY,      //
// ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS        //
// SOFTWARE. 								      //
==============================================================================*/

// rbp.cc

#include "rbp.h"
#include "enviro_extra.h"
#include "netstru_extra.h"

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

void RBpUnitSpec::InitLinks() {
  BpUnitSpec::InitLinks();
  taBase::Own(initial_act, this);
}

void RBpUnitSpec::InitState(Unit* u) {
  BpUnitSpec::InitState(u);
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

void RBpUnitSpec::UpdateAfterEdit() {
  BpUnitSpec::UpdateAfterEdit();
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
  Con_Group* recv_gp;
  int g;
  FOR_ITR_GP(Con_Group, recv_gp, ru->recv., g) {
    Layer* fmlay = recv_gp->prjn->from;
    if(fmlay->lesion || !(fmlay->ext_flag & Unit::EXT))
      continue;		// don't get from the non-clamped layers!
    ru->clmp_net += recv_gp->Compute_Net(ru);
  }
  if(ru->bias != NULL)
    ru->clmp_net += ru->bias->wt;
}

void RBpUnitSpec::Compute_Net(Unit* u) {
  RBpUnit* ru = (RBpUnit*)u;
  ru->prv_net = ru->net; // save current net as previous
  if(fast_hard_clamp_net) {
    Con_Group* recv_gp;
    ru->net = ru->clmp_net;
    int g;
    FOR_ITR_GP(Con_Group, recv_gp, u->recv., g) {
      Layer* fmlay = recv_gp->prjn->from;
      if(fmlay->lesion || (fmlay->ext_flag & Unit::EXT))
	continue;		// don't get from the clamped layers
      u->net += recv_gp->Compute_Net(u);
    }
  }
  else {
    BpUnitSpec::Compute_Net(u);
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
  ((BpConSpec*)bias_spec.spec)->B_Compute_dWt((BpCon*)u->bias, (BpUnit*)u);
}

void RBpUnitSpec::UpdateWeights(Unit* u) {
  if((u->ext_flag & Unit::EXT) && !soft_clamp && !updt_clamped_wts) return; // don't update for clamped units
  UnitSpec::UpdateWeights(u);
  ((BpConSpec*)bias_spec.spec)->B_UpdateWeights((BpCon*)u->bias, (BpUnit*)u);
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

void RBpUnit::InitExterns() {
  prv_ext_flag = ext_flag;
  prv_targ = targ;
  prv_ext = ext;
  BpUnit::InitExterns();
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
//  	RBpTrial	//
//////////////////////////

void RBpTrial::Initialize() {
  time = 0.0f;
  dt = 1.0f;
  time_window = 10;
  bp_gap = 1;
  real_time = false;
  time_win_ticks = 10;
  bp_gap_ticks = 1;
  bp_performed = false;
  log_counter = true;

  min_unit = &TA_RBpUnit;
  min_con_group = &TA_BpCon_Group;
  min_con = &TA_BpCon;
}

void RBpTrial::InitLinks() {
  TrialProcess::InitLinks();
  if(!taMisc::is_loading && (final_stats.size == 0)) {
    RBpSE_Stat* st = (RBpSE_Stat*)final_stats.NewEl(1, &TA_RBpSE_Stat);
    st->CreateAggregates(Aggregate::SUM);
  }
}

void RBpTrial::UpdateAfterEdit() {
  BpTrial::UpdateAfterEdit();
  GetUnitBufSize(false);
  time_win_ticks = (int)ceil((float)time_window / dt);
  bp_gap_ticks = (int)ceil((float)bp_gap / dt);
}

void RBpTrial::Compute_ClampExt() {
  Layer* lay;
  taLeafItr l_itr;
  FOR_ITR_EL(Layer, lay, network->layers., l_itr) {
    if(lay->lesion || (~lay->ext_flag & Unit::EXT)) continue;
    RBpUnit* u;
    taLeafItr u_itr;
    FOR_ITR_EL(RBpUnit, u, lay->units., u_itr) {
      u->Compute_ClampExt();
    }
  }
}

void RBpTrial::Compute_Act() {
  network->Compute_Net();	// two-stage update of nets and acts
  network->Compute_Act();
}

void RBpTrial::Compute_dEdA_dEdNet() {
  // send the error back in two stages, first dEdA, then dEdNet
  Layer* lay;
  taLeafItr l_itr;
  FOR_ITR_EL(Layer, lay, network->layers., l_itr) {
    if(lay->lesion) continue;
    RBpUnit* u;
    taLeafItr u_itr;
    FOR_ITR_EL(RBpUnit, u, lay->units., u_itr) {
      u->Compute_dEdA();
    }
  }
#ifdef DMEM_COMPILE
  network->dmem_share_units.Aggregate(3, MPI_SUM);
#endif

  FOR_ITR_EL(Layer, lay, network->layers., l_itr) {
    if(lay->lesion) continue;
    RBpUnit* u;
    taLeafItr u_itr;
    FOR_ITR_EL(RBpUnit, u, lay->units., u_itr) {
      u->Compute_Error();
      u->Compute_dEdNet();
    }
  }
}

void RBpTrial::StoreState() {
  Layer* lay;
  taLeafItr l_itr;
  FOR_ITR_EL(Layer, lay, network->layers., l_itr) {
    if(lay->lesion) continue;
    RBpUnit* u;
    taLeafItr u_itr;
    FOR_ITR_EL(RBpUnit, u, lay->units., u_itr) {
      u->StoreState();
    }
  }
}

void RBpTrial::InitForBP() {
  Layer* lay;
  taLeafItr l_itr;
  FOR_ITR_EL(Layer, lay, network->layers., l_itr) {
    if(lay->lesion) continue;
    RBpUnit* u;
    taLeafItr u_itr;
    FOR_ITR_EL(RBpUnit, u, lay->units., u_itr) {
      u->InitForBP();
    }
  }
}

void RBpTrial::StepBack(int tick) {
  // step back at given point in time
  Layer* lay;
  taLeafItr l_itr;
  FOR_ITR_EL(Layer, lay, network->layers., l_itr) {
    if(lay->lesion) continue;
    RBpUnit* u;
    taLeafItr u_itr;
    FOR_ITR_EL(RBpUnit, u, lay->units., u_itr) {
      u->StepBack(tick);
    }
  }
}

void RBpTrial::RestoreState(int tick) {
  // restore state to values at given point in time
  Layer* lay;
  taLeafItr l_itr;
  FOR_ITR_EL(Layer, lay, network->layers., l_itr) {
    if(lay->lesion) continue;
    RBpUnit* u;
    taLeafItr u_itr;
    FOR_ITR_EL(RBpUnit, u, lay->units., u_itr) {
      u->RestoreState(tick);
    }
  }
}

void RBpTrial::CopyContext() {
  Layer* lay;
  taLeafItr l_itr;
  FOR_ITR_EL(Layer, lay, network->layers., l_itr) {
    if(lay->lesion || !lay->unit_spec.spec->InheritsFrom(TA_RBpContextSpec))
      continue;

    RBpUnit* u;
    taLeafItr u_itr;
    FOR_ITR_EL(RBpUnit, u, lay->units., u_itr) {
      ((RBpContextSpec*)u->spec.spec)->CopyContext(u);
    }
  }
}

void RBpTrial::ResetStored() {
  Layer* lay;
  taLeafItr l_itr;
  FOR_ITR_EL(Layer, lay, network->layers., l_itr) {
    if(lay->lesion)
      continue;

    RBpUnit* u;
    taLeafItr u_itr;
    FOR_ITR_EL(RBpUnit, u, lay->units., u_itr) {
      u->ResetStored();
    }
  }
}

void RBpTrial::ShiftBuffers() {
  // shift buffers by bp_gap
  Layer* lay;
  taLeafItr l_itr;
  FOR_ITR_EL(Layer, lay, network->layers., l_itr) {
    if(lay->lesion) continue;
    RBpUnit* u;
    taLeafItr u_itr;
    FOR_ITR_EL(RBpUnit, u, lay->units., u_itr) {
      u->ShiftBuffers(bp_gap_ticks);
    }
  }
}

int RBpTrial::GetUnitBufSize(bool in_updt_after) {
  if(network == NULL) return -1;
  Layer* lay;
  taLeafItr l_itr;
  FOR_ITR_EL(Layer, lay, network->layers., l_itr) {
    if((lay->lesion) || (lay->units.leaves == 0))
      continue;
    RBpUnit* u = (RBpUnit*)lay->units.Leaf(0);
    RBpUnitSpec* us = (RBpUnitSpec*)u->spec.spec;
    if(us->dt != dt) {
      dt = us->dt;
      taMisc::Error("RBpTrial: Copying dt from unit spec:", (String)dt);
      if(!in_updt_after)
	UpdateAfterEdit();	// get new tick values
    }
    return u->ext_flags.length;
  }
  return -1;
}

bool RBpTrial::CheckNetwork() {
  if(network && (network->dmem_sync_level != Network::DMEM_SYNC_NETWORK)) {
    network->dmem_sync_level = Network::DMEM_SYNC_NETWORK;
  }
  return TrialProcess::CheckNetwork();
}

bool RBpTrial::CheckUnit(Unit* ck) {
  bool rval = BpTrial::CheckUnit(ck);
  if(!rval) return rval;
  RBpUnitSpec* usp = (RBpUnitSpec*)ck->spec.spec;
  if(usp == NULL)	return true;
  if(!usp->store_states) {
    int chs = taMisc::Choice
      ("Units should be storing their activity states for learning, but are not currently",
       "Switch to store states", "Leave not storing states");
    if(chs == 0)
      usp->store_states = true;
  }
  return true;
}

bool RBpTrial::PerformBP() {
  bp_performed = false;
  int buf_sz = GetUnitBufSize();
  if(buf_sz < 0)
    return false;

  if(buf_sz < time_win_ticks+1)
    return false;

  PerformBP_impl();
  bp_performed = true;
  return true;
}

void RBpTrial::PerformBP_impl() {
  InitForBP();			// clear current and prev error values
  int buf_sz = GetUnitBufSize();
  int i;
  for(i=buf_sz-2; i>=0; i--) {
    Compute_dEdA_dEdNet();
    Compute_dWt();
    if(i > 0)
      StepBack(i-1);		// step back to previous time
  }
  RestoreState(buf_sz-1);	// restore activations to end values
  if(real_time)
    ShiftBuffers();		// no need to shift buffers when not in real time mode
}

void RBpTrial::Init_impl() {
  BpTrial::Init_impl();
  bp_performed = false;
}

void RBpTrial::Loop() {
  if(network == NULL) return;

  SetCurLrate();

  if(cur_event != NULL) {	// an explicit reset is required to define new pats
    network->InitExterns();	// initialize external flags, etc
    cur_event->ApplyPatterns(network);
  }

  int buf_sz = GetUnitBufSize();
  if(buf_sz == 0) {		// units were just reset, time has just started
    time = 0;
    Compute_ClampExt();		// make sure clamped external input shows up in act
    StoreState();		// store the initial state
    UpdateLogs();		// generate log output for initial state
  }

  Compute_Act();

  if((epoch_proc != NULL) && (epoch_proc->wt_update != EpochProcess::TEST)) {
    PerformBP();		// peform bp if necessary
  }

  time += dt;			// increment time
}

void RBpTrial::GetCntrDataItems() {
  if(cntr_items.size < 3)
    cntr_items.EnforceSize(3);
  SchedProcess::GetCntrDataItems();
  DataItem* it = (DataItem*)cntr_items.FastEl(1);
  it->SetNarrowName("Time");
  it = (DataItem*)cntr_items.FastEl(2);
  it->SetNarrowName("BP");
}

void RBpTrial::GenCntrLog(LogData* ld, bool gen) {
  BpTrial::GenCntrLog(ld, gen);
  if(gen) {
    if(cntr_items.size < 3)
      GetCntrDataItems();
    ld->AddFloat(cntr_items.FastEl(0), time);
    ld->AddFloat(cntr_items.FastEl(1), (float)bp_performed);
  }
}



//////////////////////////
//  	RBpSequence	//
//////////////////////////

void RBpSequence::Initialize() {
  sub_proc_type = &TA_RBpTrial;
  min_unit = &TA_RBpUnit;
  min_con_group = &TA_BpCon_Group;
  min_con = &TA_BpCon;
}

void RBpSequence::Init_impl() {
  SchedProcess::Init_impl();

  if((environment == NULL) || (network == NULL) || (sequence_epoch == NULL))
    return;
  taBase::SetPointer((TAPtr*)&cur_event_gp, sequence_epoch->cur_event_gp);
  RBpTrial* trl = (RBpTrial*)FindSubProc(&TA_RBpTrial);
  if((trl == NULL) || (cur_event_gp == NULL))
    return;

  tick.val = 0;
  GetEventList();
  GetCurEvent();
  InitNetState();
}

int RBpSequence::GetMaxTick(float& last_time) {
  if(cur_event_gp == NULL) return 0;
  if(!cur_event_gp->InheritsFrom(TA_TimeEvent_MGroup))
    return cur_event_gp->EventCount();

  RBpTrial* trl = (RBpTrial*)FindSubProc(&TA_RBpTrial);
  if((trl == NULL) || (cur_event_gp == NULL))
    return 0;

  if(cur_event_gp->EventCount() == 0)
    return 0;

  last_time = 0.0f;
  // use last event's time as a maximum
  if(cur_event_gp->InheritsFrom(TA_TimeEvent_MGroup)) {
    last_time = ((TimeEvent_MGroup*)cur_event_gp)->end_time;
  }
  else {
    TimeEvent* te = (TimeEvent*)cur_event_gp->GetEvent(cur_event_gp->EventCount()-1);
    if(te->InheritsFrom(TA_TimeEvent))
      last_time = te->time;
  }
  return (int)ceil(last_time / trl->dt);
}

void RBpSequence::GetEventList() {
  event_list.Reset();
  RBpTrial* trl = (RBpTrial*)FindSubProc(&TA_RBpTrial);
  // nothing fancy if not a time group
  if((trl == NULL) || !cur_event_gp->InheritsFrom(TA_TimeEvent_MGroup)) {
    SequenceProcess::GetEventList();
    return;
  }
  float last_time = 0.0f;
  tick.max = GetMaxTick(last_time);

  int i;
  for(i=0; i<tick.max; i++)
    event_list.Add(i);
  if(order == PERMUTED)
    event_list.Permute();

  // set trial's time window based on length of current sequence
  if(!trl->real_time) {
    trl->time_window = last_time;
    trl->UpdateAfterEdit();
  }
}

void RBpSequence::GetCurEvent() {
  if((cur_event_gp == NULL) || (sequence_epoch == NULL))
    return;
  RBpTrial* trl = (RBpTrial*)FindSubProc(&TA_RBpTrial);
  if(!cur_event_gp->InheritsFrom(TA_TimeEvent_MGroup) || (trl == NULL)) {
    SequenceProcess::GetCurEvent();
    return;
  }

  float last_time = 0.0f;
  if(tick.max != GetMaxTick(last_time)) {
    GetEventList();
    if(tick.val >= tick.max) {
      taBase::SetPointer((TAPtr*)&cur_event, NULL);
      return;
    }
  }

  TimeEvent* ev = NULL;
  if((order == SEQUENTIAL) || (order == PERMUTED)) {
    float time = (float)(event_list[tick.val] + 1) * trl->dt;
    ev = ((TimeEvent_MGroup*)cur_event_gp)->GetTimeEvent(time);
  }
  else {
    int evnt_no = 1 + Random::IntZeroN(tick.max);
    float time = (float)evnt_no * trl->dt;
    ev = ((TimeEvent_MGroup*)cur_event_gp)->GetTimeEvent(time);
  }

  taBase::SetPointer((TAPtr*)&cur_event, ev);

  // copy cur_event to epoch so that trial process can access it there..
  taBase::SetPointer((TAPtr*)&(sequence_epoch->cur_event), cur_event);
}

void RBpSequence::Loop() {
  if((cur_event_gp == NULL) || (sequence_epoch == NULL))
    return;
  float last_time = 0.0f;
  if(tick.max != GetMaxTick(last_time)) {
    GetEventList();
    if(tick.val >= tick.max) {
      taBase::SetPointer((TAPtr*)&cur_event, NULL);
      return;
    }
    GetCurEvent();
  }
  if(sub_proc != NULL)
    sub_proc->Run();
  if(!bailing) {
    if(sequence_epoch->wt_update == EpochProcess::ON_LINE)
      network->UpdateWeights();
  }
}


//////////////////////////
// 	RBpSE_Stat	//
//////////////////////////

void RBpSE_Stat::Initialize() {
  min_unit = &TA_RBpUnit;
}

void RBpSE_Stat::Network_Stat() {
  if(network == NULL) return;
  Layer* lay;
  taLeafItr l_itr;
  FOR_ITR_EL(Layer, lay, network->layers., l_itr) {
    if((lay->lesion) || (lay->units.leaves == 0))
      continue;
    RBpUnit* u = (RBpUnit*)lay->units.Leaf(0);
    se.val *= ((RBpUnitSpec*)u->spec.spec)->dt; // find dt and multiply..
    return;
  }
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

void RBpContextSpec::Copy_(const RBpContextSpec& cp) {
  hysteresis = cp.hysteresis;
  hysteresis_c = cp.hysteresis_c;
  variable = cp.variable;
  unit_flags = cp.unit_flags;
}

void RBpContextSpec::UpdateAfterEdit() {
  RBpUnitSpec::UpdateAfterEdit();
  hysteresis_c = 1.0f - hysteresis;
  var_md = TA_RBpUnit.members.FindName(variable);
  if(var_md == NULL)
    taMisc::Error("RBpContextSpec: could not find variable:",variable,"in RBpUnit type");
}

bool RBpContextSpec::CheckConfig(Unit* un, Layer* lay, TrialProcess* tp) {
  if(!RBpUnitSpec::CheckConfig(un, lay, tp)) return false;
  if(var_md == NULL) {
    taMisc::Error("RBpContextSpec: could not find variable:",variable,"in RBpUnit type");
    return false;
  }
  Con_Group* recv_gp = (Con_Group*)un->recv.SafeGp(0); // first group
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
}

void RBpContextSpec::CopyContext(RBpUnit* u) {
  Con_Group* recv_gp = (Con_Group*)u->recv.SafeGp(0); // first group
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
//	Almeida-Pineda Algorithm	//
//////////////////////////////////////////


//////////////////////////
// 	CycleProcess	//
//////////////////////////

void APBpCycle::Initialize() {
  sub_proc_type = NULL;
  apbp_settle = NULL;
  apbp_trial = NULL;

  min_unit = &TA_RBpUnit;
  min_con_group = &TA_BpCon_Group;
  min_con = &TA_BpCon;
}

void APBpCycle::CutLinks() {
  CycleProcess::CutLinks();
  apbp_settle = NULL;
  apbp_trial = NULL;
}

void APBpCycle::UpdateAfterEdit() {
  CycleProcess::UpdateAfterEdit();
  apbp_settle = (APBpSettle*)FindSuperProc(&TA_APBpSettle);
  apbp_trial = (APBpTrial*)FindSuperProc(&TA_APBpTrial);
}

void APBpCycle::Compute_Act() {
  network->Compute_Net();	// two-stage update of nets and acts
  network->Compute_Act();
}

void APBpCycle::Compute_Error() {
  // compute errors
  Layer* lay;
  taLeafItr l_itr;
  FOR_ITR_EL(Layer, lay, network->layers., l_itr) {
    if(lay->lesion || !(lay->ext_flag & Unit::TARG)) // only compute err on targs
      continue;

    BpUnit* u;
    taLeafItr u_itr;
    FOR_ITR_EL(BpUnit, u, lay->units., u_itr) {
      u->dEdA = 0.0f;
      u->Compute_Error();
    }
  }
}

void APBpCycle::Compute_dEdA_dEdNet() {
  // send the error back in two stages, first dEdA, then dEdNet
  Layer* lay;
  taLeafItr l_itr;
  FOR_ITR_EL(Layer, lay, network->layers., l_itr) {
    if(lay->lesion) continue;
    RBpUnit* u;
    taLeafItr u_itr;
    FOR_ITR_EL(RBpUnit, u, lay->units., u_itr) {
      u->Compute_dEdA();
    }
  }
#ifdef DMEM_COMPILE
  network->dmem_share_units.Aggregate(3, MPI_SUM);
#endif

  FOR_ITR_EL(Layer, lay, network->layers., l_itr) {
    if(lay->lesion) continue;
    RBpUnit* u;
    taLeafItr u_itr;
    FOR_ITR_EL(RBpUnit, u, lay->units., u_itr) {
      u->Compute_Error();
      u->Compute_dEdNet();
    }
  }
}

void APBpCycle::Loop() {
  if((apbp_settle == NULL) || (apbp_trial == NULL))	return;
  if(apbp_trial->phase == APBpTrial::ACT_PHASE)
    Compute_Act();
  else
    Compute_dEdA_dEdNet();
}

//////////////////////////
// 	SettleProcess	//
//////////////////////////

void APBpSettle::Initialize() {
  sub_proc_type = &TA_APBpCycle;
  apbp_trial = NULL;
  cycle.max = 50;

  min_unit = &TA_RBpUnit;
  min_con_group = &TA_BpCon_Group;
  min_con = &TA_BpCon;
}

void APBpSettle::InitLinks() {
  SettleProcess::InitLinks();
  if(!taMisc::is_loading && (loop_stats.size == 0))
    loop_stats.NewEl(1, &TA_APBpMaxDa_De);
}

void APBpSettle::CutLinks() {
  SettleProcess::CutLinks();
  apbp_trial = NULL;
}

void APBpSettle::UpdateAfterEdit() {
  SettleProcess::UpdateAfterEdit();
  apbp_trial = (APBpTrial*)FindSuperProc(&TA_APBpTrial);
}

void APBpSettle::Compute_ClampExt() {
  Layer* lay;
  taLeafItr l_itr;
  FOR_ITR_EL(Layer, lay, network->layers., l_itr) {
    if((lay->lesion) || !(lay->ext_flag & Unit::EXT))
      continue;

    RBpUnit* u;
    taLeafItr u_itr;
    FOR_ITR_EL(RBpUnit, u, lay->units., u_itr)
      u->Compute_ClampExt();
  }
}

void APBpSettle::Compute_HardClampNet() {
  Layer* lay;
  taLeafItr l_itr;
  FOR_ITR_EL(Layer, lay, network->layers., l_itr) {
    if(lay->lesion) continue;
    RBpUnit* u;
    taLeafItr u_itr;
    FOR_ITR_EL(RBpUnit, u, lay->units., u_itr)
      u->Compute_HardClampNet();
  }
}

void APBpSettle::Init_impl() {
  SettleProcess::Init_impl();
  if((apbp_trial == NULL) || (network == NULL))
    return;

  if((apbp_trial->cur_event != NULL) && apbp_trial->cur_event->InheritsFrom(TA_DurEvent))
    cycle.SetMax((int)((DurEvent*)apbp_trial->cur_event)->duration);

  if(apbp_trial->phase_no == 1) {	// bp phase
  }
  else {
    network->InitExterns();
    if(apbp_trial->cur_event != NULL)
      apbp_trial->cur_event->ApplyPatterns(network);
    Compute_ClampExt();		// make sure clamped external input shows up in act
    Compute_HardClampNet();
  }
}

//////////////////////////
// 	TrialProcess	//
//////////////////////////

void APBpTrial::Initialize() {
  sub_proc_type = &TA_APBpSettle;
  phase = ACT_PHASE;
  phase_no.SetMax(2);
  no_bp_stats = true;
  no_bp_test = true;
  trial_init = INIT_STATE;

  min_unit = &TA_RBpUnit;
  min_con_group = &TA_BpCon_Group;
  min_con = &TA_BpCon;
}

void APBpTrial::InitLinks() {
  TrialProcess::InitLinks();
  taBase::Own(phase_no, this);
  if(!taMisc::is_loading && (final_stats.size == 0)) {
    SE_Stat* st = (SE_Stat*)final_stats.NewEl(1, &TA_SE_Stat);
    st->CreateAggregates(Aggregate::SUM);
  }
}

void APBpTrial::Copy_(const APBpTrial& cp) {
  phase_no = cp.phase_no;
  phase = cp.phase;
  trial_init = cp.trial_init;
  no_bp_stats = cp.no_bp_stats;
  no_bp_test = cp.no_bp_test;
}

void APBpTrial::C_Code() {
  bool stop_crit = false;	// a stopping criterion was reached
  bool stop_force = false;	// either the Stop button was pressed or our Step level was reached

  if(re_init) {
    Init();
    InitProcs();
  }

  if((cur_event != NULL) && (cur_event->InheritsFrom(TA_DurEvent)) &&
     (((DurEvent*)cur_event)->duration <= 0)) {
    SetReInit(true);
    return;
  }

  do {
    Loop();			// user defined code goes here
    if(!bailing) {
      UpdateCounters();
      LoopProcs();		// check/run loop procs (using mod based on counter)
      if(!no_bp_stats || (phase == ACT_PHASE)) {
	LoopStats();		// udpate in-loop stats
	if(log_loop)
	  UpdateLogs();		// generate log output
      }
      UpdateState();		// update process state vars

      stop_crit = Crit();
      if(!stop_crit) {		// if at critera, going to quit anyway, so don't
	stop_force = StopCheck();// check for stopping (either by Stop button or Step)
      }
    }
  }
  while(!bailing && !stop_crit && !stop_force);
  // loop until we reach criterion (e.g. ctr > max) or are forcibly stopped

  if(stop_crit) {
    Final();
    FinalProcs();
    FinalStats();
    if(!log_loop)
      UpdateLogs();
    UpdateDisplays();		// update displays after the loop
    SetReInit(true);		// made it through the loop
    FinalStepCheck();		// always stop at end if i'm the step process
  }
  else {
    bailing = true;
  }
}

void APBpTrial::Init_impl() {
  if(network == NULL)	return;
  TrialProcess::Init_impl();
  phase_no.SetMax(2);
  bool is_testing = false;
  if(no_bp_test && (epoch_proc != NULL) &&
     (epoch_proc->wt_update == EpochProcess::TEST))
  {
    phase_no.SetMax(1);		// just do one loop (the minus phase)
    is_testing = true;
  }
  phase = ACT_PHASE;

  if(trial_init == INIT_STATE)
    network->InitState();

  SetCurLrate();
}

void APBpTrial::UpdateState() {
  TrialProcess::UpdateState();
  phase = BP_PHASE;
}

void APBpTrial::SetCurLrate() {
  Layer* lay;
  taLeafItr l_itr;
  FOR_ITR_EL(Layer, lay, network->layers., l_itr) {
    if(lay->lesion)	continue;
    BpUnit* u;
    taLeafItr u_itr;
    FOR_ITR_EL(BpUnit, u, lay->units., u_itr)
      u->SetCurLrate(network->epoch);
  }
}

void APBpTrial::Compute_dWt() {
  if(network == NULL)	return;
  network->Compute_dWt();
}

void APBpTrial::Final() {
  TrialProcess::Final();
  if((epoch_proc != NULL) &&
     (epoch_proc->wt_update != EpochProcess::TEST))
    Compute_dWt();
}

bool APBpTrial::CheckNetwork() {
  if(network && (network->dmem_sync_level != Network::DMEM_SYNC_NETWORK)) {
    network->dmem_sync_level = Network::DMEM_SYNC_NETWORK;
  }
  return TrialProcess::CheckNetwork();
}

bool APBpTrial::CheckUnit(Unit* ck) {
  bool rval = TrialProcess::CheckUnit(ck);
  if(!rval) return rval;
  RBpUnitSpec* usp = (RBpUnitSpec*)ck->spec.spec;
  if(usp == NULL)	return true;
  if(usp->store_states) {
    int chs = taMisc::Choice
      ("Units do not need to store their activity states for learning, but are",
       "Switch not store states", "Leave storing states");
    if(chs == 0)
      usp->store_states = false;
  }
  return true;
}

void APBpTrial::GetCntrDataItems() {
  if(cntr_items.size < 2)
    cntr_items.EnforceSize(2);
  SchedProcess::GetCntrDataItems();
  DataItem* it = (DataItem*)cntr_items.FastEl(1);
  it->SetNarrowName("Phase");
  it->is_string = true;
}

void APBpTrial::GenCntrLog(LogData* ld, bool gen) {
  TrialProcess::GenCntrLog(ld, gen);
  if(gen) {
    if(cntr_items.size < 2)
      GetCntrDataItems();
    if(phase == ACT_PHASE)
      ld->AddString(cntr_items.FastEl(1), "act");
    else
      ld->AddString(cntr_items.FastEl(1), "bp");
  }
}


//////////////////////////
// 	Stats		//
//////////////////////////

void APBpMaxDa_De::Initialize() {
  net_agg.op = Aggregate::MAX;
  da_de.stopcrit.flag = true;	// defaults
  da_de.stopcrit.val = .01f;
  da_de.stopcrit.cnt = 1;
  da_de.stopcrit.rel = CritParam::LESSTHANOREQUAL;
  has_stop_crit = true;
  net_agg.op = Aggregate::MAX;
  loop_init = INIT_START_ONLY;

  min_unit = &TA_RBpUnit;
}

void APBpMaxDa_De::InitStat() {
  da_de.InitStat(InitStatVal());
  InitStat_impl();
}

void APBpMaxDa_De::Init() {
  da_de.Init();
  Init_impl();
}

bool APBpMaxDa_De::Crit() {
  if(!has_stop_crit)    return false;
  if(n_copy_vals > 0)   return copy_vals.Crit();
  return da_de.Crit();
}

void APBpMaxDa_De::Network_Init() {
  InitStat();
}

void APBpMaxDa_De::Unit_Stat(Unit* unit) {
  float fda = fabsf(((RBpUnit*)unit)->da);
  net_agg.ComputeAgg(&da_de, fda);
  fda = fabsf(((RBpUnit*)unit)->ddE); // do both, one will be below, so its ok..
  net_agg.ComputeAgg(&da_de, fda);
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

void NoisyRBpUnitSpec::UpdateAfterEdit() {
  RBpUnitSpec::UpdateAfterEdit();
  sqrt_dt = sqrtf(dt);
}

void NoisyRBpUnitSpec::InitLinks() {
  RBpUnitSpec::InitLinks();
  taBase::Own(noise, this);
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

void BpWizard::SRNContext(Network* net) {
  if(net == NULL) {
    taMisc::Error("SRNContext: must have basic constructed network first");
    return;
  }
  Project* proj = GET_MY_OWNER(Project);
  OneToOnePrjnSpec* otop = (OneToOnePrjnSpec*)pdpMisc::FindMakeSpec(proj, "CtxtPrjn", &TA_OneToOnePrjnSpec);
  BpContextSpec* ctxts = (BpContextSpec*)pdpMisc::FindMakeSpec(proj, "CtxtUnits", &TA_BpContextSpec);

  if((otop == NULL) || (ctxts == NULL)) {
    return;
  }

  Layer* hidden = net->FindLayer("Hidden");
  Layer* ctxt = net->FindMakeLayer("Context");

  if((hidden == NULL) || (ctxt == NULL)) return;

  ctxt->SetUnitSpec(ctxts);
  ctxt->n_units = hidden->n_units;
  ctxt->geom = hidden->geom;

  net->layers.MoveAfter(hidden, ctxt);
  net->FindMakePrjn(ctxt, hidden, otop); // one-to-one into the ctxt layer
  net->FindMakePrjn(hidden, ctxt); 	 // std prjn back into the hidden from context
  net->Build();
  net->Connect();
}

bool BpWizard::ToTimeEvents(Environment* env) {
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
}



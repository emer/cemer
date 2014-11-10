// Copyright, 1995-2013, Regents of the University of Colorado,
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

#include "LeabraThreadMgr.h"

#include <LeabraNetwork>
#include <DataTable>
#include <Program>

#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(LeabraTask);
TA_BASEFUNS_CTORS_DEFN(RunWaitTime);
TA_BASEFUNS_CTORS_DEFN(RunWaitTime_List);
TA_BASEFUNS_CTORS_DEFN(LeabraThreadMgr);

#ifndef Q_ATOMIC_INT_FETCH_AND_ADD_IS_ALWAYS_NATIVE
#warning "thread optimization warning: int fetch add is not native!"
#endif

#ifndef Q_ATOMIC_INT_FETCH_AND_ADD_IS_WAIT_FREE
#warning "thread optimization warning: int fetch add is not wait free!"
#endif


String RunWaitTime::ReportAvg(float rescale) {
  String rval;// = name;
  rval << " run avg: " << (rescale * run.avg_used.avg)
       << ", sum: " << run.avg_used.sum
       << ", wait avg: " << (rescale * wait.avg_used.avg)
       << ", sum: " << wait.avg_used.sum;
  return rval;
}

void RunWaitTime_List::ResetUsed(int st_idx, int n) {
  if(st_idx >= size) return;
  int mx = MIN(size, st_idx+n);
  for(int i=st_idx; i<mx; i++) {
    FastEl(i)->ResetUsed();
  }
}

void LeabraTask::Initialize() {
}

void LeabraTask::Destroy() {
  network.CutLinks();
}

void LeabraTask::EndStep(QAtomicInt& stage, RunWaitTime& time, int cyc) {
  LeabraThreadMgr* mg = mgr();
  const bool timers_on = mg->timers_on;
  if(timers_on) {
    time.EndRun();
  }
  int trg = (cyc+1) * mg->tasks.size;

  // acquire vs. ordered is not really a big deal here -- generating a release is
  // essentially automatic whenever a volitile variable is written to anyway
  int cur_cnt = stage.fetchAndAddOrdered(1); // write should be acquire based like lock?
  if(cur_cnt == trg) { // we were the last guy
    return;
  }

  if(timers_on) {
    time.StartWait(false);
  }

  while(cur_cnt < trg) {
    // taManagedThread::usleep(1); // just slows down a tiny bit, no value..
#if (QT_VERSION >= 0x050000)
    cur_cnt = stage.loadAcquire();
#else
    cur_cnt = (int)stage;       // should be ordered semantics??
#endif
  }

  if(timers_on) {
    time.EndWait();
  }
}

// fixed allocation based on thread id -- less flexible..
// performance is close to the nibble code, but likely much less robust
// to load differences etc
void LeabraTask::RunUnitsFixedAlloc(LeabraThreadUnitCall& unit_call) {
  LeabraThreadMgr* mg = mgr();
  LeabraNetwork* net = (LeabraNetwork*)network.ptr();

  const int n_task = mg->tasks.size;
  const int chk = mg->unit_chunks;
  const int un_st = (task_id * chk) + 1; // 1 offset
  const int un_inc = ((n_task-1) * chk);
  const int un_mx = net->units_flat.size;
  int i;
  int ci;

  // 1 2 3 4 5 6 7 8 9 10 11 12
  // 0 0 0             0  0  0
  //       1 1 1 
  //             2 2 2

  i = un_st; ci = 0;
  while(i<un_mx) {
    LeabraUnit* un = (LeabraUnit*)net->units_flat[i];
    unit_call.call(un, net, task_id); // task id indicates threading, and which thread
    i++; ci++;
    if(ci == chk) {
      i += un_inc; ci = 0;
    }
  }
}

// dynamic allocation with nibbling
void LeabraTask::RunUnits(LeabraThreadUnitCall& unit_call, QAtomicInt& lpidx) {
  LeabraThreadMgr* mg = mgr();
  LeabraNetwork* net = (LeabraNetwork*)network.ptr();

  const int chk = mg->unit_chunks;
  const int un_mx = net->units_flat.size;

  while(true) {
    const int nxt_uidx = lpidx.fetchAndAddOrdered(chk);
    if(nxt_uidx >= un_mx)
      break;
    const int mx = MIN(un_mx, nxt_uidx + chk);
    for(int i=nxt_uidx; i <mx; i++) {
      LeabraUnit* un = (LeabraUnit*)net->units_flat[i];
      unit_call.call(un, net, task_id); // task id indicates threading, and which thread
    }
    if(mx == un_mx) break;
  }
}

void LeabraTask::RunUnitsStep(LeabraThreadUnitCall& unit_call, QAtomicInt& lpidx,
                              QAtomicInt& stage, RunWaitTime& time, int cyc,
                              bool con_level, bool reset_used) {
  LeabraThreadMgr* mg = mgr();
  const bool timers_on = mg->timers_on;
  if(timers_on) {
    if(reset_used)
      time.ResetUsed();
    StartTime(time);
  }
  if(!con_level && mg->unit_fixed) {
    RunUnitsFixedAlloc(unit_call);
  }
  else {
    RunUnits(unit_call, lpidx);
  }
  EndStep(stage, time, cyc);

  if(timers_on)
    time.IncrAvg();
}

void LeabraTask::RunUnitsTime(LeabraThreadUnitCall& unit_call, QAtomicInt& lpidx,
                              RunWaitTime& time, bool reset_used) {
  LeabraThreadMgr* mg = mgr();
  const bool timers_on = mg->timers_on;
  if(timers_on) {
    if(reset_used)
      time.ResetUsed();
    StartTime(time);
  }
  RunUnits(unit_call, lpidx);
  if(timers_on) {
    EndTime(time);
    time.IncrAvg();           // resets always incr
  }
}

void LeabraTask::RunLayers(LeabraThreadLayerCall& layer_call, QAtomicInt& lpidx) {
  LeabraThreadMgr* mg = mgr();
  LeabraNetwork* net = (LeabraNetwork*)network.ptr();
  const int n_task = mg->tasks.size;

  // const int n_lays = net->active_layer_idx.size;
  // for(int i=task_id; i<n_lays; i+= n_task) {
  //   const int li = net->active_layer_idx[i];
  //   LeabraLayer* lay = (LeabraLayer*)net->layers.Leaf(li);
  //   layer_call.call(lay, net, task_id);
  // }

  const int n_lays = net->active_layer_idx.size + 1;
  // because of units starting at 1, have to offset by 1 and then subtract below

  while(true) {
    const int nxt_lidx = lpidx.fetchAndAddOrdered(1);
    if(nxt_lidx >= n_lays)
      break;
    const int li = net->active_layer_idx[nxt_lidx-1]; // sub 1
    LeabraLayer* lay = (LeabraLayer*)net->layers.Leaf(li);
    layer_call.call(lay, net, task_id);
  }
}

void LeabraTask::RunLayersStep(LeabraThreadLayerCall& layer_call, QAtomicInt& lpidx,
                               QAtomicInt& stage, RunWaitTime& time, int cyc,
                               bool reset_used) {
  LeabraThreadMgr* mg = mgr();
  const bool timers_on = mg->timers_on;
  if(timers_on) {
    if(reset_used)
      time.ResetUsed();
    StartTime(time);
  }
  RunLayers(layer_call, lpidx);
  EndStep(stage, time, cyc);
  if(timers_on)
    time.IncrAvg();
}

void LeabraTask::Cycle_Run() {
  LeabraThreadMgr* mg = mgr();
  LeabraNetwork* net = (LeabraNetwork*)network.ptr();
  const int n_lays = net->active_layer_idx.size;

  const bool timers_on = mg->timers_on;

  const int st_cyc = net->cycle;  // our starting cycle
  const int n_cyc = (mg->quarter ? net->times.quarter : 1);
  const int n_task = mg->tasks.size;

  for(int cyc=0; cyc < n_cyc; cyc++) {
    int cur_net_cyc = st_cyc + cyc;
    // this replicates LeabraNetwork::Cycle()

    if(task_id == 0) mg->loop_idx1 = 1;          // reset next guy
#ifdef CUDA_COMPILE
    StartTime(send_netin_time);
    if(task_id == 0) {
      net->Cuda_Send_Netin();
    }
    EndStep(mg->stage_net, send_netin_time, cyc);
#else
    { // Send_NetinDelta
      LeabraThreadUnitCall un_call(&LeabraUnit::Send_NetinDelta);
      RunUnitsStep(un_call, mg->loop_idx0, mg->stage_net, send_netin_time, cyc, true);
      // true = connection level
    }
#endif

    if(task_id == 0) mg->loop_idx0 = 1;          // reset next guy
    { // Compute_NetinInteg();
      LeabraThreadUnitCall un_call(&LeabraUnit::Compute_NetinInteg);
      RunUnitsStep(un_call, mg->loop_idx1, mg->stage_net_int, netin_integ_time, cyc,
                   false);
      // false = unit level
    }

    if(task_id == 0) mg->loop_idx1 = 1;          // reset next guy
    { // Compute_Inhib();
      LeabraThreadLayerCall lay_call(&LeabraLayer::Compute_Inhib);
      RunLayersStep(lay_call, mg->loop_idx0, mg->stage_inhib, inhib_time, cyc);
    }

    if(net->net_misc.lay_gp_inhib) {
      if(task_id == 0) {        // first thread does this..
        net->Compute_Inhib_LayGp();
      }
      // this will not complete before we hit Compute_Act, so we need a step here..
      EndStep(mg->stage_lay_gp, inhib_time, cyc);
    }

    if(task_id == 0) mg->loop_idx0 = 1;
    { // Compute_Act();
      LeabraThreadUnitCall un_call(&LeabraUnit::Compute_Act_l);
      RunUnitsStep(un_call, mg->loop_idx1, mg->stage_act, act_time, cyc, false);
      // false = unit level
    }

    // have to do this early before anyone depends on it..
    if(task_id == 0) {        // first thread does this..
      net->Compute_CycleStats_Pre();
    }

    if(task_id == 0) mg->loop_idx1 = 1; 
    { // Compute_Act_Post();
      LeabraThreadUnitCall un_call(&LeabraUnit::Compute_Act_Post);
      RunUnitsStep(un_call, mg->loop_idx0, mg->stage_act_post, act_post_time, cyc, false);
      // false = unit level);
    }

    if(task_id == 0) mg->loop_idx0 = 1;          // reset next guy
    { // Compute_CycleStats();
      LeabraThreadLayerCall lay_call(&LeabraLayer::Compute_CycleStats);
      RunLayersStep(lay_call, mg->loop_idx1, mg->stage_cyc_stats, cycstats_time, cyc);

      if(task_id == 0) {        // first thread does this..
        net->Compute_CycleStats_Post(); // output name
      }
    }

    if(task_id == 0) {
      net->Cycle_IncrCounters();
    }
  }
}

void LeabraTask::Send_TICtxtNetins() {
  LeabraThreadMgr* mg = mgr();

  if(task_id == 0) mg->loop_idx1 = 1;          // reset next guy
  {
    LeabraThreadUnitCall un_call(&LeabraUnit::Send_TICtxtNetin);
    RunUnitsStep(un_call, mg->loop_idx0, mg->stage_ctxt, ti_netin_time, 0, true);
    // cyc = 0, true = con_level
  }

  if(task_id == 0) mg->loop_idx0 = 1;          // reset next guy
  { 
    LeabraThreadUnitCall un_call(&LeabraUnit::Send_TICtxtNetin_Post);
    RunUnitsStep(un_call, mg->loop_idx1, mg->stage_ctxt_p, ti_netin_time, 0, false);
    // cyc = 0, false = unit level
  }
}

void LeabraTask::Compute_dWt() {
  LeabraThreadMgr* mg = mgr();
  LeabraNetwork* net = (LeabraNetwork*)network.ptr();

  if(task_id == 0) mg->loop_idx1 = 1;          // reset next guy
  { 
    LeabraThreadUnitCall un_call(&LeabraUnit::Compute_dWt_l);
    RunUnitsStep(un_call, mg->loop_idx0, mg->stage_dwt, dwt_time, 0, true);
    // cyc = 0, true = con_level
  }

  if(task_id == 0) mg->loop_idx0 = 1;          // reset next guy
  if(net->net_misc.dwt_norm) {
    LeabraThreadUnitCall un_call(&LeabraUnit::Compute_dWt_Norm);
    RunUnitsStep(un_call, mg->loop_idx1, mg->stage_dwt_norm, dwt_norm_time, 0, true);
    // cyc = 0, true = con_level
  }
}

void LeabraTask::Compute_Weights() {
  LeabraThreadMgr* mg = mgr();

  if(task_id == 0) mg->loop_idx1 = 0;          // reset next guy
  LeabraThreadUnitCall un_call(&LeabraUnit::Compute_Weights_l);
  RunUnitsStep(un_call, mg->loop_idx0, mg->stage_wt, wt_time, 0, true);
  // cyc = 0, true = con_level
}

void LeabraTask::ThreadReport(DataTable& dt) {
  int idx;
  DataCol* thc = dt.FindMakeColName("thread", idx, VT_INT);
  DataCol* stat = dt.FindMakeColName("stat", idx, VT_STRING);
  DataCol* rca = dt.FindMakeColName("run_avg", idx, VT_FLOAT);
  DataCol* rcs = dt.FindMakeColName("run_sum", idx, VT_FLOAT);
  DataCol* wca = dt.FindMakeColName("wait_avg", idx, VT_FLOAT);
  DataCol* wcs = dt.FindMakeColName("wait_sum", idx, VT_FLOAT);
  
  float rescale = 1.0e6;        // how many microseconds
  dt.AddBlankRow();
  thc->SetValAsInt(task_id, -1);
  stat->SetValAsString("send_netin_time", -1);
  rca->SetValAsFloat(send_netin_time.run.avg_used.avg * rescale, -1);
  wca->SetValAsFloat(send_netin_time.wait.avg_used.avg * rescale, -1);
  rcs->SetValAsFloat(send_netin_time.run.avg_used.sum, -1);
  wcs->SetValAsFloat(send_netin_time.wait.avg_used.sum, -1);

  dt.AddBlankRow();
  thc->SetValAsInt(task_id, -1);
  stat->SetValAsString("netin_integ_time", -1);
  rca->SetValAsFloat(netin_integ_time.run.avg_used.avg * rescale, -1);
  wca->SetValAsFloat(netin_integ_time.wait.avg_used.avg * rescale, -1);
  rcs->SetValAsFloat(netin_integ_time.run.avg_used.sum, -1);
  wcs->SetValAsFloat(netin_integ_time.wait.avg_used.sum, -1);

  dt.AddBlankRow();
  thc->SetValAsInt(task_id, -1);
  stat->SetValAsString("inhib_time", -1);
  rca->SetValAsFloat(inhib_time.run.avg_used.avg * rescale, -1);
  wca->SetValAsFloat(inhib_time.wait.avg_used.avg * rescale, -1);
  rcs->SetValAsFloat(inhib_time.run.avg_used.sum, -1);
  wcs->SetValAsFloat(inhib_time.wait.avg_used.sum, -1);

  dt.AddBlankRow();
  thc->SetValAsInt(task_id, -1);
  stat->SetValAsString("act_time", -1);
  rca->SetValAsFloat(act_time.run.avg_used.avg * rescale, -1);
  wca->SetValAsFloat(act_time.wait.avg_used.avg * rescale, -1);
  rcs->SetValAsFloat(act_time.run.avg_used.sum, -1);
  wcs->SetValAsFloat(act_time.wait.avg_used.sum, -1);

  dt.AddBlankRow();
  thc->SetValAsInt(task_id, -1);
  stat->SetValAsString("act_post_time", -1);
  rca->SetValAsFloat(act_post_time.run.avg_used.avg * rescale, -1);
  wca->SetValAsFloat(act_post_time.wait.avg_used.avg * rescale, -1);
  rcs->SetValAsFloat(act_post_time.run.avg_used.sum, -1);
  wcs->SetValAsFloat(act_post_time.wait.avg_used.sum, -1);

  dt.AddBlankRow();
  thc->SetValAsInt(task_id, -1);
  stat->SetValAsString("cycstats_time", -1);
  rca->SetValAsFloat(cycstats_time.run.avg_used.avg * rescale, -1);
  wca->SetValAsFloat(cycstats_time.wait.avg_used.avg * rescale, -1);
  rcs->SetValAsFloat(cycstats_time.run.avg_used.sum, -1);
  wcs->SetValAsFloat(cycstats_time.wait.avg_used.sum, -1);

  dt.AddBlankRow();
  thc->SetValAsInt(task_id, -1);
  stat->SetValAsString("dwt_time", -1);
  rca->SetValAsFloat(dwt_time.run.avg_used.avg * rescale, -1);
  wca->SetValAsFloat(dwt_time.wait.avg_used.avg * rescale, -1);
  rcs->SetValAsFloat(dwt_time.run.avg_used.sum, -1);
  wcs->SetValAsFloat(dwt_time.wait.avg_used.sum, -1);

  dt.AddBlankRow();
  thc->SetValAsInt(task_id, -1);
  stat->SetValAsString("dwt_norm_time", -1);
  rca->SetValAsFloat(dwt_norm_time.run.avg_used.avg * rescale, -1);
  wca->SetValAsFloat(dwt_norm_time.wait.avg_used.avg * rescale, -1);
  rcs->SetValAsFloat(dwt_norm_time.run.avg_used.sum, -1);
  wcs->SetValAsFloat(dwt_norm_time.wait.avg_used.sum, -1);

  dt.AddBlankRow();
  thc->SetValAsInt(task_id, -1);
  stat->SetValAsString("wt_time", -1);
  rca->SetValAsFloat(wt_time.run.avg_used.avg * rescale, -1);
  wca->SetValAsFloat(wt_time.wait.avg_used.avg * rescale, -1);
  rcs->SetValAsFloat(wt_time.run.avg_used.sum, -1);
  wcs->SetValAsFloat(wt_time.wait.avg_used.sum, -1);

  dt.AddBlankRow();
  thc->SetValAsInt(task_id, -1);
  stat->SetValAsString("ti_netin_time", -1);
  rca->SetValAsFloat(ti_netin_time.run.avg_used.avg * rescale, -1);
  wca->SetValAsFloat(ti_netin_time.wait.avg_used.avg * rescale, -1);
  rcs->SetValAsFloat(ti_netin_time.run.avg_used.sum, -1);
  wcs->SetValAsFloat(ti_netin_time.wait.avg_used.sum, -1);

}

void LeabraTask::run() {
  LeabraThreadMgr* mg = mgr();
  LeabraNetwork* net = (LeabraNetwork*)network.ptr();

  while(true) {
#if (QT_VERSION >= 0x050000)
    const int run_st = mg->run_state.loadAcquire(); // find out where we're at
#else
    const int run_st = (int)mg->run_state;
#endif
    bool prog_stop = (Program::global_run_state != Program::RUN); // program has stopped!

    switch(run_st) {
    case LeabraThreadMgr::NOT_RUNNING:
      return;                   // well, we better not be!
    case LeabraThreadMgr::ACTIVE_WAIT:           // just keep waiting..
      if(task_id == 0) {
        taMisc::Info("task 0 got an ACTIVE_WAIT run state -- shouldn't happen");
        return;
      }
      if(prog_stop) {
        if(task_id == 1) {      // this guy is delegated to update run state in this case
          mg->run_state = LeabraThreadMgr::NOT_RUNNING;
        }
        return;
      }
      continue;                 // loop again here until we get something different
    case LeabraThreadMgr::RUN_CYCLE:
      Cycle_Run();
      if(task_id == 0) {
        mg->run_state = LeabraThreadMgr::NOT_RUNNING; // stopped!
      }
      return;                   // this is the final step for testing
    case LeabraThreadMgr::RUN_TI_NETS:
      Send_TICtxtNetins();
      if(task_id == 0) {
        mg->run_state = LeabraThreadMgr::NOT_RUNNING; // stopped!
      }
      return;
    case LeabraThreadMgr::RUN_DWT:
      Compute_dWt();
      if(task_id == 0) {
        mg->run_state = LeabraThreadMgr::NOT_RUNNING; // stopped!
        // stopping here makes a BIG diff in performance!
      }
      return;                   // this is the final step in computation so we bail
    case LeabraThreadMgr::RUN_WT:
      Compute_Weights();
      if(task_id == 0) {
        mg->run_state = LeabraThreadMgr::NOT_RUNNING; // stopped!
      }
      return;                   // this is the final step in computation so we bail
    }

    EndStep(mg->done_run, done_time, 0); // get all to sync here -- don't re-sample run state (which should be active_wait) until task_0 has updated it!!
    done_time.IncrAvg();

    if(task_id == 0) {                   // we always bail!
      return;
    }
  }
}

void LeabraThreadMgr::Initialize() {
  run_state = NOT_RUNNING;
  n_threads_act = 1;
  quarter = true;
  unit_chunks = 32;
  unit_fixed = false;
  timers_on = false;
  using_threads = false;
  n_threads_prev = n_threads;
  task_type = &TA_LeabraTask;
  InitStages();
}

void LeabraThreadMgr::Destroy() {
}

void LeabraThreadMgr::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(!taMisc::is_loading && n_threads != n_threads_prev) {
    network()->BuildUnits_Threads(); // calls InitAll where n_threads_prev is set..
  }
}

void LeabraThreadMgr::InitAll() {
  if((threads.size == n_threads-1) && (tasks.size == n_threads)) {
    // same as before -- nothing to do?
  }
  else {
    n_threads_prev = n_threads;
    inherited::InitAll();
  }
  LeabraNetwork* net = (LeabraNetwork*)network();
  if(net->n_cons == 0 || net->units_flat.size == 0) return; // nothing to do..

  int un_idx = 1;
  for(int i=0;i<tasks.size;i++) {
    LeabraTask* uct = (LeabraTask*)tasks[i];
    uct->network = net;
  }
}

bool LeabraThreadMgr::CanRun() {
  Network* net = network();

  int min_units = (unit_chunks * tasks.size) / 2;

  bool other_reasons = (net->units_flat.size < min_units);

  if(n_threads == 1 || other_reasons) {
    using_threads = false;
    n_threads_act = 1;
    return false;
  }
  return true;
}

void LeabraThreadMgr::InitStages() {
  loop_idx0 = 1;
  loop_idx1 = 1;
  loop_idx2 = 1;

  done_run = 0;

  stage_net = 0;
  stage_net_int = 0;

  stage_inhib = 0;
  stage_lay_gp = 0;

  stage_act = 0;
  stage_act_post = 0;
  stage_cyc_stats = 0;

  stage_deep5b = 0;
  stage_deep5b_p = 0;
  stage_ctxt = 0;
  stage_ctxt_p = 0;
  
  stage_dwt = 0;
  stage_dwt_norm = 0;
  stage_wt = 0;
}

void LeabraThreadMgr::Run(RunStates run_typ) {
  using_threads = true;
  n_threads_act = tasks.size;

  // only task 0 gets to set run state, except for program stopping, where task 1 is it

#if (QT_VERSION >= 0x050000)
  const int cur_run_state = run_state.loadAcquire();
#else
  const int cur_run_state = (int)run_state;
#endif

  if(!(cur_run_state == NOT_RUNNING || cur_run_state == ACTIVE_WAIT)) {
    taMisc::Error("threading programmer error: run state is not NOT_RUNNING or ACTIVE_WAIT at start of threaded call -- please report bug!", String(cur_run_state));
    return;
  }

  InitStages();

  run_state = run_typ; // get 'em going -- active wait guys will go NOW!
  if(cur_run_state == NOT_RUNNING) {
    RunThreads();         // then run the subsidiary guys
    tasks[0]->run();      // run our own set..
  }
  else {
    tasks[0]->run();      // run our own set..
  }

  // task 0 will always return here -- other guys will be in wait release or not running
  // all the run_state updating is handled in run()

  // finally, always need to sync at end to ensure that everyone is done!
#if (QT_VERSION >= 0x050000)
  const int end_run_state = run_state.loadAcquire();
#else
  const int end_run_state = (int)run_state;
#endif
  if(end_run_state == NOT_RUNNING) {
    SyncThreads();              // stop them all!
  }
}

void LeabraThreadMgr::ThreadReport(DataTable* table) {
  if(!table) return;
  table->StructUpdate(true);
  table->ResetData();

  for(int i=0; i<tasks.size; i++) {
    LeabraTask* uct = (LeabraTask*)tasks[i];
    uct->ThreadReport(*table);
  }
  table->StructUpdate(false);
}

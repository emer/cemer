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

String RunWaitTime::ReportAvg(float rescale) {
  String rval;// = name;
  rval << " run: " << (rescale * run.avg_used.avg)
       << ", wait: " << (rescale * wait.avg_used.avg);
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
    mg->cur_un_idx = 1;    // as the last guy, we get to reset the nibble count
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
  if(cur_cnt == trg) { // all done now
    mg->cur_un_idx = 1; // this is a bit wasteful -- everyone will try at same time
    return;
  }

  if(timers_on) {
    time.EndWait();
  }
}

#if 0
// fixed allocation based on thread id -- less flexible..
void LeabraTask::RunUnits(LeabraThreadUnitCall& unit_call) {
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

#else 

// dynamic allocation with nibbling
void LeabraTask::RunUnits(LeabraThreadUnitCall& unit_call) {
  LeabraThreadMgr* mg = mgr();
  LeabraNetwork* net = (LeabraNetwork*)network.ptr();

  const int chk = mg->unit_chunks;
  const int un_mx = net->units_flat.size;

  while(true) {
    int nxt_uidx = mg->cur_un_idx.fetchAndAddOrdered(chk);
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

#endif

void LeabraTask::RunUnitsStep(LeabraThreadUnitCall& unit_call, QAtomicInt& stage,
                              RunWaitTime& time, int cyc, bool reset_used) {
  LeabraThreadMgr* mg = mgr();
  const bool timers_on = mg->timers_on;
  if(timers_on) {
    if(reset_used)
      time.ResetUsed();
    StartTime(time);
  }
  RunUnits(unit_call);
  EndStep(stage, time, cyc);
  if(timers_on)
    time.IncrAvg();
}

void LeabraTask::RunUnitsTime(LeabraThreadUnitCall& unit_call,
                              RunWaitTime& time, bool reset_used) {
  LeabraThreadMgr* mg = mgr();
  const bool timers_on = mg->timers_on;
  if(timers_on) {
    if(reset_used)
      time.ResetUsed();
    StartTime(time);
  }
  RunUnits(unit_call);
  if(timers_on) {
    EndTime(time);
    time.IncrAvg();           // resets always incr
  }
}


void LeabraTask::Cycle_Run() {
  LeabraThreadMgr* mg = mgr();
  LeabraNetwork* net = (LeabraNetwork*)network.ptr();

  const bool timers_on = mg->timers_on;

  const int st_ct_cyc = net->ct_cycle;  // our starting cycle
  const int n_cyc = mg->n_cycles;
  const int n_task = mg->tasks.size;

  const int n_lays = net->active_layer_idx.size;

  for(int cyc=0; cyc < n_cyc; cyc++) {
    int cur_net_cyc = st_ct_cyc + cyc;
    // this replicates LeabraNetwork::Cycle()

    // Compute_SRAvg_State();
    if(task_id == 0) {        // first thread does this -- very fast -- get it done early
      net->Compute_SRAvg_State();
    }

    { // Send_NetinDelta
      LeabraThreadUnitCall un_call(&LeabraUnit::Send_NetinDelta);
      RunUnitsStep(un_call, mg->stage_net, send_netin_time, cyc);
    }

    { // Compute_NetinInteg();
      LeabraThreadUnitCall un_call(&LeabraUnit::Compute_NetinInteg);
      RunUnitsStep(un_call, mg->stage_net_int, netin_integ_time, cyc);
    }

    { // Compute_Inhib();
      if(timers_on)
        StartTime(inhib_time);
      for(int i=task_id; i<n_lays; i+= n_task) {
        int li = net->active_layer_idx[i];
        LeabraLayer* lay = (LeabraLayer*)net->layers.Leaf(li);
        lay->Compute_Inhib(net);
      }
      EndStep(mg->stage_inhib, inhib_time, cyc);
    }

    if(net->net_misc.lay_gp_inhib) {
      if(task_id == 0) {        // first thread does this..
        net->Compute_Inhib_LayGp();
        // todo: this needs to apply all the way out to layer groups -- also it is 
        // incompatible with the self setting.. probably need to get rid of self!
      }
    }

    { // Compute_Act();
      LeabraThreadUnitCall un_call(&LeabraUnit::Compute_Act_l);
      RunUnitsStep(un_call, mg->stage_act, act_time, cyc);
    }

    if(net->Compute_SRAvg_Cons_Test()) {
      LeabraThreadUnitCall un_call(&LeabraUnit::Compute_SRAvg_Cons);
      RunUnitsTime(un_call, sravg_cons_time); // no sync -- no dependency with next
    }

    { // Compute_CycleStats();
      if(timers_on)
        StartTime(cycstats_time);
      if(task_id == 0) {        // first thread does this..
        net->Compute_CycleStats_Pre();
      }
      for(int i=task_id; i<n_lays; i+= n_task) {
        int li = net->active_layer_idx[i];
        LeabraLayer* lay = (LeabraLayer*)net->layers.Leaf(li);
        lay->Compute_CycleStats(net);
      }
      EndStep(mg->stage_cyc_stats, cycstats_time, cyc);
    }

    // Compute_CycSynDep();
    if(net->net_misc.cyc_syn_dep && (net->ct_cycle % net->net_misc.syn_dep_int == 0)) {
      LeabraThreadUnitCall un_call(&LeabraUnit::Compute_CycSynDep);
      RunUnitsTime(un_call, cycsyndep_time);
    }

    // Compute_MidMinus();           // check for mid-minus and run if so (PBWM)
    // todo: figure this one out!

    if(task_id == 0) {
      net->cycle++;
      net->ct_cycle++;
      net->time += net->time_inc; // always increment time..
    }
  }
}

void LeabraTask::TI_Send_Netins() {
  LeabraThreadMgr* mg = mgr();

  {
    LeabraThreadUnitCall un_call(&LeabraUnit::TI_Send_Deep5bNetin);
    RunUnitsStep(un_call, mg->stage_deep5b, ti_netin_time, 0); // cyc = 0
  }
  { 
    LeabraThreadUnitCall un_call(&LeabraUnit::TI_Send_Deep5bNetin_Post);
    RunUnitsStep(un_call, mg->stage_deep5b_p, ti_netin_time, 0, false); // cyc = 0, no reset
  }

  {
    LeabraThreadUnitCall un_call(&LeabraUnit::TI_Send_CtxtNetin);
    RunUnitsStep(un_call, mg->stage_ctxt, ti_netin_time, 0, false); // cyc = 0, no reset
  }
  { 
    LeabraThreadUnitCall un_call(&LeabraUnit::TI_Send_CtxtNetin_Post);
    RunUnitsStep(un_call, mg->stage_ctxt_p, ti_netin_time, 0, false); // cyc = 0, no reset
  }
}

void LeabraTask::Compute_dWt() {
  LeabraThreadMgr* mg = mgr();
  LeabraNetwork* net = (LeabraNetwork*)network.ptr();

  { 
    LeabraThreadUnitCall un_call(&LeabraUnit::Compute_dWt_l);
    RunUnitsStep(un_call, mg->stage_dwt, dwt_time, 0, true); // cyc = 0, reset
  }

  if(net->net_misc.dwt_norm_used) {
    LeabraThreadUnitCall un_call(&LeabraUnit::Compute_dWt_Norm);
    RunUnitsStep(un_call, mg->stage_dwt_norm, dwt_norm_time, 0, true); // cyc = 0, reset
  }
}

void LeabraTask::Compute_Weights() {
  LeabraThreadMgr* mg = mgr();

  LeabraThreadUnitCall un_call(&LeabraUnit::Compute_Weights_l);
  RunUnitsStep(un_call, mg->stage_wt, wt_time, 0, true); // cyc = 0, reset
}

void LeabraTask::ThreadReport(DataTable& dt) {
  int idx;
  DataCol* thc = dt.FindMakeColName("thread", idx, VT_INT);
  DataCol* stat = dt.FindMakeColName("stat", idx, VT_STRING);
  DataCol* sn_r = dt.FindMakeColName("send_net_run", idx, VT_FLOAT);
  DataCol* sn_w = dt.FindMakeColName("send_net_wait", idx, VT_FLOAT);
  DataCol* ni_r = dt.FindMakeColName("net_int_run", idx, VT_FLOAT);
  DataCol* ni_w = dt.FindMakeColName("net_int_wait", idx, VT_FLOAT);
  DataCol* ih_r = dt.FindMakeColName("inhib_run", idx, VT_FLOAT);
  DataCol* ih_w = dt.FindMakeColName("inhib_wait", idx, VT_FLOAT);
  DataCol* ac_r = dt.FindMakeColName("act_run", idx, VT_FLOAT);
  DataCol* ac_w = dt.FindMakeColName("act_wait", idx, VT_FLOAT);
  DataCol* cs_r = dt.FindMakeColName("cyc_stat_run", idx, VT_FLOAT);
  DataCol* cs_w = dt.FindMakeColName("cyc_stat_wait", idx, VT_FLOAT);
  DataCol* dw_r = dt.FindMakeColName("dwt_run", idx, VT_FLOAT);
  DataCol* dw_w = dt.FindMakeColName("dwt_wait", idx, VT_FLOAT);
  DataCol* dwn_r = dt.FindMakeColName("dwt_norm_run", idx, VT_FLOAT);
  DataCol* dwn_w = dt.FindMakeColName("dwt_norm_wait", idx, VT_FLOAT);
  DataCol* wt_r = dt.FindMakeColName("wt_run", idx, VT_FLOAT);
  DataCol* wt_w = dt.FindMakeColName("wt_wait", idx, VT_FLOAT);
  DataCol* dn_w = dt.FindMakeColName("done_wait", idx, VT_FLOAT);

  LeabraNetwork* net = (LeabraNetwork*)network.ptr();
  int tot_cyc = net->ct_time.total_cycles;

  float rescale = 1.0e6;        // how many microseconds
  dt.AddBlankRow();
  thc->SetValAsInt(task_id, -1);
  stat->SetValAsString("avg", -1);
  sn_r->SetValAsFloat(send_netin_time.run.avg_used.avg * rescale, -1);
  sn_w->SetValAsFloat(send_netin_time.wait.avg_used.avg * rescale, -1);
  ni_r->SetValAsFloat(netin_integ_time.run.avg_used.avg * rescale, -1);
  ni_w->SetValAsFloat(netin_integ_time.wait.avg_used.avg * rescale, -1);
  ih_r->SetValAsFloat(inhib_time.run.avg_used.avg * rescale, -1);
  ih_w->SetValAsFloat(inhib_time.wait.avg_used.avg * rescale, -1);
  ac_r->SetValAsFloat(act_time.run.avg_used.avg * rescale, -1);
  ac_w->SetValAsFloat(act_time.wait.avg_used.avg * rescale, -1);
  cs_r->SetValAsFloat(cycstats_time.run.avg_used.avg * rescale, -1);
  cs_w->SetValAsFloat(cycstats_time.wait.avg_used.avg * rescale, -1);
  dw_r->SetValAsFloat(dwt_time.run.avg_used.avg * rescale, -1);
  dw_w->SetValAsFloat(dwt_time.wait.avg_used.avg * rescale, -1);
  dwn_r->SetValAsFloat(dwt_norm_time.run.avg_used.avg * rescale, -1);
  dwn_w->SetValAsFloat(dwt_norm_time.wait.avg_used.avg * rescale, -1);
  wt_r->SetValAsFloat(wt_time.run.avg_used.avg * rescale, -1);
  wt_w->SetValAsFloat(wt_time.wait.avg_used.avg * rescale, -1);
  dn_w->SetValAsFloat(done_time.wait.avg_used.avg * rescale, -1);

  dt.AddBlankRow();
  thc->SetValAsInt(task_id, -1);
  stat->SetValAsString("sum", -1);
  sn_r->SetValAsFloat(send_netin_time.run.avg_used.sum, -1);
  sn_w->SetValAsFloat(send_netin_time.wait.avg_used.sum, -1);
  ni_r->SetValAsFloat(netin_integ_time.run.avg_used.sum, -1);
  ni_w->SetValAsFloat(netin_integ_time.wait.avg_used.sum, -1);
  ih_r->SetValAsFloat(inhib_time.run.avg_used.sum, -1);
  ih_w->SetValAsFloat(inhib_time.wait.avg_used.sum, -1);
  ac_r->SetValAsFloat(act_time.run.avg_used.sum, -1);
  ac_w->SetValAsFloat(act_time.wait.avg_used.sum, -1);
  cs_r->SetValAsFloat(cycstats_time.run.avg_used.sum, -1);
  cs_w->SetValAsFloat(cycstats_time.wait.avg_used.sum, -1);
  dw_r->SetValAsFloat(dwt_time.run.avg_used.sum, -1);
  dw_w->SetValAsFloat(dwt_time.wait.avg_used.sum, -1);
  dwn_r->SetValAsFloat(dwt_norm_time.run.avg_used.sum, -1);
  dwn_w->SetValAsFloat(dwt_norm_time.wait.avg_used.sum, -1);
  wt_r->SetValAsFloat(wt_time.run.avg_used.sum, -1);
  wt_w->SetValAsFloat(wt_time.wait.avg_used.sum, -1);
  dn_w->SetValAsFloat(done_time.wait.avg_used.sum, -1);
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
      TI_Send_Netins();
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
  n_cycles = 10;
  unit_chunks = 32;
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
  int tot_cyc = net->ct_time.total_cycles * 2;              // buffer

  int un_idx = 1;
  for(int i=0;i<tasks.size;i++) {
    LeabraTask* uct = (LeabraTask*)tasks[i];
    uct->network = net;
  }
}

bool LeabraThreadMgr::CanRun() {
  Network* net = network();

  int min_units = unit_chunks * tasks.size;

  bool other_reasons = (net->units_flat.size < min_units);

  if(n_threads == 1 || other_reasons) {
    using_threads = false;
    n_threads_act = 1;
    return false;
  }
  return true;
}

void LeabraThreadMgr::InitStages() {
  cur_un_idx = 1;               // starts at 1

  done_run = 0;

  stage_net = 0;
  stage_net_int = 0;

  stage_inhib = 0;

  stage_act = 0;
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

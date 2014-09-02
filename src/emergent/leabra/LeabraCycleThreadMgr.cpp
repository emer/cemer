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

#include "LeabraCycleThreadMgr.h"

#include <LeabraNetwork>
#include <taMisc>

// todo: timer flag to turn off timing -- how long is that taking?
// timing separate for layer vs. unit level!  then we can optimize each separately!
// actually: separate timers for each function, and separate start/end for each
// then we can really optimize each one separately
// flat linked list of active (non-lesioned) layers for better distribution
// ApplyInhib happens at unit level

TA_BASEFUNS_CTORS_DEFN(LeabraCycleTask);
TA_BASEFUNS_CTORS_DEFN(RunWaitTime);
TA_BASEFUNS_CTORS_DEFN(LeabraCycleThreadMgr);

String RunWaitTime::ReportAvg(float rescale) {
  String rval = name;
  rval << " run: " << (rescale * run.avg_used.avg)
       << ", wait: " << (rescale * wait.avg_used.avg);
  return rval;
}

void LeabraCycleTask::Initialize() {
  uidx_st = 0;
  uidx_ed = 0;
  lay_st = 0;
  lay_ed = 0;
}

void LeabraCycleTask::Destroy() {
  network.CutLinks();
}

void LeabraCycleTask::StartCycle() {
  send_netin_time.ResetUsed();
  netin_integ_time.ResetUsed();
  inhib_time.ResetUsed();
  act_time.ResetUsed();
  cycstats_time.ResetUsed();

  sravg_cons_time.ResetUsed();
  cycsyndep_time.ResetUsed();
}

void LeabraCycleTask::EndStep(QAtomicInt& stage, RunWaitTime& time, int cyc) {
  LeabraCycleThreadMgr* mg = mgr();
  time.EndRun();
  if(mg->tasks.size == 1 || !mg->sync_steps) {
    return;
  }
  int trg = (cyc+1) * mg->tasks.size;

  int cur_cnt = stage.fetchAndAddOrdered(1);
  if(cur_cnt == trg) { // we were the last guy
    return;
  }

  time.StartWait(false);

  while(cur_cnt < trg) {
    // taManagedThread::usleep(1); // just slows down a tiny bit, no value..
    cur_cnt = stage.loadAcquire();
  }

  time.EndWait();
}

void LeabraCycleTask::EndCycle() {
  send_netin_time.IncrAvg();
  netin_integ_time.IncrAvg();
  inhib_time.IncrAvg();
  act_time.IncrAvg();
  cycstats_time.IncrAvg();

  sravg_cons_time.IncrAvg();
  cycsyndep_time.IncrAvg();
}

void LeabraCycleTask::run() {
  LeabraCycleThreadMgr* mg = mgr();
  LeabraNetwork* net = (LeabraNetwork*)network.ptr();

  StartCycle();

  for(int cyc=0; cyc < mg->n_cycles; cyc++) {
    // this replicates LeabraNetwork::Cycle()

    // Compute_SRAvg_State();
    if(task_id == 0) {        // first thread does this -- very fast -- get it done early
      net->Compute_SRAvg_State();
    }

    // Send_NetinDelta
    StartStep(send_netin_time);
    for(int i=uidx_st; i<uidx_ed; i++) {
      LeabraUnit* un = (LeabraUnit*)net->units_flat[i];
      un->Send_NetinDelta(net, task_id);
    }
    EndStep(mg->stage_net, send_netin_time, cyc);

    // Compute_NetinInteg();
    StartStep(netin_integ_time);
    for(int i=uidx_st; i<uidx_ed; i++) {
      LeabraUnit* un = (LeabraUnit*)net->units_flat[i];
      un->Compute_NetinInteg(net, task_id);
    }
    EndStep(mg->stage_net_int, netin_integ_time, cyc);

    // Compute_Inhib();
    StartStep(inhib_time);
    for(int i=lay_st; i<lay_ed; i++) {
      if(i >= net->layers.leaves) continue;
      LeabraLayer* lay = (LeabraLayer*)net->layers.Leaf(i);
      if(lay->lesioned()) continue;
      lay->Compute_Inhib(net);
    }
    EndStep(mg->stage_inhib, inhib_time, cyc);

    if(net->net_misc.lay_gp_inhib) {
      if(task_id == 0) {        // first thread does this..
        net->Compute_Inhib_LayGp();
        // todo: this needs to apply all the way out to layer groups -- also it is 
        // incompatible with the self setting.. probably need to get rid of self!
      }
    }

    // Compute_Act();
    StartStep(act_time);
    for(int i=uidx_st; i<uidx_ed; i++) {
      LeabraUnit* un = (LeabraUnit*)net->units_flat[i];
      un->Compute_Act(net, task_id); // includes SRAvg
    }
    EndStep(mg->stage_act, act_time, cyc);

    if(net->Compute_SRAvg_Cons_Test()) {
      StartStep(sravg_cons_time);
      for(int i=uidx_st; i<uidx_ed; i++) {
        LeabraUnit* un = (LeabraUnit*)net->units_flat[i];
        un->Compute_SRAvg_Cons(net, task_id);
      }
      EndTime(sravg_cons_time); // no sync -- no dependency with next
    }

    // Compute_CycleStats();
    StartStep(cycstats_time);
    if(task_id == 0) {        // first thread does this..
      net->Compute_CycleStats_Pre();
    }
    for(int i=lay_st; i<lay_ed; i++) {
      if(i >= net->layers.leaves) continue;
      LeabraLayer* lay = (LeabraLayer*)net->layers.Leaf(i);
      if(lay->lesioned()) continue;
      lay->Compute_CycleStats(net);
    }
    EndStep(mg->stage_cyc_stats, cycstats_time, cyc);

    // Compute_CycSynDep();
    if(net->net_misc.cyc_syn_dep) {
      StartStep(cycsyndep_time);
      if(net->ct_cycle % net->net_misc.syn_dep_int == 0) {
        for(int i=uidx_st; i<uidx_ed; i++) {
          LeabraUnit* un = (LeabraUnit*)net->units_flat[i];
          un->Compute_CycSynDep(net, task_id);
        }
      }
      // no sync needed here really -- no dependency on next steps
      EndTime(cycsyndep_time);
    }

    // Compute_MidMinus();           // check for mid-minus and run if so (PBWM)
    // todo: figure this one out!

    if(task_id == 0) {
      net->cycle++;
      net->ct_cycle++;
      net->time += net->time_inc; // always increment time..
    }
  }

  EndCycle();
}

String LeabraCycleTask::ThreadReport() {
  float rescale = 1.0e6;        // how many microseconds
  String rval;
  rval << task_id << ": " << "un: " << uidx_st << " - " << uidx_ed
       << "  lay: " << lay_st << " - " << lay_ed
       << "  || "
       << send_netin_time.ReportAvg(rescale) << "; "
       << netin_integ_time.ReportAvg(rescale) << "; "
       << inhib_time.ReportAvg(rescale) << "; "
       << act_time.ReportAvg(rescale) << "; "
       << cycstats_time.ReportAvg(rescale) << "; "
       << "\n";
  return rval;
}

void LeabraCycleThreadMgr::Initialize() {
  n_threads_act = 1;
  n_cycles = 1;
  input_cost = 0.1f;
  target_cost = 0.5f;
  min_units = taMisc::thread_defaults.min_units;
  sync_steps = true;
  using_threads = false;
  n_threads_prev = n_threads;
  task_type = &TA_LeabraCycleTask;
  InitStages();
}

void LeabraCycleThreadMgr::Destroy() {
}

void LeabraCycleThreadMgr::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(!taMisc::is_loading && n_threads != n_threads_prev) {
    network()->BuildUnits_Threads(); // calls InitAll where n_threads_prev is set..
  }
}

void LeabraCycleThreadMgr::InitAll() {
  if((threads.size == n_threads-1) && (tasks.size == n_threads)) {
    // same as before -- nothing to do?
  }
  else {
    n_threads_prev = n_threads;
    inherited::InitAll();
  }
  LeabraNetwork* net = (LeabraNetwork*)network();
  if(net->n_cons == 0 || net->units_flat.size == 0) return; // nothing to do..

  // allocate units to threads based on number of sending connections
  float cons_per_thr = net->n_cons_cost / (float)tasks.size;
  if(cons_per_thr < 1.0f)          // this shouldn't happen..
    cons_per_thr = 1.0f;
  int un_idx = 1;
  for(int i=0;i<tasks.size;i++) {
    LeabraCycleTask* uct = (LeabraCycleTask*)tasks[i];
    uct->network = net;

    float cons_th = 0.0f;
    uct->uidx_st = un_idx;
    while(cons_th < cons_per_thr && un_idx < net->units_flat.size) {
      LeabraUnit* u = (LeabraUnit*)net->units_flat[un_idx++];
      cons_th += u->n_send_cons_cost;
    }
    uct->uidx_ed = un_idx;
  }
  LeabraCycleTask* uct = (LeabraCycleTask*)tasks.Peek(); // last one
  uct->uidx_ed = net->units_flat.size;

  // allocate layers to threads
  float lay_per_th = net->layers.leaves / tasks.size;
  if(lay_per_th < 1.0f)
    lay_per_th = 1.0f;          // no dupes!
  int lays = 0;
  for(int i=0;i<tasks.size;i++) {
    LeabraCycleTask* uct = (LeabraCycleTask*)tasks[i];
    float nw_lays = lay_per_th * (float)(i+1);
    uct->lay_st = lays;
    uct->lay_ed = (int)(nw_lays + 0.5f);
    lays = uct->lay_ed;
  }
  uct = (LeabraCycleTask*)tasks.Peek(); // last one
  uct->lay_ed = net->layers.leaves;
}

bool LeabraCycleThreadMgr::CanRun() {
  Network* net = network();

  bool other_reasons = (net->units_flat.size < min_units
                        || net->units_flat.size < tasks.size);

  if(n_threads == 1 || other_reasons) {
    using_threads = false;
    n_threads_act = 1;
    return false;
  }
  return true;
}

void LeabraCycleThreadMgr::InitStages() {
  stage_net = 0;
  stage_net_int = 0;

  stage_inhib = 0;

  stage_act = 0;
  stage_cyc_stats = 0;
}

void LeabraCycleThreadMgr::Run() {
  using_threads = true;
  n_threads_act = tasks.size;

  InitStages();

  RunThreads();         // then run the subsidiary guys
  tasks[0]->run();      // run our own set..

  // finally, always need to sync at end to ensure that everyone is done!
  SyncThreads();
}

String LeabraCycleThreadMgr::ThreadReport() {
  String rval;
  for(int i=0; i<tasks.size; i++) {
    LeabraCycleTask* uct = (LeabraCycleTask*)tasks[i];
    rval << uct->ThreadReport();
  }
  return rval;
}

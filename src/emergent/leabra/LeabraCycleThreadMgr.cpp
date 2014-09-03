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
#include <DataTable>
#include <taMisc>

// todo: timer flag to turn off timing -- how long is that taking?
// timing separate for layer vs. unit level!  then we can optimize each separately!
// actually: separate timers for each function, and separate start/end for each
// then we can really optimize each one separately
// flat linked list of active (non-lesioned) layers for better distribution
// ApplyInhib happens at unit level

TA_BASEFUNS_CTORS_DEFN(LeabraCycleTask);
TA_BASEFUNS_CTORS_DEFN(RunWaitTime);
TA_BASEFUNS_CTORS_DEFN(RunWaitTime_List);
TA_BASEFUNS_CTORS_DEFN(LeabraCycleThreadMgr);

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

void LeabraCycleTask::Initialize() {
  un_scon_st = 0;
  un_scon_ed = 0;
  un_st = 0;
  un_ed = 0;
  lay_st = 0;
  lay_ed = 0;
}

void LeabraCycleTask::Destroy() {
  network.CutLinks();
}

void LeabraCycleTask::InitTimers(int tot_cyc) {
  send_netin_time.SetSize(tot_cyc);
  netin_integ_time.SetSize(tot_cyc);
  inhib_time.SetSize(tot_cyc);
  act_time.SetSize(tot_cyc);
  cycstats_time.SetSize(tot_cyc);

  sravg_cons_time.SetSize(tot_cyc);
  cycsyndep_time.SetSize(tot_cyc);
}

void LeabraCycleTask::StartCycle(int st_ct_cyc, int n_run_cyc) {
  send_netin_time.ResetUsed(st_ct_cyc, n_run_cyc);
  netin_integ_time.ResetUsed(st_ct_cyc, n_run_cyc);
  inhib_time.ResetUsed(st_ct_cyc, n_run_cyc);
  act_time.ResetUsed(st_ct_cyc, n_run_cyc);
  cycstats_time.ResetUsed(st_ct_cyc, n_run_cyc);

  sravg_cons_time.ResetUsed(st_ct_cyc, n_run_cyc);
  cycsyndep_time.ResetUsed(st_ct_cyc, n_run_cyc);
}

void LeabraCycleTask::EndStep(QAtomicInt& stage, RunWaitTime_List& time, int cyc,
                              int cur_net_cyc) {
  LeabraCycleThreadMgr* mg = mgr();
  time[cur_net_cyc]->EndRun();
  if(mg->tasks.size == 1 || !mg->sync_steps) {
    return;
  }
  int trg = (cyc+1) * mg->tasks.size;

  int cur_cnt = stage.fetchAndAddOrdered(1);
  if(cur_cnt == trg) { // we were the last guy
    return;
  }

  time[cur_net_cyc]->StartWait(false);

  while(cur_cnt < trg) {
    // taManagedThread::usleep(1); // just slows down a tiny bit, no value..
    cur_cnt = stage.loadAcquire();
  }

  time[cur_net_cyc]->EndWait();
}

void LeabraCycleTask::EndCycle(int cur_net_cyc) {
  send_netin_time[cur_net_cyc]->IncrAvg();
  netin_integ_time[cur_net_cyc]->IncrAvg();
  inhib_time[cur_net_cyc]->IncrAvg();
  act_time[cur_net_cyc]->IncrAvg();
  cycstats_time[cur_net_cyc]->IncrAvg();

  sravg_cons_time[cur_net_cyc]->IncrAvg();
  cycsyndep_time[cur_net_cyc]->IncrAvg();
}

void LeabraCycleTask::run() {
  LeabraCycleThreadMgr* mg = mgr();
  LeabraNetwork* net = (LeabraNetwork*)network.ptr();

  int st_ct_cyc = net->ct_cycle;  // our starting cycle
  int nmx;

  StartCycle(st_ct_cyc, mg->n_cycles);

  for(int cyc=0; cyc < mg->n_cycles; cyc++) {
    int cur_net_cyc = st_ct_cyc + cyc;
    // this replicates LeabraNetwork::Cycle()

    // Compute_SRAvg_State();
    if(task_id == 0) {        // first thread does this -- very fast -- get it done early
      net->Compute_SRAvg_State();
    }

    // Send_NetinDelta
    StartStep(send_netin_time, cur_net_cyc);
    nmx = net->snet_un_to_th_n.FastEl1d(task_id);
    for(int i=0; i<nmx; i++) {
      int uidx = net->snet_un_to_th.FastEl2d(i, task_id);
      LeabraUnit* un = (LeabraUnit*)net->units_flat[uidx];
      un->Send_NetinDelta(net, task_id);
    }
    EndStep(mg->stage_net, send_netin_time, cyc, cur_net_cyc);

    // Compute_NetinInteg();
    StartStep(netin_integ_time, cur_net_cyc);
    nmx = net->snet_un_to_th_n.FastEl1d(task_id);
    for(int i=0; i<nmx; i++) {
      int uidx = net->snet_un_to_th.FastEl2d(i, task_id);
      LeabraUnit* un = (LeabraUnit*)net->units_flat[uidx];
      un->Compute_NetinInteg(net, task_id);
    }
    EndStep(mg->stage_net_int, netin_integ_time, cyc, cur_net_cyc);

    // Compute_Inhib();
    StartStep(inhib_time, cur_net_cyc);
    for(int i=lay_st; i<lay_ed; i++) {
      if(i >= net->layers.leaves) continue;
      LeabraLayer* lay = (LeabraLayer*)net->layers.Leaf(i);
      if(lay->lesioned()) continue;
      lay->Compute_Inhib(net);
    }
    EndStep(mg->stage_inhib, inhib_time, cyc, cur_net_cyc);

    if(net->net_misc.lay_gp_inhib) {
      if(task_id == 0) {        // first thread does this..
        net->Compute_Inhib_LayGp();
        // todo: this needs to apply all the way out to layer groups -- also it is 
        // incompatible with the self setting.. probably need to get rid of self!
      }
    }

    // Compute_Act();
    StartStep(act_time, cur_net_cyc);
    nmx = net->snet_un_to_th_n.FastEl1d(task_id);
    for(int i=0; i<nmx; i++) {
      int uidx = net->snet_un_to_th.FastEl2d(i, task_id);
      LeabraUnit* un = (LeabraUnit*)net->units_flat[uidx];
      un->Compute_Act(net, task_id); // includes SRAvg
    }
    EndStep(mg->stage_act, act_time, cyc, cur_net_cyc);

    if(net->Compute_SRAvg_Cons_Test()) {
      StartStep(sravg_cons_time, cur_net_cyc);
      nmx = net->snet_un_to_th_n.FastEl1d(task_id);
      for(int i=0; i<nmx; i++) {
        int uidx = net->snet_un_to_th.FastEl2d(i, task_id);
        LeabraUnit* un = (LeabraUnit*)net->units_flat[uidx];
        un->Compute_SRAvg_Cons(net, task_id);
      }
      EndTime(sravg_cons_time, cur_net_cyc); // no sync -- no dependency with next
    }

    // Compute_CycleStats();
    StartStep(cycstats_time, cur_net_cyc);
    if(task_id == 0) {        // first thread does this..
      net->Compute_CycleStats_Pre();
    }
    for(int i=lay_st; i<lay_ed; i++) {
      if(i >= net->layers.leaves) continue;
      LeabraLayer* lay = (LeabraLayer*)net->layers.Leaf(i);
      if(lay->lesioned()) continue;
      lay->Compute_CycleStats(net);
    }
    EndStep(mg->stage_cyc_stats, cycstats_time, cyc, cur_net_cyc);

    // Compute_CycSynDep();
    if(net->net_misc.cyc_syn_dep) {
      StartStep(cycsyndep_time, cur_net_cyc);
      if(net->ct_cycle % net->net_misc.syn_dep_int == 0) {
        nmx = net->snet_un_to_th_n.FastEl1d(task_id);
        for(int i=0; i<nmx; i++) {
          int uidx = net->snet_un_to_th.FastEl2d(i, task_id);
          LeabraUnit* un = (LeabraUnit*)net->units_flat[uidx];
          un->Compute_CycSynDep(net, task_id);
        }
      }
      // no sync needed here really -- no dependency on next steps
      EndTime(cycsyndep_time, cur_net_cyc);
    }

    // Compute_MidMinus();           // check for mid-minus and run if so (PBWM)
    // todo: figure this one out!

    if(task_id == 0) {
      net->cycle++;
      net->ct_cycle++;
      net->time += net->time_inc; // always increment time..
    }
    EndCycle(cur_net_cyc);
  }
}

void LeabraCycleTask::ThreadReport(DataTable& dt) {
  int idx;
  DataCol* thc = dt.FindMakeColName("thread", idx, VT_INT);
  DataCol* cyc = dt.FindMakeColName("cycle", idx, VT_INT);
  DataCol* unsc = dt.FindMakeColName("un_st", idx, VT_INT);
  DataCol* unec = dt.FindMakeColName("un_ed", idx, VT_INT);
  DataCol* lysc = dt.FindMakeColName("lay_st", idx, VT_INT);
  DataCol* lyec = dt.FindMakeColName("lay_ed", idx, VT_INT);
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

  LeabraNetwork* net = (LeabraNetwork*)network.ptr();
  int tot_cyc = net->ct_time.total_cycles;

  float rescale = 1.0e6;        // how many microseconds
  for(int i=0; i<tot_cyc; i++) {
    dt.AddBlankRow();
    thc->SetValAsInt(task_id, -1);
    cyc->SetValAsInt(i, -1);
    unsc->SetValAsInt(un_scon_st, -1);
    unec->SetValAsInt(un_scon_ed, -1);
    lysc->SetValAsInt(lay_st, -1);
    lyec->SetValAsInt(lay_ed, -1);

    sn_r->SetValAsFloat(send_netin_time[i]->run.avg_used.avg * rescale, -1);
    sn_w->SetValAsFloat(send_netin_time[i]->wait.avg_used.avg * rescale, -1);
    ni_r->SetValAsFloat(netin_integ_time[i]->run.avg_used.avg * rescale, -1);
    ni_w->SetValAsFloat(netin_integ_time[i]->wait.avg_used.avg * rescale, -1);
    ih_r->SetValAsFloat(inhib_time[i]->run.avg_used.avg * rescale, -1);
    ih_w->SetValAsFloat(inhib_time[i]->wait.avg_used.avg * rescale, -1);
    ac_r->SetValAsFloat(act_time[i]->run.avg_used.avg * rescale, -1);
    ac_w->SetValAsFloat(act_time[i]->wait.avg_used.avg * rescale, -1);
    cs_r->SetValAsFloat(cycstats_time[i]->run.avg_used.avg * rescale, -1);
    cs_w->SetValAsFloat(cycstats_time[i]->wait.avg_used.avg * rescale, -1);
  }
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
  int tot_cyc = net->ct_time.total_cycles * 2;              // buffer

  // allocate units to threads based on number of sending connections
  float cons_per_thr = net->n_cons_cost / (float)tasks.size;
  if(cons_per_thr < 1.0f)          // this shouldn't happen..
    cons_per_thr = 1.0f;
  int un_idx = 1;
  for(int i=0;i<tasks.size;i++) {
    LeabraCycleTask* uct = (LeabraCycleTask*)tasks[i];
    uct->network = net;
    uct->InitTimers(tot_cyc);

    float cons_th = 0.0f;
    uct->un_scon_st = un_idx;
    while(cons_th < cons_per_thr && un_idx < net->units_flat.size) {
      LeabraUnit* u = (LeabraUnit*)net->units_flat[un_idx++];
      cons_th += u->n_send_cons_cost;
    }
    uct->un_scon_ed = un_idx;
  }
  LeabraCycleTask* uct = (LeabraCycleTask*)tasks.Peek(); // last one
  uct->un_scon_ed = net->units_flat.size;

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

void LeabraCycleThreadMgr::ThreadReport(DataTable* table) {
  if(!table) return;
  table->StructUpdate(true);
  table->ResetData();

  for(int i=0; i<tasks.size; i++) {
    LeabraCycleTask* uct = (LeabraCycleTask*)tasks[i];
    uct->ThreadReport(*table);
  }
  table->StructUpdate(false);
}

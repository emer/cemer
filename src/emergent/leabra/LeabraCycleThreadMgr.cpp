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


TA_BASEFUNS_CTORS_DEFN(LeabraCycleThreadMgr);

TA_BASEFUNS_CTORS_DEFN(LeabraCycleTask);

void LeabraCycleTask::Initialize() {
  uidx_st = 0;
  uidx_ed = 0;
  lay_st = 0;
  lay_ed = 0;
  avg_run_time = 0.0f;
  avg_wait_time = 0.0f;
  avg_time_n = 0;
}

void LeabraCycleTask::Destroy() {
  network.CutLinks();
}

void LeabraCycleTask::SyncAtom(QAtomicInt& stage) {
  LeabraCycleThreadMgr* mg = mgr();
  if(mg->tasks.size == 1) return;
  int cur_cnt = stage.fetchAndAddOrdered(1);
  if(cur_cnt >= mg->tasks.size) // we were the last guy
    return;

  run_time.EndTimer();
  wait_time.StartTimer(false);

  // todo: could try other sync's here..
  while(cur_cnt < mg->tasks.size) {
    taManagedThread::usleep(1);
    cur_cnt = stage.load();
  }

  wait_time.EndTimer();
  run_time.StartTimer(false);
}

void LeabraCycleTask::run() {
  LeabraCycleThreadMgr* mg = mgr();
  LeabraNetwork* net = (LeabraNetwork*)network.ptr();

  wait_time.ResetUsed();

  run_time.StartTimer(true);    // reset

  for(int cyc=0; cyc < mg->n_cycles; cyc++) {
    // this replicates LeabraNetwork::Cycle()
    // Send_NetinDelta

    for(int i=uidx_st; i<uidx_ed; i++) {
      LeabraUnit* un = (LeabraUnit*)net->units_flat[i];
      un->Send_NetinDelta(net, task_id);
    }
    if(mg->sync_steps)
      SyncAtom(mg->stage_net);

    // Compute_NetinInteg();
    for(int i=uidx_st; i<uidx_ed; i++) {
      LeabraUnit* un = (LeabraUnit*)net->units_flat[i];
      un->Compute_NetinInteg(net, task_id);
    }
    if(mg->sync_steps)
      SyncAtom(mg->stage_net_int);

    // Compute_NetinStats();
    // Compute_Inhib();
    // Compute_ApplyInhib();
    if(!net->net_misc.lay_gp_inhib) {
      for(int i=lay_st; i<lay_ed; i++) {
        if(i >= net->layers.leaves) continue;
        LeabraLayer* lay = (LeabraLayer*)net->layers.Leaf(i);
        lay->Compute_NetinStats(net);
        lay->Compute_Inhib(net);
        lay->Compute_ApplyInhib(net);
        // todo: break apply inhib out into separate steps -- unit-level one should be
        // done in unit loop, not layer loop..
      }
      if(mg->sync_steps)
        SyncAtom(mg->stage_inhib);
    }
    else {
      // layer-group inhibition is a LOT more expensive coordination-wise
      for(int i=lay_st; i<lay_ed; i++) {
        if(i >= net->layers.leaves) continue;
        LeabraLayer* lay = (LeabraLayer*)net->layers.Leaf(i);
        lay->Compute_NetinStats(net);
        lay->Compute_Inhib(net);
      }
      if(mg->sync_steps)
        SyncAtom(mg->stage_inhib_lay);
      if(task_id == 0) {        // first thread does this..
        net->Compute_Inhib_LayGp();
      }
      if(mg->sync_steps)
        SyncAtom(mg->stage_inhib_gp);
      for(int i=lay_st; i<lay_ed; i++) {
        if(i >= net->layers.leaves) continue;
        LeabraLayer* lay = (LeabraLayer*)net->layers.Leaf(i);
        lay->Compute_ApplyInhib(net);
      }
      if(mg->sync_steps)
        SyncAtom(mg->stage_inhib);
    }

    // Compute_Act();
    for(int i=uidx_st; i<uidx_ed; i++) {
      LeabraUnit* un = (LeabraUnit*)net->units_flat[i];
      un->Compute_Act(net, task_id);
    }
    if(mg->sync_steps)
      SyncAtom(mg->stage_act);

    // Compute_CycleStats();
    if(task_id == 0) {        // first thread does this..
      net->Compute_CycleStats(); // todo: need to break this out into two separate ones
      // one that is layer-parallel and one that is not..
    }

    // Compute_CycSynDep();
    if(net->net_misc.cyc_syn_dep) {
      if(net->ct_cycle % net->net_misc.syn_dep_int == 0) {
        for(int i=uidx_st; i<uidx_ed; i++) {
          LeabraUnit* un = (LeabraUnit*)net->units_flat[i];
          un->Compute_CycSynDep(net, task_id);
        }
      }
      // no sync needed here really -- no dependency on next steps
    }

    // Compute_SRAvg_State();
    if(task_id == 0) {        // first thread does this..
      net->Compute_SRAvg_State();
    }

    if(mg->sync_steps)
      SyncAtom(mg->stage_sravg_state);

    // Compute_SRAvg();
    for(int i=uidx_st; i<uidx_ed; i++) {
      LeabraUnit* un = (LeabraUnit*)net->units_flat[i];
      un->Compute_SRAvg(net, task_id);
    }

    if(mg->sync_steps)
      SyncAtom(mg->stage_sravg);

    // Compute_MidMinus();           // check for mid-minus and run if so (PBWM)
    // todo: figure this one out!

    if(task_id == 0) {        // first thread does this..
      mg->InitStages();           // start stages over within loop
    }
  }

  run_time.EndTimer();

  avg_time_n++;
  if(avg_time_n == 1) {
    avg_run_time = run_time.s_used;
    avg_wait_time = wait_time.s_used;
  }
  else {
    // continuous integrating of average
    avg_run_time += (run_time.s_used + avg_run_time * (float)(avg_time_n - 1)) /
      (float)avg_time_n;
    avg_wait_time += (wait_time.s_used + avg_wait_time * (float)(avg_time_n - 1)) /
      (float)avg_time_n;
  }
}

String LeabraCycleTask::ThreadReport() {
  String rval;
  rval << task_id << ": " << "un: " << uidx_st << " - " << uidx_ed
       << "  lay: " << lay_st << " - " << lay_ed
       << "  || run: " << avg_run_time
       << " wait: " << avg_wait_time
       << "\n";
  return rval;
}

void LeabraCycleThreadMgr::Initialize() {
  n_threads_act = 1;
  n_cycles = 1;
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
  int cons_per_thr = net->n_cons / tasks.size;
  if(cons_per_thr < 1)          // this shouldn't happen..
    cons_per_thr = 1;
  int un_idx = 1;
  for(int i=0;i<tasks.size;i++) {
    LeabraCycleTask* uct = (LeabraCycleTask*)tasks[i];
    uct->network = net;

    int cons_th = 0;
    uct->uidx_st = un_idx;
    while(cons_th < cons_per_thr && un_idx < net->units_flat.size) {
      LeabraUnit* u = (LeabraUnit*)net->units_flat[un_idx++];
      cons_th += u->n_send_cons;
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

  stage_inhib_lay = 0;
  stage_inhib_gp = 0;
  stage_inhib = 0;

  stage_act = 0;
  stage_cyc_stats = 0;

  stage_sravg_state = 0;
  stage_sravg = 0;
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

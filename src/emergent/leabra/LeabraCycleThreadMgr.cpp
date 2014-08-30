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
  uidx_st = -1;
  uidx_ed = -1;
}

void LeabraCycleTask::Destroy() {
  network.CutLinks();
}

void LeabraCycleTask::SyncAtom(QAtomicInt& stage) {
  LeabraCycleThreadMgr* mg = mgr();
  int cur_cnt = stage.fetchAndAddOrdered(1);
  if(cur_cnt >= mg->tasks.size) // we were the last guy
    return;

  run_time.EndTimer();
  wait_time.StartTimer(false);

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

  // the entire scope of running is here!
  // Send_NetinDelta
  for(int i=uidx_st; i<uidx_ed; i++) {
    LeabraUnit* un = (LeabraUnit*)net->units_flat[i];
    un->Send_NetinDelta(net, task_id);
  }
  if(mg->sync_steps)
    SyncAtom(mg->stage_net);

  for(int i=uidx_st; i<uidx_ed; i++) {
    LeabraUnit* un = (LeabraUnit*)net->units_flat[i];
    un->Send_NetinDelta_Post(net);
//    net->send_netin_tmp[??] = 0.0f;  // clear it!
  }
  //  Compute_ExtraNetin();         // layer level extra netinput for special algorithms
  // Compute_NetinInteg();
  // Compute_NetinStats();

  // Compute_Inhib();
  // Compute_ApplyInhib();

  // Compute_Act();
  // Compute_CycleStats();

  // Compute_CycSynDep();

  // Compute_SRAvg_State();
  // Compute_SRAvg();              // note: only ctleabra variants do con-level compute here
  // Compute_MidMinus();           // check for mid-minus and run if so (PBWM)

  run_time.EndTimer();
}

void LeabraCycleThreadMgr::Initialize() {
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
  int cons_per_thr = net->n_cons / threads.size;
  int un_idx = 1;
  int cons_th = 0;
  for(int i=0;i<tasks.size;i++) {
    LeabraCycleTask* uct = (LeabraCycleTask*)tasks[i];
    uct->network = net;

    uct->uidx_st = un_idx;
    while(cons_th < cons_per_thr && un_idx < net->units_flat.size) {
      LeabraUnit* u = (LeabraUnit*)net->units_flat[un_idx++];
      cons_th += u->n_send_cons;
    }
    uct->uidx_ed = un_idx;
  }
  if(un_idx < net->units_flat.size) {
    LeabraCycleTask* uct = (LeabraCycleTask*)tasks.Peek(); // last one
    uct->uidx_ed = net->units_flat.size;
  }

  // todo: allocate layers to threads

}

bool LeabraCycleThreadMgr::CanRun() {
  Network* net = network();

  bool other_reasons = (net->units_flat.size < min_units
                        || net->units_flat.size < tasks.size);

  if(n_threads == 1 || other_reasons) {
    return false;
  }
  return true;
}

void LeabraCycleThreadMgr::InitStages() {
  stage_net = 0;
  stage_net_post = 0;
  stage_exnet = 0;
  stage_net_int = 0;
  stage_net_stats = 0;
  stage_inhib = 0;
  stage_applyinhib = 0;
  stage_act = 0;
  stage_cyc_stats = 0;
  stage_cyc_syndep = 0;
  stage_sravg = 0;
}

void LeabraCycleThreadMgr::Run() {
  using_threads = true;

  InitStages();

  RunThreads();         // then run the subsidiary guys
  tasks[0]->run();      // run our own set..

  // finally, always need to sync at end to ensure that everyone is done!
  SyncThreads();
}


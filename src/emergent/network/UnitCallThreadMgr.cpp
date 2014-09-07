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

#include "UnitCallThreadMgr.h"
#include <Network>

#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(UnitCallThreadMgr);

TA_BASEFUNS_CTORS_DEFN(UnitCallTask);

void UnitCallTask::Initialize() {
  unit_call = NULL;
}

void UnitCallTask::Destroy() {
  network.CutLinks();
  unit_call = NULL;
}

void UnitCallTask::run() {
  UnitCallThreadMgr* mg = mgr();
  const int nib_stop = mg->nibble_stop;
#if (QT_VERSION >= 0x050000)
  const int nib_start = mg->nibble_i.loadAcquire();
#else
  const int nib_start = (int)mg->nibble_i;
#endif


  if(nib_start == nib_stop) return; // all done!

  if(nib_start < nib_stop) {            // a forward run
    const int nib_chnk = mg->nibble_chunk;
    while(true) {
      int nxt_uidx = mg->nibble_i.fetchAndAddOrdered(nib_chnk);
      if(nxt_uidx >= nib_stop) break;
      const int mx = MIN(nib_stop, nxt_uidx + nib_chnk);
      for(int i=nxt_uidx; i <mx; i++) {
        Unit* un = network->units_flat[i];
        unit_call->call(un, network, task_id); // task id indicates threading, and which thread
      }
      if(mx == nib_stop) break;         // we're the last guy
    }
  }
  else {                        // backwards!
    const int nib_chnk = -mg->nibble_chunk;
    while(true) {
      int nxt_uidx = mg->nibble_i.fetchAndAddOrdered(nib_chnk);
      if(nxt_uidx < nib_stop) break;
      const int mx = MAX(nib_stop, nxt_uidx + nib_chnk);
      for(int i=nxt_uidx; i>=mx; i--) {
        Unit* un = network->units_flat[i];
        unit_call->call(un, network, task_id); // task id indicates threading, and which thread
      }
      if(mx == nib_stop) break;         // we're the last guy
    }
  }
}

void UnitCallThreadMgr::Initialize() {
  nibble_chunk = 8;
  min_units = 3000;
  ignore_lay_sync = false;
  nibble_i = -1;
  nibble_stop = 0;
  using_threads = false;
  n_threads_prev = n_threads;
  task_type = &TA_UnitCallTask;
}

void UnitCallThreadMgr::Destroy() {
}

void UnitCallThreadMgr::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(!taMisc::is_loading && n_threads != n_threads_prev) {
    network()->BuildUnits_Threads(); // calls InitAll where n_threads_prev is set..
  }
}

void UnitCallThreadMgr::InitAll() {
  if((threads.size == n_threads-1) && (tasks.size == n_threads)) return; // fast bail if same
  n_threads_prev = n_threads;
  inherited::InitAll();
  Network* net = network();
  for(int i=0;i<tasks.size;i++) {
    UnitCallTask* uct = (UnitCallTask*)tasks[i];
    uct->network = net;
  }
}

void UnitCallThreadMgr::Run(ThreadUnitCall* unit_call,
                            bool backwards, bool layer_sync) {
  Network* net = network();

  bool other_reasons = (net->units_flat.size < min_units
                        || net->units_flat.size < tasks.size);

  if(n_threads == 1 || other_reasons) {

    if(get_timing && n_threads == 1 && !other_reasons) { // only include if running in 1 thread only, and if n_threads was larger, it would not have been run in thread0
      total_time.StartTimer(false);     // don't reset
      run_time.StartTimer(false);               // don't reset
    }

    RunThread0(unit_call, backwards);

    if(get_timing && n_threads == 1 && !other_reasons) { // only include if running in 1 thread only, and if n_threads was larger, it would not have been run in thread0
      total_time.EndTimer();
      run_time.EndTimer();
    }
  }
  else {
    const int nt = tasks.size;
    for(int i=0;i<nt;i++) {
      UnitCallTask* uct = (UnitCallTask*)tasks[i];
      uct->unit_call = unit_call;
    }

    if(backwards) {
      if(layer_sync && !ignore_lay_sync)
        RunThreads_BkwdLaySync(unit_call);
      else
        RunThreads_BkwdNetSync(unit_call);
    }
    else {
      if(layer_sync && !ignore_lay_sync)
        RunThreads_FwdLaySync(unit_call);
      else
        RunThreads_FwdNetSync(unit_call);
    }
  }
}

void UnitCallThreadMgr::RunThread0(ThreadUnitCall* unit_call, bool backwards) {
  using_threads = false;
  Network* net = network();
  const int nu = net->units_flat.size;
  if(backwards) {
    for(int i=nu-1;i>=1;i--) {  // 0 = dummy idx
      unit_call->call(net->units_flat[i], net, -1); // -1 indicates no threading
    }
  }
  else {                        // forwards
    for(int i=1;i<nu;i++) {     // 0 = dummy idx
      unit_call->call(net->units_flat[i], net, -1); // -1 indicates no threading
    }
  }
}

void UnitCallThreadMgr::RunThreads_FwdNetSync(ThreadUnitCall* unit_call) {
  // note: units_flat[0] is a dummy so we have to add 1 to all idx's and skip over it
  using_threads = true;
  Network* net = network();
  const int nu = net->units_flat.size-1;        // 0 = dummy idx
  const int nt = tasks.size;

  nibble_i = 1;
  nibble_stop = 1+nu;

  RunThreads();         // then run the subsidiary guys

  tasks[0]->run();      // run our own set..

  // note: all the nibbling is automatic within the single run() deploy
  // finally, always need to sync at end to ensure that everyone is done!
  SyncThreads();
}

void UnitCallThreadMgr::RunThreads_BkwdNetSync(ThreadUnitCall* unit_call) {
  // note: units_flat[0] is a dummy so we have to add 1 to all idx's and skip over it
  using_threads = true;
  Network* net = network();
  const int nu = net->units_flat.size-1;        // 0 = dummy idx
  const int nt = tasks.size;
  nibble_i = nu;    // where to start nibbling
  nibble_stop = 1;      // 0 = dummy idx

  RunThreads();         // then run the subsidiary guys

  tasks[0]->run();      // run our own set..

  // note: all the nibbling is automatic within the single run() deploy
  // finally, always need to sync at end to ensure that everyone is done!
  SyncThreads();
}

void UnitCallThreadMgr::RunThreads_FwdLaySync(ThreadUnitCall* unit_call) {
  using_threads = true;
  Network* net = network();
  const int nt = tasks.size;

  // note: this has same logic as net sync but all within each layer
  // IMPORTANT: lay sync guys MUST have all units.leaves units in units_flat --
  // the run code assumes this is true

  FOREACH_ELEM_IN_GROUP(Layer, lay, net->layers) {
    if(lay->lesioned()) continue; // don't even add units from lesioned layers!!
    int st_idx = lay->units_flat_idx;
    const int nu = lay->units.leaves;

    if(nu < min_units || nu < nt) {
      // run locally
      for(int i=0;i<nu;i++) {
        unit_call->call(net->units_flat[st_idx + i], net, -1); // -1 indicates no threading
      }
    }
    else {
      nibble_i = st_idx;
      nibble_stop = st_idx + nu;

      RunThreads();     // then run the subsidiary guys

      tasks[0]->run();          // run our own set..

      // note: all the nibbling is automatic within the single run() deploy
      // finally, always need to sync at end to ensure that everyone is done!
      SyncThreads();
    }
  }
}

void UnitCallThreadMgr::RunThreads_BkwdLaySync(ThreadUnitCall* unit_call) {
  using_threads = true;
  Network* net = network();
  const int nt = tasks.size;

  // note: this has same logic as net sync but all within each layer
  // IMPORTANT: lay sync guys MUST have all units.leaves units in units_flat --
  // the run code assumes this is true

  int li;
  for(li=net->layers.leaves-1; li>=0; li--) {
    Layer* lay = net->layers.Leaf(li);
    if(lay->lesioned()) continue; // don't even add units from lesioned layers!!
    int st_idx = lay->units_flat_idx;
    const int nu = lay->units.leaves;

    if(nu < min_units || nu < nt) {
      // run locally
      for(int i=nu-1;i>=0;i--) {
        unit_call->call(net->units_flat[st_idx + i], net, -1); // -1 indicates no threading
      }
    }
    else {
      nibble_i = st_idx + nu-1;
      nibble_stop = st_idx;

      RunThreads();     // then run the subsidiary guys

      tasks[0]->run();          // run our own set..

      // note: all the nibbling is automatic within the single run() deploy
      // finally, always need to sync at end to ensure that everyone is done!
      SyncThreads();
    }
  }
}


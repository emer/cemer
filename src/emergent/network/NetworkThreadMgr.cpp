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

#include "NetworkThreadMgr.h"
#include <Network>

#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(NetworkThreadMgr);

TA_BASEFUNS_CTORS_DEFN(NetworkThreadTask);

void NetworkThreadTask::Initialize() {
}

void NetworkThreadTask::Destroy() {
  network.CutLinks();
}

void NetworkThreadTask::run() {
  NetworkThreadMgr* mg = mgr();
  meth_call.call(network, task_id); // task id indicates threading, and which thread
}


void NetworkThreadTask::SyncSpin0(int usec_wait) {
  NetworkThreadMgr* mg = mgr();

#if (QT_VERSION >= 0x050000)
  const int cur_step = mg->sync_step0.loadAcquire();
#else
  const int cur_step = (int)mg->sync_step0;
#endif

  const int nxt_step = cur_step+1;
  const int trg = nxt_step * mg->tasks.size;

  // acquire vs. ordered is not really a big deal here -- generating a release is
  // essentially automatic whenever a volitile variable is written to anyway
  int cur_cnt = mg->sync_ctr0.fetchAndAddOrdered(1);

  if(cur_cnt == trg) { // we were the last guy
    mg->sync_step0.testAndSetOrdered(cur_step, nxt_step);
    return;
  }

  if(mg->get_timing) {
    wait_time.StartTimer(true); // this is the only reason this has to be in Task guy
  }

  while(cur_cnt < trg) {
    if(usec_wait > 0)
      taManagedThread::usleep(usec_wait);
#if (QT_VERSION >= 0x050000)
    cur_cnt = mg->sync_ctr0.loadAcquire();
#else
    cur_cnt = (int)mg->sync_ctr0;
#endif
  }
  mg->sync_step0.testAndSetOrdered(cur_step, nxt_step);

  if(mg->get_timing) {
    wait_time.EndIncrAvg();
  }
}

void NetworkThreadTask::SyncSpin1(int usec_wait) {
  NetworkThreadMgr* mg = mgr();

#if (QT_VERSION >= 0x050000)
  const int cur_step = mg->sync_step1.loadAcquire();
#else
  const int cur_step = (int)mg->sync_step1;
#endif

  const int nxt_step = cur_step+1;
  const int trg = nxt_step * mg->tasks.size;

  // acquire vs. ordered is not really a big deal here -- generating a release is
  // essentially automatic whenever a volitile variable is written to anyway
  int cur_cnt = mg->sync_ctr1.fetchAndAddOrdered(1);

  if(cur_cnt == trg) { // we were the last guy
    mg->sync_step1.testAndSetOrdered(cur_step, nxt_step);
    return;
  }

  if(mg->get_timing) {
    wait_time.StartTimer(true); // this is the only reason this has to be in Task guy
  }

  while(cur_cnt < trg) {
    if(usec_wait > 0)
      taManagedThread::usleep(usec_wait);
#if (QT_VERSION >= 0x050000)
    cur_cnt = mg->sync_ctr1.loadAcquire();
#else
    cur_cnt = (int)mg->sync_ctr1;
#endif
  }
  mg->sync_step1.testAndSetOrdered(cur_step, nxt_step);

  if(mg->get_timing) {
    wait_time.EndIncrAvg();
  }
}

void NetworkThreadTask::SyncSpin2(int usec_wait) {
  NetworkThreadMgr* mg = mgr();

#if (QT_VERSION >= 0x050000)
  const int cur_step = mg->sync_step2.loadAcquire();
#else
  const int cur_step = (int)mg->sync_step2;
#endif

  const int nxt_step = cur_step+1;
  const int trg = nxt_step * mg->tasks.size;

  // acquire vs. ordered is not really a big deal here -- generating a release is
  // essentially automatic whenever a volitile variable is written to anyway
  int cur_cnt = mg->sync_ctr2.fetchAndAddOrdered(1);

  if(cur_cnt == trg) { // we were the last guy
    mg->sync_step2.testAndSetOrdered(cur_step, nxt_step);
    return;
  }

  if(mg->get_timing) {
    wait_time.StartTimer(true); // this is the only reason this has to be in Task guy
  }

  while(cur_cnt < trg) {
    if(usec_wait > 0)
      taManagedThread::usleep(usec_wait);
#if (QT_VERSION >= 0x050000)
    cur_cnt = mg->sync_ctr2.loadAcquire();
#else
    cur_cnt = (int)mg->sync_ctr2;
#endif
  }
  mg->sync_step2.testAndSetOrdered(cur_step, nxt_step);

  if(mg->get_timing) {
    wait_time.EndIncrAvg();
  }
}


////////////////////////////////////////////////////////////
//              Mgr

void NetworkThreadMgr::Initialize() {
  task_type = &TA_NetworkThreadTask;
}

void NetworkThreadMgr::Destroy() {
}

void NetworkThreadMgr::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(!taMisc::is_loading && n_threads != n_threads_prev) {
    network()->UpdtAfterNetMod();
  }
}

void NetworkThreadMgr::InitAll() {
  if((threads.size == n_threads-1) && (tasks.size == n_threads)) return; // fast bail if same
  n_threads_prev = n_threads;
  inherited::InitAll();
  Network* net = network();
  for(int i=0;i<tasks.size;i++) {
    NetworkThreadTask* uct = (NetworkThreadTask*)tasks[i];
    uct->network = net;
  }
}

void NetworkThreadMgr::Run(NetworkThreadCall& meth_call) {
  sync_ctr0 = 0;
  sync_step0 = 0;
  sync_ctr1 = 0;
  sync_step1 = 0;
  sync_ctr2 = 0;
  sync_step2 = 0;

  Network* net = network();
  const int nt = tasks.size;
  for(int i=0;i<nt;i++) {
    NetworkThreadTask* ntt = (NetworkThreadTask*)tasks[i];
    ntt->meth_call = meth_call;
  }

  inherited::Run();
}

void NetworkThreadMgr::SyncSpin(int thread_no, int sync_no, int usec_wait) {
  NetworkThreadTask* ntt = (NetworkThreadTask*)tasks[thread_no];
  switch(sync_no) {
  case 0:
    ntt->SyncSpin0(usec_wait);
    break;
  case 1:
    ntt->SyncSpin1(usec_wait);
    break;
  case 2:
    ntt->SyncSpin2(usec_wait);
    break;
  }
}


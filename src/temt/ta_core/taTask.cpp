// Copyright, 1995-2013, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of The Emergent Toolkit
//
//   This library is free software; you can redistribute it and/or
//   modify it under the terms of the GNU Lesser General Public
//   License as published by the Free Software Foundation; either
//   version 2.1 of the License, or (at your option) any later version.
//
//   This library is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//   Lesser General Public License for more details.

#include "taTask.h"

#include <taThreadMgr>

TA_BASEFUNS_CTORS_DEFN(taTask);
SMARTREF_OF_CPP(taTask);


void taTask::Initialize() {
  task_id = -1; 
}

taThreadMgr* taTask::mgr() {
  return (taThreadMgr*)owner->GetOwner();
}

void taTask::SyncSpin0(int usec_wait) {
  taThreadMgr* mg = mgr();

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

void taTask::SyncSpin1(int usec_wait) {
  taThreadMgr* mg = mgr();

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

void taTask::SyncSpin2(int usec_wait) {
  taThreadMgr* mg = mgr();

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


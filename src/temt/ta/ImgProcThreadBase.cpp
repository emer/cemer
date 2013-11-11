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

#include "ImgProcThreadBase.h"

#include <taMisc>

void ImgProcCallTask::Initialize() {
  img_proc_call = NULL;
}

void ImgProcCallTask::Destroy() {
  img_proc_call = NULL;
}

void ImgProcCallTask::run() {
  ImgProcCallThreadMgr* mg = mgr();
  ImgProcThreadBase* base = mg->img_proc();

  // all nibbling all the time
  const int nib_chnk = mg->nibble_chunk;
  const int nib_stop = mg->n_cmp_units;

  while(true) {
    int nxt_uidx = mg->nibble_i.fetchAndAddOrdered(nib_chnk);
    if(nxt_uidx >= nib_stop) break;
    const int mx = MIN(nib_stop, nxt_uidx + nib_chnk);
    for(int i=nxt_uidx; i <mx; i++) {
      img_proc_call->call(base, i, task_id); // task id indicates threading, and which thread
    }
    if(mx == nib_stop) break;           // we're the last guy
  }
}

/////////////////////////////////
//              Mgr

void ImgProcCallThreadMgr::Initialize() {
  min_units = taMisc::thread_defaults.min_units;
  nibble_chunk = taMisc::thread_defaults.nibble_chunk;
  task_type = &TA_ImgProcCallTask;
}

void ImgProcCallThreadMgr::Destroy() {
}

void ImgProcCallThreadMgr::Run(ThreadImgProcCall* img_proc_call, int n_cmp_un) {
  InitAll();                    // make sure

  n_cmp_units = n_cmp_un;

  ImgProcThreadBase* base = img_proc();
  if(n_threads == 1 || n_cmp_units < min_units || n_cmp_units < tasks.size) {
    for(int i=0;i<n_cmp_units;i++) {
      img_proc_call->call(base, i, -1); // -1 indicates no threading
    }
  }
  else {
    // everything is done with nibbling -- so much more effective at load balancing
    nibble_i = 0;

    // set the call
    for(int i=0;i<tasks.size;i++) {
      ImgProcCallTask* uct = (ImgProcCallTask*)tasks[i];
      uct->img_proc_call = img_proc_call;
    }

    // then run the subsidiary guys
    RunThreads();

    tasks[0]->run();            // run our own set..

    // finally, always need to sync at end to ensure that everyone is done!
    SyncThreads();
  }
}

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
  meth_call.call(network, task_id); // task id indicates threading, and which thread
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
  const int nt = tasks.size;
  for(int i=0;i<nt;i++) {
    NetworkThreadTask* ntt = (NetworkThreadTask*)tasks[i];
    ntt->meth_call = meth_call;
  }

  inherited::Run();
}


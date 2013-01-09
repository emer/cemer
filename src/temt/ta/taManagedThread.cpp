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

#include "taManagedThread.h"

taManagedThread::taManagedThread(taThreadMgr* mg)
  : mgr(mg)
{
  //  m_task = NULL;
  m_active = false;
  m_running = false;
  m_stop_req = false;
}

taManagedThread::~taManagedThread() {
  m_task.CutLinks();
}

// this is the QThread run state
void taManagedThread::run() {
  if(!mgr) return;		// should not happen!

  mgr->wait_mutex.lock();	// expects to be in lock at start of loop
  m_active = true;
  mgr->n_active++;		// add one to the active list -- in mutex
  while(!m_stop_req) {
    mgr->wait.wait(&mgr->wait_mutex); // we wait until we get the go signal
    // note: comes out of the wait with mutex locked

    if(m_stop_req) break;	// bail -- still locked!
 
    // update all state info here while still under mutex -- keeps it fully sane
    m_running = true;
    mgr->n_running++; // add one to the running list
    mgr->n_started++; // add one to the started list

    mgr->wait_mutex.unlock();	// now finally ready to let go and run free

    if(m_task) {
      m_task->run();
    }

    mgr->wait_mutex.lock();	// lock at this point, so the full trip into wait is deterministic
    m_running = false;
    mgr->n_running--;   // subtract one from running list
  }
  m_active = false;
  mgr->n_active--;
  mgr->wait_mutex.unlock();	// only unlock at final exit
}

void taManagedThread::setTask(taTask* t) {
  if (m_task.ptr() == t) return;
  m_task = t;
}

void taManagedThread::stopMe() {
  m_stop_req = true;
}

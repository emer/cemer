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
#include <taThreadMgr>

#ifdef TA_OS_LINUX
#include <pthread.h>
#endif

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

#ifdef TA_OS_LINUX
#if 0
  // set processor affinity on linux, based on task number
  // this is just for experimentation -- does not deal with load balancing!!

   cpu_set_t cpuset;
   CPU_ZERO(&cpuset);
   CPU_SET(task()->task_id, &cpuset);

   pthread_t current_thread = pthread_self();    
   pthread_setaffinity_np(current_thread, sizeof(cpu_set_t), &cpuset);
#endif
#endif
  
  if(mgr->spin_wait)
    run_spin();
  else
    run_wait();
}

void taManagedThread::run_wait() {
  mgr->wait_mutex.lock();	// expects to be in lock at start of loop
  m_active = true;
  mgr->n_active.fetchAndAddOrdered(1);	// add one to the active list -- in mutex
  while(!m_stop_req) {
    mgr->wait.wait(&mgr->wait_mutex); // we wait until we get the go signal
    // note: comes out of the wait with mutex locked

    if(m_stop_req) break;	// bail -- still locked!
 
    // update all state info here while still under mutex -- keeps it fully sane
    m_running = true;
    mgr->n_running.fetchAndAddOrdered(1); // add one to the running list
    mgr->n_started.fetchAndAddOrdered(1); // add one to the started list

    mgr->wait_mutex.unlock();	// now finally ready to let go and run free

    if(m_task) {
      m_task->run();
    }

    mgr->wait_mutex.lock();	// lock now, so full trip into wait is derministic
    m_running = false;
    mgr->n_running.fetchAndAddOrdered(-1);   // subtract one from running list
  }
  m_active = false;
  mgr->n_active.fetchAndAddOrdered(-1);
  mgr->wait_mutex.unlock();	// only unlock at final exit
}

void taManagedThread::run_spin() {
  m_active = true;
  mgr->n_active.fetchAndAddOrdered(1);	// add one to the active list
  while(!m_stop_req) {
    if(m_stop_req) break;	// bail -- we end

#if (QT_VERSION >= 0x050000)
    const int cur_to_run = mgr->n_to_run.loadAcquire();
#else
    const int cur_to_run = (int)mgr->n_to_run;
#endif

    if(cur_to_run == 0) {       // nothing to run yet
      if(mgr->sync_sleep_usec > 0)
        taManagedThread::usleep(mgr->sync_sleep_usec);
      continue;
    }
 
    m_running = true;
    mgr->n_running.fetchAndAddOrdered(1); // add one to the running list
    mgr->n_started.fetchAndAddOrdered(1); // add one to the started list

    if(m_task) {
      m_task->run();
    }

    m_running = false;
    mgr->n_running.fetchAndAddOrdered(-1);   // subtract one from running list
  }
  m_active = false;
  mgr->n_active.fetchAndAddOrdered(-1);
}

void taManagedThread::setTask(taTask* t) {
  if (m_task.ptr() == t) return;
  m_task = t;
}

void taManagedThread::stopMe() {
  m_stop_req = true;
}

// Copyright 2013-2018, Regents of the University of Colorado,
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

#include "taTaskThread.h"

#include <QMutexLocker>

/*
  Note: the Wait/Mutex synchronization paradigm here was taken
  from Qt documentation and other sources. It is non-obvious,
  non-trivial, and you should definitely not mess with it!!!
  
  Note that many mutex operations have been commented out --
  since the only variable is an int (an enum), and int values
  are always loaded/stored atomically by the CPU, there is no
  need to add the overhead of a mutex lock/unlock just for
  testing for a single value, or setting the value.
  
  States
  
  The thread can be in one of four states:
  Blocked/Active* - thread is waiting to run a task; set task and release()
  Running/Active - task is now running
  Done/Active - task is finished; sync() can (optionally) be used to sync;
    note that syncing is required before setting new task parameters 
  Inactive -- this is the shutdown/delete state only
  * startup state
  
  References
  
  Norton, S.J., & DiPasquale, M.D. (1997). ThreadTime. 
    Upper Saddle River, NJ: Prentice Hall.
  Qt 4.3 Assistant
*/

Qt::HANDLE taTaskThread::m_main_thread_id = QThread::currentThreadId();

void taTaskThread::DeleteTaskThread(taTaskThread* tt) {
  if (tt->isActive()) {
    tt->terminate();
  }
  // note: spin-waiting seemed to be the only stable way to do this 
  // do NOT use Wait() -- it seemed to lock up the app
  while (!tt->isFinished());
  delete tt;
}

bool taTaskThread::inMainThread() {
  return (currentThreadId() == m_main_thread_id);
}

taTaskThread::taTaskThread(bool log_, int affinity_)
: m_affinity(affinity_),
//  m_main_thread_id(currentThreadId()), 
  m_log(log_) 
{
  m_thread_id = 0;
  m_task = NULL;
  m_active = true;
  m_state = TS_BLOCKED;
  // note: not a taBase, so we need to do init links here
  start_latency.InitLinks();
  run_time.InitLinks();
}

taTaskThread::~taTaskThread() {
  run_time.CutLinks();
  start_latency.CutLinks();
}

void taTaskThread::runTask() {
  mutex.lock();
  if (m_log) start_latency.StartTimer(false); // don't reset
  m_state = TS_RUNNING;
  mutex.unlock();
//note: unlock mutex BEFORE run to avoid spurious context switches
  released.wakeAll();
}

// this is the QThread run state
void taTaskThread::run() {
  m_thread_id = currentThreadId();
//   SetAffinity();

  while (m_active) {
    mutex.lock();
    while (m_state != TS_RUNNING)
      released.wait(&mutex);
    mutex.unlock();
    
    if (m_log) start_latency.EndTimer();
    if (m_task) {
      if (m_log) run_time.StartTimer(false); // don't reset
      m_task->run();
     if (m_log) run_time.EndTimer();
    }
    
    mutex.lock();
    m_state = TS_DONE;
    mutex.unlock();
    synced.wakeAll(); // noop if no one waiting
  }
}

void taTaskThread::SetAffinity() {
//TODO: check/set affinity
}

void taTaskThread::setTask(taTask* t) {
  if (m_task.ptr() == t) return;
  m_task = t;
}

void taTaskThread::sync() {
  // note: the most likely case (esp when main thread does tasks too)
  // is that the thread is already done, just check/change without locking
  if (m_state != TS_DONE) {
    mutex.lock();
    while (m_state != TS_DONE)
      synced.wait(&mutex);
    mutex.unlock();
  }
  m_state = TS_BLOCKED;
}

void taTaskThread::terminate() {
  if (!m_active) return;
  
  mutex.lock();
  m_active = false;
  m_task = NULL; // don't call setTask, because that also suspends
  m_state = TS_RUNNING; // need to resume so it will finish!
  released.wakeAll();
  mutex.unlock();
//  QThread::terminate(); //WARNING: including this causes the dude to hang
}



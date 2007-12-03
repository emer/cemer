// Copyright, 1995-2007, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of The Emergent Toolkit
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

#include "ta_engine.h"

#include "ta_project.h"

////////////////////////////
//      taTask  	  //
////////////////////////////

/*void taTask::Initialize() {
}

void taTask::Destroy() {
}

void taTask::Copy_(const taTask& cp) {
} */


////////////////////////////
//      taEngine  	  //
////////////////////////////

void taEngine::Initialize() {
}

void taEngine::Destroy() {
}

void taEngine::Copy_(const taEngine& cp) {
  tasks = cp.tasks;
}

void taEngine::InitLinks() {
  inherited::InitLinks();
  taBase::Own(&tasks, this);
}

void taEngine::CutLinks() {
  tasks.CutLinks();
  inherited::CutLinks();
}

#ifdef TA_USE_THREADS

/*
  Note: the Wait/Mutex synchronization paradigm here was taken
  from Qt documentation. It is non-obvious, non-trivial, and you
  should definitely not mess with it!!!
  
*/

void taTaskThread::DeleteTaskThread(taTaskThread* tt) {
  if (tt->isActive()) {
    tt->terminate();
  }
  // note: spin-waiting seemed to be the only stable way to do this 
  // do NOT use Wait() -- it seemed to lock up the app
  while (!tt->isFinished());
  delete tt;
}

taTaskThread::taTaskThread() {
  m_thread_id = 0;
  m_task = NULL;
  m_active = true;
  m_suspended = true;
  // note: not a taBase, so we need to do init links here
  start_latency.InitLinks();
  run_time.InitLinks();
}

taTaskThread::~taTaskThread() {
  run_time.CutLinks();
  start_latency.CutLinks();
}

void taTaskThread::resume() {
  if (!m_suspended) return;
  mutex.lock();
  m_suspended = false;
  start_latency.StartTimer(false); // no reset
  wc.wakeAll();;
  mutex.unlock();
}

void taTaskThread::run() {
  m_thread_id = currentThreadId();
  while (m_active) {
    mutex.lock();
    while (m_suspended)
      wc.wait(&mutex);
      
    start_latency.EndTimer();
    if (m_task) {
      run_time.StartTimer(false); // no reset
      m_task->run();
      run_time.EndTimer();
    }
    m_suspended = true;
    if (!m_active) break;
    mutex.unlock();
  }
}

void taTaskThread::setTask(taTask* t) {
  if (m_task == t) return;
  suspend();
  m_task = t;
}

void taTaskThread::suspend() {
  if (m_suspended) return;
  mutex.lock();
  m_suspended = true;
  mutex.unlock();
}

void taTaskThread::terminate() {
  if (!m_active) return;
  
  mutex.lock();
  m_active = false;
  m_task = NULL; // don't call setTask, because that also suspends
  m_suspended = false; // need to resume so it will finish!
  wc.wakeAll();;
  mutex.unlock();
//  QThread::terminate(); //WARNING: including this causes the dude to hang
}


#endif // TA_USE_THREADS
 

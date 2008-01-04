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

void taTask::Initialize() {
  task_id = -1; 
  proc_id = 0;
  m_inst = NULL;
}

/*void taTask::Destroy() {
}

void taTask::Copy_(const taTask& cp) {
} */


taBase* taTask::SetOwner(taBase* own) {
  if (own) {
    m_inst = (taEngineInst*)own->GetOwner(&TA_taEngineInst);
  } else {
    m_inst = NULL;
  }
  return inherited::SetOwner(own);
}

////////////////////////////
//  taEngineInst  	  //
////////////////////////////

void taEngineInst::Initialize() {
  use_log = false; // set by Engine
  taBase::Own(m_engine, this);
  taBase::Own(log_table, this);
}

void taEngineInst::Destroy() {
  log_table = NULL;
  m_engine = NULL;
}

void taEngineInst::InitLinks() {
  inherited::InitLinks();
  taBase::Own(&tasks, this);
}

void taEngineInst::CutLinks() {
  tasks.CutLinks();
  inherited::CutLinks();
}

void taEngineInst::AssertLogTable() {
  if (!use_log || (bool)log_table) return;
  taProject* proj = GET_MY_OWNER(taProject);
  if (proj) {
    DataTable* dt = proj->GetNewAnalysisDataTable(m_engine->GetName());
    dt->Reset();
    log_table = dt;
  }
}

void taEngineInst::setTaskCount(int val) {
  if (val == tasks.size) return;
  else tasks.SetSize(val);
}

void taEngineInst::WriteLogRecord() {
  if (!(use_log && (bool)log_table)) return;
  if ((max_rows >= 0) && (log_table->rows >= max_rows)) return; 
  log_table->AddBlankRow();
  WriteLogRecord_impl();
}

////////////////////////////
//      taEngine  	  //
////////////////////////////

void taEngine::Initialize() {
#ifdef DEBUG
  use_log = true;
#else
  use_log = false;
#endif
  max_rows = 2000;
}

void taEngine::Destroy() {
}

taEngineInst* taEngine::MakeEngineInst_impl() const {
  taEngineInst* rval = NewEngineInst_impl();
  rval->m_engine = const_cast<taEngine*>(this);
  rval->use_log = use_log;
  rval->max_rows = max_rows;
  return rval;
}

#ifdef TA_USE_THREADS

#include <QMutexLocker>

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
  m_state = TS_BLOCKED;
  // note: not a taBase, so we need to do init links here
  start_latency.InitLinks();
  run_time.InitLinks();
}

taTaskThread::~taTaskThread() {
  run_time.CutLinks();
  start_latency.CutLinks();
}

void taTaskThread::release() {
  QMutexLocker ml(&mutex); // locked now
/*  if (m_state != TS_BLOCKED) {
    taMisc::Error("taTaskThread::release: expected m_state to be TS_BLOCKED was: ",
    String(m_state));
    return;
  }*/
  start_latency.StartTimer(); // reset
  m_state = TS_RUNNING;
  released.wakeAll();
  //ml unlocks on delete
}

void taTaskThread::run() {
  m_thread_id = currentThreadId();
  while (m_active) {
    mutex.lock();
    while (m_state != TS_RUNNING)
      released.wait(&mutex);
    mutex.unlock();
    
    start_latency.EndTimer();
    if (m_task) {
      run_time.StartTimer(); // reset
      m_task->run();
      run_time.EndTimer();
    }
    
    mutex.lock();
    m_state = TS_DONE;
    synced.wakeAll();
    mutex.unlock();
  }
}

void taTaskThread::setTask(taTask* t) {
  if (m_task.ptr() == t) return;
  m_task = t;
}

void taTaskThread::sync() {
  QMutexLocker ml(&mutex); // locked now
  while (m_state != TS_DONE)
    synced.wait(&mutex);
  m_state = TS_BLOCKED;
  //ml unlocks
}

void taTaskThread::terminate() {
  if (!m_active) return;
  
  mutex.lock();
  m_active = false;
  m_task = NULL; // don't call setTask, because that also suspends
  m_state = TS_RUNNING; // need to resume so it will finish!
  released.wakeAll();;
  mutex.unlock();
//  QThread::terminate(); //WARNING: including this causes the dude to hang
}


#endif // TA_USE_THREADS
 

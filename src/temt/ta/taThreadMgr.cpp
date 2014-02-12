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

#include "taThreadMgr.h"
#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(taThreadMgr);

using namespace std;


taThreadMgr_PList taThreadMgr::all_thread_mgrs;

void taThreadMgr::Initialize() {
  n_threads = taMisc::thread_defaults.n_threads;
  sync_sleep_usec = 1;
  terminate_max_wait = 10000;
  task_type = NULL;
  get_timing = false;
  n_wake_in_sync = 0;
  run_time_pct = 0.0;
  sync_time_pct = 0.0;
  n_to_run = 0;
  n_running = 0;
  n_started = 0;
  n_active = 0;
}

void taThreadMgr::Destroy() {
  CutLinks();
}

void taThreadMgr::InitLinks() {
  inherited::InitLinks();
  taBase::Own(tasks, this);
  taBase::Own(run_time, this);
  taBase::Own(sync_time, this);
  taBase::Own(total_time, this);
  all_thread_mgrs.Add(this);	// we're real -- add to global list
}

void taThreadMgr::CutLinks() {
  all_thread_mgrs.RemoveEl(this); // get off global list
  RemoveThreads();
  tasks.CutLinks();
}

void taThreadMgr::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  n_threads = MAX(n_threads, 1);
  TestWarning(!task_type, "UAE", "task_type is still NULL -- this should have been set in the code -- please file a bug report");
}

void taThreadMgr::InitAll() {
  if((threads.size == n_threads-1) && (tasks.size == n_threads)) return; // fast bail if same
  InitThreads();
  CreateTasks();
  SetTasksToThreads();		// this actually starts threads
}

void taThreadMgr::RemoveAll() {
  RemoveThreads();
  tasks.Reset();
}

void taThreadMgr::InitThreads() {
  if(threads.size == n_threads-1) return;
  int n_to_make = n_threads-1;	// 0 = main guy!

  // have to create and destroy wholesale, because they are all waiting on the same signal
  RemoveThreads();
  for(int i = 0; i < n_to_make; ++i) {
    taManagedThread* tt = new taManagedThread(this);
    threads.Add(tt);
  }
  n_active = 0;			// reset now for sure
}

void taThreadMgr::RemoveThreads() {
  SyncThreads();		// make sure all done running!

  int old_cnt = threads.size;
  for (int i = old_cnt - 1; i >= 0; i--) {
    taManagedThread* tt = threads[i];
    tt->stopMe();		// tell them to stop
  }

  wait_mutex.lock();
  wait.wakeAll();		// now wake them up, where they meet certain death..
  wait_mutex.unlock();

  taManagedThread::usleep(sync_sleep_usec*100); // give some time to wakeup..
  int n_waits = 0;

  wait_mutex.lock();
  while(n_active > 0 && n_waits < terminate_max_wait) { // wait for everyone to exit
    wait_mutex.unlock();
    taManagedThread::usleep(sync_sleep_usec*10);    // this is going to be slower, so wait longer
    n_waits++;
    wait_mutex.lock();
  }
  wait_mutex.unlock();

  // if any remain, terminate them!!
  for (int i = old_cnt - 1; i >= 0; i--) {
    taManagedThread* tt = threads[i];
    if(tt->isActive())
      tt->terminate();		// nuke with vengance..
    threads.RemoveIdx(i);
    delete tt;			// nuke..
  }
  n_active = 0;			// now 0 for sure
}

void taThreadMgr::TerminateAllThreads() {
  int old_cnt = all_thread_mgrs.size;
  for (int i = old_cnt - 1; i >= 0; i--) {
    taThreadMgr* tt = all_thread_mgrs[i];
    tt->RemoveThreads();
  }
}

void taThreadMgr::CreateTasks() {
  if(TestError(!task_type, "CreateTasks", "task_type is NULL -- this is a programmer error -- please file a bug report"))
    return;
  if(tasks.size == n_threads && tasks.el_typ == task_type) return;
  tasks.Reset();
  tasks.el_typ = task_type;
  tasks.New(n_threads, task_type);
}

void taThreadMgr::SetTasksToThreads() {
  if(TestError(!task_type, "SetTasksToThreads", "task_type is NULL -- this is a programmer error -- please file a bug report"))
    return;
  for(int i=0;i<threads.size;i++) {
    taManagedThread* tt = threads[i];
    tt->setTask(tasks[i+1]);
    tt->start(); // starts paused -- now actually start the thread!
  }

  // wait here to make sure the damn threads actually startup!
  taManagedThread::usleep(sync_sleep_usec*100); // give some time to wakeup..
  wait_mutex.lock();
  while(n_active < threads.size) { // wait for everyone to start and get into first wait
    wait_mutex.unlock();
    taManagedThread::usleep(sync_sleep_usec*10);    // this is going to be slower, so wait longer
    wait_mutex.lock();
  }
  wait_mutex.unlock();
  // now we know for sure everyone is groovy
}

void taThreadMgr::RunThreads() {
  if(get_timing)    total_time.StartTimer(false); // don't reset

  // do all this under the mutex, so we know that everyone is fully back in wait
  wait_mutex.lock();
  n_to_run = threads.size;
  n_started = 0;
  n_running = 0;
  wait.wakeAll();
  wait_mutex.unlock();

  if(get_timing)    run_time.StartTimer(false); // don't reset
}

void taThreadMgr::SyncThreads() {
  if(get_timing)    run_time.EndTimer();

  if(get_timing) {
    sync_time.StartTimer(false); // don't reset
    if(n_started < n_to_run)
      n_wake_in_sync += n_to_run - n_started;
  }

  // note: one could wrap the following code all in wait_mutex.lock and unlock, but
  // because the threads are mutex on all the increments, esp the n_running one, 
  // it is not necessary!  the mutex will be hit next time a run threads guy happens
  // and that will definitely ensure that everyone got back home..

  while(n_started < n_to_run) { // wait for other guys to start
    taManagedThread::usleep(sync_sleep_usec);
  }
  while(n_running > 0) {	// then wait for everyone to finish
    taManagedThread::usleep(sync_sleep_usec);
  }

  if(get_timing) {
    sync_time.EndTimer();
    total_time.EndTimer();
  }
}

void taThreadMgr::Run() {
  InitAll();			// fast if no diff
  if(tasks.size == 0) return;
  RunThreads();
  tasks[0]->run();		// task 0 run in main thread
  SyncThreads();
}


void taThreadMgr::StartTimers() {
  get_timing = true;
  run_time.ResetUsed();
  sync_time.ResetUsed();
  total_time.ResetUsed();
  n_wake_in_sync = 0;
}

void taThreadMgr::EndTimers(bool print_report) {
  get_timing = false;
  if(total_time.s_used > 0.0) {
    run_time_pct = run_time.s_used / total_time.s_used;
    sync_time_pct = sync_time.s_used / total_time.s_used;
    wake_in_sync_pct = (double)n_wake_in_sync / (double)(run_time.n_used * threads.size);
  }
  else {
    run_time_pct = 0.0;
    sync_time_pct = 0.0;
    wake_in_sync_pct = 0.0;
  }

  if(print_report) {
    cout << GetTypeDef()->name << " thread report for n_threads: " << n_threads << endl;
    cout << "total time:   " << total_time.s_used << endl;  
    cout << "run time:     " << run_time.s_used << " \t%: " << run_time_pct << endl;  
    cout << "sync time:    " << sync_time.s_used << " \t%: " << sync_time_pct << endl;  
    cout << "wake in sync: " << n_wake_in_sync << " \t%: " << wake_in_sync_pct << endl;  
  }
}

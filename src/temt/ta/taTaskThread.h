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

#ifndef taTaskThread_h
#define taTaskThread_h 1

// parent includes:
#ifndef __MAKETA__
#include <QThread>
#endif

// member includes:
#include <TimeUsedHR>
#include <taTask>
#ifndef __MAKETA__
#include <QMutex>
#include <QWaitCondition>
#endif

// declare all other types mentioned but not required to include:


/* TaskThread
  A task thread is a worker thread that remains persistent over a long
  period of time (i.e., is constantly re-used), and designed for
  running a task (without further interruption) until completion, followed
  by synchronization.  it is up to the taTask guy to do any and all
  coordination with other threads, etc
  
  The usage is as follows:
  
  - create/start -- this puts the thread in the Blocked condition
  - assign a task
  - runTask -- release thread to run given task
  - thread is now running
  - 'sync' -- this waits for the thread to finish the task -- the main thread
      will block until the thread is finished
*/

TypeDef_Of(taTaskThread);

class TA_API taTaskThread: public QThread {
  // ##NO_TOKENS ##NO_UPDATE_AFTER ##CAT_Thread a stand-alone persisent worker thread that can be started and stopped without having to create and destroy the thread object itself.  see taManagedThread for a similar thread object designed to work in concert with other threads more efficiently
INHERITED(QThread)
public:
  enum ThreadState {
    TS_BLOCKED, // waiting to be released (to run the task)
    TS_RUNNING, // running the task
    TS_DONE,    // finished running the task, waiting to sync
  };
  
  static bool		inMainThread(); // return true for the caller if it is the main thread of the program
  static void		DeleteTaskThread(taTaskThread* tt); 
   // use this to delete, do not delete directly!

  TimeUsedHR		start_latency; 	// amount of time waiting to start
  TimeUsedHR		run_time; 	// amount of time actually running jobs
  
  inline bool		isActive() const {return m_active;}
  // currently alive and ready to be used -- could be in any of ThreadState states

  void			setLog(bool log_on) {m_log = log_on;}
  // set whether to log data or not
  inline bool		log() const {return m_log;} 
  // log timing data (start latency, run_time)

  taTask*		task() const {return m_task;}
  // current task we're associated with
  void			setTask(taTask* t);
  // assign a task for this thread

  void			runTask();	// run the task in this thread
  void 			sync(); 	// sync up with the thread after finished running task
  void			terminate(); 	// note: lexical override only
  
  taTaskThread(bool log = false, int affinity = -1); // NEVER make static version, only via new, and always delete with Delete method

protected:
  ~taTaskThread(); // do not elevate to public, always delete through static guy
  
  QMutex		mutex;// #IGNORE
  QWaitCondition 	released;// #IGNORE
  QWaitCondition 	synced;// #IGNORE
  taTaskRef		m_task;
  const int		m_affinity; // -1-default; note: this may be ignored
#ifndef __MAKETA__
  static Qt::HANDLE	m_main_thread_id; // set on create (may be needed for affinity)
  Qt::HANDLE		m_thread_id; // for the thread, set in run
#endif
  volatile ThreadState	m_state;
  volatile bool		m_active;
  bool			m_log;
  
  override void 	run();
  void			SetAffinity(); // called from run() in thread
};

#endif // taTaskThread_h

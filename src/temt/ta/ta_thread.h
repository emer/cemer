// Copyright, 1995-2007, Regents of the University of Colorado,
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


// ta_thread.h -- multi-threading interface

#ifndef TA_THREAD_H
#define TA_THREAD_H

#include "ta_group.h"
#include "ta_time.h"

#ifndef __MAKETA__
#include <QtCore/QList>
#include <QtCore/QThread>
#include <QtCore/QMutex>
#include <QtCore/QWaitCondition>
#endif

// external refs
class QThread; 
//


///////////////////////////////////////////////////////////////
// 	Tasks and Threads

class TA_API taTask: public taOBase {
  // ##TOKENS ##INSTANCE ##CAT_Thread a single processing element: just a generic wrapper for whatever you want a thread to do: just has a run function and an id
INHERITED(taOBase)
public:
  int			task_id; // #READ_ONLY #SHOW #NO_COPY unique id per list of tasks -- typically one per thread

  virtual void		run() {} // must be overridden, to dispatch actual proc
  
  override int		GetIndex() const {return task_id;}
  override void		SetIndex(int val) {task_id = val;}
  TA_BASEFUNS(taTask);
private:
  void	Copy_(const taTask& cp) {}
  void	Initialize();
  void	Destroy() {}
};

TA_SMART_PTRS(taTask);

class TA_API taTask_List : public taList<taTask> {
  // ##CAT_Thread a list of tasks
  INHERITED(taList<taTask>)
public:
  override String 	GetTypeDecoKey() const { return "Task"; }
  TA_BASEFUNS_NOCOPY(taTask_List);
private:
  void	Initialize() {SetBaseType(&TA_taTask);}
  void 	Destroy()		{Reset(); }; //
};

//

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
class TA_API taTaskThread: public QThread {
  // ##NO_TOKENS ##NO_UPDATE_AFTER ##CAT_Thread our thread object
INHERITED(QThread)
public:
  enum ThreadState {
    TS_BLOCKED, // waiting to be released (to run the task)
    TS_RUNNING, // running the task
    TS_DONE,    // finished running the task, waiting to sync
  };
  
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
  const Qt::HANDLE	m_main_thread_id; // set on create (may be needed for affinity)
  Qt::HANDLE		m_thread_id; // for the thread, set in run
#endif
  volatile ThreadState	m_state;
  volatile bool		m_active;
  bool			m_log;
  
  override void 	run();
  void			SetAffinity(); // called from run() in thread
};

class TA_API taTaskThread_PList : public taPtrList<taTaskThread> {
  // ##NO_TOKENS ##NO_UPDATE_AFTER ##CAT_Thread list of task threads
  //INHERITED(taPtrList<taTaskThread>)
  public:
};


class TA_API taThreadMgr : public taOBase {
  // ##CAT_Thread thread manager base class
INHERITED(taOBase)
public:
  int			n_threads; // #MIN_1 #NO_SAVE NOTE: not saved -- initialized from user prefs.  desired number of threads to use -- typically the number of physical processors (cores) available, and is initialized to that.
  bool			log_timing; // #EXPERT whether to log the timing information about the threads

  taTask_List		tasks;	 // #NO_SAVE #READ_ONLY the tasks for the threads to perform -- we manage these and allocate them to threads
  taTaskThread_PList	threads; // #NO_SAVE #READ_ONLY the threads -- memory managed by InitThreads and RemoveThreads

  static taTaskThread_PList all_threads; // #NO_SAVE #READ_ONLY all threads -- maintains a global list in addition to the local lists per mgr

  void		InitThreads();	// initialize (create) n_threads threads
  void		RemoveThreads();// remove all the threads

  void		CreateTasks(TypeDef* task_type); // create n_threads tasks of given type
  void		SetTasksToThreads(); // set the tasks to the threads

  static void	TerminateAllThreads();
  // static function for terminating all the threads, e.g., in the err signal handler or quit routine
  
  void 	InitLinks();
  void	CutLinks();
  TA_BASEFUNS_NOCOPY(taThreadMgr);
private:
  void	Initialize();
  void	Destroy();
};


///////////////////////////////////////////////////////////////
// 	Helpful code for tasks that call methods on objects

// this was modified from http://www.partow.net/programming/templatecallback/index.html
// by Arash Partow, available for free use under simple common license

#ifndef __MAKETA__

template <class Class, typename ReturnType, typename Parameter>
class taTaskMethCall1 {
  // useful template for creating tasks that call the same method on a list of same objs
public:
  typedef ReturnType (Class::*Method)(Parameter);

  taTaskMethCall1(Method _method) {
    method        = _method;
  }

  ReturnType call(Class* base, Parameter parameter) {
    return (base->*method)(parameter);
  }
private:
  Method  method;
};

template <class Class, typename ReturnType, typename Parameter1, typename Parameter2>
class taTaskMethCall2 {
  // useful template for creating tasks that call the same method on a list of same objs
public:
  typedef ReturnType (Class::*Method)(Parameter1, Parameter2);

  taTaskMethCall2(Method _method) {
    method        = _method;
  }

  ReturnType call(Class* base, Parameter1 parameter1, Parameter2 parameter2) {
    return (base->*method)(parameter1, parameter2);
  }
private:
  Method  method;
};

// note: can define as many of these as you need..

#endif // __MAKETA__

#endif


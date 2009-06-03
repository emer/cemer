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
class taThreadMgr;

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

class TA_API taTaskThread_PList : public taPtrList<taTaskThread> {
  // ##NO_TOKENS ##NO_UPDATE_AFTER ##CAT_Thread list of task threads
  //INHERITED(taPtrList<taTaskThread>)
  public:
};

////////////////////////////////////////////////////////////////////////////
//	A Managed Thread -- everything is controlled by the taThreadMgr 

class TA_API taManagedThread: public QThread {
  // ##NO_TOKENS ##NO_UPDATE_AFTER ##CAT_Thread a fully managed thread -- controlled by the taThreadMgr and not capable of operating without it, but much more efficient for collective operations than the taTaskThread object
INHERITED(QThread)
public:
  taThreadMgr*		mgr;
  // our manager -- controls everything
  
  inline bool		isActive() const { return m_active; }
  // currently alive -- means it is somewhere in the run() function
  void			stopMe();
  // stop this thread -- just sets m_stop_req = true -- manager needs to start tasks to get them to actually see the new signal and then stop -- all tasks must be stopped at the same time!
  inline bool		isRunning() const { return m_running; }
  // is actually running a task at this time -- otherwise it is waiting on the mgr->wait wait condition

  taTask*		task() const {return m_task;}
  // current task we're associated with
  void			setTask(taTask* t);
  // assign a task for this thread

  taManagedThread(taThreadMgr* mg);
  // should only be called by the mgr..
  ~taManagedThread();
protected:
  taTaskRef		m_task;
  bool			m_active;  // set to true when in run() state
  bool			m_running; // set to true when running
  bool			m_stop_req; // set to true to signal that this thread should exit
  
  override void 	run();
};

class TA_API taManagedThread_PList : public taPtrList<taManagedThread> {
  // ##NO_TOKENS ##NO_UPDATE_AFTER ##CAT_Thread list of task threads
  //INHERITED(taPtrList<taManagedThread>)
  public:
};

class TA_API taThreadMgr_PList : public taPtrList<taThreadMgr> {
  // ##NO_TOKENS ##NO_UPDATE_AFTER ##CAT_Thread list of thread managers
  //INHERITED(taPtrList<taManagedThread>)
  public:
};


class TA_API taThreadMgr : public taOBase {
  // ##CAT_Thread thread manager base class -- controls a set of taManagedThread objects that are all deployed and synchronized together to perform a specific task that is determined by a taTask object type -- there are n_threads tasks and n_threads-1 sub-threads, with the main thread running task 0
INHERITED(taOBase)
public:
  static taThreadMgr_PList all_thread_mgrs; // #NO_SAVE #READ_ONLY all thread managers -- used for global termination as they must all be terminated through the manager -- managers automatically add and remove themselves from this list

  int			n_threads; 	// #MIN_1 #NO_SAVE NOTE: not saved -- initialized from user prefs.  desired number of threads to use -- typically the number of physical processors (cores) available -- see preferences/options thread_defaults field for details.
  int			sync_sleep_usec; // #EXPERT #NO_SAVE microseconds to sleep while waiting for threads to synchronize -- not typically adjusted by the user, and not saved, but availble for testing purposes
  TypeDef*		task_type;	 // #NO_SAVE #READ_ONLY the type of task object to create -- this should be set *prior* to calling InitAll() and should NOT change during the lifetime of the manager, unless an explicit RemoveAll() call is made first

  taTask_List		tasks;	 // #NO_SAVE #READ_ONLY the tasks for the threads to perform -- we manage these and allocate them to threads -- all are of type task_type
  taManagedThread_PList	threads; // #NO_SAVE #READ_ONLY the threads -- we manage them completely

  ///////////////////////////////////////////
  // Stats on the overhead of the system

  bool			get_timing;	// #NO_SAVE #READ_ONLY collect timing information as the system runs -- this is set by StartTimers and turned off by EndTimers
  TimeUsedHR		run_time; 	// #EXPERT total time (in secs and fractions thereof) from end of RunThreads() call (after telling threads to wake up) to start of SyncThreads() call
  TimeUsedHR		sync_time;	// #EXPERT total time (in secs and fractions thereof) in SyncThreads() waiting to sync up the threads
  TimeUsedHR		total_time;	// #EXPERT total time (in secs and fractions thereof) from start of RunThreads() to end of SyncThreads()
  int			n_wake_in_sync; // #EXPERT number of threads that had to be woken in the sync process -- i.e., they hadn't even started running by the time the main thread finished!

  double		run_time_pct; 	// #EXPERT percent of total time spent running -- computed in EndTimers()
  double		sync_time_pct;	// #EXPERT percent of total time spent syncing -- computed in EndTimers()
  double		wake_in_sync_pct; // #EXPERT percent of total threads run that had to be woken during the sync process

  //////////////////////////////////////////////////////
  //		These are used by the managed threads

  int			n_to_run;
  // #IGNORE number of threads that should start to run -- this is set to threads.size at start of run, and checked in sync threads
  int			n_running;
  // #IGNORE number of threads that are currently running -- atomically incremented and decremented by the threads as they run and finish their task
  int			n_started;
  // #IGNORE number of threads that actually started the task -- this is reset to 0 at start of run, and atomically incremented by the threads when they start running -- ensures that everyone runs..
  QMutex		wait_mutex;
  // #IGNORE mutex for guarding the wait guy
  QWaitCondition 	wait;
  // #IGNORE overall wait condition -- all threads are waiting for the wakeAll from this condition, unless they are actually running

  //////////////////////////////////////////////////////
  //		Main interface for users

  virtual void	InitAll();	// initialize the threads and tasks -- this checks for current sizes and is very fast if nothing has changed, so is safe to insert at start of computation just to be sure -- can be overloaded with other initialization functionality too though..
  virtual void	RemoveAll();	// remove all the threads and tasks -- generally only called if task_type is changed, such that a subsequent InitAll will create all new guys

  void 	Run();		// actually run the overall set of tasks -- this is a sample basic function that calls InitAll(), RunThreads() then runs task[0] on the main thread, then calls SyncThreads() -- subclasses can provide customized functions that initialize task parameters etc

  virtual void	RunThreads();	// start the threads running their current task: NOTE this is ONLY called on the actual threads, and does not run the main thread
  virtual void	SyncThreads();	// synchronize the threads at the end of running to ensure everyone has finished their task and is ready to move on

  virtual void	StartTimers();
  // Start accumulating timing information on all threads -- must be called *after* everything is initialized and ready to run
  virtual void	EndTimers(bool print_report = true);
  // Finish accumulating timing information on all threads, compute summary information, and optionally report that to cout

  static void	TerminateAllThreads();
  // static function for terminating all the threads, e.g., in the err signal handler or quit routine
  
  void 	InitLinks();
  void	CutLinks();
  TA_BASEFUNS_NOCOPY(taThreadMgr);
protected:
  void	UpdateAfterEdit_impl();

  // these are the basic housekeeping functions that are called by InitAll and RemoveAll

  void	InitThreads();
  // initialize (create) n_threads-1 threads -- checks for size first and returns quickly if correct
  void	RemoveThreads();
  // remove all the threads -- tells them to stop, then waits for them to actually stop, then deletes them

  void	CreateTasks();
  // create n_threads tasks of given type
  void	SetTasksToThreads(); // set the tasks to the threads

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


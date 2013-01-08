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

#ifndef taThreadMgr_h
#define taThreadMgr_h 1

// parent includes:
#include <taOBase>

// member includes:
#include <taThreadMgr_PList>
#include <taTask_List>
#include <taManagedThread_PList>
#include <TimeUsedHR>

// declare all other types mentioned but not required to include:
class TypeDef; // 


class TA_API taThreadMgr : public taOBase {
  // ##CAT_Thread thread manager base class -- controls a set of taManagedThread objects that are all deployed and synchronized together to perform a specific task that is determined by a taTask object type -- there are n_threads tasks and n_threads-1 sub-threads, with the main thread running task 0
INHERITED(taOBase)
public:
  static taThreadMgr_PList all_thread_mgrs; // #NO_SAVE #READ_ONLY all thread managers -- used for global termination as they must all be terminated through the manager -- managers automatically add and remove themselves from this list

  int			n_threads; 	// #MIN_1 #NO_SAVE NOTE: not saved -- initialized from user prefs.  desired number of threads to use -- typically the number of physical processors (cores) available -- see preferences/options thread_defaults field for details.
  int			sync_sleep_usec; // #EXPERT #NO_SAVE microseconds to sleep while waiting for threads to synchronize -- not typically adjusted by the user, and not saved, but availble for testing purposes
  int			terminate_max_wait; // #EXPERT #NO_SAVE number of iterations to wait for threads to self-terminate before taking them out more forcefully

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
  int			n_active;
  // #IGNORE number of threads that are activated and ready to start running -- used for syncing on task startup
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

#endif // taThreadMgr_h

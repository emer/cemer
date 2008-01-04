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

// ta_engine.h -- execution engines, and the threaded engine

#ifndef TA_ENGINE_H
#define TA_ENGINE_H

#include "ta_group.h"
#include "ta_time.h"

#include "ta_def.h"
#include "ta_TA_type.h"


// external refs

// forwards
class taEngine;
TA_SMART_PTRS(taEngine);
class taTask;
TA_SMART_PTRS(taTask);
class taEngine_Group;
class taEngineInst;


class TA_API taTask: public taOBase {
  // ##TOKENS ##INSTANCE ##CAT_Task a single processing instance for an engine
INHERITED(taOBase)
public:
  int			task_id; // unique id per logical set, ex. one per thread
  int			proc_id; // current proc being run
  
  inline taEngineInst*	inst() const {return m_inst;} // typically lex override
  
  virtual void		run() {} // must be overridden, to dispatch actual proc
  
  override int		GetIndex() const {return task_id;}
  override void		SetIndex(int val) {task_id = val;}
  override taBase*	SetOwner(taBase* own);
  TA_BASEFUNS(taTask);
protected:
  taEngineInst*		m_inst; // set automatically in SetOwner
private:
  void	Copy_(const taTask& cp) {}
  void	Initialize();
  void	Destroy() {}
};

class TA_API taTask_List : public taList<taTask> {
  // ##CAT_Task a list of tasks
  INHERITED(taList<taTask>)
public:
  
  override String 	GetTypeDecoKey() const { return "Task"; }
  TA_BASEFUNS_NOCOPY(taTask_List);
private:
  void	Initialize() {SetBaseType(&TA_taTask);}
  void 	Destroy()		{Reset(); }; //
}; //


class TA_API taEngineInst: public taOBase { // ##NO_INSTANCE #VIRT_BASE ##NO_TOKENS extensible runtime-only structure that contains client-global data and tasks organized for efficient access by runtime Engines
INHERITED(taOBase)
public:
  taEngineRef		m_engine;
  taTask_List		tasks; // #NO_SAVE -- set tasks with setTaskCount
  
  inline int		taskCount() const {return tasks.size;} // #NO_SHOW
  virtual void		setTaskCount(int val);
  taTask*		task(int i) {return tasks.FastEl(i);} // #NO_SHOW
  
  void	InitLinks();
  void	CutLinks();
  TA_BASEFUNS_NOCOPY(taEngineInst);
private:
  void	Initialize();
  void	Destroy();
};

class TA_API taEngine: public taNBase {
  // ##TOKENS ##INSTANCE #VIRT_BASE ##DEF_NAME_STYLE_2 an object that manages XxxInst instances of a specific computing engine
INHERITED(taNBase)
public:
  
  taEngineInst*		MakeEngineInst() const {return MakeEngineInst_impl();}
  
  TA_BASEFUNS_NOCOPY(taEngine);
protected:
  virtual taEngineInst*	MakeEngineInst_impl() const;
  virtual taEngineInst*	NewEngineInst_impl() const {return NULL;} // override to make exact type
private:
  void	Initialize();
  void	Destroy();
};


class TA_API taEngine_Group : public taGroup<taEngine> {
  // #CAT_Task a collection of engines -- there is usually a separate group for each family of engines
INHERITED(taGroup<taEngine>)
public:
  override String 	GetTypeDecoKey() const { return "Engine"; }

  TA_BASEFUNS(taEngine_Group);

private:
  void	Copy_(const taEngine_Group& cp) {}
  void	Initialize() {}
  void 	Destroy() {Reset(); };
};


#ifdef TA_USE_THREADS

#ifndef __MAKETA__

#include <QtCore/QList>
#include <QtCore/QThread>
#include <QtCore/QMutex>
#include <QtCore/QWaitCondition>

#define AtomicFetchAdd(p_operand, incr) \
   q_atomic_fetch_and_add_int(p_operand, incr)

//

/* TaskThread
  A task thread is a worker thread that is created once, and designed for
  running a task in a "one-shot" manner, followed by syncronization.
  
  The usage is as follows:
  
  - create/start -- this puts the thread in the Blocked condition
  - assign a task
  - 'resume' -- this releases the thread to run the task
  - thread is now running
  - 'sync' -- this waits for the thread to finish the task -- the main thread
      will block until the thread is finished

*/
class TA_API taTaskThread: public QThread {
INHERITED(QThread)
public:
  enum ThreadState {
    TS_BLOCKED, // waiting to be released (to run the task)
    TS_RUNNING, // running the task
    TS_DONE,    // finished running the task, waiting to sync
  };
  
  static void		DeleteTaskThread(taTaskThread* tt); 
   // use this to delete, do not delete directly!

  TimeUsed		start_latency; // amount of time waiting to start
  TimeUsed		run_time; // amount of time actually running jobs
  
  inline bool		isActive() const {return m_active;}
//  inline bool		isSuspended() const {return m_suspended;}
  
  taTask*		task() const {return m_task;}
  void			setTask(taTask* t);
  void 			sync(); // sync up with the thread after finished running task
  void			release(); // release the task ready to run
  void			terminate(); //note: lexical override only
  
  taTaskThread(); // NEVER make static version, only via new, and always delete with Delete method

protected:
  ~taTaskThread(); // do not elevate to public, always delete through static guy
  
  QMutex		mutex;// #IGNORE
  QWaitCondition 	released;// #IGNORE
  QWaitCondition 	synced;// #IGNORE
  Qt::HANDLE		m_thread_id; // for the thread, set in run
  
  taTaskRef		m_task;
  ThreadState		m_state;
  bool			m_active;
  
  override void 	run();
};

typedef QList<taTaskThread*> taTaskThread_List;

#endif  // QTaskThread


#endif // TA_USE_THREADS
 
#endif

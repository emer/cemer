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

#ifndef __MAKETA__
# include <QtCore/QThread>
# include <QtCore/QMutex>
# include <QtCore/QWaitCondition>
#endif

// external refs

// forwards
class taEngine;
TA_SMART_PTRS(taEngine);
class taTask;
TA_SMART_PTRS(taTask);
class taEngine_Group;


class TA_API taTask: public taNBase {
  // ##TOKENS ##INSTANCE ##CAT_Task a single processing instance for an engine
INHERITED(taNBase)
public:
  int			task_id; // unique id per logical set, ex. one per thread
  int			proc_id; // current proc being run
  
  virtual void		run() {} // must be overridden, to dispatch actual proc
  
  TA_BASEFUNS(taTask);

private:
  void	Copy_(const taTask& cp) {}
  void	Initialize() {task_id = -1; proc_id = 0;}
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


class TA_API taEngine: public taNBase {
  // ##TOKENS ##INSTANCE ##CAT_Task a singleton object that will handle the tasks of its class
INHERITED(taNBase)
public:
  taTask_List		tasks; // #NO_SAVE
  
  inline int		taskCount() const {return tasks.size;} // #NO_SHOW
  taTask*		task(int i) {return tasks.FastEl(i);} // #NO_SHOW
  
  void	InitLinks();
  void	CutLinks();
  TA_BASEFUNS(taEngine);

private:
  void	Copy_(const taEngine& cp);
  void	Initialize();
  void	Destroy();
};

template<class T>
class TA_API taEngineT: public taEngine {
  // template to create sets of Engine/Task pairs
INHERITED(taEngine)
public:

  T*			task(int i) {return static_cast<T*>(tasks.FastEl(i));} // #NO_SHOW
  
  TA_TMPLT_BASEFUNS(taEngineT,T);

private:
  TMPLT_NOCOPY(taEngineT,T)
  void	Initialize() {tasks.SetBaseType(T::StatTypeDef(0));} 
  void	Destroy() {}
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
class TA_API taTaskThread: public QThread {
INHERITED(QThread)
public:
  static void		DeleteTaskThread(taTaskThread* tt); 
   // use this to delete, do not delete directly!

  TimeUsed		start_latency; // amount of time waiting to start
  TimeUsed		run_time; // amount of time actually running jobs
  
  inline bool		isActive() const {return m_active;}
  inline bool		isSuspended() const {return m_suspended;}
  
  taTask*		task() const {return m_task;}
  void			setTask(taTask* t);
  void 			suspend();
  void			resume();
  void			terminate(); //note: lexical override only
  
  taTaskThread(); // NEVER make static version, only via new, and always delete with Delete method

protected:
  ~taTaskThread(); // do not elevate to public, always delete through static guy
  
  QMutex		mutex;// #IGNORE
  QWaitCondition 	wc;// #IGNORE
  Qt::HANDLE		m_thread_id; // for the thread, set in run
  
  taTaskRef		m_task;
  bool			m_suspended;
  bool			m_active;
  
  override void 	run();
};
#endif  // QTaskThread


#endif // TA_USE_THREADS
 
#endif

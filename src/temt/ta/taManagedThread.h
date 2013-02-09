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

#ifndef taManagedThread_h
#define taManagedThread_h 1

// parent includes:
#ifndef __MAKETA__
#include <QThread>
#endif

// member includes:
#include <taTask>

// declare all other types mentioned but not required to include:
class taThreadMgr; // 


taTypeDef_Of(taManagedThread);

class TA_API taManagedThread: public QThread {
  // ##NO_TOKENS ##NO_UPDATE_AFTER ##CAT_Thread a fully managed thread -- controlled by the taThreadMgr and not capable of operating without it, but much more efficient for collective operations than the taTaskThread object
INHERITED(QThread)
public:
#ifndef __MAKETA__
  USING(inherited::usleep);
#endif
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

#endif // taManagedThread_h

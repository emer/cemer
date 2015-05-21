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

#ifndef taTask_h
#define taTask_h 1

// parent includes:
#include <taOBase>

// smartptr, ref includes
#include <taSmartRefT>
#include <taSmartPtrT>

// member includes:
#include <TimeUsedHR>

// declare all other types mentioned but not required to include:
class taThreadMgr;


taTypeDef_Of(taTask);

class TA_API taTask: public taOBase {
  // ##TOKENS ##INSTANCE ##CAT_Thread a single processing element: just a generic wrapper for whatever you want a thread to do: just has a run function and an id
INHERITED(taOBase)
public:
  int			task_id; // #READ_ONLY #SHOW #NO_COPY unique id per list of tasks -- typically one per thread
  TimeUsedHR            wait_time; // amount of time spent in spin-lock wait

  virtual void		run() {} // must be overridden, to dispatch actual proc
  
  void SyncSpin0(int usec_wait = 0);
  // synchronize all the threads using atomic int counters and spin waiting (active looping) -- if usec_wait > 0, then sleep for given number of micro seconds during each wait loop
  void SyncSpin1(int usec_wait = 0);
  // synchronize all the threads using atomic int counters and spin waiting (active looping) -- if usec_wait > 0, then sleep for given number of micro seconds during each wait loop
  void SyncSpin2(int usec_wait = 0);
  // synchronize all the threads using atomic int counters and spin waiting (active looping) -- if usec_wait > 0, then sleep for given number of micro seconds during each wait loop

  taThreadMgr*  mgr();

  int		GetIndex() const override {return task_id;}
  void		SetIndex(int val) override {task_id = val;}
  TA_BASEFUNS(taTask);
private:
  void	Copy_(const taTask& cp) {}
  void	Initialize();
  void	Destroy() {}
};

TA_SMART_PTRS(TA_API, taTask);

#endif // taTask_h

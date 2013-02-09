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

// declare all other types mentioned but not required to include:


taTypeDef_Of(taTask);

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

#endif // taTask_h

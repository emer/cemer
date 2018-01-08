// Co2018ght 2013-2017, Regents of the University of Colorado,
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

#include "UndoDiffThreadMgr.h"
#include <UndoDiffTask>

TA_BASEFUNS_CTORS_DEFN(UndoDiffThreadMgr);

void UndoDiffThreadMgr::Initialize() {
  n_threads = 2;                // don't use 0, just 1..
  task_type = &TA_UndoDiffTask;
}

void UndoDiffThreadMgr::Destroy() {
}

void UndoDiffThreadMgr::Run() {
  n_threads = 2;                // don't use 0, just 1..
  InitAll();
  RunThreads();                 // just run the thread, not main guy
}

TimeUsedHR* UndoDiffThreadMgr::UndoTimer() {
  if(tasks.size < 2) return NULL;
  return &(((UndoDiffTask*)tasks[1])->diff_time);
}

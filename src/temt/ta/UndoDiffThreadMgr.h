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

#ifndef UndoDiffThreadMgr_h
#define UndoDiffThreadMgr_h 1

// parent includes:
#include <taThreadMgr>

// member includes:

// declare all other types mentioned but not required to include:
class taUndoMgr; // 


TypeDef_Of(UndoDiffThreadMgr);

class TA_API UndoDiffThreadMgr : public taThreadMgr {
  // #INLINE thread manager for UndoDiff tasks -- manages threads and tasks, and coordinates threads running the tasks
INHERITED(taThreadMgr)
public:
  taUndoMgr*    undo_mgr()      { return (taUndoMgr*)owner; }

  override void Run();

  TA_BASEFUNS_NOCOPY(UndoDiffThreadMgr);
private:
  void  Initialize();
  void  Destroy();
};

#endif // UndoDiffThreadMgr_h

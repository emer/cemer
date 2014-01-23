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

#ifndef UndoDiffTask_h
#define UndoDiffTask_h 1

// parent includes:
#include <taTask>

// member includes:

// declare all other types mentioned but not required to include:
class UndoDiffThreadMgr; // 


taTypeDef_Of(UndoDiffTask);

class TA_API UndoDiffTask : public taTask {
  // threading task for computing the diff on undo save data -- fork this off to save time
INHERITED(taTask)
public:
  void run() CPP11_OVERRIDE;
  // runs specified chunk of computation (encode diff)

  UndoDiffThreadMgr* mgr() { return (UndoDiffThreadMgr*)owner->GetOwner(); }

  TA_BASEFUNS_NOCOPY(UndoDiffTask);
private:
  void  Initialize();
  void  Destroy();
};

#endif // UndoDiffTask_h

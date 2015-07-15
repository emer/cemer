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

#ifndef PluginMakeTask_h
#define PluginMakeTask_h 1

// parent includes:
#include <taTask>

// member includes:

// declare all other types mentioned but not required to include:
class PluginMakeThreadMgr; // 


taTypeDef_Of(PluginMakeTask);

class TA_API PluginMakeTask : public taTask {
  // threading task for compiling plugin
INHERITED(taTask)
public:
  void run() override;
  // runs specified chunk of computation (encode diff)

  PluginMakeThreadMgr* mgr() { return (PluginMakeThreadMgr*)owner->GetOwner(); }

  TA_BASEFUNS_NOCOPY(PluginMakeTask);
private:
  void	Initialize();
  void	Destroy();
};

#endif // PluginMakeTask_h

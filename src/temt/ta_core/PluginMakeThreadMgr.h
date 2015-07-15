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

#ifndef PluginMakeThreadMgr_h
#define PluginMakeThreadMgr_h 1

// parent includes:
#include <taThreadMgr>

// member includes:

// declare all other types mentioned but not required to include:


taTypeDef_Of(PluginMakeThreadMgr);

class TA_API PluginMakeThreadMgr : public taThreadMgr {
  // #INLINE thread manager for PluginMake tasks -- manages threads and tasks, and coordinates threads running the tasks
INHERITED(taThreadMgr)
public:
  // following are args for the MakePlugin function that is called
  bool		make_pending;
  String	plugin_path;
  String	plugin_name;
  bool		system_plugin;
  bool		full_rebuild;

  void		MakePlugin(const String& plugin_path, const String& plugin_name,
			   bool system_plugin = false, bool full_rebuild=true);
  // runs the task for this
  
  TA_BASEFUNS_NOCOPY(PluginMakeThreadMgr);
private:
  void	Initialize();
  void	Destroy();
};

#endif // PluginMakeThreadMgr_h

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

#include "PluginMakeThreadMgr.h"

void PluginMakeThreadMgr::Initialize() {
  n_threads = 2;                // don't use 0, just 1..
  task_type = &TA_PluginMakeTask;
}

void PluginMakeThreadMgr::Destroy() {
}

void PluginMakeThreadMgr::MakePlugin(const String& pl_path, const String& pl_name,
                                     bool sys_plugin, bool full_reb) {
  n_threads = 2;                // don't use 0, just 1..
  InitAll();
  make_pending = true;
  plugin_path = pl_path;
  plugin_name = pl_name;
  system_plugin = sys_plugin;
  full_rebuild = full_reb;
  RunThreads();                 // just run the thread, not main guy
}


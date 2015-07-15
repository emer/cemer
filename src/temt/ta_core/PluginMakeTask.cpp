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

#include "PluginMakeTask.h"
#include <PluginMakeThreadMgr>
#include <taPlugins>

TA_BASEFUNS_CTORS_DEFN(PluginMakeTask);


void PluginMakeTask::Initialize() {
}

void PluginMakeTask::Destroy() {
}

void PluginMakeTask::run() {
  PluginMakeThreadMgr* mkmg = mgr();
  if(!mkmg) return;
  if(mkmg->isDestroying()) return; // checks owner..
  if(!mkmg->make_pending) return;

  taPlugins::MakePlugin_impl(mkmg->plugin_path, mkmg->plugin_name, mkmg->system_plugin,
                             mkmg->full_rebuild);
  mkmg->make_pending = false;
}

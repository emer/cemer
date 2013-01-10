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

#include "taPluginInst.h"

taPluginInst::taPluginInst(const String& fileName)
:inherited(fileName)
{
  name = PluginNameFmFileName(fileName);
  plugin_rep = NULL;
  load_state = LS_NOT_LOADED;
}

IPlugin* taPluginInst::plugin() {
  QObject* in = instance();
  return qobject_cast<IPlugin*>(in);
}

String taPluginInst::PluginNameFmFileName(const String& fname) {
  String plugin_nm = taMisc::GetFileFmPath(fname).before(".");
  if(plugin_nm.startsWith("lib")) plugin_nm = plugin_nm.after("lib");
  if(taMisc::app_suffix.nonempty())
    plugin_nm = plugin_nm.before(taMisc::app_suffix);
  return plugin_nm;
}

bool taPluginInst::InitPlugin() {
  IPlugin* ipl = plugin();
  int err = ipl->InitializePlugin();
  if (err == 0) {
    load_state = LS_PLUG_INIT;
    return true;
  } else {
    taMisc::Error("**Initialization of plugin '", fileName().toLatin1(), "' failed with err code ", String(err),
      " -- try recompiling the plugin, or else remove it from the plugin folder");
  }
  load_state = LS_INIT_FAIL;
  return false;
}

bool taPluginInst::InitTypes() {
  IPlugin* ipl = plugin();
  if (!plugin_rep) {
    taMisc::Warning("Unexpected missing plugin_rep for: ", fileName().toLatin1());
  } else if (ipl) {
    taMisc::in_plugin_init++;
    TypeDef* td_last = taMisc::plugin_loading;
    taMisc::plugin_loading = plugin()->GetTypeDef();
    int err = ipl->InitializeTypes();
    taMisc::plugin_loading = td_last;
    taMisc::in_plugin_init--;
    if (err == 0) {
      load_state = LS_TYPE_INIT;
      return true;
    } else {
      taMisc::Warning("Plugin::InitializeTypes() failed with error code ", String(err),
        " for plugin: ", fileName().toLatin1());
    }
  } else {
    taMisc::Warning("Could not get IPlugin interface for plugin: ", fileName().toLatin1());
  }
  load_state = LS_TYPE_FAIL;
  return false;
}


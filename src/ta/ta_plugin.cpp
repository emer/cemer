// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/CSS
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

// ta_plugin.cpp

#include "ta_plugin.h"

#include "ta_type.h"
#include "ta_platform.h"

#include <QDir>


//////////////////////////
//  taPlugin		//
//////////////////////////

taPlugin::taPlugin(const String& fileName)
:inherited(fileName)
{
}

IPlugin* taPlugin::plugin() {
  QObject* in = instance();
  return qobject_cast<IPlugin*>(in); 
}


//////////////////////////
//  taPlugins		//
//////////////////////////

String_PArray taPlugins::plugin_folders; 
taPlugin_PList taPlugins::plugins; 

void taPlugins::AddPluginFolder(const String& folder) {
  plugin_folders.AddUnique(folder);
}

taPlugin* taPlugins::LoadPlugin(const String& fileName) {
  bool failed = false;
  taPlugin* rval = new taPlugin(fileName);
  taMisc::Warning("Attempting to load plugin: ", fileName);
  if (rval->load()) {
    // get the plugin object, and initialize types
    IPlugin* ipl = rval->plugin();
    if (ipl) {
      int err = ipl->InitializeTypes();
      if (err != 0) {
        failed = true;
        taMisc::Warning("**Plugin::InitializeTypes() failed with error code ", String(err),
          " for plugin: ", fileName);
     }
    } else {
      failed = true;
      taMisc::Warning("**Could not get IPlugin interface for plugin: ", fileName);
    }
  } else {
    failed = true;
    taMisc::Warning("**Could not load plugin: ", fileName, " (check versions, try recompiling plugin)");
  }
  if (failed) {
    delete rval;
    rval = NULL;
  } else {
    // TODO: log success
    taMisc::Warning("Loaded plugin: ", fileName);
  }
  return rval;
}

void taPlugins::InitPlugins() {
  taPlugin* pl;
  IPlugin* ipl;
  int err;
  // note: we don't kill plugins that have initialization errors, but 
  // they may cause problems
  for (int i = 0; i < plugins.size; ++i) {
    pl = plugins.FastEl(i);
    ipl = pl->plugin();
    err = ipl->InitializePlugin();
    if (err != 0) {
      taMisc::Error("**Initialization of plugin '", pl->fileName(), "' failed with err code ", String(err),
        " -- try recompiling the plugin, or else remove it from the plugin folder");
    }
  } 
}

void taPlugins::LoadPlugins() {
  String folder;
  for (int i = 0; i < plugin_folders.size; ++i) {
    folder = plugin_folders.FastEl(i);
    QDir pluginsDir(folder);
    // enumerate all files in the folder, and try loading
    foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {
      taPlugin* pl = LoadPlugin(pluginsDir.absoluteFilePath(fileName));
      if (pl) {
        plugins.Add(pl);
      }
    }
  } 
}

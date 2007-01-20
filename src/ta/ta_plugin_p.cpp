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

// ta_plugin_p.cpp

#include "ta_plugin_p.h"

#include "ta_qtclipdata.h"
#include "ta_platform.h"

#include <QDir>


//////////////////////////
//  taPluginInst	//
//////////////////////////

taPluginInst::taPluginInst(const String& fileName)
:inherited(fileName)
{
  load_state = LS_NOT_LOADED;
}

IPlugin* taPluginInst::plugin() {
  QObject* in = instance();
  return qobject_cast<IPlugin*>(in); 
}


bool taPluginInst::InitPlugin() {
  IPlugin* ipl = plugin();
  int err = ipl->InitializePlugin();
  if (err == 0) {
    load_state = LS_PLUG_INIT;
    return true;
  } else {
    taMisc::Error("**Initialization of plugin '", fileName(), "' failed with err code ", String(err),
      " -- try recompiling the plugin, or else remove it from the plugin folder");
  }
  load_state = LS_INIT_FAIL;
  return false;
}

bool taPluginInst::InitTypes() {
  IPlugin* ipl = plugin();
  if (ipl) {
    int err = ipl->InitializeTypes();
    if (err == 0) {
      load_state = LS_TYPE_INIT;
      return true;
    } else {
      taMisc::Warning("**Plugin::InitializeTypes() failed with error code ", String(err),
        " for plugin: ", fileName());
    }
  } else {
    taMisc::Warning("**Could not get IPlugin interface for plugin: ", fileName());
  }
  load_state = LS_TYPE_FAIL;
  return false;
}

//////////////////////////
//  taPlugins		//
//////////////////////////

String_PArray taPlugins::plugin_folders; 
taPluginInst_PList taPlugins::plugins; 

void taPlugins::AddPluginFolder(const String& folder) {
  plugin_folders.AddUnique(folder);
}

taPluginInst* taPlugins::LoadPlugin(const String& fileName) {
  taPluginInst* rval = new taPluginInst(fileName);
//TODO: log to a file  taMisc::Warning("Attempting to load plugin: ", fileName);
  // get the plugin object, and initialize types
  if (!rval->load()) {
    taMisc::Warning("**Could not load plugin: ", fileName, " (check versions, try recompiling plugin)");
    delete rval;
    rval = NULL;
  }
  if (rval) {
    // TODO: log success
//TODO: commit log    taMisc::Warning("Loaded plugin: ", fileName);
  }
  return rval;
}

void taPlugins::EnumeratePlugins() {
  String folder;
  for (int i = 0; i < plugin_folders.size; ++i) {
    folder = plugin_folders.FastEl(i);
    QDir pluginsDir(folder);
    // enumerate all files in the folder, and try loading
    foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {
      taPluginInst* pl = LoadPlugin(pluginsDir.absoluteFilePath(fileName));
      if (pl) {
        pl->load_state = taPluginInst::LS_LOADED;
        plugins.Add(pl);
      }
    }
  } 
}


//////////////////////////
//  taPlugin		//
//////////////////////////

void taPlugin::Initialize() {
  enabled = false; // be conservative, and require user to enable
  loaded = false;
  reconciled = false;
  plugin = NULL;
}

void taPlugin::Copy_(const taPlugin& cp) {
  enabled = cp.enabled;
  loaded = false; // never for a copy
  reconciled = false;
  name = cp.name;
  desc = cp.desc;
  unique_id = cp.unique_id;
  version = cp.version;
  filename = cp.filename;
  url = cp.url;
  //intore plugin -- only used for insts, not descs
}

//////////////////////////
//  taPlugin_List	//
//////////////////////////

void taPlugin_List::LoadPlugins() {
  // first, get us uptodate with actual plugin list
  ReconcilePlugins();
  
  // unload any plugins not used
  for (int i = size - 1; i >= 0; --i) {
    taPlugin* pl = FastEl(i);
    if (pl->enabled) continue;
    
    taPluginInst* pli = pl->plugin;
    pli->unload();
    pl->plugin = NULL;
    taPlugins::plugins.RemoveEl(pli);
  }
    
  // register types for everyone, before trying to init any plugin
  for (int i = 0; i < size; ++i) {
    taPlugin* pl = FastEl(i);
    if (!pl->enabled) continue;
    
    taPluginInst* pli = pl->plugin;
    
    if (pli->load_state != taPluginInst::LS_LOADED) continue;
    if (!pli->InitTypes()) continue;
  } 
  
  // finally, we can formally initialize each plugin
  for (int i = 0; i < size; ++i) {
    taPlugin* pl = FastEl(i);
    if (!pl->enabled) continue;
    
    taPluginInst* pli = pl->plugin;
    if (pli->load_state != taPluginInst::LS_TYPE_INIT) continue;
    if (!pli->InitPlugin()) continue;
    pl->loaded = true;
    pl->DataChanged(DCR_ITEM_UPDATED);
  } 
}


void taPlugin_List::ReconcilePlugins() {
  taVersion ver;
  for (int i = 0; i < taPlugins::plugins.size; ++i) {
    taPluginInst* pli = taPlugins::plugins.FastEl(i);
    IPlugin* ip = pli->plugin();
    String uid = ip->uniqueId();
    taPlugin* pl = FindName(uid);
    if (pl) {
      // update
    } else {
      // create new 
      pl = (taPlugin*)New(1);
      pl->unique_id = uid;
      //TEMP: enable by default, until save/load of prefs is done
      pl->enabled = true;
    }
    // update or init fields
    pl->plugin = pli;
    pl->reconciled = true;
    pl->name = ip->name();
    pl->desc = ip->desc();
    ver.Clear();
    ip->GetVersion(ver);
    pl->version = ver.toString();
    pl->filename = pli->fileName();
    
    pl->DataChanged(DCR_ITEM_UPDATED);
  }
   
   // now, nuke any that are missing
   for (int i = size - 1; i >= 0; --i) {
     taPlugin* pl = FastEl(i);
     if (!pl->reconciled)
       RemoveIdx(i);
   }
}

void taPlugin_List::QueryEditActions_impl(const taiMimeSource* ms,
  int& allowed, int& forbidden)
{
  allowed = taiClipData::EA_COPY;
  forbidden = ~taiClipData::EA_COPY;
}

void taPlugin_List::ChildQueryEditActions_impl(const MemberDef* md, const taBase* child,
    const taiMimeSource* ms, int& allowed, int& forbidden)
{
  allowed = taiClipData::EA_COPY;
  forbidden = ~taiClipData::EA_COPY;
}

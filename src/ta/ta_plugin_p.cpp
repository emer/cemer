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
  plugin_rep = NULL;
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
  if (!plugin_rep) {
    taMisc::Warning("Unexpected missing plugin_rep for: ", fileName());
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
        " for plugin: ", fileName());
    }
  } else {
    taMisc::Warning("Could not get IPlugin interface for plugin: ", fileName());
  }
  load_state = LS_TYPE_FAIL;
  return false;
}

//////////////////////////
//  taPlugins		//
//////////////////////////

String_PArray taPlugins::plugin_folders; 
taPluginInst_PList taPlugins::plugins; 
String taPlugins::logfile;

void taPlugins::AddPluginFolder(const String& folder) {
  plugin_folders.AddUnique(folder);
}

void taPlugins::InitLog(const String& logfile_) {
  logfile = logfile_;
  //dummy open/create
  ofstream ofs(logfile_, ios::out);
  if (!ofs.good()) {
    taMisc::Warning("Could not open plugin log file:", logfile);
  } else
    ofs.close();
}

void taPlugins::AppendLogEntry(const String& entry) {
  ofstream ofs(logfile, ios::app);
  if (!ofs.good()) {
    taMisc::Warning("Could not open plugin log file:", logfile);
  } else {
    ofs << entry << "\n";
    ofs.close();
  }
}

taPluginInst* taPlugins::LoadPlugin(const String& fileName) {
  taPluginInst* rval = new taPluginInst(fileName);
  taMisc::Warning("Attempting to load plugin: ", fileName);
  // get the plugin object, and initialize types
  String log_entry;
  if (rval->load())  {
    rval->load_state = taPluginInst::LS_LOADED;
    log_entry = "Loaded: " + fileName;
  } else {
    rval->load_state = taPluginInst::LS_LOAD_FAIL;
#if QT_VERSION >= 0x040200
    log_entry = "**Could not load: " + fileName + String("; Qt error msg: ").cat(
      rval->errorString().toLatin1().data());
#else
    log_entry = "**Could not load: " + fileName;
#endif
    taMisc::Warning(log_entry);
  }
  AppendLogEntry(log_entry);
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
        plugins.Add(pl);
      }
    }
  } 
}


//////////////////////////
//  taPluginBase	//
//////////////////////////

void taPluginBase::Initialize() {
}

void taPluginBase::Copy_(const taPluginBase& cp) {
  name = cp.name;
  desc = cp.desc;
  unique_id = cp.unique_id;
  version = cp.version;
  url = cp.url;
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

void taPlugin::Copy_(const taPlugin& cp) { // usually not copied
  filename = cp.filename;
  enabled = cp.enabled;
  loaded = false; // never for a copy
  reconciled = false;
  plugin = NULL;
}


//////////////////////////
//  taPluginDep		//
//////////////////////////

void taPluginDep::Initialize() {
  dep_check = DC_OK;
}

void taPluginDep::Copy_(const taPluginDep& cp) {
  dep_check = cp.dep_check; // not really used for copies
}

void taPluginDep::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  if (dep_check != DC_OK) {
    if (!quiet) {
      String msg; 
      switch (dep_check) {
      case DC_MISSING: msg = " is missing"; break;
      case DC_NOT_LOADED: msg = " is not loaded"; break;
      default: break; // compiler food
      }
      taMisc::CheckError("Required plugin: ", 
        GetDisplayName(), msg);
    }
    rval = false;
  }
}


//////////////////////////
//  taPluginBase_List	//
//////////////////////////

taPluginBase* taPluginBase_List::FindUniqueId(const String& value) {
  for (int i = 0; i < size; ++i) {
    taPluginBase* rval = FastEl(i);
    if (!rval) continue;
    if (rval->unique_id == value) return rval;
  }
  return NULL;
}

void taPluginBase_List::QueryEditActions_impl(const taiMimeSource* ms,
  int& allowed, int& forbidden)
{
  allowed = taiClipData::EA_COPY;
  forbidden = ~taiClipData::EA_COPY;
}

void taPluginBase_List::ChildQueryEditActions_impl(const MemberDef* md, const taBase* child,
    const taiMimeSource* ms, int& allowed, int& forbidden)
{
  allowed = taiClipData::EA_COPY;
  forbidden = ~taiClipData::EA_COPY;
}

//////////////////////////
//  taPlugin_List	//
//////////////////////////

taPlugin* taPlugin_List::FindFilename(const String& value) {
  for (int i = 0; i < size; ++i) {
    taPlugin* rval = FastEl(i);
    if (!rval) continue;
    if (rval->filename == value) return rval;
  }
  return NULL;
}

void taPlugin_List::LoadPlugins() {
  // first, get us uptodate with actual plugin list
  ReconcilePlugins();
  
  // unload any plugins not used
  for (int i = size - 1; i >= 0; --i) {
    taPlugin* pl = FastEl(i);
    if (pl->enabled) continue;
    
    taPluginInst* pli = pl->plugin;
    if (!pli) continue;
    
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
  // go through all the successfully or unsucessfully loaded plugins
  for (int i = 0; i < taPlugins::plugins.size; ++i) {
    taPluginInst* pli = taPlugins::plugins.FastEl(i);
    taPlugin* pl = NULL;
    // if loaded, match by id, else match by filename (so we still track failed loads)
    if (pli->load_state == taPluginInst::LS_LOADED) {
      IPlugin* ip = pli->plugin();
      String uid = ip->uniqueId();
      pl = FindUniqueId(uid);
      // if it was recorded before, update, otherwise make new
      if (pl) {
        // update
      } else {
        // create new 
        pl = (taPlugin*)New(1);
        pl->unique_id = uid;
      }
      pl->name = ip->name();
      pl->desc = ip->desc();
      ver.Clear();
      ip->GetVersion(ver);
      pl->version = ver.toString();
    } else { // not loaded -- but match up the filename to a persistent guy if found
      pl = FindFilename(pli->fileName());
      // we don't update anything, and we don't make a new guy if not found before,
      // since we only persistently track successful plugins, not failed ones
      // init/update
      if (pl) {
      } else {
        // create new 
        pl = (taPlugin*)New(1);
        // since we can't know anything about the guy, just name him by the file
        pl->name = QFileInfo(pli->fileName()).fileName();
      }
    }
    pli->plugin_rep = pl; // can be null
    if (pl) {
      // update or init fields
      pl->plugin = pli;
      pl->reconciled = true;
      pl->filename = pli->fileName();
      pl->DataChanged(DCR_ITEM_UPDATED);
    }
  }
   
   // now, nuke any that are missing
   for (int i = size - 1; i >= 0; --i) {
     taPlugin* pl = FastEl(i);
     if (!pl->reconciled)
       RemoveIdx(i);
   }
}


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

#include "taPlugin_List.h"
#include <taPlugin>
#include <taPluginInst>
#include <taPlugins>
#include <taVersion>
#include <IPlugin>

#include <taMisc>

#include <QFileInfo>


using namespace std;


static void RootFilename(String& f) {
  QFileInfo fi(f);
  f = fi.baseName();
  // just blindly remove any suffixes we use
  f.gsub("_dbg", "");
  f.gsub("_nogui", "");
  f.gsub("_mpi", "");
}

static bool RootFilenameEq(String f1, String f2) {
  //note: sleazy: we just see if either matches first part of other
  RootFilename(f1);
  RootFilename(f2);
  return (f1 == f2);
}

taPlugin* taPlugin_List::FindFilename(const String& value) {
  // we only look for a match to the un-path'ed, un-suffixed name
  // ex /blah/myplugin.so == /foo/myplugin_dbg.so
  for (int i = 0; i < size; ++i) {
    taPlugin* rval = FastEl(i);
    if (!rval) continue;
    if (RootFilenameEq(rval->filename, value))
      return rval;
  }
  return NULL;
}

void taPlugin_List::ListAllPlugins() {
  ReconcilePlugins();

  for (int i = 0; i < size; ++i) {
    taPlugin* pl = FastEl(i);
    cerr << i << ":\t" << pl->name << "\t" << (pl->enabled ? "enabled" : "disabled")
         << "\t" << pl->desc << "\t" << pl->unique_id << "\t" << pl->version << "\t"
         << pl->filename << endl;
  }
}

void taPlugin_List::EnableAllPlugins() {
  ReconcilePlugins();

  for (int i = 0; i < size; ++i) {
    taPlugin* pl = FastEl(i);
    pl->enabled = true;
    cerr << i << ":\t" << pl->name << "\t" << (pl->enabled ? "enabled" : "disabled")
         << "\t" << pl->desc << "\t" << pl->unique_id << "\t" << pl->version << "\t"
         << pl->filename << endl;
  }
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

  // make a blank line in log
  taPlugins::AppendLogEntry("");

  String log_entry;
  // register types for everyone
  for (int i = 0; i < size; ++i) {
    taPlugin* pl = FastEl(i);
    if (!pl->enabled) continue;

    taPluginInst* pli = pl->plugin;

    if (pli->load_state != taPluginInst::LS_LOADED) continue;

    log_entry = "Attempting to InitTypes for plugin: " + pl->filename;
    taPlugins::AppendLogEntry(log_entry);
    if (pli->InitTypes()) {
      log_entry = "Successfully ran InitTypes for plugin: " + pl->filename;
      taPlugins::AppendLogEntry(log_entry);
    } else {
      log_entry = "Could not run InitTypes for plugin: " + pl->filename;
      taPlugins::AppendLogEntry(log_entry, true);
      continue; // in case more is added after this if
    }
  }
}


void taPlugin_List::InitPlugins() {
  // make a blank line in log
  taPlugins::AppendLogEntry("");

  String log_entry;
  // initialize each plugin
  for (int i = 0; i < size; ++i) {
    taPlugin* pl = FastEl(i);
    if (!pl->enabled) continue;

    taPluginInst* pli = pl->plugin;
    if (pli->load_state != taPluginInst::LS_TYPE_INIT) continue;
    log_entry = "Attempting to InitPlugin for plugin: " + pl->filename;
    taPlugins::AppendLogEntry(log_entry);
    if (pl->InitPlugin()) {
      log_entry = "Successfully ran InitPlugin for plugin: " + pl->filename;
      taPlugins::AppendLogEntry(log_entry);
    } else {
      log_entry = "Could not run InitPlugin for plugin: " + pl->filename;
      taPlugins::AppendLogEntry(log_entry, true);
      continue;
    }
    pl->SigEmitUpdated();
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
      ip->GetVersion(ver);
      pl->version = ver.toString();
    }
    else { // not loaded -- but match up the root filename to a persistent guy if found
      pl = FindFilename(pli->fileName());
      // we don't update anything, and we don't make a new guy if not found before,
      // since we only persistently track successful plugins, not failed ones
      // init/update
      if (pl) {
      }
      else {
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
      pl->up_to_date = (pli->load_state != taPluginInst::LS_OUT_OF_DATE);
      pl->mod_time = pli->mod_time;
      pl->mod_time_int = pli->mod_time_int;
      pl->SigEmitUpdated();
    }
  }

   // now, nuke any that are missing
   for (int i = size - 1; i >= 0; --i) {
     taPlugin* pl = FastEl(i);
     if (!pl->reconciled)
       RemoveIdx(i);
   }
}

void taPlugin_List::ViewPluginLog() {
  if (taPlugins::logfile.empty()) return; // shouldn't happen...
  taMisc::EditFile(taPlugins::logfile);
}

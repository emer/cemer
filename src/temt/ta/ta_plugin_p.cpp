// Copyright, 1995-2007, Regents of the University of Colorado,
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

// ta_plugin_p.cpp

#include "ta_plugin_p.h"

#include "ta_qtclipdata.h"
#include "ta_platform.h"
#include "ta_project.h"

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

void taPlugins::AppendLogEntry(const String& entry, bool warn) {
  ofstream ofs(logfile, ios::app);
  if (!ofs.good()) {
    taMisc::Warning("Could not open plugin log file:", logfile);
  } else {
    if (warn) taMisc::Warning(entry);
#ifdef DEBUG
    else      taMisc::Info(entry);
#endif
    if (warn)
      ofs << "**WARNING: ";
    ofs << entry << "\n";
    ofs.close();
  }
}

taPluginInst* taPlugins::ProbePlugin(const String& fileName) {
  taPluginInst* rval = new taPluginInst(fileName);
  String log_entry = "Attempting to probe plugin: " + fileName;
#ifdef DEBUG
  taMisc::Info(log_entry);
#endif
  AppendLogEntry(log_entry);
  // get the plugin object, and initialize types
  if (rval->load())  {
    rval->load_state = taPluginInst::LS_LOADED;
    log_entry = "Successfully probed plugin: " + fileName;
    AppendLogEntry(log_entry);
  } else {
    rval->load_state = taPluginInst::LS_LOAD_FAIL;
#if QT_VERSION >= 0x040200
    log_entry = "Could not probe: " + fileName + String("; Qt error msg: ").cat(
      rval->errorString().toLatin1().data());
#else
    log_entry = "Could not probe: " + fileName;
#endif
    AppendLogEntry(log_entry, true);
  }
  return rval;
}

void taPlugins::EnumeratePlugins() {
  String folder;
  for (int i = 0; i < plugin_folders.size; ++i) {
    folder = plugin_folders.FastEl(i);
    QDir pluginsDir(folder);
    // enumerate all files in the folder, filter by build string, then try loading
    // for platforms with known dylib suffixes, only look for those
    QString filt("*");
    if (taMisc::build_str.nonempty()) {
      filt.append("_").append(taMisc::build_str.chars());
    }
#ifdef TA_OS_LINUX
    filt += ".so";
#elif defined(TA_OS_WIN)
    filt += ".dll";
#elif  defined(TA_OS_MAC)
    filt += ".dylib";
#else // huh??? what else?
    filt += ".*";
#endif
    pluginsDir.setNameFilter(filt);
    foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {
      taPluginInst* pl = ProbePlugin(pluginsDir.absoluteFilePath(fileName));
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
  state_type = NULL;
}

void taPlugin::Copy_(const taPlugin& cp) { // usually not copied
  filename = cp.filename;
  enabled = cp.enabled;
  loaded = false; // never for a copy
  reconciled = false;
  plugin = NULL;
  state_type = cp.state_type; // ???
}

bool taPlugin::InitPlugin() {
  if (!plugin) return false;
  if (!plugin->InitPlugin()) return false;
  loaded = true; // we are officially "loaded" -- need this for state...
  // find state type, if any -- qualify before assigned to pl
  // we show to user for clarity, and if they want to add after the fact
  state_classname = capitalize(name) + "PluginState";
  TypeDef* pst = taMisc::types.FindName(state_classname);
  if (pst && pst->DerivesFrom(&TA_taFBase))
    state_type = pst;
  if (state_type && tabMisc::root) {
    taFBase* opt = (taFBase*)tabMisc::root->plugin_state.New(
      1, state_type, state_type->name + "_inst");
    if (!opt) {
      taPlugins::AppendLogEntry("Could not create state object for plugin");
      return false;
    }
    String filename = taMisc::prefs_dir + PATH_SEP + opt->name;
    opt->SetFileName(filename);
    if (taPlatform::fileExists(filename)) {
      int res = opt->Load(filename);
      // note: we don't fail the plugin just because state didn't load...
      if (!res) {
        String msg = "Load of previously saved state for " + opt->name +
          " did not succeed from file: '" + filename + "'";
        taMisc::Warning(msg);
        taPlugins::AppendLogEntry(msg);
      }
    }
  }
  return true;
}

void taPlugin::PluginOptions() {
  taBase* opt_tab = NULL;
  if (state_type && tabMisc::root) {
    opt_tab = tabMisc::root->plugin_state.FindType(state_type);
  }
  if (opt_tab) {
    if (opt_tab->EditDialog(true)) {
      opt_tab->Save();
    }
  } else {
    taMisc::Confirm("This plugin does not have user-configurable options.");
  }
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


void taPlugin_List::ViewPluginLog() {
  if (taPlugins::logfile.empty()) return; // shouldn't happen...
  taMisc::EditFile(taPlugins::logfile);
  
}


//////////////////////////
//  PluginWizard	//
//////////////////////////

// NOTE: header is in ta_project.cpp due to dependency issues

void PluginWizard::Initialize() {
  plugin_name = "myplugin";
  class_name_prefix = "Myplugin";
  plugin_type = UserPlugin;
  default_location = true;
  validated = false;
  plugin_location = taMisc::user_app_dir + PATH_SEP + "plugins" + PATH_SEP +
        plugin_name;
  desc = "enter description of your plugin";
  uniqueId = "pluginname.dept.organization.org";
  url = "(replace this with a url for help or information on the plugin)";
}

void PluginWizard::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  validated = false;
  // modify it to force it to be C-valid
  plugin_name = taMisc::StringCVar(plugin_name);
  class_name_prefix = capitalize(plugin_name);
  if (default_location) {
    if (plugin_type == UserPlugin) {
      plugin_location = taMisc::user_app_dir + PATH_SEP + "plugins" + PATH_SEP +
        plugin_name;
    } else {
      plugin_location = taMisc::app_dir + PATH_SEP + "plugins" + PATH_SEP +
        plugin_name;
    }
  } else {
#ifdef TA_OS_WIN
    plugin_location.gsub("/", "\\");
#endif
    plugin_location = taPlatform::noFinalSep(plugin_location);
  }
}

void PluginWizard::CheckThisConfig_impl(bool quiet, bool& ok) {
  inherited::CheckThisConfig_impl(quiet, ok);
  // must be C valid, otherwise bail -- we assume our transform made it legal...
  if (CheckError(plugin_name.empty(), quiet, ok,
    "you must provide a C-valid name for your plugin")) 
    return;
  
  CheckError((plugin_name == "template"), quiet, ok,
    "you cannot use the name \"template\"");
  //TODO: do our name conflict checks!
  //TODO: check if a plugin already exists there!
}

bool PluginWizard::Validate() {
  validated = CheckConfig_Gui();
  return validated;
}

void PluginWizard::AddTemplatedFiles(bool upgrade_only) {
  files.Add("CMakeLists.txt");
  if (upgrade_only) return;
  files.Add("template.cpp");
  files.Add("template_def.h");
  files.Add("template.h");
  files.Add("template_pl.cpp");
  files.Add("template_pl.h");
  files.Add("template_qtso.cpp");
  files.Add("template_qtso.h");
}

void PluginWizard::AddCopyFiles(bool upgrade_only) {
  if (upgrade_only) return;
#ifndef TA_OS_WIN
  files.Add("configure");
#endif
}
void PluginWizard::TemplatizeFile(const String& src_file,
  const String& src, String& dst, bool& ok)
{
  dst = src;
  dst.makeUnique();
  dst.gsub("template", plugin_name);
  dst.gsub("Template", class_name_prefix);
  dst.gsub("TEMPLATE", upcase(plugin_name));
  if (src_file.contains("CMakeLists.txt")) {
    dst.gsub("@PLUGIN_VERSION_MAJOR@", version.major);
    dst.gsub("@PLUGIN_VERSION_MINOR@", version.minor);
    dst.gsub("@PLUGIN_VERSION_PATCH@", version.step);
  }
  // specific values that occur in the template_pl files
  if (src_file.contains("template_pl.")) {
    dst.gsub("@EMERGENT_PLUGIN_DESC@", desc);
    dst.gsub("@EMERGENT_PLUGIN_UNIQUEID@", uniqueId);
    dst.gsub("@EMERGENT_PLUGIN_URL@", desc);
    dst.gsub("@PLUGIN_VERSION_MAJOR@", version.major);
    dst.gsub("@PLUGIN_VERSION_MINOR@", version.minor);
    dst.gsub("@PLUGIN_VERSION_PATCH@", version.step);
  }
//TODO: detailed enablings of things like _qtso names, etc.
}

void PluginWizard::CreateDestFile(const String& src_file, 
    const String& dst_file, bool& ok)
{
  fstream fsrc, fdst;
  fsrc.open(src_file, ios::in | ios::binary);
  if (TestError((!fsrc.is_open()),
    "PluginWizard::CreateDestFile", 
    "Could not open template file:", src_file)) {
    ok = false; 
    return;
  }
  fdst.open(dst_file, ios::out | ios::binary);
  if (TestError((!fdst.is_open()),
    "PluginWizard::CreateDestFile", 
    "Could not open destination file:", dst_file,
    " -- make sure the file does not exist and/or is not write_protected and/or you have permission to create files in the destination")) {
    ok = false; 
    return;
  }
  String src;
  if (TestError(src.Load_str(fsrc),
    "PluginWizard::CreateDestFile", 
    "Could not read contents of file:", src_file)) {
    ok = false; 
    return;
  }
  fsrc.close();
  String dst;
  TemplatizeFile(src_file, src, dst, ok);
  if (!ok) return;
  if (TestError((dst.Save_str(fdst)),
    "PluginWizard::CreateDestFile", 
    "Could not write results to:", dst_file,
    " -- make sure the file does not exist and/or is not write_protected and/or you have permission to create files in the destination")) {
    ok = false; 
    return;
  }
  fdst.close();
}

bool PluginWizard::MakePlugin() {
  bool upgrade_only = false; //TODO: check for upgrade
  if (TestError((!validated),
    "PluginWizard::MakePlugin", 
    "You must Validate the plugin before you can make it"))
    return false;
  // extract dirs
  src_dir = taMisc::app_dir + PATH_SEP + "plugins" + PATH_SEP + "template" + PATH_SEP;
  
  // make the dest dir
  if (TestError(!taPlatform::mkdir(plugin_location),
    "PluginWizard::MakePlugin", 
    "Could not make folder for plugin -- make sure the path is valid, and you have permission to create a folder in that location"))
    return false;
  // std build dirs
  if (TestError(!taPlatform::mkdir(plugin_location + PATH_SEP + "build"),
    "PluginWizard::MakePlugin", 
    "Could not make 'build' subfolder for plugin -- make sure the path is valid, and you have permission to create a folder in that location"))
    return false;
  if (TestError(!taPlatform::mkdir(plugin_location + PATH_SEP + "build_dbg"),
    "PluginWizard::MakePlugin", 
    "Could not make 'build' subfolder for plugin -- make sure the path is valid, and you have permission to create a folder in that location"))
    return false;
  
  // build file list to templatize
  files.Reset();
  AddTemplatedFiles(upgrade_only);
  
  // iterate files
  bool ok = true;
  for (int i = 0; (i < files.size) && ok; ++i) {
    String src_file = files[i];
    String dst_file = src_file;
    // note: files like CMakeLists.txt keep their name
    dst_file.gsub("template", plugin_name);
    CreateDestFile(
      src_dir + src_file, 
      plugin_location + PATH_SEP + dst_file, 
      ok
    );
  }
  
  // files to just link or copy (no templating)
  files.Reset();
  AddCopyFiles(upgrade_only);
  
  // iterate files
  for (int i = 0; (i < files.size) && ok; ++i) {
    String src_file = src_dir + files[i];
    String dst_file = plugin_location + PATH_SEP + files[i];
    // note: files like CMakeLists.txt keep their name
    dst_file.gsub("template", plugin_name);
    // try linking first
    if (!QFile::link(src_file, dst_file))
      ok = QFile::copy(src_file, dst_file);
  }
  if (ok) {
    taMisc::Info("The plugin was created successfully! To build and install the plugin... TODO: instructions");
  } else {
    taMisc::Error(
      "PluginWizard::MakePlugin", 
      "Could not copy and templatize files for plugin -- make sure the path is valid, and you have permission to create files in that location");
    return false;
  }
  return true;
}

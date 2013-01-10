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

#include "taPlugin.h"

void taPlugin::Initialize() {
  enabled = false; // be conservative, and require user to enable
  loaded = false;
  up_to_date = false;
  reconciled = false;
  plugin = NULL;
  state_type = NULL;
  mod_time_int = 0;
}

void taPlugin::Copy_(const taPlugin& cp) { // usually not copied
  filename = cp.filename;
  enabled = cp.enabled;
  loaded = false; // never for a copy
  up_to_date = false;
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

  // Just cap first letter -- capitalize() also downcases the rest of the string.
  state_classname = name + "PluginState";
  state_classname[0] = (char) toupper(state_classname[0]);

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
    if (taMisc::FileExists(filename)) {
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

void taPlugin::ParseFileName(String& base_path, String& plugin_nm_full, String& plugin_nm) {
  base_path = taMisc::GetDirFmPath(filename);
  plugin_nm_full = taMisc::GetFileFmPath(filename);
  plugin_nm = taPluginInst::PluginNameFmFileName(plugin_nm_full);
}

bool taPlugin::Compile() {
  String base_path;  String plugin_nm_full;  String plugin_nm;
  ParseFileName(base_path, plugin_nm_full, plugin_nm);

  String plug_path = base_path + PATH_SEP + plugin_nm;
  QFileInfo qfi(plug_path);
  if(!qfi.isDir()) {
    taMisc::Error("Compile Plugin -- plugin directory named:", plug_path, "does not exist for plugin file named:", plugin_nm_full, "cannot build plugin");
    return false;
  }

  bool sys_plug = false;
  if(plug_path.contains(taMisc::app_plugin_dir)) sys_plug = true;

  taPlugins::MakePlugin(plug_path, plugin_nm, sys_plug);

  return true;
}

bool taPlugin::Clean() {
  String base_path;  String plugin_nm_full;  String plugin_nm;
  ParseFileName(base_path, plugin_nm_full, plugin_nm);

  bool sys_plug = false;
  if(base_path.contains(taMisc::app_plugin_dir)) sys_plug = true;

  taPlugins::CleanPlugin(base_path, plugin_nm_full, sys_plug);

  return true;
}

bool taPlugin::LoadWiz() {
  String base_path;  String plugin_nm_full;  String plugin_nm;
  ParseFileName(base_path, plugin_nm_full, plugin_nm);

  String plug_path = base_path + PATH_SEP + plugin_nm;
  QFileInfo qfi(plug_path);
  if(!qfi.isDir()) {
    taMisc::Error("LoadWiz Plugin -- plugin directory named:", plug_path, "does not exist for plugin file named:", plugin_nm_full, "cannot load wizard");
    return false;
  }

  String wiz_file;

  String orig_src_file = plug_path + PATH_SEP + "orig_src_path.txt";
  String orig_src_path;
  QFileInfo qfiosf(orig_src_file);
  if(!qfiosf.isFile()) {
    taMisc::Error("LoadWiz Plugin -- orig_src_path.txt file not found in:", plug_path,
                  "(plugin predates version 5.1) -- may not find the correct wizard");
    wiz_file = plug_path + PATH_SEP + "PluginWizard.wiz";
    QFileInfo qfiwiz(wiz_file);
    if(!qfiwiz.isFile()) {
      taMisc::Error("LoadWiz Plugin -- PluginWizard.wiz file not found in:", plug_path,
                    "(plugin predates version 5.0.2) -- auto wizard loading not possible -- try to find the original source directory manually to see if the file might be there");
      return false;
    }
  }
  else {
    fstream spstr;
    spstr.open(orig_src_file, ios::in);
    orig_src_path.Load_str(spstr);
    spstr.close();
    wiz_file = orig_src_path + PATH_SEP + "PluginWizard.wiz";
  }

  PluginWizard* wiz = (PluginWizard*)tabMisc::root->wizards.FindName("PluginWizard");
  if(!wiz) {
    taMisc::Error("LoadWiz Plugin -- PluginWizard object not found in root -- this is weird and should be reported to developers");
    return false;
  }

  taMisc::Info("Loading wizard info from:", wiz_file);

  wiz->LoadWiz(wiz_file);
  if(orig_src_path.nonempty())
    wiz->plugin_location = orig_src_path; // update it!
  wiz->ShowWiz();
  return true;
}

bool taPlugin::Editor() {
  bool rval = LoadWiz();
  if(!rval) return false;

  PluginWizard* wiz = (PluginWizard*)tabMisc::root->wizards.FindName("PluginWizard");
  if(!wiz) return false;
  return wiz->Editor();
}

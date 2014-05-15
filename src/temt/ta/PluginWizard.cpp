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

#include "PluginWizard.h"
#include <taPlugins>
#include <iPluginEditor>
#include <MainWindowViewer>
#include <iMainWindowViewer>

#include <taMisc>
#include <tabMisc>
#include <taRootBase>

#include <QFileInfo>
#include <QDir>

TA_BASEFUNS_CTORS_DEFN(PluginWizard);

using namespace std;


void PluginWizard::Initialize() {
  auto_open = false;
  plugin_name = "myplugin";
  class_name_prefix = "Myplugin";
  plugin_type = UserPlugin;
  validated = false;
  created = false;
  editor = NULL;
  plugin_location = taMisc::user_dir + PATH_SEP + taMisc::GetUserPluginDir() +
    PATH_SEP + plugin_name;
  desc = "enter description of your plugin";
  uniqueId = "myplugin.dept.organization.org";
  url = "(replace this with a url for help or information on the plugin)";
}

void PluginWizard::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  validated = false;
  // modify it to force it to be C-valid
  plugin_name = taMisc::StringCVar(plugin_name);
  if(plugin_name.length() > 0) {
    // Just cap first letter -- capitalize() also downcases the rest of the string.
    class_name_prefix = plugin_name;
    class_name_prefix[0] = (char) toupper(class_name_prefix[0]);
  }
  if(plugin_name != "myplugin") {
    if(uniqueId.contains("myplugin")) uniqueId.gsub("myplugin", plugin_name);
    if(plugin_location.contains("myplugin")) plugin_location.gsub("myplugin", plugin_name);
  }
#ifdef TA_OS_WIN
  plugin_location.gsub("/", "\\");
#endif
  plugin_location = taMisc::NoFinalPathSep(plugin_location);
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

  if(CheckError(plugin_name.contains("_"), quiet, ok,
                "plugin name cannot contain an _ (underbar) character -- this is used for identifying different build types and cannot appear in the main plugin name -- I just removed it from the name -- please double check that it looks ok"))
    plugin_name.gsub("_", "");

  CheckError(plugin_name != "myplugin" && uniqueId.startsWith("myplugin."), quiet, ok,
             "the uniqueId cannot start with the default 'myplugin.' -- it MUST actually be unique!");
  CheckError(uniqueId.contains(".dept.organization."), quiet, ok,
             "the uniqueId cannot contain the initial '.dept.organization.' -- it MUST actually be unique -- please change to be your actual department and organization -- make one up if you need to!");

  QFileInfo qfi(plugin_location);
  CheckError((qfi.isDir()), quiet, ok,
             "plugin location already exists:", plugin_location,
             "you can move out of way or load wiz from that location to compile");

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
  if (TestError(!src.Load_str(fsrc),
    "PluginWizard::CreateDestFile",
    "Could not read contents of file:", src_file)) {
    ok = false;
    return;
  }
  fsrc.close();
  String dst;
  TemplatizeFile(src_file, src, dst, ok);
  if (!ok) return;
  if (TestError(!dst.Save_str(fdst),
    "PluginWizard::CreateDestFile",
    "Could not write results to:", dst_file,
    " -- make sure the file does not exist and/or is not write_protected and/or you have permission to create files in the destination")) {
    ok = false;
    return;
  }
  fdst.close();
}

bool PluginWizard::Create() {
  bool upgrade_only = false; //TODO: check for upgrade
  if (TestError((!validated),
    "PluginWizard::Create",
    "You must Validate the plugin before you can make it"))
    return false;
  // extract dirs
  src_dir = taMisc::app_dir + PATH_SEP + taMisc::GetSysPluginDir() + PATH_SEP +
    "template" + PATH_SEP;

  QFileInfo qfi(plugin_location);
  if(qfi.isDir()) {
    String wiz_file = plugin_location + PATH_SEP + "PluginWizard.wiz";
    QFileInfo qfiwiz(wiz_file);
    if(qfiwiz.isFile()) {
      int chs = taMisc::Choice("Plugin directory: " + plugin_location + " already exists and has previously-created Wizard data -- you can load that existing information (overwrites current configuration info in this wizard) or rename existing directory to _old and create a new blank plugin, or cancel", "Load Existing", "Rename Existing, Make New", "Cancel");
      if(chs == 2) return false;
      if(chs == 0) {
        LoadWiz(wiz_file);
        return false;
      }
      String justpath = taMisc::GetDirFmPath(plugin_location);
      String dirnm = taMisc::GetFileFmPath(plugin_location);
      QDir qdp(justpath);
      qdp.rename(dirnm, dirnm + "_old");
    }
    else {
      int chs = taMisc::Choice("Plugin directory: " + plugin_location + " already exists but does NOT have previously-created Wizard data -- you can rename existing directory to _old and create a new blank plugin, or cancel", "Rename Existing, Make New", "Cancel");
      if(chs == 1) return false;
      String justpath = taMisc::GetDirFmPath(plugin_location);
      String dirnm = taMisc::GetFileFmPath(plugin_location);
      QDir qdp(justpath);
      qdp.rename(dirnm, dirnm + "_old");
    }
  }

  // make the dest dir
  if (TestError(!taMisc::MakeDir(plugin_location),
    "PluginWizard::Create",
    "Could not make folder for plugin -- make sure the path is valid, and you have permission to create a folder in that location"))
    return false;
  // std build dirs
  if (TestError(!taMisc::MakeDir(plugin_location + PATH_SEP + "build"),
    "PluginWizard::Create",
    "Could not make 'build' subfolder for plugin -- make sure the path is valid, and you have permission to create a folder in that location"))
    return false;
  if (TestError(!taMisc::MakeDir(plugin_location + PATH_SEP + "build_dbg"),
    "PluginWizard::Create",
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
    // try linking first -- not!
//     if (!QFile::link(src_file, dst_file))
    ok = QFile::copy(src_file, dst_file);
    if(ok) {
      // copy permissions!
      QFile::Permissions sp = QFile::permissions(src_file);
      if(files[i] == "configure") {
        sp |= QFile::ExeOwner | QFile::ExeGroup | QFile::ExeOther;
      }
      QFile::setPermissions(dst_file, sp);
      // taMisc::Info("set permissions on:",dst_file,"to:",String(sp, "%x"));
    }
  }

  created = true;               // must come before saving!!

  SaveAs(plugin_location + PATH_SEP + "PluginWizard.wiz"); // save our settings!!

  if (ok) {
    taMisc::Info("The plugin was created successfully! See the CMakeLists.txt file in your plugin folder for build instructions");
  } else {
    taMisc::Error(
      "PluginWizard::Create",
      "Could not copy and templatize files for plugin -- make sure the path is valid, and you have permission to create files in that location");
    return false;
  }
  return true;
}

bool PluginWizard::Compile() {
  if (TestError((!created),
    "PluginWizard::Compile",
    "You must Create the plugin before you can compile it"))
    return false;

  return taPlugins::MakePlugin(plugin_location, plugin_name, (bool)plugin_type);
}

bool PluginWizard::Clean() {
  if (TestError((!created),
    "PluginWizard::Compile",
    "You must Create the plugin before you can clean it"))
    return false;

  if(plugin_type == SystemPlugin)
    taPlugins::CleanSystemPlugin(plugin_name);
  else
    taPlugins::CleanUserPlugin(plugin_name);
  return true;
}

bool PluginWizard::Editor() {
  if(editor) {
    editor->show();
    editor->raise();
    return true;
  }
  editor = iPluginEditor::New(plugin_location, plugin_name);
  return true;
}

bool PluginWizard::LoadWiz(const String& wiz_file) {
  bool rval = Load(wiz_file);
  if(rval) {
    if(wiz_file.firstchar() != '.') {
      plugin_location = taMisc::GetDirFmPath(wiz_file);
      taMisc::Info("set location to:", plugin_location);
    }
  }
  return rval;
}

bool PluginWizard::SaveWiz() {
  if (TestError((!created),
    "PluginWizard::SaveWiz",
    "You must Create the plugin before you can save the wizard file"))
    return false;
  String fname = plugin_location + PATH_SEP + "PluginWizard.wiz";
  SaveAs(fname);
  return true;
}

bool PluginWizard::MakeAllPlugins(bool user_only) {
  if(user_only) {
    taPlugins::MakeAllUserPlugins();
  }
  else {
    taPlugins::MakeAllPlugins();
  }
  return true;
}

bool PluginWizard::ShowWiz() {
  if(!taMisc::gui_active) return false;

  MainWindowViewer* vwr = (MainWindowViewer*)tabMisc::root->viewers.SafeEl(0);
  if(!vwr) return false;

  taiSigLink* link = (taiSigLink*)GetSigLink();
  if (!link) return false;

  iMainWindowViewer* imwv = vwr->widget();
  if(!imwv) return false;

  imwv->AssertBrowserItem(link);
  return true;
}

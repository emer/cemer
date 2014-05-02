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

#include "taPlugins.h"
#include <PluginMakeThreadMgr>
#include <taMisc>
#include <tabMisc>
#include <taRootBase>

#include <QFileInfo>
#include <QDateTime>
#include <QStringList>
#include <QDir>

using namespace std;


String_PArray taPlugins::plugin_folders;
taPluginInst_PList taPlugins::plugins;
String taPlugins::logfile;
int taPlugins::plugins_out_of_date = 0;

PluginMakeThreadMgr* taPlugins::make_thread = NULL;

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
    else      taMisc::DebugInfo(entry);
    if (warn)
      ofs << "**WARNING: ";
    ofs << entry << "\n";
    ofs.close();
  }
}

taPluginInst* taPlugins::ProbePlugin(const String& fileName) {
  taPluginInst* rval = new taPluginInst(fileName);
  String log_entry = "Attempting to probe plugin: " + fileName;
  AppendLogEntry(log_entry);

  QFileInfo fi(fileName);
  rval->mod_time_int = fi.lastModified().toTime_t();
  rval->mod_time = fi.lastModified().toString();
  if(rval->mod_time_int < taMisc::exe_mod_time_int) {
    String msg;
    msg << "===================================================================\n"
        << "A plugin is out of date vs. the executable, and must be recompiled\n"
        << "It will not be loaded.\n"
        << "plugin file: " << fileName << "\n"
        << "last modified:       " << rval->mod_time << "\n"
        << "executable last mod: " << taMisc::exe_mod_time << "\n"
        << "====================================================================\n";
    cerr << msg;
    AppendLogEntry(msg);
    rval->load_state = taPluginInst::LS_OUT_OF_DATE;
    plugins_out_of_date++;
  }
  else {
    // get the plugin object, and initialize types
    if (rval->load())  {
      rval->load_state = taPluginInst::LS_LOADED;
      log_entry = "Successfully probed plugin: " + fileName;
      AppendLogEntry(log_entry);
    }
    else {
      rval->load_state = taPluginInst::LS_LOAD_FAIL;
#if QT_VERSION >= 0x040200
      log_entry = "Could not probe: " + fileName
        + String("; Qt error msg: ").cat(rval->errorString().toLatin1().data());
#else
      log_entry = "Could not probe: " + fileName;
#endif
      AppendLogEntry(log_entry, true);
    }
  }
  return rval;
}

String taPlugins::PlatformPluginExt() {
  String filt;
#ifdef TA_OS_LINUX
  filt = ".so";
#elif defined(TA_OS_WIN)
  filt = ".dll";
#elif  defined(TA_OS_MAC)
  filt = ".dylib";
#else // huh??? what else?
  filt = ".*";
#endif
  return filt;
}

bool taPlugins::EnumeratePlugins() {
  String folder;
  for (int i = 0; i < plugin_folders.size; ++i) {
    folder = plugin_folders.FastEl(i);
    QDir pluginsDir(folder);
    // enumerate files in the folder, filter by build string (if any), then try loading
    // for platforms with known dylib suffixes, only look for those
    QString filt("*");
    if (taMisc::app_suffix.nonempty()) {
      filt.append(taMisc::app_suffix.chars());
    }
    filt += PlatformPluginExt().chars();
    QStringList fltrs;
    fltrs.append(filt);

    pluginsDir.setNameFilters(fltrs);
    foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {
      // if this is the default release build, then reject any others
      if (taMisc::app_suffix.empty()) {
        if(fileName.contains("_")) continue;
      }
      QString full_path = pluginsDir.absoluteFilePath(fileName);
      taPluginInst* pl = ProbePlugin(full_path);
      if (pl) {
        plugins.Add(pl);
      }
    }
  }
  return true;
}

void taPlugins::MakeAllPlugins() {
  MakeAllUserPlugins();
  MakeAllSystemPlugins();
}

void taPlugins::MakeAllOutOfDatePlugins() {
  taMisc::Info("=========================================================================");
  taMisc::Info("Making All Out Of Date Plugins");

  for(int i=0; i< plugins.size; i++) {
    taPluginInst* pli = plugins.FastEl(i);
    if(pli->load_state != taPluginInst::LS_OUT_OF_DATE) continue;
    String fnm = pli->fileName();
    if(fnm.contains(taMisc::app_plugin_dir)) {
      MakeSystemPlugin(pli->name);
    }
    else {
      MakeUserPlugin(pli->name);
    }
  }
}

void taPlugins::MakeAllUserPlugins() {
  taMisc::Info("=========================================================================");
  taMisc::Info("Making All UserPlugins");
  taMisc::Info("=========================================================================");
  QDir pluginsDir(taMisc::user_plugin_dir);
  QString filt("*");
  if (taMisc::app_suffix.nonempty()) {
    filt.append(taMisc::app_suffix.chars());
  }
  filt += PlatformPluginExt().chars();
  QStringList fltrs;
  fltrs.append(filt);

  pluginsDir.setNameFilters(fltrs);
  foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {
    // if this is the default release build, then reject any others
    if (taMisc::app_suffix.empty()) {
      if (fileName.contains("_")) continue;
    }
    String plugin_nm_full = fileName;
    String plugin_nm = taPluginInst::PluginNameFmFileName(fileName);
    String plug_path = taMisc::user_plugin_dir + PATH_SEP + plugin_nm;
    QFileInfo qfi(plug_path);
    if(!qfi.isDir()) {
      taMisc::Info("MakeAllUserPlugins -- plugin directory named:", plug_path, "does not exist for plugin file named:", plugin_nm_full, "cannot build plugin");
      continue;
    }
    MakePlugin(plug_path, plugin_nm, false); // no system
  }
}

void taPlugins::MakeAllSystemPlugins() {
  taMisc::Info("=========================================================================");
  taMisc::Info("Making All System Plugins");
  taMisc::Info("=========================================================================");
  QDir pluginsDir(taMisc::app_plugin_dir);
  QString filt("*");
  if (taMisc::app_suffix.nonempty()) {
    filt.append(taMisc::app_suffix.chars());
  }
  filt += PlatformPluginExt().chars();
  QStringList fltrs;
  fltrs.append(filt);

  pluginsDir.setNameFilters(fltrs);
  foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {
    // if this is the default release build, then reject any others
    if (taMisc::app_suffix.empty()) {
      if (fileName.contains("_")) continue;
    }
    String plugin_nm_full = fileName;
    String plugin_nm = taPluginInst::PluginNameFmFileName(fileName);

    String plug_path = taMisc::app_plugin_dir + PATH_SEP + plugin_nm;
    QFileInfo qfi(plug_path);
    if(!qfi.isDir()) {
      taMisc::Info("MakeAllSystemPlugins -- plugin directory named:", plug_path, "does not exist for plugin file named:", plugin_nm_full, "cannot build plugin");
      continue;
    }

    MakePlugin(plug_path, plugin_nm, true); // system
  }
}

bool taPlugins::MakeUserPlugin(const String& plugin_name) {
  String plug_path = taMisc::user_plugin_dir + PATH_SEP + plugin_name;
  QFileInfo qfi(plug_path);
  if(!qfi.isDir()) {
    taMisc::Info("MakeUserPlugin -- plugin directory named:", plug_path, "does not exist -- cannot build plugin");
    return false;
  }
  return MakePlugin(plug_path, plugin_name, false); // no system
}

bool taPlugins::MakeSystemPlugin(const String& plugin_name) {
  String plug_path = taMisc::app_plugin_dir + PATH_SEP + plugin_name;
  QFileInfo qfi(plug_path);
  if(!qfi.isDir()) {
    taMisc::Info("MakeSystemPlugin -- plugin directory named:", plug_path, "does not exist -- cannot build plugin");
    return false;
  }
  return MakePlugin(plug_path, plugin_name, true); // system
}

// turns out that good ol' system() call works better in mac and windows..

// class InteractiveProcess : public QProcess {
//   static int stdinClone;
// public:
//   InteractiveProcess(QObject *parent = 0) : QProcess(parent) {
//     if (stdinClone == -1)
//       stdinClone = ::dup(fileno(stdin));
//   }
// protected:
//   void setupChildProcess() {
//     ::dup2(stdinClone, fileno(stdin));
//   }
// };

// int InteractiveProcess::stdinClone = -1;

bool taPlugins::ExecMakeCmd(const String& cmd, const String& working_dir) {
  cout << cmd << endl;
  QString curpath = QDir::currentPath();
  QDir::setCurrent(working_dir);
  int ret_code = system(cmd);
  QDir::setCurrent(curpath);    // restore previous
  return 0 == ret_code;

// #ifdef TA_OS_WIN
//   QProcess proc;
// #else
//   InteractiveProcess proc;
// #endif
//   proc.setWorkingDirectory(working_dir);
//   proc.start(cmd);
//   if(!proc.waitForStarted()) return false;
//   if(!proc.waitForFinished()) return false;
//   QByteArray result = proc.readAll();
//   taMisc::Info(result);
//   return true;
}

bool taPlugins::MakePlugin(const String& plugin_path, const String& plugin_name,
                           bool system_plugin, bool full_rebuild) {
  if(taMisc::gui_active) {
    if(!make_thread) {
      make_thread = new PluginMakeThreadMgr;
      taBase::Own(make_thread, tabMisc::root); // own by root..
    }
    else {
      if(make_thread->n_running > 0)
        make_thread->SyncThreads();     // sync now before running again..
    }
    make_thread->MakePlugin(plugin_path, plugin_name, system_plugin, full_rebuild);
  }
  else {
    // no gui = just do it now
    return MakePlugin_impl(plugin_path, plugin_name, system_plugin, full_rebuild);
  }
  return true;
}

bool taPlugins::MakePlugin_impl(const String& plugin_path, const String& plugin_name,
                           bool system_plugin, bool full_rebuild) {
  cout << "=========================================================================" << endl;
  cout << "Making Plugin: " << plugin_name << " in dir: " << plugin_path << endl;
  cout << "=========================================================================" << endl;

  String build_dir = "build";
  if(taMisc::app_suffix.nonempty())
    build_dir += taMisc::app_suffix;
  String build_path = plugin_path + PATH_SEP + build_dir;

  String sudo_cmd;

#ifndef TA_OS_WIN
  if(system_plugin)
    sudo_cmd = "sudo ";
#endif

  String make_cmd = sudo_cmd;
  String env_cmd;
#ifdef TA_OS_WIN
  // pass full environment setup into each make command..
  env_cmd = taMisc::plugin_make_env_cmd + " & ";
  make_cmd += env_cmd + "nmake";
#else
  make_cmd += "make";
#endif

  if(full_rebuild) {
#ifdef TA_OS_WIN
    // first nuke anything existing!
    String rmdir_cmd = "rmdir " + build_dir + " /s /q";
    if(!ExecMakeCmd(rmdir_cmd, plugin_path)) return false;

    QDir qdr(plugin_path);
    qdr.mkdir(build_dir);
#else
    String rmdir_cmd = sudo_cmd + "/bin/rm -rf " + build_dir;
    if(!ExecMakeCmd(rmdir_cmd, plugin_path)) return false;

    String mkdir_cmd = sudo_cmd + "/bin/mkdir " + build_dir;
    if(!ExecMakeCmd(mkdir_cmd, plugin_path)) return false;
#endif

    String cmake_cmd = env_cmd + sudo_cmd + "cmake ../ ";
    if(taMisc::build_str.nonempty()) {
      if(taMisc::build_str.contains("dbg")) {
        cmake_cmd += "-DCMAKE_BUILD_TYPE=Debug ";
      }
      else {
        cmake_cmd += "-DCMAKE_BUILD_TYPE=RelWithDebInfo ";
      }
      if(taMisc::build_str.contains("mpi")) {
        cmake_cmd += "-DMPI_BUILD=TRUE ";
      }
    }
    else {
      cmake_cmd += "-DCMAKE_BUILD_TYPE=RelWithDebInfo ";
    }

    String extra_suffix = taMisc::ExtraAppSuffix();
    if (extra_suffix.nonempty()) {
      cmake_cmd += "-DEXTRA_SUFFIX=" + extra_suffix + " ";
    }

#if (QT_VERSION >= 0x050000)
    cmake_cmd += "-DQT_USE_5=ON -DQTDIR=$QTDIR ";
#else
    cmake_cmd += "-DQT_USE_4=ON -DQTDIR=$QTDIR ";
#endif

#if defined(USE_SSE8)
    cmake_cmd += "-DCMAKE_CXX_FLAGS=-DUSE_SSE8 ";
#endif

    if(system_plugin)
      cmake_cmd += "-DEMERGENT_PLUGIN_TYPE=System ";
#ifdef TA_OS_WIN
    cmake_cmd += "-G \"NMake Makefiles\"";
#endif

    if(!ExecMakeCmd(cmake_cmd, build_path)) return false;

    // always start off with a make clean to ensure everything is rebuilt
    String make_clean = make_cmd + " clean";
    if(!ExecMakeCmd(make_clean, build_path)) return false;
  }

  // straight make
  if(!ExecMakeCmd(make_cmd, build_path)) return false;

  // make install
  String make_install = make_cmd + " install";
  if(!ExecMakeCmd(make_install, build_path)) return false;

  return true;
}

//////////////////////////////////////////////////
//      Clean Plugins

void taPlugins::CleanAllPlugins() {
  CleanAllUserPlugins();
  CleanAllSystemPlugins();
}

void taPlugins::CleanAllOutOfDatePlugins() {
  taMisc::Info("=========================================================================");
  taMisc::Info("Cleaning All Out Of Date Plugins");

  for(int i=0; i< plugins.size; i++) {
    taPluginInst* pli = plugins.FastEl(i);
    if(pli->load_state != taPluginInst::LS_OUT_OF_DATE) continue;
    String fnm = pli->fileName();
    if(fnm.contains(taMisc::app_plugin_dir)) {
      CleanPlugin(taMisc::app_plugin_dir, fnm, true);
    }
    else {
      CleanPlugin(taMisc::user_plugin_dir, fnm, false);
    }
  }
}

void taPlugins::CleanAllUserPlugins() {
  taMisc::Info("=========================================================================");
  taMisc::Info("Cleaning All User Plugins");
  taMisc::Info("=========================================================================");
  QDir pluginsDir(taMisc::user_plugin_dir);
  QString filt("*");
  if (taMisc::app_suffix.nonempty()) {
    filt.append(taMisc::app_suffix.chars());
  }
  filt += PlatformPluginExt().chars();
  QStringList fltrs;
  fltrs.append(filt);

  pluginsDir.setNameFilters(fltrs);
  foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {
    // if this is the default release build, then reject any others
    if (taMisc::app_suffix.empty()) {
      if (fileName.contains("_")) continue;
    }
    String plugin_nm_full = fileName;
    CleanPlugin(taMisc::user_plugin_dir, plugin_nm_full, false); // no system
  }
}

void taPlugins::CleanAllSystemPlugins() {
  taMisc::Info("=========================================================================");
  taMisc::Info("Cleaning All System Plugins");
  taMisc::Info("=========================================================================");
  QDir pluginsDir(taMisc::app_plugin_dir);
  QString filt("*");
  if (taMisc::app_suffix.nonempty()) {
    filt.append(taMisc::app_suffix.chars());
  }
  filt += PlatformPluginExt().chars();
  QStringList fltrs;
  fltrs.append(filt);

  pluginsDir.setNameFilters(fltrs);
  foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {
    // if this is the default release build, then reject any others
    if (taMisc::app_suffix.empty()) {
      if (fileName.contains("_")) continue;
    }
    String plugin_nm_full = fileName;
    CleanPlugin(taMisc::app_plugin_dir, plugin_nm_full, false); // no system
  }
}

bool taPlugins::CleanUserPlugin(const String& plugin_name) {
  String plugin_nm_full =  "lib" + plugin_name;
  if(taMisc::app_suffix.nonempty()) {
    plugin_nm_full += taMisc::app_suffix;
  }
  plugin_nm_full += PlatformPluginExt();
  return CleanPlugin(taMisc::user_plugin_dir, plugin_nm_full, false); // no system
}

bool taPlugins::CleanSystemPlugin(const String& plugin_name) {
  String plugin_nm_full =  "lib" + plugin_name;
  if(taMisc::app_suffix.nonempty()) {
    plugin_nm_full += taMisc::app_suffix;
  }
  plugin_nm_full += PlatformPluginExt();
  return CleanPlugin(taMisc::app_plugin_dir, plugin_nm_full, true); // system
}

bool taPlugins::CleanPlugin(const String& plugin_path, const String& plugin_name,
                           bool system_plugin) {
  taMisc::Info("=========================================================================");
  taMisc::Info("Cleaning Plugin:", plugin_name, "in dir:", plugin_path);
  taMisc::Info("=========================================================================");

  String sudo_cmd;
#ifndef TA_OS_WIN
  if(system_plugin)
    sudo_cmd = "sudo ";
#endif

  String rm_cmd = sudo_cmd + "rm " + plugin_name;
  if(!ExecMakeCmd(rm_cmd, plugin_path)) return false;
  return true;
}


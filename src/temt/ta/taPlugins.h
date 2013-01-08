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

#ifndef taPlugins_h
#define taPlugins_h 1

// parent includes:

// member includes:
#include <String_PArray>
#include <taPluginInst_PList>

// declare all other types mentioned but not required to include:
class PluginMakeThreadMgr; // 


class TA_API taPlugins { // #NO_INSTANCE global object to manage plugins
public:
  static String_PArray	plugin_folders;
  // folders to search for plugins (taMisc::user_plugin_dir, app_plugin_dir)
  static taPluginInst_PList	plugins;
  // plugins that have been loaded -- they remain for the lifetime of program
  static String		logfile;
  static PluginMakeThreadMgr* make_thread;
  // manages making of threads
  static int		plugins_out_of_date;
  // number of plugins that were out of date -- can trigger automatic recompile
  
  static void		AddPluginFolder(const String& folder);
  // adds a folder, note: ignores duplicates
  static void		InitLog(const String& logfile);
  // clear the log file
  static void		AppendLogEntry(const String& entry, bool warn = false);
  // append entry, with nl, highlighting if a warning

  static String		PlatformPluginExt();
  // get the plugin (dynamic library) filename extension for this platform (.so, .dll, dylib)

  static bool		EnumeratePlugins();
  // enumerates, and lo-level loads -- returns false if any files are out of date vs. the executable and need to be rebuilt

  static void		MakeAllPlugins();
  // make all plugins in user and system directories
  static void		MakeAllOutOfDatePlugins();
  // make all out of date plugins
  static void		MakeAllUserPlugins();
  // make all plugins in user directory
  static void		MakeAllSystemPlugins();
  // make all plugins in system directory
  static bool		MakeUserPlugin(const String& plugin_name);
  // make specified plugin in user directory
  static bool		MakeSystemPlugin(const String& plugin_name);
  // make specified plugin in user directory

  static bool		ExecMakeCmd(const String& cmd, const String& working_dir);
  // execute given system command in given working directory, also echoing it to the screen and its results

  static bool		MakePlugin(const String& plugin_path, const String& plugin_name,
				   bool system_plugin = false, bool full_rebuild=true);
  // make specified plugin in given full path to plugin source (plugin name only used for info purposes) -- basic interface for making plugins -- if system_plugin, then make command is prefixed with "sudo" and plugin type is set as System (else defaults to user) -- full_rebuild does a full cmake and make clean -- otherwise, just does a basic make
  static bool		MakePlugin_impl(const String& plugin_path, const String& plugin_name,
					bool system_plugin = false, bool full_rebuild=true);
  // actually does the job -- above calls thread to do it

  static void		CleanAllPlugins();
  // clean all plugins in user and system directories
  static void		CleanAllOutOfDatePlugins();
  // clean all out of date plugins
  static void		CleanAllUserPlugins();
  // clean all plugins in user directory
  static void		CleanAllSystemPlugins();
  // clean all plugins in system directory
  static bool		CleanUserPlugin(const String& plugin_name);
  // clean specified plugin in user directory
  static bool		CleanSystemPlugin(const String& plugin_name);
  // clean specified plugin in user directory

  static bool		CleanPlugin(const String& plugin_path, const String& plugin_name,
				   bool system_plugin = false);
  // clean specified plugin in given full path to plugin directory (user or app plugin_dir) -- plugin_name is the full filename of the plugin

protected:
  
  static taPluginInst*	ProbePlugin(const String& fileName); 
    // try loading the plugin, returns the loader object if successful
};

#endif // taPlugins_h

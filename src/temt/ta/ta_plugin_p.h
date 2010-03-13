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


// ta_plugin_p.h: private tacss implementation for plugins (not included in plugins)

#ifndef TA_PLUGIN_P_H
#define TA_PLUGIN_P_H

#include "ta_plugin.h"
#include "ta_base.h"

class iPluginEditor; //

// forwards
class taPlugin;
class taPlugin_List;//

class TA_API taPluginInst: public QPluginLoader { // ##NO_INSTANCE an instance of a plugin
  Q_OBJECT
INHERITED(QPluginLoader)
public:
  enum LoadState {
    LS_INIT_FAIL	= -3, // failure trying to init plugin
    LS_TYPE_FAIL	= -2, // failure trying to init types
    LS_LOAD_FAIL	= -1, // could not be loaded (prob needs to be recompiled)
    LS_NOT_LOADED	=  0, // true if not yet loaded, or unloaded
    LS_LOADED		    , // true once lo-level loaded, for enumeration
    LS_TYPE_INIT	    , // true once the type initialization done (can no longer unload)
    LS_PLUG_INIT	      // true once initialized
  };
  
  taPlugin* 		plugin_rep; // nulled if pl deletes
  LoadState		load_state; // true once probed, for enumeration
    
  IPlugin*		plugin(); // access to the plugin object -- note: should be valid, because we don't register failed probes
  
  bool			InitTypes(); // done first, and only if enabled; true if succeed
  bool			InitPlugin();  // done last, and only if enabled
  
  taPluginInst(const String& fileName);
};


class TA_API taPluginInst_PList: public taPtrList<taPluginInst> { // #NO_INSTANCE
INHERITED(taPtrList<taPluginInst>)
public:
  taPluginInst_PList() {}
  ~taPluginInst_PList() {Reset();}
protected:
  override void		El_Done_(void* it) { delete ((taPluginInst*)it);}
};


class TA_API taPlugins { // #NO_INSTANCE global object to manage plugins
public:
  static String_PArray	plugin_folders;
  // folders to search for plugins (taMisc::user_plugin_dir, app_plugin_dir)
  static taPluginInst_PList	plugins;
  // plugins that have been loaded -- they remain for the lifetime of program
  static String		logfile;
  
  static void		AddPluginFolder(const String& folder);
  // adds a folder, note: ignores duplicates
  static void		InitLog(const String& logfile);
  // clear the log file
  static void		AppendLogEntry(const String& entry, bool warn = false);
  // append entry, with nl, highlighting if a warning

  static String		PlatformPluginExt();
  // get the plugin (dynamic library) filename extension for this platform (.so, .dll, dylib)

  static void		EnumeratePlugins(); // enumerates, and lo-level loads

  static void		MakeAllPlugins();
  // make all plugins in user and system directories
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
				   bool system_plugin = false);
  // make specified plugin in given full path to plugin source (plugin name only used for info purposes) -- basic interface for making plugins -- if system_plugin, then make command is prefixed with "sudo" and plugin type is set as System (else defaults to user)


  static void		CleanAllPlugins();
  // clean all plugins in user and system directories
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

class TA_API taPluginBase: public taOBase {
  // #VIRT_BASE ##NO_TOKENS ##NO_UPDATE_AFTER taBase rep of a plugin -- these can be out of date w/ respect to actual plugins
INHERITED(taOBase)
public:
  String		name; // #READ_ONLY #SHOW  the plugin name, provided by the plugin 
  String		desc; // #READ_ONLY #SHOW the plugin description, provided by the plugin
  String		unique_id; // #READ_ONLY #SHOW a unique string to identify the plugin
  String		version; // #AKA_plugin_version #READ_ONLY #SHOW  the plugin's version (as of when plugin was loaded)
  String		url; // #READ_ONLY #SHOW a url that provides information on the plugin; used mostly for when missing in a proj file

  String        GetName() const { return name; } // note: user can't set name
  void          SetDefaultName() {} 
  String	GetDesc() const {return desc;}
  void		Copy_(const taPluginBase& cp); //note: we only use this for descs, not actual plugins
  TA_ABSTRACT_BASEFUNS(taPluginBase);
private:
  void	Initialize();
  void	Destroy() {}
};


class TA_API taPlugin: public taPluginBase {
  // taBase rep of a plugin -- these can be out of date w/ respect to actual plugins
INHERITED(taPluginBase)
public:
  String		filename; // #READ_ONLY #SHOW #FILE_DIALOG_LOAD the plugin's filename
  bool			enabled; // set if this plugin should be loaded when the app starts
  bool			loaded; // / #READ_ONLY #SHOW #NO_SAVE set if the plugin is loaded and initialized
  bool			reconciled; // #IGNORE true once reconciled; we delete those with no plugin
  
  taPluginInst*		plugin; // #IGNORE the plugin, if loaded (not used for descs)
  String		state_classname; // #READ_ONLY #SHOW #NO_SAVE the name of the the cached state type, if any -- is based on the plugin name, and must inherit taFBase
  TypeDef*		state_type; // #READ_ONLY #SHOW #NO_SAVE cached state type, if any -- is based on the plugin name, and must inherit taFBase
  
  bool			InitPlugin();
  // #IGNORE initializes the plugin, including making/loading state object if any -- assumes it has been reconciled

  virtual void		PluginOptions();
  // #BUTTON open the Options dialog for this plugin (if it has one)

  virtual bool		Compile();
  // #BUTTON compile the plugin from the existing source code -- does a make and a make install
  virtual bool		Clean();
  // #BUTTON remove (clean) the plugin -- prevents it from being loaded
  virtual bool		LoadWiz();
  // #BUTTON load an existing wizard configuration file saved from a prior wizard create step
  virtual bool		Editor();
  // #BUTTON open the plugin file editor to edit plugin source files -- does LoadWiz() and then calls Editor on that, to get back to the original source files and not the installed source files

  virtual void		ParseFileName(String& base_path, String& plugin_nm_full,
				      String& plugin_nm);
  // parse the filename to extract the base path, and full plugin library file name, plus the short plugin name that should correspond to the source directory name

  int	GetEnabled() const {return enabled && loaded;}
  void	SetEnabled(bool value) {enabled = value;}

  void	Copy_(const taPlugin& cp); //note: we only use this for descs, not actual plugins
  TA_BASEFUNS(taPlugin);
protected:
#ifndef __MAKETA__
  QPointer<iPluginEditor> editor;
#endif

private:
  void	Initialize();
  void	Destroy() {}
};


class TA_API taPluginDep: public taPluginBase {
  // describes a plugin dependency -- appears in root, and streamed to files
INHERITED(taPluginBase)
public:
  enum DepCheck {
    DC_OK,		// a-ok
    DC_MISSING, 	// required guy is missing
    DC_NOT_LOADED	// guy is listed, but not loaded
  };
  
  DepCheck		dep_check; // #READ_ONLY #SHOW #NO_SAVE set if plugin_dep is missing in plugins
  
  TA_BASEFUNS(taPluginDep); //
protected:
  override void CheckThisConfig_impl(bool quiet, bool& rval); // only for _deps
private:
  void	Initialize();
  void	Destroy() {}
  void	Copy_(const taPluginDep& cp); //note: we only use this for descs, not actual plugins
};


class TA_API taPluginBase_List: public taList<taPluginBase> {
  // ##CHILDREN_INLINE ##NO_UPDATE_AFTER ##NO_TOKENS plugins available to the program (also used for deps)
INHERITED(taList<taPluginBase>)
public:
  taPluginBase*		FindUniqueId(const String& value); // find by unique_id
  
  TA_BASEFUNS_NOCOPY(taPluginBase_List);
  
protected:
// forbid most clip ops, since we are managed based on existing plugins
  override void		QueryEditActions_impl(const taiMimeSource* ms,
    int& allowed, int& forbidden);
  override void		ChildQueryEditActions_impl(const MemberDef* md, const taBase* child,
    const taiMimeSource* ms, int& allowed, int& forbidden);

private:
  void	Initialize() {}
  void	Destroy() {}
};

class TA_API taPlugin_List: public taPluginBase_List {
  // plugins available to the program (also used for descs)
INHERITED(taPluginBase_List)
public:
  inline taPlugin*	FastEl(int i) {return (taPlugin*)inherited::FastEl(i);}
  taPlugin*		FindFilename(const String& value);
  inline taPlugin*	FindUniqueId(const String& value)
    {return (taPlugin*)inherited::FindUniqueId(value);}
  
  void			LoadPlugins(); // Load all the enabled plugins (init's type system), unload remainder
  void			InitPlugins(); // Initialize all the loaded plugins -- creates/loads state object first; then plugin can post Wizards, etc.
  
  void			ViewPluginLog(); // #MENU_CONTEXT #BUTTON view the most recent plugin log
  TA_BASEFUNS_NOCOPY(taPlugin_List);
  
protected:
  void		ReconcilePlugins(); // reconciles our list with list of plugins
private:
  void	Initialize() {SetBaseType(&TA_taPlugin);} // usually upclassed
  void	Destroy() {}
};

#endif

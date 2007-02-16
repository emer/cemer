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


// ta_plugin_p.h: private tacss implementation for plugins (not included in plugins)

#ifndef TA_PLUGIN_P_H
#define TA_PLUGIN_P_H

#include "ta_plugin.h"
#include "ta_base.h"


// forwards
class taPlugin;
class taPlugin_List;

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
  static String_PArray	plugin_folders; // folders to search for plugins
  static taPluginInst_PList	plugins; // plugins that have been loaded -- they remain for the lifetime of program
  static String		logfile;
  
  static void		AddPluginFolder(const String& folder); // adds a folder, note: ignores duplicates
  static void		InitLog(const String& logfile); // clear the log file
  static void		AppendLogEntry(const String& entry, bool warn = false); // append entry, with nl, highlighting if a warning
  static void		EnumeratePlugins(); // enumerates, and lo-level loads
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
  String		version; // #READ_ONLY #SHOW  the plugin's version (as of when plugin was loaded)
  String		url; // #READ_ONLY #SHOW a url that provides information on the plugin; used mostly for when missing in a proj file
  
  String        GetName() const { return name; } // note: user can't set name
  void          SetDefaultName() {} 
  String	GetDesc() const {return desc;}
  void		Copy_(const taPluginBase& cp); //note: we only use this for descs, not actual plugins
  COPY_FUNS(taPluginBase, taOBase);
  TA_ABSTRACT_BASEFUNS(taPluginBase);
private:
  void	Initialize();
  void	Destroy() {}
};


class TA_API taPlugin: public taPluginBase {
  // taBase rep of a plugin -- these can be out of date w/ respect to actual plugins
INHERITED(taPluginBase)
public:
  String		filename; // #READ_ONLY #SHOW the plugin's filename
  bool			enabled; // set if this plugin should be loaded when the app starts
  bool			loaded; // / #READ_ONLY #SHOW #NO_SAVE set if the plugin is loaded and initialized
  bool			reconciled; // #IGNORE true once reconciled; we delete those with no plugin
  taPluginInst*		plugin; // #IGNORE the plugin, if loaded (not used for descs)
  
  int	GetEnabled() const {return enabled;}
  void	SetEnabled(bool value) {enabled = value;}
  void	Copy_(const taPlugin& cp); //note: we only use this for descs, not actual plugins
  COPY_FUNS(taPlugin, taPluginBase);
  TA_BASEFUNS(taPlugin);
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
  
  void		Copy_(const taPluginDep& cp); //note: we only use this for descs, not actual plugins
  COPY_FUNS(taPluginDep, taPluginBase);
  TA_BASEFUNS(taPluginDep);
#ifndef __MAKETA__
  using inherited::Copy;
#endif
protected:
  override void CheckThisConfig_impl(bool quiet, bool& rval); // only for _deps
private:
  void	Initialize();
  void	Destroy() {}
};


class TA_API taPluginBase_List: public taList<taPluginBase> {
  // ##CHILDREN_INLINE ##NO_UPDATE_AFTER plugins available to the program (also used for deps)
INHERITED(taList<taPluginBase>)
public:
  taPluginBase*		FindUniqueId(const String& value); // find by unique_id
  
  TA_BASEFUNS(taPluginBase_List);
  
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
  
  void		LoadPlugins(); // Load and initialize all the enabled plugins, unload remainder
  
  void			ViewPluginLog(); // #MENU_CONTEXT #BUTTON view the most recent plugin log
  TA_BASEFUNS(taPlugin_List);
  
protected:
  void		ReconcilePlugins(); // reconciles our list with list of plugins
private:
  void	Initialize() {SetBaseType(&TA_taPlugin);} // usually upclassed
  void	Destroy() {}
};

#endif

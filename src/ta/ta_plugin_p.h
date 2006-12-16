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

class taPluginInst: public QPluginLoader { // ##NO_INSTANCE an instance of a plugin
  Q_OBJECT
INHERITED(QPluginLoader)
public:
  enum LoadState {
    LS_INIT_FAIL	= -2, // failure trying to init plugin
    LS_TYPE_FAIL	= -1, // failure trying to init types
    LS_NOT_LOADED	=  0, // true if not yet loaded, or unloaded
    LS_LOADED		    , // true once lo-level loaded, for enumeration
    LS_TYPE_INIT	    , // true once the type initialization done (can no longer unload)
    LS_PLUG_INIT	      // true once initialized
  };
  
  LoadState		load_state; // true once lo-loaded, for enumeration
    
  IPlugin*		plugin(); // access to the plugin object -- note: should be valid, because we don't register failed loads
  
  bool			InitTypes(); // done first, and only if enabled; true if succeed
  bool			InitPlugin();  // done last, and only if enabled
  
  taPluginInst(const String& fileName);
};


class taPluginInst_PList: public taPtrList<taPluginInst> { // #NO_INSTANCE
INHERITED(taPtrList<taPluginInst>)
public:
  taPluginInst_PList() {}
  ~taPluginInst_PList() {Reset();}
protected:
  override void		El_Done_(void* it) { delete ((taPluginInst*)it);}
};


class taPlugins { // #NO_INSTANCE global object to manage plugins
public:
  static String_PArray	plugin_folders; // folders to search for plugins
  static taPluginInst_PList	plugins; // plugins that have been loaded -- they remain for the lifetime of program
  
  static void		AddPluginFolder(const String& folder); // adds a folder, note: ignores duplicates
  static void		EnumeratePlugins(); // enumerates, and lo-level loads
protected:
  static taPluginInst*	LoadPlugin(const String& fileName); 
    // try loading the plugin, returns the loader object if successful
};

class taPlugin: public taOBase {
  // #NO_TOKENS taBase rep of a plugin -- these can be out of date w/ respect to actual plugins
INHERITED(taOBase)
public:
  bool			enabled; // set if this plugin should be loaded when the app starts
  bool			loaded; // / #READ_ONLY #SHOW #NO_SAVE set if the plugin is loaded and initialized
  bool			reconciled; // #IGNORE true once reconciled; we delete those with no plugin
  String		name; // #READ_ONLY #SHOW #NO_SAVE the plugin name, provided by the plugin 
  String		desc; // #READ_ONLY #SHOW #NO_SAVE the plugin description, provided by the plugin
  String		unique_id; // #READ_ONLY #SHOW a unique string to identify the plugin
  String		version; // #READ_ONLY #SHOW #NO_SAVE the plugin's version
  String		filename; // #READ_ONLY #SHOW #NO_SAVE the plugin's filename
  
  taPluginInst*		plugin;
  
  String        GetName() const { return unique_id; } // NOTE: unique_id is the canonical name for this guy
  void          SetDefaultName() {} 
  String	GetDesc() const {return desc;}
  //note: copying doesn't make sense, so we forbid
  TA_BASEFUNS(taPlugin);
private:
  void	Initialize();
  void	Destroy() {}
};


class taPlugin_List: public taList<taPlugin> {
  // #CHILDREN_INLINE plugins available to the program
INHERITED(taList<taPlugin>)
public:
  void		LoadPlugins(); // Load and initialize all the enabled plugins, unload remainder
  
  TA_BASEFUNS(taPlugin_List);
  
protected:
  void		ReconcilePlugins(); // reconciles our list with list of plugins
// forbid most clip ops, since we are managed based on existing plugins
  override void		QueryEditActions_impl(const taiMimeSource* ms,
    int& allowed, int& forbidden);
  override void		ChildQueryEditActions_impl(const MemberDef* md, const taBase* child,
    const taiMimeSource* ms, int& allowed, int& forbidden);

private:
  void	Initialize() {SetBaseType(&TA_taPlugin);}
  void	Destroy() {}
};

#endif

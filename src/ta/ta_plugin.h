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


// ta_plugin.h: defines architecture for plugins
// NOTE: this is only include in TA_GUI builds

#ifndef TA_PLUGIN_H
#define TA_PLUGIN_H

#include "ta_stdef.h"
#include "ta_type.h"

#ifndef __MAKETA__
# include <QPluginLoader>
#endif


class IPlugin  { // #VIRT_BASE basic interface for a ta plugin; int ret codes use 0=success, !0=errcode
public:
  virtual ~IPlugin() {}

  virtual int		InitializeTypes() const = 0;
    // called when loading plugin to initialize types -- implementer must call ta_init_Xxx() routine
  virtual int		InitializePlugin() = 0;
    // called to initialize plugin -- it can do things like create classes
};

#ifndef __MAKETA__
Q_DECLARE_INTERFACE(IPlugin, "pdp.IPlugin/1.0")
#endif


class taPlugin: public QPluginLoader { // ##NO_INSTANCE
  Q_OBJECT
INHERITED(QPluginLoader)
public:
  IPlugin*		plugin(); // access to the plugin object -- note: should be valid, because we don't register failed loads
  
  taPlugin(const String& fileName);
};


class taPlugin_PList: public taPtrList<taPlugin> { // #NO_INSTANCE
INHERITED(taPtrList<taPlugin>)
public:
  taPlugin_PList() {}
  ~taPlugin_PList() {Reset();}
protected:
  override void		El_Done_(void* it) { delete ((taPlugin*)it);}
};


class taPlugins { // #NO_INSTANCE global object to manage plugins
public:
  static String_PArray	plugin_folders; // folders to search for plugins
  static taPlugin_PList	plugins; // plugins that have been loaded -- they remain for the lifetime of program
  
  static void		AddPluginFolder(const String& folder); // adds a folder, note: ignores duplicates
  static void		LoadPlugins(); // finds and loads all the plugins
  static void		InitPlugins(); // Initializes all the loaded plugins
protected:
  static taPlugin*	LoadPlugin(const String& fileName); 
    // try loading the plugin, returns the loader object if successful
};

#endif

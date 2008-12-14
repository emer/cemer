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


// ta_plugin.h: defines architecture for plugins, included by tacss and plugins

#ifndef TA_PLUGIN_H
#define TA_PLUGIN_H

#include "ta_stdef.h"
#include "ta_type.h"

#ifndef __MAKETA__
# include <QObject> // for clients of this file, since they will inherit an obj from QObject
# include <QPluginLoader>
#endif


class IPlugin: public virtual ITypedObject  { // #VIRT_BASE basic interface for a ta plugin; int ret codes use 0=success, !0=errcode
public:
// the following interfaces are used in the plugin enumeration stage
  virtual const char*	desc() {return "(no description provided)";}
  virtual const char*	name() {return "pluginname";}
  virtual const char*	uniqueId() {return "pluginname.dept.organization.org";}
  virtual const char*	url() {return "put a full url here, but put this in the .cpp file because of the maketa double slash bug";}
  
  virtual int		NotifyTacssVersion(const taVersion& tav, bool& is_ok) {return 0;}
    // we pass ta/css version; set is_ok false if this version is no good for plugin
  virtual int		GetVersion(taVersion& tav) {return -1;}
    // major.minor.step.build -- used to put version dependency stamp into project files

// the following routines are only used if the plugin is actually loaded into tacss
  virtual int		InitializeTypes() = 0;
    // called when loading plugin to initialize types -- implementer must call ta_init_Xxx() routine
  virtual int		InitializePlugin() = 0;
    // called to initialize plugin -- it can do things like create classes
  virtual TypeDef*	GetPluginStateType() {return NULL;}
    // returns the type of the state object, that will be created in root.plugin_state
    
  virtual ~IPlugin() {}
};

#ifndef __MAKETA__
Q_DECLARE_INTERFACE(IPlugin, "pdp.IPlugin/1.0")
#endif


#endif

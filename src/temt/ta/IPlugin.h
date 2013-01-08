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

#ifndef IPlugin_h
#define IPlugin_h 1

// parent includes:
#include <ITypedObject>

// member includes:

// declare all other types mentioned but not required to include:
class taVersion; // 


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
    // major.minor.step.build -- the plugin version

// the following routines are only used if the plugin is actually loaded into tacss
  virtual int		InitializeTypes() = 0;
    // called when loading plugin to initialize types -- implementer must call ta_init_Xxx() routine
  virtual int		InitializePlugin() = 0;
    // called to initialize plugin -- it can do things like create classes
    
  virtual ~IPlugin() {}
};

#endif // IPlugin_h

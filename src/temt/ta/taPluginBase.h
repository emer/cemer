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

#ifndef taPluginBase_h
#define taPluginBase_h 1

// parent includes:
#include <taOBase>

// member includes:

// declare all other types mentioned but not required to include:


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

#endif // taPluginBase_h

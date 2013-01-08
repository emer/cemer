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

#ifndef taPluginInst_h
#define taPluginInst_h 1

// parent includes:

// member includes:

// declare all other types mentioned but not required to include:
class taPlugin; // 
class IPlugin; // 


class TA_API taPluginInst: public QPluginLoader { // ##NO_INSTANCE an instance of a plugin
  Q_OBJECT
INHERITED(QPluginLoader)
public:
  enum LoadState {
    LS_OUT_OF_DATE	= -4, // library file is out of date -- needs to be recompiled
    LS_INIT_FAIL	= -3, // failure trying to init plugin
    LS_TYPE_FAIL	= -2, // failure trying to init types
    LS_LOAD_FAIL	= -1, // could not be loaded (prob needs to be recompiled)
    LS_NOT_LOADED	=  0, // true if not yet loaded, or unloaded
    LS_LOADED		    , // true once lo-level loaded, for enumeration
    LS_TYPE_INIT	    , // true once the type initialization done (can no longer unload)
    LS_PLUG_INIT	      // true once initialized
  };
  
  String		name;	    // derived from filename
  taPlugin* 		plugin_rep; // nulled if pl deletes
  LoadState		load_state; // true once probed, for enumeration
  String		mod_time; // #READ_ONLY #NO_SAVE #SHOW date and time when the library plugin file was last modified (installed)
  int64_t		mod_time_int; // #READ_ONLY #NO_SAVE #NO_SHOW time stamp for library file last modification date (internal seconds since jan 1 1970 time units) -- this is used as a trigger for determining when to rebuild
    
  IPlugin*		plugin(); // access to the plugin object -- note: should be valid, because we don't register failed probes
  
  bool			InitTypes(); // done first, and only if enabled; true if succeed
  bool			InitPlugin();  // done last, and only if enabled

  static String		PluginNameFmFileName(const String& fname);
  // extract plugin name from file name
  
  taPluginInst(const String& fileName);
};

#endif // taPluginInst_h

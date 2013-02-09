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

#ifndef taPlugin_h
#define taPlugin_h 1

// parent includes:
#include <taPluginBase>

// member includes:
#ifndef __MAKETA__
#include <QPointer>
#endif
class iPluginEditor; // #IGNORE


// declare all other types mentioned but not required to include:
class TypeDef; // 
class taPluginInst; // #IGNORE


taTypeDef_Of(taPlugin);

class TA_API taPlugin: public taPluginBase {
  // taBase rep of a plugin -- these can be out of date w/ respect to actual plugins
INHERITED(taPluginBase)
public:
  String		filename; // #READ_ONLY #SHOW #FILE_DIALOG_LOAD the plugin's filename
  bool			enabled; // set if this plugin should be loaded when the app starts
  bool			loaded; // #DEF_true #READ_ONLY #SHOW #NO_SAVE set if the plugin is loaded and initialized
  bool			up_to_date; // #DEF_true #READ_ONLY #SHOW #NO_SAVE set if the plugin file is up-to-date relative to the executable -- if this is false then the plugin will not be loaded, and must be recompiled
  String		mod_time; // #READ_ONLY #NO_SAVE #SHOW date and time when the library plugin file was last modified (installed)
  int64_t		mod_time_int; // #READ_ONLY #NO_SAVE #NO_SHOW time stamp for library file last modification date (internal seconds since jan 1 1970 time units) -- this is used as a trigger for determining when to rebuild
  bool			reconciled; // #IGNORE true once reconciled; we delete those with no plugin
  
  taPluginInst*		plugin; // #IGNORE the plugin, if loaded (not used for descs)
  String		state_classname; // #READ_ONLY #SHOW #NO_SAVE the name of the the cached state type, if any -- is based on the plugin name, and must inherit taFBase
  TypeDef*		state_type; // #READ_ONLY #SHOW #NO_SAVE cached state type, if any -- is based on the plugin name, and must inherit taFBase
  
  bool			InitPlugin();
  // #IGNORE initializes the plugin, including making/loading state object if any -- assumes it has been reconciled

  virtual void		PluginOptions();
  // #BUTTON open the Options dialog for this plugin (if it has one)

  virtual bool		Compile();
  // #BUTTON #CONFIRM compile the plugin (make and make install) from the INSTALLED source code -- this is NOT going to reflect any changes you might have made to the source code in the actual plugin home source directory, which have not been compiled yet -- it is ONLY intended for freshening an existing install after a rebuild of the main emergent codebase -- please do LoadWiz and Compile from the PluginWizard to compile new source changes 
  virtual bool		Clean();
  // #BUTTON #CONFIRM remove (clean) the plugin, e.g., to prevent it from being loaded if it is causing problems -- this just removes the compiled library object file and does not remove the source code from the install directory, so it can be regenerated from that later if desired
  virtual bool		LoadWiz();
  // #BUTTON load an existing wizard configuration file saved from a prior wizard create step -- this then enables further interaction with the plugin code via the PluginWizard (editing, compiling etc) -- importantly, the PluginWizard operates on the original source code.
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

#endif // taPlugin_h

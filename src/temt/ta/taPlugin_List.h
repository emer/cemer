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

#ifndef taPlugin_List_h
#define taPlugin_List_h 1

// parent includes:
#include <taPluginBase_List>

// member includes:

// declare all other types mentioned but not required to include:
class taPlugin; // 
TypeDef_Of(taPlugin);

TypeDef_Of(taPlugin_List);

class TA_API taPlugin_List: public taPluginBase_List {
  // ##EXPAND_DEF_1 plugins available to the program (also used for descs)
INHERITED(taPluginBase_List)
public:
  inline taPlugin*	FastEl(int i) {return (taPlugin*)inherited::FastEl(i);}
  taPlugin*		FindFilename(const String& value);
  inline taPlugin*	FindUniqueId(const String& value)
    {return (taPlugin*)inherited::FindUniqueId(value);}
  
  void			EnableAllPlugins(); // enable all the available plugins
  void			ListAllPlugins(); // list all the available plugins and their status
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

#endif // taPlugin_List_h

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

#ifndef taPluginDep_h
#define taPluginDep_h 1

// parent includes:
#include <taPluginBase>

// member includes:

// declare all other types mentioned but not required to include:


taTypeDef_Of(taPluginDep);

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
  void CheckThisConfig_impl(bool quiet, bool& rval) override; // only for _deps
private:
  void	Initialize();
  void	Destroy() {}
  void	Copy_(const taPluginDep& cp); //note: we only use this for descs, not actual plugins
};

#endif // taPluginDep_h

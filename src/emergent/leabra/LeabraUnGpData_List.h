// Copyright, 1995-2013, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of Emergent
//
//   Emergent is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   Emergent is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.

#ifndef LeabraUnGpData_List_h
#define LeabraUnGpData_List_h 1

// parent includes:
#include <LeabraUnGpData>
#include <taList>

// member includes:

// declare all other types mentioned but not required to include:

TypeDef_Of(LeabraUnGpData_List);

class LEABRA_API LeabraUnGpData_List: public taList<LeabraUnGpData> {
  // ##NO_TOKENS ##NO_UPDATE_AFTER ##CAT_Network ##NO_EXPAND_ALL list of unit group data for leabra unit subgroups
INHERITED(taList<LeabraUnGpData>)
public:

  override String 	GetTypeDecoKey() const { return "Unit"; }

  NOCOPY(LeabraUnGpData_List)
  TA_BASEFUNS(LeabraUnGpData_List);
private:
  void	Initialize() 		{ SetBaseType(&TA_LeabraUnGpData); }
  void 	Destroy()		{ };
};

#endif // LeabraUnGpData_List_h

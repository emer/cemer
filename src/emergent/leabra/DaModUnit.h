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

#ifndef DaModUnit_h
#define DaModUnit_h 1

// parent includes:
#include <LeabraUnit>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(DaModUnit);

class E_API DaModUnit : public LeabraUnit {
  // obsolete -- now incorporated into base LeabraUnit
INHERITED(LeabraUnit)
public:
  TA_BASEFUNS_NOCOPY(DaModUnit);
private:
 void	Initialize()    { };
  void	Destroy()	{ };
};

#endif // DaModUnit_h

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

#ifndef DMemShareVar_h
#define DMemShareVar_h 1

// parent includes:
#include <taBase>

// member includes:

// declare all other types mentioned but not required to include:


class TA_API DMemShareVar : public taBase {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS ##NO_UPDATE_AFTER ##CAT_DMem definition of a variable of a given type (FLOAT, DOUBLE, INT) that each proc has some instances of (can be multiple members of a given object) -- these can all be Allgather'ed to sync across procs
INHERITED(taBase)
public:

  void	Dummy(const DMemShareVar&) { };
  TA_BASEFUNS(DMemShareVar);
private:
  NOCOPY(DMemShareVar)
  void 	Initialize() { };
  void 	Destroy()	{ };
};

#endif // DMemShareVar_h

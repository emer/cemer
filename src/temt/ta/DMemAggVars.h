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

#ifndef DMemAggVars_h
#define DMemAggVars_h 1

// parent includes:
#include <taBase>

// member includes:

// declare all other types mentioned but not required to include:


class TA_API DMemAggVars : public taBase {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS ##NO_UPDATE_AFTER ##CAT_DMem collection of a variables of a different types (FLOAT, DOUBLE, INT) that *each proc has an instance of* (can be multiple members of a given object) -- these can all be Allreduced'ed to aggregate across procs (must all use same agg op -- use diff objs for diff ops if fixed)
INHERITED(taBase)
public:

  void	Dummy(const DMemAggVars&) { };
  TA_BASEFUNS_NOCOPY(DMemAggVars);
private:
  void 	Initialize() { };
  void 	Destroy()	{ };
};

#endif // DMemAggVars_h

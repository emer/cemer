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

#ifndef FullPrjnSpec_h
#define FullPrjnSpec_h 1

// parent includes:
#include <ProjectionSpec>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(FullPrjnSpec);

class E_API FullPrjnSpec : public ProjectionSpec {
  // Full connectivity between layers
INHERITED(ProjectionSpec)
public:
  override void Connect_impl(Projection* prjn);
  // Connection function for full connectivity
  int 	ProbAddCons_impl(Projection* prjn, float p_add_con, float init_wt = 0.0f);

  TA_BASEFUNS_NOCOPY(FullPrjnSpec);
private:
  void	Initialize() 		{ };
  void 	Destroy()		{ };
};

#endif // FullPrjnSpec_h

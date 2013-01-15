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

#ifndef GpMapConvergePrjnSpec_h
#define GpMapConvergePrjnSpec_h 1

// parent includes:
#include <ProjectionSpec>

// member includes:

// declare all other types mentioned but not required to include:

TypeDef_Of(GpMapConvergePrjnSpec);

class EMERGENT_API GpMapConvergePrjnSpec : public ProjectionSpec {
  // #AKA_GpAggregatePrjnSpec generates a converging map of the units within a sending layer that has unit groups into a receiving layer that has the same geometry as one of the unit groups -- each recv unit receives from the corresponding unit in all of the sending unit groups
INHERITED(ProjectionSpec)
public:
  
  void 		Connect_impl(Projection* prjn);

  TA_SIMPLE_BASEFUNS(GpMapConvergePrjnSpec);
protected:
  SPEC_DEFAULTS;
private:
  void	Initialize();
  void 	Destroy()		{ };
  void	Defaults_init() 	{ };
};

#endif // GpMapConvergePrjnSpec_h

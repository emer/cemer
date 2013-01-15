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

#ifndef GpMapDivergePrjnSpec_h
#define GpMapDivergePrjnSpec_h 1

// parent includes:
#include <ProjectionSpec>

// member includes:

// declare all other types mentioned but not required to include:

TypeDef_Of(GpMapDivergePrjnSpec);

class EMERGENT_API GpMapDivergePrjnSpec : public ProjectionSpec {
  // projects from a layer without unit groups into a receiving layer with unit groups and that has the same unit geometry in each of its unit groups as the sending layer -- each unit projects to the corresponding unit in all of the receiving unit groups
INHERITED(ProjectionSpec)
public:
  
  void 		Connect_impl(Projection* prjn);

  TA_SIMPLE_BASEFUNS(GpMapDivergePrjnSpec);
protected:
  SPEC_DEFAULTS;
private:
  void	Initialize();
  void 	Destroy()		{ };
  void	Defaults_init() 	{ };
};

#endif // GpMapDivergePrjnSpec_h

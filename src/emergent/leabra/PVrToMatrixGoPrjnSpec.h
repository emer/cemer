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

#ifndef PVrToMatrixGoPrjnSpec_h
#define PVrToMatrixGoPrjnSpec_h 1

// parent includes:
#include "network_def.h"
#include <ProjectionSpec>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(PVrToMatrixGoPrjnSpec);

class E_API PVrToMatrixGoPrjnSpec : public ProjectionSpec {
  // Projection from PVLV PVr layer to Matrix_Go layer -- PVr=1 value unit (right-most) connects to Output Go units, while PVr=.5 value unit (middle) connects to Input and Maint Go units, providing a bias for output gating when rewards are expected
INHERITED(ProjectionSpec)
public:
  void	Connect_impl(Projection* prjn);

  TA_BASEFUNS_NOCOPY(PVrToMatrixGoPrjnSpec);
protected:
  SPEC_DEFAULTS;
private:
  void	Initialize()		{ };
  void 	Destroy()		{ };
  void	Defaults_init() 	{ };
};

#endif // PVrToMatrixGoPrjnSpec_h

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

#ifndef GpCustomPrjnSpecBase_h
#define GpCustomPrjnSpecBase_h 1

// parent includes:
#include "network_def.h"
#include <ProjectionSpec>

// member includes:
#include <Layer>

// declare all other types mentioned but not required to include:

eTypeDef_Of(GpCustomPrjnSpecBase);

class E_API GpCustomPrjnSpecBase : public ProjectionSpec {
  // #VIRT_BASE basic custom group-level projection spec -- core methods for connecting groups
INHERITED(ProjectionSpec)
public:
  virtual void Connect_Gp(Projection* prjn, Layer::AccessMode racc_md, int rgpidx,
			  Layer::AccessMode sacc_md, int sgpidx, bool make_cons);
  // make a projection from all senders in sugp into all receivers in rugp -- uses two-pass loop for allocating vs. making cons -- no pre-allocation

  TA_BASEFUNS_NOCOPY(GpCustomPrjnSpecBase);
protected:
  SPEC_DEFAULTS;
private:
  void	Initialize()		{ };
  void 	Destroy()		{ };
  void	Defaults_init() 	{ };
};

#endif // GpCustomPrjnSpecBase_h

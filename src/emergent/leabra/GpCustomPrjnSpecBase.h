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
  virtual void AllocGp_Recv(Projection* prjn, Layer::AccessMode racc_md, int rgpidx,
			    Layer::AccessMode sacc_md, int n_send);
  // allocate recv cons for given recv gp, for given number of sending groups
  virtual void AllocGp_Send(Projection* prjn, Layer::AccessMode sacc_md, int sgpidx,
			    Layer::AccessMode racc_md, int n_recv);
  // allocate send cons for given send gp, for given number of recv groups
  virtual void Connect_Gp(Projection* prjn, Layer::AccessMode racc_md, int rgpidx,
			  Layer::AccessMode sacc_md, int sgpidx,
			  int extra_rgp_alloc = 0, int extra_sgp_alloc = 0);
  // make a projection from all senders in sugp into all receivers in rugp -- if extra_*gp_alloc > 0 then alloc this many extra groups worth of [recv/send] for the [recv/send] -- use -1 for already allocated to prevent re-allocation

  TA_BASEFUNS_NOCOPY(GpCustomPrjnSpecBase);
protected:
  SPEC_DEFAULTS;
private:
  void	Initialize()		{ };
  void 	Destroy()		{ };
  void	Defaults_init() 	{ };
};

#endif // GpCustomPrjnSpecBase_h

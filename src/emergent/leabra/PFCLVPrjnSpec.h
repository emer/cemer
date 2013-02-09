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

#ifndef PFCLVPrjnSpec_h
#define PFCLVPrjnSpec_h 1

// parent includes:
#include <GpCustomPrjnSpecBase>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(PFCLVPrjnSpec);

class E_API PFCLVPrjnSpec : public GpCustomPrjnSpecBase {
  // A special projection spec for PFC to LVe/i layers. If n unit groups (stripes) in LV == PFC, then it makes Gp one-to-one projections; if LV stripes == 1, it makes a single full projection; if LV stripes == PFC + 1, the first projection is full and the subsequent are gp one-to-one; if recv fm multiple PFC layers, same logic applies to each
INHERITED(GpCustomPrjnSpecBase)
public:

  void	Connect_impl(Projection* prjn);

  TA_BASEFUNS_NOCOPY(PFCLVPrjnSpec);
protected:
  SPEC_DEFAULTS;
private:
  void	Initialize()		{ };
  void 	Destroy()		{ };
  void	Defaults_init() 	{ };
};

#endif // PFCLVPrjnSpec_h

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

#ifndef PFCDeepGatedConSpec_h
#define PFCDeepGatedConSpec_h 1

// parent includes:
#include <LeabraConSpec>

// member includes:
#include <LeabraNetwork>

// declare all other types mentioned but not required to include:

eTypeDef_Of(PFCDeepGatedConSpec);

class E_API PFCDeepGatedConSpec : public LeabraConSpec {
  // projection from act_ctxt variable, which represents activation of PFC Deep layers
INHERITED(LeabraConSpec)
public:

  // TODO: this is actually going to be tricky b/c we can't use net_delta stuff without all the extra biz for that -- maybe easiest thing is to gate the regular superficial activations instead?

  TA_SIMPLE_BASEFUNS(PFCDeepGatedConSpec);
protected:
  SPEC_DEFAULTS;
private:
  void 	Initialize();
  void	Destroy()		{ };
  void	Defaults_init();
};

#endif // PFCDeepGatedConSpec_h

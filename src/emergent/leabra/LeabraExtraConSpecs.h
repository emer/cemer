// Copyright 2017, Regents of the University of Colorado,
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

#ifndef LeabraExtraConSpecs_h
#define LeabraExtraConSpecs_h 1

// this file includes all the non-base LeabraConSpec subtypes for the main
// compile space

// parent includes:
#include <LeabraConSpec>

#include <State_main>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(DeepCtxtConSpec);

class E_API DeepCtxtConSpec : public LeabraConSpec {
  // #AKA_LeabraTICtxtConSpec sends deep layer deep_raw activation values to deep_ctxt_net variable on receiving units -- typically used to integrate across the local context within a layer, providing both temporal integration (TI) learning, and the basis for normalizing attentional signals -- use for SELF projection in a layer -- wt_scale should be set to 1, 1
INHERITED(LeabraConSpec)
public:

#include <DeepCtxtConSpec>
  
  bool  CheckConfig_RecvCons(Projection* prjn, bool quiet=false) override;

  void  GetPrjnName(Projection& prjn, String& nm) override;

  TA_SIMPLE_BASEFUNS(DeepCtxtConSpec);
private:
  void Initialize();
  void Destroy()     { };
};


#endif // LeabraExtraConSpecs_h

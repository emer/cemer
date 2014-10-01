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

#ifndef GPiUnitSpec_h
#define GPiUnitSpec_h 1

// parent includes:
#include <LeabraUnitSpec>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(GPiGoNogoSpec);

class E_API GPiGoNogoSpec : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra weighting of Go vs. NoGo inputs
INHERITED(SpecMemberBase)
public:
  float        nogo;            // #MIN_0 how much to weight NoGo inputs

  String       GetTypeDecoKey() const override { return "UnitSpec"; }

  TA_SIMPLE_BASEFUNS(GPiGoNogoSpec);
protected:
  SPEC_DEFAULTS;
private:
  void  Initialize();
  void  Destroy()       { };
  void  Defaults_init();
};

eTypeDef_Of(GPiUnitSpec);

class E_API GPiUnitSpec : public LeabraUnitSpec {
  // GPi globus pallidus internal segment, analogous with SNr -- major output pathway of the basal ganglia.  This integrates 
INHERITED(LeabraUnitSpec)
public:
  GPiGoNogoSpec   go_nogo;      // how to weight the Go vs. NoGo pathway inputs

  void	Compute_NetinRaw(LeabraUnit* u, LeabraNetwork* net, int thread_no=-1) override;

  bool  CheckConfig_Unit(Unit* un, bool quiet=false) override;
  void  HelpConfig();   // #BUTTON get help message for configuring this spec

  TA_SIMPLE_BASEFUNS(GPiUnitSpec);
protected:
  SPEC_DEFAULTS;
private:
  void  Initialize();
  void  Destroy()     { };
  void	Defaults_init();
};

#endif // GPiUnitSpec_h

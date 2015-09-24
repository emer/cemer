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

#ifndef BasAmygUnitSpec_h
#define BasAmygUnitSpec_h 1

// parent includes:
#include <D1D2UnitSpec>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(BasAmygUnitSpec);

class E_API BasAmygUnitSpec : public D1D2UnitSpec {
  // Basal Amygdala units -- specifies the subtype for valence X acquisition vs. extinction (4 subtypes from these combinations) -- these then determine the D1 vs. D2 type
INHERITED(D1D2UnitSpec)
public:
  enum AcqExt { // acquisition vs. extinction type
    ACQ,        // acquisition neurons -- learn from LatAmyg and da_p
    EXT,        // extinction neurons -- learn from context / pfc input and ACQ up-state signal and da_p (using D2 receptors)
  };

  AcqExt        acq_ext;        // acquisition vs. extinction sub-type
  Valence       valence;        // US valence coding -- appetitive vs. aversive
  DAReceptor    dar;            // #READ_ONLY #SHOW type of dopamine receptor: D1 vs. D2 -- computed automatically from acq_ext and valence
 
  TA_SIMPLE_BASEFUNS(BasAmygUnitSpec);
protected:
  SPEC_DEFAULTS;
  void  UpdateAfterEdit_impl() override;
private:
  void  Initialize();
  void  Destroy()     { };
  void  Defaults_init();
};

#endif // BasAmygUnitSpec_h

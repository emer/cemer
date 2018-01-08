// Co2018ght 2017-22018 Regents of the University of Colorado,
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

// this must be included in a defined State_core/main/cuda context with appropriate
// #ifndef multiple-include protection AT THAT LEVEL not here..

#ifdef __MAKETA__
#pragma maketa_file_is_target ProjectionSpec
#endif

class STATE_CLASS(RenormInitWtsSpec) : public STATE_CLASS(taOBase) {
  // #STEM_BASE ##INLINE ##CAT_Projection parameters for renormalizing initial weight values
INHERITED(taOBase)
public:
  bool          on;             // renormalize initial weight values
  bool          mult_norm;      // #CONDSHOW_ON_on use multiplicative normalization to rescale the weight values to hit the target value, which is appropriate for all-positive weight values (e.g., Leabra) -- otherwise use addition to add/subtract a constant from all weights to hit the target value, which is approparite for pos/neg signed weight values (e.g., backprop)
  float		avg_wt;	        // #CONDSHOW_ON_on target average weight value per connection to renormalize to -- e.g., the sum across all weights will be adjusted so that it equals n_cons * avg_wt

  STATE_DECO_KEY("ProjectionSpec");
  STATE_TA_STD_CODE(RenormInitWtsSpec);
private:
  void 	Initialize()  { on = false;  mult_norm = true;  avg_wt = 0.5f; }
};


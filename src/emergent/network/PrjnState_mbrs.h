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

// this must be included in a defined State_cpp/cuda/main context with appropriate
// #ifndef multiple-include protection AT THAT LEVEL not here..

#ifdef __MAKETA__
#pragma maketa_file_is_target Projection
#pragma maketa_file_is_target PrjnState
#endif


class STATE_CLASS(PrjnConStats) : public STATE_CLASS(taOBase) {
  // ##INLINE ##NO_TOKENS ##CAT_Network connection statistics for projections -- included in PrjnState / Projection
INHERITED(taOBase)
public:
  int           max_size;     // #CAT_State max number of connections created per unit across all the units in this projection
  int           max_alloc;    // #CAT_State max number of connections allocated per unit across all the units in this projection
  float         pct_alloc;    // #CAT_State proportion of max_size / max_alloc 
  float         pct_units;    // #CAT_State proportion of max_size / other layer n_units -- proportion of units connected at maximum -- can be useful for tuning SymmetricPrjnSpec values

  INLINE void   UpdtStatsFmCons(int size, int alloc) {
    max_size = MAX(size, max_size);     max_alloc = MAX(alloc, max_alloc);
  }
  // #IGNORE update stats from connection
  
  INLINE void   FinalStats(int n_units) {
    if(max_alloc > 0) pct_alloc = (float)max_size / (float)max_alloc;
    else              pct_alloc = 0.0f;
    if(n_units > 0)   pct_units = (float)max_size / (float)n_units;
    else              pct_units = 0.0f;
  }
  // #IGNORE update final stats based on accumulated max's and number of units in layer
  
  INLINE void   ResetStats() { max_size = max_alloc = 0; pct_alloc = pct_units = 0.0f; }
  // reset stats
  
  STATE_DECO_KEY("Projection");
  STATE_TA_STD_CODE(PrjnConStats);
private:
  INLINE void	Initialize() {
    ResetStats();
  }
};


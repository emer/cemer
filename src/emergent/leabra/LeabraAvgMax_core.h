// Copyright 2013-20188 Regents of the University of Colorado,
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

// include this file in other _core files that include this type as a member
// outside of _core, include / refer to _cpp / _cuda versions

// this pragma ensures that maketa properly grabs this type information even though
// this file is included in the other files -- we get ta info for main and _cpp, not cuda

#ifdef __MAKETA__
#pragma maketa_file_is_target LeabraLayer
#pragma maketa_file_is_target LeabraLayerState
#endif

// member includes:
// these are needed for FLT_MAX
#ifndef __MAKETA__
#include <math.h>
#include <limits.h>
#include <float.h>
#endif

#define LEABRA_AVG_MAX STATE_CLASS(LeabraAvgMax)

class LeabraAvgMax_cpp; // 

class STATE_CLASS(LeabraAvgMax) : public STATE_CLASS(taOBase) {
  // ##INLINE ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra holds average and max statistics
INHERITED(taOBase)
public:
  float		avg;		// #DMEM_AGG_SUM average value
  float		max;		// #DMEM_AGG_SUM maximum value
  int 		max_i;		// index of unit with maximum value
  float		sum;		// #DMEM_AGG_SUM sum for computing average
  int		n;		// #DMEM_AGG_SUM number of items in sum

  INLINE void	InitVals()	{ avg = sum = 0.0f; n = 0; max = -FLT_MAX; max_i = -1; }
  // init for computing update from new data

  INLINE void	UpdtVals(float val, int idx)
  { sum += val; ++n; if(val > max) { max = val; max_i = idx; } }
  // update from data as it comes in

  INLINE void	CalcAvg()
  { if(n > 0) avg = sum / (float)n; else { avg = sum; max = 0.0f; } }
  // compute the avg after doing UpdtVals on all the data

  INLINE void	UpdtFmAvgMax(const STATE_CLASS(LeabraAvgMax)& oth)
  { sum += oth.sum; n += oth.n; if(oth.max > max) { max = oth.max; max_i = oth.max_i; } }
  // update a higher-order guy from a lower-level guy (e.g., layer from unit group)

  INLINE void	CopyFmAvgMax(const STATE_CLASS(LeabraAvgMax)& oth)
  { avg = oth.avg;  max = oth.max; max_i = oth.max_i; sum = oth.sum; n = oth.n; }
  // copy from other

#ifdef STATE_MAIN
  LeabraAvgMax& operator =(const LeabraAvgMax_cpp& oth)
  { avg = oth.avg;  max = oth.max; max_i = oth.max_i; sum = oth.sum; n = oth.n; return *this; }
#endif  

  STATE_DECO_KEY("Layer");
  STATE_TA_STD_CODE(LeabraAvgMax);
private:
  void	Initialize()    { avg = sum = max = 0.0f; n = 0; max_i = -1; }
};

class STATE_CLASS(LeabraInhibVals) : public STATE_CLASS(taOBase) {
  // ##INLINE ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra holds values for computed inhibition
INHERITED(taOBase)
public:
  float         ffi;            // computed feedforward inhibition
  float         fbi;            // computed feedback inhibition (total)
  float		g_i;		// overall value of the inhibition -- this is what is added into the unit g_i inhibition level (along with any synaptic unit-driven inhibition)
  float		g_i_orig; 	// original value of the inhibition (before any layer group effects set in)
  float		lay_g_i;	// for unit groups, this is the layer-level inhibition that is MAX'd with the unit-group level inhibition to produce the net inhibition, if unit_gp_inhib is on

  INLINE void   InitVals() {
    ffi = 0.0f;  fbi = 0.0f;  g_i = 0.0f;  g_i_orig = 0.0f;  lay_g_i = 0.0f;
  }
  // initialize the inhibition values

  STATE_DECO_KEY("Layer");
  STATE_TA_STD_CODE(LeabraInhibVals);
private:
  void	Initialize()    { InitVals(); }
};


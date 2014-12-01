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

#ifndef AvgMaxVals_h
#define AvgMaxVals_h 1

// parent includes:
#include "network_def.h"
#include <taOBase>

// member includes:
// these are needed for FLT_MAX
#include <math.h>
#include <limits.h>
#include <float.h>

// declare all other types mentioned but not required to include:

eTypeDef_Of(AvgMaxValsRaw);

class E_API AvgMaxValsRaw {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra raw average and max values -- not a taBase object -- for thread-specific memory 
public:
  float		avg;		// #DMEM_AGG_SUM average value
  float		max;		// #DMEM_AGG_SUM maximum value
  int 		max_i;		// index of unit with maximum value
  float		sum;		// #DMEM_AGG_SUM sum for computing average
  int		n;		// #DMEM_AGG_SUM number of items in sum

  inline void	InitVals()	{ avg = sum = 0.0f; n = 0; max = -FLT_MAX; max_i = -1; }
  // init for computing update from new data

  inline void	UpdtVals(float val, int idx)
  { sum += val; ++n; if(val > max) { max = val; max_i = idx; } }
  // update from data as it comes in

  inline void	CalcAvg()
  { if(n > 0) avg = sum / (float)n; else avg = sum; }
  // compute the avg after doing UpdtVals on all the data

  inline void	UpdtFmAvgMax(const AvgMaxValsRaw& oth)
  { sum += oth.sum; n += oth.n; if(oth.max > max) { max = oth.max; max_i = oth.max_i; } }
  // update a higher-order guy from a lower-level guy (e.g., layer from unit group)
};


eTypeDef_Of(AvgMaxVals);

class E_API AvgMaxVals : public taOBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra holds average and max statistics
INHERITED(taOBase)
public:
  bool		cmpt;		// whether to compute these values or not -- only for optional values
  float		avg;		// #DMEM_AGG_SUM average value
  float		max;		// #DMEM_AGG_SUM maximum value
  int 		max_i;		// index of unit with maximum value
  float		sum;		// #DMEM_AGG_SUM sum for computing average
  int		n;		// #DMEM_AGG_SUM number of items in sum

  inline void	InitVals()	{ avg = sum = 0.0f; n = 0; max = -FLT_MAX; max_i = -1; }
  // init for computing update from new data

  inline void	UpdtVals(float val, int idx)
  { sum += val; ++n; if(val > max) { max = val; max_i = idx; } }
  // update from data as it comes in

  inline void	CalcAvg()
  { if(n > 0) avg = sum / (float)n; else avg = sum; }
  // compute the avg after doing UpdtVals on all the data

  inline void	UpdtFmAvgMax(const AvgMaxVals& oth)
  { sum += oth.sum; n += oth.n; if(oth.max > max) { max = oth.max; max_i = oth.max_i; } }
  // update a higher-order guy from a lower-level guy (e.g., layer from unit group)

  inline void	UpdtFmAvgMaxRaw(const AvgMaxValsRaw& oth)
  { sum += oth.sum; n += oth.n; if(oth.max > max) { max = oth.max; max_i = oth.max_i; } }
  // update the official guy from raw data

  String       GetTypeDecoKey() const override { return "Layer"; }

  void	Copy_(const AvgMaxVals& cp);
  TA_BASEFUNS(AvgMaxVals);
private:
  void	Initialize();
  void 	Destroy()	{ };
};

#endif // AvgMaxVals_h

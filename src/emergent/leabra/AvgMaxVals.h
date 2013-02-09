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

eTypeDef_Of(AvgMaxVals);

class E_API AvgMaxVals : public taOBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra holds average and max statistics
INHERITED(taOBase)
public:
  bool		cmpt;		// whether to compute these values or not -- only for optional values
  float		avg;		// #DMEM_AGG_SUM average value
  float		max;		// #DMEM_AGG_SUM maximum value
  int 		max_i;		// index of unit with maximum value

  inline void	InitVals()	{ avg = 0.0f; max = -FLT_MAX; max_i = -1; }
  inline void	UpdtVals(float val, int idx)
  { avg += val; if(val > max) { max = val; max_i = idx; } }
  inline void	CalcAvg(int n) { if(n > 0) avg /= (float)n; }

  inline void	UpdtFmAvgMax(const AvgMaxVals& oth, int gpn, int idx)
  { avg += oth.avg * (float)gpn; if(oth.max > max) { max = oth.max; max_i = idx; } }

  override String       GetTypeDecoKey() const { return "Layer"; }

  void	Copy_(const AvgMaxVals& cp);
  TA_BASEFUNS(AvgMaxVals);
private:
  void	Initialize();
  void 	Destroy()	{ };
};

#endif // AvgMaxVals_h

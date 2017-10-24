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

// this must be included in a defined State_core/main/cuda context with appropriate
// #ifndef multiple-include protection AT THAT LEVEL not here..

#pragma maketa_file_is_target Network
#pragma maketa_file_is_target NetworkState


class STATE_CLASS(NetStatsSpecs) : public STATE_CLASS(taOBase) {
  // ##INLINE ##NO_TOKENS ##CAT_Network parameters for how stats are computed
INHERITED(taOBase)
public:
  bool          sse_unit_avg;   // #CAT_Statistic compute sse as average sse over units (i.e., divide by total number of target units in layer)
  bool          sse_sqrt;       // #CAT_Statistic take the square root of the SSE, producing a Euclidian distance instead of raw sse (note this cannot technically be added across trials in a linear fashion, as raw sse can)
  float         cnt_err_tol;    // #CAT_Statistic tolerance for computing the count of number of errors over current epoch
  bool          prerr;          // #CAT_Statistic compute precision and recall error values over units, at same time as computing sum squared error (SSE)

  STATE_DECO_KEY("Network");
  STATE_TA_STD_CODE(NetStatsSpecs);
private:
  INLINE void	Initialize() {
    sse_unit_avg = false;
    sse_sqrt = false;
    cnt_err_tol = 0.0f;
    prerr = false;
  }
};

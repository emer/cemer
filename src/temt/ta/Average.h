// Copyright, 1995-2013, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of The Emergent Toolkit
//
//   This library is free software; you can redistribute it and/or
//   modify it under the terms of the GNU Lesser General Public
//   License as published by the Free Software Foundation; either
//   version 2.1 of the License, or (at your option) any later version.
//
//   This library is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//   Lesser General Public License for more details.

#ifndef Average_h
#define Average_h 1

// parent includes:
#include <taOBase>

// member includes:

// declare all other types mentioned but not required to include:

taTypeDef_Of(Average);

class TA_API Average : public taOBase {
  // encapsulates the sum and count values for computing an average value in an incremental fashion over time
INHERITED(taOBase)
public:
  float         avg;    // the computed average value that was last computed, as sum / n -- this may not reflect current sum, n values -- just depends on when GetAvg was computed -- see UpdtAvg versions of increment methods
  float         sum;    // overall sum of values accumulated since last reset
  int           n;      // the number of values accumulated since last reset


  inline void   ResetSum()
  { sum = 0.0f; n = 0; }
  // #CAT_Average reset the sum and n accumulation variables
  inline void   ResetAvg()
  { ResetSum(); avg = 0.0f; }
  // #CAT_Average reset the sum and n accumulation variables, and the computed average value

  inline float  GetAvg()
  { if(n > 0) avg = sum / (float)n; return avg; }
  // #CAT_Average compute the average as sum / n, update the avg member to store this value, and return it

  inline void   Increment(float val)
  { sum += val; n++; }
  // #CAT_Average increment the sum by given value, and n by one count
  inline float  IncrementAvg(float val)
  { Increment(val); return GetAvg(); }
  // #CAT_Average increment the sum by given value, and n by one count, and return new average value (updates avg field)

  inline void   IncrementN(float val, int n_new)
  { sum += val; n += n_new; }
  // #CAT_Average increment the sum by given value, and n by given count -- for a batch update of multiple items
  inline float  IncrementNAvg(float val, int n_new)
  { IncrementN(val, n_new); return GetAvg(); }
  // #CAT_Average increment the sum by given value, and n by given count, and return new average value (updates avg field)

  TA_BASEFUNS_LITE(Average);
private:
  void  Initialize()  { ResetAvg(); }
  void  Destroy()     { };
  void  Copy_(const Average& cp) { avg = cp.avg; sum = cp.sum; n = cp.n; }
};

#endif // Average_h

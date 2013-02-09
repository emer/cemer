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

#ifndef PRerrVals_h
#define PRerrVals_h 1

// parent includes:
#include "network_def.h"
#include <taOBase>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(PRerrVals);

class E_API PRerrVals : public taOBase {
  // ##NO_TOKENS #INLINE #NO_UPDATE_AFTER ##CAT_Network contains precision and recall error values
INHERITED(taOBase)
public:
  float         true_pos;       // #DMEM_AGG_SUM true positive values -- e.g., how many active targets were correctly activated by the network (actually a sum over graded activations -- if(targ > act) act else targ)
  float         false_pos;      // #DMEM_AGG_SUM false positive values -- e.g., how many inactive targets were incorrectly activated by the network (actually a sum over graded activations -- if(act > targ) act - targ)
  float         false_neg;      // #DMEM_AGG_SUM false negative values -- e.g., how many active targets were incorrectly not activated by the network (actually a sum over graded activations -- if(targ > act) targ - act)
  float         precision;      // precision = true_pos / (true_pos + false_pos) -- how many of the positive responses were true positives -- i.e., of the responses the network made, how many were correct -- does not include any of the false negatives, so the network could be under-responding (see recall)
  float         recall;         // recall = true_pos / (true_pos + false_neg) -- how many true positive responses were there relative to the total number of positive targets -- if it did not respond to a number of cases where it should have, then recall will be low
  float         fmeasure;       // fmeasure = 2 * precision * recall / (precision + recall) -- harmonic mean of precision and recall -- is 1 when network is performing optimally -- this is the best overall value to look at for summary performance

  void          InitVals() { true_pos = false_pos = false_neg = precision = recall = fmeasure = 0.0f; }
  // initialize all values to 0

  void          IncrVals(const PRerrVals& vls)
  { true_pos += vls.true_pos; false_pos += vls.false_pos; false_neg += vls.false_neg; }
  // increment values from another set of values

  void          ComputePR() {
    if(true_pos > 0.0f) {
      precision = true_pos / (true_pos + false_pos); recall = true_pos / (true_pos + false_neg);
      fmeasure = 2 * precision * recall / (precision + recall);
    }
    else {
      precision = recall = fmeasure = 0.0f;
    }
  }
  // compute the precision, recall, and fmeasure values based on current raw stats values

  SIMPLE_COPY(PRerrVals);
  TA_BASEFUNS(PRerrVals);
private:
  void  Initialize()            { InitVals(); }
  void  Destroy()               { };
};

#endif // PRerrVals_h

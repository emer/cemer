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
#include <taMath_float>
// declare all other types mentioned but not required to include:

eTypeDef_Of(PRerrVals);

class E_API PRerrVals : public taOBase {
  // ##NO_TOKENS #INLINE #NO_UPDATE_AFTER ##CAT_Network contains precision and recall error values
INHERITED(taOBase)
public:
  float         true_pos;       // #DMEM_AGG_SUM true positive values -- e.g., how many active targets were correctly activated by the network (actually a sum over graded activations -- if(targ > act) act else targ)
  float         false_pos;      // #DMEM_AGG_SUM false positive values -- e.g., how many inactive targets were incorrectly activated by the network (actually a sum over graded activations -- if(act > targ) act - targ)
  float         false_neg;      // #DMEM_AGG_SUM false negative values -- e.g., how many active targets were incorrectly not activated by the network (actually a sum over graded activations -- if(targ > act) targ - act)
  float         true_neg;      // #DMEM_AGG_SUM true negative values -- e.g., to what extent was the unit correctly off
  float         precision;      // precision = true_pos / (true_pos + false_pos) -- how many of the positive responses were true positives -- i.e., of the responses the network made, how many were correct -- does not include any of the false negatives, so the network could be under-responding (see recall)
  float         recall;         // recall = true_pos / (true_pos + false_neg) -- how many true positive responses were there relative to the total number of positive targets -- if it did not respond to a number of cases where it should have, then recall will be low
  float         specificity;    // specificity = true_neg / (false_pos + true_neg) -- proportion of true negatives correctly identified as such
  float         fallout;       // fallout = fp / (fp + tn) -- fallout - false positive rate
  float         fdr;           // false discovery rate = fp / (fp + tp)
  float         fmeasure;       // fmeasure = 2 * precision * recall / (precision + recall) -- harmonic mean of precision and recall -- is 1 when network is performing optimally
  float         mcc;            // mcc = ((tp*tn) - (fp*fn)) / sqrt((tp+fp)*(tp+fn)*(tn+fp)*(tn+fn)) -- Matthews Correlation Coefficient. balanced statistic that reflects all of the kinds of errors the network could make. ranges from -1 to 1. this is the best overall value to look at for summary performance
  void          InitVals() { true_pos = false_pos = false_neg = true_neg = specificity = mcc = precision = recall = fmeasure = 0.0f; }
  // initialize all values to 0

  void          IncrVals(const PRerrVals& vls)
  { true_pos += vls.true_pos; false_pos += vls.false_pos; false_neg += vls.false_neg; true_neg += vls.true_neg; }
  // increment values from another set of values

  void          ComputePR() {
    precision = recall = specificity = fdr = fallout = fmeasure = mcc = 0.0f;
    float tp = true_pos; float fp = false_pos; float tn = true_neg; float fn = false_neg;

    precision   = tp > 0 && fp > 0  ? tp/(tp+fp):0;
    recall      = tp > 0 && fn > 0  ? tp/(tp+fn):0;
    specificity = fp > 0 && tn > 0  ? tn/(fp+tn):0;
    fdr         = tp > 0 && fp > 0  ? fp/(fp+tp):0;
    fallout     = fp > 0 && tn > 0  ? fp/(fp+tn):0;
    fmeasure    = tp > 0 && fp > 0 && fn > 0 ? 2*tp/(2*tp+fp+fn):0;
    mcc         = tp > 0 && fp > 0 && tn > 0 && fn > 0 ? (tp*tn-fp*fn)/taMath_float::sqrt((tp+fp)*(tp+fn)*(tn+fp)*(tn+fn)):0;
  }
  // compute the precision, recall, and fmeasure values based on current raw stats values

  SIMPLE_COPY(PRerrVals);
  TA_BASEFUNS(PRerrVals);
private:
  void  Initialize()            { InitVals(); }
  void  Destroy()               { };
};

#endif // PRerrVals_h

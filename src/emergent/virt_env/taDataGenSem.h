// Co2018ght 2016-2017, Regents of the University of Colorado,
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

#ifndef taDataGenSem_h
#define taDataGenSem_h 1

// parent includes:
#include <taDataGen>
#include "network_def.h"

// member includes:

// declare all other types mentioned but not required to include:
class DataTable_Group; //

eTypeDef_Of(taDataGenSem);

class E_API taDataGenSem : public taDataGen {
  // semantic vector generation subclass of taDataGen -- has routines that generate sparse bit vectors based on a target distance matrix of item similarities
INHERITED(taDataGen) 
public:

  static float 	SemVecGen_DistMatDist
  (DataTable* dist_mat, DataTable* dist_mat2, DataTable* delta_mat = NULL,
   float dist_off=0.0f);
  // #CAT_SemVecGen utility function: compute normalized SS dist between two distance matricies -- if delta_mat is non-null, it records the deltas between the two matricies (must have same format) -- if dist_off != 0 then it is subtracted from each element of the first dist_mat prior to comparison

  static void 	SemVecGen_GetDistTopK(DataTable* dist_mat, int k_n, DataTable* dist_topk);
  // #CAT_SemVecGen utility function: get a kval on the distances, which is distance value such that k_n items are >= kval -- these are ones that network should make as similar as possible, while others are made dissimilar

  static void 	SemVecGen_GetDistBotK(DataTable* dist_mat, int k_n, DataTable* dist_botk);
  // #CAT_SemVecGen utility function: get a kval on the distances, which is distance value such that k_n items are <= kval -- these are ones that network should make as similar as possible, while others are made dissimilar

  static bool	SemVecGen_Flip
    (bool toward, DataTable* dest, DataTable* work, DataTable* work_sum,
     Variant dest_name_col, Variant dest_vec_col,
     DataTable* trg_dist_mat, DataTable* dist_kmat, DataTable* bit_freqs,
     bool use_k_thr = true, int k_val = 5, float p_flip_items = 0.1f,
     float same_wt = 1.0f, float diff_wt = 1.0f, float softmax_gain = 0.4f,
     float freq_wt = 1.0f, float trg_freq = .15f, float dist_off = 0.0f);
  // #CAT_SemVecGen #MENU_BUTTON #MENU_ON_SemVecGen generate semantic vector patterns in destination column of dest datatable (which must exist and is used for starting state -- duplicate prior to calling if you want to save it) by flipping individual bits in the patterns.  Bits are chosen according to a softmax function weighted by the sum of distance * other rep's bits -- bits are flipped in on/off pairs to ensure that kwta constraint is preserved (assumes binary 1/0 inputs).   toward = flip to increase similarity of closest patterns, else flip weights to decrease similarity of most dissimilar patterns.  use_k_thr = only focus on the top/bottom k closest items (as represented in dist_kmat table -- top for toward, bottom for away) -- k_val is the number of items in that top / bottom k -- needed for rescaling freq_wt.  p_flip_items = proportion of items that get their bits flipped -- this should be as small as possible (such that one flip occurs) toward the end of the process, but can be higher earlier on.  same_wt = weight when two patterns have same bit, diff_wt = weight when two patterns have diff bit.  softmax_gain = multiplier on softmax -- 0 = uniform probability, higher values = use weighted probabilities more.  freq_weight and bit_freqs drive constraint toward keeping bits relatively uniform in frequency across patterns -- penalizes turning on bits that are already high frequency, and turning off ones that are already low frequency, compared to target frequency trg_freq.  dist_off is an offset to subtract from all the target distances 

  static bool	SemVecGen_FlipStats
    (DataTable* dest, Variant dest_name_col, Variant dest_vec_col,
     DataTable* trg_dist_mat, DataTable* lrn_stats, DataTable* dest_dist, 
     float dist_off, float softmax_gain = 0.4f, float p_flip_items = .01f,
     taMath::DistMetric metric = taMath::INNER_PROD, bool norm=true, float tol=0.0f);
  // #CAT_SemVecGen #MENU_BUTTON #MENU_ON_SemVecGen compute stats for the learning process -- distance between dist matrix of dest and the target dist matrix, and also record relevant parmeters in use at the time, to trace overall adaptation strategy -- distance metrics should match original and generally should be normalized inner product

  TA_BASEFUNS_NOCOPY(taDataGenSem);
private:
  void Initialize()  { };
  void Destroy()     { };
};

#endif // taDataGenSem_h

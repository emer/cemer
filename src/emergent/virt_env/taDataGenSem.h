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

  static float 	SemVecGen_DistMatDist(DataTable* dist_mat, DataTable* dist_mat2,
				      DataTable* delta_mat = NULL);
  // #CAT_SemVecGen utility function: compute normalized SS dist between two distance matricies -- if delta_mat is non-null, it records the deltas between the two matricies (must have same format)

  static bool	SemVecGenGA(DataTable* dest, Variant dest_name_col, Variant dest_vec_col,
			    DataTable* dist_mat, DataTable* ga_stats,
			    DataTable_Group* ga_pop_group, DataTable_Group* ga_dist_group,
			    bool resume = false,
			    int pop_size = 100, int n_gens = 10, float pct_mate = 0.1f, 
			    float prob_loser_mate = 0.0f, bool rnd_crossover = false,
			    float pct_act = 0.25f, float pct_mutate = 0.01f,
			    taMath::DistMetric metric = taMath::INNER_PROD, bool norm=true,
			    float tol=0.0f);
  // #CAT_SemVecGen #MENU_BUTTON #MENU_ON_SemVecGen generate semantic vector patterns in destination column using a genetic algorithm (GA) such that the results fit the pairwise distances represented in the distance matrix dist_mat (this should be a non-matrix form with N rows and N columns of distances, and the first column is the name).  resume = pick up where last run left off.  pop_size = population of vectors to use -- each "individual" is a DataTable (stored in ga_pop_group data group) with N rows of feature vectors modeled after dest & dest_col, names from dist_mat.  n_gens = number of generations to run.  pct_act = proportion activity in feature vectors (which obey consistent kwta constraint).  pct_mate = proportion of top-scoring individuals that get to mate.  prob_loser_mate = probability that one parent is instead chosen from the "loser" population (at random).  rnd_crossover = amount of crossover is random from 0 to 50% (otherwise always just 50% exactly from each parent).  pct_mutate = probability of mutating any random bit in offspring.  top individual is always retained.  distance metrics must match what was used in the original dist_mat table!
    static void	SemVecGenGA_Mate(DataTable* kid, const String& dest_col, 
				 DataTable* par1, DataTable* par2,
				 int bits_on, int mutate_k, bool rnd_crossover);
    // #CAT_SemVecGen mate two tables to create a new offspring

  static void 	SemVecGen_GetDistTopK(DataTable* dist_mat, int k_n, DataTable* dist_topk);
  // #CAT_SemVecGen utility function: get a kval on the distances, which is distance value such that k_n items are >= kval -- these are ones that network should make as similar as possible, while others are made dissimilar

  static void 	SemVecGen_GetDistBotK(DataTable* dist_mat, int k_n, DataTable* dist_botk);
  // #CAT_SemVecGen utility function: get a kval on the distances, which is distance value such that k_n items are <= kval -- these are ones that network should make as similar as possible, while others are made dissimilar

  static bool	SemVecGenLearn_Grad(DataTable* dest, DataTable* dest_tmp,
				    Variant dest_name_col, Variant dest_vec_col,
				    DataTable* dist_mat, DataTable* dist_topk,
				    bool topk_thr = true,
				    float lrate = 0.001f, float dist_pow = 1.0f,
				    float noise_var = 0.005f, float p_learn = 1.0f);
  // #CAT_SemVecGen #MENU_BUTTON #MENU_ON_SemVecGen generate semantic vector patterns in destination column of dest datatable (which must exist and is used for starting state -- duplicate prior to calling if you want to save it) using gradient-based learning, such that the results fit the pairwise distances represented in the distance matrix dist_mat (this should be normalized inner product (cosine) in a non-matrix form with N rows and N columns of distances, and the first column is the name).  algorithm moves bit patterns closer to each other in proportion to their target distances, using a subtract-the-average normalization scheme.  learning is on-line (works best).  lrate = learning rate -- best value depends on other factors -- use stats to dial.  dist_pow = exponent (power) applied to the distance values -- higher powers produce greater differential focus on higher (closer) distance values.  noise_var = variance on additive gaussian noise value.  p_learn = proportion of items that are learned (unlike the _Flip algo, this doesn't seem to be that useful here)

  static bool	SemVecGenLearn_Bin(DataTable* dest, Variant dest_name_col,
				   Variant dest_vec_col, DataTable* dist_mat,
				   float pct_act = 0.25f,  float lrate = 1.0f,
				   float min_val = -1.0f, float max_val = 1.0f);
  // #CAT_SemVecGen #MENU_BUTTON #MENU_ON_SemVecGen binarize the semantic vector patterns in destination column of dest datatable (which must exist and is used for starting state -- duplicate prior to calling if you want to save it) -- imposes kwta constraint with pct_act, and lrate < 1 only goes part of the way toward the fully binarized state.  lrate = learning rate.  distance metrics must match what was used in the original dist_mat table!

  static bool	SemVecGenLearn_GradStats(DataTable* dest, Variant dest_name_col,
			     Variant dest_vec_col,
			     DataTable* dist_mat, DataTable* lrn_stats,
			     DataTable* dest_dist, 
			     float lrate = 0.001f, float dist_pow = 1.0f,
			     float noise_var = 0.01f, 
 			     taMath::DistMetric metric = taMath::INNER_PROD, bool norm=true,
			     float tol=0.0f);
  // #CAT_SemVecGen #MENU_BUTTON #MENU_ON_SemVecGen compute stats for the learning process -- distance between dist matrix of dest and the target dist matrix, and also record relevant parmeters in use at the time, to trace overall adaptation strategy -- distance metrics should match original and generally should be normalized inner product

  static bool	SemVecGenLearn_FlipOn(DataTable* dest, DataTable* dest_tmp,
				    Variant dest_name_col, Variant dest_vec_col,
				    DataTable* dist_mat, DataTable* dist_topk,
				    bool topk_thr = true,
				    float softmax_gain = 0.4f, float p_flip_bit = 0.01f);
  // #CAT_SemVecGen #MENU_BUTTON #MENU_ON_SemVecGen generate semantic vector patterns in destination column of dest datatable (which must exist and is used for starting state -- duplicate prior to calling if you want to save it) by flipping individual bits in the patterns.  Bits are chosen according to a softmax function weighted by the sum of distance * other rep's bits -- bits are flipped in on/off pairs to ensure that kwta constraint is preserved.  this is much better at converging all the way relative to the _Grad/_Bin combination.  topk_thr = only focus on the top k closest items (as represented in dist_topk guy).  softmax_gain = multiplier on softmax -- 0 = uniform probability, higher values = use weighted probabilities more.  p_flip_bit = proportion of items that get their bits flipped -- this should be as small as possible (such that one flip occurs) toward the end of the process, but can be higher earlier on

  static bool	SemVecGenLearn_FlipOff(DataTable* dest, DataTable* dest_tmp,
				    Variant dest_name_col, Variant dest_vec_col,
				    DataTable* dist_mat, DataTable* dist_botk,
				    bool botk_thr = true,
				    float softmax_gain = 0.4f, float p_flip_bit = 0.01f);
  // #CAT_SemVecGen #MENU_BUTTON #MENU_ON_SemVecGen generate semantic vector patterns in destination column of dest datatable (which must exist and is used for starting state -- duplicate prior to calling if you want to save it) by flipping individual bits in the patterns.  Bits are chosen according to a softmax function weighted by the sum of distance * other rep's bits -- bits are flipped in on/off pairs to ensure that kwta constraint is preserved.  this is much better at converging all the way relative to the _Grad/_Bin combination.  topk_thr = only focus on the top k closest items (as represented in dist_topk guy).  softmax_gain = multiplier on softmax -- 0 = uniform probability, higher values = use weighted probabilities more.  p_flip_bit = proportion of items that get their bits flipped -- this should be as small as possible (such that one flip occurs) toward the end of the process, but can be higher earlier on

  static bool	SemVecGenLearn_FlipStats(DataTable* dest, Variant dest_name_col,
			     Variant dest_vec_col,
			     DataTable* dist_mat, DataTable* lrn_stats,
			     DataTable* dest_dist, 
			     float softmax_gain = 0.4f, float p_flip_bit = .01f,
			     taMath::DistMetric metric = taMath::INNER_PROD, bool norm=true,
			     float tol=0.0f);
  // #CAT_SemVecGen #MENU_BUTTON #MENU_ON_SemVecGen compute stats for the learning process -- distance between dist matrix of dest and the target dist matrix, and also record relevant parmeters in use at the time, to trace overall adaptation strategy -- distance metrics should match original and generally should be normalized inner product

  TA_BASEFUNS_NOCOPY(taDataGenSem);
private:
  void Initialize()  { };
  void Destroy()     { };
};

#endif // taDataGenSem_h

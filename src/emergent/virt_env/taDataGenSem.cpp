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

#include "taDataGenSem.h"

#include <DataTable>
#include <DataTable_Group>
#include <float_Matrix>
#include <taMath_float>
#include <float_Array>
#include <Random>
#include <taDataAnal>
#include <css_machine.h>

#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(taDataGenSem);

bool taDataGenSem::SemVecGenGA(DataTable* dest, Variant dest_name_col, Variant dest_vec_col,
			       DataTable* dist_mat, DataTable* ga_stats,
			       DataTable_Group* ga_pop_group, DataTable_Group* ga_dist_group,
			       bool resume,
			       int pop_size, int n_gens, float pct_mate, float prob_loser_mate,
			       bool rnd_crossover, float pct_act, float pct_mutate,
			       taMath::DistMetric metric, bool norm, float tol) {
  if(!dest) {
    taMisc::Error("taDataGenSem::SemVecGenGA dest is NULL -- must exist and be properly formatted");
    return false;
  }
  if(!dist_mat) {
    taMisc::Error("taDataGenSem::SemVecGenGA dist_mat is NULL -- must exist and be properly formatted");
    return false;
  }
  if(!ga_stats) {
    taMisc::Error("taDataGenSem::SemVecGenGA ga_stats is NULL -- must exist");
    return false;
  }
  if(!ga_pop_group) {
    taMisc::Error("taDataGenSem::SemVecGenGA ga_pop_group is NULL -- must exist");
    return false;
  }
  if(!ga_dist_group) {
    taMisc::Error("taDataGenSem::SemVecGenGA ga_dist_group is NULL -- must exist");
    return false;
  }

  DataCol* dest_nm_da = dest->GetColData(dest_name_col); // gives err
  if(!dest_nm_da) return false;
  DataCol* dest_vec_da = dest->GetColData(dest_vec_col); // gives err
  if(!dest_vec_da) return false;

  int vec_bits = dest_vec_da->cell_size();
  int bits_on = (int)((float)vec_bits * pct_act + .5f);
  if(bits_on < 1) bits_on = 1;

  int mutate_k = (int)((float)vec_bits * pct_mutate + .5f);
  if(pct_mutate > 0.0f)
    if(mutate_k < 1) mutate_k = 1;

  int top_k = (int)((float)pop_size * pct_mate + .5f);
  if(top_k < 2) top_k = 2;

  DataCol* dist_nms_col = dist_mat->data.SafeEl(0);
  if(!dist_nms_col) {
    taMisc::Error("taDataGenSem::SemVecGenGA dist_mat does not have names column as first col");
    return false;
  }

  int n_vecs = dist_mat->rows;	// number of vectors..

  // configure the stats guy

  ga_stats->StructUpdate(true);
  ga_stats->ResetData();
  int idx;
  DataCol* gen_da = ga_stats->FindMakeColName("gen", idx, VT_INT);
  DataCol* best_da = ga_stats->FindMakeColName("best_err", idx, VT_FLOAT);
  DataCol* thresh_da = ga_stats->FindMakeColName("thresh_err", idx, VT_FLOAT);
  DataCol* worst_da = ga_stats->FindMakeColName("worst_err", idx, VT_FLOAT);
  ga_stats->StructUpdate(false);

  // first create random initial individuals
  if(!resume) {
    ga_pop_group->Reset();	// nuke any existing
    DataTable_Group* pop_gp0 = (DataTable_Group*)ga_pop_group->NewGp(1);
    DataTable_Group* pop_gp1 = (DataTable_Group*)ga_pop_group->NewGp(1);
    for(int i=0;i<pop_size;i++) {
      DataTable* indv = pop_gp0->NewEl(1);
      indv->StructUpdate(true);
      indv->Copy_NoData(*dest);		// give it same structure
      // actually do want to save population for possible redo
//       indv->ClearDataFlag(DataTable::SAVE_ROWS);
      indv->EnforceRows(n_vecs);
      DataCol* dan = indv->GetColData(dest_name_col);
      dan->AR()->CopyFrom(dist_nms_col->AR()); // copy names over
      PermutedBinary(indv, dest_vec_da->name, bits_on); // make random patterns
      indv->StructUpdate(false);
    }
  }

  // next create distance matricies
  ga_dist_group->Reset();	// nuke any existing
  for(int i=0;i<pop_size;i++) {
    DataTable* indv = ga_dist_group->NewEl(1);
    indv->StructUpdate(true);
    indv->ClearDataFlag(DataTable::SAVE_ROWS);
    indv->Copy_NoData(*dist_mat);		// give it same structure
    indv->StructUpdate(false);
  }
  // distances for current generation
  DataTable* ga_gen_dists = ga_dist_group->NewEl(1);
  ga_gen_dists->StructUpdate(true);
  ga_gen_dists->NewCol(VT_INT, "indiv");	// individual index in group
  ga_gen_dists->NewCol(VT_FLOAT, "ga_err");	// gen algo err measure for this guy
  ga_gen_dists->EnforceRows(pop_size);
  ga_gen_dists->StructUpdate(false);

  float ss_norm = 1.0f / (float)(n_vecs * n_vecs);

  DataTable* last_best = NULL;

  // now start cranking
  for(int gen=0; gen < n_gens; gen++) {

    int pop_gp_src_idx = gen % 2;	  // what is source group index
    int pop_gp_dst_idx = (gen+1) % 2; // what is dest group index

    DataTable_Group* pop_gp_src = (DataTable_Group*)ga_pop_group->gp[pop_gp_src_idx];
    DataTable_Group* pop_gp_dst = (DataTable_Group*)ga_pop_group->gp[pop_gp_dst_idx];

    ga_gen_dists->data[0]->InitValsToRowNo();
    // compute distance matricies of individuals
    for(int i=0;i<pop_size;i++) {
      DataTable* indv = pop_gp_src->FastEl(i);
      DataTable* idist = ga_dist_group->FastEl(i);
      // generate individual's distance metric
      taDataAnal::DistMatrixTable(idist, false, indv, dest_vec_da->name, dest_nm_da->name,
				  metric, norm, tol);
      float ss_dist = 0.0f;
      // now add up the ss-distances between the distance table and the original distance table
      for(int row=0; row < n_vecs; row++) {
	for(int col=1; col < n_vecs+1; col++) {
	  float org_dist = dist_mat->GetValAsFloat(col, row);
	  float indv_dist = idist->GetValAsFloat(col, row);
	  float dst = org_dist - indv_dist;
	  ss_dist += dst * dst;
	}
      }
      ss_dist *= ss_norm;	// normalize by n
      ga_gen_dists->SetValAsFloat(ss_dist, "ga_err", i);	// gen algo err measure for this guy
      taMisc::RunPending();
      if(cssMisc::cur_top && cssMisc::cur_top->external_stop)
	return false;
    }

    // find the top guys
    ga_gen_dists->Sort(1, true); // sort by distance (col 1), ascending = smaller first
    pop_gp_dst->Reset();	 // nuke the dest group
    for(int i=0;i<pop_size-1;i++) { // make kiddos
      int par1_dx = Random::IntZeroN(top_k);
      int par2_dx = Random::IntZeroN(top_k);
      if(Random::BoolProb(prob_loser_mate)) {
	par2_dx = Random::IntMinMax(top_k, pop_size); // pick a loser, any loser..
      }
      while (par2_dx == par1_dx) par2_dx = Random::IntZeroN(top_k); // can't be same..
      DataTable* par1 = pop_gp_src->FastEl(ga_gen_dists->GetValAsInt(0, par1_dx));
      DataTable* par2 = pop_gp_src->FastEl(ga_gen_dists->GetValAsInt(0, par2_dx));
      DataTable* kid = pop_gp_dst->NewEl(1);
//       kid->ClearDataFlag(DataTable::SAVE_ROWS);
      kid->name = "kid_" + String(i) + "_gen_" + String(gen);
      SemVecGenGA_Mate(kid, dest_vec_da->name, par1, par2, bits_on, mutate_k, rnd_crossover);
    }
    last_best = pop_gp_src->FastEl(ga_gen_dists->GetValAsInt(0, 0)); // best guy
    pop_gp_dst->Transfer(last_best);						  // grab it!
    last_best->name = "last_best_gen_" + String(gen);

    float best_err = ga_gen_dists->GetValAsFloat(1, 0);
    float thresh_err = ga_gen_dists->GetValAsFloat(1, top_k);
    float worst_err = ga_gen_dists->GetValAsFloat(1, pop_size-1);
    ga_stats->AddBlankRow();
    ga_stats->SetValAsInt(gen, "gen", -1);
    ga_stats->SetValAsFloat(best_err, "best_err", -1);
    ga_stats->SetValAsFloat(thresh_err, "thresh_err", -1);
    ga_stats->SetValAsFloat(worst_err, "worst_err", -1);
    ga_stats->WriteClose();
    taMisc::RunPending();
    if(cssMisc::cur_top && cssMisc::cur_top->external_stop)
      return false;
  }

  if(last_best) {
    dest->CopyFrom(last_best);	// grab the last best guy!
  }

  return true;
}

void taDataGenSem::SemVecGenGA_Mate(DataTable* kid, const String& dest_col, 
				    DataTable* par1, DataTable* par2, int bits_on,
				    int mutate_k, bool rnd_crossover) {
  kid->StructUpdate(true);
  kid->CopyFrom(par1);		// first just imitate first parent

  // indicies of the on and off bits
  int_Array on_bits;	// bits that are on in kid (and off in 2nd parent for first case)
  int_Array off_bits;	// bits that are off in kid and on in 2nd parent

  for(int i=0; i<kid->rows; i++) {
    float_Matrix* kmat = (float_Matrix*)kid->GetValAsMatrix(dest_col, i);
    taBase::Ref(kmat);
    float_Matrix* par2mat = (float_Matrix*)par2->GetValAsMatrix(dest_col, i);
    taBase::Ref(par2mat);

    // first do the crossover, using uniform 1/2 mixing from each parent
    on_bits.Reset();
    off_bits.Reset();
    for(int j=0;j<kmat->size;j++) {
      if(kmat->FastEl_Flat(j) > .5f && par2mat->FastEl_Flat(j) < .5f)
	on_bits.Add(j);
      if(kmat->FastEl_Flat(j) < .5f && par2mat->FastEl_Flat(j) > .5f)
	off_bits.Add(j);
    }
    on_bits.Permute();
    off_bits.Permute();
    int mx = MIN(on_bits.size, off_bits.size);
    int n_cross = mx / 2;
    if(rnd_crossover) {
      n_cross = Random::IntZeroN(n_cross+1); // inclusive of n_cross..
    }
    for(int j=0; j<n_cross; j++) {
      kmat->FastEl_Flat(on_bits[j]) = 0.0f; // turn off
      kmat->FastEl_Flat(off_bits[j]) = 1.0f; // turn on
    }

    // then randomly flip some bits!
    on_bits.Reset();	// now 
    off_bits.Reset();
    for(int j=0;j<kmat->size;j++) {
      if(kmat->FastEl_Flat(j) > .5f)
	on_bits.Add(j);
      if(kmat->FastEl_Flat(j) < .5f)
	off_bits.Add(j);
    }
    on_bits.Permute();
    off_bits.Permute();
    mx = MIN(on_bits.size, off_bits.size); // should be bits_on
    mx = MIN(mx, mutate_k);
    for(int j=0; j<mx; j++) {
      kmat->FastEl_Flat(on_bits[j]) = 0.0f; // turn off
      kmat->FastEl_Flat(off_bits[j]) = 1.0f; // turn on
    }

    taBase::unRefDone(kmat);
    taBase::unRefDone(par2mat);
  }
  kid->StructUpdate(false);
}

//////////////////////////////////////////////////////////
//		Gradient Learning

float taDataGenSem::SemVecGen_DistMatDist(DataTable* dist_mat, DataTable* dist_mat2,
					  DataTable* delta_mat) {
  int n_vecs = dist_mat->rows;	// number of vectors..
  float ss_norm = 1.0f / (float)(n_vecs * n_vecs);
  float ss_dist = 0.0f;
  // now add up the ss-distances between the distance table and the original distance table
  for(int row=0; row < n_vecs; row++) {
    for(int col=1; col < n_vecs+1; col++) {
      float org_dist = dist_mat->GetValAsFloat(col, row);
      float indv_dist = dist_mat2->GetValAsFloat(col, row);
      float dst = org_dist - indv_dist;
      if(delta_mat) {
	delta_mat->SetValAsFloat(dst, col, row);
      }
      ss_dist += dst * dst;
    }
  }
  return ss_dist * ss_norm;
}

bool taDataGenSem::SemVecGenLearn_Grad(DataTable* dest, DataTable* dest_tmp,
				       Variant dest_name_col, Variant dest_vec_col,
				       DataTable* dist_mat, DataTable* dist_topk,
				       bool topk_thr, float lrate, float dist_pow,
				       float noise_var, float p_flip_bit) {
  if(!dest) {
    taMisc::Error("taDataGenSem::SemVecGenLearn_Grad dest is NULL -- must exist and be properly formatted");
    return false;
  }
  if(!dest_tmp) {
    taMisc::Error("taDataGenSem::SemVecGenLearn_Grad dest_tmp is NULL -- must exist and be properly formatted");
    return false;
  }
  if(!dist_mat) {
    taMisc::Error("taDataGenSem::SemVecGenLearn_Grad dist_mat is NULL -- must exist and be properly formatted");
    return false;
  }
  if(!dist_topk) {
    taMisc::Error("taDataGenSem::SemVecGenLearn_Grad dist_topk is NULL -- must exist and be properly formatted");
    return false;
  }

  DataCol* dest_nm_da = dest->GetColData(dest_name_col); // gives err
  if(!dest_nm_da) return false;
  DataCol* dest_vec_da = dest->GetColData(dest_vec_col); // gives err
  if(!dest_vec_da) return false;

  DataCol* dest_tmp_vec_da = dest_tmp->GetColData(dest_vec_col); // gives err
  if(!dest_tmp_vec_da) return false;

  DataCol* topk_da = dist_topk->GetColData("kval"); // gives err
  if(!topk_da) return false;

  dest_tmp_vec_da->InitVals(0.0f);

  int vec_bits = dest_vec_da->cell_size();

  // effective lrate to normalize for effect of dist_pow -- .4 is general "topk" dist guy
  float lrate_eff = lrate * (0.4f / taMath_float::pow(0.4f, dist_pow));

  int n_flip_bit = (int)(p_flip_bit * (float)dest->rows + .5f);
  if(n_flip_bit < 1) n_flip_bit = 1;

  static int_Array sorder;
  if(sorder.size != dest->rows) {
    sorder.SetSize(dest->rows);
    sorder.FillSeq();
  }
  sorder.Permute();

  static int_Array oorder;
  if(oorder.size != dest->rows) {
    oorder.SetSize(dest->rows);
    oorder.FillSeq();
  }

  static float_Array chg_tmp;
  if(chg_tmp.size != vec_bits) {
    chg_tmp.SetSize(vec_bits);
  }

  // turns out that having everyone move toward each other at the same time is a BAD 
  // idea -- just do it incrementally so the moves build on each other -- like all online lrning

  // algo that works the best reliably is the SUB_AVG

// 	float chg = schg + sbit * obit * dist_mult + Random::Gauss(noise_var);
//  	float chg = schg + -sbit * obit * dist_mult + Random::Gauss(noise_var);
// definitely works the best to have JUST the other guy in chg, NOT sbit or -sbit
// raw signal!

  for(int sidx=0;sidx < n_flip_bit; sidx++) {
    int srow = sorder[sidx];
    float dkval = topk_da->GetValAsFloat(srow);

    // everyone starts out with same raw numbers
    for(int oidx=0;oidx < dest->rows; oidx++) {
      int orow = oorder[oidx];
      if(orow == srow) continue;			    // skip self!
      float dist = dist_mat->GetValAsFloat(orow + 1, srow); // col, row -- assume 1 offset
      float dist_mult = 0.0f;
      if(topk_thr) {
	if(dist >= dkval)
	  dist_mult = lrate_eff * dist; // only if over threshold
	else
	  continue;		// all zero -- just bail
      }
      else {
	dist_mult = lrate_eff * taMath_float::pow(MAX(dist, 0.0f), dist_pow);
      }
      float on_avg = 0.0f;
      float off_avg = 0.0f;
      float on_n = 0.0f;
      float off_n = 0.0f;
      for(int k=0;k<vec_bits;k++) {
	float sbit = dest_vec_da->GetValAsFloatM(srow, k);
	float obit = dest_vec_da->GetValAsFloatM(orow, k);
  	float chg = obit * dist_mult + Random::Gauss(noise_var);
	chg_tmp.FastEl(k) = chg;
	if(sbit > 0.0f) {
	  on_avg += chg;
	  on_n += 1.0f;
	}
	else {
	  off_avg += chg;
	  off_n += 1.0f;
	}
      }
      if(on_n > 0.0f)
	on_avg /= on_n;
      if(off_n > 0.0f)
	off_avg /= off_n;
      for(int k=0;k<vec_bits;k++) {
	float sbit = dest_vec_da->GetValAsFloatM(srow, k);
	float chg = chg_tmp.FastEl(k);
	float nwval;
	if(sbit > 0.0f) {
	  nwval = sbit + (chg - on_avg);
	}
	else {
	  nwval = sbit + (chg - off_avg);
	}
	if(nwval > 1.0f)	nwval = 1.0f;
	if(nwval < -1.0f)	nwval = -1.0f;
	dest_vec_da->SetValAsFloatM(nwval, srow, k);
      }
    }
  }
  return true;
}

bool taDataGenSem::SemVecGenLearn_Bin(DataTable* dest, Variant dest_name_col,
				      Variant dest_vec_col, DataTable* dist_mat,
				      float pct_act, float lrate,
				      float min_val, float max_val) {
  if(!dest) {
    taMisc::Error("taDataGenSem::SemVecGenLearn_Bin dest is NULL -- must exist and be properly formatted");
    return false;
  }
  if(!dist_mat) {
    taMisc::Error("taDataGenSem::SemVecGenLearn_Bin dist_mat is NULL -- must exist and be properly formatted");
    return false;
  }

  DataCol* dest_nm_da = dest->GetColData(dest_name_col); // gives err
  if(!dest_nm_da) return false;
  DataCol* dest_vec_da = dest->GetColData(dest_vec_col); // gives err
  if(!dest_vec_da) return false;

  int vec_bits = dest_vec_da->cell_size();
  int bits_on = (int)((float)vec_bits * pct_act + .5f);
  if(bits_on < 1) bits_on = 1;

  for(int srow=0;srow < dest->rows; srow++) {
    float_Matrix* kmat = (float_Matrix*)dest_vec_da->GetValAsMatrix(srow);
    taBase::Ref(kmat);
    float kval = taMath_float::vec_kwta(kmat, bits_on, true);
    for(int k=0;k<vec_bits;k++) {
      float sbit = dest_vec_da->GetValAsFloatM(srow, k);
      float nwtrg;
      if(sbit >= kval)	nwtrg = max_val;
      else nwtrg = min_val;
      float nwval = sbit + lrate * (nwtrg - sbit);
      dest_vec_da->SetValAsFloatM(nwval, srow, k);
    }
    taBase::unRefDone(kmat);
  }
  return true;
}

void taDataGenSem::SemVecGen_GetDistTopK(DataTable* dist_mat, int k_n, DataTable* dist_topk) {
  dist_topk->StructUpdate(true);
  int idx;
  dist_topk->FindMakeColName("kval", idx, VT_FLOAT);
  int n_vecs = dist_mat->rows;	// number of vectors..
  float_Matrix kmat;
  kmat.SetGeom(1, n_vecs);
  for(int row=0; row < n_vecs; row++) {
    for(int col=1; col < n_vecs+1; col++) {
      float org_dist = dist_mat->GetValAsFloat(col, row);
      kmat.Set_Flat(org_dist, col);
    }
    float kval = taMath_float::vec_kwta(&kmat, k_n, true);
    dist_topk->SetValAsFloat(kval, "kval", row);
  }
  dist_topk->StructUpdate(false);
}

void taDataGenSem::SemVecGen_GetDistBotK(DataTable* dist_mat, int k_n, DataTable* dist_botk) {
  dist_botk->StructUpdate(true);
  int idx;
  dist_botk->FindMakeColName("kval", idx, VT_FLOAT);
  int n_vecs = dist_mat->rows;	// number of vectors..
  float_Matrix kmat;
  kmat.SetGeom(1, n_vecs);
  for(int row=0; row < n_vecs; row++) {
    for(int col=1; col < n_vecs+1; col++) {
      float org_dist = dist_mat->GetValAsFloat(col, row);
      kmat.Set_Flat(org_dist, col);
    }
    float kval = taMath_float::vec_kwta(&kmat, k_n, false); // descending
    dist_botk->SetValAsFloat(kval, "kval", row);
  }
  dist_botk->StructUpdate(false);
}

bool taDataGenSem::SemVecGenLearn_FlipOn(DataTable* dest, DataTable* dest_tmp,
				       Variant dest_name_col, Variant dest_vec_col,
				       DataTable* dist_mat, DataTable* dist_topk,
				       bool topk_thr, float softmax_gain, float p_flip_bit) {
  if(!dest) {
    taMisc::Error("taDataGenSem::SemVecGenLearn_Grad dest is NULL -- must exist and be properly formatted");
    return false;
  }
  if(!dest_tmp) {
    taMisc::Error("taDataGenSem::SemVecGenLearn_Grad dest_tmp is NULL -- must exist and be properly formatted");
    return false;
  }
  if(!dist_mat) {
    taMisc::Error("taDataGenSem::SemVecGenLearn_Grad dist_mat is NULL -- must exist and be properly formatted");
    return false;
  }
  if(!dist_topk) {
    taMisc::Error("taDataGenSem::SemVecGenLearn_Grad dist_topk is NULL -- must exist and be properly formatted");
    return false;
  }

  DataCol* dest_nm_da = dest->GetColData(dest_name_col); // gives err
  if(!dest_nm_da) return false;
  DataCol* dest_vec_da = dest->GetColData(dest_vec_col); // gives err
  if(!dest_vec_da) return false;

  DataCol* dest_tmp_vec_da = dest_tmp->GetColData(dest_vec_col); // gives err
  if(!dest_tmp_vec_da) return false;

  DataCol* topk_da = dist_topk->GetColData("kval"); // gives err
  if(!topk_da) return false;

  dest_tmp_vec_da->InitVals(0.0f);

  int vec_bits = dest_vec_da->cell_size();

  int n_flip_bit = (int)(p_flip_bit * (float)dest->rows + .5f);
  if(n_flip_bit < 1) n_flip_bit = 1;

  static int_Array sorder;
  if(sorder.size != dest->rows) {
    sorder.SetSize(dest->rows);
    sorder.FillSeq();
  }
  sorder.Permute();
  // first gather probability votes for flipping bits
  for(int sidx=0;sidx < n_flip_bit; sidx++) {
    int srow = sorder[sidx];
    float dkval = topk_da->GetValAsFloat(srow);
    for(int orow=0;orow < dest->rows; orow++) {
      if(orow == srow) continue;			    // skip self!
      float dist = dist_mat->GetValAsFloat(orow + 1, srow); // col, row -- assume 1 offset
      if(topk_thr && (dist < dkval)) continue;		     // don't do anything for non-top-k guys
      for(int k=0;k<vec_bits;k++) {
	float schg = dest_tmp_vec_da->GetValAsFloatM(srow, k);
	float sbit = dest_vec_da->GetValAsFloatM(srow, k);
	float obit = dest_vec_da->GetValAsFloatM(orow, k);
	float chg;
	if(sbit > 0.0f)	// if our bit is on, see if we should turn it off
	  chg = -dist * obit;	// if obit is -1, we get +1 vote for off, else -1 vote for off
	else
	  chg = dist * obit;	// if obit is +1 we want to turn it on, else keep off
	float nwval = schg + chg;
	dest_tmp_vec_da->SetValAsFloatM(nwval, srow, k);
      }
    }
  }

  // then compute softmax on flip probs and actually flip a bit
  for(int sidx=0;sidx < n_flip_bit; sidx++) {
    int srow = sorder[sidx];
    float_Matrix* kmat = (float_Matrix*)dest_tmp_vec_da->GetValAsMatrix(srow);
    taBase::Ref(kmat);
    float flip_on_sum = 0.0f;
    float flip_off_sum = 0.0f;
    for(int k=0;k<vec_bits;k++) {
      float& el = kmat->FastEl_Flat(k);
      // this turns out to be very bad -- nuke!!
//       if(el <= 0.0f) {
// 	el = 0.0f;
// 	continue;
//       }
      el = expf(softmax_gain * MAX(el, 0.0f));
      float sbit = dest_vec_da->GetValAsFloatM(srow, k);
      if(sbit > 0.0f)
	flip_off_sum += el;
      else
	flip_on_sum += el;
    }
    float pon = Random::ZeroOne();
    float poff = Random::ZeroOne();
    float sum_on_p = 0.0f;
    float sum_off_p = 0.0f;
    int bit_on = -1;
    int bit_off = -1;
    for(int k=0;k<vec_bits;k++) {
      float& el = kmat->FastEl_Flat(k);
      float sbit = dest_vec_da->GetValAsFloatM(srow, k);
      if(sbit > 0.0f) {
	if(bit_off < 0) {
	  float pel = el / flip_off_sum;
	  sum_off_p += pel;
	  if(sum_off_p >= poff) {
	    bit_off = k;
	  }
	}
      }
      else {
	if(bit_on < 0) {
	  float pel = el / flip_on_sum;
	  sum_on_p += pel;
	  if(sum_on_p >= pon) {
	    bit_on = k;
	  }
	}
      }
    }
    if(bit_on >= 0 && bit_off >= 0) {
      // flip bits!
      dest_vec_da->SetValAsFloatM(-1.0f, srow, bit_off);
      dest_vec_da->SetValAsFloatM(1.0f, srow, bit_on);
    }

    taBase::unRefDone(kmat);
  }
  return true;
}

bool taDataGenSem::SemVecGenLearn_FlipOff(DataTable* dest, DataTable* dest_tmp,
			       Variant dest_name_col, Variant dest_vec_col,
			       DataTable* dist_mat, DataTable* dist_botk,
			       bool botk_thr, float softmax_gain, float p_flip_bit) {
  if(!dest) {
    taMisc::Error("taDataGenSem::SemVecGenLearn_Grad dest is NULL -- must exist and be properly formatted");
    return false;
  }
  if(!dest_tmp) {
    taMisc::Error("taDataGenSem::SemVecGenLearn_Grad dest_tmp is NULL -- must exist and be properly formatted");
    return false;
  }
  if(!dist_mat) {
    taMisc::Error("taDataGenSem::SemVecGenLearn_Grad dist_mat is NULL -- must exist and be properly formatted");
    return false;
  }
  if(!dist_botk) {
    taMisc::Error("taDataGenSem::SemVecGenLearn_Grad dist_botk is NULL -- must exist and be properly formatted");
    return false;
  }

  DataCol* dest_nm_da = dest->GetColData(dest_name_col); // gives err
  if(!dest_nm_da) return false;
  DataCol* dest_vec_da = dest->GetColData(dest_vec_col); // gives err
  if(!dest_vec_da) return false;

  DataCol* dest_tmp_vec_da = dest_tmp->GetColData(dest_vec_col); // gives err
  if(!dest_tmp_vec_da) return false;

  DataCol* botk_da = dist_botk->GetColData("kval"); // gives err
  if(!botk_da) return false;

  dest_tmp_vec_da->InitVals(0.0f);

  int vec_bits = dest_vec_da->cell_size();

  int n_flip_bit = (int)(p_flip_bit * (float)dest->rows + .5f);
  if(n_flip_bit < 1) n_flip_bit = 1;

  static int_Array sorder;
  if(sorder.size != dest->rows) {
    sorder.SetSize(dest->rows);
    sorder.FillSeq();
  }
  sorder.Permute();
  // first gather probability votes for flipping bits
  for(int sidx=0;sidx < n_flip_bit; sidx++) {
    int srow = sorder[sidx];
    float dkval = botk_da->GetValAsFloat(srow);
    for(int orow=0;orow < dest->rows; orow++) {
      if(orow == srow) continue;			    // skip self!
      float dist = dist_mat->GetValAsFloat(orow + 1, srow); // col, row -- assume 1 offset
      if(botk_thr && (dist > dkval)) continue;		     // don't do anything for non-top-k guys
      float inv_dist = 1.0f - dist;
      for(int k=0;k<vec_bits;k++) {
	float schg = dest_tmp_vec_da->GetValAsFloatM(srow, k);
	float sbit = dest_vec_da->GetValAsFloatM(srow, k);
	float obit = dest_vec_da->GetValAsFloatM(orow, k);
	float chg;
	if(sbit > 0.0f)	// if our bit is on, see if we should turn it off
	  chg = inv_dist * obit; // if obit is +1, we get +1 vote for off, else -1 vote for off
	else
	  chg = -inv_dist * obit; // if obit is -1 we want to turn it on, else keep off
	float nwval = schg + chg;
	dest_tmp_vec_da->SetValAsFloatM(nwval, srow, k);
      }
    }
  }

  // then compute softmax on flip probs and actually flip a bit
  for(int sidx=0;sidx < n_flip_bit; sidx++) {
    int srow = sorder[sidx];
    float_Matrix* kmat = (float_Matrix*)dest_tmp_vec_da->GetValAsMatrix(srow);
    taBase::Ref(kmat);
    float flip_on_sum = 0.0f;
    float flip_off_sum = 0.0f;
    for(int k=0;k<vec_bits;k++) {
      float& el = kmat->FastEl_Flat(k);
      el = expf(softmax_gain * MAX(el, 0.0f));
      float sbit = dest_vec_da->GetValAsFloatM(srow, k);
      if(sbit > 0.0f)
	flip_off_sum += el;
      else
	flip_on_sum += el;
    }
    float pon = Random::ZeroOne();
    float poff = Random::ZeroOne();
    float sum_on_p = 0.0f;
    float sum_off_p = 0.0f;
    int bit_on = -1;
    int bit_off = -1;
    for(int k=0;k<vec_bits;k++) {
      float& el = kmat->FastEl_Flat(k);
      float sbit = dest_vec_da->GetValAsFloatM(srow, k);
      if(sbit > 0.0f) {
	if(bit_off < 0) {
	  float pel = el / flip_off_sum;
	  sum_off_p += pel;
	  if(sum_off_p >= poff) {
	    bit_off = k;
	  }
	}
      }
      else {
	if(bit_on < 0) {
	  float pel = el / flip_on_sum;
	  sum_on_p += pel;
	  if(sum_on_p >= pon) {
	    bit_on = k;
	  }
	}
      }
    }
    if(bit_on >= 0 && bit_off >= 0) {
      // flip bits!
      dest_vec_da->SetValAsFloatM(-1.0f, srow, bit_off);
      dest_vec_da->SetValAsFloatM(1.0f, srow, bit_on);
    }

    taBase::unRefDone(kmat);
  }
  return true;
}

bool taDataGenSem::SemVecGenLearn_GradStats(DataTable* dest, Variant dest_name_col,
			Variant dest_vec_col,
		      DataTable* dist_mat, DataTable* lrn_stats,
		      DataTable* dest_dist, 
		      float lrate, float dist_pow, float noise_var,
		      taMath::DistMetric metric, bool norm, float tol) {
  if(!dest) {
    taMisc::Error("taDataGenSem::SemVecGenLearn_Stats dest is NULL -- must exist and be properly formatted");
    return false;
  }
  if(!dist_mat) {
    taMisc::Error("taDataGenSem::SemVecGenLearn_Stats dist_mat is NULL -- must exist and be properly formatted");
    return false;
  }
  if(!lrn_stats) {
    taMisc::Error("taDataGenSem::SemVecGenLearn_Stats lrn_stats is NULL -- must exist");
    return false;
  }
  if(!dest_dist) {
    taMisc::Error("taDataGenSem::SemVecGenLearn_Stats dest_dist is NULL -- must exist");
    return false;
  }

  DataCol* dest_nm_da = dest->GetColData(dest_name_col); // gives err
  if(!dest_nm_da) return false;
  DataCol* dest_vec_da = dest->GetColData(dest_vec_col); // gives err
  if(!dest_vec_da) return false;

  lrn_stats->StructUpdate(true);
  int idx;
  lrn_stats->FindMakeColName("iter", idx, VT_INT);
  lrn_stats->FindMakeColName("err", idx, VT_FLOAT);
  lrn_stats->FindMakeColName("lrate", idx, VT_FLOAT);
  lrn_stats->FindMakeColName("dist_pow", idx, VT_FLOAT);
  lrn_stats->FindMakeColName("noise_var", idx, VT_FLOAT);
  lrn_stats->StructUpdate(false);

  taDataAnal::DistMatrixTable(dest_dist, false, dest, dest_vec_da->name, dest_nm_da->name,
			      metric, norm, tol);

  float err = SemVecGen_DistMatDist(dist_mat, dest_dist);

  lrn_stats->AddBlankRow();
  lrn_stats->SetValAsInt(lrn_stats->rows, "iter", -1);
  lrn_stats->SetValAsFloat(err, "err", -1);
  lrn_stats->SetValAsFloat(lrate, "lrate", -1);
  lrn_stats->SetValAsFloat(dist_pow, "dist_pow", -1);
  lrn_stats->SetValAsFloat(noise_var, "noise_var", -1);
  lrn_stats->WriteClose();
  return true;
}

bool taDataGenSem::SemVecGenLearn_FlipStats(DataTable* dest, Variant dest_name_col,
			Variant dest_vec_col,
		      DataTable* dist_mat, DataTable* lrn_stats,
		      DataTable* dest_dist, 
		      float softmax_gain, float p_flip_bit,
		      taMath::DistMetric metric, bool norm, float tol) {
  if(!dest) {
    taMisc::Error("taDataGenSem::SemVecGenLearn_Stats dest is NULL -- must exist and be properly formatted");
    return false;
  }
  if(!dist_mat) {
    taMisc::Error("taDataGenSem::SemVecGenLearn_Stats dist_mat is NULL -- must exist and be properly formatted");
    return false;
  }
  if(!lrn_stats) {
    taMisc::Error("taDataGenSem::SemVecGenLearn_Stats lrn_stats is NULL -- must exist");
    return false;
  }
  if(!dest_dist) {
    taMisc::Error("taDataGenSem::SemVecGenLearn_Stats dest_dist is NULL -- must exist");
    return false;
  }

  DataCol* dest_nm_da = dest->GetColData(dest_name_col); // gives err
  if(!dest_nm_da) return false;
  DataCol* dest_vec_da = dest->GetColData(dest_vec_col); // gives err
  if(!dest_vec_da) return false;

  lrn_stats->StructUpdate(true);
  int idx;
  lrn_stats->FindMakeColName("iter", idx, VT_INT);
  lrn_stats->FindMakeColName("err", idx, VT_FLOAT);
  lrn_stats->FindMakeColName("softmax_gain", idx, VT_FLOAT);
  lrn_stats->FindMakeColName("p_flip_bit", idx, VT_FLOAT);
  lrn_stats->StructUpdate(false);

  taDataAnal::DistMatrixTable(dest_dist, false, dest, dest_vec_da->name, dest_nm_da->name,
			      metric, norm, tol);

  float err = SemVecGen_DistMatDist(dist_mat, dest_dist);

  lrn_stats->AddBlankRow();
  lrn_stats->SetValAsInt(lrn_stats->rows, "iter", -1);
  lrn_stats->SetValAsFloat(err, "err", -1);
  lrn_stats->SetValAsFloat(softmax_gain, "softmax_gain", -1);
  lrn_stats->SetValAsFloat(p_flip_bit, "p_flip_bit", -1);
  lrn_stats->WriteClose();
  return true;
}


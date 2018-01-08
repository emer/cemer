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

float taDataGenSem::SemVecGen_DistMatDist
(DataTable* dist_mat, DataTable* dist_mat2, DataTable* delta_mat, float dist_off) {
  bool d1_1c = (dist_mat->rows == 1); // one cell
  float_MatrixPtr d1vec;
  int n_vecs;
  if(d1_1c) {
    d1vec = (float_Matrix*)dist_mat->GetValAsMatrix(0, 0);
    n_vecs = d1vec->dim(0);
  }
  else {
    n_vecs = dist_mat->rows;
  }

  bool d2_1c = (dist_mat2->rows == 1); // one cell
  float_MatrixPtr d2vec;
  if(d2_1c) {
    d2vec = (float_Matrix*)dist_mat2->GetValAsMatrix(0, 0);
  }
  
  float ss_norm = 1.0f / (float)(n_vecs * n_vecs);
  float ss_dist = 0.0f;
  // now add up the ss-distances between the distance table and the original distance table
  for(int row=0; row < n_vecs; row++) {
    for(int col=1; col < n_vecs+1; col++) {
      float org_dist;
      if(d1_1c)
        org_dist = d1vec->FastEl2d(row, col-1);
      else
        org_dist = dist_mat->GetValAsFloat(col, row);
      org_dist -= dist_off;
      float indv_dist;
      if(d2_1c)
        indv_dist = d2vec->FastEl2d(row, col-1);
      else
        indv_dist = dist_mat2->GetValAsFloat(col, row);
      float dst = org_dist - indv_dist;
      if(delta_mat) {
	delta_mat->SetValAsFloat(dst, col, row);
      }
      ss_dist += dst * dst;
    }
  }
  return ss_dist * ss_norm;
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

bool taDataGenSem::SemVecGen_Flip
(bool toward, DataTable* dest, DataTable* work, DataTable* work_sum,
 Variant dest_name_col, Variant dest_vec_col,
 DataTable* trg_dist_mat, DataTable* dist_kmat, DataTable* bit_freqs,
 bool use_k_thr, int k_val, float p_flip_items, float same_wt, float diff_wt,
 float softmax_gain, float freq_wt, float trg_freq, float dist_off) {
  if(!dest) {
    taMisc::Error("taDataGenSem::SemVecGen_Flip dest is NULL -- must exist and be properly formatted");
    return false;
  }
  if(!work) {
    taMisc::Error("taDataGenSem::SemVecGen_Flip work is NULL -- must exist -- will be automatically formatted");
    return false;
  }
  if(!work_sum) {
    taMisc::Error("taDataGenSem::SemVecGen_Flip work_sum is NULL -- must exist -- will be automatically formatted");
    return false;
  }
  if(!trg_dist_mat) {
    taMisc::Error("taDataGenSem::SemVecGen_Flip trg_dist_mat is NULL -- must exist and be properly formatted");
    return false;
  }
  if(!dist_kmat) {
    taMisc::Error("taDataGenSem::SemVecGen_Flip dist_kmat is NULL -- must exist and be properly formatted");
    return false;
  }

  DataCol* dest_nm_da = dest->GetColData(dest_name_col); // gives err
  if(!dest_nm_da) return false;
  DataCol* dest_vec_da = dest->GetColData(dest_vec_col); // gives err
  if(!dest_vec_da) return false;

  int idx;
  DataCol* wts = work->FindMakeColMatrixN
    ("weights", VT_FLOAT, dest_vec_da->cell_geom, idx);
  wts->InitVals(0.0f);
  wts->desc = "raw weights accumulated from the distance factors, for all bits";
  DataCol* wts_on = work->FindMakeColMatrixN
    ("wts_on", VT_FLOAT, dest_vec_da->cell_geom, idx);
  wts_on->InitVals(0.0f);
  wts_on->desc = "weights after multiplication by softmax_gain and with the freq penalty, for bits that are CURRENTLY ON (for selecting one to turn OFF)";
  DataCol* wts_off = work->FindMakeColMatrixN
    ("wts_off", VT_FLOAT, dest_vec_da->cell_geom, idx);
  wts_off->InitVals(0.0f);
  wts_off->desc = "weights after multiplication by softmax_gain and with the freq penalty, for bits that are CURRENTLY OFF (for selecting one to turn ON)";
  DataCol* exps_on = work->FindMakeColMatrixN
    ("exps_on", VT_FLOAT, dest_vec_da->cell_geom, idx);
  exps_on->InitVals(0.0f);
  exps_on->desc = "exponential of weights for bits that are CURRENTLY ON (for selecting one to turn OFF)";
  DataCol* exps_off = work->FindMakeColMatrixN
    ("exps_off", VT_FLOAT, dest_vec_da->cell_geom, idx);
  exps_off->InitVals(0.0f);
  exps_off->desc = "exponential of weights for bits that are CURRENTLY OFF (for selecting one to turn ON)";
  DataCol* probs_on = work->FindMakeColMatrixN
    ("probs_on", VT_FLOAT, dest_vec_da->cell_geom, idx);
  probs_on->InitVals(0.0f);
  probs_on->desc = "final probabilities for bits that are CURRENTLY ON (for selecting one to turn OFF)";
  DataCol* probs_off = work->FindMakeColMatrixN
    ("probs_off", VT_FLOAT, dest_vec_da->cell_geom, idx);
  probs_off->InitVals(0.0f);
  probs_off->desc = "final probabilities for bits that are CURRENTLY OFF (for selecting one to turn ON)";
  
  work->EnforceRows(dest->rows);
  
  DataCol* k_da = dist_kmat->GetColData("kval"); // gives err
  if(!k_da) return false;

  DataCol* freq_da = bit_freqs->GetColData(0); // gives err
  if(!freq_da) return false;

  int vec_bits = dest_vec_da->cell_size();

  int n_flip_bit = (int)(p_flip_items * (float)dest->rows + .5f);
  if(n_flip_bit < 1) n_flip_bit = 1;

  float freq_wt_eff = freq_wt; // scale proportionally
  if(use_k_thr) {
    freq_wt_eff *= (float)k_val / (float)dest->rows; // correct for freq
  }
  
  static int_Array sorder;
  if(sorder.size != dest->rows) {
    sorder.SetSize(dest->rows);
    sorder.FillSeq();
  }
  sorder.Permute();
  // first gather probability votes for flipping bits
  for(int sidx=0;sidx < n_flip_bit; sidx++) {
    int srow = sorder[sidx];
    float dkval = k_da->GetValAsFloat(srow);
    for(int orow=0;orow < dest->rows; orow++) {
      if(orow == srow) continue;			    // skip self!
      float dist = trg_dist_mat->GetValAsFloat(orow + 1, srow); // col, row -- assume 1 offset
      if(toward) {
        if(use_k_thr && (dist < dkval)) continue; // don't do anything for non-top-k guys
      }
      else {
        if(use_k_thr && (dist > dkval)) continue; // don't do anything for non-bot-k guys
      }

      dist -= dist_off;
      if(!toward)
        dist = 1.0f - dist;
        
      for(int k=0;k<vec_bits;k++) {
	float chg_wt = wts->GetValAsFloatM(srow, k); // weight toward changing
	float sbit = dest_vec_da->GetValAsFloatM(srow, k);
	float obit = dest_vec_da->GetValAsFloatM(orow, k);
	float dwt;

        if(sbit == obit) {        // bits are the same
          if(toward) dwt = -same_wt * dist; // punish changing same bits in proportion to similarity
          else       dwt = same_wt * dist;  // reward changing same bits in proportion to distance
        }
        else {
          if(toward) dwt = diff_wt * dist; // reward changing diff bits in proportion to similarity
          else       dwt = -diff_wt * dist;  // punish changing diff bits in proportion to distance
        }          
        chg_wt += dwt;
	wts->SetValAsFloatM(chg_wt, srow, k);
      }
    }
  }

  // then compute softmax on flip probs and actually flip a bit
  for(int sidx=0;sidx < n_flip_bit; sidx++) {
    int srow = sorder[sidx];
    float_MatrixPtr wt_mat;
    wt_mat = (float_Matrix*)wts->GetValAsMatrix(srow);
    float_MatrixPtr wton_mat;
    wton_mat = (float_Matrix*)wts_on->GetValAsMatrix(srow);
    float_MatrixPtr wtoff_mat;
    wtoff_mat = (float_Matrix*)wts_off->GetValAsMatrix(srow);
    float_MatrixPtr expon_mat;
    expon_mat = (float_Matrix*)exps_on->GetValAsMatrix(srow);
    float_MatrixPtr expoff_mat;
    expoff_mat = (float_Matrix*)exps_off->GetValAsMatrix(srow);
    float_MatrixPtr pon_mat;
    pon_mat = (float_Matrix*)probs_on->GetValAsMatrix(srow);
    float_MatrixPtr poff_mat;
    poff_mat = (float_Matrix*)probs_off->GetValAsMatrix(srow);
    float on_sum = 0.0f;
    float off_sum = 0.0f;
    for(int k=0;k<vec_bits;k++) {
      float& wt = wt_mat->FastEl_Flat(k);
      float& wt_on = wton_mat->FastEl_Flat(k);
      float& wt_off = wtoff_mat->FastEl_Flat(k);
      float& exp_on = expon_mat->FastEl_Flat(k);
      float& exp_off = expoff_mat->FastEl_Flat(k);
      float freq = (freq_da->GetValAsFloatM(0, k) - trg_freq);
      float sbit = dest_vec_da->GetValAsFloatM(srow, k);
      if(sbit > 0.0f) {         // bit is currently on -- flip it off..
        wt_on = softmax_gain * wt + freq_wt_eff * freq;
        exp_on = expf(wt_on);
	on_sum += exp_on;
      }
      else {
        wt_off = softmax_gain * wt - freq_wt_eff * freq; // less likely to flip on
        exp_off = expf(wt_off);
	off_sum += exp_off;
      }
    }
    float pon_trg = Random::ZeroOne();
    float poff_trg = Random::ZeroOne();
    float sum_on_p = 0.0f;
    float sum_off_p = 0.0f;
    int bit_on = -1;
    int bit_off = -1;
    for(int k=0;k<vec_bits;k++) {
      float& exp_on = expon_mat->FastEl_Flat(k);
      float& exp_off = expoff_mat->FastEl_Flat(k);
      float& p_on = pon_mat->FastEl_Flat(k);
      float& p_off = poff_mat->FastEl_Flat(k);
      float sbit = dest_vec_da->GetValAsFloatM(srow, k);
      if(sbit > 0.0f) {
        p_on = exp_on / on_sum; // normalize
	if(bit_on < 0) {
	  sum_on_p += p_on;
	  if(sum_on_p >= pon_trg) {
	    bit_on = k;
	  }
	}
      }
      else {
        p_off = exp_off / off_sum;
	if(bit_off < 0) {
	  sum_off_p += p_off;
	  if(sum_off_p >= poff_trg) {
	    bit_off = k;
	  }
	}
      }
    }
    if(bit_on >= 0 && bit_off >= 0) {
      // flip bits!  on -> off and off -> on
      dest_vec_da->SetValAsFloatM(1.0f, srow, bit_off);
      dest_vec_da->SetValAsFloatM(0.0f, srow, bit_on);
    }
  }

  // compute summary stats

  taDataAnal::MatrixCellFreq
    (work_sum, work, "weights", false, 0.5f, false, false);
  taDataAnal::MatrixCellFreq
    (work_sum, work, "wts_on", false, 0.5f, false, false);
  taDataAnal::MatrixCellFreq
    (work_sum, work, "wts_off", false, 0.5f, false, false);
  taDataAnal::MatrixCellFreq
    (work_sum, work, "exps_on", false, 0.5f, false, false);
  taDataAnal::MatrixCellFreq
    (work_sum, work, "exps_off", false, 0.5f, false, false);
  taDataAnal::MatrixCellFreq
    (work_sum, work, "probs_on", false, 0.5f, false, false);
  taDataAnal::MatrixCellFreq
    (work_sum, work, "probs_off", false, 0.5f, false, false);
  
  return true;
}

bool taDataGenSem::SemVecGen_FlipStats
(DataTable* dest, Variant dest_name_col, Variant dest_vec_col, DataTable* trg_dist_mat,
 DataTable* lrn_stats, DataTable* dest_dist, float dist_off, float softmax_gain, float p_flip_items,
 taMath::DistMetric metric, bool norm, float tol) {
  if(!dest) {
    taMisc::Error("taDataGenSem::SemVecGenLearn_Stats dest is NULL -- must exist and be properly formatted");
    return false;
  }
  if(!trg_dist_mat) {
    taMisc::Error("taDataGenSem::SemVecGenLearn_Stats trg_dist_mat is NULL -- must exist and be properly formatted");
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
  lrn_stats->FindMakeColName("p_flip_items", idx, VT_FLOAT);
  lrn_stats->StructUpdate(false);

  taDataAnal::DistMatrixTable(dest_dist, false, dest, dest_vec_da->name, dest_nm_da->name,
			      metric, norm, tol);

  float err = SemVecGen_DistMatDist(trg_dist_mat, dest_dist, NULL, dist_off);

  lrn_stats->AddBlankRow();
  lrn_stats->SetValAsInt(lrn_stats->rows, "iter", -1);
  lrn_stats->SetValAsFloat(err, "err", -1);
  lrn_stats->SetValAsFloat(softmax_gain, "softmax_gain", -1);
  lrn_stats->SetValAsFloat(p_flip_items, "p_flip_items", -1);
  lrn_stats->WriteClose();
  return true;
}


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

float taDataGenSem::SemVecGen_DistMatDist(DataTable* dist_mat, DataTable* dist_mat2,
					  DataTable* delta_mat) {
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
    float_MatrixPtr kmat;
    kmat = (float_Matrix*)dest_tmp_vec_da->GetValAsMatrix(srow);
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
    float_MatrixPtr kmat;
    kmat = (float_Matrix*)dest_tmp_vec_da->GetValAsMatrix(srow);
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
  }
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


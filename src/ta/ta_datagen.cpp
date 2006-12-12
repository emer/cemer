// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/CSS
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

#include "ta_datagen.h"
#include "css_machine.h"
#include "ta_geometry.h"
#include "ta_project.h"		// for debugging

#include <QDir>

bool taDataGen::CheckDims(float_Matrix* mat, int dims) {
  if(!mat) return false;
  if(mat->dims() != dims) {
    taMisc::Warning("taDataGen: matrix:", mat->name, "path:", mat->GetPath(),
		    "is not of dimension:", String(dims), "it is:", String(mat->dims()));
    return false;
  }
  return true;
}

DataArray_impl* taDataGen::GetFloatMatrixDataCol(DataTable* src_data, const String& data_col_nm) {
  if(!src_data) return NULL;
  int idx;
  DataArray_impl* da = src_data->FindColName(data_col_nm, idx, true); // err msg
  if(!da)
    return NULL;
  if(!da->is_matrix) {
    taMisc::Error("taDataAnal: column named:", data_col_nm,
		  "is not a matrix in data table:", src_data->name);
    return NULL;
  }
  if(da->valType() != VT_FLOAT) {
    taMisc::Error("taDataAnal: column named:", data_col_nm,
		  "is not of type float in data table:", src_data->name);
    return NULL;
  }
  return da;
}

///////////////////////////////////////////////////////////////////
// basic operations

bool taDataGen::Clear(DataTable* data, const String& col_nm, float val) {
  if(col_nm.empty()) {
    bool rval = true;
    for(int pn = 0;pn<data->data.size;pn++) {
      DataArray_impl* da = data->data.FastEl(pn);
      if(da->is_matrix && da->valType() == VT_FLOAT) {
	if(!Clear(data, da->name, val))
	  rval = false;
      }
    }
    return rval;
  }
  DataArray_impl* da = GetFloatMatrixDataCol(data, col_nm);
  if(!da) return false;
  data->DataUpdate(true);
  for(int i=0;i<da->rows();i++) {
    float_Matrix* mat = (float_Matrix*)da->GetValAsMatrix(i);
    taBase::Ref(mat);
    mat->InitVals(val);
    taBase::unRefDone(mat);
  }
  data->DataUpdate(false);
  return true;
}

bool taDataGen::SimpleMath(DataTable* data, const String& col_nm, const SimpleMathSpec& math) {
  if(col_nm.empty()) {
    bool rval = true;
    for(int pn = 0;pn<data->data.size;pn++) {
      DataArray_impl* da = data->data.FastEl(pn);
      if(da->is_matrix && da->valType() == VT_FLOAT) {
	if(!SimpleMath(data, da->name, math))
	  rval = false;
      }
    }
    return rval;
  }
  DataArray_impl* da = GetFloatMatrixDataCol(data, col_nm);
  if(!da) return false;
  data->DataUpdate(true);
  for(int i=0;i<da->rows();i++) {
    float_Matrix* mat = (float_Matrix*)da->GetValAsMatrix(i);
    taBase::Ref(mat);
    taMath_float::vec_simple_math(mat, math);
    taBase::unRefDone(mat);
  }
  data->DataUpdate(false);
  return true;
}

///////////////////////////////////////////////////////////////////
// drawing routines

bool taDataGen::WritePoint(float_Matrix* mat, int x, int y, float color, bool wrap) {
  if(!CheckDims(mat, 2)) return false;
  bool clipped = (TwoDCoord::WrapClipOne(wrap, x, mat->dim(0)) ||
		  TwoDCoord::WrapClipOne(wrap, y, mat->dim(1)));
  if(!wrap && clipped)
    return false;
  mat->FastEl(x,y) = color;
  return true;
}

bool taDataGen::RenderLine(float_Matrix* mat, int xs, int ys, int xe, int ye,
			   float color, bool wrap)
{
  if(!CheckDims(mat, 2)) return false;
  int xd = xe - xs;
  int yd = ye - ys;
  int x,y;
  if((xd == 0) && (yd == 0)) {
    WritePoint(mat, xs, ys, color, wrap);
    return true;
  }
  if(xd == 0) {
    if(yd > 0) { for(y=ys;y<=ye;y++) WritePoint(mat, xs, y, color, wrap); }
    else       { for(y=ys;y>=ye;y--) WritePoint(mat, xs, y, color, wrap); }
    return true;
  }
  if(yd == 0) {
    if(xd > 0)	{ for(x=xs;x<=xe;x++) WritePoint(mat, x, ys, color, wrap); }
    else	{ for(x=xs;x>=xe;x--) WritePoint(mat, x, ys, color, wrap); }
    return true;
  }
  if(abs(xd) > abs(yd)) {
    if(yd > 0) yd++; else yd--;
    if(xd > 0) {
      for(x=xs; x<=xe; x++) {
	y = ys + (yd * (x - xs)) / abs(xd);
	if(yd > 0) y = MIN(ye, y);
	else y = MAX(ye, y);
	WritePoint(mat, x, y, color, wrap);
      }
    }
    else {
      for(x=xs; x>=xe; x--) {
	y = ys + (yd * (xs - x)) / abs(xd);
	if(yd > 0) y = MIN(ye, y);
	else y = MAX(ye, y);
	WritePoint(mat, x, y, color, wrap);
      }
    }
  }
  else {
    if(xd > 0) xd++; else xd--;
    if(yd > 0) {
      for(y=ys; y<=ye; y++) {
	x = xs + (xd * (y - ys)) / abs(yd);
	if(xd > 0) x = MIN(xe, x);
	else x = MAX(xe, x);
	WritePoint(mat, x, y, color, wrap);
      }
    }
    else {
      for(y=ys; y>=ye; y--) {
	x = xs + (xd * (ys - y)) / abs(yd);
	if(xd > 0) x = MIN(xe, x);
	else x = MAX(xe, x);
	WritePoint(mat, x, y, color, wrap);
      }
    }
  }
  return true;
}

bool taDataGen::WriteXPoints(float_Matrix* mat, int x, int y,
			     const float_Matrix& color, int wdth, bool wrap)
{
  if(!CheckDims(mat, 2)) return false;
  int del = (wdth - 1) / 2;
  int i;
  for(i=0; i<wdth; i++)
    WritePoint(mat, x, y+i-del, color.FastEl(i), wrap);
  return true;
}

bool taDataGen::WriteYPoints(float_Matrix* mat, int x, int y,
			     const float_Matrix& color, int wdth, bool wrap)
{
  if(!CheckDims(mat, 2)) return false;
  int del = (wdth - 1) / 2;
  int i;
  for(i=0; i<wdth; i++)
    WritePoint(mat, x+i-del, y, color.FastEl(i), wrap);
  return true;
}

bool taDataGen::RenderWideLine(float_Matrix* mat, int xs, int ys, int xe, int ye,
			       const float_Matrix& color, int wdth, bool wrap)
{
  if(!CheckDims(mat, 2)) return false;
  int xd = xe - xs;
  int yd = ye - ys;
  int x,y;
  if((xd == 0) && (yd == 0)) {
    WriteXPoints(mat, xs, ys, color, wdth, wrap);
    WriteYPoints(mat, xs, ys, color, wdth, wrap);
    return true;
  }
  if(xd == 0) {
    if(yd > 0) { for(y=ys;y<=ye;y++) WriteXPoints(mat, xs, y, color, wdth, wrap); }
    else       { for(y=ys;y>=ye;y--) WriteXPoints(mat, xs, y, color, wdth, wrap); }
    return true;
  }
  if(yd == 0) {
    if(xd > 0)	{ for(x=xs;x<=xe;x++) WriteYPoints(mat, x, ys, color, wdth, wrap); }
    else	{ for(x=xs;x>=xe;x--) WriteYPoints(mat, x, ys, color, wdth, wrap); }
    return true;
  }
  if(abs(xd) > abs(yd)) {
    if(xd > 0) {
      for(x=xs; x<=xe; x++) {
	y = ys + (yd * (x - xs)) / abs(xd);
	WriteXPoints(mat, x, y, color, wdth, wrap);
      }
    }
    else {
      for(x=xs; x>=xe; x--) {
	y = ys + (yd * (xs - x)) / abs(xd);
	WriteXPoints(mat, x, y, color, wdth, wrap);
      }
    }
  }
  else {
    if(yd > 0) {
      for(y=ys; y<=ye; y++) {
	x = xs + (xd * (y - ys)) / abs(yd);
	WriteYPoints(mat, x, y, color, wdth, wrap);
      }
    }
    else {
      for(y=ys; y>=ye; y--) {
	x = xs + (xd * (ys - y)) / abs(yd);
	WriteYPoints(mat, x, y, color, wdth, wrap);
      }
    }
  }
  return true;
}

///////////////////////////////////////////////////////////////////
// random pattern generation

bool taDataGen::AddNoiseMat(float_Matrix* mat, const Random& rnd_spec) {
  for(int i=0; i<mat->size; i++)
    mat->FastEl_Flat(i) += rnd_spec.Gen();
  return true;
}

bool taDataGen::AddNoise(DataTable* data, const String& col_nm, const Random& rnd_spec) {
  if(!data) return false;
  if(col_nm.empty()) {
    bool rval = true;
    for(int pn = 0;pn<data->data.size;pn++) {
      DataArray_impl* da = data->data.FastEl(pn);
      if(da->is_matrix && da->valType() == VT_FLOAT) {
	if(!AddNoise(data, da->name, rnd_spec))
	  rval = false;
      }
    }
    return rval;
  }
  DataArray_impl* da = GetFloatMatrixDataCol(data, col_nm);
  if(!da) return false;
  data->DataUpdate(true);
  for(int i=0;i<da->rows();i++) {
    float_Matrix* mat = (float_Matrix*)da->GetValAsMatrix(i);
    taBase::Ref(mat);
    AddNoiseMat(mat, rnd_spec);
    taBase::unRefDone(mat);
  }
  data->DataUpdate(false);
  return true;
}

bool taDataGen::PermutedBinaryMat(float_Matrix* mat, int n_on, float on_val, float off_val) {
  float_Array ary;
  ary.SetSize(mat->size);
  int n_max = MIN(mat->size, n_on);
  int i;
  for(i=0; i<n_max; i++)
    ary.Set(i, on_val);
  for(;i<mat->size;i++)
    ary.Set(i, off_val);
  ary.Permute();
  for(i=0;i<mat->size;i++) {
    mat->FastEl_Flat(i) = ary.FastEl(i);
  }
  return true;
}

bool taDataGen::PermutedBinary(DataTable* data, const String& col_nm, int n_on,
			       float on_val, float off_val) {
  if(!data) return false;
  if(col_nm.empty()) {
    bool rval = true;
    for(int pn = 0;pn<data->data.size;pn++) {
      DataArray_impl* da = data->data.FastEl(pn);
      if(da->is_matrix && da->valType() == VT_FLOAT) {
	if(!PermutedBinary(data, da->name, n_on, on_val, off_val))
	  rval = false;
      }
    }
    return rval;
  }
  DataArray_impl* da = GetFloatMatrixDataCol(data, col_nm);
  if(!da) return false;
  data->DataUpdate(true);
  for(int i=0;i<da->rows();i++) {
    float_Matrix* mat = (float_Matrix*)da->GetValAsMatrix(i);
    taBase::Ref(mat);
    PermutedBinaryMat(mat, n_on, on_val, off_val);
    taBase::unRefDone(mat);
  }
  data->DataUpdate(false);
  return true;
}

bool taDataGen::PermutedBinary_MinDist(DataTable* data, const String& col_nm, int n_on,
				       float dist, taMath::DistMetric metric,
				       bool norm, float tol)
{
  if(!data) return false;
  if(col_nm.empty()) {
    bool rval = true;
    for(int pn = 0;pn<data->data.size;pn++) {
      DataArray_impl* da = data->data.FastEl(pn);
      if(da->is_matrix && da->valType() == VT_FLOAT) {
	if(!PermutedBinary_MinDist(data, da->name, n_on, dist, metric, norm, tol))
	  rval = false;
      }
    }
    return rval;
  }
  DataArray_impl* da = GetFloatMatrixDataCol(data, col_nm);
  if(!da) return false;
  bool larger_further = taMath::dist_larger_further(metric);
  int bogus_count = 0;
  data->DataUpdate(true);
  for(int i =0;i<da->rows();i++) {
    float_Matrix* mat = (float_Matrix*)da->GetValAsMatrix(i);
    taBase::Ref(mat);
    int cnt = 100 + (10 * (i + 1));   // 100 plus 10 more for every new stim
    bool ok = false;
    float min_d;
    do {
      PermutedBinaryMat(mat, n_on);
      min_d = LastMinDist(da, i, metric, norm, tol);
      cnt--;
      if(larger_further)
	ok = (min_d >= dist);
      else
	ok = (min_d <= dist);
    } while(!ok && (cnt > 0));
    taBase::unRefDone(mat);
    
    if(cnt == 0) {
      taMisc::Warning("*** PermutedBinary_MinDist row:", String(i), "dist of:", (String)min_d,
		     "under dist limit:", (String)dist);
      bogus_count++;
    }
    if(bogus_count > 5) {
      taMisc::Warning("PermutedBinary_MinDist Giving up after 5 stimuli under the limit, set limits lower");
      data->DataUpdate(false);
      return false;
    }
  }
  data->DataUpdate(false);
  return true;
}

bool taDataGen::FlipBitsMat(float_Matrix* mat, int n_off, int n_on) {
  int_Array on_ary, off_ary;
  int i;
  for(i=0; i<mat->size; i++) {
    if(mat->FastEl_Flat(i) == 1.0)
      on_ary.Add(i);
    else
      off_ary.Add(i);
  }
  on_ary.Permute();
  off_ary.Permute();
  int n_max = MIN(on_ary.size, n_off);
  for(i=0; i<n_max; i++)
    mat->FastEl_Flat(on_ary.FastEl(i)) = 0.0f;
  n_max = MIN(off_ary.size, n_on);
  for(i=0; i<n_max; i++)
    mat->FastEl_Flat(off_ary.FastEl(i)) = 1.0f;
  return true;
}

bool taDataGen::FlipBits(DataTable* data, const String& col_nm, int n_off, int n_on) {
  if(!data) return false;
  if(col_nm.empty()) {
    bool rval = true;
    for(int pn = 0;pn<data->data.size;pn++) {
      DataArray_impl* da = data->data.FastEl(pn);
      if(da->is_matrix && da->valType() == VT_FLOAT) {
	if(!FlipBits(data, da->name, n_off, n_on))
	  rval = false;
      }
    }
    return rval;
  }
  DataArray_impl* da = GetFloatMatrixDataCol(data, col_nm);
  if(!da) return false;
  data->DataUpdate(true);
  for(int i=0;i<da->rows();i++) {
    float_Matrix* mat = (float_Matrix*)da->GetValAsMatrix(i);
    taBase::Ref(mat);
    FlipBitsMat(mat, n_off, n_on);
    taBase::unRefDone(mat);
  }
  data->DataUpdate(false);
  return true;
}

bool taDataGen::FlipBits_MinMax(DataTable* data, const String& col_nm, int n_off, int n_on,
				float min_dist, float max_dist,
				taMath::DistMetric metric, bool norm, float tol)
{
  if(!data) return false;
  if(col_nm.empty()) {
    bool rval = true;
    for(int pn = 0;pn<data->data.size;pn++) {
      DataArray_impl* da = data->data.FastEl(pn);
      if(da->is_matrix && da->valType() == VT_FLOAT) {
	if(!FlipBits_MinMax(data, da->name, n_off, n_on, min_dist, max_dist,
			    metric, norm, tol))
	  rval = false;
      }
    }
    return rval;
  }
  DataArray_impl* da = GetFloatMatrixDataCol(data, col_nm);
  if(!da) return false;
  data->DataUpdate(true);
  float_Matrix orig_pat;
  int bogus_count = 0;
  for(int i=0;i<da->rows();i++) {
    float_Matrix* mat = (float_Matrix*)da->GetValAsMatrix(i);
    taBase::Ref(mat);
    int cnt = 100 + (10 * (i + 1));   // 100 plus 10 more for every new stim
    bool ok = false;
    float min_d, max_d;
    orig_pat.Reset();
    orig_pat = *mat;
    do {
      FlipBitsMat(mat, n_off, n_on);
      min_d = LastMinMaxDist(da, i, max_d, metric, norm, tol);
      cnt--;
      ok = ((min_d >= min_dist) && (max_d <= max_dist));
      if(!ok)			// restore original pattern if not ok..
	*mat = orig_pat;
    } while(!ok && (cnt > 0));
    taBase::unRefDone(mat);
    
    if(cnt == 0) {
      taMisc::Warning("*** FlipBits_MinMax, row:", String(i), "min/max dist of:",
		      String(min_d), String(max_d), "not within dist limits:",
		      String(min_dist), String(max_dist));
      bogus_count++;
    }
    if(bogus_count > 5) {
      taMisc::Warning("FlipBits_MinMax: Giving up after 5 stimuli under the limit, set limits lower");
      data->DataUpdate(false);
      return false;
    }
  }
  data->DataUpdate(false);
  return true;
}

float taDataGen::LastMinDist(DataArray_impl* da, int row, taMath::DistMetric metric,
			     bool norm, float tol)
{
  bool larger_further = taMath::dist_larger_further(metric);
  float rval;
  if(larger_further)
    rval = taMath::flt_max;
  else
    rval  = -taMath::flt_max;
  if(row == 0) return rval;

  row = MIN(da->rows()-1, row);
  float_Matrix* mat = (float_Matrix*)da->GetValAsMatrix(row);
  taBase::Ref(mat);
  for(int i=0;i<row;i++) {
    float_Matrix* o_mat = (float_Matrix*)da->GetValAsMatrix(i);
    taBase::Ref(o_mat);
    float dst = taMath_float::vec_dist(mat, o_mat, metric, norm, tol);
    if(larger_further)
      rval = MIN(dst, rval);
    else
      rval = MAX(dst, rval);
    taBase::unRefDone(o_mat);
  }
  taBase::unRefDone(mat);
  return rval;
}

float taDataGen::LastMinMaxDist(DataArray_impl* da, int row, float& max_dist,
				taMath::DistMetric metric, bool norm, float tol)
{
  float rval = taMath::flt_max;
  max_dist = 0;
  if(row == 0) return rval;

  row = MIN(da->rows()-1, row);
  float_Matrix* mat = (float_Matrix*)da->GetValAsMatrix(row);
  taBase::Ref(mat);
  for(int i=0;i<row;i++) {
    float_Matrix* o_mat = (float_Matrix*)da->GetValAsMatrix(i);
    taBase::Ref(o_mat);
    float dst = taMath_float::vec_dist(mat, o_mat, metric, norm, tol);
    rval = MIN(dst, rval);
    max_dist = MAX(dst, max_dist);
    taBase::unRefDone(o_mat);
  }
  taBase::unRefDone(mat);
  return rval;
}

// bool taDataGen::FlipBits_GpMinMax(int pat_no, int n_off, int n_on,
// 				    float within_min_dist, float within_max_dist,
// 				    float between_dist, taMath::DistMetric metric,
// 				    bool norm, float tol, int st_gp, int ed_gp)
// {
//   if(pat_no < 0) {
//     EventSpec* es = (EventSpec*)event_specs.DefaultEl();
//     if(es == NULL) return;
//     for(int pn = 0; pn < es->patterns.size; pn++) {
//       FlipBits_GpMinMax(pn, n_off, n_on, within_min_dist, within_max_dist, between_dist,
// 			metric, norm, tol, st_gp, ed_gp);
//     }
//     return;
//   }
//   bool larger_further = taMath::dist_larger_further(metric);
//   taMath orig_pat;
//   int bogus_count = 0;
//   int g;
//   int mx_gp = events.gp.size;
//   if(ed_gp >= 0)
//     mx_gp = MIN(ed_gp, mx_gp);
//   for(g = st_gp; g < mx_gp; g++) {
//     Event_MGroup* gp = (Event_MGroup*)events.gp.FastEl(g);

//     int ev_ctr = 0;
//     taLeafItr i;
//     Event* ev;
//     FOR_ITR_EL(Event, ev, gp->, i) {
//       Pattern* pat = (Pattern*)ev->patterns.Leaf(pat_no);
//       if(pat == NULL) {
// 	taMisc::Error("*** Pattern number:", String(pat_no), "not found");
// 	return;
//       }
//       int cnt = 100 + (10 * (ev_ctr + 1));   // 100 plus 10 more for every new stim
//       bool ok = false;
//       float min_d, max_d, min_w, max_w;
//       min_w = FLT_MAX;
//       orig_pat.Reset();
//       orig_pat = pat->value;
//       do {
// 	FlipBitsPat(pat, n_off, n_on);
// 	min_d = GpWithinMinMaxDist(gp, ev_ctr, pat_no, max_d, metric, norm, tol);
// 	cnt--;
// 	ok = ((min_d >= within_min_dist) && (max_d <= within_max_dist));
// 	if(!ok)			// restore original pattern if not ok..
// 	  pat->value = orig_pat;
// 	else {
// 	  min_w = GpLastMinMaxDist(g, pat, pat_no, max_w, metric, norm, tol);
// 	  if(larger_further)
// 	    ok = (min_w >= between_dist);
// 	  else
// 	    ok = (max_w <= between_dist); // todo: check this!
// 	  if(!ok)
// 	    pat->value = orig_pat;
// 	}
//       } while(!ok && (cnt > 0));
    
//       if(cnt == 0) {
// 	taMisc::Error("*** Event:", ev->name, "within min/max dist of:", String(min_d), 
// 		      String(max_d), "not within limits:", String(within_min_dist),
// 		      String(within_max_dist), "or between:", String(min_w),
// 		      "over:",String(between_dist));
// 	bogus_count++;
//       }
//       if(bogus_count > 5) {
// 	taMisc::Error("Giving up after 5 stimuli under the limit, set limits lower");
// 	return;
//       }
//       ev_ctr++;
//     }
//   }
//   UpdateAllViews();
// }

// float taDataGen::GpWithinMinMaxDist(Event_MGroup* gp, int n, int pat_no, float& max_dist,
// 				      taMath::DistMetric metric, bool norm, float tol)
// {
//   float rval = FLT_MAX;
//   max_dist = 0;
//   if(n == 0) return rval;

//   n = MIN(gp->leaves-1, n);
//   Event* trg_ev = (Event*)gp->Leaf(n);
//   Pattern* trg_pat = (Pattern*)trg_ev->patterns.Leaf(pat_no);
//   if(trg_pat == NULL) {
//     taMisc::Error("*** Pattern number:", String(pat_no), "not found");
//     return rval;
//   }

//   taLeafItr i;
//   Event* ev;
//   FOR_ITR_EL(Event, ev, gp->, i) {
//     if(ev == trg_ev) break;
//     Pattern* pat = (Pattern*)ev->patterns.Leaf(pat_no);
//     if(pat == NULL) {
//       taMisc::Error("*** Pattern number:", String(pat_no), "not found");
//       return rval;
//     }
//     float dst = trg_pat->value.Dist(pat->value, metric, norm, tol);
//     rval = MIN(dst, rval);
//     max_dist = MAX(dst, max_dist);
//   }
//   return rval;
// }

// float taDataGen::GpMinMaxDist(Event_MGroup* gp, Pattern* trg_pat, int pat_no, float& max_dist,
// 				taMath::DistMetric metric, bool norm, float tol)
// {
//   float rval = FLT_MAX;
//   max_dist = 0;
//   taLeafItr i;
//   Event* ev;
//   FOR_ITR_EL(Event, ev, gp->, i) {
//     Pattern* pat = (Pattern*)ev->patterns.Leaf(pat_no);
//     if(pat == NULL) {
//       taMisc::Error("*** Pattern number:", String(pat_no), "not found");
//       return rval;
//     }
//     float dst = trg_pat->value.Dist(pat->value, metric, norm, tol);
//     rval = MIN(dst, rval);
//     max_dist = MAX(dst, max_dist);
//   }
//   return rval;
// }

// float taDataGen::GpLastMinMaxDist(int gp_no, Pattern* trg_pat, int pat_no, float& max_dist,
// 				    taMath::DistMetric metric, bool norm, float tol, int st_gp)
// {
//   float rval = FLT_MAX;
//   max_dist = 0;
//   int mx_gp = MIN(gp_no, events.gp.size);
//   int g;
//   for(g=st_gp; g<mx_gp; g++) {
//     Event_MGroup* gp = (Event_MGroup*)events.gp.FastEl(g);
//     float mx_dst;
//     float dst = GpMinMaxDist(gp, trg_pat, pat_no, mx_dst, metric, norm, tol);
//     rval = MIN(dst, rval);
//     max_dist = MAX(mx_dst, max_dist);
//   }
//   return rval;
// }

///////////////////////////////////////////////////////////////////
// misc data functions

bool taDataGen::GetDirFiles(DataTable* dest, const String& dir_path, 
			    const String& filter, bool recursive,
			    const String& fname_col_nm,
			    const String& path_col_nm) {
  if(!dest) return false;
  dest->StructUpdate(true);

  if(recursive) {
    taMisc::Warning("Warning: GetDirFiles does not yet support the recursive flag!");
  }

  int fname_idx = -1;
  if(!fname_col_nm.empty())
    dest->FindMakeColName(fname_col_nm, fname_idx, DataTable::VT_STRING, 0);

  int path_idx = -1;
  if(!path_col_nm.empty())
    dest->FindMakeColName(path_col_nm, path_idx, DataTable::VT_STRING, 0);

  bool found_some = false;
  QDir dir(dir_path);
  QStringList files = dir.entryList();
  if(files.size() == 0) return false;
  for(int i=0;i<files.size();i++) {
    String fl = files[i];
    if(filter.empty() || fl.contains(filter)) {
      dest->AddBlankRow();
      found_some = true;
      if(fname_idx >= 0) {
	dest->SetValAsString(fl, fname_idx, -1);
      }
      if(path_idx >= 0) {
	dest->SetValAsString(dir_path + "/" + fl, path_idx, -1);
      }
    }
    // todo: deal with recursive flag
  }
  
  return found_some;
}

/////////////////////////////////////////////////////////
//   programs to support data operations
/////////////////////////////////////////////////////////

void DataGenCall::Initialize() {
  min_type = &TA_taDataGen;
  object_type = &TA_taDataGen;
}

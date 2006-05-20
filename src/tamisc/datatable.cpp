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



// datatable.cc

#include "datatable.h"

#ifdef TA_GUI
#  include "datatable_qtso.h"
#endif

#include <limits.h>
#include <float.h>
#include <ctype.h>
/*obs
/////////////////////////
//	DataItem	//
//////////////////////////

void DataItem::Initialize() {
  is_string = false;
  vec_n = 0;
  disp_opts = " ";		// always pad with initial blank
}

void DataItem::Copy_(const DataItem& cp) {
  name = cp.name;
  disp_opts = cp.disp_opts;
  is_string = cp.is_string;
  vec_n = cp.vec_n;
}

String DataItem::DispOptionAfter(const String& opt) {
  String opt_after = disp_opts.after(opt);
  opt_after = opt_after.before(',');
  return opt_after;
}

void DataItem::SetStringName(const String& nm) {
  name = "$";
  name += nm;
  is_string = true;
}

void DataItem::SetNarrowName(const String& nm) {
  name = "|";
  name += nm;
  AddDispOption("NARROW");
}

void DataItem::SetFloatVecNm(const String& nm, int n) {
  if(n > 1) {
    name = String("<") + (String)n + ">" + nm;
    vec_n = n;
  }
  else {
    name = nm;
    vec_n = 0;
  }
}

void DataItem::SetStringVecNm(const String& nm, int n) {
  is_string = true;
  if(n > 1) {
    name = String("$<") + (String)n + ">" + nm;
    vec_n = n;
  }
  else {
    name = String("$") + nm;
    vec_n = 0;
  }
}

void DataItem::AddDispOption(const String& opt) {
  String nm = " ";		// pad with preceding blank to provide start cue
  nm += String(opt) + ",";
  if(HasDispOption(nm))
    return;
  disp_opts += nm;
}


//////////////////////////
//   LogData		//
//////////////////////////

void LogData::InitLinks() {
  inherited::InitLinks();
  taBase::Own(items, this);
  taBase::Own(index, this);
  taBase::Own(r_data, this);
  taBase::Own(s_data, this);
}

void LogData::CutLinks() {
  s_data.CutLinks();
  r_data.CutLinks();
  index.CutLinks();
  items.CutLinks();
  inherited::InitLinks();
}

void LogData::Copy_(const LogData& cp) {
  items.BorrowUnique(cp.items); // link group, so borrow
  index = cp.index;
  r_data = cp.r_data;
  s_data = cp.s_data;
}

void LogData::Reset() {
  items.Reset();
  index.Reset();
  r_data.Reset();
  s_data.Reset();
}

void LogData::AddFloat(DataItem* head, float val) {
  items.Link(head);
  r_data.Add(val);
  index.Add(r_data.size-1);
}

void LogData::AddString(DataItem* head, char* val) {
  items.Link(head);
  s_data.Add(val);
  index.Add(s_data.size-1);
}

void LogData::InitBlankData() {
  r_data.Reset();
  s_data.Reset();
  index.Reset();
  int i;
  for(i=0; i<items.size; i++) {
    if(IsString(i)) {
      s_data.Add("");
      index.Add(s_data.size-1);
    }
    else {
      r_data.Add(0);
      index.Add(r_data.size-1);
    }
  }
}

bool LogData::CompareItems(const LogData& cmp) {
  // quick and dirty is so much faster, user can do GetHeaders when necessary..
  if(cmp.items.size == items.size)
    return true;
  return false;
}
*/

/////////////////////////
//	float_RArray	//
//////////////////////////

void float_RArray::InitLinks() {
  float_Array::InitLinks();
  taBase::Own(range, this);
}

void float_RArray::Copy_(const float_RArray& cp) {
  range = cp.range;
}

float float_RArray::AbsMaxVal(int& idx, int start, int end) const {
  if(end == -1)	end = size;  else end = MIN(size, end);
  idx = start;
  float rval = fabsf(FastEl(start));
  int i;
  for(i=start+1;i<end;i++) {
    if(fabsf(FastEl(i)) > rval) {
      idx = i;
      rval = fabsf(FastEl(i));
    }
  }
  return rval;
}

void float_RArray::Add(const float& it) {
  if(size == 0)
    range.Init(it);		// first one is an 'init'
  float_Array::Add(it);
}

float float_RArray::Correl(const float_RArray& oth, int start, int end) const {
  if(end == -1)	end = size;  else end = MIN(size, end);
  end = MIN(end, oth.size);
  if(end <= start)	return 0.0f;
  float my_mean = Mean(start,end);
  float oth_mean = oth.Mean(start,end);
  float my_var = 0.0f;
  float oth_var = 0.0f;
  float rval = 0.0f;
  int i;
  for(i=start;i<end;i++) {
    float my_val = FastEl(i) - my_mean;
    float oth_val = oth.FastEl(i) - oth_mean;
    rval += my_val * oth_val;
    my_var += my_val * my_val;
    oth_var += oth_val * oth_val;
  }
  float var_prod = sqrtf(my_var * oth_var);
  if(var_prod != 0.0f)
    return rval / var_prod;
  else
    return 0.0f;
}

float float_RArray::Covar(const float_RArray& oth, int start, int end) const {
  if(end == -1)	end = size;  else end = MIN(size, end);
  end = MIN(end, oth.size);
  if(end <= start)	return 0.0f;
  float my_mean = Mean(start,end);
  float oth_mean = oth.Mean(start,end);
  float rval = 0.0f;
  int i;
  for(i=start;i<end;i++)
    rval += (FastEl(i) - my_mean) * (oth.FastEl(i) - oth_mean);
  int sz = end - start;
  return rval / (float)sz;
}

void float_RArray::Insert(const float& item, int idx, int n_els) {
  UpdateRange(item);
  float_Array::Insert(item, idx, n_els);
}

void float_RArray::ItemAdded_(const void* it_, int) {
  const float& it = *(static_cast<const float*>(it_));
  UpdateRange(it);
}

void float_RArray::ItemRemoved_(const void*, int) {
  UpdateAllRange();
}

void float_RArray::ItemsChanged_() {
  UpdateAllRange();
}

float float_RArray::MaxVal(int& idx, int start, int end) const {
  if(end == -1)	end = size;  else end = MIN(size, end);
  idx = start;
  float rval = FastEl(start);
  int i;
  for(i=start+1;i<end;i++) {
    if(FastEl(i) > rval) {
      idx = i;
      rval = FastEl(i);
    }
  }
  return rval;
}

float float_RArray::Mean(int start, int end) const {
  if(end == -1)	end = size;  else end = MIN(size, end);
  if(end <= start)	return 0.0f;
  int sz = end - start;
  return Sum(start,end) / (float)sz;
}

void float_RArray::UpdateAllRange() {
  if(size == 0) {
    range.Init(0.0f);
    return;
  }
  range.Init(FastEl(0));
  int i;
  for(i=1; i<size; i++)
    UpdateRange(FastEl(i));
}

float float_RArray::MinVal(int& idx, int start, int end) const {
  if(end == -1)	end = size;  else end = MIN(size, end);
  idx = start;
  float rval = FastEl(start);
  int i;
  for(i=start+1;i<end;i++) {
    if(FastEl(i) < rval) {
      idx = i;
      rval = FastEl(i);
    }
  }
  return rval;
}

float float_RArray::Sum(int start, int end) const {
  if(end == -1)	end = size;  else end = MIN(size, end);
  float rval = 0.0f;
  int i;
  for(i=start;i<end;i++)
    rval += FastEl(i);
  return rval;
}

float float_RArray::Var(float mean, bool use_mean, int start, int end) const {
  if(end == -1)	end = size;  else end = MIN(size, end);
  if(end <= start)	return 0.0f;
  if(!use_mean)    mean = Mean(start,end);
  float rval = 0.0f;
  int i;
  for(i=start;i<end;i++)
    rval += (FastEl(i) - mean) * (FastEl(i) - mean);
  int sz = (end - start)-1;
  if(sz == 0) sz = 1;
  return rval / (float)sz;
}

float float_RArray::StdDev(float mean, bool use_mean, int start, int end) const {
  return sqrtf(Var(mean, use_mean, start, end));
}

float float_RArray::SEM(float mean, bool use_mean, int start, int end) const {
  if(end == -1)	end = size;  else end = MIN(size, end);
  if(end <= start)	return 0.0f;
  return StdDev(mean, use_mean, start, end) / sqrtf(float(end - start));
}

float float_RArray::SSLength(int start, int end) const {
  if(end == -1)	end = size;  else end = MIN(size, end);
  float rval = 0.0f;
  int i;
  for(i=start;i<end;i++)
    rval += FastEl(i) * FastEl(i);
  return rval;
}

float float_RArray::InnerProd(const float_RArray& oth, bool norm,
			      int start, int end) const
{
  if(end == -1)	end = size;  else end = MIN(size, end);
  end = MIN(end, oth.size);
  if(end == 0)    return 0.0f;
  float rval = 0.0f;
  int i;
  for(i=start;i<end;i++)
    rval += FastEl(i) * oth.FastEl(i);
  if(norm) {
    float dist = sqrtf(SSLength(start, end) * oth.SSLength(start, end));
    if(dist != 0.0f)
      rval /= dist;
  }
  return rval;
}

float float_RArray::SumSquaresDist(const float_RArray& oth, bool norm, float tolerance,
				   int start, int end) const
{
  if(end == -1)	end = size;  else end = MIN(size, end);
  end = MIN(end, oth.size);
  float rval = 0.0f;
  int i;
  for(i=start;i<end;i++) {
    float d = FastEl(i) - oth.FastEl(i);
    if(fabs(d) <= tolerance)  d = 0.0f;
    rval += d * d;
  }
  if(norm) {
    float dist = SSLength(start, end) + oth.SSLength(start, end);
    if(dist != 0.0f)
      rval /= dist;
  }
  return rval;
}

float float_RArray::EuclidDist(const float_RArray& oth, bool norm, float tolerance,
				   int start, int end) const
{
  return sqrtf(SumSquaresDist(oth, norm, tolerance, start, end));
}

float float_RArray::HammingDist(const float_RArray& oth, bool norm, float tolerance,
				   int start, int end) const
{
  if(end == -1)	end = size;  else end = MIN(size, end);
  end = MIN(end, oth.size);
  float rval = 0.0f;
  float alen = 0.0f;
  float blen = 0.0f;
  int i;
  for(i=start;i<end;i++) {
    float d = fabs(FastEl(i) - oth.FastEl(i));
    if(d <= tolerance)  d = 0.0f;
    rval += d;
    if(norm) {
      alen += fabs(FastEl(i));
      blen += fabs(oth.FastEl(i));
    }
  }
  if(norm) {
    float dist = alen + blen;
    if(dist != 0.0f)
      rval /= dist;
  }
  return rval;
}

float float_RArray::CrossEntropy(const float_RArray& oth, int start, int end) const
{
  if(end == -1)	end = size;  else end = MIN(size, end);
  end = MIN(end, oth.size);
  if(end == 0)    return 0.0f;
  float rval = 0.0f;
  int i;
  for(i=start;i<end;i++) {
    float p = FastEl(i);
    float q = oth.FastEl(i);
    q = MAX(q,0.000001f); q = MIN(q,0.999999f);
    if(p >= 1.0f)
      rval += -logf(q);
    else if(p <= 0.0f)
      rval += -logf(1.0f - q);
    else
      rval += p * logf(p/q) + (1.0f - p) * logf((1.0f - p) / (1.0f - q));
  }
  return rval;
}

float float_RArray::Dist(const float_RArray& oth, DistMetric metric, bool norm,
			 float tolerance, int start, int end) const
{
  switch(metric) {
  case SUM_SQUARES:
    return SumSquaresDist(oth, norm, tolerance, start, end);
  case EUCLIDIAN:
    return EuclidDist(oth, norm, tolerance, start, end);
  case HAMMING:
    return HammingDist(oth, norm, tolerance, start, end);
  case COVAR:
    return Covar(oth, start, end);
  case CORREL:
    return Correl(oth, start, end);
  case INNER_PROD:
    return InnerProd(oth, norm, start, end);
  case CROSS_ENTROPY:
    return CrossEntropy(oth, start, end);
  }
  return 0.0f;
}

bool float_RArray::LargerFurther(DistMetric metric) {
  switch(metric) {
  case SUM_SQUARES:
    return true;
  case EUCLIDIAN:
    return true;
  case HAMMING:
    return true;
  case COVAR:
    return false;
  case CORREL:
    return false;
  case INNER_PROD:
    return false;
  case CROSS_ENTROPY:
    return false;
  }
  return 0.0f;
}

void float_RArray::Histogram(const float_RArray& oth, float bin_size) {
  Reset();
  float_RArray tmp = oth;	// need to sort it!
  tmp.Sort();
  float min = tmp[0];
  float max = tmp.Peek();
  int src_idx = 0;
  int trg_idx = 0;
  float cur_val;
  for(cur_val = min; cur_val <= max; cur_val += bin_size, trg_idx++) {
    float cur_max = cur_val + bin_size;
    Add(0);
    float& cur_hist = FastEl(trg_idx);
    while((src_idx < tmp.size) && (tmp[src_idx] < cur_max)) {
      cur_hist += 1.0f;
      src_idx++;
    }
  }
}

void float_RArray::AggToArray(const float_RArray& oth, Aggregate& agg,
			      int start, int end)
{
  if(end == -1)	end = size;  else end = MIN(size, end);
  end = MIN(end, oth.size);
  int i;
  for(i=start;i<end;i++)
    agg.ComputeAggNoUpdt(FastEl(i), oth.FastEl(i));
  ItemsChanged_();
}

float float_RArray::AggToVal(Aggregate& agg, int start, int end) const {
  if(end == -1)	end = size;  else end = MIN(size, end);
  float rval = agg.InitAggVal();
  agg.Init();
  int i;
  for(i=start;i<end;i++)
    agg.ComputeAgg(rval, FastEl(i));
  return rval;
}

float float_RArray::NormLen(float len, int start, int end) {
  if(end == -1)	end = size;  else end = MIN(size, end);
  if(end == 0) 	return 0.0f;
  float scale = (len * len) / SSLength(start,end);
  int i;
  for(i=start;i<end;i++) {
    float mag = (FastEl(i) * FastEl(i)) * scale;
    FastEl(i) = (FastEl(i) >= 0.0f) ? mag : -mag;
  }
  ItemsChanged_();
  return scale;
}

float float_RArray::NormSum(float sum, float min_val, int start, int end) {
  if(end == -1)	end = size;  else end = MIN(size, end);
  if(end == 0)	return 0.0f;
  float act_sum = 0.0f;
  int i;
  for(i=start;i<end;i++)
    act_sum += (FastEl(i) - range.min);
  float scale = (sum / act_sum);
  for(i=start;i<end;i++)
    FastEl(i) = ((FastEl(i) - range.min) * scale) + min_val;
  ItemsChanged_();
  return scale;
}

float float_RArray::NormMax(float max, int start, int end) {
  if(end == -1)	end = size;  else end = MIN(size, end);
  if(end == 0)	return 0.0f;
  int idx;
  float cur_max = MaxVal(idx, start, end);
  float scale = (max / cur_max);
  int i;
  for(i=start;i<end;i++)
    FastEl(i) *= scale;
  ItemsChanged_();
  return scale;
}

float float_RArray::NormAbsMax(float max, int start, int end) {
  if(end == -1)	end = size;  else end = MIN(size, end);
  if(end == 0)	return 0.0f;
  int idx;
  float cur_max = AbsMaxVal(idx, start, end);
  float scale = (max / cur_max);
  int i;
  for(i=start;i<end;i++)
    FastEl(i) *= scale;
  ItemsChanged_();
  return scale;
}

void float_RArray::SimpleMath(const SimpleMathSpec& math_spec, int start, int end) {
  if(end == -1)	end = size;  else end = MIN(size, end);
  if(end == 0)  return;
  int i;
  for(i=start;i<end;i++)
    FastEl(i) = math_spec.Evaluate(FastEl(i));
  ItemsChanged_();
}

void float_RArray::SimpleMathArg(const float_RArray& arg_ary, const SimpleMathSpec& math_spec,
				 int start, int end)
{
  if(end == -1)	end = size;  else end = MIN(size, end);
  if(end == 0)  return;
  SimpleMathSpec myms = math_spec;
  int i;
  for(i=start;i<end;i++) {
    if(i < arg_ary.size)
      myms.arg = arg_ary.FastEl(i);
    FastEl(i) = myms.Evaluate(FastEl(i));
  }
  ItemsChanged_();
}

int float_RArray::Threshold(float thresh, float low, float high, int start, int end) {
  if(end == -1)	end = size;  else end = MIN(size, end);
  if(end == 0)  return 0;
  int rval = 0;
  int i;
  for(i=start;i<end;i++) {
    if(FastEl(i) >= thresh) {
      FastEl(i) = high;
      rval++;
    }
    else
      FastEl(i) = low;
  }
  range.min = low;
  range.max = high;
  return rval;
}

void float_RArray::WritePoint(const TwoDCoord& geom, int x, int y, float color, bool wrap) {
  bool clipped = (TwoDCoord::WrapClipOne(wrap, x, geom.x) ||
		  TwoDCoord::WrapClipOne(wrap, y, geom.y));
  if(!wrap && clipped)
    return;
  int idx = (y * geom.x) + x;
  if (InRange(idx)) {
    FastEl(idx) = color;
    ItemsChanged_();
  }
}

void float_RArray::RenderLine(const TwoDCoord& geom, int xs, int ys, int xe, int ye,
			      float color, bool wrap)
{
  int xd = xe - xs;
  int yd = ye - ys;
  int x,y;
  if((xd == 0) && (yd == 0)) {
    WritePoint(geom, xs, ys, color, wrap);
    return;
  }
  if(xd == 0) {
    if(yd > 0) { for(y=ys;y<=ye;y++) WritePoint(geom, xs, y, color, wrap); }
    else       { for(y=ys;y>=ye;y--) WritePoint(geom, xs, y, color, wrap); }
    return;
  }
  if(yd == 0) {
    if(xd > 0)	{ for(x=xs;x<=xe;x++) WritePoint(geom, x, ys, color, wrap); }
    else	{ for(x=xs;x>=xe;x--) WritePoint(geom, x, ys, color, wrap); }
    return;
  }
  if(abs(xd) > abs(yd)) {
    if(yd > 0) yd++; else yd--;
    if(xd > 0) {
      for(x=xs; x<=xe; x++) {
	y = ys + (yd * (x - xs)) / abs(xd);
	if(yd > 0) y = MIN(ye, y);
	else y = MAX(ye, y);
	WritePoint(geom, x, y, color, wrap);
      }
    }
    else {
      for(x=xs; x>=xe; x--) {
	y = ys + (yd * (xs - x)) / abs(xd);
	if(yd > 0) y = MIN(ye, y);
	else y = MAX(ye, y);
	WritePoint(geom, x, y, color, wrap);
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
	WritePoint(geom, x, y, color, wrap);
      }
    }
    else {
      for(y=ys; y>=ye; y--) {
	x = xs + (xd * (ys - y)) / abs(yd);
	if(xd > 0) x = MIN(xe, x);
	else x = MAX(xe, x);
	WritePoint(geom, x, y, color, wrap);
      }
    }
  }
}

void float_RArray::WriteXPoints(const TwoDCoord& geom, int x, int y,
				const float_RArray& color, int wdth, bool wrap)
{
  int del = (wdth - 1) / 2;
  int i;
  for(i=0; i<wdth; i++)
    WritePoint(geom, x, y+i-del, color[i], wrap);
}

void float_RArray::WriteYPoints(const TwoDCoord& geom, int x, int y,
				const float_RArray& color, int wdth, bool wrap)
{
  int del = (wdth - 1) / 2;
  int i;
  for(i=0; i<wdth; i++)
    WritePoint(geom, x+i-del, y, color[i], wrap);
}

void float_RArray::RenderWideLine(const TwoDCoord& geom, int xs, int ys, int xe, int ye,
				  const float_RArray& color, int wdth, bool wrap)
{
  int xd = xe - xs;
  int yd = ye - ys;
  int x,y;
  if((xd == 0) && (yd == 0)) {
    WriteXPoints(geom, xs, ys, color, wdth, wrap);
    WriteYPoints(geom, xs, ys, color, wdth, wrap);
    return;
  }
  if(xd == 0) {
    if(yd > 0) { for(y=ys;y<=ye;y++) WriteXPoints(geom, xs, y, color, wdth, wrap); }
    else       { for(y=ys;y>=ye;y--) WriteXPoints(geom, xs, y, color, wdth, wrap); }
    return;
  }
  if(yd == 0) {
    if(xd > 0)	{ for(x=xs;x<=xe;x++) WriteYPoints(geom, x, ys, color, wdth, wrap); }
    else	{ for(x=xs;x>=xe;x--) WriteYPoints(geom, x, ys, color, wdth, wrap); }
    return;
  }
  if(abs(xd) > abs(yd)) {
    if(xd > 0) {
      for(x=xs; x<=xe; x++) {
	y = ys + (yd * (x - xs)) / abs(xd);
	WriteXPoints(geom, x, y, color, wdth, wrap);
      }
    }
    else {
      for(x=xs; x>=xe; x--) {
	y = ys + (yd * (xs - x)) / abs(xd);
	WriteXPoints(geom, x, y, color, wdth, wrap);
      }
    }
  }
  else {
    if(yd > 0) {
      for(y=ys; y<=ye; y++) {
	x = xs + (xd * (y - ys)) / abs(yd);
	WriteYPoints(geom, x, y, color, wdth, wrap);
      }
    }
    else {
      for(y=ys; y>=ye; y--) {
	x = xs + (xd * (ys - y)) / abs(yd);
	WriteYPoints(geom, x, y, color, wdth, wrap);
      }
    }
  }
}

///////////////////////////////////////////////////////////////////
//  Matricies,  Eigenvectors / Values

void float_RArray::GetMatCol(int col_dim, float_RArray& col_vec, int col_no) {
  col_vec.Reset();
  int rows = size / col_dim;
  for(int i=0;i<rows; i++) {
    col_vec.Add(FastMatEl(col_dim, i, col_no));
  }
}

void float_RArray::GetMatRow(int col_dim, float_RArray& row_vec, int row_no) {
  row_vec.Reset();
  int sti = row_no * col_dim;
  for(int i=0;i<col_dim; i++, sti++) {
    row_vec.Add(FastEl(sti));
  }
}

void float_RArray::CopyFmTriMat(int dim, const float_RArray& tri_mat) {
  EnforceSize(dim * dim);
  int i, j;
  for(i=0;i<dim;i++) {
    for(j=i;j<dim;j++) {
      FastMatEl(dim,i,j) = FastMatEl(dim,j,i) = tri_mat.FastTriMatEl(dim,i,j);
    }
  }
}

bool float_RArray::TriDiagMatRed(int n, float_RArray& d, float_RArray& e) {
  if(size != n * n) {
    taMisc::Error("*** TriDiagMatRed: matrix is not of appropriate size for dimensionality:",String(n));
    return false;
  }

  // this code is adapted from the NRC tred2.c function!

  d.EnforceSize(n);
  e.EnforceSize(n);

  int l,k,j,i;
  float scale,hh,h,g,f;
  for(i=n;i>=2;i--) {
    l=i-1;
    h=scale=0.0;
    if(l > 1) {
      for(k=1;k<=l;k++)
	scale += fabs(FastMatEl1(n,i,k));
      if(scale == 0.0)
	e.FastEl1(i) = FastMatEl1(n,i,l);
      else {
	for(k=1;k<=l;k++) {
	  float& vl = FastMatEl1(n, i,k);
	  vl /= scale;
	  h += vl * vl;
	}
	f= FastMatEl1(n, i, l);
	g = f > 0 ? -sqrtf(h) : sqrtf(h);
	e.FastEl1(i)=scale*g;
	h -= f*g;
	FastMatEl1(n,i,l)=f-g;
	f=0.0;
	for (j=1;j<=l;j++) {
	  /* Next statement can be omitted if eigenvectors not wanted */
	  FastMatEl1(n,j,i)=FastMatEl1(n,i,j)/h;
	  g=0.0;
	  for (k=1;k<=j;k++)
	    g += FastMatEl1(n,j,k)*FastMatEl1(n,i,k);
	  for (k=j+1;k<=l;k++)
	    g += FastMatEl1(n,k,j)*FastMatEl1(n,i,k);
	  e.FastEl1(j)=g/h;
	  f += e.FastEl1(j)*FastMatEl1(n,i,j);
	}
	hh=f/(h+h);
	for (j=1;j<=l;j++) {
	  f=FastMatEl1(n,i,j);
	  e.FastEl1(j)=g=e.FastEl1(j)-hh*f;
	  for (k=1;k<=j;k++)
	    FastMatEl1(n,j,k) -= (f*e.FastEl1(k)+g*FastMatEl1(n,i,k));
	}
      }
    } else
      e.FastEl1(i)=FastMatEl1(n,i,l);
    d.FastEl1(i)=h;
  }
  /* Next statement can be omitted if eigenvectors not wanted */
  d[0]=0.0;
  e[0]=0.0;
  /* Contents of this loop can be omitted if eigenvectors not
     wanted except for statement d[i]=a[i][i]; */
  for (i=1;i<=n;i++) {
    l=i-1;
    if (d.FastEl1(i)) {
      for (j=1;j<=l;j++) {
	g=0.0;
	for (k=1;k<=l;k++)
	  g += FastMatEl1(n,i,k)*FastMatEl1(n,k,j);
	for (k=1;k<=l;k++)
	  FastMatEl1(n,k,j) -= g*FastMatEl1(n,k,i);
      }
    }
    d.FastEl1(i)=FastMatEl1(n,i,i);
    FastMatEl1(n,i,i)=1.0;
    for (j=1;j<=l;j++)
      FastMatEl1(n,j,i)=FastMatEl1(n,i,j)=0.0;
  }
  return true;
}

#define SIGN(a,b) ((b)<0 ? -fabs(a) : fabs(a))

bool float_RArray::TriDiagQL(int n, float_RArray& d, float_RArray& e) {
  int m,l,iter,i,k;
  float s,r,p,g,f,dd,c,b;

  // this code is adapted from the NRC tqli.c function!

  for(i=2;i<=n;i++) e.FastEl1(i-1)=e.FastEl1(i);
  e.FastEl1(n)=0.0;
  for (l=1;l<=n;l++) {
    iter=0;
    do {
      for(m=l;m<=n-1;m++) {
	dd=fabs(d.FastEl1(m))+fabs(d.FastEl1(m+1));
	if (fabs(e.FastEl1(m))+dd == dd) break;
      }
      if(m != l) {
	if (iter++ == 30) {
	  taMisc::Error("*** TriDiagQL: Too many iterations!");
	  return false;
	}
	g=(d.FastEl1(l+1)-d.FastEl1(l))/(2.0*e.FastEl1(l));
	r=sqrt((g*g)+1.0);
	g=d.FastEl1(m)-d.FastEl1(l)+e.FastEl1(l)/(g+SIGN(r,g));
	s=c=1.0f;
	p=0.0;
	for(i=m-1;i>=l;i--) {
	  f=s*e.FastEl1(i);
	  b=c*e.FastEl1(i);
	  if(fabs(f) >= fabs(g)) {
	    c=g/f;
	    r=sqrt((c*c)+1.0);
	    e.FastEl1(i+1)=f*r;
	    c *= (s=1.0/r);
	  } else {
	    s=f/g;
	    r=sqrt((s*s)+1.0);
	    e.FastEl1(i+1)=g*r;
	    s *= (c=1.0/r);
	  }
	  g=d.FastEl1(i+1)-p;
	  r=(d.FastEl1(i)-g)*s+2.0*c*b;
	  p=s*r;
	  d.FastEl1(i+1)=g+p;
	  g=c*r-b;
	  /* Next loop can be omitted if eigenvectors not wanted */
	  for (k=1;k<=n;k++) {
	    f=FastMatEl1(n,k,i+1);
	    FastMatEl1(n,k,i+1)=s*FastMatEl1(n,k,i)+c*f;
	    FastMatEl1(n,k,i)=c*FastMatEl1(n,k,i)-s*f;
	  }
	}
	d.FastEl1(l) -= p;
	e.FastEl1(l)=g;
	e.FastEl1(m)=0.0;
      }
    } while (m != l);
  }
  return true;
}

bool float_RArray::Eigens(int n, float_RArray& evals) {
  evals.EnforceSize(n);

  if(size != n * n) {
    taMisc::Error("*** Eigens: matrix is not of appropriate size for dimensionality:",String(n));
    return false;
  }
  float_RArray off_diags;
  off_diags.EnforceSize(n);
  TriDiagMatRed(n, evals, off_diags);
  if(!TriDiagQL(n, evals, off_diags))
    return false;
  return true;
}

bool float_RArray::MDS(int dim, float_RArray& xcoords, float_RArray& ycoords, int x_axis_c, int y_axis_c, bool print_evals) {
  if(size != dim * dim) {
    taMisc::Error("*** MDS: matrix is not of appropriate size for dimensionality:",String(dim));
    return false;
  }
  if((x_axis_c < 0) || (x_axis_c >= dim)) {
    taMisc::Error("*** MDS: x_axis component must be between 0 and",String(dim-1));
    return false;
  }
  if((y_axis_c < 0) || (y_axis_c >= dim)) {
    taMisc::Error("*** MDS: y_axis component must be between 0 and",String(dim-1));
    return false;
  }

  int i,j;
  // first square the individual elements
  for(i=0;i<size;i++) FastEl(i) *= FastEl(i);

  // then double-center the matrix
  for(i=0; i<dim; i++) {
    double sum = 0.0;
    for(j=0; j<dim; j++)
      sum += FastMatEl(dim, i, j);
    sum /= (double)dim;
    for(j=0; j<dim; j++)
      FastMatEl(dim, i, j) -= sum;
  }
  for(j=0; j<dim; j++) {
    double sum = 0.0;
    for(i=0; i<dim; i++)
      sum += FastMatEl(dim, i, j);
    sum /= (double)dim;
    for(i=0; i<dim; i++)
      FastMatEl(dim, i, j) -= sum;
  }

  for(i=0;i<size;i++) FastEl(i) *= -.5f;

  float_RArray evals;
  Eigens(dim, evals);
  if(print_evals) {
    cerr << "eigen vals: " << endl;
    evals.List(cerr);
    cerr << endl;
  }

  // multiply the eigenvectors by sqrt(eigen values)
  xcoords.Reset();
  int x_axis_rev = dim - 1 - x_axis_c;
  if(print_evals) {
    cerr << "X coordinate (" << x_axis_c << ") eigen value: " << evals[x_axis_rev] << endl;
  }
  float evsq = sqrt(fabs(evals[x_axis_rev]));
  for(i=0;i<dim;i++) {
    float val = FastMatEl(dim, i, x_axis_rev) * evsq;
    xcoords.Add(val);
  }

  ycoords.Reset();
  int y_axis_rev = dim - 1 - y_axis_c;
  if(print_evals) {
    cerr << "Y coordinate (" << y_axis_c << ") eigen value: " << evals[y_axis_rev] << endl;
  }
  evsq = sqrt(fabs(evals[y_axis_rev]));
  for(i=0;i<dim;i++) {
    float val = FastMatEl(dim, i, y_axis_rev) * evsq;
    ycoords.Add(val);
  }

  return true;
}

//////////////////////////
//   DataArray_impl	//
//////////////////////////

void DataArray_impl::DecodeName(String nm, String& base_nm, int& vt, int& vec_col, int& col_cnt) {
  base_nm = nm;
  vt = -1; // unknown
  vec_col = -1;
  if (nm.empty()) return;
  // check type character, float has none
  if (nm[0] == '|') {
    nm = nm.after('|');
    vt = VT_INT;
  } else if (nm[0] == '$') {
    nm = nm.after('$');
    vt = VT_STRING;
  } else {
    vt = VT_FLOAT;
  }
  base_nm = nm;
  if (nm.empty()) return;
  // check if a vec start
  if (nm[0] == '<') {
    nm = nm.after('<');
    col_cnt = (int)nm.before('>');
    base_nm = nm.after('>');
    vec_col = 0;
    return;
  }
  // check if a non-col0 vector column -- NOTE: this is not 100% bulletproof
  String col_str = nm;
  if (col_str.contains('_')) { // may be vec column
    while (col_str.contains('_'))
      col_str = col_str.after('_');
    if (col_str.isInt()) {
      vec_col = (int)col_str;
      return;
    }
  }
}

String DataArray_impl::ValTypeToStr(ValType vt) {
  static String str_String("String");
  static String str_float("float");
  static String str_int("int");
  static String str_byte("byte");
  static String str_Variant("Variant");
  switch (vt) {
  case VT_STRING: return str_String;
  case VT_FLOAT: return str_float;
  case VT_INT: return str_int;
  case VT_BYTE: return str_byte;
  case VT_VARIANT: return str_Variant;
  default: return _nilString; // compiler food
  }
}

void DataArray_impl::Initialize() {
  save_to_file = true;
  is_matrix = false;
  // default initialize to scalar
  cell_geom.EnforceSize(1);
  cell_geom.Set(0, 1);
}

void DataArray_impl::InitLinks() {
  inherited::InitLinks();
  taBase::Own(cell_geom, this);
  taMatrix* ar = AR();
  if (ar != NULL)
    taBase::Own(ar, this);
}

void DataArray_impl::CutLinks() {
  cell_geom.CutLinks();
  inherited::CutLinks();
}

void DataArray_impl::Copy_(const DataArray_impl& cp) {
  disp_opts = cp.disp_opts;
  is_matrix = cp.is_matrix;
  cell_geom = cp.cell_geom;
}

void DataArray_impl::Init() {
  taMatrix* ar = AR(); //cache
  if (is_matrix) {
    MatrixGeom tdim = cell_geom;
    tdim.EnforceSize(tdim.size + 1); // leaves the new outer dim = 0, which is flex sizing
    ar->SetGeomN(tdim);
  } else {
    ar->SetGeom(1, 0); // sets to 1-d, with flex sizing
  }
}

void DataArray_impl::UpdateAfterEdit() {
  //TODO: is_matrix needs to be true if framesize > 1
  // TODO: check if frame size has changed (ie, dim change, or matrix change)
  //TEMP: maybe this is enough???
  Init();
  inherited::UpdateAfterEdit();
}

void DataArray_impl::AddDispOption(const String& opt) {
  String nm = " ";		// pad with preceding blank to provide start cue
  nm += String(opt) + ",";
  if(HasDispOption(nm))
    return;
  disp_opts += nm;
}

DataTable* DataArray_impl::dataTable() {
  DataTable* rval = GET_MY_OWNER(DataTable);
  if (rval) 
    rval = (DataTable*)rval->root_gp;
  return rval;
}

int DataArray_impl::displayWidth() const {// low level display width, in tabs (8 chars/tab), taken from spec
  int rval = 2; // default
  // explicit width has highest priority
  String wd = DispOptionAfter(" WIDTH=");
  if (!wd.empty())
    rval = (int)wd;
  // NARROW implies width of 1
  else if (HasDispOption(" NARROW,"))
    rval = 1;
  return rval;
}

const String DataArray_impl::DispOptionAfter(const String& opt) const {
  String rval = disp_opts.after(opt);
  rval = rval.before(',');
  return rval;
}

bool DataArray_impl::Dump_QuerySaveMember(MemberDef* md) {
  if (md->name == "ar") {
    // if no save, don't need to check DataTable global
    if (!save_to_file) return false;
    DataTable* dt = dataTable();
    if (dt)
      return dt->save_data;
    else return true;
  } else return inherited::Dump_QuerySaveMember(md);
}

String DataArray_impl::GetColText(int col, int itm_idx) {
  switch (col) {
  case 0: return GetDisplayName();
  case 1: return ValTypeToStr(valType());
  case 2: return disp_opts;
  default: return _nilString;
  }
}

String DataArray_impl::GetDisplayName() const {
  String rval; int vt; int vec_col; int col_cnt;
  DecodeName(name, rval, vt, vec_col, col_cnt);
  return rval;
}

const String DataArray_impl::GetValAsString_impl(int row, int cell) const {
  const taMatrix* ar = AR(); //cache, and preserves constness
  return ar->SafeElAsStr_Flat(IndexOfEl_Flat(row, cell));
} 

const Variant DataArray_impl::GetValAsVar_impl(int row, int cell) const {
  const taMatrix* ar = AR(); //cache, and preserves constness
  return ar->SafeElAsVar_Flat(IndexOfEl_Flat(row, cell));
} 

int DataArray_impl::IndexOfEl_Flat(int row, int cell) const {
  if ((cell < 0) || (cell >= cell_size())) return -1;
  if (row < 0) row = rows() + row; // abs row, if request was from end
  if (row < 0) return -1;
  return (row * cell_size()) + cell;
} 

bool DataArray_impl::SetValAsString_impl(const String& val, int row, int cell) {
  AR()->SetFmStr_Flat(val, IndexOfEl_Flat(row, cell)); // note: safe operation
  return true;
} 

bool DataArray_impl::SetValAsVar_impl(const Variant& val, int row, int cell) {
  AR()->SetFmVar_Flat(val, IndexOfEl_Flat(row, cell)); // note: safe operation
  return true;
} 


//////////////////////////
//  ColDescriptor	//
//////////////////////////

void ColDescriptor::Initialize() {
  col_num = -1; // for standalone, means "at end", otherwise, when in list, is set to index number
  val_type = DataArray_impl::VT_FLOAT; // most common type
  save_to_file = true;
  is_matrix = false;
  cell_geom.EnforceSize(1);
}

void ColDescriptor::InitLinks() {
  inherited::InitLinks();
  taBase::Own(cell_geom, this);
}
 
void ColDescriptor::CutLinks() {
  cell_geom.CutLinks();
  inherited::CutLinks();
}

void ColDescriptor::Copy_(const ColDescriptor& cp) {
  col_num = cp.col_num;
  val_type = cp.val_type; 
  disp_opts = cp.disp_opts;
  save_to_file = cp.save_to_file;
  is_matrix = cp.is_matrix;
  cell_geom = cp.cell_geom;
}

void ColDescriptor::CopyFromDataArray(const DataArray_impl& cp) {
  name = cp.name;
  col_num = -1; //TODO
  val_type = cp.valType(); 
  disp_opts = cp.disp_opts;
  save_to_file = cp.save_to_file;
  is_matrix = cp.is_matrix;
  cell_geom = cp.cell_geom;
}



String ColDescriptor::GetColText(int col, int) {
  switch (col) {
  case 0: return col_num;
  case 1: return name;
  case 2: return DataArray_impl::ValTypeToStr(val_type);
  case 3: return disp_opts;
  case 4: return save_to_file;
  case 5: return is_matrix;
  case 6: return taMatrix::GeomToString(cell_geom);
  default: return _nilString; // compiler food
  }
}

//////////////////////////
//  ColDescriptor_List	//
//////////////////////////

void ColDescriptor_List::El_SetIndex_(void* it, int idx) {
  ((ColDescriptor*)it)->col_num = idx;
}

String ColDescriptor_List::GetColHeading(int col) {
  switch (col) {
  case 0: return "Col #";
  case 1: return "Col Name";
  case 2: return "Value Type";
  case 3: return "Disp Opts";
  case 4: return "Save";
  case 5: return "Is Matrix";
  case 6: return "Matrix Geom";
  default: return _nilString; // compiler food
  }
}


//////////////////////////
//	DataTable	//
//////////////////////////

void DataTable::Initialize() {
  SetBaseType(&TA_DataArray);	// the impl doesn't inherit properly..
  rows = 0;
  save_data = true;
#ifdef TA_GUI
  m_dtm = NULL; // returns new if none exists, or existing -- enables views to be shared
#endif
}

void DataTable::Destroy() {
#ifdef TA_GUI
  if (m_dtm) {
   delete m_dtm;
   m_dtm = NULL;
  }
#endif
}

void DataTable::Copy_(const DataTable& cp) {
  rows = cp.rows;
  save_data = cp.save_data;
}

void DataTable::AddColDispOpt(const String& dsp_opt, int col) {
  DataArray_impl* da = GetColData(col);
  if(da != NULL) da->AddDispOption(dsp_opt);
}

void DataTable::AddArrayToRow(float_RArray& tar) {
  if(tar.size == 0)	return;
  RowAdding();
  int cnt = 0;
  taLeafItr i;
  DataArray_impl* ar;
  FOR_ITR_EL(DataArray_impl, ar, this->, i) {
    if(ar->InheritsFrom(TA_float_Data))
      ((float_RArray*)ar->AR())->Add(tar.FastEl(cnt));
    cnt++;
    if(cnt >= tar.size)	break;
  }
  RowAdded();
}

void DataTable::AddBlankRow() {
  if (leaves == 0) return;
  RowAdding();
  taLeafItr i;
  DataArray_impl* ar;
  FOR_ITR_EL(DataArray_impl, ar, this->, i) {
    taMatrix* mat = ar->AR();
    if (!mat) continue;
    mat->EnforceFrames(mat->frames() + 1);
  }
  RowAdded();
}

/*obs void DataTable::AddRow(LogData& ld) {
  int cur_i = 0;		// current item at top level
  int subgp_gpi = 0;		// current subgroup index (of the group)
  int subgp_i = 0;		// current subgroup index (of the items in the group)
  int subgp_max = 0;		// max for current subgroup
  DataTable* subgp = NULL;	// the subgroup

  RowAdding();
  int ldi;
  for (ldi=0; ldi < ld.items.size; ldi++) {
    DataItem* ditem = ld.items.FastEl(ldi);
    if (ld.IsVec(ldi)) {
      if (this->gp.size > subgp_gpi)
	subgp = (DataTable*)this->gp[subgp_gpi];
      else {
	return;			// should not happen!
      }
      subgp_gpi++;
      subgp_max = ld.GetVecN(ldi);
      if (subgp_max <= 0)
	subgp_max = 1;		// need at least one in group (this one!)
      subgp_i = 0;

      SetFieldData(ld, ldi, ditem_i);
    } else {
      if (subgp != NULL) {	// in a subgroup
	subgp_i++;		// increment the index
	if (subgp_i >= subgp_max) { // done with this group
	  subgp = NULL;
	  SetFieldData(ld, ldi, ditem, this, cur_i);
	  cur_i++;
	} else {			// get item from this group
	  SetFieldData(ld, ldi, ditem_i);
	}
      } else {			// in top-level group
	SetFieldData(ld, ldi, ditem, this, cur_i);
	cur_i++;
      }
    }
  }
  RowAdded();
} */

void DataTable::AddRowToArray(float_RArray& tar, int row_num) const {
  taLeafItr i;
  DataArray_impl* ar;
  FOR_ITR_EL(DataArray_impl, ar, this->, i) {
    float val = 0;
    if(ar->InheritsFrom(TA_float_Data))
      val = ((float_RArray*)ar->AR())->SafeEl(row_num);
    tar.Add(val);
  }
}

void DataTable::AggRowToArray(float_RArray& tar, int row_num, Aggregate& agg) const {
  if(tar.size < leaves)
    tar.Insert(0, tar.size, leaves - tar.size);
  int cnt = 0;
  taLeafItr i;
  DataArray_impl* ar;
  FOR_ITR_EL(DataArray_impl, ar, this->, i) {
    float val = 0;
    if(ar->InheritsFrom(TA_float_Data))
      val = ((float_RArray*)ar->AR())->SafeEl(row_num);
    agg.ComputeAggNoUpdt(tar.FastEl(cnt), val);
    cnt++;
  }
}

float DataTable::AggRowToVal(int row_num, Aggregate& agg) const {
  float rval = agg.InitAggVal();
  agg.Init();
  taLeafItr i;
  DataArray_impl* ar;
  FOR_ITR_EL(DataArray_impl, ar, this->, i) {
    float val = 0;
    if(ar->InheritsFrom(TA_float_Data))
      val = ((float_RArray*)ar->AR())->SafeEl(row_num);
    agg.ComputeAgg(rval, val);
  }
  return rval;
}

void DataTable::AggArrayToRow(const float_RArray& tar, int row_num, Aggregate& agg) {
  if(tar.size == 0)	return;
  int cnt = 0;
  taLeafItr i;
  DataArray_impl* ar;
  FOR_ITR_EL(DataArray_impl, ar, this->, i) {
    if(ar->InheritsFrom(TA_float_Data)) {
      float_Matrix* far = static_cast<float_Matrix*>(ar->AR());
      if (far->InRange(row_num)) {
        float& val = far->FastEl(row_num); //WARN: this is NOT very good practice!!!
        agg.ComputeAggNoUpdt(val, tar.FastEl(cnt)); //we pass the ref of inside the array to this routine
      }
    }
    cnt++;
    if(cnt >= tar.size)	break;
  }
}

void DataTable::AllocRows(int n) {
  taLeafItr i;
  DataArray_impl* ar;
  FOR_ITR_EL(DataArray_impl, ar, this->, i) {
    ar->AR()->AllocFrames(n); //noop if already has more alloc'ed
  }
}

int DataTable::Dump_Load_Value(istream& strm, TAPtr par) {
  int c = inherited::Dump_Load_Value(strm, par);
  if (c == EOF) return EOF;
  if (c == 2) return 2; // signal that it was just a path
  // otherwise, if data was loaded, we need to set the rows
  if (save_data && IsRoot()) {
    int i;
    DataArray_impl* col;
    for (i = 0; i < cols(); ++i) {
      col = GetColData(i);
      if (!col->save_to_file) continue;
      int frms = col->AR()->frames();
      // number of rows is going to be = biggest number in individual cols
      rows = max(rows, frms);
    }
  }
  return c;
}

String DataTable::GetColHeading(int col) {
  switch (col) {
  case 0: return "Col Name";
  case 1: return "Data Type";
  case 2: return "Disp Opts";
  default: return _nilString;
  }
}

DataArray_impl* DataTable::GetColData(int col) const {
  DataTable* tbl = NULL;
  if (col >= leaves) return NULL;
  else return Leaf(col);
}

taMatrix* DataTable::GetColMatrix(int col) const {
  DataArray_impl* da = GetColData(col);
  if (da) return da->AR();
  else return NULL;
}


#ifdef TA_GUI
QAbstractItemModel* DataTable::GetDataModel() {
  if (!m_dtm) {
    m_dtm = new DataTableModel();
    m_dtm->setDataTable(this);
  }
  return m_dtm;
}
#endif

float DataTable::GetValAsFloat(int col, int row) {
  DataArray_impl* da = GetColData(col);
  int i;
  if (da &&  idx(row, da->rows(), i))
    return da->GetValAsFloat(i);
  else return 0.0f;
}

const String DataTable::GetValAsString(int col, int row) const {
  DataArray_impl* da = GetColData(col);
  int i;
  if (da &&  idx(row, da->rows(), i))
    return da->GetValAsString(i);
  else return "n/a";
}

const Variant DataTable::GetValAsVar(int col, int row) const {
  DataArray_impl* da = GetColData(col);
  int i;
  if (da &&  idx(row, da->rows(), i))
    return da->GetValAsVar(i);
  else return _nilVariant;
}

bool DataTable::hasData(int col, int row) {
  DataArray_impl* da = GetColData(col);
  int i;
  return (da && idx(row, da->rows(), i));
}


int DataTable::MaxLength() {
  return rows;
}
/*obs int DataTable::MaxLength() {
  int max = 0;
  taLeafItr i;
  DataArray_impl* ar;
  FOR_ITR_EL(DataArray_impl, ar, this->, i) {
    max = MAX(max,ar->AR()->size);
  }
  return max;
} */

int DataTable::MinLength() {
  if(size == 0) return 0;
  int min = INT_MAX;
  taLeafItr i;
  DataArray_impl* ar;
  FOR_ITR_EL(DataArray_impl, ar, this->, i) {
    min = MIN(min,ar->AR()->size);
  }
  return min;
}

DataArray_impl* DataTable::NewCol(DataArray_impl::ValType val_type, const String& col_nm) {
  StructUpdate(true);
  DataArray_impl* rval = NewCol_impl(val_type, col_nm);
  rval->Init(); // asserts geom
  StructUpdate(false);
  return rval;
}

DataArray_impl* DataTable::NewCol_impl(DataArray_impl::ValType val_type, const String& col_nm) {
  TypeDef* td;
  switch (val_type) {
  case DataArray_impl::VT_STRING: td = &TA_String_Data; break;
  case DataArray_impl::VT_FLOAT:  td = &TA_float_Data; break;
  case DataArray_impl::VT_INT:  td = &TA_int_Data; break;
  case DataArray_impl::VT_BYTE:  td = &TA_byte_Data; break;
  case DataArray_impl::VT_VARIANT:  td = &TA_Variant_Data; break;
  default: return NULL; // compiler food
  }
  DataArray_impl* rval = (DataArray_impl*) NewEl(1, td);
  rval->name = col_nm;
  // additional specialized initialization
  switch (val_type) {
  case DataArray_impl::VT_STRING: 
    break;
  case DataArray_impl::VT_FLOAT: 
    break;
  case DataArray_impl::VT_INT: 
    rval->AddDispOption("NARROW");
    break;
  case DataArray_impl::VT_BYTE:  
    break;
  default: break; // compiler food
  }
  return rval;
}

float_Data* DataTable::NewColFloat(const String& col_nm) {
  return (float_Data*)NewCol(DataArray_impl::VT_FLOAT, col_nm);
}

int_Data* DataTable::NewColInt(const String& col_nm) {
  return (int_Data*)NewCol(DataArray_impl::VT_INT, col_nm);
}

DataArray_impl* DataTable::NewColMatrixGeom(DataArray_impl::ValType val_type, const String& col_nm,
    const MatrixGeom& cell_geom) 
{
  StructUpdate(true);
  DataArray_impl* rval = NewCol_impl(val_type, col_nm);
  rval->is_matrix = true;
  rval->cell_geom = cell_geom;
  rval->Init(); // asserts geom
  StructUpdate(false);
  return rval;
}

DataArray_impl* DataTable::NewColMatrix(DataArray_impl::ValType val_type, const String& col_nm,
    int dims, int d0, int d1, int d2, int d3)
{
  MatrixGeom geom(dims);
  if (dims > 0) geom.FastEl(0) = d0; //note: required, but is checked in the validation routine
  if (dims > 1) geom.FastEl(1) = d1;
  if (dims > 2) geom.FastEl(2) = d2;
  if (dims > 3) geom.FastEl(3) = d3;
  String err_msg;
  if (!taMatrix::GeomIsValid(geom, &err_msg)) {
    taMisc::Error("Invalid geom:", err_msg);
    return NULL;
  }
  
  DataArray_impl* rval = NewColMatrixGeom(val_type, col_nm, geom);
  return rval;
}

String_Data* DataTable::NewColString(const String& col_nm) {
  return (String_Data*)NewCol(DataArray_impl::VT_STRING, col_nm);
}

DataTable* DataTable::NewGroupFloat(const String& col_nm, int n) {
//TODO: obs
  StructUpdate(true);
  DataTable* rval = (DataTable*)NewGp(1);
  rval->el_typ = &TA_float_Data;
  rval->EnforceSize(n);
  rval->name = col_nm;
  if(n > 0) {
    float_Data* da = rval->NewColFloat(col_nm);
    da->name = String("<") + (String)n + ">" + col_nm + "_0"; // <n> indicates vector
  }
  int i;
  for(i=1;i<n;i++) {
    float_Data* da = rval->NewColFloat(col_nm);
    da->name = String(col_nm) + "_" + String(i);
  }
  StructUpdate(false);
  return rval;
}

DataTable* DataTable::NewGroupInt(const String& col_nm, int n) {
//TODO: obs
  StructUpdate(true);
  DataTable* rval = (DataTable*)NewGp(1);
  rval->el_typ = &TA_int_Data;
  rval->EnforceSize(n);
  rval->name = String("|") + col_nm;
  if(n > 0) {
    int_Data* da = (int_Data*)rval->NewCol(DataArray_impl::VT_INT, col_nm);
    da->name = String("|<") + (String)n + ">" + col_nm + "_0"; // <n> indicates vector
    da->AddDispOption("NARROW");
  }
  int i;
  for(i=1;i<n;i++) {
    int_Data* da = (int_Data*)rval->NewCol(DataArray_impl::VT_INT, col_nm);
    da->name = String("|") + String(col_nm) + "_" + String(i);
    da->AddDispOption("NARROW");
  }
  StructUpdate(false);
  return rval;
}

DataTable* DataTable::NewGroupString(const String& col_nm, int n) {
//TODO: obs
  StructUpdate(true);
  DataTable* rval = (DataTable*)NewGp(1);
  rval->el_typ = &TA_String_Data;
  rval->EnforceSize(n);
  rval->name = String("$") + col_nm;
  if(n > 0) {
    String_Data* da = rval->NewColString(col_nm);
    da->name = String("$<") + (String)n + ">" + col_nm + "_0"; // <n> indicates vector
  }
  int i;
  for(i=1;i<n;i++) {
    String_Data* da = rval->NewColString(col_nm);
    da->name = String("$") + String(col_nm) + "_" + String(i);
  }
  StructUpdate(false);
  return rval;
}

void DataTable::PutArrayToCol(const float_RArray& ar, int col) {
/*TODO
  float_Matrix* far = GetColFloatArray(col);
  if (far != NULL) {
    far->CopyFrom((taBase*)&ar);
  }*/
} 

void DataTable::PutArrayToRow(const float_RArray& tar, int row_num) {
  if (tar.size == 0)	return;
  int cnt = 0;
  taLeafItr i;
  DataArray_impl* ar;
  FOR_ITR_EL(DataArray_impl, ar, this->, i) {
    if (ar->InheritsFrom(TA_float_Data)) {
      ar->SetValAsFloat(tar.FastEl(cnt), row_num);
    }
    ++cnt;
    if (cnt >= tar.size) break;
  }
}

void DataTable::RemoveRow(int row) {
  if (!RowInRangeNormalize(row)) return;
  DataUpdate(true);
  taLeafItr i;
  DataArray_impl* ar;
  FOR_ITR_EL(DataArray_impl, ar, this->, i) {
    int act_row;
    if (idx(row, ar->AR()->size, act_row))
      ar->AR()->RemoveFrame(act_row);
  }
  --rows;
  DataUpdate(false);
}

bool DataTable::RowInRangeNormalize(int& row) {
  if (row < 0) row = rows + row;
  return ((row >= 0) && (row < rows));
} 


/*TODO void DataTable::ShiftUp(int num_rows) {

  if (num_rows >= rows) {
    ResetData();
    return;
  }
  DataUpdate(true);
  taLeafItr i;
  DataArray_impl* ar;
  FOR_ITR_EL(DataArray_impl, ar, this->, i) {
    int act_num_rows = num_rows - (rows - ar->AR()->size);
    if (act_num_rows > 0)
      ar->AR()->ShiftLeft(act_num_rows);
  }
  rows -= num_rows;
  DataUpdate(false); 
}*/


void DataTable::Reset() {
  StructUpdate(true);
  inherited::Reset();
  rows = 0;
  StructUpdate(false);
}

void DataTable::ResetData() {
  DataUpdate(true);
  taLeafItr i;
  DataArray_impl* ar;
  FOR_ITR_EL(DataArray_impl, ar, this->, i) {
    ar->AR()->Reset();
  }
  rows = 0;
  DataUpdate(false);
}

void DataTable::RowAdding() {
  DataUpdate(true);
}

void DataTable::RowAdded() {
  ++rows;
  DataUpdate(false);
}

/*
  Header format: 
    "+" indicates concatenated elements
    {} indicates optionally repeated elements
    format is tab-separated, newline-terminated
    data-type codes are as follows:
      | - int (aka "Narrow")
      $ - String
      (none) - float
      @ - byte
    Scalar Header name is as follows:
      type-code+name
    Matrix Header master name is as follows:
      <dimcount{,dimcount}>[0{,0}]+type-code+name
    Matrix Header slave names are as follows:
      [dimval{,dimval}]+type-code+name
      
    ex:
    &StrCol	|IntCol	FloatCol	<1,2>[0,0]@ByteMat [0,1]@ByteMat
*/
void DataTable::SaveHeader(ostream& strm) {
/*TODO
  bool first = true;
  DataArray_impl* da;
  taLeafItr itr;
  String rootnm;
  FOR_ITR_EL(DataArray_impl, da, this->, itr) {
    if (!da->save_to_file) goto cont1;
    // we must precheck for invalid matrix types
    if (da->cell_size() == 0) goto cont1; // TODO: should probably issue a warning
    
    // get root name, which has type info
    rootnm = "";
    switch (da->valType()) {
    case DataArray_impl::VT_STRING:
      rootnm = "$";
      break;
    case DataArray_impl::VT_FLOAT:
      break;
    case DataArray_impl::VT_INT:
      rootnm = "|";
      break;
    case DataArray_impl::VT_BYTE:
      rootnm = "@";
      break;
    default: goto cont1; // unknown
    }
    rootnm = rootnm + da->name;
    if (!first) {
      ostrm << '\t';
      first = false;
    }
    if (da->is_matrix()) { // if matrix, we output master col, then slave cols
      String hdnm = "<"; // for each col, esp for matrix
      
    } else {
      ostrm << rootnm;
    }
    if ((display_labels.size > i) && !display_labels[i].empty())
      hdnm = display_labels[i];
    int wdth = 2;
    if (da->HasDispOption(" NARROW,"))
      wdth = 1;
    LogColumn(strm, hdnm, wdth);
cont1: ;
  }
  strm << "\n";
  strm.flush();
*/
}

void DataTable::SaveData(ostream& strm) {
}

void DataTable::LoadHeader(istream& strm) {
}

void DataTable::LoadData(istream& strm, int max_recs) {
  ResetData();
}

void DataTable::SetColName(const String& col_nm, int col) {
  DataArray_impl* da = GetColData(col);
  if(da != NULL) da->name = col_nm;
}

/*obs void DataTable::SetCols(LogData& ld) {
  int cur_i = 0;		// current item at top level
  int subgp_gpi = 0;		// current subgroup index (of the group)
  int subgp_i = 0;		// current subgroup index (of the items in the group)
  int subgp_max = 0;		// max for current subgroup
  DataTable* subgp = NULL;	// the subgroup

  int ldi;
  StructUpdate(true);
  for (ldi=0; ldi< ld.items.size; ldi++) {
    DataItem* ditem = ld.items.FastEl(ldi);
    if (ditem->vec_n > 1) {
      if (gp.size > subgp_gpi)
	subgp = (DataTable*)gp[subgp_gpi];
      else
	subgp = (DataTable*)NewGp(1);
      subgp_gpi++;
      subgp_max = ditem->vec_n;
      subgp_i = 0;

      subgp->name = ditem->name;	// group gets name of first element
      if(!subgp->name.empty() && (subgp->name[0] == '<'))
	subgp->name = subgp->name.after('>'); // get rid of vector notation

      if(subgp->size > subgp_max)
	subgp->EnforceSize(subgp_max); // trim excess (but don't add -- could be wrong)
      SetFieldHead(ditem_i);
    } else {
      if (subgp != NULL) {	// in a subgroup
	subgp_i++;		// increment the index
	if (subgp_i >= subgp_max) { // done with this group
	  subgp = NULL;
	  SetFieldHead(ditem, this, cur_i);
	  cur_i++;
	} else {			// get item from this group
	  SetFieldHead(ditem_i);
	}
      } else {			// in top-level group
	SetFieldHead(ditem, this, cur_i);
	cur_i++;
      }
    }
  }
  if(size > cur_i)		// keep it the same size
    EnforceSize(cur_i);
  if (gp.size > subgp_gpi)	// keep it the same size
    gp.EnforceSize(subgp_gpi);
  StructUpdate(false);
} */

void DataTable::SetSaveToFile(bool save_to_file) {
  taLeafItr i;
  DataArray_impl* ar;
  FOR_ITR_EL(DataArray_impl, ar, this->, i) {
    ar->save_to_file = save_to_file;
  }
}

void DataTable::SetValAsFloat(float val, int col, int row) {
  DataArray_impl* da = GetColData(col);
  if (!da) return;
  if (da->is_matrix) return;
  int i;
  if (idx(row, da->rows(), i)) {
    da->SetValAsFloat(val, i);
  }
}

void DataTable::SetValAsString(const String& val, int col, int row) {
  DataArray_impl* da = GetColData(col);
  if (!da) return;
  if (da->is_matrix) return;
  int i;
  if (idx(row, da->rows(), i)) {
    da->SetValAsString(val, i);
  }
}

void DataTable::SetValAsVar(const Variant& val, int col, int row) {
  DataArray_impl* da = GetColData(col);
  if (!da) return;
  if (da->is_matrix) return;
  int i;
  if (idx(row, da->rows(), i)) {
    da->SetValAsVar(val, i);
  }
}


void DataTable::UpdateAllRanges() {
/*TODO
  taLeafItr i;
  DataArray_impl* ar;
  FOR_ITR_EL(DataArray_impl, ar, this->, i) {
    if(ar->InheritsFrom(TA_float_Data)) {
      ((float_Matrix*)ar->AR())->UpdateAllRange();
    }
  } */
}




//////////////////////////
// 	ClustNode	//
//////////////////////////

void ClustLink::Initialize() {
  dist = 0.0f;
  node = NULL;
}

void ClustLink::Copy_(const ClustLink& cp) {
  dist = cp.dist;
  taBase::SetPointer((TAPtr*)&node, cp.node);
}

void ClustLink::CutLinks() {
  taBase::DelPointer((TAPtr*)&node);
  taBase::CutLinks();
}

void ClustNode::Initialize() {
  pat = NULL;
  leaf_idx = -1;
  leaf_max = -1;
  leaf_dists = NULL;
  par_dist = 0.0f;
  nn_dist = 0.0f;
  tmp_dist = 0.0f;
  y = 0.0f;
}

void ClustNode::InitLinks() {
  taNBase::InitLinks();
  taBase::Own(children, this);
  taBase::Own(nns, this);
}

void ClustNode::CutLinks() {
  taBase::DelPointer((TAPtr*)&leaf_dists);
  taBase::DelPointer((TAPtr*)&pat);
  children.Reset();
  nns.Reset();
  taNBase::CutLinks();
}

void ClustNode::SetPat(float_RArray* pt) {
  taBase::SetPointer((TAPtr*)&pat, pt);
}

void ClustNode::AddChild(ClustNode* nd, float dst) {
  ClustLink* lk = new ClustLink;
  taBase::SetPointer((TAPtr*)&(lk->node), nd);
  lk->dist = dst;
  children.Add(lk);
}

void ClustNode::LinkNN(ClustNode* nd, float dst) {
  ClustLink* lk = new ClustLink;
  taBase::SetPointer((TAPtr*)&(lk->node), nd);
  lk->dist = dst;
  nns.Add(lk);
}

bool ClustNode::RemoveChild(ClustNode* nd) {
  bool rval = false;
  int i;
  for(i=children.size-1;i>=0;i--) {
    if(GetChild(i) == nd) {
      children.Remove(i);
      rval = true;
    }
  }
  return rval;
}

int ClustNode::FindChild(ClustNode* nd) {
  int i;
  for(i=0;i<children.size;i++) {
    if(GetChild(i) == nd)
      return i;
  }
  return -1;
}

void ClustNode::Cluster(float_RArray::DistMetric metric,
			bool norm, float tol)
{
  if(!float_RArray::LargerFurther(metric)) {
    taMisc::Error("Cluster requires distance metric where larger = further apart");
    return;
  }

  if(children.size <= 1) {
    taMisc::Error("Cluster requires at least 2 items to operate on!");
    return;
  }

  // first get indicies for all leaves, and their distances!
  taBase::SetPointer((TAPtr*)&leaf_dists, new float_RArray);
  for(int i=0; i<children.size; i++) {
    ClustNode* nd = GetChild(i);
    nd->leaf_idx = i;
    nd->leaf_max = children.size;
    taBase::SetPointer((TAPtr*)&(nd->leaf_dists), leaf_dists);
    for(int j=i;j<children.size;j++) {
      ClustNode* ond = GetChild(j);
      float dst = nd->pat->Dist(*(ond->pat), metric, norm, tol);
      leaf_dists->Add(dst);
    }
  }

  do {
    // set nearest neighbor pointers
    NNeighbors(metric, norm, tol);
    // find closest and make a new node
  } while (ClustOnClosest(metric));

  SetParDists(0.0f, metric);

  taBase::DelPointer((TAPtr*)&leaf_dists);
}

void ClustNode::Graph(ostream& strm) {
  SetYs(0.5f);
  nn_dist = .1f;
  Graph_impl(strm);
}

void ClustNode::Graph_impl(ostream& strm) {
  if(pat != NULL) {
    strm << par_dist - nn_dist << " " << y << "\n";
    strm << par_dist << " " << y << " \"" << name << "\"\n";
    strm << par_dist - nn_dist << " " << y << "\n";
    strm << par_dist - nn_dist << " " << y << "\n";
  }
  else {
    strm << par_dist - nn_dist << " " << y << "\n";
    strm << par_dist << " " << y << "\n";

    int i;
    for(i=0; i<children.size; i++) {
      ClustLink* nd = (ClustLink*)children[i];
      nd->node->Graph_impl(strm);
      strm << par_dist << " " << y << "\n";
    }

    strm << par_dist - nn_dist << " " << y << "\n";
  }
}

void ClustNode::XGraph(const String& fnm, const String& title) {
  fstream fh;
  fh.open(fnm, ios::out);
  fh << "TitleText: " << title << "\n";
  Graph(fh);
  fh.close(); fh.clear();
  String cmd = String("xgraph -0 \"\" ") + fnm + "&";
  system(cmd);
}

void ClustNode::GraphData(DataTable* dt) {
  SetYs(0.5f);
  nn_dist = 0.0f;
  dt->Reset();
  dt->NewColFloat("X");
  dt->NewColFloat("Y");
  dt->NewColString("label");
  dt->AddColDispOpt("DISP_STRING", 2);
  dt->AddColDispOpt("AXIS=1", 2); // labels use same axis as y values
  dt->AddColDispOpt("STRING_COORDS=1", 2); // use y values
  GraphData_impl(dt);
  dt->UpdateAllRanges();
/*TODO: ranges
  float_Matrix* xar = dt->GetColFloatArray(0);
  dt->AddColDispOpt(String("MAX=") + String(xar->range.max * 1.15f), 0); // adds extra room for labels

  float_Matrix* yar = dt->GetColFloatArray(1);
  dt->AddColDispOpt(String("MAX=") + String(yar->range.max + .3f), 1); // adds extra room for labels
  dt->AddColDispOpt("MIN=0.2", 1); */
}

void ClustNode::GraphData_impl(DataTable* dt) {
  if(pat != NULL) {
    dt->AddBlankRow();
    dt->SetValAsFloat(par_dist - nn_dist, 0, -1);
    dt->SetValAsFloat(y, 1, -1);

    dt->AddBlankRow();
    dt->SetValAsFloat(par_dist, 0, -1);
    dt->SetValAsFloat(y, 1, -1);
    dt->SetValAsString(name, 2, -1);

    dt->AddBlankRow();
    dt->SetValAsFloat(par_dist - nn_dist, 0, -1);
    dt->SetValAsFloat(y, 1, -1);

    dt->AddBlankRow();
    dt->SetValAsFloat(par_dist - nn_dist, 0, -1);
    dt->SetValAsFloat(y, 1, -1);
  }
  else {
    dt->AddBlankRow();
    dt->SetValAsFloat(par_dist - nn_dist, 0, -1);
    dt->SetValAsFloat(y, 1, -1);

    dt->AddBlankRow();
    dt->SetValAsFloat(par_dist, 0, -1);
    dt->SetValAsFloat(y, 1, -1);

    int i;
    for(i=0; i<children.size; i++) {
      ClustLink* nd = (ClustLink*)children[i];
      nd->node->GraphData_impl(dt);
      dt->AddBlankRow();
      dt->SetValAsFloat(par_dist, 0, -1);
      dt->SetValAsFloat(y, 1, -1);
    }

    dt->AddBlankRow();
    dt->SetValAsFloat(par_dist - nn_dist, 0, -1);
    dt->SetValAsFloat(y, 1, -1);
  }
}

static const float clust_dist_tol = 1.0e-6f;

bool ClustNode::ClustOnClosest(float_RArray::DistMetric) {
  if(children.size < 2)
    return false;		// cannot have any more clustering to do!

  float min_d = FLT_MAX;
  int min_idx=-1;
  int i;
  for(i=0; i<children.size; i++) { // find node with closest neighbors
    ClustNode* nd = GetChild(i);
    if(nd->nn_dist < min_d) {
      min_d = nd->nn_dist;
      min_idx = i;
    }
  }
  if(min_idx < 0) return false;
  // make a new cluster around this node
  ClustNode* nd = GetChild(min_idx);
  ClustNode* new_clust = new ClustNode;
  AddChild(new_clust, min_d);
  // add the min node and its nearest neighbors to the new cluster
  new_clust->AddChild(nd);
  children.Remove(min_idx);
  for(i=0;i<nd->nns.size; i++) {
    ClustLink* nlk = (ClustLink*)nd->nns[i];
    if(fabs(nlk->dist - min_d) > clust_dist_tol)
      continue;
    new_clust->AddChild(nlk->node);
    RemoveChild(nlk->node); // and the nns
  }

  // then finally check if any other nns at min_d have other nns not already obtained
  for(i=0;i<nd->nns.size; i++) {
    ClustLink* nlk = (ClustLink*)nd->nns[i];
    if(fabs(nlk->dist - min_d) > clust_dist_tol)
      continue;
    ClustNode* nn = nlk->node;
    int j;
    for(j=0; j<nn->nns.size; j++) {
      ClustLink* nn_nlk = (ClustLink*)nn->nns[j];
      if(fabs(nn_nlk->dist - min_d) > clust_dist_tol)
	continue;
      if(new_clust->FindChild(nn_nlk->node) < 0) { // not in new clust yet
	new_clust->AddChild(nn_nlk->node);
	RemoveChild(nn_nlk->node); // and remove from main list
      }
    }
  }

  if(children.size < 2)
    return false;		// cannot have any more clustering to do!
  return true;
}

void ClustNode::NNeighbors(float_RArray::DistMetric metric,
		    bool norm, float tol)
{
  int i;
  for(i=0; i<children.size; i++) {
    ClustNode* nd = GetChild(i);
    nd->nns.Reset();
    nd->nn_dist = FLT_MAX;
  }
  for(i=0; i<children.size; i++) {
    ClustNode* nd = GetChild(i);
    float min_d = FLT_MAX;
    int j;
    for(j=i+1; j<children.size; j++) {
      ClustNode* ond = GetChild(j);
      ond->tmp_dist = nd->Dist(*ond, metric, norm, tol);
      min_d = MIN(ond->tmp_dist, min_d);
    }
    for(j=i+1; j<children.size; j++) {
      ClustNode* ond = GetChild(j);
      if(fabs(ond->tmp_dist - min_d) < clust_dist_tol) {
	nd->LinkNN(ond, min_d);	// link together with distance
	ond->LinkNN(nd, min_d);
      }
    }
  }
  // now make a 2nd pass and get smallest distance for each node and its neighbors
  for(i=0; i<children.size; i++) {
    ClustNode* nd = GetChild(i);
    nd->nn_dist = FLT_MAX;
    int j;
    for(j=0; j<nd->nns.size; j++) {
      ClustLink* nlk = (ClustLink*)nd->nns[j];
      nd->nn_dist = MIN(nd->nn_dist, nlk->dist);
    }
  }
}

float ClustNode::Dist(const ClustNode& oth, float_RArray::DistMetric metric,
		      bool norm, float tol) const
{
  float rval = 0.0f;
  if(pat != NULL) {
    if(oth.pat != NULL) {
      if(leaf_dists != NULL) {
	rval = leaf_dists->FastTriMatEl(leaf_max, leaf_idx, oth.leaf_idx);
      }
      else {
	rval = pat->Dist(*(oth.pat), metric, norm, tol);
      }
    }
    else {
      rval = oth.Dist(*this, metric, norm, tol);
    }
  }
  else {
    int i;
    for(i=0; i<children.size; i++) {
      ClustNode* nd = GetChild(i);
      if(nd->pat != NULL) {
	if(oth.pat != NULL) {
	  if(leaf_dists != NULL) {
	    rval += leaf_dists->FastTriMatEl(leaf_max, nd->leaf_idx, oth.leaf_idx);
	  }
	  else {
	    rval += nd->pat->Dist(*(oth.pat), metric, norm, tol);
	  }
	}
	else {
	  rval += oth.Dist(*nd, metric, norm, tol);
	}
      }
      else {
	rval += nd->Dist(oth, metric, norm, tol);
      }
    }
    if(children.size > 1)
      rval /= (float)children.size;
  }
  return rval;
}

void ClustNode::SetYs(float y_init) {
  static float global_y;
  if(y_init != -1.0f)
    global_y = y_init;
  if(pat == NULL) {
    float y_avg = 0.0f;
    int i;
    for(i=0; i<children.size; i++) {
      ClustNode* nd = GetChild(i);
      nd->SetYs();
      y_avg += nd->y;
    }
    if(children.size > 1)
      y_avg /= (float)children.size; // average of all y's of kids
    y = y_avg;
  }
  else {
    y = global_y;
    global_y += 1.0f;
  }
}

float ClustNode::SetParDists(float par_d, float_RArray::DistMetric metric) {
  par_dist = par_d + nn_dist;
  float max_d = -FLT_MAX;
  int i;
  for(i=0; i<children.size; i++) {
    ClustNode* nd = GetChild(i);
    float dst = nd->SetParDists(par_dist, metric);
    max_d = MAX(dst, max_d);
  }
  return max_d;
}

//////////////////////////
// 	DA View Specs	//
//////////////////////////

void DA_ViewSpec::Initialize(){
  visible = true;
  data_array = NULL;
}

void DA_ViewSpec::CutLinks() {
  taBase::DelPointer((TAPtr*)&data_array);
  taNBase::CutLinks();
}

void DA_ViewSpec::Copy_(const DA_ViewSpec& cp) {
  display_name = cp.display_name;
  visible = cp.visible;
}

void DA_ViewSpec::UpdateAfterEdit() {
  taNBase::UpdateAfterEdit();
  if (data_array == NULL) {
    DT_ViewSpec* dtv = GET_MY_OWNER(DT_ViewSpec);
    if(dtv != NULL) {
      int idx = dtv->Find(this);
      if(idx >= 0) {
	DataTable* dt = dtv->data_table;
	if(dt != NULL) {
	  DataArray_impl* da = dt->SafeEl(idx);
	  if(da != NULL)
	    taBase::SetPointer((TAPtr*)&data_array, da);
	}
	else
	  taMisc::Error("*** Null data_table in:", dtv->GetPath());
      }
    }
  }
  if(!(name.empty()) && (display_name.empty()))
    display_name = DA_ViewSpec::CleanName(name);
}

void DA_ViewSpec::UpdateView() {
  DT_ViewSpec* dtv = GET_MY_OWNER(DT_ViewSpec);
  if(dtv != NULL) dtv->UpdateAfterEdit();
}

void DA_ViewSpec::SetGpVisibility(bool vis) {
  DT_ViewSpec* dtv = GET_MY_OWNER(DT_ViewSpec);
  if(dtv != NULL) dtv->SetVisibility(vis);
}

String DA_ViewSpec::ValAsString(int row) {
  if (data_array)
  //TODO: go through a format string, if supplied
    return data_array->GetValAsString(row);
  else
    return _nilString;
}

void DA_ViewSpec::CopyToGp(MemberDef* md) {
  if(md == NULL) return;
  DT_ViewSpec* dtv = GET_MY_OWNER(DT_ViewSpec);
  if(dtv == NULL) return;
  int i;
  for(i=0;i<dtv->size;i++) {
    DA_ViewSpec* vs = (DA_ViewSpec*)dtv->FastEl(i);
    if(!vs->InheritsFrom(GetTypeDef())) continue;
    md->CopyFromSameType((void*)vs, (void*)this);
  }
  UpdateView();
}

bool DA_ViewSpec::BuildFromDataArray(DataArray_impl* nda) {
  bool first_time = false;
  if (data_array == NULL) {
    first_time = true;
  }
  if(nda != NULL) {
    taBase::SetPointer((TAPtr*)&data_array, nda);
  }
  if (data_array == NULL)
    UpdateAfterEdit();
  if (data_array == NULL)	// still..
    return false;

  // always replace name and throwout old data
  // don't throw out the old data if we are just now creating this
  // view since this results in the user not being able to create
  // an additional view of data in a current view. Since the names
  // will never originally match up, the array's were always being reset.

  if (name != data_array->name) {
    if (!first_time)
      data_array->AR()->Reset();
    name = data_array->name;
    display_name = DA_ViewSpec::CleanName(name);
  }
  // only copy display options first time, since user may override in view
  if (first_time) {
    if (data_array->HasDispOption(" HIDDEN,"))
      visible = false;
  }
  return true;
}


String DA_ViewSpec::CleanName(String& name){
  String result = name;
  // <xxx> is a comment and should be removed
  if(result.empty()) return result;
  int pos;
  if(((pos = result.index('<')) != -1) && ((pos = result.index('>',pos+1)) != -1))
    result = result.after(pos);
  if(result.empty()) return result;
  if((result[0] == '|') || (result[0] == '$'))
    result = result.after(0);
  return result;
}

//////////////////////////
// 	DT View Specs	//
//////////////////////////

void DT_ViewSpec::Initialize(){
  data_table = NULL;
  visible = true;
#ifdef TA_GUI
  def_font.pointSize = 8;
#endif
}

void DT_ViewSpec::Destroy(){
  CutLinks();
}

void DT_ViewSpec::InitLinks() {
  inherited::InitLinks();
#ifdef TA_GUI
  taBase::Own(def_font, this);
#endif
}

void DT_ViewSpec::CutLinks() {
#ifdef TA_GUI
  def_font.CutLinks();
#endif
  taBase::DelPointer((TAPtr*)&data_table);
  inherited::CutLinks();
}

void DT_ViewSpec::Copy_(const DT_ViewSpec& cp) {
  display_name = cp.display_name;
  visible = cp.visible;
#ifdef TA_GUI
  def_font = cp.def_font;
#endif
}

void DT_ViewSpec::UpdateAfterEdit(){
  inherited::UpdateAfterEdit();
  BuildFromDataTable(data_table);
  if(!(name.empty()) && (display_name.empty()))
    display_name = DA_ViewSpec::CleanName(name);
  TAPtr own = GetOwner();
  while((own != NULL) && (own->InheritsFrom(&TA_taList_impl))){
    own = own->GetOwner();
  }
  if(own != NULL){
    own->UpdateAfterEdit();
  }
}

void DT_ViewSpec::SetVisibility(bool vis) {
  visible = vis;
  int i;
  for(i=0;i<size;i++) {
    DA_ViewSpec* vs = (DA_ViewSpec*)FastEl(i);
    vs->visible = visible;	// always propagate
  }
  UpdateAfterEdit();
}

bool DT_ViewSpec::BuildFromDataTable(DataTable* tdt){
  if(tdt != NULL) {
    taBase::SetPointer((TAPtr*)&data_table, tdt);
  }
  if(data_table == NULL)
    return false;

  bool same_size = ((size == data_table->size) && (leaves == data_table->leaves)
    && (gp.size == data_table->gp.size));

  bool same_name = (name == data_table->name);

  if(same_size && same_name) {
    return false;
  }

  ReBuildFromDataTable();
  return true;
}

void DT_ViewSpec::ReBuildFromDataTable() {
  // first save the current view information, then try to update based on old info
  DT_ViewSpec* old = (DT_ViewSpec*)Clone();

  int delta = (int)fabs(float(data_table->size - size)); // change in size
  delta = MAX(delta, 4);	// minimum look-ahead
  int gp_delta = (int)fabs(float(data_table->gp.size - gp.size)); // change in size
  gp_delta = MAX(gp_delta, 4);

  // ensure number of elements is the same;
  EnforceSize(data_table->size);

  name = data_table->name;	// always get the name
  if(!name.empty() && display_name.empty())
    display_name = DA_ViewSpec::CleanName(name);

  int old_i = 0;
  int i;
  for(i=0;i<size;i++) {
    DataArray_impl* nda =     ((DataArray_impl *) data_table->FastEl(i));
    DA_ViewSpec*    ndavs = (DA_ViewSpec *) FastEl(i);
    ndavs->BuildFromDataArray(nda);

    if(old_i < old->size) {
      DA_ViewSpec* oldvs = (DA_ViewSpec *)old->FastEl(old_i);
      if(oldvs->name == nda->name) { // simple match, done
	old_i++;
	ndavs->CopyFrom(oldvs); // get the info from the old guy
	continue;
      }
      // look ahead for other options
      int j;
      for(j=old_i;(j<=old_i+delta) && (j < old->size);j++) {
	oldvs = (DA_ViewSpec *)old->FastEl(j);
	if(oldvs->name == ndavs->name) { // simple match, done
	  old_i = j;
	  ndavs->CopyFrom(oldvs); // get the info from the old guy
	  continue;
	}
      }
    }
  }

  // ensure the groups matchup
  gp.el_base = gp.el_typ = GetTypeDef();
  gp.EnforceSize(data_table->gp.size);

  old_i = 0;
  for(i=0;i<data_table->gp.size;i++){
    DT_ViewSpec* nvs = (DT_ViewSpec *) FastGp(i);
    DataTable* ndt = (DataTable *) data_table->FastGp(i);
    nvs->el_base = nvs->el_typ = el_typ;
    if(!nvs->BuildFromDataTable(ndt))
      nvs->ReBuildFromDataTable(); // make sure it is rebuilt!

    if(old_i < old->gp.size) {
      DT_ViewSpec* oldvs = (DT_ViewSpec *)old->FastGp(old_i);
      if(oldvs->name == nvs->name) { // simple match, done
	old_i++;
	nvs->CopyFrom(oldvs); // get the info from the old guy
	nvs->EnforceSize(ndt->size); // make sure we don't get bigger from the copy!!
	continue;
      }
      int j;
      for(j=old_i;(j<=old_i+gp_delta) && (j < old->gp.size);j++) {
	oldvs = (DT_ViewSpec *)old->FastGp(j);
	if(oldvs->name == nvs->name) { // simple match, done
	  old_i = j;
	  nvs->CopyFrom(oldvs); // get the info from the old guy
	  nvs->EnforceSize(ndt->size); // make sure we don't get bigger from the copy!!!
	  continue;
	}
      }
    }
  }

  delete old;
}

void DT_ViewSpec::SetDispNms(const String& base_name) {
  display_name = base_name;
  int i;
  for(i=0;i<size;i++) {
    DA_ViewSpec* vs = (DA_ViewSpec*)FastEl(i);
    vs->display_name = base_name;
    vs->display_name += String("_") + String(i);
  }
}

void DT_ViewSpec::RmvNmPrefix() {
  int i;
  for(i=0;i<size;i++) {
    DA_ViewSpec* vs = (DA_ViewSpec*)FastEl(i);
    if(vs->display_name.contains('_')) vs->display_name = vs->display_name.after('_');
  }
}

//////////////////////////////////
// 	DA Text View Specs	//
//////////////////////////////////

void DA_TextViewSpec::Initialize() {
  width = 2;
}

void DA_TextViewSpec::Destroy() {

}

bool DA_TextViewSpec::BuildFromDataArray(DataArray_impl* tda) {
  bool result = DA_ViewSpec::BuildFromDataArray(tda);
  if (data_array == NULL) return result;

  //set width as greater of static width, and current widest value -- this is aproximate
  width = MAX(data_array->displayWidth(), ((data_array->maxColWidth() + 4) / 8));
  return result;
}

//////////////////////////////////
// 	DA Net View Specs	//
//////////////////////////////////

void DA_NetViewSpec::Initialize() {
  label_index = -1;
}

void DA_NetViewSpec::Destroy() {
}




//////////////////////////////////
// 	DA Grid View Specs	//
//////////////////////////////////

void DA_GridViewSpec::Initialize(){
  display_style = BLOCK;
  scale_on = true;
}

void DA_GridViewSpec::Destroy() {

}

bool DA_GridViewSpec::BuildFromDataArray(DataArray_impl* nda){
  bool result = DA_ViewSpec::BuildFromDataArray(nda);
  if(data_array == NULL)
    return result;
  if(data_array->HasDispOption(" TEXT,"))
    display_style = TEXT;
  else if(data_array->HasDispOption(" NARROW,"))
    display_style = TEXT;		// use text for narrow items as a default..
  else if(data_array->InheritsFrom(TA_String_Data))
    display_style = TEXT;
  return result;
}

void DA_GridViewSpec::InitLinks(){
  DA_ViewSpec::InitLinks();
  taBase::Own(pos, this);
}

void DA_GridViewSpec::Copy_(const DA_GridViewSpec& cp) {
  pos = cp.pos;
  display_style = cp.display_style;
}


//////////////////////////////////
// 	DT Grid View Specs	//
//////////////////////////////////

void DT_GridViewSpec::Initialize(){
  use_gp_name = true;
  display_style = DA_GridViewSpec::BLOCK;
  layout = LFT_RGT_BOT_TOP;
  scale_on = true;
  customized = false;
}

void DT_GridViewSpec::Destroy() {
}

void DT_GridViewSpec::Reset() {
  DT_ViewSpec::Reset();
  customized = false;
}

void DT_GridViewSpec::InitLinks(){
  DT_ViewSpec::InitLinks();
  taBase::Own(pos, this);
  taBase::Own(geom, this);
  taBase::Own(full_geom, this);
}

void DT_GridViewSpec::Copy_(const DT_GridViewSpec& cp) {
  pos = cp.pos;
  geom = cp.geom;
  full_geom = cp.full_geom;
  use_gp_name = cp.use_gp_name;
  display_style = cp.display_style;
}

bool DT_GridViewSpec::BuildFromDataTable(DataTable* tdt){
  bool result = DT_ViewSpec::BuildFromDataTable(tdt);
  if(!result) return result;
  if(size <= 0) return result;
  DA_GridViewSpec* vs = (DA_GridViewSpec *) FastEl(0); // first el determines group options!
  if(vs->data_array->disp_opts.contains(" GEOM_X=")) {
    String geo = vs->data_array->disp_opts;
    geo = geo.after(" GEOM_X=");
    String gx = geo.before(',');
    geo = geo.after(" GEOM_Y=");
    String gy = geo.before(',');
    geom.x = (int)gx;
    geom.y = (int)gy;
  }
  if(vs->data_array->HasDispOption(" USE_EL_NAMES,")) {
    use_gp_name = false;
  }
  if(vs->data_array->HasDispOption(" USE_GP_NAME,")) {
    use_gp_name = true;
  }

  if(customized)
    UpdateGeom();
  else
    UpdateLayout();
  return result;
}

void DT_GridViewSpec::UpdateGeom() {
  PosTDCoord gm;
  int i;
  for(i=0;i<size;i++) {
    DA_GridViewSpec* vs = (DA_GridViewSpec *) FastEl(i);
    if(!vs->visible) continue;
    gm.x = MAX(gm.x, (vs->pos.x - pos.x) + 1);	// subtract our own position!
    gm.y = MAX(gm.y, (vs->pos.y - pos.y) + 1);
  }
  full_geom.x = pos.x + gm.x;
  full_geom.y = pos.y + gm.y;
  for(i=0;i<gp.size;i++) {
    DT_GridViewSpec* dt = (DT_GridViewSpec *) FastGp(i);
    if(!dt->visible) continue;
    dt->UpdateGeom();
    full_geom.x = MAX(full_geom.x, dt->full_geom.x);
    full_geom.y = MAX(full_geom.y, dt->full_geom.y);
  }
}

void DT_GridViewSpec::UpdateAfterEdit(){
  DT_ViewSpec::UpdateAfterEdit();
  UpdateGeom();
}

int DT_GridViewSpec::UpdateLayout(MatrixLayout ml) {
  customized = false;
  if(ml == DEFAULT)
    ml = layout;
  else
    layout = ml;		// set current default to be this

  DA_GridViewSpec* da;
  int vis_size = 0;
  int i;
  for(i=0;i<size;i++){
    da = (DA_GridViewSpec *) FastEl(i);
    if(!da->visible) continue;
    vis_size++;
    if(da->display_style == DA_GridViewSpec::TEXT) {
      if(da->data_array->HasDispOption(" NARROW,"))
	vis_size+=1;		// narrow text = 2
      else
	vis_size+=3;		// wide text = 4
    }
  }
  // insufficient geometry so use sqrt
  if((geom.x * geom.y) < vis_size) {
    if(vis_size < 25) {		// make it linear if a reasonable width..
      geom.x = vis_size;
      geom.y = 1;
    }
    else {
      geom.y = (int)sqrt(float(vis_size));
      geom.x = vis_size / geom.y;
      while((geom.x * geom.y) < vis_size)
	geom.x++;
    }
  }
  int loc[2];   int start[2];  int mod[2];
  int limiter = 0;
  int limit[2];  limit[0]= geom.x; limit[1] = geom.y;
  int maxx = 0;
  int extrax = 0;
  switch(ml){
  case LFT_RGT_TOP_BOT:
    loc[0] = start[0] = 0;   loc[1] = start[1] = geom.y-1;
    mod[0] = 1; mod[1] = -1; limiter = 0;
    break;
  case LFT_RGT_BOT_TOP:
    loc[0] = start[0] = 0;   loc[1] = start[1] = 0;
    mod[0] = 1; mod[1] = 1;  limiter = 0;
    break;
  case BOT_TOP_LFT_RGT:
    loc[0] = start[0] = 0;   loc[1] = start[1] = 0;
    mod[0] = 1; mod[1] = 1;  limiter = 1;
    break;
  case TOP_BOT_LFT_RGT:
    loc[0] = start[0] = 0;   loc[1] = start[1] = geom.y-1;
    mod[0] = 1; mod[1] = -1; limiter = 1;  limit[1] = -1;
    break;
  case DEFAULT:			// just to shut up the compiler
    break;
  }
  for(i=0;i<size;i++){
    da = (DA_GridViewSpec *) FastEl(i);
    if(!da->visible) continue;
    da->pos.x = pos.x + loc[0];
    da->pos.y = pos.y + loc[1];
    if(da->display_style == DA_GridViewSpec::TEXT) {
      int incv = 3;
      if(da->data_array->HasDispOption(" NARROW,"))
	incv = 1;
      if(limiter == 0)
	loc[0] += incv;
      else
	extrax = MAX(incv, extrax);
    }
    maxx = MAX(maxx,loc[0]);
    loc[limiter] += mod[limiter];
    if(loc[limiter] == limit[limiter]){
      loc[limiter] = start[limiter];
      loc[!limiter] += mod[!limiter];
      if(limiter == 1)
	loc[0] += mod[0] * extrax;
      extrax = 0;
    }
  }
  loc[0] = maxx + 2;
  // now layout the subgroups to the right
  for(i=0;i<gp.size;i++){
    DT_GridViewSpec* dt = (DT_GridViewSpec*) gp.FastEl(i);
    if(!dt->visible) continue;
    dt->pos.x = pos.x + loc[0];
    dt->pos.y = pos.y;		// same position as us
    loc[0] += dt->UpdateLayout() + 2;	// always use default! and add a space between subsequent subgroups
    maxx = MAX(maxx,loc[0]);
  }
  UpdateGeom();
  return maxx;
}

void DT_GridViewSpec::GetMinMaxScale(MinMax& mm, bool first) {
  static bool frst;
  if(first)
    frst = true;
  int i;
  for(i=0;i<size;i++){
    DA_GridViewSpec* vs = (DA_GridViewSpec *) FastEl(i);
    if(!vs->visible || !vs->scale_on || (vs->display_style == DA_GridViewSpec::TEXT)) continue;
/*nn    if(vs->data_array->AR() == NULL) {
      vs->data_array->NewAR();
      continue;
    }*/
    if(!vs->data_array->InheritsFrom(&TA_float_Data)) continue;
    float_RArray* ar = (float_RArray*)vs->data_array->AR();
    if(frst) {
      frst = false;
      mm.max = ar->range.max;
      mm.min = ar->range.min;
    }
    else {
      mm.UpdateRange(ar->range);
    }
  }
  for(i=0;i<gp.size;i++){
    DT_GridViewSpec* dt = (DT_GridViewSpec*) gp.FastEl(i);
    if(!dt->visible || !dt->scale_on) continue;
    dt->GetMinMaxScale(mm, frst);
  }
}

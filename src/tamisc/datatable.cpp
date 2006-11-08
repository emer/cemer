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
  // todo: Agg changed
//   if(end == -1)	end = size;  else end = MIN(size, end);
//   end = MIN(end, oth.size);
//   int i;
//   for(i=start;i<end;i++) {
//     double tmp_me = FastEl(i);
//     agg.ComputeAggNoUpdt(tmp_me, oth.FastEl(i));
//     FastEl(i) = tmp_me;
//   }
  ItemsChanged_();
}

float float_RArray::AggToVal(Aggregate& agg, int start, int end) const {
  // todo: Agg changed
//   if(end == -1)	end = size;  else end = MIN(size, end);
//   double rval = agg.InitAggVal();
//   agg.Init();
//   int i;
//   for(i=start;i<end;i++)
//     agg.ComputeAgg(rval, FastEl(i));
//   return rval;
  return 0.0;
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
//	DataTable	//
//////////////////////////

/* 3.2 routines:

void DataTable::AddArrayToRow(float_RArray& tar) {
  if(tar.size == 0)	return;
  int n = 1;
  RowsAdding(n, true);
  int cnt = 0;
  taLeafItr i;
  DataArray_impl* ar;
  FOR_ITR_EL(DataArray_impl, ar, data., i) {
    if(ar->InheritsFrom(TA_float_Data))
      ((float_RArray*)ar->AR())->Add(tar.FastEl(cnt));
    cnt++;
    if(cnt >= tar.size)	break;
  }
  RowsAdding(n, false);
}

void DataTable::AddRowToArray(float_RArray& tar, int row_num) const {
  taLeafItr i;
  DataArray_impl* ar;
  FOR_ITR_EL(DataArray_impl, ar, data., i) {
    float val = 0;
    if(ar->InheritsFrom(TA_float_Data))
      val = ((float_RArray*)ar->AR())->SafeEl(row_num);
    tar.Add(val);
  }
}

void DataTable::AggRowToArray(float_RArray& tar, int row_num, Aggregate& agg) const {
  if (tar.size < cols())
    tar.Insert(0, tar.size, cols() - tar.size);
  int cnt = 0;
  taLeafItr i;
  DataArray_impl* ar;
  FOR_ITR_EL(DataArray_impl, ar, data., i) {
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
  FOR_ITR_EL(DataArray_impl, ar, data., i) {
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
  FOR_ITR_EL(DataArray_impl, ar, data., i) {
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

void DataTable::PutArrayToCol(const float_RArray& ar, int col) {
/ *TODO
  float_Matrix* far = GetColFloatArray(col);
  if (far != NULL) {
    far->CopyFrom((taBase*)&ar);
  }* /
} 

void DataTable::PutArrayToRow(const float_RArray& tar, int row_num) {
  if (tar.size == 0)	return;
  int cnt = 0;
  taLeafItr i;
  DataArray_impl* ar;
  FOR_ITR_EL(DataArray_impl, ar, data., i) {
    if (ar->InheritsFrom(TA_float_Data)) {
      ar->SetValAsFloat(tar.FastEl(cnt), row_num);
    }
    ++cnt;
    if (cnt >= tar.size) break;
  }
}

*/


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
  inherited::InitLinks();
  taBase::Own(children, this);
  taBase::Own(nns, this);
}

void ClustNode::CutLinks() {
  taBase::DelPointer((TAPtr*)&leaf_dists);
  taBase::DelPointer((TAPtr*)&pat);
  children.Reset();
  nns.Reset();
  inherited::CutLinks();
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
  DataArray_impl* da = dt->NewColString("label");
  da->SetUserData("DISP_STRING", true);
  da->SetUserData("AXIS", 1); // labels use same axis as y values
  da->SetUserData("STRING_COORDS", 1); // use y values
  GraphData_impl(dt);
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


//////////////////////////////////
// 	DA Grid View Specs	//
//////////////////////////////////

void GridColViewSpec::Initialize(){
  display_style = TEXT; // updated later in build
  text_width = 16;
  num_prec = 5;
  mat_layout = BOT_ZERO; // typical default for data patterns
  scale_on = true;
  col_width = 0.0f;
  row_height = 0.0f;
}

void GridColViewSpec::Copy_(const GridColViewSpec& cp){
  display_style = cp.display_style;
  text_width = cp.text_width;
  num_prec = cp.num_prec;
  mat_layout = cp.mat_layout;
  scale_on = cp.scale_on;
  // others recalced
}

void GridColViewSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if (text_width < 2) text_width = 2; // smallest practical
  if (num_prec < 2) num_prec = 2;
}

void GridColViewSpec::UpdateFromDataCol_impl(bool first){
  inherited::UpdateFromDataCol_impl(first);
  DataArray_impl* dc = dataCol(); //note: exists, because we were called
  if (first) {
    // just get the display width, don't worry about maxwidth
    text_width = dc->displayWidth();
  
    if (dc->isMatrix() && dc->isNumeric()) {
      if (dc->GetUserData("IMAGE").toBool()) {
        display_style = IMAGE;
        mat_layout = BOT_ZERO;
      } else {
        display_style = BLOCK;
        mat_layout = BOT_ZERO;
      }
    } else /*obs  if (dc->GetUserData("TEXT").toBool() ||
      dc->GetUserData(DataArray_impl::udkey_narrow).toBool() ||
      dc->InheritsFrom(TA_String_Data)
    )*/ {
      display_style = TEXT;
    }
  }
}

void GridColViewSpec::DataColUnlinked() {
  col_width = 0.0f;
  row_height = 0.0f;
}

void GridColViewSpec::Render_impl() {
  inherited::Render_impl(); // prob nothing
  //NOTE: we just calc everything in points, then adjust at the end
  // cache some params
  GridTableViewSpec* par = parent();
  DataArray_impl* dc = dataCol(); // cache
  col_width = 0.0f;
  row_height = 0.0f;
  if (!dc) return;
  
  float blk_pts = par->mat_block_pts; 
  float brd_pts = par->mat_border_pts; 
  float fnt_pts = par->font.pointSize;
  // first stab at col width is the normal size text, for scalar width
  float min_col_wd = fnt_pts * text_width;
  if (dc->isMatrix()) // shrink font for mats
    fnt_pts *=  par->mat_font_scale; 
  
  // get 2d equivalent cell geom values
  iVec2i cg;
  dc->Get2DCellGeom(cg); //note: 1x1 for scalar
  float row_ht = 0.0f;
  float col_wd = 0.0f;
  float tmp; // to avoid multi-calcs in min/max 
  if (display_style & BLOCK_MASK) {
    col_wd += (blk_pts * cg.x) + (brd_pts * (cg.x - 1));
    tmp =  (blk_pts * cg.y) + (brd_pts * (cg.y - 1));
    row_ht = MAX(row_ht, tmp);
  }
  if (display_style == TEXT_AND_BLOCK) {
    col_wd += par->mat_sep_pts;
  }
  if (display_style & TEXT_MASK) {
    col_wd += (fnt_pts * text_width * cg.x) + (brd_pts * (cg.x - 1));
    // row height, and number of rows -- ht ~ 12/8 x wd
    tmp = (fnt_pts * cg.y * t3Misc::char_ht_to_wd_pts) +
      (brd_pts * (cg.y - 1));
    row_ht = MAX(row_ht, tmp);
  }
  if (display_style == IMAGE) {
    float px_pts = par->pixel_pts; 
    tmp = px_pts * cg.x;
    col_wd = MAX(col_wd, tmp);
    row_ht += px_pts * cg.y;
  }
  col_wd = MAX(col_wd, min_col_wd);
  // change to geoms
  col_width = col_wd * t3Misc::geoms_per_pt;
  row_height = row_ht * t3Misc::geoms_per_pt;
}


//////////////////////////////////
//  GridTableViewSpec		//
//////////////////////////////////

void GridTableViewSpec::Initialize() {
  col_specs.SetBaseType(&TA_GridColViewSpec);
  grid_margin_pts = 4.0f;
  grid_line_pts = 3.0f;
  SetMatSizeModel_impl(SMALL_BLOCKS);
}

void GridTableViewSpec::Destroy() {
}

void GridTableViewSpec::Copy_(const GridTableViewSpec& cp) {
  grid_margin_pts = cp.grid_margin_pts;
  grid_line_pts = cp.grid_line_pts;
  metrics_model = cp.metrics_model;
  mat_block_pts = cp.mat_block_pts;
  mat_border_pts = cp.mat_border_pts;
  mat_sep_pts = cp.mat_sep_pts;
  mat_font_scale = cp.mat_font_scale;
  pixel_pts = cp.pixel_pts;
}

void GridTableViewSpec::UpdateAfterEdit_impl(){
  inherited::UpdateAfterEdit_impl();
  // just blindly enforce all minimums
  if (grid_margin_pts < 0.0f) grid_margin_pts = 0.0f;
  if (grid_line_pts <  0.1f) grid_line_pts =  0.1f;
  if (mat_block_pts < 0.1f) mat_block_pts = 0.1f;
  if (mat_border_pts < 0.0f) mat_border_pts = 0.0f;
  if (mat_sep_pts < 0.0f) mat_sep_pts = 0.0f;
  if (mat_font_scale < 0.1f) mat_font_scale = 0.1f;
  if (pixel_pts < 0.1f) pixel_pts = 0.1f;
  // now, unconditionally apply any model
  SetMatSizeModel_impl(mat_size_model);
}

void GridTableViewSpec::DataDataChanged_impl(int dcr, void* op1, void* op2) {
  inherited::DataDataChanged_impl(dcr, op1, op2);
  TableView* tv = GET_MY_OWNER(TableView);
  if (!tv) return;
  tv->DataChanged_DataTable(dcr, op1, op2);
}

void GridTableViewSpec::DataDestroying() {
  TableView* tv = GET_MY_OWNER(TableView);
  if (tv) {
    tv->DataChanged_DataTable(DCR_ITEM_DELETING, NULL, NULL);
  }
  inherited::DataDestroying();
}

void GridTableViewSpec::GetMinMaxScale(MinMax& mm, bool first) {
/*TODO NOTE: this "frst" business is BROKEN -- can't use global static this way!!!
  static bool frst;
  if(first)
    frst = true; */
  int i;
  for (i=0;i< col_specs.size; i++){
    GridColViewSpec* vs = (GridColViewSpec *) col_specs.FastEl(i);
    if (!(vs->visible && vs->scale_on &&
      (vs->display_style & GridColViewSpec::BLOCK_MASK)))
      continue;
    if (!vs->dataCol()->isNumeric()) continue; // shouldn't happen!
/*BROKEN    if(frst) {
      frst = false;
      mm.max = ar->range.max;
      mm.min = ar->range.min;
    }
    else {
      mm.UpdateRange(ar->range);
    } */
  }
//TODO: need to add ranging to datatable
//TEMP
  mm.min = -1.0f;
  mm.max = 1.0f;
}

void GridTableViewSpec::SetMatSizeModel_impl(MatSizeModel mm){
  mat_size_model = mm;
  switch (mm) {
  case CUSTOM_METRICS: break;
  case SMALL_BLOCKS:
    mat_block_pts = 2.0f;
    mat_border_pts = 0.5f;
    mat_sep_pts = 2.0f;
    mat_font_scale = 0.5f;
    pixel_pts = 0.5f;
    break;
  case MEDIUM_BLOCKS:
    mat_block_pts = 8.0f;
    mat_border_pts = 2.0f;
    mat_sep_pts = 4.0f;
    mat_font_scale = 0.75f;
    pixel_pts = 2.0f;
    break;
  case LARGE_BLOCKS:
    mat_block_pts = 16.0f;
    mat_border_pts = 4.0f;
    mat_sep_pts = 6.0f;
    mat_font_scale = 0.8f;
    pixel_pts = 4.0f;
    break;
  }
}

/* -*- C++ -*- */
/*=============================================================================
//									      //
// This file is part of the TypeAccess/C-Super-Script software package.	      //
//									      //
// Copyright (C) 1995 Randall C. O'Reilly, Chadley K. Dawson, 		      //
//		      James L. McClelland, and Carnegie Mellon University     //
//     									      //
// Permission to use, copy, modify, and distribute this software and its      //
// documentation for any purpose is hereby granted without fee, provided that //
// the above copyright notice and this permission notice appear in all copies //
// of the software and related documentation.                                 //
// 									      //
// Note that the PDP++ software package, which contains this package, has a   //
// more restrictive copyright, which applies only to the PDP++-specific       //
// portions of the software, which are labeled as such.			      //
//									      //
// Note that the taString class, which is derived from the GNU String class,  //
// is Copyright (C) 1988 Free Software Foundation, written by Doug Lea, and   //
// is covered by the GNU General Public License, see ta_string.h.             //
// The iv_graphic library and some iv_misc classes were derived from the      //
// InterViews morpher example and other InterViews code, which is             //
// Copyright (C) 1987, 1988, 1989, 1990, 1991 Stanford University             //
// Copyright (C) 1991 Silicon Graphics, Inc.				      //
//									      //
// THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,         //
// EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY 	      //
// WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  	      //
// 									      //
// IN NO EVENT SHALL CARNEGIE MELLON UNIVERSITY BE LIABLE FOR ANY SPECIAL,    //
// INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND, OR ANY DAMAGES  //
// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER OR NOT     //
// ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF LIABILITY,      //
// ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS        //
// SOFTWARE. 								      //
==============================================================================*/

// aggregate.cc

#include "aggregate.h"

#include <math.h>
#include <limits.h>
#include <float.h>

//////////////////////////
//  	CountParam     	//
//////////////////////////

void CountParam::Initialize() {
  rel = LESSTHANOREQUAL;
  val = 0.0f;
}

void CountParam::Copy_(const CountParam& cp) {
  rel = cp.rel;
  val = cp.val;
}

bool CountParam::Evaluate(float cmp) const {
  switch(rel) {
  case EQUAL:
    if(cmp == val)	return true;
    break;
  case NOTEQUAL:
    if(cmp != val)	return true;
    break;
  case LESSTHAN:
    if(cmp < val)	return true;
    break;
  case GREATERTHAN:
    if(cmp > val)	return true;
    break;
  case LESSTHANOREQUAL:
    if(cmp <= val)	return true;
    break;
  case GREATERTHANOREQUAL:
    if(cmp >= val)	return true;
    break;
  }
  return false;
}


//////////////////////////
//  	Aggregate    	//
//////////////////////////

void Aggregate::Initialize() {
  op = SUM;
  no0 = false;
  n_updt = 0;
}

void Aggregate::InitLinks() {
  taOBase::InitLinks();
  taBase::Own(count, this);
}

void Aggregate::Destroy() {
}

void Aggregate::Copy_(const Aggregate& cp) {
  op = cp.op;
  count = cp.count;
  n_updt = cp.n_updt;
}

void Aggregate::Init() {
  n_updt = 0;
}

float Aggregate::InitAggVal() const {
  if(op == Aggregate::MIN)
    return FLT_MAX;
  if(op == Aggregate::MAX)
    return -FLT_MAX;
  if(op == Aggregate::PROD)
    return 1.0;
  return 0;
}

bool Aggregate::ComputeAggNoUpdt(float& to, float fm) {
  if(no0 && (fm == 0.0f)) return false;
  switch(op) {
  case DEFAULT:
  case LAST:	AggLAST(to,fm); break;
  case SUM:	AggSUM(to,fm); 	break;
  case PROD:	AggPROD(to,fm); break;
  case MIN:	AggMIN(to,fm); 	break;
  case MAX:	AggMAX(to,fm); 	break;
  case AVG:	AggAVG(to,fm); 	break;
  case COPY:	AggCOPY(to,fm); break;
  case COUNT: 	AggCOUNT(to,fm);break;
  }
  if(fm != 0.0f) return true;
  return false;
}

void Aggregate::ComputeAgg(float& to, float fm) {
  ComputeAggNoUpdt(to, fm);
  if(!no0 || (fm != 0.0f))
    n_updt++;
}

const char* Aggregate::GetAggName() const {
  switch(op) {
  case DEFAULT:
  case LAST:	return "lst";
  case SUM:	return "sum";
  case PROD:	return "prd";
  case MIN:	return "min";
  case MAX:	return "max";
  case AVG:	return "avg";
  case COPY:	return "cpy";
  case COUNT: 	return "cnt";
  }
  return "n/a";
}

String Aggregate::AppendAggName(const char* nm) const {
  String rval = nm;
  rval += ":";
  rval += GetAggName();
  return rval;
}

String Aggregate::PrependAggName(const char* nm) const {
  String rval = GetAggName();
  rval += "_";
  rval += nm;
  return rval;
}


//////////////////////////
//  	SimpleMathSpec 	//
//////////////////////////

void SimpleMathSpec::Initialize() {
  opr = NONE;
  arg = 0.0f;
  lw = -1.0f;
  hi = 1.0f;
}

void SimpleMathSpec::Copy_(const SimpleMathSpec& cp) {
  opr = cp.opr;
  arg = cp.arg;
  lw = cp.lw;
  hi = cp.hi;
}

float SimpleMathSpec::Evaluate(float val) const {
  switch(opr) {
  case NONE:
    return val;
  case THRESH:
    return (val >= arg) ? hi : lw;
  case ABS:
    return fabsf(val);
  case SQUARE:
    return val * val;
  case SQRT:
    return sqrtf(val);
  case LOG:
    return logf(val);
  case LOG10:
    return ((val <= 0) ? FLT_MIN_10_EXP : log10(val));
  case EXP:
    return expf(val);
  case ADD:
    return val + arg;
  case SUB:
    return val - arg;
  case MUL:
    return val * arg;
  case DIV:
    return val / arg;
  case POWER:
    return powf(val, arg);
  case GTEQ:
    return MAX(val, arg);
  case LTEQ:
    return MIN(val, arg);
  case GTLTEQ:
    val = MIN(val, hi);
    return MAX(val, lw);
  }
  return val;
}


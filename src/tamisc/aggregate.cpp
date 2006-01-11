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


//////////////////////////
//  	CritParam      	//
//////////////////////////

void CritParam::Initialize() {
  flag = false;
  rel = LESSTHANOREQUAL;
  val = 0.0f;
  n_met = 0;
  cnt = 1;
};

void CritParam::Copy_(const CritParam& cp) {
  flag = cp.flag;
  rel = cp.rel;
  val = cp.val;
  cnt = cp.cnt;
  n_met = cp.n_met;
}

bool CritParam::Evaluate(float cmp) {
  if(!flag) return false;
  bool met = false;
  switch(rel) {
  case EQUAL:
    if(cmp == val)	met = true;
    break;
  case NOTEQUAL:
    if(cmp != val)	met = true;
    break;
  case LESSTHAN:
    if(cmp < val)	met = true;
    break;
  case GREATERTHAN:
    if(cmp > val)	met = true;
    break;
  case LESSTHANOREQUAL:
    if(cmp <= val)	met = true;
    break;
  case GREATERTHANOREQUAL:
    if(cmp >= val)	met = true;
    break;
  }
  if(met) {
    n_met++;
    if(n_met >= cnt) return true;
  }
  else
    n_met = 0;		// reset the counter (has to be consecutive)
  return false;
}

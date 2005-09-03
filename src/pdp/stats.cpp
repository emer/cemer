/* -*- C++ -*- */
/*=============================================================================
//									      //
// This file is part of the PDP++ software package.			      //
//									      //
// Copyright (C) 1995 Randall C. O'Reilly, Chadley K. Dawson, 		      //
//		      James L. McClelland, and Carnegie Mellon University     //
//     									      //
// Permission to use, copy, and modify this software and its documentation    //
// for any purpose other than distribution-for-profit is hereby granted	      //
// without fee, provided that the above copyright notice and this permission  //
// notice appear in all copies of the software and related documentation.     //
//									      //
// Permission to distribute the software or modified or extended versions     //
// thereof on a not-for-profit basis is explicitly granted, under the above   //
// conditions. 	HOWEVER, THE RIGHT TO DISTRIBUTE THE SOFTWARE OR MODIFIED OR  //
// EXTENDED VERSIONS THEREOF FOR PROFIT IS *NOT* GRANTED EXCEPT BY PRIOR      //
// ARRANGEMENT AND WRITTEN CONSENT OF THE COPYRIGHT HOLDERS.                  //
// 									      //
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

// stats.cc

#include "stats.h"
#include "netstru.h"
#include "sched_proc.h"
#include "pdpshell.h"

#include <limits.h>
#include <float.h>

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


//////////////////////////
//  	StatVal     	//
//////////////////////////

void StatVal::Initialize() {
  Init();
  val = 0.0f;
}

void StatVal::InitLinks() {
  DataItem::InitLinks();
  taBase::Own(stopcrit, this);
}

void StatVal::Copy_(const StatVal& cp) {
  val = cp.val;
  str_val = cp.str_val;
  stopcrit = cp.stopcrit;
}

void StatVal_List::InitStat(float value) {
  int i;
  for(i=0; i<size; i++)
    ((StatVal*)FastEl(i))->InitStat(value);
}

void StatVal_List::Init() {
  int i;
  for(i=0; i<size; i++)
    ((StatVal*)FastEl(i))->Init();
}

bool StatVal_List::Crit() {
  int i;
  for(i=0; i<size; i++) {
    if(((StatVal*)FastEl(i))->Crit())
      return true;
  }
  return false;
}

void StatVal_List::NameStatVals(const char* nm, const char* opts, bool is_string) {
  if(size <= 0)    return;
  StatVal* sv = (StatVal*)FastEl(0);
  if(is_string)
    sv->SetStringVecNm(nm, size);
  else
    sv->SetFloatVecNm(nm, size);
  sv->AddDispOption(opts);
  int i;
  for(i=1; i<size; i++) {
    StatVal* sv = (StatVal*)FastEl(i);
    if(is_string)
      sv->SetStringName(nm);
    else
      sv->SetName(nm);
    sv->AddDispOption(opts);
  }
}

bool StatVal_List::HasStopCrit() {
  int i;
  for(i=0; i<size; i++) {
    StatVal* sv = (StatVal*)FastEl(i);
    if(sv->stopcrit.flag)
      return true;
  }
  return false;
}

void StatVal_List::GenLogData(LogData* ld) {
  int i;
  for(i=0; i<size; i++) {
    StatVal* sv = (StatVal*)FastEl(i);
    if(sv->is_string)
      ld->AddString(sv, sv->str_val);
    else
      ld->AddFloat(sv, sv->val);
  }
}

//////////////////////////
//  	StatValAgg     	//
//////////////////////////

void StatValAgg::Initialize() {
}

void StatValAgg::Destroy() {
}

void StatValAgg::NewCopyAgg(StatVal* fm) {
  if(owner == NULL)	return;
  Stat* ownr = (Stat*)owner;
  StatVal* sv;
  if(ownr->copy_vals.size > ownr->n_copy_vals) {
    sv = (StatVal*)ownr->copy_vals.FastEl(ownr->n_copy_vals);
  }
  else {
    sv = (StatVal*)fm->Clone();
    ownr->copy_vals.Add(sv);
  }
  String nm = fm->name;
  String fmt;
  if(!nm.empty() && ((nm[0] == '|') || (nm[0] == '$'))) {
    fmt = nm[0]; nm = nm.after(0);
  }
  if(sv->vec_n > 0) {		// get rid of any vector info..
    sv->vec_n = 0;
    int pos;
    if(((pos = nm.index('<')) != -1) && ((pos = nm.index('>',pos+1)) != -1))
      nm = nm.after(pos);
  }
  sv->name = fmt + String("cpy_") + nm + "_" + String(ownr->n_copy_vals);
  sv->val = fm->val;
  ownr->n_copy_vals++;
}

void StatValAgg::ComputeAgg(StatVal* to, StatVal* fm) {
  if(op == COPY) {
    NewCopyAgg(fm);
    IncUpdt();
  }
  else {
    ComputeAgg(to->val, fm->val);
  }
}

void StatValAgg::ComputeAgg(StatVal* to, float fm_val) {
  static StatVal fm_stat;
  if(op == COPY) {
    fm_stat.val = fm_val;
    fm_stat.name = to->name;
    NewCopyAgg(&fm_stat);
    IncUpdt();
  }
  else {
    ComputeAgg(to->val, fm_val);
  }
}

bool StatValAgg::ComputeAggNoUpdt(StatVal* to, StatVal* fm) {
  if(op == COPY) {
    NewCopyAgg(fm);
    return true;
  }
  return ComputeAggNoUpdt(to->val, fm->val);
}

bool StatValAgg::ComputeAggsNoUpdt(StatVal_List* to, StatVal_List* fm) {
  if(fm->size <= 0)
    return false;
  if(op == COPY) {
    int j;
    for(j=0; j<fm->size; j++) {
      NewCopyAgg((StatVal*)fm->FastEl(j));
    }
    return true;
  }

  bool some_nonzero = false;
  to->EnforceSize(fm->size);
  int j;
  for(j=0; j<fm->size; j++) {
    some_nonzero |= ComputeAggNoUpdt((StatVal*)to->FastEl(j), (StatVal*)fm->FastEl(j));
  }
  return some_nonzero;
}

void StatValAgg::ComputeAggs(StatVal_List* to, StatVal_List* fm) {
  bool some_nonzero = ComputeAggsNoUpdt(to,fm);
  if(!no0 || some_nonzero)
    IncUpdt();
}

// this assumes NoUpdt
bool StatValAgg::AggFromCopy(StatVal_List* fm) {
  if(owner == NULL)	return false;
  Stat* ownr = (Stat*)owner;

  if(op == COPY) {		// copying a copy, the list gets bigger..
    int j;
    for(j=0; j<fm->size; j++) {
      // use the from variable as a template for creating new copy statvals..
      ComputeAggNoUpdt((StatVal*)fm->FastEl(j), (StatVal*)fm->FastEl(j));
    }
    return true;
  }

  bool some_nonzero = false;
  int j;
  for(j=0; j<fm->size; j++) {
    StatVal* fmsv = (StatVal*)fm->FastEl(j);
    StatVal* sv;
    if(ownr->copy_vals.size > j)
      sv = (StatVal*)ownr->copy_vals.FastEl(j);
    else {
      sv = (StatVal*)fmsv->Clone();
      ownr->copy_vals.Add(sv);
    }
    some_nonzero |= ComputeAggNoUpdt(sv, fmsv);
  }
  ownr->n_copy_vals = fm->size;
  return some_nonzero;
}


//////////////////////////
//  	AggStat     	//
//////////////////////////

void AggStat::Initialize() {
  from = NULL;
  real_stat = NULL;
  type_safe = true;
}

void AggStat::CutLinks() {
  taBase::DelPointer((TAPtr*)&from);
  StatValAgg::CutLinks();
}

void AggStat::Copy_(const AggStat& cp) {
  //  taBase::SetPointer((TAPtr*)&from,cp.from);
  type_safe = cp.type_safe;
}

void AggStat::UpdateAfterEdit() {
  StatValAgg::UpdateAfterEdit();

  if(owner == NULL)
    return;

  if(from != NULL) {
    if(type_safe && !owner->InheritsFrom(from->GetTypeDef())) {
      taMisc::Error("Incompatible Aggregation of type:",from->GetTypeDef()->name,
	       "into type:",owner->GetTypeDef()->name);
      from = NULL;
    }
  }
  FindRealStat();
  owner->UpdateAfterEdit();
}

void AggStat::SetFrom(Stat* frm) {
  taBase::SetPointer((TAPtr*)&from, frm);
}

void AggStat::FindRealStat() {
  if(owner == NULL) {
    real_stat = NULL;
    return;
  }

  Stat* tmp = (Stat*)owner;
  while ((tmp->time_agg.from != NULL) && (tmp->time_agg.from != tmp))
    tmp = tmp->time_agg.from;

  real_stat = tmp;
}

String AggStat::AppendAggName(const char* nm) const {
  String rval = nm;
  if(from == NULL)
    return rval;
  rval += ":";
  rval += GetAggName();
  return rval;
}

String AggStat::PrependAggName(const char* nm) const {
  if(from == NULL)
    return (String)nm;
  String rval = GetAggName();
  rval += "_";
  rval += nm;
  return rval;
}


//////////////////////////
//  	Stat     	//
//////////////////////////

void Stat::Initialize() {
  own_proc = NULL;
  has_stop_crit = false;
  n_copy_vals = 0;
  loop_init = INIT_IN_LOOP;
  log_stat = true;
  layer = NULL;
}

void Stat::InitLinks() {
  Process::InitLinks();
  own_proc = GET_MY_OWNER(SchedProcess);
  taBase::Own(time_agg, this);
  taBase::Own(net_agg, this);
  taBase::Own(copy_vals, this);
  // own all the stats in the object
  // just in this one case..
  TypeDef*   td = GetTypeDef();
  int i;
  for(i=TA_Stat.members.size; i<td->members.size;i++){
    MemberDef* md=td->members.FastEl(i);
    if(md->type->InheritsFrom(&TA_StatVal))
      taBase::Own((StatVal*)md->GetOff(this), this);
    else if(md->type->InheritsFrom(&TA_StatVal_List))
      taBase::Own((StatVal_List*) md->GetOff(this), this);
  }
  NameStatVals();
  Init();
}

void Stat::CutLinks() {
  DeleteAggregates();
  copy_vals.CutLinks();
  time_agg.CutLinks();
  net_agg.CutLinks();
  taBase::DelPointer((TAPtr*)&layer);
  own_proc = NULL;
  Process::CutLinks();
}

void Stat::Copy_(const Stat& cp) {
  //  has_stop_crit = cp.has_stop_crit;
  //  n_copy_vals = cp.n_copy_vals;
  loop_init = cp.loop_init;
  log_stat = cp.log_stat;
  time_agg = cp.time_agg;
  net_agg = cp.net_agg;
  copy_vals = cp.copy_vals;
  taBase::SetPointer((TAPtr*)&layer, cp.layer);
  FindOwnAggFrom(cp);
}

static String stat_name_agg_sv(Stat* stat, StatVal* fm, StatVal* to) {
  String nm = fm->name;
  int pos;
  if(((pos = nm.index('<')) != -1) && ((pos = nm.index('>',pos+1)) != -1))
    nm = nm.after(pos);
  String fmt;
  if(!nm.empty() && ((nm[0] == '|') || (nm[0] == '$'))) {
    fmt = nm[0]; nm = nm.after(0);
  }
  if(stat->time_agg.from->time_agg.from != NULL) { // already an aggregator, remove time
    String rmv = stat->time_agg.from->time_agg.GetAggName();
    if(nm.before('_') == rmv) nm = nm.after('_');
  }
  nm = stat->time_agg.PrependAggName(nm);
  nm = fmt + nm;
  to->disp_opts = fm->disp_opts;
  to->is_string = fm->is_string;
  return nm;
}

void Stat::NameStatVals() {
  Stat* rstat = time_agg.real_stat;
  TypeDef* td = GetTypeDef();
  if(time_agg.from != NULL) {	// aggregator, just copy from previous values..
    if(time_agg.op == Aggregate::COPY) {
      // reset any statval_lists because we're not using them!
      int i;
      for(i=TA_Stat.members.size; i<td->members.size;i++){
	MemberDef* md=td->members.FastEl(i);
	if(md->type->InheritsFrom(&TA_StatVal_List)) {
	  StatVal_List* svl = (StatVal_List*)md->GetOff(this);
	  svl->Reset();
	}
      }
      // but names of copy_vals are made as we construct them, so do nothing else here
    }
    else {			// all non-copy time agg: name the stats from previous guys
      if(time_agg.from->copy_vals.size > 0) { // agging from a copy stat
	// reset any statval_lists because we're not using them!
	int i;
	for(i=TA_Stat.members.size; i<td->members.size;i++){
	  MemberDef* md=td->members.FastEl(i);
	  if(md->type->InheritsFrom(&TA_StatVal_List)) {
	    StatVal_List* svl = (StatVal_List*)md->GetOff(this);
	    svl->Reset();
	  }
	}
	copy_vals.EnforceSize(time_agg.from->copy_vals.size); // always have same size as real stat
	n_copy_vals = copy_vals.size;
	int mx = MIN(time_agg.from->copy_vals.size, copy_vals.size);
	for(i=0; i<mx; i++) {
	  StatVal* tosv = (StatVal*)copy_vals.FastEl(i);
	  StatVal* fmsv = (StatVal*)time_agg.from->copy_vals.FastEl(i);
	  String nm = stat_name_agg_sv(this, fmsv, tosv);
	  if((i==0) && (copy_vals.size > 1)) // only for vec > 1
	    tosv->SetFloatVecNm(nm, copy_vals.size);
	  else {
	    tosv->name = nm;
	    tosv->vec_n = 0;
	  }
	}
      }
      else {			// agging from a regular stat
	copy_vals.Reset();
	n_copy_vals = 0;
	int i;
	for(i=TA_Stat.members.size; i<td->members.size;i++){
	  MemberDef* md=td->members.FastEl(i);
	  if(md->type->InheritsFrom(&TA_StatVal)) {
	    StatVal* fm = (StatVal*) md->GetOff((void*)time_agg.from);
	    StatVal* to = (StatVal*) md->GetOff((void*)this);
	    String nm = stat_name_agg_sv(this, fm, to);
	    to->name = nm;
	  }
	  else if(md->type->InheritsFrom(&TA_StatVal_List)) {
	    StatVal_List* fm = (StatVal_List *) md->GetOff((void*)time_agg.from);
	    StatVal_List* to = (StatVal_List *) md->GetOff((void*)this);
	    to->EnforceSize(fm->size);
	    int j;
	    for(j=0;j<to->size;j++) {
	      StatVal* fmsv = (StatVal*)fm->FastEl(j);
	      StatVal* tosv = (StatVal*)to->FastEl(j);
	      String nm = stat_name_agg_sv(this, fmsv, tosv);
	      if((j==0) && (to->size > 1)) // only for vec > 1
		tosv->SetFloatVecNm(nm, to->size);
	      else {
		tosv->name = nm;
		tosv->vec_n = 0;
	      }
	    }
	  }
	}
      }
    }
  }
  else {			// non-aggregator: name the raw stats according to variable
    String misc_nm;
    if((rstat != NULL) && (rstat->layer != NULL)) {
      misc_nm = rstat->layer->name;
      if(misc_nm.length() > 4)
	misc_nm = misc_nm.before(4);
    }
    if(net_agg.op != Aggregate::COPY) {
      copy_vals.Reset();
      n_copy_vals = 0;
    }
    int i;
    for(i=TA_Stat.members.size; i<td->members.size;i++){
      MemberDef* md=td->members.FastEl(i);
      String nm;
      if(!misc_nm.empty())
	nm = misc_nm + "_" + md->name;
      else
	nm = md->name;
      String ag_nm = net_agg.PrependAggName(nm);
      if(md->type->InheritsFrom(&TA_StatVal)) {
	StatVal* sv = (StatVal*)md->GetOff(this);
	sv->SetName(ag_nm);
      }
      else if(md->type->InheritsFrom(&TA_StatVal_List)) {
	StatVal_List* svl = (StatVal_List*)md->GetOff(this);
	svl->NameStatVals(ag_nm, "", false);
      }
      if(net_agg.op == Aggregate::COPY) {
	copy_vals.NameStatVals(ag_nm, "", false);
      }
    }
  }
}

bool Stat::HasStopCrit() {
  if((n_copy_vals > 0) && copy_vals.HasStopCrit())
    return true;
  TypeDef* td = GetTypeDef();
  int i;
  for(i=TA_Stat.members.size; i<td->members.size;i++){
    MemberDef* md = td->members.FastEl(i);
    if(md->type->InheritsFrom(&TA_StatVal)) {
      if(((StatVal*) md->GetOff(this))->stopcrit.flag)
	return true;
    }
    else if(md->type->InheritsFrom(&TA_StatVal_List)) {
      if(((StatVal_List*) md->GetOff(this))->HasStopCrit())
	return true;
    }
  }
  return false;
}

void Stat::UpdateAfterEdit() {
  Process::UpdateAfterEdit();
  time_agg.FindRealStat();
  Stat* rstat = time_agg.real_stat;
  if((time_agg.from != NULL) && (rstat != NULL)) { // if aggregator
    net_agg = rstat->net_agg;	// always copy stuff from real stat
    taBase::SetPointer((TAPtr*)&layer, rstat->layer);
    mod.flag = rstat->mod.flag;
  }
  else {
    if(net_agg.op != Aggregate::COPY) {
      copy_vals.Reset();
      n_copy_vals = 0;
    }
  }
  NameStatVals();
  has_stop_crit = HasStopCrit();
  String altnm = AltTypeName();
  if((own_proc != NULL) && (name.contains(GetTypeDef()->name) || name.contains(altnm))) {
    String nw_nm = own_proc->name + "_" + altnm;
    name = time_agg.PrependAggName(net_agg.PrependAggName(nw_nm));
    if(layer != NULL)
      name += String("_") + layer->name;
    else if((time_agg.real_stat != NULL) && (time_agg.real_stat->layer != NULL))
      name += String("_") + time_agg.real_stat->layer->name;
  }
  UpdateAggregates();		// update our aggreagators too!
}

float Stat::InitStatVal() {
  if(time_agg.from != NULL)
    return time_agg.InitAggVal();
  return net_agg.InitAggVal();
}

void Stat::InitStat_impl() {
  net_agg.Init();  copy_vals.InitStat(InitStatVal());
}

// implementors of subtypes should replace this general function with one
// that specifically calls InitStat() on StatVal objects directly and:
// InitStat_impl();

void Stat::InitStat() {
  float init_val = InitStatVal();
  TypeDef*   td = GetTypeDef();
  int i;
  for(i=TA_Stat.members.size; i<td->members.size;i++) {
    MemberDef* md=td->members.FastEl(i);
    if(md->type->InheritsFrom(&TA_StatVal))
      ((StatVal*) md->GetOff(this))->InitStat(init_val);
    else if(md->type->InheritsFrom(&TA_StatVal_List))
      ((StatVal_List*) md->GetOff(this))->InitStat(init_val);
  }
  InitStat_impl();
}

void Stat::Init_impl() {
  time_agg.Init();  InitStat();	n_copy_vals = 0;
}

// implementors of subtypes should replace this general function with
// specific (Init and Crit)

// Init() specifically calls Init() on StatVal objects directly, and:
// Init_impl();

void Stat::Init() {
  if(loop_init == NO_INIT) return;
  TypeDef*   td = GetTypeDef();
  int i;
  for(i=TA_Stat.members.size; i<td->members.size;i++){
    MemberDef* md = td->members.FastEl(i);
    if(md->type->InheritsFrom(&TA_StatVal))
      ((StatVal*) md->GetOff(this))->Init();
    else if(md->type->InheritsFrom(&TA_StatVal_List))
      ((StatVal_List*) md->GetOff(this))->Init();
  }
  Init_impl();
}

// Crit() checks has_stop_crit flag and if true, calls Crit() on statvals directly
// can replace Crit() with first two if statements plus direct call on statval

bool Stat::Crit() {
  if(!has_stop_crit)    return false;
  if(n_copy_vals > 0)   return copy_vals.Crit();

  TypeDef*   td = GetTypeDef();
  int i;
  for(i=TA_Stat.members.size; i<td->members.size;i++){
    MemberDef* md = td->members.FastEl(i);
    if(md->type->InheritsFrom(&TA_StatVal)) {
      if(((StatVal*) md->GetOff(this))->Crit())
	return true;
    }
    else if(md->type->InheritsFrom(&TA_StatVal_List)) {
      if(((StatVal_List*) md->GetOff(this))->Crit())
	return true;
    }
  }
  return false;
}

void Stat::C_Code() {
  if(time_agg.from != NULL)
    ComputeAggregates();
  else
    Network_Run();
}

void Stat::Network_Run() {
  if(network == NULL)
    return;
  Network_Init();
  Layer_Run();
  Network_Stat();
}

bool Stat::CheckLayerInNet() {
  if(layer == NULL) return true;
  if(layer->own_net == network)
    return true;

  taMisc::Error("*** Stat:", name, "layer:", layer->name,
		 "is not in current network, finding one of same name...");
  Layer* nw_lay = (Layer*)network->layers.FindName(layer->name);
  if(nw_lay == NULL) {
    taMisc::Error("==> could not find layer with same name in current network, aborting");
    return false;
  }
  taBase::SetPointer((TAPtr*)&layer, nw_lay);
  return true;
}

void Stat::Layer_Run() {
  if(layer != NULL) {
    if(!CheckLayerInNet()) return;
    if(layer->lesion)      return;
    Layer_Init(layer);
    Unit_Run(layer);
    Layer_Stat(layer);
    return;
  }
  Layer* lay;
  taLeafItr i;
  FOR_ITR_EL(Layer, lay, network->layers., i) {
    if(lay->lesion)
      continue;
    Layer_Init(lay);
    Unit_Run(lay);
    Layer_Stat(lay);
  }
}
void Stat::Unit_Run(Layer* lay) {
  Unit* unit;
  taLeafItr i;
  FOR_ITR_EL(Unit, unit, lay->units., i) {
    Unit_Init(unit);
    RecvCon_Run(unit);
    Unit_Stat(unit);
  }
}

void Stat::RecvCon_Run(Unit* unit) {
  Con_Group* cg;
  int i;
  FOR_ITR_GP(Con_Group, cg, unit->recv., i) {
    int j;
    for(j=0; j<cg->size; j++)
      Con_Stat(unit, cg->Cn(j), cg->Un(j));
  }
}

void Stat::SendCon_Run(Unit* unit) {
  Con_Group* cg;
  int i;
  FOR_ITR_GP(Con_Group, cg, unit->send., i) {
    int j;
    for(j=0; j<cg->size; j++)
      Con_Stat(cg->Un(j), cg->Cn(j), unit);
  }
}

LogData& Stat::GenLogData(LogData* ld) {
  if(ld == NULL)
    ld = &log_data;
  if(!log_stat)  return *ld;

  if(n_copy_vals > 0) {
    copy_vals.EnforceSize(n_copy_vals);
    if(copy_vals.size > 1) {
      StatVal* sv = (StatVal*)copy_vals.FastEl(0);
      if(sv->vec_n != copy_vals.size) {
	String nm = sv->name;
	int pos;
	if(((pos = nm.index('<')) != -1) && ((pos = nm.index('>',pos+1)) != -1))
	  nm = nm.after(pos);
	if(nm.empty())
	  nm = ((StatVal*)copy_vals.FastEl(1))->name;
	sv->SetFloatVecNm(nm, copy_vals.size);
      }
    }
    copy_vals.GenLogData(ld);
  }
  else {
    TypeDef* td = GetTypeDef();
    int i;
    for(i=TA_Stat.members.size; i<td->members.size;i++){
      MemberDef* md=td->members.FastEl(i);
      if(md->type->InheritsFrom(&TA_StatVal)) {
	StatVal* sv = (StatVal*)md->GetOff(this);
	if(sv->is_string)
	  ld->AddString(sv, sv->str_val);
	else
	  ld->AddFloat(sv, sv->val);
      }
      else if(md->type->InheritsFrom(&TA_StatVal_List)) {
	((StatVal_List*)md->GetOff(this))->GenLogData(ld);
      }
    }
  }
  return *ld;
}

void Stat::ComputeAggregates() {
  if(time_agg.from == NULL)
    return;

  bool some_nonzero = false; // whether all the from data was zeros (for no0) case
  if(time_agg.from->n_copy_vals > 0) {
    some_nonzero |= time_agg.AggFromCopy(&(time_agg.from->copy_vals));
  }
  else {
    TypeDef* td = time_agg.from->GetTypeDef();
    int i;
    for(i=TA_Stat.members.size; i< td->members.size; i++) {
      MemberDef* md = td->members.FastEl(i);

      if(md->type->InheritsFrom(&TA_StatVal)) {
	StatVal* fm = (StatVal*) md->GetOff((void*)time_agg.from);
	StatVal* to = (StatVal*) md->GetOff((void*)this);
	some_nonzero |= time_agg.ComputeAggNoUpdt(to, fm);
      }
      else if(md->type->InheritsFrom(&TA_StatVal_List)) {
	StatVal_List* fm = (StatVal_List *) md->GetOff((void*)time_agg.from);
	StatVal_List* to = (StatVal_List *) md->GetOff((void*)this);
	some_nonzero |= time_agg.ComputeAggsNoUpdt(to, fm);
      }
    }
  }
  if(!time_agg.no0 || some_nonzero)
    time_agg.IncUpdt();		// only inc the updt once at the end..
}

Aggregate::Operator Stat::GetAggOpForProc(SchedProcess* sproc, Aggregate::Operator agg_op) {
  if(agg_op != Aggregate::DEFAULT) return agg_op;

  String_PArray& typeopts = sproc->GetTypeDef()->opts;
  int opt;
  if((opt = typeopts.FindContains("AGGOP_")) >= 0) {
    String op = typeopts.FastEl(opt).after("AGGOP_");
    EnumDef* lop;
    if((lop = (TA_AggStat.FindEnum((char*)op))) != NULL) {
      return (Aggregate::Operator)lop->enum_no;
    }
  }
  return AggStat::LAST;
}

void Stat::CreateAggregates(Aggregate::Operator agg_op) {
  if(own_proc == NULL) return;
  SchedProcess* sproc = own_proc->super_proc;
  Stat*		stat_to_agg = this;

  while((sproc != NULL) && (sproc->InheritsFrom(&TA_SchedProcess))) {
    Aggregate::Operator use_op = agg_op;
    if(stat_to_agg != this)
      use_op = GetAggOpForProc(sproc, Aggregate::DEFAULT); // use default for higher levels
    Stat* nag = sproc->MakeAggregator(stat_to_agg, SchedProcess::LOOP_STATS, use_op);
    sproc = sproc->super_proc;
    stat_to_agg = nag;
  }
  UpdateAfterEdit();
  taMisc::DelayedMenuUpdate(own_proc);
}

void Stat::DeleteAggregates() {
  if(own_proc == NULL) return;
  SchedProcess* sproc= (SchedProcess *) own_proc->super_proc;
  Stat*		stat_to_agg = this;

  while((sproc != NULL) && (sproc->InheritsFrom(&TA_SchedProcess))) {
    Stat* next_stat_to_agg = NULL;
    int i;
    for(i=sproc->loop_stats.size-1; i>=0; i--) {
      Stat* ast = (Stat*)sproc->loop_stats.FastEl(i);
      if(ast->time_agg.from == stat_to_agg) {
	if(next_stat_to_agg == NULL) // get the first one (most likely to be big agg)
	  next_stat_to_agg = ast;
	else
	  ast->DeleteAggregates(); // be thorough anyway (a little recursion can't hurt)
	sproc->loop_stats.Remove(i);
      }
    }
    if(next_stat_to_agg != NULL)
      stat_to_agg = next_stat_to_agg;
    sproc = (SchedProcess *) sproc->super_proc;
  }
}

void Stat::UpdateAggregates() {
  Stat* aggr = FindAggregator();
  if(aggr != NULL) {
    aggr->UpdateAfterEdit();
    aggr->UpdateAggregates();
  }
}

Stat* Stat::FindAggregator() {
  if(own_proc == NULL) return NULL;
  SchedProcess* sproc= (SchedProcess *) own_proc->super_proc;
  Stat*		stat_to_agg = this;

  while((sproc != NULL) && (sproc->InheritsFrom(&TA_SchedProcess))) {
    int i;
    for(i=sproc->loop_stats.size-1; i>=0; i--) {
      Stat* ast = (Stat*)sproc->loop_stats.FastEl(i);
      if(ast->time_agg.from == stat_to_agg)
	return ast;
    }
    sproc = (SchedProcess *) sproc->super_proc;
  }
  return NULL;
}

bool Stat::FindOwnAggFrom(const Stat& cp) {
  if((cp.time_agg.from == NULL) || (cp.own_proc == NULL) || (own_proc == NULL)) return true;
  if(time_agg.from != NULL) return false;	// if we're already set, don't do anything
  // find out if from is in sub proc stats somewhere
  if((cp.own_proc->sub_proc == NULL) || (own_proc->sub_proc == NULL))
    return false;// some kind of bizzare agg from outside of hierarch
  int lfidx;
  lfidx = cp.own_proc->sub_proc->final_stats.FindLeaf(cp.time_agg.from);
  if(lfidx >= 0) {
    if(own_proc->sub_proc->final_stats.leaves >= lfidx) return false; // not avail!
    Stat* st = (Stat*)own_proc->sub_proc->final_stats.Leaf(lfidx);
    if(st->GetTypeDef() != GetTypeDef()) return false;
    taBase::SetPointer((TAPtr*)&(time_agg.from),st);
    return true;
  }
  lfidx = cp.own_proc->sub_proc->loop_stats.FindLeaf(cp.time_agg.from);
  if(lfidx >= 0) {
    if(own_proc->sub_proc->loop_stats.leaves >= lfidx) return false; // not avail!
    Stat* st = (Stat*)own_proc->sub_proc->loop_stats.Leaf(lfidx);
    if(st->GetTypeDef() != GetTypeDef()) return false;
    taBase::SetPointer((TAPtr*)&(time_agg.from),st);
    return true;
  }
  return false;
}


//////////////////////////////////
// 	Stat_Group		//
//////////////////////////////////

bool Stat_Group::Close_Child(TAPtr obj) {
  SchedProcess* sp = GET_MY_OWNER(SchedProcess);
  if(sp != NULL) {
    taMisc::DelayedMenuUpdate(sp);
  }
  return Remove(obj);		// otherwise just nuke it
}

bool Stat_Group::nw_itm_def_arg = false;

Stat* Stat_Group::FindMakeStat(TypeDef* td, const char* nm, bool& nw_itm) {
  nw_itm = false;
  if(td == NULL) return NULL;
  Stat* st = (Stat*)FindType(td);
  if(st != NULL) {
    if((nm == NULL) || (st->name == nm)) return st;
  }
  nw_itm = true;
  st = (Stat*)NewEl(1, td);
  if(nm != NULL) st->name = nm;
  taMisc::DelayedMenuUpdate(GetOwner());
  return st;
}

Stat* Stat_Group::FindAggregator(Stat* of_stat, Aggregate::Operator agg_op) {
  Stat* st;
  taLeafItr i;
  FOR_ITR_EL(Stat, st, this->, i) {
    if(st->time_agg.from == of_stat) {
      if(agg_op == Aggregate::DEFAULT) return st;
      if(st->time_agg.op == agg_op) return st;
    }
  }
  return NULL;
}

MonitorStat* Stat_Group::FindMonitor(TAPtr of_obj, const char* of_var) {
  Stat* st;
  taLeafItr i;
  FOR_ITR_EL(Stat, st, this->, i) {
    if((st->time_agg.from != NULL) || !st->InheritsFrom(&TA_MonitorStat)) continue;
    MonitorStat* mst = (MonitorStat*)st;
    if(mst->objects.size != 1) continue;
    if(mst->objects[0] == of_obj) {
      if(of_var == NULL) return mst;
      if(mst->variable == of_var) return mst;
    }
  }
  return NULL;
}

MonitorStat* Stat_Group::FindMakeMonitor(TAPtr of_obj, const char* of_var, bool& nw_itm) {
  nw_itm = false;
  MonitorStat* mst = FindMonitor(of_obj, of_var);
  if(mst != NULL) return mst;
  nw_itm = true;
  mst = (MonitorStat*)NewEl(1, &TA_MonitorStat);
  mst->SetObject(of_obj);
  mst->SetVariable(of_var);
  taMisc::DelayedMenuUpdate(GetOwner());
  return mst;
}

//////////////////////////
// 	SE_Stat		//
//////////////////////////

void SE_Stat::Initialize() {
  tolerance = 0.0f;
}

void SE_Stat::NameStatVals() {
  Stat::NameStatVals();
  se.AddDispOption("MIN=0");
  se.AddDispOption("TEXT");
}

void SE_Stat::InitStat() {
  float init_val = InitStatVal();
  se.InitStat(init_val);
  InitStat_impl();
}

void SE_Stat::Init() {
  if(loop_init == NO_INIT) return;
  se.Init();
  Init_impl();
}

bool SE_Stat::Crit() {
  if(!has_stop_crit)    return false;
  if(n_copy_vals > 0)   return copy_vals.Crit();
  return se.Crit();
}

void SE_Stat::Network_Init() {
  InitStat();
}

void SE_Stat::Layer_Run() {
  if(layer != NULL) {
    if(!CheckLayerInNet()) return;
    if(layer->lesion)      return;
    Layer_Init(layer);
    Unit_Run(layer);
    Layer_Stat(layer);
    return;
  }
  Layer *lay;
  taLeafItr i;
  FOR_ITR_EL(Layer, lay, network->layers., i) {
    // only target layers need to be checked for error
    if(lay->lesion || !((lay->ext_flag & Unit::TARG) || (lay->ext_flag & Unit::COMP)))
      continue;
    Layer_Init(lay);
    Unit_Run(lay);
    Layer_Stat(lay);
  }
}

void SE_Stat::Unit_Stat(Unit* unit) {
  if(!((unit->ext_flag & Unit::TARG) || (unit->ext_flag & Unit::COMP)))
    return;
  float tmp = fabsf(unit->act - unit->targ);
  if(tmp >= tolerance)
    tmp *= tmp;
  else
    tmp = 0.0f;
  net_agg.ComputeAgg(&se, tmp);
}

///////////////////////////
// 	Monitor Stat	 //
///////////////////////////

void MonitorStat::Initialize() {
  net_agg.op = Aggregate::COPY;
  variable = "act";
}

void MonitorStat::InitLinks() {
  Stat::InitLinks();
  taBase::Own(objects,this);
  taBase::Own(ptrs,this);
  objects.SetBaseType(&TA_taBase);
  ptrs.SetBaseType(&TA_taBase);
  taBase::Own(pre_proc_1,this);
  taBase::Own(pre_proc_2,this);
  taBase::Own(pre_proc_3,this);
}

void MonitorStat::CutLinks() {
  MonitorStat* rstat = (MonitorStat*)time_agg.real_stat;
  if(rstat != NULL) {
    int i;
    for(i=0;i<rstat->objects.size;i++) {
      TAPtr obj = rstat->objects.FastEl(i);
      if(obj == NULL)
	continue;
      Network* net = GET_OWNER(obj, Network);
      if(net != NULL)
	taMisc::DelayedMenuUpdate(net);
    }
  }
  objects.RemoveAll();
  ptrs.RemoveAll();
  Stat::CutLinks();
}

void MonitorStat::Copy_(const MonitorStat& cp) {
  mon_vals = cp.mon_vals;
  objects.BorrowUnique(cp.objects);	// borrow because its a link group
  variable = cp.variable;
  pre_proc_1 = cp.pre_proc_1;
  pre_proc_2 = cp.pre_proc_2;
  pre_proc_3 = cp.pre_proc_3;
}

void MonitorStat::UpdateAfterEdit() {
  Stat::UpdateAfterEdit();
  if((owner == NULL) || (own_proc == NULL) || (time_agg.real_stat == NULL))    return;
  MonitorStat* rstat = (MonitorStat*)time_agg.real_stat;
  if(rstat->variable.empty())
    return;
  if(time_agg.from != NULL) {
    variable = rstat->variable;
    objects.RemoveAll();
    ptrs.RemoveAll();
    members.RemoveAll();
  }
  else {
    ScanObjects();
  }
  // add variable and object name, if relevant
  String altnm = AltTypeName();
  if(name.contains(GetTypeDef()->name) || name.contains(altnm)) {
    name += String("_") + variable;
    if(rstat->objects.size > 0) {
      String objnm = GetObjName(rstat->objects.FastEl(0));
      if(!objnm.empty())
	name += String("_") + objnm;
    }
  }
}

String MonitorStat::GetObjName(TAPtr obj) {
  String nm = obj->GetName();
  if(!nm.empty()) {
    if(nm.length() > 4) nm = nm.before(4);
    return nm;
  }
  if(obj->InheritsFrom(TA_Unit)) {
    Unit* u = (Unit*)obj;
    Layer* lay = GET_OWNER(obj, Layer);
    if(lay != NULL) {
      nm = lay->name;
      if(nm.length() > 4) nm = nm.before(4);
      int index = ((Unit_Group*)u->owner)->Find(u);
      nm += String("[") + String(index) + "]";
      return nm;
    }
  }
  else if(obj->InheritsFrom(TA_Unit_Group)) {
    Unit_Group* ug = (Unit_Group*)obj;
    Layer* lay = GET_OWNER(obj, Layer);
    if(lay != NULL) {
      nm = lay->name;
      if(nm.length() > 4) nm = nm.before(4);
      nm += ".un";
      if(ug != &(lay->units)) {
	int index = lay->units.gp.Find(ug);
	if(index >= 0)
	  nm += String("[") + String(index) + "]";
      }
      return nm;
    }
  }
  return "no_nm";
}

void MonitorStat::NameStatVals() {
  if(time_agg.from != NULL) {
    Stat::NameStatVals();
  }
  // otherwise just use current values!
}

void MonitorStat::InitStat() {
  mon_vals.InitStat(InitStatVal());
  InitStat_impl();
}

void MonitorStat::Init() {
  if(loop_init == NO_INIT) return;
  mon_vals.Init();
  Init_impl();
}

bool MonitorStat::Crit() {
  if(!has_stop_crit)    return false;
  if(n_copy_vals > 0)   return copy_vals.Crit();
  return mon_vals.Crit();
}

static StatVal* get_new_mon_val(MonitorStat* st, String nm) {
  StatVal* sv = NULL;
  if(st->mon_vals.size > st->n_copy_vals) {
    sv = (StatVal*)st->mon_vals.FastEl(st->n_copy_vals++);
    if(!sv->name.contains(nm)) {
      sv->val = 0.0f;
      sv->str_val = "";
      sv->stopcrit.Initialize();
      sv->disp_opts = "";
      sv->is_string = false;
      sv->vec_n = 0;
    }
  }
  else {
    st->n_copy_vals++;
    sv = (StatVal*)st->mon_vals.New(1);
  }
  sv->name = nm;
  return sv;
}

void MonitorStat::ScanConGroup(Con_Group* mcg, char* varname, Projection* p) {
  Con_Group* cg;
  int i;
  MemberDef* md;
  FOR_ITR_GP(Con_Group, cg, mcg->, i) {
    if((p != NULL) && (cg->prjn != p)) continue;
    md = cg->el_typ->members.FindNameR(varname);
    if(md == NULL) continue;
    String unitname;
    if(net_agg.op==Aggregate::COPY) {
      Unit* u = GET_OWNER(cg,Unit);
      if(u == NULL) continue;
      unitname = GetObjName(u) + String("[") + String(i) + "]";
    }
    int j;
    for(j=0; j<cg->size; j++) {
      Connection* c = (Connection *) cg->Cn(j);
      ptrs.Link(c);
      members.Add(md);
      if(net_agg.op==Aggregate::COPY) {
	String valname = unitname + "[" + String(j) + "]." + String(varname);
	get_new_mon_val(this, valname);
      }
    }
  }
}

void MonitorStat::ScanUnit(Unit* u, Projection* p) {
  TAPtr ths = u;
  void* temp;
  MemberDef* md = NULL;
  String varname = variable;
  if(variable.contains('.')) {
    varname = variable.before('.');
    // for selected projections:
    if(varname=="r") varname = "recv";
    else if(varname=="s") varname = "send";
    md = u->FindMembeR(varname,temp);
    if((md == NULL) || (temp == NULL)) return;
    if(md->type->ptr == 1) {
      ths = *((TAPtr*)temp);
      if(ths == NULL) return;
    }
    else
      ths = (TAPtr) temp;
    varname = variable.after('.');
    if(ths->InheritsFrom(&TA_Con_Group)) {
      ScanConGroup((Con_Group *) ths,varname,p);
      return;
    }
    // otherwise it must be bias or someother sub object
  }
  if(ths == NULL) return;
  md = ths->FindMember((const char*)varname);
  if(md == NULL) return;
  members.Add(md);
  ptrs.Link(ths);
  if(net_agg.op == Aggregate::COPY) {
    String valname = GetObjName(u) + String(".") + variable;
    get_new_mon_val(this, valname);
  }
}

void MonitorStat::ScanProjection(Projection* p) {
  int temp=0;
  MemberDef* md = p->FindMember((char *) variable,temp);
  if(md != NULL) {
    if(net_agg.op == Aggregate::COPY) {
      String valname = GetObjName(p) + String(".") + variable;
      get_new_mon_val(this, valname);
    }
    ptrs.Link(p);
    members.Add(md);
    return;
  }
  Layer* lay = NULL;
  if(variable.before('.') == "r") lay = p->layer;
  else if(variable.before('.') == "s") lay = p->from;
  if(lay == NULL) {
    taMisc::Error("MonitorStat Projection does not have layer's set or",
		   "selected variable does not apply to connections");
    return;
  }
  taLeafItr i;
  Unit* u;
  FOR_ITR_EL(Unit, u, lay->units., i)
    ScanUnit(u, p);
}

void MonitorStat::ScanLayer(Layer* lay) {
  TAPtr ths = lay;
  MemberDef* md = NULL;
  String varname = variable;
  if(variable.contains('.')) {
    varname = variable.before('.');
    md = lay->FindMember((const char*)varname);
    if(md != NULL) {
      if(md->type->ptr == 1)
	ths = *((TAPtr*)md->GetOff((void*)lay));
      else
	ths = (TAPtr) md->GetOff((void*)lay);
      varname = variable.after('.');
    }
    else			// variable not there, go to units
      ths = NULL;
  }
  if(ths != NULL) {
    // now search from 'ths'
    md = ths->FindMember((const char*)varname);
    if(md != NULL) {
      if(net_agg.op == Aggregate::COPY) {
	String valname = GetObjName(lay) + String(".") + variable;
	get_new_mon_val(this, valname);
      }
      ptrs.Link(ths);
      members.Add(md);
      return;
    }
  }
  // the default is to scan the units for the variable
  taLeafItr i;
  Unit* u;
  FOR_ITR_EL(Unit, u, lay->units., i)
    ScanUnit(u);
  if(mon_vals.size > 1) {
    StatVal* sv = (StatVal *) mon_vals.FastEl(0);
    sv->disp_opts = "";
    String xgeom = String("GEOM_X=") + String(lay->geom.x);
    String ygeom = String("GEOM_Y=") + String(lay->geom.y);
    sv->AddDispOption(xgeom);
    sv->AddDispOption(ygeom);
  }
}

void MonitorStat::ScanUnitGroup(Unit_Group* ug) {
  TAPtr ths = ug;
  MemberDef* md = NULL;
  String varname = variable;
  if(variable.contains('.')) {
    varname = variable.before('.');
    md = ug->FindMember((const char*)varname);
    if(md != NULL) {
      if(md->type->ptr == 1)
	ths = *((TAPtr*)md->GetOff((void*)ug));
      else
	ths = (TAPtr) md->GetOff((void*)ug);
      varname = variable.after('.');
    }
    else			// variable not there, go to units
      ths = NULL;
  }
  if(ths != NULL) {
    // now search from 'ths'
    md = ths->FindMember((const char*)varname);
    if(md != NULL) {
      if(net_agg.op == Aggregate::COPY) {
	String valname = GetObjName(ug) + String(".") + variable;
	get_new_mon_val(this, valname);
      }
      ptrs.Link(ths);
      members.Add(md);
      return;
    }
  }
  // the default is to scan the units for the variable
  taLeafItr i;
  Unit* u;
  FOR_ITR_EL(Unit, u, ug->, i)
    ScanUnit(u);
  if(mon_vals.size > 1) {
    StatVal* sv = (StatVal *) mon_vals.FastEl(0);
    sv->disp_opts = "";
    String xgeom = String("GEOM_X=") + String(ug->geom.x);
    String ygeom = String("GEOM_Y=") + String(ug->geom.y);
    sv->AddDispOption(xgeom);
    sv->AddDispOption(ygeom);
  }
}

void MonitorStat::ScanObjects() {
  if((time_agg.real_stat == NULL) || (time_agg.from != NULL))
    return;
  MonitorStat* rstat = (MonitorStat*)time_agg.real_stat;
  n_copy_vals = 0;		// use this as a counter for mon_vals!
  tabMisc::NotifyEdits(&mon_vals);
  ptrs.RemoveAll();
  members.RemoveAll();
  if(rstat->objects.size == 0) return;
  int i;
  for(i=0;i<rstat->objects.size;i++){
    TAPtr obj = rstat->objects.FastEl(i);
    if(obj == NULL)
      continue;
    if(obj->InheritsFrom(&TA_Unit)) ScanUnit((Unit*) obj);
    else if(obj->InheritsFrom(&TA_Layer)) ScanLayer((Layer*) obj);
    else if(obj->InheritsFrom(&TA_Unit_Group)) ScanUnitGroup((Unit_Group*) obj);
    else if(obj->InheritsFrom(&TA_Projection))
      ScanProjection((Projection*) obj);
    else {			// could be any type of object
      int temp = 0;
      MemberDef* md = obj->FindMember((char *) variable,temp);
      if(md != NULL) {
	if(net_agg.op == Aggregate::COPY) {
	  String valname = GetObjName(obj) + String(".") + variable;
	  get_new_mon_val(this, valname);
	}
	ptrs.Link(obj);
	members.Add(md);
      }
    }
  }
  if(net_agg.op != Aggregate::COPY) {
    mon_vals.EnforceSize(1);
    String nm = variable;
    String objnm = GetObjName(rstat->objects.FastEl(0));
    if(!objnm.empty())
      nm += String("_") + objnm;
    nm = net_agg.PrependAggName(nm);
    get_new_mon_val(this, nm);
  }
  else {
    mon_vals.EnforceSize(n_copy_vals);
    if(mon_vals.size > 0) {
      StatVal* sv = (StatVal*)mon_vals.FastEl(0);
      String nm = sv->name;
      sv->SetFloatVecNm(nm, mon_vals.size);
    }
  }
  n_copy_vals = 0;
}

void MonitorStat::Network_Init() {
  InitStat();
}

void MonitorStat::Network_Stat() {
  if((time_agg.real_stat == NULL) || (variable.empty()))
    return;
  if(net_agg.op == Aggregate::COPY) {
    if(members.size != mon_vals.size) ScanObjects();
    if(members.size != mon_vals.size) {
      taMisc::Error("*** MonitorStat: unable to get correct number of stats for members (maybe there are no objects to monitor?)",
		    GetPath());
      return;
    }
  }
  else {
    if(mon_vals.size != 1) ScanObjects();
  }
  int i;
  for(i=0;i<members.size;i++) {
    MemberDef* md = members.FastEl(i);
    float mbval = 0.0f;
    if((md->type == &TA_float) ||
       (md->type->InheritsFrom(&TA_float)))
       mbval = *((float*)md->GetOff((void*)ptrs.FastEl(i)));
    else if(md->type->InheritsFrom(&TA_int)){
      mbval = (float) *((int *) md->GetOff((void*)ptrs.FastEl(i)));
    }
    // pre-process..
    mbval = pre_proc_3.Evaluate(pre_proc_2.Evaluate(pre_proc_1.Evaluate(mbval)));
    if(net_agg.op == Aggregate::COPY) {
      StatVal* sv = (StatVal*)mon_vals.FastEl(i);
      sv->val = mbval;
    }
    else {
      net_agg.ComputeAgg(((StatVal*)mon_vals.FastEl(0))->val, mbval);
    }
  }
}

void MonitorStat::SetVariable(const char* varnm){
  variable = varnm;
  UpdateAfterEdit();
}

void MonitorStat::SetObject(TAPtr obj){
  objects.Reset();
  objects.Link(obj);
  UpdateAfterEdit();
}

void MonitorStat::AddObject(TAPtr obj){
  objects.Link(obj);
  UpdateAfterEdit();
}

void MonitorStat::SetObjsFmNet() {
  if(network == NULL) return;
/*TODO  if(network->views()->size == 0) return;
  NetView* nv = (NetView*)network->views.DefaultEl();
  taBase_List* lst = nv->GetSelectGroup();
  if(lst->size == 0) {
    taMisc::Error("No objects selected to monitor!");
    return;
  }
  objects.Reset();
  int i;
  for(i=0;i<lst->size;i++){
    objects.Link(lst->FastEl(i));
  } */
  UpdateAfterEdit();
}



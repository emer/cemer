// Copyright, 1995-2007, Regents of the University of Colorado,
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



// netstru.cc
// Functions specific to structures defined in netstru.h


#include "netstru.h"
#include "netdata.h"
#include "emergent_project.h"
#include "ta_data.h"
#include "ta_filer.h"

#ifdef TA_GUI
#include "ta_qt.h"
#include "ta_qtgroup.h"
#include "netstru_qtso.h"
#endif

// Static function -- could be put onto Network, but I didn't want to change header
bool Network_InStructUpdate(Network* net) {
  taDataLink* dl = net->data_link(); // doesn't autocreate
  return (dl ? (dl->dbuCnt() > 0) : false);
}

//////////////////////////
//  SigmoidSpec		//
//////////////////////////

#ifdef _MSC_VER
const float SigmoidSpec::SIGMOID_MAX_VAL = 0.999999f;
const float SigmoidSpec::SIGMOID_MIN_VAL = 0.000001f;
const float SigmoidSpec::SIGMOID_MAX_NET = 13.81551f;
#else
const float SigmoidSpec::SIGMOID_MAX_VAL;
const float SigmoidSpec::SIGMOID_MIN_VAL;
const float SigmoidSpec::SIGMOID_MAX_NET;
#endif

//////////////////////////
//  	Schedule	//
//////////////////////////

void SchedItem::Initialize() {
  start_ctr = 0;
  start_val = 0.0f;
  duration = 0;
  step = .01f;
}

String SchedItem::GetDesc() const {
  String rval = String(start_ctr) + ": " + String(start_val);
  return rval;
}

void Schedule::Initialize() {
  last_ctr = -1;
  default_val = 1.0f;
  cur_val = 0.0f;
  interpolate = true;
  SetBaseType(&TA_SchedItem);
}

void Schedule::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  last_ctr = -1;
  cur_val = 0;
  int lst_ctr = -1;
  Schedule temp;
  int i;
  for(i=0; i < size; i++) {
    SchedItem* itm = FastEl(i);
    if(itm->start_ctr < lst_ctr) {
      temp.Transfer(itm);
      i--;
    }
    else {
      lst_ctr = itm->start_ctr;
    }
  }
  for (; temp.size > 0 ;) {
    SchedItem* itm = temp.FastEl(0);
    int j;
    for(j=0; (j < size) && (itm->start_ctr > FastEl(j)->start_ctr); j++);
    Insert(itm,j);		// always insert item in new spot
    temp.RemoveIdx(0);
  }

  if(size > 0)
    FastEl(0)->start_ctr = 0;

  for(i=0; i < size; i++) {
    SchedItem* itm = FastEl(i);
    if(i == (size - 1)) {
      itm->duration = 1;
      itm->step = 1.0;
      break;
    }
    SchedItem* nxt = FastEl(i+1);
    itm->duration = nxt->start_ctr - itm->start_ctr;
    itm->duration = MAX(itm->duration, 1);
    itm->step = (nxt->start_val - itm->start_val) / (float)itm->duration;
  }
}

float Schedule::GetVal(int ctr) {
  if((size <  1) || (ctr < 0)) {
    cur_val = default_val;
    return default_val;
  }

  if(ctr == last_ctr)
    return cur_val;

  last_ctr = ctr;
  int i;
  for(i=0; i < size; i++) {
    SchedItem* itm = FastEl(i);
    if((ctr >= itm->start_ctr) && (ctr < (itm->start_ctr + itm->duration))) {
      if(i == size-1)
	cur_val = itm->start_val;
      else {
	if(interpolate)
	  cur_val = itm->GetVal(ctr);
	else
	  cur_val = itm->start_val;
      }
      return cur_val;
    }
  }
  cur_val = FastEl(size-1)->start_val;
  return cur_val;
}

#define DEFAULT_3D_NETSIZE_X 5
#define DEFAULT_3D_NETSIZE_Y 2
#define DEFAULT_3D_NETSIZE_Z 3
#define DEFAULT_3D_NETVIEW_SKEW .15

#define DEFAULT_2D_NETSIZE_X 5
#define DEFAULT_2D_NETSIZE_Y 12
#define DEFAULT_2D_NETSIZE_Z 0
#define DEFAULT_2D_NETVIEW_SKEW 0

////////////////////////////////////////
//	Connection - Groups & Specs   //
////////////////////////////////////////

static int conspec_repl_bias_ptr(UnitSpec* us, ConSpec* old, ConSpec* nw) {
  int cnt = 0;
  UnitSpec* u2;
  taLeafItr i;
  FOR_ITR_EL(UnitSpec, u2, us->children., i) {
    if(u2->bias_spec.spec.ptr() == old) {
      u2->bias_spec.SetSpec(nw); // update to new
      cnt++;
    }
    if(u2->children.leaves > 0)
      cnt += conspec_repl_bias_ptr(u2, old, nw);
  }
  return cnt;
}

void ConSpec::Initialize() {
  min_obj_type = &TA_Connection;
  rnd.type = Random::UNIFORM;
  rnd.mean = 0.0f;
  rnd.var = .5f;
}

void ConSpec::InitLinks() {
  BaseSpec::InitLinks();
  children.SetBaseType(&TA_ConSpec); // allow all of this general spec type to be created under here
  children.el_typ = GetTypeDef(); // but make the default to be me!
  taBase::Own(rnd,this);
  taBase::Own(wt_limits,this);
}

void ConSpec::CutLinks() {
  BaseSpec::CutLinks();
}

void ConSpec::Copy_(const ConSpec& cp) {
  //  min_con_type = cp.min_con_type;  // don't do this -- could be going between types
  rnd = cp.rnd;
  wt_limits = cp.wt_limits;
}

bool ConSpec::CheckConfig_RecvCons(RecvCons* cg, bool quiet) {
  return true;
}

void ConSpec::Init_Weights_Net() {
  Network* net = (Network *) GET_MY_OWNER(Network);
  if (!net) return;
  net->Init_Weights();
  net->UpdateAllViews();
}

bool ConSpec::CheckObjectType_impl(taBase* obj) {
  TypeDef* con_tp = &TA_Connection;
  if(obj->InheritsFrom(&TA_RecvCons)) {
    con_tp = ((RecvCons*)obj)->con_type;
  }
  else if(obj->InheritsFrom(&TA_SendCons)) {
    con_tp = ((SendCons*)obj)->con_type;
  }
  else if(obj->InheritsFrom(&TA_Projection)) {
    con_tp = ((Projection*)obj)->con_type;
  }
  else if(obj->InheritsFrom(&TA_UnitSpec)) {
    if(((UnitSpec*)obj)->bias_con_type)
      con_tp = ((UnitSpec*)obj)->bias_con_type;
  }
  if(!con_tp->InheritsFrom(min_obj_type))
    return false;
  return true;
}

bool ConSpec::CheckType_impl(TypeDef* td) {
  if (td->InheritsFrom(TA_Projection))
    return true;
  return inherited::CheckType_impl(td);
}


void ConSpec::ApplySymmetry(RecvCons* cg, Unit* ru) {
  if(!wt_limits.sym) return;
  Connection* rc, *su_rc;
  for(int i=0; i<cg->cons.size;i++) {
    rc = cg->Cn(i);
    su_rc = RecvCons::FindRecipRecvCon(cg->Un(i), ru, cg->prjn->layer);
    if(su_rc)
      su_rc->wt = rc->wt;	// set other's weight to ours (otherwise no random..)
  }
}

/////////////////////////////////////////////////////////////////////
//	ConArray

void ConArray::Initialize() {
  con_size = sizeof(Connection);
  con_type = &TA_Connection;
  size = 0;
  alloc_size = 0;
  cons = NULL;
}

void ConArray::Destroy() {
  CutLinks();
}

void ConArray::CutLinks() {
  Free();
}

void ConArray::Free() {
  if(cons) { free(cons); cons = NULL; }
  size = 0;
  alloc_size = 0;
}
void ConArray::Copy_(const ConArray& cp) {
  SetType(cp.con_type);
  Alloc(cp.alloc_size);
  CopyCons(cp);
}

void ConArray::SetType(TypeDef* cn_tp) {
  if(con_type == cn_tp) return;
  if(cons) {
#ifdef DEBUG
    taMisc::Warning("ConArray SetType error: set new type after connections were allocated!");
#endif
    Free();
  }
  con_type = cn_tp;
  con_size = cn_tp->size;
}

void ConArray::Alloc(int sz) {
  if(sz == alloc_size) return;
  if(cons)
    Free();
  alloc_size = sz;
  cons = (char*)malloc(alloc_size * con_size);
}

void ConArray::SetSize(int sz) {
  if(sz > alloc_size) {
    taMisc::Error("ConArray SetSize error: requesting size:", String(sz), "more than allocated:", String(alloc_size), ".  Programmer must increase size of allocation in Connect_impl function!");
    sz = alloc_size;
  }
  if(sz > size) {
    memset((void*)FastEl(size), 0, (sz - size) * con_size);
  }
  size = sz;
}

bool ConArray::RemoveIdx(int i) {
  if(!InRange(i)) return false;
  // note: doing this piecewize because memcpy is undefined if overlapping
  // and we don't need the full non-destructive memmove
  for(int j=i; j<size-1; j++)
    memcpy((void*)FastEl(j), (void*)FastEl(j+1), con_size);
  size--;
  return true;
}

/////////////////////////////////////////////////////////////////////
//	UnitPtrList

int UnitPtrList::UpdatePointers_NewPar(taBase* old_par, taBase* new_par) { 
  int nchg = 0;
  if(old_par->InheritsFrom(&TA_Network) && new_par->InheritsFrom(&TA_Network)) {
    // this is optimized for networks to use the getmyleafindex
    Network* nw_net = (Network*)new_par;
    Network* old_net = (Network*)old_par;
    for(int i=size-1; i >= 0; i--) {
      Unit* itm = FastEl(i);
      if(!itm) continue;
      Layer* old_lay = GET_OWNER(itm,Layer);
      int lidx = old_net->layers.FindLeafEl(old_lay);
      int uidx = itm->GetMyLeafIndex();
      if((lidx >= 0) && (uidx >= 0)) {
	Layer* nw_lay = (Layer*)nw_net->layers.Leaf(lidx);
	if(nw_lay) {
	  Unit* nw_un = (Unit*)nw_lay->units.Leaf(uidx);
	  if(nw_un) {
	    ReplaceLinkIdx(i, nw_un);
	    nchg++;
	  }
	  else {
	    RemoveIdx(i);
	  }
	}
      }
    }
  }
  else {
    for(int i=size-1; i >= 0; i--) {
      Unit* itm = FastEl(i);
      if(!itm) continue;
      taBase* old_own = itm->GetOwner(old_par->GetTypeDef());
      if(old_own != old_par) continue;
      String old_path = itm->GetPath(NULL, old_par);
      MemberDef* md;
      Unit* nitm = (Unit*)new_par->FindFromPath(old_path, md);
      if(nitm) {
	ReplaceLinkIdx(i, nitm);
	nchg++;
      }
      else {
	RemoveIdx(i);
      }
    }
  }
  return nchg;
}

int UnitPtrList::UpdatePointers_NewParType(TypeDef* par_typ, taBase* new_par) { 
  if(size <= 0) return 0;
  Unit* itm = FastEl(0);
  taBase* old_par = itm->GetOwner(par_typ);
  return UpdatePointers_NewPar(old_par, new_par);
}

int UnitPtrList::UpdatePointers_NewObj(taBase* old_ptr, taBase* new_ptr) {
  int nchg = 0;
  for(int i=size-1; i>=0; i--) {
    Unit* itm = FastEl(i);
    if(!itm) continue;
    if(itm == old_ptr) {	   // if it is the old guy, it is by defn a link because we're not the owner..
      if(!new_ptr)		   // if replacement is null, just remove it
	RemoveIdx(i);
      else
	ReplaceLinkIdx(i, (Unit*)new_ptr);    // it is a link to old guy; replace it!
      nchg++;
    }
  }
  return nchg;
}

/////////////////////////////////////////////////////////////////////
//	RecvCons

void RecvCons::Initialize() {
  // derived classes need to set new basic con types
  con_type = &TA_Connection;
  prjn = NULL;
  send_idx = -1;
  m_con_spec = NULL;
}

void RecvCons::InitLinks() {
  inherited::InitLinks();
  taBase::Own(cons, this);
}

void RecvCons::CutLinks() {
  cons.CutLinks();
  units.Reset();
  taBase::DelPointer((taBase**)&prjn);
  m_con_spec = NULL;
  inherited::CutLinks();
}

void RecvCons::Copy_(const RecvCons& cp) {
  // just do a full copy here
  SetConType(cp.con_type);
  cons = cp.cons;
  units.Borrow(cp.units);	// note: updated in UpdatePointers_newpar after network copy
  m_con_spec = cp.m_con_spec;
  taBase::SetPointer((taBase**)&prjn, cp.prjn);
//   send_idx = cp.send_idx;	// do not copy: updated in FixPrjnIndexes after network copy
}

void RecvCons::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  cons.SetType(con_type);	// always impose our type on it..
}

void RecvCons::SetConType(TypeDef* cn_tp) {
  con_type = cn_tp;
  cons.SetType(con_type);
}

void RecvCons::CheckThisConfig_impl(bool quiet, bool& rval) { 
  inherited::CheckThisConfig_impl(quiet, rval);

  if(CheckError(!GetConSpec(), quiet, rval, "No con spec set")) {
    return;			// fatal!
  }
  if(CheckError(!prjn, quiet, rval, "null projection! (Connect will fix)")) {
    return; 			// fatal!
  }
  if(CheckError((GetTypeDef() != prjn->recvcons_type), quiet, rval,
		"type does not match recvcons_type for projection -- should be:",
		prjn->recvcons_type->name)) {
    prjn->projected = false;
  }
  if(CheckError((con_type != prjn->con_type), quiet, rval,
		"connection type does not match prjn con_type -- should be:",
		prjn->con_type->name)) {
    prjn->projected = false;
  }
  if(cons.size > 0) {		// connections exist
    if(CheckError((send_idx < 0) || (send_idx != prjn->send_idx), quiet, rval,
		  "unset send_idx, do FixPrjnIndexes or Connect")) {
      prjn->projected = false;
    }

    Unit* su = Un(0);
    if(CheckError(!su, quiet, rval,
		  "sending unit is null when it should not be!  rebuild network!")) {
      prjn->projected = false;
    }
    else if(CheckError((su->send.size <= send_idx), quiet, rval,
		  "send_idx is out of range on sending unit. Do Actions/Remove Cons, then Build, Connect on Network")) {
      prjn->projected = false;
    }
    else {
      SendCons* sucg = su->send.SafeEl(send_idx);
      if(CheckError(!sucg, quiet, rval,
		    "send_idx is not set correctly (send guy is NULL). Do Actions/Remove Cons, then Build, Connect on Network")) {
	prjn->projected = false;
      }
      else {
	if(CheckError((sucg->prjn != prjn), quiet, rval,
		      "send_idx doesn't have correct prjn on sending unit. Do Actions/Remove Cons, then Build, Connect on Network")) {
	  prjn->projected = false;
	}
      }
    }
  }
  if(!GetConSpec()->CheckConfig_RecvCons(this, quiet)) 
    rval = false; 
}

int RecvCons::UpdatePointers_NewPar(taBase* old_par, taBase* new_par) { 
  int nchg = inherited::UpdatePointers_NewPar(old_par, new_par);
  nchg += units.UpdatePointers_NewPar(old_par, new_par);
  return nchg;
}

int RecvCons::UpdatePointers_NewParType(TypeDef* par_typ, taBase* new_par) { 
  int nchg = inherited::UpdatePointers_NewParType(par_typ, new_par);
  nchg += units.UpdatePointers_NewParType(par_typ, new_par);
  return nchg;
}

int RecvCons::UpdatePointers_NewObj(taBase* old_ptr, taBase* new_ptr) {
  int nchg = inherited::UpdatePointers_NewObj(old_ptr, new_ptr);
  nchg += units.UpdatePointers_NewObj(old_ptr, new_ptr);
  return nchg;
}

void RecvCons::Copy_Weights(const RecvCons* src) {
  int mx = MIN(cons.size, src->cons.size);
  int i;
  for(i=0; i < mx; i++)
    Cn(i)->wt = src->Cn(i)->wt;
}

bool RecvCons::ChangeMyType(TypeDef*) {
  TestError(true, "ChangeMyType", "Cannot change type of con_groups -- change type setting in projection and reconnect network instead");
  return false;
}

void RecvCons::LinkSendCons(Unit* ru) {
  for(int j=0; j< units.size; j++) {
    Unit* su = Un(j);
    if(!su) continue;

    SendCons* send_gp = NULL;
    if(send_idx >= 0)
      send_gp = su->send.SafeEl(send_idx);
    if(!send_gp)
      send_gp = su->send.FindPrjn(prjn);
    if(send_gp) {
      send_gp->units.LinkUnique(ru);
      send_gp->cons.LinkUnique(Cn(j));
    }
  }
}


/////////////////////////////////////////////////////////////
// 	Connection Creation/Deletion

void RecvCons::AllocCons(int no) {
  if(no <= 0) return;
  cons.Alloc(no);
  units.AllocExact(no);
}

Connection* RecvCons::NewCon(Unit* un) {
  units.Link(un);
  if(TestError(cons.size >= cons.alloc_size, "Newcon",
		 "already at maximum allocated size.",
		 "Programmer must increase size of allocation in Connect_impl function!")) {	
  }
  else {
    cons.New(1);
  }
  return cons.FastEl(cons.size-1);
}

bool RecvCons::RemoveConIdx(int i) {
  units.RemoveIdx(i);
  return cons.RemoveIdx(i);
}

bool RecvCons::RemoveConUn(Unit* un) {
  int idx;
  if((idx = units.FindEl(un)) < 0)
    return false;
  return RemoveConIdx(idx);
}

void RecvCons::RemoveAll() {
  cons.Reset();
  units.Reset();
}

int RecvCons::FindConFromIdx(Unit* un) const {
  return units.FindEl(un);
}

int RecvCons::FindConFromNameIdx(const String& unit_nm) const {
  return units.FindNameIdx(unit_nm);
}

Connection* RecvCons::FindConFrom(Unit* un) const {
  int idx = FindConFromIdx(un);
  if(idx < 0) return NULL;
  return cons.SafeEl(idx);
}

Connection* RecvCons::FindConFromName(const String& unit_nm) const {
  int idx = FindConFromNameIdx(unit_nm);
  if(idx < 0) return NULL;
  return cons.SafeEl(idx);
}

Connection* RecvCons::FindRecipRecvCon(Unit* su, Unit* ru, Layer* ru_lay) {
  for(int g=0; g<su->recv.size; g++) {
    RecvCons* cg = su->recv.FastEl(g);
    if(!cg->prjn || (cg->prjn->from.ptr() != ru_lay)) continue;
    Connection* con = cg->FindConFrom(ru);
    if(con) return con;
  }
  return NULL;
}

Connection* RecvCons::FindRecipSendCon(Unit* ru, Unit* su, Layer* su_lay) {
  for(int g=0; g<ru->send.size; g++) {
    SendCons* cg = ru->send.FastEl(g);
    if(!cg->prjn || (cg->prjn->layer != su_lay)) continue;
    Connection* con = cg->FindConFrom(su);
    if(con) return con;
  }
  return NULL;
}

void RecvCons::MonitorVar(NetMonitor* net_mon, const String& variable) {
  if(!net_mon) return;
  net_mon->AddObject(this, variable);
}

/////////////////////////////////////////////////////////////
// 	Weight ops

void RecvCons::TransformWeights(const SimpleMathSpec& trans) {
  int i;
  for(i=0; i < cons.size; i++)
    Cn(i)->wt = trans.Evaluate(Cn(i)->wt);
  Unit* ru = GET_MY_OWNER(Unit);
  Init_Weights_post(ru);	// update weights after mod
}

void RecvCons::AddNoiseToWeights(const Random& noise_spec) {
  int i;
  for(i=0; i < cons.size; i++)
    Cn(i)->wt += noise_spec.Gen();
  Unit* ru = GET_MY_OWNER(Unit);
  Init_Weights_post(ru);	// update weights after mod
}

int RecvCons::PruneCons(Unit* un, const SimpleMathSpec& pre_proc,
			    Relation::Relations rel, float cmp_val)
{
  Relation cond;
  cond.rel = rel; cond.val = cmp_val;
  int rval = 0;
  int j;
  for(j=cons.size-1; j>=0; j--) {
    if(cond.Evaluate(pre_proc.Evaluate(Cn(j)->wt))) {
      un->DisConnectFrom(Un(j), prjn);
      rval++;
    }
  }
  return rval;
}

int RecvCons::LesionCons(Unit* un, float p_lesion, bool permute) {
  int rval = 0;
  if(permute) {
    rval = (int) (p_lesion * (float)cons.size);
    if(rval == 0) return 0;
    int_Array ary;
    int j;
    for(j=0; j<cons.size; j++)
      ary.Add(j);
    ary.Permute();
    ary.size = rval;
    ary.Sort();
    for(j=ary.size-1; j>=0; j--)
      un->DisConnectFrom(Un(ary.FastEl(j)), prjn);
  }
  else {
    int j;
    for(j=cons.size-1; j>=0; j--) {
      if(Random::ZeroOne() <= p_lesion) {
	un->DisConnectFrom(Un(j), prjn);
	rval++;
      }
    }
  }
  return rval;
}

/////////////////////////////////////////////////////////////
// 	To/From Arrays/Matrix

bool RecvCons::ConValuesToArray(float_Array& ary, const char* variable) {
  MemberDef* md = con_type->members.FindName(variable);
  if(TestWarning(!md || !md->type->InheritsFrom(TA_float), "ConValuesToArray",
		 "Variable:", variable, "not found or not a float on units of type:",
		 con_type->name)) {
    return false;
  }
  for(int i=0; i<cons.size; i++) {
    float* val = (float*)md->GetOff((void*)Cn(i));
    ary.Add(*val);
  }
  return true;
}

bool RecvCons::ConValuesToMatrix(float_Matrix& mat, const char* variable) {
  MemberDef* md = con_type->members.FindName(variable);
  if(TestWarning(!md || !md->type->InheritsFrom(TA_float), "ConValuesToMatrix",
		 "Variable:", variable, "not found or not a float on units of type:",
		 con_type->name)) {
    return false;
  }
  if(TestWarning(mat.size < cons.size, "ConValuesToMatrix", "matrix size too small")) {
    return false;
  }

  for(int i=0; i<cons.size; i++) {
    float* val = (float*)md->GetOff((void*)Cn(i));
    mat.FastEl_Flat(i) = *val;
  }
  return true;
}

bool RecvCons::ConValuesFromArray(float_Array& ary, const char* variable) {
  MemberDef* md = con_type->members.FindName(variable);
  if(TestWarning(!md || !md->type->InheritsFrom(TA_float), "ConValuesFromArray",
		 "Variable:", variable, "not found or not a float on units of type:",
		 con_type->name)) {
    return false;
  }
  int mx = MIN(cons.size, ary.size);
  for(int i=0; i<mx; i++) {
    float* val = (float*)md->GetOff((void*)Cn(i));
    *val = ary[i];
  }
  Unit* ru = GET_MY_OWNER(Unit);
  Init_Weights_post(ru);	// update weights after mod
  return true;
}

bool RecvCons::ConValuesFromMatrix(float_Matrix& mat, const char* variable) {
  MemberDef* md = con_type->members.FindName(variable);
  if(TestWarning(!md || !md->type->InheritsFrom(TA_float), "ConValuesFromMatrix",
		 "Variable:", variable, "not found or not a float on units of type:",
		 con_type->name)) {
    return false;
  }
  int mx = MIN(cons.size, mat.size);
  for(int i=0; i<mx; i++) {
    float* val = (float*)md->GetOff((void*)Cn(i));
    *val = mat.FastEl_Flat(i);
  }
  Unit* ru = GET_MY_OWNER(Unit);
  Init_Weights_post(ru);	// update weights after mod
  return true;
}

/////////////////////////////////////////////////////////////
// 	Save/Load Weights

void RecvCons::SaveWeights_strm(ostream& strm, Unit*, RecvCons::WtSaveFormat fmt) {
  if((prjn == NULL) || (!(bool)prjn->from)) {
    strm << "<Cn 0>\n";
    goto end_tag;		// don't do anything
  }
  strm << "<Cn " << cons.size << ">\n";
  switch(fmt) {
  case RecvCons::TEXT:
    for(int i=0; i < cons.size; i++) {
      int lidx = Un(i)->GetMyLeafIndex();
      if(TestWarning(lidx < 0, "SaveWeights_strm", "can't find unit")) {
	lidx = 0;
      }
      strm << lidx << " " << Cn(i)->wt << "\n";
    }
    break;
  case RecvCons::BINARY:
    for(int i=0; i < cons.size; i++) {
      int lidx = Un(i)->GetMyLeafIndex();
      if(TestWarning(lidx < 0, "SaveWeights_strm", "can't find unit")) {
	lidx = 0;
      }
      strm.write((char*)&(lidx), sizeof(lidx));
      strm.write((char*)&(Cn(i)->wt), sizeof(Cn(i)->wt));
    }
    strm << "\n";
    break;
  }
 end_tag:
  strm << "</Cn>\n";
}

// return values:
// TAG_END = successfully got to end of thing;
// TAG_NONE = some kind of error
// TAG_EOF = EOF

int RecvCons::LoadWeights_StartTag(istream& strm, const String& tag, String& val, bool quiet) {
  String in_tag;
  int stat = taMisc::read_tag(strm, in_tag, val);
  if(stat == taMisc::TAG_END) return taMisc::TAG_NONE; // some other end -- not good
  if(stat != taMisc::TAG_GOT) {
    if(!quiet) taMisc::Warning("RecvCons::LoadWeights: bad read of start tag:", tag);
    return stat;
  }
  if(in_tag != tag) {
    if(!quiet) taMisc::Warning("RecvCons::LoadWeights: read different start tag:", in_tag,
			       "expecting:", tag);
    return taMisc::TAG_NONE; // bumping up against some other tag
  }
  return stat;
}

int RecvCons::LoadWeights_EndTag(istream& strm, const String& trg_tag, String& cur_tag, int& stat, bool quiet) {
  String val;
  if(stat != taMisc::TAG_END)	// haven't already hit the end
    stat = taMisc::read_tag(strm, cur_tag, val);
  if((stat != taMisc::TAG_END) || (cur_tag != trg_tag)) {
    if(!quiet) taMisc::Warning("RecvCons::LoadWeights: bad read of end tag:", trg_tag, "got:",
			       cur_tag, "stat:", String(stat));
    if(stat == taMisc::TAG_END) stat = taMisc::TAG_NONE;
  }
  return stat;
}

int RecvCons::LoadWeights_strm(istream& strm, Unit* ru, RecvCons::WtSaveFormat fmt, bool quiet) {
  if((prjn == NULL) || (!(bool)prjn->from)) {
    return SkipWeights_strm(strm, fmt, quiet); // bail
  }
  String tag, val;
  int stat = RecvCons::LoadWeights_StartTag(strm, "Cn", val, quiet);
  if(stat != taMisc::TAG_GOT) return stat;

  int sz = (int)val;
  if(sz < 0) {
    TestWarning(!quiet, "LoadWeights_strm", "read size < 0");
    return taMisc::TAG_NONE;
  }
  ru->n_recv_cons += sz - cons.size;
  if(cons.alloc_size == 0) cons.Alloc(sz); // loading into empty group: make room
  cons.SetSize(sz);			   // this will give an error if sz > alloc_size

  for(int i=0; i < cons.size; i++) {
    int lidx;
    if(fmt == RecvCons::TEXT) {
      taMisc::read_till_eol(strm);
      lidx = (int)taMisc::LexBuf.before(' ');
      Cn(i)->wt = (float)taMisc::LexBuf.after(' ');
    }
    else {			// binary
      strm.read((char*)&(lidx), sizeof(lidx));
      strm.read((char*)&(Cn(i)->wt), sizeof(Cn(i)->wt));
    }

    Unit* su = prjn->from->units.Leaf(lidx);
    if(!su) {
      TestWarning(!quiet, "LoadWeights_strm", "unit at leaf index: ",
		  String(lidx), "not found in layer:", prjn->from->name);
      i--; cons.SetSize(cons.size-1);
      ru->n_recv_cons--;
      continue;
    }
    SendCons* send_gp = su->send.SafeEl(prjn->send_idx);
    if(!send_gp) {
      TestWarning(!quiet, "LoadWeights_strm", "unit at leaf index: ",
		  String(lidx), "does not have proper send group:", String(prjn->send_idx));
      i--; cons.SetSize(cons.size-1);
      ru->n_recv_cons--;
      continue;
    }
    if(units.size <= i) {
      units.Link(su);
      int sidx = send_gp->units.FindEl(ru);
      if(sidx >= 0) {
	send_gp->cons.ReplaceLinkIdx(sidx, Cn(i));
      }
      else {
	send_gp->LinkCon(Cn(i), ru);
      }
    }
    else if(su != Un(i)) {
      units.ReplaceLinkIdx(i, su);
      int sidx = send_gp->units.FindEl(ru);
      if(sidx >= 0) {
	send_gp->cons.ReplaceLinkIdx(sidx, Cn(i));
      }
      else {
	send_gp->LinkCon(Cn(i), ru);
      }
    }
  }
  RecvCons::LoadWeights_EndTag(strm, "Cn", tag, stat, quiet);

  Init_Weights_post(ru);	// update weights after loading
  return stat;			// should be tag end!
}

int RecvCons::SkipWeights_strm(istream& strm, RecvCons::WtSaveFormat fmt, bool quiet) {
  String tag, val;
  int stat = RecvCons::LoadWeights_StartTag(strm, "Cn", val, quiet);
  if(stat != taMisc::TAG_GOT) return stat;

  int sz = (int)val;
  if(sz < 0) {
    return taMisc::TAG_NONE;
  }

  for(int i=0; i < sz; i++) {
    int lidx;
    float wt;
    if(fmt == RecvCons::TEXT) {
      taMisc::read_till_eol(strm);
    }
    else {			// binary
      strm.read((char*)&(lidx), sizeof(lidx));
      strm.read((char*)&(wt), sizeof(wt));
    }
  }
  RecvCons::LoadWeights_EndTag(strm, "Cn", tag, stat, quiet);
  return stat;
}

void RecvCons::SaveWeights(const String& fname, Unit* ru, RecvCons::WtSaveFormat fmt) {
  taFiler* flr = GetSaveFiler(fname, ".wts", true);
  if(flr->ostrm)
    SaveWeights_strm(*flr->ostrm, ru, fmt);
  flr->Close();
  taRefN::unRefDone(flr);
}

int RecvCons::LoadWeights(const String& fname, Unit* ru, RecvCons::WtSaveFormat fmt, bool quiet) {
  taFiler* flr = GetLoadFiler(fname, ".wts", true);
  int rval = 0;
  if(flr->istrm)
    rval = LoadWeights_strm(*flr->istrm, ru, fmt, quiet);
  flr->Close();
  taRefN::unRefDone(flr);
  return rval;
}

/////////////////////////////////////////////////////////////
// 	Dump Load/Save

// have to implement after save_value because we're not saving a real
// path that can be loaded with Load

int RecvCons::Dump_Save_Value(ostream& strm, taBase* par, int indent) {
  int rval = inherited::Dump_Save_Value(strm, par, indent); // first dump members
  if(!rval)
    return rval;

  // close off the regular members
  taMisc::indent(strm, indent,1) << "};\n";

  // output the units
  taMisc::indent(strm, indent, 1) << "{ con_alloc = " << cons.alloc_size << ";\n";
  taMisc::indent(strm, indent+1, 1) << "units = {";
  for(int i=0; i<units.size; i++) {
    if(Un(i))
      strm << Un(i)->GetMyLeafIndex() << "; ";
    else
      strm << -1 << "; ";	// null..
  }
  strm << "};\n";

  // output the connection values
  for(int j=0; j<con_type->members.size; j++) {
    MemberDef* md = con_type->members.FastEl(j);
    if((md->type->ptr > 0) || (md->HasOption("NO_SAVE")))
      continue;
    taMisc::indent(strm, indent+1,1) << md->name << " = {";
    for(int i=0; i<cons.size; i++) {
      strm << md->type->GetValStr(md->GetOff((void*)Cn(i))) << "; ";
    }
    strm << "};\n";
  }
  return true;
}

int RecvCons::Dump_Load_Value(istream& strm, taBase*) {
  int rval = inherited::Dump_Load_Value(strm); // first dump members
  if((rval == EOF) || (rval == 2))
    return rval;

  // just got type information -- impose it on cons:
  cons.SetType(con_type);

  int c = taMisc::read_till_lbracket(strm);	// get past opening bracket
  if(c == EOF) return EOF;
  c = taMisc::read_word(strm);
  if(TestWarning(taMisc::LexBuf != "con_alloc", "Dump_Load_Value",
		 "Expecting: 'con_alloc' in load file, got:",
		 taMisc::LexBuf,"instead")) {
    return false;
  }
  // skip =
  c = taMisc::skip_white(strm);
  if(TestWarning(c != '=', "Dump_Load_Value",
		 "Missing '=' in dump file for con_alloc in RecvCons")) {
    return false;
  }
  c = taMisc::read_till_semi(strm);
  int con_alloc = (int)taMisc::LexBuf;
  if(con_alloc > cons.alloc_size) {
    cons.Alloc(con_alloc);	// will free any existing -- ok because we lose links anyway
    units.Alloc(con_alloc);
  }

  c = taMisc::read_word(strm);
  if(TestWarning(taMisc::LexBuf != "units",
		 "Dump_Load_Value", "Expecting 'units' in load file, got:",
		 taMisc::LexBuf,"instead")) {
    return false;
  }
  // skip =
  c = taMisc::skip_white(strm);
  if(TestWarning(c != '=', "Dump_Load_Value", "Missing '=' in dump file for unit")) {
    return false;
  }
  // skip {
  c = taMisc::skip_white(strm);
  if(TestWarning(c != '{', "Dump_Load_Value", "Missing '{' in dump file for unit")) {
    return false;
  }

  // first read in the units
  Unit_Group* ug = NULL;
  if(prjn && prjn->from.ptr()) 
    ug = &(prjn->from->units);
  int c_count = 0;		// number of connections
  while(true) {
    c = taMisc::read_till_rb_or_semi(strm);
    if(c == EOF) return EOF;
    if(c == '}') break;
    Unit* un = NULL;
    int lfidx = (int)taMisc::LexBuf;
    if(ug && (lfidx >= 0)) {
      un = (Unit*)ug->Leaf(lfidx);
      if(TestWarning(!un, "Dump_Load_Value", "Connection unit not found")) {
	continue;
      }
    }
    if(units.size > c_count)
      units.ReplaceLinkIdx(c_count, un);
    else
      units.Link(un);
    c_count++;
  }

  if(c_count <= cons.alloc_size)
    cons.SetSize(c_count);
  else {
    TestWarning(true, "Dump_Load_Value", "More connections read than allocated.",
		"weights will be incomplete");
  }

  // now read in the values
  while(true) {
    c = taMisc::read_word(strm);
    if(c == EOF) return EOF;
    if(c == '}') {
      if(strm.peek() == ';') strm.get(); // get the semi
      break;		// done
    }
    MemberDef* md = con_type->members.FindName(taMisc::LexBuf);
    if(TestWarning(!md, "Dump_Load_Value",
		   "Connection member not found:", taMisc::LexBuf)) {
      c = taMisc::skip_past_err(strm);
      if(c == '}') break;
      continue;
    }
    // skip =
    c = taMisc::skip_white(strm);
    if(TestWarning(c != '=', "Dump_Load_Value",
		   "Missing '=' in dump file for unit")) {
      c = taMisc::skip_past_err(strm);
      continue;
    }
    // skip {
    c = taMisc::skip_white(strm);
    if(TestWarning(c != '{', "Dump_Load_Value",
		   "Missing '{' in dump file for unit")) {
      c = taMisc::skip_past_err(strm);
      continue;
    }

    int i = 0;
    while(true) {
      c = taMisc::read_till_rb_or_semi(strm);
      if(c == EOF) return EOF;
      if(c == '}') break;
      if(i >= cons.size) {
	c = taMisc::skip_past_err_rb(strm); // bail to ending rb
	break;
      }
      Connection* cn = Cn(i);
      md->type->SetValStr(taMisc::LexBuf, md->GetOff((void*)cn));
      i++;
    }
  }
  
  if(prjn && prjn->con_spec.spec) {
    SetConSpec(prjn->con_spec.spec); // must set conspec b/c not otherwise saved or set
    Unit* ru = GET_MY_OWNER(Unit);
    Init_Weights_post(ru);	// update weights after loading
  }
  return true;
}

/////////////////////////////////////////////////////////////////////
//	RecvCons_List

RecvCons* RecvCons_List::NewPrjn(Projection* aprjn) {
  RecvCons* rval = (RecvCons*)New(1, aprjn->recvcons_type);
  taBase::SetPointer((taBase**)&(rval->prjn), aprjn);
  rval->SetConType(aprjn->con_type); // set type of connection to this type..
  rval->SetConSpec(aprjn->con_spec.SPtr());
  return rval;
}

int RecvCons_List::FindPrjnIdx(Projection* aprjn) const {
  for(int g=0; g<size; g++) {
    RecvCons* cg = FastEl(g);
    if(cg->prjn == aprjn) {
      return g;
    }
  }
  return -1;
}

RecvCons* RecvCons_List::FindPrjn(Projection* aprjn) const {
  int idx = FindPrjnIdx(aprjn);
  if(idx >= 0) return FastEl(idx);
  return NULL;
}

int RecvCons_List::FindFromIdx(Layer* from) const {
  for(int g=0; g<size; g++) {
    RecvCons* cg = FastEl(g);
    if((cg->prjn) && (cg->prjn->from.ptr() == from)) {
      return g;
    }
  }
  return -1;
}

RecvCons* RecvCons_List::FindFrom(Layer* from) const {
  int idx = FindFromIdx(from);
  if(idx >= 0) return FastEl(idx);
  return NULL;
}

int RecvCons_List::FindFromNameIdx(const String& fm_nm) const {
  for(int g=0; g<size; g++) {
    RecvCons* cg = FastEl(g);
    if((cg->prjn) && (cg->prjn->from->name == fm_nm)) {
      return g;
    }
  }
  return -1;
}

RecvCons* RecvCons_List::FindFromName(const String& fm_nm) const {
  int idx = FindFromNameIdx(fm_nm);
  if(idx >= 0) return FastEl(idx);
  return NULL;
}


int RecvCons_List::FindTypeFromIdx(TypeDef* prjn_td, Layer* from) const {
  for(int g=0; g<size; g++) {
    RecvCons* cg = FastEl(g);
    if((cg->prjn) && (cg->prjn->from.ptr() == from) &&
       (cg->prjn->InheritsFrom(prjn_td)))
    {
      return g;
    }
  }
  return -1;
}

RecvCons* RecvCons_List::FindTypeFrom(TypeDef* prjn_td, Layer* from) const {
  int idx = FindTypeFromIdx(prjn_td, from);
  if(idx >= 0) return FastEl(idx);
  return NULL;
}


bool RecvCons_List::RemovePrjn(Projection* aprjn) {
  bool rval = false;
  int g;
  for(g=size-1; g>=0; g--) {
    RecvCons* cg = FastEl(g);
    if(cg->prjn == aprjn) {
      cg->prjn->projected = false;
      RemoveIdx(g);
      rval = true;
    }
  }
  return rval;
}

bool RecvCons_List::RemoveFrom(Layer* from) {
  bool rval = false;
  int g;
  for(g=size-1; g>=0; g--) {
    RecvCons* cg = FastEl(g);
    if((cg->prjn) && (cg->prjn->from.ptr() == from)) {
      cg->prjn->projected = false;
      RemoveIdx(g);
      rval = true;
    }
  }
  return rval;
}

/////////////////////////////////////////////////////////////////////
//	SendCons

void SendCons::Initialize() {
  // derived classes need to set new basic con types
  con_type = &TA_Connection;
  prjn = NULL;
  recv_idx = -1;
  m_con_spec = NULL;
}

void SendCons::InitLinks() {
  inherited::InitLinks();
}

void SendCons::CutLinks() {
  cons.Reset();
  units.Reset();
  taBase::DelPointer((taBase**)&prjn);
  m_con_spec = NULL;
  inherited::CutLinks();
}

void SendCons::Copy_(const SendCons& cp) {
  SetConType(cp.con_type);
  // do not copy these: they are updated in LinkSendCons after network copy
//   cons.Borrow(cp.cons);
//   units.Borrow(cp.units);
  m_con_spec = cp.m_con_spec;
  taBase::SetPointer((taBase**)&prjn, cp.prjn);
//   recv_idx = cp.recv_idx; // do not copy: updated in FixPrjnIndexes after network copy
}

void SendCons::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
}

int SendCons::UpdatePointers_NewObj(taBase* old_ptr, taBase* new_ptr) {
  int nchg = inherited::UpdatePointers_NewObj(old_ptr, new_ptr);
  nchg += units.UpdatePointers_NewObj(old_ptr, new_ptr);
  return nchg;
}

void SendCons::CheckThisConfig_impl(bool quiet, bool& rval) { 
  inherited::CheckThisConfig_impl(quiet, rval);

  if(CheckError(!GetConSpec(), quiet, rval, "No con spec set")) {
    return;			// fatal!
  }
  if(CheckError(!prjn, quiet, rval, "null projection! (Connect will fix)")) {
    return; 			// fatal!
  }
  if(CheckError((GetTypeDef() != prjn->sendcons_type), quiet, rval,
		"type does not match sendcons_type for projection -- should be:",
		prjn->recvcons_type->name)) {
    prjn->projected = false;
  }
  if(CheckError((con_type != prjn->con_type), quiet, rval,
		"connection type does not match prjn con_type -- should be:",
		prjn->con_type->name)) {
    prjn->projected = false;
  }
  if(cons.size > 0) {		// connections exist
    if(CheckError((recv_idx < 0) || (recv_idx != prjn->recv_idx), quiet, rval,
		  "unset recv_idx, do FixPrjnIndexes or Connect")) {
      prjn->projected = false;
    }
    Unit* ru = Un(0);
    if(CheckError((ru->recv.size <= recv_idx), quiet, rval,
		  "recv_idx is out of range on recv unit. Do Actions/Remove Cons, then Build, Connect on Network")) {
      prjn->projected = false;
    }
    else {
      RecvCons* rucg = ru->recv.SafeEl(recv_idx);
      if(CheckError(!rucg, quiet, rval,
		    "recv_idx is not set correctly (recv guy is NULL). Do Actions/Remove Cons, then Build, Connect on Network")) {
	prjn->projected = false;
      }
      else {
	if(CheckError((rucg->prjn != prjn), quiet, rval,
		      "recv_idx doesn't have correct prjn on recv unit. Do Actions/Remove Cons, then Build, Connect on Network")) {
	  prjn->projected = false;
	}
      }
    }
  }
}

bool SendCons::ChangeMyType(TypeDef*) {
  TestError(true, "ChangeMyType", "Cannot change type of con_groups -- change type setting in projection and reconnect network instead");
  return false;
}

void SendCons::SetConType(TypeDef* cn_tp) {
  con_type = cn_tp;
}

void SendCons::MonitorVar(NetMonitor* net_mon, const String& variable) {
  if(!net_mon) return;
  net_mon->AddObject(this, variable);
}

/////////////////////////////////////////////////////////////
// 	Connection Creation/Deletion

void SendCons::LinkCon(Connection* cn, Unit* un) {
  cons.Link(cn);
  units.Link(un);
}

bool SendCons::RemoveConIdx(int i) {
  units.RemoveIdx(i);
  return cons.RemoveIdx(i);
}

bool SendCons::RemoveConUn(Unit* un) {
  int idx;
  if((idx = units.FindEl(un)) < 0)
    return false;
  return RemoveConIdx(idx);
}

void SendCons::RemoveAll() {
  cons.Reset();
  units.Reset();
}

int SendCons::FindConFromIdx(Unit* un) const {
  return units.FindEl(un);
}

int SendCons::FindConFromNameIdx(const String& unit_nm) const {
  return units.FindNameIdx(unit_nm);
}

Connection* SendCons::FindConFrom(Unit* un) const {
  int idx = FindConFromIdx(un);
  if(idx < 0) return NULL;
  return cons.SafeEl(idx);
}

Connection* SendCons::FindConFromName(const String& unit_nm) const {
  int idx = FindConFromNameIdx(unit_nm);
  if(idx < 0) return NULL;
  return cons.SafeEl(idx);
}

/////////////////////////////////////////////////////////////
// 	To/From Arrays/Matrix

bool SendCons::ConValuesToArray(float_Array& ary, const char* variable) {
  MemberDef* md = con_type->members.FindName(variable);
  if(TestWarning(!md || !md->type->InheritsFrom(TA_float), "ConValuesToArray",
		 "Variable:", variable, "not found or not a float on units of type:",
		 con_type->name)) {
    return false;
  }
  for(int i=0; i<cons.size; i++) {
    float* val = (float*)md->GetOff((void*)Cn(i));
    ary.Add(*val);
  }
  return true;
}

bool SendCons::ConValuesToMatrix(float_Matrix& mat, const char* variable) {
  MemberDef* md = con_type->members.FindName(variable);
  if(TestWarning(!md || !md->type->InheritsFrom(TA_float), "ConValuesToMatrix",
		 "Variable:", variable, "not found or not a float on units of type:",
		 con_type->name)) {
    return false;
  }
  if(TestWarning(mat.size < cons.size, "ConValuesToMatrix", "matrix size too small")) {
    return false;
  }

  for(int i=0; i<cons.size; i++) {
    float* val = (float*)md->GetOff((void*)Cn(i));
    mat.FastEl_Flat(i) = *val;
  }
  return true;
}

bool SendCons::ConValuesFromArray(float_Array& ary, const char* variable) {
  MemberDef* md = con_type->members.FindName(variable);
  if(TestWarning(!md || !md->type->InheritsFrom(TA_float), "ConValuesFromArray",
		 "Variable:", variable, "not found or not a float on units of type:",
		 con_type->name)) {
    return false;
  }
  int mx = MIN(cons.size, ary.size);
  for(int i=0; i<mx; i++) {
    float* val = (float*)md->GetOff((void*)Cn(i));
    *val = ary[i];
  }
  return true;
}

bool SendCons::ConValuesFromMatrix(float_Matrix& mat, const char* variable) {
  MemberDef* md = con_type->members.FindName(variable);
  if(TestWarning(!md || !md->type->InheritsFrom(TA_float), "ConValuesFromMatrix",
		 "Variable:", variable, "not found or not a float on units of type:",
		 con_type->name)) {
    return false;
  }
  int mx = MIN(cons.size, mat.size);
  for(int i=0; i<mx; i++) {
    float* val = (float*)md->GetOff((void*)Cn(i));
    *val = mat.FastEl_Flat(i);
  }
  return true;
}

/////////////////////////////////////////////////////////////////////
//	SendCons_List

SendCons* SendCons_List::NewPrjn(Projection* aprjn) {
  SendCons* rval = (SendCons*)New(1, aprjn->sendcons_type);
  taBase::SetPointer((taBase**)&(rval->prjn), aprjn);
  rval->con_type = aprjn->con_type; // set type of connection to this type..
  rval->SetConSpec(aprjn->con_spec.SPtr());
  return rval;
}

int SendCons_List::FindPrjnIdx(Projection* aprjn) const {
  for(int g=0; g<size; g++) {
    SendCons* cg = FastEl(g);
    if(cg->prjn == aprjn) {
      return g;
    }
  }
  return -1;
}

SendCons* SendCons_List::FindPrjn(Projection* aprjn) const {
  int idx = FindPrjnIdx(aprjn);
  if(idx >= 0) return FastEl(idx);
  return NULL;
}

int SendCons_List::FindToIdx(Layer* to) const {
  for(int g=0; g<size; g++) {
    SendCons* cg = FastEl(g);
    if((cg->prjn) && (cg->prjn->layer == to)) {
      return g;
    }
  }
  return -1;
}

SendCons* SendCons_List::FindTo(Layer* to) const {
  int idx = FindToIdx(to);
  if(idx >= 0) return FastEl(idx);
  return NULL;
}

int SendCons_List::FindToNameIdx(const String& to_nm) const {
  for(int g=0; g<size; g++) {
    SendCons* cg = FastEl(g);
    if((cg->prjn) && (cg->prjn->layer->name == to_nm)) {
      return g;
    }
  }
  return -1;
}

SendCons* SendCons_List::FindToName(const String& to_nm) const {
  int idx = FindToNameIdx(to_nm);
  if(idx >= 0) return FastEl(idx);
  return NULL;
}


int SendCons_List::FindTypeToIdx(TypeDef* prjn_td, Layer* to) const {
  for(int g=0; g<size; g++) {
    SendCons* cg = FastEl(g);
    if((cg->prjn) && (cg->prjn->layer == to) &&
       (cg->prjn->InheritsFrom(prjn_td)))
    {
      return g;
    }
  }
  return -1;
}

SendCons* SendCons_List::FindTypeTo(TypeDef* prjn_td, Layer* to) const {
  int idx = FindTypeToIdx(prjn_td, to);
  if(idx >= 0) return FastEl(idx);
  return NULL;
}

bool SendCons_List::RemovePrjn(Projection* aprjn) {
  bool rval = false;
  int g;
  for(g=size-1; g>=0; g--) {
    SendCons* cg = FastEl(g);
    if(cg->prjn == aprjn) {
      cg->prjn->projected = false;
      RemoveEl(cg);
      rval = true;
    }
  }
  return rval;
}

bool SendCons_List::RemoveTo(Layer* to) {
  bool rval = false;
  int g;
  for(g=size-1; g>=0; g--) {
    SendCons* cg = FastEl(g);
    if((cg->prjn) && (cg->prjn->layer == to)) {
      cg->prjn->projected = false;
      RemoveEl(cg);
      rval = true;
    }
  }
  return rval;
}

////////////////////////
//	Unit	      //
////////////////////////

void UnitSpec::Initialize() {
  min_obj_type = &TA_Unit;
  act_range.max = 1.0f; act_range.min = 0.0f;
  act_range.range = 1.0f; act_range.scale = 1.0f;
  bias_con_type = NULL;
  sse_tol = 0.0f;
}

void UnitSpec::InitLinks() {
  inherited::InitLinks();
  children.SetBaseType(&TA_UnitSpec); // allow all of this general spec type to be created under here
  children.el_typ = GetTypeDef(); // but make the default to be me!
  taBase::Own(act_range, this);
  taBase::Own(bias_spec, this);
  // don't do this if loading -- creates specs in all the wrong places..
  // specs that own specs have this problem
  if(!taMisc::is_loading) {
    Network* net = (Network *) GET_MY_OWNER(Network);
    if(net && !net->HasBaseFlag(COPYING))
      bias_spec.SetDefaultSpec(this);
  }
}

void UnitSpec::CutLinks() {
  bias_spec.CutLinks();
  inherited::CutLinks();
}

void UnitSpec::Copy_(const UnitSpec& cp) {
  act_range = cp.act_range;
  bias_con_type = cp.bias_con_type;
  bias_spec = cp.bias_spec;
  sse_tol = cp.sse_tol;
}

bool UnitSpec::CheckConfig_Unit(Unit* un, bool quiet) {
  return true;
}

void UnitSpec::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  if(!bias_con_type) return;
  if(CheckError(!bias_spec.spec, quiet, rval,
		 "Bias con type of:", bias_con_type->name,
		 "does not have a spec set!"))
    return;
  CheckError((!bias_con_type->InheritsFrom(bias_spec.spec->min_obj_type)), quiet, rval,
	     "Bias con type of:", bias_con_type->name,
	     "is not of the correct type for the bias con spec,"
	     "which needs at least a:", bias_spec.spec->min_obj_type->name);
}

bool UnitSpec::CheckType_impl(TypeDef* td) {
  // other specs are allowed to own any kind of other spec,
  // and layers and projections also contain specs..
  if (td->InheritsFrom(TA_Layer))
    return true;
  return inherited::CheckType_impl(td);
}

bool UnitSpec::CheckObjectType_impl(TAPtr obj) {
  // other specs are allowed to own any kind of other spec,
  // and layers and projections also contain specs..
  if (obj->InheritsFrom(TA_Layer))
    return true;
  return inherited::CheckObjectType_impl(obj);
}

void UnitSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  act_range.UpdateAfterEdit();
}

void UnitSpec::BuildBiasCons() {
  Network* net = (Network *) GET_MY_OWNER(Network);
  if (!net) return;
  net->BuildUnits();
}

////////////////////////////////////////////////////////////////////////////////
//	Below are the primary computational interface to the Network Objects
//	for performing algorithm-specific activation and learning
//	Many functions operate directly on the units via threads, and then
//	call through to the layers for any layer-level subsequent processing
// 	units typically call spec versions except for basic stuff

// 	Init functions are NOT threaded, while Compute functions are

void UnitSpec::Init_Acts(Unit* u, Network* net) {
  u->Init_InputData();
  u->net = 0.0f;
  u->act = 0.0f;
}

void UnitSpec::Init_dWt(Unit* u, Network* net) {
  for(int g = 0; g < u->recv.size; g++) {
    RecvCons* recv_gp = u->recv.FastEl(g);
    if(recv_gp->prjn->from->lesioned() || !recv_gp->cons.size) continue;
    recv_gp->Init_dWt(u);
  }
  if(u->bias.cons.size && u->bias.Cn(0))
    bias_spec->C_Init_dWt(&u->bias, u->bias.Cn(0), u, NULL); // this is a virtual fun
}

void UnitSpec::Init_Weights(Unit* u, Network* net) {
#ifdef DMEM_COMPILE
  if(!u->DMem_IsLocal()) {
    // make up for random numbers not being used for non-local connections.
    for(int i=0; i<u->n_recv_cons; i++) Random::ZeroOne();
  }
  else
#endif
    {
      for(int g = 0; g < u->recv.size; g++) {
	RecvCons* recv_gp = u->recv.FastEl(g);
	// ignore lesion here because n_recv_cons does not take into account lesioned layers, so dmem would get out of sync
	//    if(!recv_gp->prjn->from->lesioned())
	recv_gp->Init_Weights(u);
      }
    }

  if(u->bias.cons.size && u->bias.Cn(0)) {
    bias_spec->C_Init_Weights(&u->bias, u->bias.Cn(0), u, NULL); // this is a virtual fun
    bias_spec->C_Init_dWt(&u->bias, u->bias.Cn(0), u, NULL); // don't forget delta too!!
  }
}

void UnitSpec::Init_Weights_post(Unit* u, Network* net) {
  for(int g = 0; g < u->recv.size; g++) {
    RecvCons* recv_gp = u->recv.FastEl(g);
    if(recv_gp->prjn->from->lesioned() || !recv_gp->cons.size) continue;
    recv_gp->Init_Weights_post(u);
  }
}

void UnitSpec::Compute_Netin(Unit* u, Network* net, int thread_no) {
  u->net = 0.0f;
  for(int g = 0; g < u->recv.size; g++) {
    RecvCons* recv_gp = u->recv.FastEl(g);
    if(recv_gp->prjn->from->lesioned() || !recv_gp->cons.size) continue;
    u->net += recv_gp->Compute_Netin(u);
  }
  if(u->bias.cons.size)
    u->net += u->bias.Cn(0)->wt;
}

void UnitSpec::Send_Netin(Unit* u, Network* net, int thread_no) {
  // typically the whole point of using sender based net input is that you want to check
  // here if the sending unit's activation (i.e., this one) is above some threshold
  // so you don't send if it isn't above that threshold..  this isn't implemented here though.
  if(thread_no < 0) thread_no = 0; // use 0 thread in tmp matrix in this case
  for(int g = 0; g < u->send.size; g++) {
    SendCons* send_gp = u->send.FastEl(g);
    Layer* tol = send_gp->prjn->layer;
    if(tol->lesioned() || !send_gp->cons.size) continue;
    send_gp->Send_Netin(net, thread_no, u);
  }
}

void UnitSpec::Compute_SentNetin(Unit* u, Network* net, float sent_netin) {
  // called by network-level Send_Netin function to integrate sent netin value
  // with current net input value -- default is just to set to net val + bias wt if avail
  u->net = sent_netin;
  if(u->bias.cons.size)
    u->net += u->bias.Cn(0)->wt;
}

void UnitSpec::Compute_Act(Unit* u, Network* net, int thread_no) {
  if(u->ext_flag & Unit::EXT)
    u->act = u->ext;
  else
    u->act = u->net;
}

void UnitSpec::Compute_dWt(Unit* u, Network* net, int thread_no) {
  for(int g = 0; g < u->recv.size; g++) {
    RecvCons* recv_gp = u->recv.FastEl(g);
    if(recv_gp->prjn->from->lesioned() || !recv_gp->cons.size) continue;
    recv_gp->Compute_dWt(u);
  }
  // NOTE: derived classes must supply bias.Cn(0)->Compute_dWt call because C_Compute_dWt
  // is not virtual, so if called here, only ConSpec version would be called.
  // This is not true of Init_Weights and Init_dWt, which are virtual.
}

void UnitSpec::Compute_Weights(Unit* u, Network* net, int thread_no) {
  for(int g = 0; g < u->recv.size; g++) {
    RecvCons* recv_gp = u->recv.FastEl(g);
    if(recv_gp->prjn->from->lesioned() || !recv_gp->cons.size) continue;
    recv_gp->Compute_Weights(u);
  }
  // NOTE: derived classes must supply bias.Cn(0)->Compute_Weights call because C_Compute_Weights
  // is not virtual, so if called here, only ConSpec version would be called.
  // This is not true of Init_Weights and Init_dWt, which are virtual.
}

float UnitSpec::Compute_SSE(Unit* u, Network* net, bool& has_targ) {
  float sse = 0.0f;
  has_targ = false;
  if(u->ext_flag & (Unit::TARG | Unit::COMP)) {
    has_targ = true;
    float uerr = u->targ - u->act;
    if(fabsf(uerr) >= sse_tol)
      sse = uerr * uerr;
  }
  return sse;
}


/////// Unit ///////

void Unit::Initialize() {
  ext_flag = NO_EXTERNAL;
  targ = 0.0f;
  ext = 0.0f;
  act = 0.0f;
  net = 0.0f;
  wt_prjn = tmp_calc1 = 0.0f;
  snap = 0.0f;
  n_recv_cons = 0;
  idx = -1;
  flat_idx = 0;
  m_unit_spec = NULL;
}

void Unit::Destroy() {
  CutLinks();
}

void Unit::InitLinks() {
  inherited::InitLinks();
  taBase::Own(recv, this);	// always own your constitutents
  taBase::Own(send, this);
  taBase::Own(bias, this);
  taBase::Own(pos, this);
  BuildUnits();
  // note: no longer supporting incremental construction by hand..
//   Layer* lay = GET_MY_OWNER(Layer);
//   if(lay && !taMisc::is_loading)
//     lay->LayoutUnits(this);
}

void Unit::CutLinks() {
  recv.CutLinks();
  send.CutLinks();
  bias.CutLinks();
  m_unit_spec = NULL;
  idx = -1;
  flat_idx = 0;
  inherited::CutLinks();
}

void Unit::Copy_(const Unit& cp) {
  pos = cp.pos;
  ext_flag = cp.ext_flag;
  targ = cp.targ;
  ext = cp.ext;
  act = cp.act;
  net = cp.net;
  wt_prjn = cp.wt_prjn;
  tmp_calc1 = cp.tmp_calc1;
  snap = cp.snap;
  recv = cp.recv;
  send = cp.send;
  bias = cp.bias;
  n_recv_cons = cp.n_recv_cons;
  m_unit_spec = cp.m_unit_spec;
}

void Unit::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  // no negative positions
  pos.x = MAX(0,pos.x); pos.y = MAX(0,pos.y);  pos.z = MAX(0,pos.z);
  // stay within layer->un_geom
//   Layer* lay = GET_MY_OWNER(Layer);
//   if(!lay) return;
//   pos.x = MIN(lay->un_geom.x-1,pos.x); pos.y = MIN(lay->un_geom.y-1,pos.y);
  pos.z = 0;			// always zero: can't go out of plane
}

int Unit::GetMyLeafIndex() {
  if(idx < 0 || !owner) return idx;
  Unit_Group* ug = (Unit_Group*)owner;
  if(ug->owner->InheritsFrom(&TA_Layer))
    return idx; // simple: we're the only unit group
  // note: this assumes only one layer of subgroups, which is all that is supported anyway
  Layer* lay = (Layer*)ug->owner->GetOwner()->GetOwner();
  int cum_idx = 0;
  for(int i=0;i<lay->units.gp.size;i++) {
    Unit_Group* sg = (Unit_Group*)lay->units.gp.FastEl(i);
    if(sg == ug) return cum_idx + idx;
    cum_idx += sg->size;
  }
  return -1;			// not found
}

void Unit::GetLayerAbsPos(TwoDCoord& lay_abs_pos) {
  //note: simplest and fastest is to just add Unit_Group
  // pos, even though it is 0 for flat guys, it saves
  // a bunch of GetOwners
  lay_abs_pos = pos;
  Unit_Group* ug = dynamic_cast<Unit_Group*>(owner);
  if (ug) { // should always succeed...
    lay_abs_pos.x += ug->pos.x;
    lay_abs_pos.y += ug->pos.y;
  }
}

void Unit::AddRelPos(TDCoord& rel_pos) {
  Unit_Group* ugp = GET_MY_OWNER(Unit_Group);
  if (ugp) {
    rel_pos += ugp->pos;
    ugp->AddRelPos(rel_pos);
  }
}
 
#ifdef DMEM_COMPILE
int Unit::dmem_this_proc = 0;
#endif

void Unit::ApplyInputData(float val, ExtType act_ext_flags, Random* ran, bool na_by_range) {
  // note: not all flag values are valid, so following is a fuzzy cascade
  // ext is the default place, so we check for 
  if(na_by_range) {
    UnitSpec* us = GetUnitSpec();
    if(us) {
      if(!us->act_range.RangeTestEq(val))
	return;
    }
  }

  if (ran && (ran->type != Random::NONE)) {
    val += ran->Gen();
  }
  if (act_ext_flags & Unit::EXT) {
    ext = val;
    SetExtFlag(Unit::EXT);
  } else {
    targ = val;
    if (act_ext_flags & Unit::TARG)
      SetExtFlag(Unit::TARG);
    else if (act_ext_flags & Unit::COMP)
      SetExtFlag(Unit::COMP);
  }
}

bool Unit::BuildUnits() {
  bool rval = false;
  if(!GetUnitSpec())
    return false;
  TypeDef* bstd = GetUnitSpec()->bias_con_type;
  if(bstd == NULL) {
    bias.Reset();
    rval = true;
    bias.SetConSpec(NULL);
  }
  else {
    bias.SetConType(bstd);
    if(bias.cons.size == 0) {
      bias.cons.Alloc(1);
      bias.NewCon(NULL);		// null unit (or should it be this!?)
    }
    bias.SetConSpec(GetUnitSpec()->bias_spec.SPtr()); // not generally used, but could be!
  }
  return rval;
}

void Unit::CheckChildConfig_impl(bool quiet, bool& rval) {
  inherited::CheckChildConfig_impl(quiet, rval);
  recv.CheckConfig(quiet, rval);
  send.CheckConfig(quiet, rval);
}

void Unit::CheckThisConfig_impl(bool quiet, bool& rval) { 
  inherited::CheckThisConfig_impl(quiet, rval);

  if(CheckError(!GetUnitSpec(), quiet, rval, "No unit spec set for unit")) {
    return;			// fatal
  }
  if(!GetUnitSpec()->CheckConfig_Unit(this, quiet)) {
    rval = false;
  }
}

bool Unit::CheckBuild(bool quiet) {
  bool rval;
  if(CheckError(!GetUnitSpec(), quiet, rval,
		"Unit CheckBuild: no unit spec set for unit")) {
    return false;		// fatal
  }
  UnitSpec* us = GetUnitSpec();
  if(!us->bias_con_type) {
    if(CheckError(bias.cons.size > 0, quiet, rval,
		  "Unit CheckBuild: bias weight exists but no type")) {
      return false;
    }
  }
  else {
    if(CheckError((!bias.cons.size || (bias.con_type != us->bias_con_type)),
		  quiet, rval, 
		  "Unit CheckBuild: bias weight null or not same type as specified in UnitSpec:", us->name)) {
      return false;
    }
  }
  return true;
}

void Unit::RemoveCons() {
  recv.RemoveAll();		// blunt, but effective
  send.RemoveAll();
  n_recv_cons = 0;
}

void Unit::MonitorVar(NetMonitor* net_mon, const String& variable) {
  if(!net_mon) return;
  net_mon->AddObject(this, variable);
}

bool Unit::Snapshot(const String& var, SimpleMathSpec& math_op, bool arg_is_snap) {
  MemberDef* md = NULL;
  Variant val = GetValFromPath(var, md, true); // true = warn
  if(val.isNull() || val.isInvalid()) return false;  // already warned
  if(math_op.opr == SimpleMathSpec::NONE) {
    snap = val.toFloat();
  }
  else {
    if(arg_is_snap)
      math_op.arg = snap;
    snap = (float)math_op.Evaluate(val.toDouble());
  }
  return true;
}

void Unit::LinkSendCons() {
  for(int g=0; g<recv.size; g++) {
    RecvCons* recv_gp = recv.FastEl(g);
    recv_gp->LinkSendCons(this);
  }
}

RecvCons* Unit::rcg_rval = NULL;
SendCons* Unit::scg_rval = NULL;

void Unit::ConnectAlloc(int no, Projection* prjn, RecvCons*& cgp) {
#ifdef DMEM_COMPILE
  if(!DMem_IsLocal() && !prjn->con_spec->DMem_AlwaysLocal()) return;
#endif
  if((prjn->recv_idx < 0) || ((cgp = recv.SafeEl(prjn->recv_idx)) == NULL)) {
    cgp = recv.NewPrjn(prjn); // sets the type
    prjn->recv_idx = recv.size-1;
  }
  cgp->AllocCons(no);
}

Connection* Unit::ConnectFrom(Unit* su, Projection* prjn, RecvCons*& recv_gp,
			      SendCons*& send_gp) {
#ifdef DMEM_COMPILE
  if(!DMem_IsLocal() && !prjn->con_spec->DMem_AlwaysLocal()) return NULL;
#endif
  if((prjn->recv_idx < 0) || ((recv_gp = recv.SafeEl(prjn->recv_idx)) == NULL)) {
    recv_gp = recv.NewPrjn(prjn);
    prjn->recv_idx = recv.size-1;
  }
  if((prjn->send_idx < 0) || ((send_gp = su->send.SafeEl(prjn->send_idx)) == NULL)) {
    send_gp = su->send.NewPrjn(prjn);
    prjn->send_idx = su->send.size-1;
  }
  if(recv_gp->send_idx < 0)
    recv_gp->send_idx = prjn->send_idx;
  if(send_gp->recv_idx < 0)
    send_gp->recv_idx = prjn->recv_idx;

  Connection* con = recv_gp->NewCon(su);
  send_gp->LinkCon(con, this);
  n_recv_cons++;
  return con;
}

Connection* Unit::ConnectFromCk(Unit* su, Projection* prjn, RecvCons*& recv_gp,
			      SendCons*& send_gp) {
#ifdef DMEM_COMPILE
  if(!DMem_IsLocal() && !prjn->con_spec->DMem_AlwaysLocal()) return NULL;
#endif
  if((prjn->recv_idx < 0) || ((recv_gp = recv.SafeEl(prjn->recv_idx)) == NULL)) {
    recv_gp = recv.NewPrjn(prjn);
    prjn->recv_idx = recv.size-1;
  }
  if((prjn->send_idx < 0) || ((send_gp = su->send.SafeEl(prjn->send_idx)) == NULL)) {
    send_gp = su->send.NewPrjn(prjn);
    prjn->send_idx = su->send.size-1;
  }
  if(recv_gp->send_idx < 0)
    recv_gp->send_idx = prjn->send_idx;
  if(send_gp->recv_idx < 0)
    send_gp->recv_idx = prjn->recv_idx;

  if(recv_gp->units.FindEl(su) >= 0) // already connected!
    return NULL;

  Connection* con = recv_gp->NewCon(su);
  send_gp->LinkCon(con, this);
  n_recv_cons++;
  return con;
}

bool Unit::DisConnectFrom(Unit* su, Projection* prjn) {
  RecvCons* recv_gp;
  SendCons* send_gp;
  if(prjn) {
    if((prjn->recv_idx < 0) || ((recv_gp = recv.SafeEl(prjn->recv_idx)) == NULL))
      return false;
    if((prjn->send_idx < 0) || ((send_gp = su->send.SafeEl(prjn->send_idx)) == NULL))
      return false;
  }
  else {
    Layer* su_lay = GET_OWNER(su,Layer);
    recv_gp = recv.FindFrom(su_lay);
    if(!recv_gp)	return false;
    if(recv_gp->send_idx >= 0)
      send_gp = su->send.SafeEl(recv_gp->send_idx);
    else
      send_gp = NULL;
    if(send_gp == NULL)
      send_gp = su->send.FindPrjn(recv_gp->prjn);
    if(send_gp == NULL) return false;
    prjn = recv_gp->prjn;
  }

  recv_gp->RemoveConUn(su);
  n_recv_cons--;
  return send_gp->RemoveConUn(this);
}

void Unit::DisConnectAll() {
  RecvCons* recv_gp;
  SendCons* send_gp;
  int g;
  int i;
  for(g=0; g<recv.size; g++) { // the removes cause the leaf_gp to crash..
    recv_gp = recv.FastEl(g);
    for(i=recv_gp->cons.size-1; i>=0; i--) {
      if(recv_gp->send_idx >= 0)
	send_gp = recv_gp->Un(i)->send.SafeEl(recv_gp->send_idx);
      else
	send_gp = NULL;
      if(send_gp == NULL)
	send_gp = recv_gp->Un(i)->send.FindPrjn(recv_gp->prjn);
      if(send_gp)
	send_gp->RemoveConUn(this);
      recv_gp->RemoveConIdx(i);
    }
    recv_gp->send_idx = -1;
  }
  for(g=0; g<send.size; g++) { // the removes cause the leaf_gp to crash..
    send_gp = send.FastEl(g);
    for(i=send_gp->cons.size-1; i>=0; i--) {
      if(send_gp->recv_idx >= 0)
	recv_gp = send_gp->Un(i)->recv.SafeEl(send_gp->recv_idx);
      else
	recv_gp = NULL;
      if(recv_gp == NULL)
	recv_gp = send_gp->Un(i)->recv.FindPrjn(send_gp->prjn);
      if(recv_gp)
	recv_gp->RemoveConUn(this);
      send_gp->RemoveConIdx(i);
    }
    send_gp->recv_idx = -1;
  }
  n_recv_cons = 0;
}

int Unit::CountRecvCons() {
  n_recv_cons = 0;
  for(int g = 0; g < recv.size; g++) {
    RecvCons* cg = recv.FastEl(g);
    n_recv_cons += cg->cons.size;
  }
  return n_recv_cons;
}

void Unit::Copy_Weights(const Unit* src, Projection* prjn) {
  if((bias.cons.size) && (src->bias.cons.size)) {
    bias.Cn(0)->wt = src->bias.Cn(0)->wt;
  }
  int mx = MIN(recv.size, src->recv.size);
  for(int i=0; i<mx; i++) {
    RecvCons* cg = recv.FastEl(i);
    RecvCons* scg = src->recv.FastEl(i);
    if(cg->prjn->from->lesioned() || ((prjn) && (cg->prjn != prjn))) continue;
    cg->Copy_Weights(scg);
  }
}

void Unit::SaveWeights_strm(ostream& strm, Projection* prjn, RecvCons::WtSaveFormat fmt) {
  strm << "<Un>\n";
  float bwt = 0.0;
  if(bias.cons.size) bwt = bias.Cn(0)->wt;
  // always write this for a consistent format
  switch(fmt) {
  case RecvCons::TEXT:
    strm << bwt << "\n";
    break;
  case RecvCons::BINARY:
    strm.write((char*)&(bwt), sizeof(bwt));
    strm << "\n";
    break;
  }
  // not using ITR here in case of DMEM where we write separate files for
  // each process -- need to include size=0 place holders for non-local units
  for(int g = 0; g < recv.size; g++) {
    RecvCons* cg = recv.FastEl(g);
    if(cg->prjn->from->lesioned() || (prjn && (cg->prjn != prjn))) continue;
    strm << "<Cg " << g << " Fm:" << cg->prjn->from->name << ">\n";
    cg->SaveWeights_strm(strm, this, fmt);
    strm << "</Cg>\n";
  }
  strm << "</Un>\n";
}

int Unit::LoadWeights_strm(istream& strm, Projection* prjn, RecvCons::WtSaveFormat fmt, bool quiet) {
  String tag, val;
  int stat = RecvCons::LoadWeights_StartTag(strm, "Un", val, quiet);
  if(stat != taMisc::TAG_GOT) return stat;

  //   String lidx = val.before(' ');
  // todo: could compare lidx with GetMyLeafIdx()...
  float bwt = 0.0;
  switch(fmt) {
  case RecvCons::TEXT:
    taMisc::read_till_eol(strm);
    bwt = (float)taMisc::LexBuf;
    break;
  case RecvCons::BINARY:
    strm.read((char*)&bwt, sizeof(bwt));
    strm.get();		// get the /n
    break;
  }
  if(bias.cons.size) {
    bias.Cn(0)->wt = bwt;
  }

#ifdef DMEM_COMPILE
  if(!DMem_IsLocal()) {
    // bypass non-local connections!
    while(true) {
      stat = taMisc::read_tag(strm, tag, val);
      if(stat != taMisc::TAG_GOT) break;		// *should* break at TAG_END of Un
      if(tag != "Cg") { stat = taMisc::TAG_NONE;  break; } // bumping up against some other tag
      stat = RecvCons::SkipWeights_strm(strm, fmt, quiet); // skip over
      if(stat != taMisc::TAG_END) break; // something is wrong
      stat = taMisc::TAG_NONE;	       // reset so EndTag will definitely read new tag
      RecvCons::LoadWeights_EndTag(strm, "Cg", tag, stat, quiet);
      if(stat != taMisc::TAG_END) break;
    }
  }
  else {
#endif
  while(true) {
    stat = taMisc::read_tag(strm, tag, val);
    if(stat != taMisc::TAG_GOT) break;			// *should* break at TAG_END
    if(tag != "Cg") { stat = taMisc::TAG_NONE;  break; } // bumping up against some other tag
    int gi = (int)val.before(' ');
    String fm = val.after("Fm:");
    RecvCons* cg = NULL;
    if(recv.size > gi) {
      cg = recv.FastEl(gi);
      if(cg->prjn->from->name != fm)
	cg = recv.FindFromName(fm);
    }
    else {
      cg = recv.FindFromName(fm); 
    }
    if(cg) {		
      stat = cg->LoadWeights_strm(strm, this, fmt, quiet);
    }
    else {
      stat = RecvCons::SkipWeights_strm(strm, fmt, quiet); // skip over
    }
    if(stat != taMisc::TAG_END) break; // something is wrong
    stat = taMisc::TAG_NONE;	       // reset so EndTag will definitely read new tag
    RecvCons::LoadWeights_EndTag(strm, "Cg", tag, stat, quiet);
    if(stat != taMisc::TAG_END) break;
  }
#ifdef DMEM_COMPILE
  }
#endif
  RecvCons::LoadWeights_EndTag(strm, "Un", tag, stat, quiet);
  return stat;
}

int Unit::SkipWeights_strm(istream& strm, RecvCons::WtSaveFormat fmt, bool quiet) {
  String tag, val;
  int stat = RecvCons::LoadWeights_StartTag(strm, "Un", val, quiet);
  if(stat != taMisc::TAG_GOT) return stat;

  float bwt = 0.0;
  switch(fmt) {
  case RecvCons::TEXT:
    taMisc::read_till_eol(strm);
    bwt = (float)taMisc::LexBuf;
    break;
  case RecvCons::BINARY:
    strm.read((char*)&bwt, sizeof(bwt));
    strm.get();		// get the /n
    break;
  }
  while(true) {
    stat = taMisc::read_tag(strm, tag, val);
    if(stat != taMisc::TAG_GOT) break;		// *should* break at TAG_END
    if(tag != "Cg") { stat = taMisc::TAG_NONE;  break; } // bumping up against some other tag
    stat = RecvCons::SkipWeights_strm(strm, fmt, quiet); // skip over
    if(stat != taMisc::TAG_END) break; // something is wrong
    stat = taMisc::TAG_NONE;	       // reset so EndTag will definitely read new tag
    RecvCons::LoadWeights_EndTag(strm, "Cg", tag, stat, quiet);
    if(stat != taMisc::TAG_END) break;
  }
  RecvCons::LoadWeights_EndTag(strm, "Un", tag, stat, quiet);
  return stat;
}

void Unit::SaveWeights(const String& fname, Projection* prjn, RecvCons::WtSaveFormat fmt) {
  taFiler* flr = GetSaveFiler(fname, ".wts", true);
  if(flr->ostrm)
    SaveWeights_strm(*flr->ostrm, prjn, fmt);
  flr->Close();
  taRefN::unRefDone(flr);
}

int Unit::LoadWeights(const String& fname, Projection* prjn, RecvCons::WtSaveFormat fmt, bool quiet) {
  taFiler* flr = GetLoadFiler(fname, ".wts", true);
  int rval = false;
  if(flr->istrm)
    rval = LoadWeights_strm(*flr->istrm, prjn, fmt, quiet);
  flr->Close();
  taRefN::unRefDone(flr);
  return rval;
}

void Unit::GetLocalistName() {
  if(name.nonempty()) return;	// only if not otherwise named!
  for(int g = 0; g < recv.size; g++) {
    RecvCons* cg = recv.FastEl(g);
    if(cg->prjn->from->lesioned()) continue;
    if(cg->units.size != 1) continue; // only 1-to-1
    Unit* un = cg->Un(0);
    if(!un->name.empty()) {
      name = un->name;
      break;			// done!
    }
  }
}

void Unit::TransformWeights(const SimpleMathSpec& trans, Projection* prjn) {
  for(int g = 0; g < recv.size; g++) {
    RecvCons* cg = recv.FastEl(g);
    if(cg->prjn->from->lesioned() || ((prjn) && (cg->prjn != prjn))) continue;
    cg->TransformWeights(trans);
  }
}

void Unit::AddNoiseToWeights(const Random& noise_spec, Projection* prjn) {
  for(int g = 0; g < recv.size; g++) {
    RecvCons* cg = recv.FastEl(g);
    if(cg->prjn->from->lesioned() || ((prjn) && (cg->prjn != prjn))) continue;
    cg->AddNoiseToWeights(noise_spec);
  }
}

int Unit::PruneCons(const SimpleMathSpec& pre_proc, Relation::Relations rel,
		       float cmp_val, Projection* prjn)
{
  int rval = 0;
  int g;
  for(g=0; g<recv.size; g++) {
    RecvCons* cg = recv.FastEl(g);
    if(cg->prjn->from->lesioned() || ((prjn) && (cg->prjn != prjn))) continue;
    rval += cg->PruneCons(this, pre_proc, rel, cmp_val);
  }
  n_recv_cons -= rval;
  return rval;
}

int Unit::LesionCons(float p_lesion, bool permute, Projection* prjn) {
  int rval = 0;
  int g;
  for(g=0; g<recv.size; g++) {
    RecvCons* cg = recv.FastEl(g);
    if(cg->prjn->from->lesioned() || ((prjn) && (cg->prjn != prjn))) continue;
    rval += cg->LesionCons(this, p_lesion, permute);
  }
  n_recv_cons -= rval;
  return rval;
}

void Unit::VarToTable(DataTable* dt, const String& variable) {
  if (!dt) {
    taProject* proj = GET_MY_OWNER(taProject);
    dt = proj->GetNewAnalysisDataTable("Unit_Var_" + variable, true);
  }

  Network* net = GET_MY_OWNER(Network);
  if(!net) return;

  NetMonitor nm;
  taBase::Own(nm, this);
  nm.AddUnit(this, variable);
  nm.items[0]->max_name_len = 20; // allow long names
  nm.SetDataNetwork(dt, net);
  nm.UpdateDataTable();
  dt->AddBlankRow();
  nm.GetMonVals();
  dt->WriteClose();
}

bool Unit::ChangeMyType(TypeDef*) {
  TestError(true, "ChangeMyType", "Cannot change type of Units -- change el_typ in Layer units group  and rebuild network instead");
  return false;
}

////////////////////////////
//	ProjectionSpec    //
////////////////////////////

void ProjectionSpec::Initialize() {
  min_obj_type = &TA_Projection;
  self_con = false;
  init_wts = false;
}

void ProjectionSpec::InitLinks() {
  BaseSpec::InitLinks();
  children.SetBaseType(&TA_ProjectionSpec); // allow all of this general spec type to be created under here
  children.el_typ = GetTypeDef(); // but make the default to be me!
}

void ProjectionSpec::RemoveCons(Projection* prjn) {
  Unit* u;
  taLeafItr i;
  if(prjn->layer) {
    FOR_ITR_EL(Unit, u, prjn->layer->units., i) {
      u->recv.RemovePrjn(prjn);
      u->n_recv_cons = 0;
    }
  }

  if(prjn->from) {
    FOR_ITR_EL(Unit, u, prjn->from->units., i)
      u->send.RemovePrjn(prjn);
  }

  prjn->recv_idx = -1;
  prjn->send_idx = -1;
  prjn->projected = false;
}

void ProjectionSpec::Connect(Projection* prjn) {
  RemoveCons(prjn);
  prjn->SetFrom();
  PreConnect(prjn);
  Connect_impl(prjn);
  Init_Weights(prjn);
  prjn->projected = true;
}

int ProjectionSpec::ProbAddCons(Projection*, float, float) {
  return 0;
}

void ProjectionSpec::Init_Weights(Projection* prjn) {
  Unit* u;
  taLeafItr i;
  FOR_ITR_EL(Unit, u, prjn->layer->units., i) {
    for(int g=0; g < u->recv.size; g++) {
      RecvCons* cg = u->recv.FastEl(g);
      if(cg->prjn == prjn)
	cg->Init_Weights(u);
    }
  }
}

void ProjectionSpec::Init_Weights_post(Projection* prjn) {
  Unit* u;
  taLeafItr i;
  FOR_ITR_EL(Unit, u, prjn->layer->units., i) {
    for(int g=0; g < u->recv.size; g++) {
      RecvCons* cg = u->recv.FastEl(g);
      if(cg->prjn == prjn)
	cg->Init_Weights_post(u);
    }
  }
}

void ProjectionSpec::C_Init_Weights(Projection*, RecvCons* cg, Unit* ru) {
  // default is just to do same thing as the conspec would have done..
  CON_GROUP_LOOP(cg, cg->C_Init_Weights(cg->Cn(i), ru, cg->Un(i)));
}

void ProjectionSpec::Init_dWt(Projection* prjn) {
  Unit* u;
  taLeafItr i;
  FOR_ITR_EL(Unit, u, prjn->layer->units., i) {
    int g;
    for(g=0; g < u->recv.size; g++) {
      RecvCons* cg = u->recv.FastEl(g);
      if(cg->prjn == prjn)
	cg->Init_dWt(u);
    }
  }
}

void ProjectionSpec::PreConnect(Projection* prjn) {
  if(!(bool)prjn->from)	return;

  // make first set of congroups to get indicies
  Unit* first_ru = (Unit*)prjn->layer->units.Leaf(0);
  Unit* first_su = (Unit*)prjn->from->units.Leaf(0);
  if((first_ru == NULL) || (first_su == NULL))
    return;
  RecvCons* recv_gp = first_ru->recv.NewPrjn(prjn);
  prjn->recv_idx = first_ru->recv.size - 1;
  SendCons* send_gp = first_su->send.NewPrjn(prjn);
  prjn->send_idx = first_su->send.size - 1;
  // set reciprocal indicies
  recv_gp->send_idx = prjn->send_idx;
  send_gp->recv_idx = prjn->recv_idx;

  // then crank out for remainder of units..
  Unit* u;
  taLeafItr i;
  FOR_ITR_EL(Unit, u, prjn->layer->units., i) {
    if(u == first_ru)	continue; // skip over first one..
    recv_gp = u->recv.NewPrjn(prjn);
    recv_gp->send_idx = prjn->send_idx;
  }
  FOR_ITR_EL(Unit, u, prjn->from->units., i) {
    if(u == first_su)	continue; // skip over first one..
    send_gp = u->send.NewPrjn(prjn);
    send_gp->recv_idx = prjn->recv_idx;
  }
}

bool ProjectionSpec::CheckConnect(Projection* prjn, bool quiet) {
  bool rval;
  if(CheckError(!prjn->projected, quiet, rval, "not connected!")) {
    return false;
  }
  if(CheckError(!prjn->con_spec.spec, quiet, rval, "has null con_spec")) {
    return false;
  }
  if(CheckError(!prjn->con_spec->CheckObjectType(prjn), quiet, rval,
		"does not have correct spec/object type")) {
    return false;
  }
  return true;
}

////////////////////////
//	Projection    //
////////////////////////

void Projection::Initialize() {
  layer = NULL;
  from_type = INIT; //was: PREV;
  con_type = &TA_Connection;
  recvcons_type = &TA_RecvCons;
  sendcons_type = &TA_SendCons;
  recv_idx = -1;
  send_idx = -1;
  recv_n = 1;
  send_n = 1;
  projected = false;
  direction = DIR_UNKNOWN;
  m_prv_con_spec = NULL;
}

void Projection::Destroy(){
  CutLinks();
}

void Projection::CutLinks() {
  if(owner == NULL) return;
  if((bool)from) {
    // remove from sending links, being sure to protect against a spurious re-delete
    taBase::Ref(this);
    from->send_prjns.RemoveEl(this);
    taBase::unRef(this);
  }
  RemoveCons();		// remove actual connections
  //  taBase::DelPointer((taBase**)&from);
  from = NULL;
  spec.CutLinks();
  con_spec.CutLinks();
  m_prv_con_spec = NULL;
  if(((bool)layer) && taMisc::gui_active) {
    owner = NULL;		// tell view that we're not really here
    if(layer->own_net) {
      layer->own_net->RemoveCons(); // get rid of connections in any other layers!
    }
  }
  layer = NULL;
  inherited::CutLinks();
}

void Projection::InitLinks() {
  inherited::InitLinks();
  Network* mynet = GET_MY_OWNER(Network);
  if(mynet) {
    mynet->SetProjectionDefaultTypes(this);
  }

  layer = GET_MY_OWNER(Layer);
  if(mynet) {
    int myindex = mynet->layers.FindLeafEl(layer);
    if(!(myindex == 0) && (from_type == PREV)) { // is it not the first?
      SetFrom();
      if((bool)from)
	name = "Fm_" + from->name;
    }
  }
  spec.SetDefaultSpec(this);
  con_spec.SetDefaultSpec(this);
}

void Projection::Copy_(const Projection& cp) {
  from_type = cp.from_type;
  from = cp.from;
  spec = cp.spec;
  con_type = cp.con_type;
  recvcons_type = cp.recvcons_type;
  sendcons_type = cp.sendcons_type;
  con_spec = cp.con_spec;
  // note: these are not copied; fixed after network copy in FixPrjnIndexes
//   recv_idx = cp.recv_idx;
//   send_idx = cp.send_idx;
//   recv_n = cp.recv_n;
//   send_n = cp.send_n;
//   projected = cp.projected;
  direction = cp.direction;

  m_prv_con_spec = cp.m_prv_con_spec;

  // this will update all pointers under us to new network if we are copied from other guy
  // only if the network is not otherwise already copying too!!
  UpdatePointers_NewPar_IfParNotCp(&cp, &TA_Network);
}

void Projection::UpdateAfterMove_impl(taBase* old_owner) {
  inherited::UpdateAfterMove_impl(old_owner);

  if(!old_owner) return;
  Network* mynet = GET_MY_OWNER(Network);
  Network* otnet = (Network*)old_owner->GetOwner(&TA_Network);
  if(!mynet || !otnet || mynet == otnet) return;  // don't update if not relevant
  UpdatePointers_NewPar(otnet, mynet);
}

void Projection::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(!(taMisc::is_loading || (from_type == INIT)))
    SetFrom();

  if((bool)from) {
    Network* mynet = GET_MY_OWNER(Network);
    Network* fmnet = GET_OWNER(from, Network);
    if(fmnet != mynet) {
      Layer* ly = mynet->FindLayer(from->name);
      if(ly) {
	from_type = CUSTOM;	// most likely already is..
	from = ly;
      }
      else {
	TestWarning(true, "UAE", "'from' layer is not within this Network, and layer of same name was not found, so setting from = PREV -- most likely you need to fix this!");
	from_type = PREV;
	SetFrom();
      }
    }
    name = "Fm_" + from->name;
  }

  UpdateConSpecs((bool)taMisc::is_loading);
//   if(taMisc::is_loading) return;
//   if(!taMisc::gui_active) return;
//   Network* net = GET_MY_OWNER(Network);
//   if(!net) return;
}

bool Projection::ChangeMyType(TypeDef* new_typ) {
  if(TestError(layer && layer->units.leaves > 0, "ChangeMyType", "You must first remove all units in the network before changing the Projection type -- otherwise it takes FOREVER -- do Network/Structure/Remove Units"))
    return false;
  return inherited::ChangeMyType(new_typ);
}

void Projection::WeightsToTable(DataTable* dt, const String& col_nm_) {
  if(!(bool)from) return;
  if (!dt) {
    taProject* proj = GET_MY_OWNER(taProject);
    dt = proj->GetNewAnalysisDataTable(name + "_Weights", true);
  }
  dt->StructUpdate(true);
  dt->ResetData();

  String col_nm = col_nm_;
  if(col_nm.empty()) col_nm = from->name;

  int idx;
  DataCol* scol = dt->FindMakeColName(col_nm, idx, VT_FLOAT, 2, from->act_geom.x, from->act_geom.y);

  taLeafItr ri;
  Unit* ru;
  FOR_ITR_EL(Unit, ru, layer->units., ri) {
    RecvCons* cg = ru->recv.FindFrom(from);
    if(cg == NULL)
      break;
    dt->AddBlankRow();
    int wi;
    for(wi=0;wi<cg->cons.size;wi++) {
      scol->SetValAsFloatM(cg->Cn(wi)->wt, -1, wi);
    }
  }
  dt->StructUpdate(false);
}

void Projection::VarToTable(DataTable* dt, const String& variable) {
  if (!dt) {
    taProject* proj = GET_MY_OWNER(taProject);
    dt = proj->GetNewAnalysisDataTable(name + "_Var_" + variable, true);
  }

  Network* net = GET_MY_OWNER(Network);
  if(!net) return;

  NetMonitor nm;
  taBase::Own(nm, this);
  nm.AddProjection(this, variable);
  nm.items[0]->max_name_len = 20; // allow long names
  nm.SetDataNetwork(dt, net);
  nm.UpdateDataTable();
  dt->AddBlankRow();
  nm.GetMonVals();
  dt->WriteClose();
}

void Projection::SetFrom() {
  if(!(bool)layer) {
    from = NULL;
    return;
  }
  Network* mynet = layer->own_net;
  if(mynet == NULL)
    return;
  int myindex = mynet->layers.FindLeafEl(layer);

  switch(from_type) { // this is where the projection is coming from
  case NEXT:
    if(TestWarning(myindex == (mynet->layers.leaves - 1), "SetFrom",
		   "Last Layer projects from NEXT layer")) {
      return;
    }
    else {
      Layer* nwly = (Layer*)mynet->layers.Leaf(myindex+1);
      if(from.ptr() == nwly) return;
//       taBase::SetPointer((taBase**)&from, nwly);
      from = nwly;
      DataChanged(DCR_ITEM_UPDATED);
    }
    break;
  case PREV:
    if(TestWarning(myindex == 0, "SetFrom",
		   "First Layer recieves projection from PREV layer")) {
      return;
    }
    else {
      Layer* nwly = (Layer*)mynet->layers.Leaf(myindex-1);
      if(from.ptr() == nwly) return;
//       taBase::SetPointer((taBase**)&from, nwly);
      from = nwly;
      DataChanged(DCR_ITEM_UPDATED);
    }
    break;
  case SELF:
    if(from.ptr() == layer) return;
//     taBase::SetPointer((taBase**)&from, layer);
    from = layer;
    DataChanged(DCR_ITEM_UPDATED);
    break;
  case CUSTOM:
    TestWarning(!(bool)from, "SetFrom", "CUSTOM projection and from is NULL");
    break;
  case INIT:
    break;
  }
}

void Projection::SetCustomFrom(Layer* fm_lay) {
//   taBase::SetPointer((taBase**)&from, fm_lay);
  from = fm_lay;
  if(fm_lay == layer)
    from_type = SELF;
  else
    from_type = CUSTOM;
  UpdateAfterEdit();
}

bool Projection::UpdateConSpecs(bool force) {
  if((!(bool)layer) || (!(bool)from)) return false;
  if(!force && (con_spec.SPtr() == m_prv_con_spec)) return false;
  ConSpec* sp = con_spec.SPtr();
  if(!sp) return false;
  m_prv_con_spec = sp;		// don't redo it
  Unit* u;
  taLeafItr i;
  FOR_ITR_EL(Unit, u, layer->units., i) {
    int g;
    for(g=0; g<u->recv.size; g++) {
      RecvCons* recv_gp = u->recv.FastEl(g);
      if(recv_gp->prjn == this) {
	if(sp->CheckObjectType(recv_gp))
	  recv_gp->SetConSpec(sp);
	else
	  return false;
      }
    }
  }
  // also do the from!
  FOR_ITR_EL(Unit, u, from->units., i) {
    int g;
    for(g=0; g<u->send.size; g++) {
      SendCons* send_gp = u->send.FastEl(g);
      if(send_gp->prjn == this) {
	if(sp->CheckObjectType(send_gp))
	  send_gp->SetConSpec(sp);
	else
	  return false;
      }
    }
  }
  return true;
}

bool Projection::SetPrjnSpec(ProjectionSpec* sp) {
  if(sp == NULL)	return false;
  spec.SetSpec(sp);
  return true;
}

bool Projection::SetConSpec(ConSpec* sp) {
  if(!sp)	return false;
  if(!con_spec.SetSpec(sp)) return false;
  if(!con_spec.CheckSpec(con_type, true)) { // quiet
    if(taMisc::Choice("The con spec you are setting is not compatible with the con type for connections in this projection -- should I change the con type to be: " +  con_spec->min_obj_type->name
		      + " (if you answer No, you will continue to get errors until a compatible selection is made)",
		      "Yes", "No") == 0) {
      con_type = con_spec->min_obj_type;
    }
  }
  return UpdateConSpecs();
}

bool Projection::SetConType(TypeDef* td) {
  if(con_type == td) return false;
  projected = false;
  con_type = td;
  return true;
}

bool Projection::SetRecvConsType(TypeDef* td) {
  if(recvcons_type == td) return false;
  projected = false;
  recvcons_type = td;
  return true;
}

bool Projection::SetSendConsType(TypeDef* td) {
  if(sendcons_type == td) return false;
  projected = false;
  sendcons_type = td;
  return true;
}

void Projection::MonitorVar(NetMonitor* net_mon, const String& variable) {
  if(!net_mon) return;
  net_mon->AddObject(this, variable);
}

void Projection::FixPrjnIndexes() {
  projected = false;
  if((!(bool)layer) || (!(bool)from)) return;
  if((layer->units.leaves == 0) || (from->units.leaves == 0)) return;
  Unit* ru = layer->units.Leaf(0);
  Unit* su = from->units.Leaf(0);
  recv_idx = ru->recv.FindPrjnIdx(this);
  send_idx = su->send.FindPrjnIdx(this);
  Unit* u;
  taLeafItr i;
  FOR_ITR_EL(Unit, u, layer->units., i) {
    for(int g=0; g<u->recv.size; g++) {
      RecvCons* recv_gp = u->recv.FastEl(g);
      if(recv_gp->prjn != this) continue;
      recv_gp->send_idx = send_idx;
    }
  }
  FOR_ITR_EL(Unit, u, from->units., i) {
    int g;
    for(g=0; g<u->send.size; g++) {
      SendCons* send_gp = u->send.FastEl(g);
      if(send_gp->prjn != this) continue;
      send_gp->recv_idx = recv_idx;
    }
  }
  projected = true;
}

int Projection::ReplaceConSpec(ConSpec* old_sp, ConSpec* new_sp) {
  if(con_spec.SPtr() != old_sp) return 0;
  con_spec.SetSpec(new_sp);
  UpdateConSpecs();
  return 1;
}

int Projection::ReplacePrjnSpec(ProjectionSpec* old_sp, ProjectionSpec* new_sp) {
  if(spec.SPtr() != old_sp) return 0;
  spec.SetSpec(new_sp);
  return 1;
}

void Projection::CheckThisConfig_impl(bool quiet, bool& rval) { 
  inherited::CheckThisConfig_impl(quiet, rval);
  
  if(!spec.CheckSpec(GetTypeDef())) {
    rval = false;
  }
  if(!con_spec.CheckSpec(con_type)) {
    rval = false;
  }
  if(CheckError((recvcons_type == &TA_RecvCons), quiet, rval,
		"recvcons_type is base type; should be special one for specific algorithm")) {
    projected = false;
  }
  if(CheckError((sendcons_type == &TA_SendCons), quiet, rval,
		"sendcons_type is base type; should be special one for specific algorithm")) {
    projected = false;
  }
}

void Projection::Copy_Weights(const Projection* src) {
  Unit* u, *su;
  taLeafItr i,si;
  for(u = (Unit*)layer->units.FirstEl(i), su = (Unit*)src->layer->units.FirstEl(si);
      (u) && (su);
      u = (Unit*)layer->units.NextEl(i), su = (Unit*)src->layer->units.NextEl(si))
  {
    u->Copy_Weights(su, this);
  }
}

void Projection::SaveWeights_strm(ostream& strm, RecvCons::WtSaveFormat fmt) {
  Unit* u;
  taLeafItr i;
  FOR_ITR_EL(Unit, u, layer->units., i)
    u->SaveWeights_strm(strm, this, fmt);
}

int Projection::LoadWeights_strm(istream& strm, RecvCons::WtSaveFormat fmt, bool quiet) {
  int rval = 0;
  Unit* u;
  taLeafItr i;
  FOR_ITR_EL(Unit, u, layer->units., i) {
    rval = u->LoadWeights_strm(strm, this, fmt, quiet);
    if(rval != taMisc::TAG_END) break;
  }
  return rval;
}

void Projection::SaveWeights(const String& fname, RecvCons::WtSaveFormat fmt) {
  taFiler* flr = GetSaveFiler(fname, ".wts", true);
  if(flr->ostrm)
    SaveWeights_strm(*flr->ostrm, fmt);
  flr->Close();
  taRefN::unRefDone(flr);
}

int Projection::LoadWeights(const String& fname, RecvCons::WtSaveFormat fmt, bool quiet) {
  taFiler* flr = GetLoadFiler(fname, ".wts", true);
  int rval = false;
  if(flr->istrm)
    rval = LoadWeights_strm(*flr->istrm, fmt, quiet);
  flr->Close();
  taRefN::unRefDone(flr);
  return rval;
}

void Projection::TransformWeights(const SimpleMathSpec& trans) {
  Unit* u;
  taLeafItr i;
  FOR_ITR_EL(Unit, u, layer->units., i)
    u->TransformWeights(trans, this);
}

void Projection::AddNoiseToWeights(const Random& noise_spec) {
  Unit* u;
  taLeafItr i;
  FOR_ITR_EL(Unit, u, layer->units., i)
    u->AddNoiseToWeights(noise_spec, this);
}

int Projection::PruneCons(const SimpleMathSpec& pre_proc,
			      Relation::Relations rel, float cmp_val)
{
  int rval = 0;
  Unit* u;
  taLeafItr i;
  FOR_ITR_EL(Unit, u, layer->units., i)
    rval += u->PruneCons(pre_proc, rel, cmp_val, this);
  return rval;
}

int Projection::LesionCons(float p_lesion, bool permute) {
  int rval = 0;
  Unit* u;
  taLeafItr i;
  FOR_ITR_EL(Unit, u, layer->units., i)
    rval += u->LesionCons(p_lesion, permute, this);
  return rval;
}


////////////////////////
//	Unit_Group    //
////////////////////////

void Unit_Group::Initialize() {
  own_lay = NULL;
  unique_geom = false;
  units_lesioned = false;
  n_units = 0;	// note: v3compat obs
}

void Unit_Group::InitLinks() {
  taGroup<Unit>::InitLinks();
  taBase::Own(pos,this);
  taBase::Own(geom,this);
  own_lay = GET_MY_OWNER(Layer);
}

void Unit_Group::CutLinks() {
  own_lay = NULL;
  taGroup<Unit>::CutLinks();
}

void Unit_Group::Copy_(const Unit_Group& cp) {
  pos = cp.pos;
  unique_geom = false;
  geom = cp.geom;
  output_name = cp.output_name;
  n_units = cp.n_units;	// note: v3compat obs
}

void Unit_Group::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if((own_lay == NULL) || (own_lay->own_net == NULL)) return;
  if(n_units != 0) {		// v3compat obs: conversion; remove at some point
    unique_geom = true;
    if(n_units != geom.x * geom.y) {
      geom.n_not_xy = true;
      geom.n = n_units;
    }
    n_units = 0;
  }
  RecomputeGeometry();
}

void Unit_Group::RemoveAll() {
  units_lesioned = false;
  inherited::RemoveAll();
}

void Unit_Group::RecomputeGeometry() {
  if((own_lay == NULL) || (own_lay->own_net == NULL)) return;
  if(!unique_geom)
    geom = own_lay->un_geom;
  else 
    geom.UpdateAfterEdit();
}

void Unit_Group::LayoutUnits(Unit* u) {
  if(owner == own_lay)
    return;			// i'm the layer subgroup, layer takes care
  RecomputeGeometry();
  int i = 0;
  TDCoord mygeo;
  Unit* un = NULL;
  for(mygeo.y=0; mygeo.y < geom.y; mygeo.y++) {
    for(mygeo.x=0; mygeo.x <geom.x; mygeo.x++) {
      if(i >= size)
        break;
      un = (Unit*)FastEl(i++);
      if((un) && (!u || un==u)) un->pos = mygeo;
      if(un==u) break;
    }
    if(un==u) break;
  }
}

bool Unit_Group::BuildUnits() {
  if((own_lay == NULL) || (owner == own_lay))	return false;
  StructUpdate(true);
  RecomputeGeometry();
  bool units_changed = false;
  if(size != geom.n)
    units_changed = true;
  SetSize(geom.n);
  EnforceType();
  Unit* u;
  taLeafItr i;
  FOR_ITR_EL(Unit, u, this->, i)
    u->BuildUnits();
  units_lesioned = false;
  StructUpdate(false);
  return units_changed;
}

bool Unit_Group::CheckBuild(bool quiet) {
  bool rval;
  if(CheckError((!units_lesioned && (size != geom.n)), quiet, rval,
		"number of units != target in group")) {
    return false;
  }
  return true;
}

void Unit_Group::MonitorVar(NetMonitor* net_mon, const String& variable) {
  if(!net_mon) return;
  net_mon->AddObject(this, variable);
}

void Unit_Group::Copy_Weights(const Unit_Group* src) {
  Unit* u, *su;
  taLeafItr i,si;
  for(u = (Unit*)FirstEl(i), su = (Unit*)src->FirstEl(si);
      (u) && (su);
      u = (Unit*)NextEl(i), su = (Unit*)src->NextEl(si))
  {
    u->Copy_Weights(su);
  }
}

void Unit_Group::SaveWeights_strm(ostream& strm, RecvCons::WtSaveFormat fmt) {
  strm << "<Ug>\n";
  Unit* u;
  taLeafItr i;
  FOR_ITR_EL(Unit, u, this->, i) {
    int lfi = u->GetMyLeafIndex();
    strm << "<UgUn " << lfi << " " << u->name << ">\n";
    u->SaveWeights_strm(strm, NULL, fmt);
    strm << "</UgUn>\n";
  }
  strm << "</Ug>\n";
}

int Unit_Group::LoadWeights_strm(istream& strm, RecvCons::WtSaveFormat fmt, bool quiet) {
  String tag, val;
  int stat = RecvCons::LoadWeights_StartTag(strm, "Ug", val, quiet);
  if(stat != taMisc::TAG_GOT) return stat;

  while(true) {
    stat = taMisc::read_tag(strm, tag, val);
    if(stat != taMisc::TAG_GOT) break;		// *should* break at TAG_END
    if(tag != "UgUn") { stat = taMisc::TAG_NONE;  break; } // bumping up against some other tag
    int lfi = (int)val.before(' ');
    if(leaves > lfi) {
      Unit* u = Leaf(lfi);
      stat = u->LoadWeights_strm(strm, NULL, fmt, quiet);
    }
    else {
      stat = Unit::SkipWeights_strm(strm, fmt, quiet);
    }
    if(stat != taMisc::TAG_END) break;
    stat = taMisc::TAG_NONE;	       // reset so EndTag will definitely read new tag
    RecvCons::LoadWeights_EndTag(strm, "UgUn", tag, stat, quiet);
    if(stat != taMisc::TAG_END) break;
  }
  RecvCons::LoadWeights_EndTag(strm, "Ug", tag, stat, quiet);
  return stat;
}

int Unit_Group::SkipWeights_strm(istream& strm, RecvCons::WtSaveFormat fmt, bool quiet) {
  String val, tag;
  int stat = RecvCons::LoadWeights_StartTag(strm, "Ug", val, quiet);
  if(stat != taMisc::TAG_GOT) return stat;

  while(true) {
    stat = taMisc::read_tag(strm, tag, val);
    if(stat != taMisc::TAG_GOT) break;		// *should* break at TAG_END
    if(tag != "UgUn") { stat = taMisc::TAG_NONE;  break; } // bumping up against some other tag
    stat = Unit::SkipWeights_strm(strm, fmt, quiet);
    if(stat != taMisc::TAG_END) break;
    stat = taMisc::TAG_NONE;	       // reset so EndTag will definitely read new tag
    RecvCons::LoadWeights_EndTag(strm, "UgUn", tag, stat, quiet);
    if(stat != taMisc::TAG_END) break;
  }
  RecvCons::LoadWeights_EndTag(strm, "Ug", tag, stat, quiet);
  return stat;
}

void Unit_Group::SaveWeights(const String& fname, RecvCons::WtSaveFormat fmt) {
  taFiler* flr = GetSaveFiler(fname, ".wts", true);
  if(flr->ostrm)
    SaveWeights_strm(*flr->ostrm, fmt);
  flr->Close();
  taRefN::unRefDone(flr);
}

int Unit_Group::LoadWeights(const String& fname, RecvCons::WtSaveFormat fmt, bool quiet) {
  taFiler* flr = GetLoadFiler(fname, ".wts", true);
  int rval = false;
  if(flr->istrm)
    rval = LoadWeights_strm(*flr->istrm, fmt, quiet);
  flr->Close();
  taRefN::unRefDone(flr);
  return rval;
}

void Unit_Group::TransformWeights(const SimpleMathSpec& trans) {
  Unit* u;
  taLeafItr i;
  FOR_ITR_EL(Unit, u, this->, i)
    u->TransformWeights(trans);
}

void Unit_Group::AddNoiseToWeights(const Random& noise_spec) {
  Unit* u;
  taLeafItr i;
  FOR_ITR_EL(Unit, u, this->, i)
    u->AddNoiseToWeights(noise_spec);
}

int Unit_Group::PruneCons(const SimpleMathSpec& pre_proc,
			Relation::Relations rel, float cmp_val)
{
  int rval = 0;
  Unit* u;
  taLeafItr i;
  FOR_ITR_EL(Unit, u, this->, i)
    rval += u->PruneCons(pre_proc, rel, cmp_val);
  return rval;
}

int Unit_Group::LesionCons(float p_lesion, bool permute) {
  int rval = 0;
  Unit* u;
  taLeafItr i;
  FOR_ITR_EL(Unit, u, this->, i)
    rval += u->LesionCons(p_lesion, permute);
  return rval;
}

int Unit_Group::LesionUnits(float p_lesion, bool permute) {
  int rval = 0;
  StructUpdate(true);
  if(permute) {
    rval = (int) (p_lesion * (float)leaves);
    if(rval == 0) return 0;
    int_Array ary;
    int j;
    for(j=0; j<leaves; j++)
      ary.Add(j);
    ary.Permute();
    ary.size = rval;
    ary.Sort();
    for(j=ary.size-1; j>=0; j--) {
      Unit* un = Leaf(ary.FastEl(j));
      un->DisConnectAll();
//       un->pos.z = -1;		// don't update yet!
      RemoveLeafEl(un);
    }
  }
  else {
    int j;
    for(j=leaves-1; j>=0; j--) {
      if(Random::ZeroOne() <= p_lesion) {
	Unit* un = (Unit*)Leaf(j);
	un->DisConnectAll();
// 	un->pos.z = -1;		// don't update yet!
	RemoveLeafIdx(j);
	rval++;
      }
    }
  }
  units_lesioned = true;	// record that units were lesioned
  int k;
  for(k=0; k<gp.size; k++) {
    Unit_Group* ug = (Unit_Group*)gp.FastEl(k);
    ug->units_lesioned = true;
  }
  StructUpdate(false);
  if (!Network_InStructUpdate(own_lay->own_net)) {
    own_lay->own_net->UpdtAfterNetMod();
  }
  return rval;
}

bool Unit_Group::UnitValuesToArray(float_Array& ary, const char* variable) {
  MemberDef* md = el_typ->members.FindName(variable);
  if(TestWarning(!md || !md->type->InheritsFrom(TA_float), "UnitValuesToArray",
		 "Variable:", variable, "not found or not a float on units of type:",
		 el_typ->name)) {
    return false;
  }
  Unit* u;
  taLeafItr i;
  FOR_ITR_EL(Unit, u, this->, i) {
    float* val = (float*)md->GetOff((void*)u);
    ary.Add(*val);
  }
  return true;
}

bool Unit_Group::UnitValuesToMatrix(float_Matrix& mat, const char* variable) {
  MemberDef* md = el_typ->members.FindName(variable);
  if(TestWarning(!md || !md->type->InheritsFrom(TA_float), "UnitValuesToMatrix",
		 "Variable:", variable, "not found or not a float on units of type:",
		 el_typ->name)) {
    return false;
  }
  if(mat.size < leaves) return false;
  int cnt=0;
  Unit* u;
  taLeafItr i;
  FOR_ITR_EL(Unit, u, this->, i) {
    float* val = (float*)md->GetOff((void*)u);
    mat.FastEl_Flat(cnt++) = *val;
  }
  return true;
}

bool Unit_Group::UnitValuesFromArray(float_Array& ary, const char* variable) {
  if(ary.size == 0) return false;
  MemberDef* md = el_typ->members.FindName(variable);
  if(TestWarning(!md || !md->type->InheritsFrom(TA_float), "UnitValuesFromArray",
		 "Variable:", variable, "not found or not a float on units of type:",
		 el_typ->name)) {
    return false;
  }
  int cnt=0;
  Unit* u;
  taLeafItr i;
  FOR_ITR_EL(Unit, u, this->, i) {
    float* val = (float*)md->GetOff((void*)u);
    *val = ary[cnt++];
    if(cnt >= ary.size)
      break;
  }
  return true;
}

bool Unit_Group::UnitValuesFromMatrix(float_Matrix& mat, const char* variable) {
  if(mat.size == 0) return false;
  MemberDef* md = el_typ->members.FindName(variable);
  if(TestWarning(!md || !md->type->InheritsFrom(TA_float), "UnitValuesFromMatrix",
		 "Variable:", variable, "not found or not a float on units of type:",
		 el_typ->name)) {
    return false;
  }
  int cnt=0;
  Unit* u;
  taLeafItr i;
  FOR_ITR_EL(Unit, u, this->, i) {
    float* val = (float*)md->GetOff((void*)u);
    *val = mat.FastEl_Flat(cnt++);
    if(cnt >= mat.size)
      break;
  }
  return true;
}

void Unit_Group::VarToTable(DataTable* dt, const String& variable) {
  if (!dt) {
    taProject* proj = GET_MY_OWNER(taProject);
    dt = proj->GetNewAnalysisDataTable(name + "_Var_" + variable, true);
  }

  Network* net = GET_MY_OWNER(Network);
  if(!net) return;

  NetMonitor nm;
  taBase::Own(nm, this);
  nm.AddUnitGroup(this, variable);
  nm.items[0]->max_name_len = 20; // allow long names
  nm.SetDataNetwork(dt, net);
  nm.UpdateDataTable();
  dt->AddBlankRow();
  nm.GetMonVals();
  dt->WriteClose();
}

bool Unit_Group::SetUnitNames(taMatrix* mat) {
  if (!mat) return false;
  int x, y;
  for (y = 0; ((y < mat->dim(1)) && (y < geom.y)); ++y) {
    for (x = 0; ((x < mat->dim(0)) && (x < geom.x)); ++x) {
      Unit* un = FindUnitFmCoord(x, y);
      if (!un) continue;
      un->SetName(mat->SafeElAsVar(x, y).toString());
    }
  }
  return true;
}

bool Unit_Group::GetUnitNames(taMatrix* mat) {
  if (!mat) return false;
  int x, y;
  for (y = 0; ((y < mat->dim(1)) && (y < geom.y)); ++y) {
    for (x = 0; ((x < mat->dim(0)) && (x < geom.x)); ++x) {
      Unit* un = FindUnitFmCoord(x, y);
      if (!un) continue;
      mat->SetFmVar((Variant)un->GetName(), x, y);
    }
  }
  return true;
}

Unit* Unit_Group::FindUnitFmCoord(int x, int y) {
  if (( x < 0) || (x >= geom.x) || (y < 0) || (y >= geom.y)) return NULL;
  int idx = y * geom.x + x;
  if (idx < size)
    return FastEl(idx);
  return NULL;
}

TwoDCoord Unit_Group::GetGpGeomPos() {
  if(!own_lay) return pos;
  TwoDCoord rval;
  if(own_lay->un_geom.x + own_lay->gp_spc.x > 0)
    rval.x = pos.x / (own_lay->un_geom.x + own_lay->gp_spc.x);
  if(own_lay->un_geom.y + own_lay->gp_spc.y > 0)
    rval.y = pos.y / (own_lay->un_geom.y + own_lay->gp_spc.y);
  return rval;
}

void Unit_Group::AddRelPos(TDCoord& rel_pos) {
  // note: vastly most likely case is a flat root group of units...
  Layer* lay = dynamic_cast<Layer*>(owner);
  if (lay) {
    rel_pos += lay->pos;
    lay->AddRelPos(rel_pos);
  } else { // better be in a group then!
    Unit_Group* ugp = GET_MY_OWNER(Unit_Group);
    if (ugp) {
      rel_pos += ugp->pos;
      ugp->AddRelPos(rel_pos);
    }
  }
}
 
bool Unit_Group::Dump_QuerySaveChildren() {
  if (!own_lay) return false; // huh? should always be valid...
  // always save if forced
  if (own_lay->own_net->HasNetFlag(Network::SAVE_UNITS_FORCE)) return true;
  // else arbitrate: true if layer says SAVE, or net says SAVE and we don't override
  return (own_lay->HasLayerFlag(Layer::SAVE_UNITS) ||
    (own_lay->own_net->HasNetFlag(Network::SAVE_UNITS)
     && !own_lay->HasLayerFlag(Layer::NO_SAVE_UNITS)));
}

////////////////////////
//  Projection_Group  //
////////////////////////

void Projection_Group::DataChanged(int dcr, void* op1, void* op2) {
  inherited::DataChanged(dcr, op1, op2);
  if(send_prjns) return;
  if (dcr == DCR_LIST_ITEM_INSERT) {
    Network* net = GET_MY_OWNER(Network);
    if (net) 
      net->RebuildAllViews();
  }
}

////////////////////////
//	Layer	      //
////////////////////////

void LayerSpec::Initialize() {
}

void LayerSpec::InitLinks() {
  BaseSpec::InitLinks();
  children.SetBaseType(&TA_LayerSpec); // allow all of this general spec type to be created under here
  children.el_typ = GetTypeDef(); // but make the default to be me!
}

void LayerSpec::CutLinks() {
  BaseSpec::CutLinks();
}

void Layer::Initialize() {
  own_net = NULL;
  lesion_ = false;
  flags = LF_NONE;
  layer_type = HIDDEN;
  unit_groups = false;
  gp_spc.x = 1;  gp_spc.y = 1;
  projections.SetBaseType(&TA_Projection);
  send_prjns.send_prjns = true;
  units.SetBaseType(&TA_Unit);
  ext_flag = Unit::NO_EXTERNAL;
  act_geom = un_geom;
  dmem_dist = DMEM_DIST_DEFAULT;
  m_prv_unit_spec = NULL;

  sse = 0.0f;
  icon_value = 0.0f;
  units_flat_idx = 0;

  n_units = 0;			// note: v3compat obs
}

void Layer::InitLinks() {
  inherited::InitLinks();
  taBase::Own(unit_spec, this);
  taBase::Own(units, this);
  taBase::Own(projections, this);
  taBase::Own(send_prjns, this);
  taBase::Own(pos, this);
  taBase::Own(un_geom, this);
  taBase::Own(gp_geom, this);
  taBase::Own(gp_spc, this);
  taBase::Own(flat_geom, this);
  taBase::Own(act_geom, this);
#ifdef DMEM_COMPILE
  taBase::Own(dmem_share_units, this);
#endif
  taBase::Own(unit_names, this);
  own_net = GET_MY_OWNER(Network);
  if(pos == 0)
    SetDefaultPos();
  units.pos.z = 0;
  unit_spec.SetDefaultSpec(this);
}

void Layer::CutLinks() {
  if(!owner) return; // already replacing or already dead
  DisConnect();
  unit_names.CutLinks();
  act_geom.CutLinks();
  flat_geom.CutLinks();
  gp_spc.CutLinks();
  gp_geom.CutLinks();
  un_geom.CutLinks();
  pos.CutLinks();
  send_prjns.CutLinks();
  projections.CutLinks();
  units.CutLinks();
  unit_spec.CutLinks();
  m_prv_unit_spec = NULL;
  inherited::CutLinks();
}


void Layer::Copy_(const Layer& cp) {
  layer_type = cp.layer_type;
  flags = cp.flags;
  pos = cp.pos;
  un_geom = cp.un_geom;
  unit_groups = cp.unit_groups;
  gp_geom = cp.gp_geom;
  gp_spc = cp.gp_spc;
  flat_geom = cp.flat_geom;
  act_geom = cp.act_geom;
  projections = cp.projections;
  units = cp.units;
  unit_spec = cp.unit_spec;
  ext_flag = cp.ext_flag;
  m_prv_unit_spec = cp.m_prv_unit_spec;

  output_name = cp.output_name;
  sse = cp.sse;
  icon_value = cp.icon_value;

  n_units = cp.n_units;		// note: v3compat obs

  // this will update all pointers under us to new network if we are copied from other guy
  // only if the network is not otherwise already copying too!!
  UpdatePointers_NewPar_IfParNotCp(&cp, &TA_Network);

  // not copied
  //  send_prjns.BorrowUnique(cp.send_prjns); // link group
}

void Layer::UpdateAfterMove_impl(taBase* old_owner) {
  inherited::UpdateAfterMove_impl(old_owner);

  if(!old_owner) return;
  Network* mynet = GET_MY_OWNER(Network);
  Network* otnet = (Network*)old_owner->GetOwner(&TA_Network);
  if(!mynet || !otnet || mynet == otnet) return;  // don't update if not relevant
  UpdatePointers_NewPar(otnet, mynet);
}

void Layer::UpdateAfterEdit() {
  inherited::UpdateAfterEdit();

  if(taMisc::is_loading) return;
}

void Layer::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();

  if (lesion_) {		// obs: v3compat conversion obs remove later
    SetLayerFlag(LESIONED);
    lesion_ = false;
  }

  // no negative geoms., y,z must be 1 (for display)
  UpdateUnitSpecs((bool)taMisc::is_loading); // force if loading
  //  SyncSendPrjns(); // this is not a good place to do this -- too frequent and unnec
  // also causes problems during copy..
  if(n_units > 0) {		// obs: v3compat conversion obs remove later
    if(n_units != un_geom.x * un_geom.y) {
      un_geom.n_not_xy = true;
      un_geom.n = n_units;
    }
    n_units = 0;
  }
  if(un_geom.z > 1) {		// obs: v3compat conversion obs remove later
    gp_geom.UpdateAfterEdit();	// get n from xy
    unit_groups = true;
    if(gp_geom.n != un_geom.z) {
      gp_geom.n_not_xy = true;
      gp_geom.n = un_geom.z;
    }
    un_geom.z = 0;
  }
  RecomputeGeometry();
  if(own_net)
    own_net->LayerPos_Cleanup();
}

void Layer::Iconify() {
  StructUpdate(true);
  SetLayerFlag(ICONIFIED);
  StructUpdate(false);
}

void Layer::DeIconify()	{
  StructUpdate(true);
  ClearLayerFlag(ICONIFIED);
  StructUpdate(false);
}

void Layer::ConnectFrom(Layer* from_lay) {
  Network* net = GET_MY_OWNER(Network);
  if (!net) return;
  Projection* prjn = net->FindMakePrjn(this, from_lay);
  if(prjn) {
    prjn->UpdateAfterEdit();
    if(taMisc::gui_active)
      tabMisc::DelayedFunCall_gui(prjn, "BrowserSelectMe");
  }
}

void Layer::ConnectBidir(Layer* from_lay) {
  Network* net = GET_MY_OWNER(Network);
  if (!net) return;
  Projection* prjn = net->FindMakePrjn(this, from_lay);
  if(prjn) {
    prjn->UpdateAfterEdit();
    if(taMisc::gui_active)
      tabMisc::DelayedFunCall_gui(prjn, "BrowserSelectMe");
  }
  prjn = net->FindMakePrjn(from_lay, this);
  if(prjn) {
    prjn->UpdateAfterEdit();
    if(taMisc::gui_active)
      tabMisc::DelayedFunCall_gui(prjn, "BrowserSelectMe");
  }
}

void Layer::ConnectSelf() {
  Network* net = GET_MY_OWNER(Network);
  if (!net) return;
  Projection* prjn = net->FindMakeSelfPrjn(this);
  if(prjn) {
    prjn->UpdateAfterEdit();
    if(taMisc::gui_active)
      tabMisc::DelayedFunCall_gui(prjn, "BrowserSelectMe");
  }
}

taBase::DumpQueryResult Layer::Dump_QuerySaveMember(MemberDef* md) {
  // only save unit_names if any (to ease backwards compat)
  if (md->name != "unit_names") 
    return inherited::Dump_QuerySaveMember(md);
  return (unit_names.dims()) ? DQR_SAVE : DQR_NO_SAVE;
}

void Layer::SyncSendPrjns() {
  Projection* p;
  taLeafItr i;
  FOR_ITR_EL(Projection, p, projections., i) {
    Layer* snd = p->from;
    if(snd == NULL) continue;
    snd->send_prjns.LinkUnique(p); // make sure senders are all represented
  }
  // now make sure that we don't have any spurious ones
  int pi;
  for(pi=send_prjns.size-1; pi>=0; pi--) {
    p = (Projection*)send_prjns.FastEl(pi);
    if(p == NULL) continue;
    if((!(bool)p->layer) || (p->from.ptr() != this))
      send_prjns.RemoveIdx(pi);	// get rid of it!
  }
}

void Layer::RecomputeGeometry() {
  un_geom.UpdateAfterEdit();
  gp_geom.UpdateAfterEdit();
  if(unit_groups) {
    flat_geom.x = un_geom.x * gp_geom.x;
    flat_geom.y = un_geom.y * gp_geom.y;
    flat_geom.n = un_geom.n * gp_geom.n;
    if(flat_geom.n != flat_geom.x * flat_geom.y)
      flat_geom.n_not_xy = true;
    // NOTE: act_geom must get computed later....
  } else {
    flat_geom = un_geom;
    act_geom = un_geom;
  }
}

ProjectBase* Layer::project() {
  ProjectBase* rval = NULL;
  if (own_net)
    rval = GET_OWNER(own_net, ProjectBase);
  return rval;
}

void Layer::SetLayerUnitGeom(int x, int y, bool n_not_xy, int n) {
  un_geom.x = x; un_geom.y = y; un_geom.n_not_xy = n_not_xy; un_geom.n = n;
  UpdateAfterEdit();
}
void Layer::SetLayerUnitGpGeom(int x, int y, bool n_not_xy, int n) {
  unit_groups = true;
  gp_geom.x = x; gp_geom.y = y; gp_geom.n_not_xy = n_not_xy; gp_geom.n = n;
  UpdateAfterEdit();
}

bool Layer::SetLayerSpec(LayerSpec*) {
  return false;			// no layer spec for layers!
}

void Layer::SetDefaultPos() {
  if(!own_net) return;
  int index = own_net->layers.FindLeafEl(this);
  pos = 0;
  switch(own_net->lay_layout) {
  case Network::THREE_D: {
    for(int i=0;i<index;i++) {
      Layer* lay = (Layer*)own_net->layers.Leaf(i);
      pos.z = MAX(pos.z, lay->pos.z + 1);
    }
    break;
  }
  case Network::TWO_D: {
    for(int i=0;i<index;i++) {
      Layer* lay = (Layer*)own_net->layers.Leaf(i);
      pos.y = MAX(pos.y, lay->pos.y + lay->un_geom.y + 2);
    }
    break;
  }
  }
}

void Layer::LayoutUnits(Unit* u) {
  StructUpdate(true);
  RecomputeGeometry();
  units.pos.z = 0;
  if(unit_groups) {
    TDCoord mygeo;
    PosTDCoord ungeo;
    act_geom.x = 0; act_geom.y = 0;
    int i = 0;
    int max_y = 0;
    for(mygeo.y=0, ungeo.y=0; mygeo.y < gp_geom.y; mygeo.y++) {
      max_y = 0;
      for(mygeo.x=0, ungeo.x=0; mygeo.x <gp_geom.x; mygeo.x++) {
	if(i >= units.gp.size)
	  break;
	Unit_Group* ug = (Unit_Group*)units.gp.FastEl(i++);
// 	if((ug->pos.x < ungeo.x) || (ug->pos.y < ungeo.y))
	// always update to new position!
	ug->pos = ungeo;
        ug->LayoutUnits();
	ungeo.x = ug->pos.x + MAX(ug->geom.x, un_geom.x) + gp_spc.x;
	max_y = MAX(ug->pos.y + ug->geom.y, max_y);
	act_geom.x = MAX(act_geom.x, ungeo.x);
      }
      ungeo.y = max_y + gp_spc.y;
      act_geom.y = MAX(act_geom.y, ungeo.y);
    }
    act_geom.y -= gp_spc.y;	// no space at the end!
    act_geom.x -= gp_spc.x;
  }
  else {
    TDCoord mygeo;
    act_geom = un_geom;
    units.geom = un_geom;		// default is to have layer's geom
    int i = 0;
    Unit* un = NULL;
    for(mygeo.y=0; mygeo.y < un_geom.y; mygeo.y++) {
      for(mygeo.x=0; mygeo.x <un_geom.x; mygeo.x++) {
	if(i >= units.leaves)
	  break;
	un = (Unit*)units.Leaf(i++);
	if((un) && (!u || un==u)) un->pos = mygeo;
	if(un==u) break;
      }
      if(un==u) break;
    }
  }
  StructUpdate(false);
}


void Layer::SetNUnits(int n_units) {
  if(un_geom.n == n_units || n_units <= 0) return; // only if diff or sensible
  un_geom.FitN(n_units);
  UpdateAfterEdit();
}

void Layer::SetNUnitGroups(int n_groups) {
  if(un_geom.n == n_groups || n_groups <= 0) return; // only if diff or sensible
  unit_groups = true;		// presumably this is the point..
  gp_geom.FitN(n_groups);
  UpdateAfterEdit();
}

void Layer::BuildUnits() {
  taMisc::Busy();
  StructUpdate(true);
  RecomputeGeometry();
  bool units_changed = false;
  if(unit_groups) {
    while(units.size > 0) {
//       ((Unit*)units.FastEl(units.size-1))->pos.z = -1; // do not update
      units.RemoveIdx(units.size-1); // get rid of any in top-level
    }
    units.gp.SetSize(gp_geom.n);
    for(int k=0; k< units.gp.size; k++) {
      Unit_Group* ug = (Unit_Group*)units.gp.FastEl(k);
      ug->UpdateAfterEdit();
      if(ug->BuildUnits())
	units_changed = true;
    }
  }
  else {
    units.gp.RemoveAll();	// in case there were any subgroups..
    if(!units_changed && (units.leaves != un_geom.n))
      units_changed = true;
    units.EnforceLeaves(un_geom.n);
    units.EnforceType();
    Unit* u;
    taLeafItr i;
    FOR_ITR_EL(Unit, u, units., i)
      u->BuildUnits();
    units.units_lesioned = false;
  }

  LayoutUnits();
  // assign the spec
  taLeafItr i;
  Unit* u;
  FOR_ITR_EL(Unit, u, units.,i) {
    u->SetUnitSpec(unit_spec.SPtr());
  }
  if(units_changed) {
    // tell all projections that they need to be connected
    Projection* pjn;
    taLeafItr j;
    FOR_ITR_EL(Projection, pjn, projections., j) {
      pjn->projected = false;
    }
    FOR_ITR_EL(Projection, pjn, send_prjns., j) {
      pjn->projected = false;
    }
  }
  SetUnitNames();
  StructUpdate(false);
  taMisc::DoneBusy();
}

void Layer::BuildUnits_Threads(Network* net) {
  units_flat_idx = net->units_flat.size;
  Unit* un;
  taLeafItr ui;
  FOR_ITR_EL(Unit, un, units., ui) {
    un->flat_idx = net->units_flat.size;
    net->units_flat.Add(un);
  }
}

void Layer::LayoutUnitGroups() {
  if(units.gp.size == 0) return;
  StructUpdate(true);
  TDCoord mygeo;
  PosTDCoord ungeo;
  act_geom.x = 0; act_geom.y = 0;
  int i = 0;
  int max_y = 0;
  for(mygeo.y=0, ungeo.y=0; mygeo.y < gp_geom.y; mygeo.y++) {
    max_y = 0;
    for(mygeo.x=0, ungeo.x=0; mygeo.x <gp_geom.x; mygeo.x++) {
      if(i >= units.gp.size)
	break;
      Unit_Group* ug = (Unit_Group*)units.gp.FastEl(i++);
      ug->pos = ungeo;
      ug->LayoutUnits();
      ungeo.x = ug->pos.x + MAX(ug->geom.x, un_geom.x) + gp_spc.x;
      max_y = MAX(ug->pos.y + ug->geom.y, max_y);
      act_geom.x = MAX(act_geom.x, ungeo.x);
    }
    ungeo.y = max_y + gp_spc.y;
    act_geom.y = MAX(act_geom.y, ungeo.y);
  }
  act_geom.y -= gp_spc.y;	// no space at the end!
  act_geom.x -= gp_spc.x;
  StructUpdate(false);
}

bool Layer::CheckBuild(bool quiet) {
  bool rval;
  if(units.gp.size > 0) {
    for(int g=0; g<units.gp.size; g++) {
      Unit_Group* ug = (Unit_Group*)units.gp.FastEl(g);
      if(!ug->CheckBuild(quiet))
	return false;
    }
  }
  else {
    if(CheckError((!units.units_lesioned && (units.size != un_geom.n)), quiet, rval,
		  "number of units != target")) {
      return false;
    }
  }

  Unit* u;
  taLeafItr ui;
  FOR_ITR_EL(Unit, u, units., ui) {
    if(CheckError((u->GetTypeDef() != units.el_typ), quiet, rval,
		  "unit type not correct -- should be:", units.el_typ->name)) {
      return false;
    }
    if(!u->CheckBuild(quiet))
      return false;
  }
  return true;
}

bool Layer::CheckConnect(bool quiet) {
  Projection* prjn;
  taLeafItr j;
  FOR_ITR_EL(Projection, prjn, projections.,j) {
    if(!prjn->CheckConnect(quiet)) return false;
  }
  return true;
}

void Layer::CheckThisConfig_impl(bool quiet, bool& rval) {
  // note: network also called our checks
  // slightly non-standard, since we bail on first error
  if(!unit_spec.CheckSpec(units.el_typ)) {
    rval = false;
  }
  if (!CheckBuild(quiet)) {rval = false; return;}
  if (!CheckConnect(quiet)) {rval = false; return;}
  inherited::CheckThisConfig_impl(quiet, rval);
  // could also checkspec for layers w/ specs
}

void Layer::CheckChildConfig_impl(bool quiet, bool& rval) {
  inherited::CheckChildConfig_impl(quiet, rval);
  // layerspec should take over this function in layers that have them!
  units.CheckConfig(quiet, rval);
  projections.CheckConfig(quiet, rval);
}

void Layer::FixPrjnIndexes() {
  Projection* p;
  taLeafItr i;
  FOR_ITR_EL(Projection, p, projections., i)
    p->FixPrjnIndexes();
}

void Layer::RemoveCons() {
  taMisc::Busy();
  Projection* p;
  taLeafItr i;
  FOR_ITR_EL(Projection, p, projections., i) {
    if(p->spec.spec)
      p->RemoveCons();
  }
  Unit* u;
  FOR_ITR_EL(Unit, u, units., i)
    u->RemoveCons();
  taMisc::DoneBusy();
}

void Layer::RemoveCons_Net() {
  taMisc::Busy();
  Unit* u;
  taLeafItr i;
  FOR_ITR_EL(Unit, u, units., i)
    u->RemoveCons();
  taMisc::DoneBusy();
}

void Layer::RemoveUnits() {
  taMisc::Busy();
  StructUpdate(true);
  if(units.gp.size == 0) {
    units.RemoveAll();
  }
  else {
    int g;
    for(g=0; g<units.gp.size; g++) {
      Unit_Group* ug = (Unit_Group*)units.gp.FastEl(g);
      ug->RemoveAll();
    }
  }
  StructUpdate(false);
  taMisc::DoneBusy();
}

void Layer::RemoveUnitGroups() {
  taMisc::Busy();
  StructUpdate(true);
  units.RemoveAll();
  StructUpdate(false);
  taMisc::DoneBusy();
}

void Layer::PreConnect() {
  Projection* p;
  taLeafItr i;
  FOR_ITR_EL(Projection, p, projections., i)
    p->PreConnect();
}

void Layer::Connect() {
  taMisc::Busy();
  StructUpdate(true);
  Projection* p;
  taLeafItr i;
  FOR_ITR_EL(Projection, p, projections., i)
    p->Connect();
  Unit* u;
  FOR_ITR_EL(Unit, u, units., i)
    u->BuildUnits();			// this is for the bias connections!
  StructUpdate(false);
  taMisc::DoneBusy();
}

void Layer::DisConnect() {
  StructUpdate(true);
  int pi;
  for(pi=send_prjns.size-1; pi>=0; pi--) {
    Projection* p = (Projection*)send_prjns.FastEl(pi);
    if(p == NULL) continue;
    if(!(bool)p->layer) {
      send_prjns.RemoveIdx(pi);
      continue;
    }
    p->layer->projections.RemoveLeafEl(p);
  }
  send_prjns.Reset();
  projections.Reset();
  StructUpdate(false);
}

int Layer::CountRecvCons() {
  int n_cons = 0;
  Unit* u;
  taLeafItr i;
  FOR_ITR_EL(Unit, u, units., i) {
    n_cons += u->CountRecvCons();
  }
  return n_cons;
}

void Layer::LinkSendCons() {
  Unit* u;
  taLeafItr i;
  FOR_ITR_EL(Unit, u, units., i)
    u->LinkSendCons();
}

void Layer::SetLayUnitExtFlags(int flg) {
  SetExtFlag(flg);
  Unit* u;
  taLeafItr i;
  FOR_ITR_EL(Unit, u, units., i)
    u->SetExtFlag((Unit::ExtType)flg);
}

void Layer::ApplyInputData(taMatrix* data, Unit::ExtType ext_flags,
	   Random* ran, const PosTwoDCoord* offset, bool na_by_range) 
{
  // note: when use LayerWriters, we typically always just get a single frame of 
  // the exact dimensions, and so ignore 'frame'
  if (!data || lesioned()) return;
  // check correct geom of data
  if(TestError((data->dims() != 2) && (data->dims() != 4), "ApplyInputData",
	       "data->dims must be 2 (2-d) or 4 (4-d)")) {
    return;
  }
  TwoDCoord offs(0,0);
  if(offset) offs = *offset;
  
  // apply flags if we are the controller (zero offset)
  if((offs.x == 0) && (offs.y == 0)) {
    ApplyLayerFlags(ext_flags);
  }
  if(data->dims() == 2) {
    ApplyInputData_2d(data, ext_flags, ran, offs, na_by_range);
  }
  else {
    if(unit_groups)
      ApplyInputData_Gp4d(data, ext_flags, ran, na_by_range); // note: no offsets -- layerwriter does check
    else
      ApplyInputData_Flat4d(data, ext_flags, ran, offs, na_by_range);
  }
}

void Layer::ApplyInputData_2d(taMatrix* data, Unit::ExtType ext_flags,
			      Random* ran, const TwoDCoord& offs, bool na_by_range) {
  for(int d_y = 0; d_y < data->dim(1); d_y++) {
    int u_y = offs.y + d_y;
    for(int d_x = 0; d_x < data->dim(0); d_x++) {
      int u_x = offs.x + d_x;
      Unit* un = FindUnitFmCoord(u_x, u_y);
      if(un) {
	float val = data->SafeElAsVar(d_x, d_y).toFloat();
	un->ApplyInputData(val, ext_flags, ran, na_by_range);
      }
    }
  }
}

void Layer::ApplyInputData_Flat4d(taMatrix* data, Unit::ExtType ext_flags,
				  Random* ran, const TwoDCoord& offs, bool na_by_range) {
  // outer-loop is data-group (groups of x-y data items)
  for(int dg_y = 0; dg_y < data->dim(3); dg_y++) {
    for(int dg_x = 0; dg_x < data->dim(2); dg_x++) {

      for(int d_y = 0; d_y < data->dim(1); d_y++) {
	int u_y = offs.y + dg_y * data->dim(1) + d_y; // multiply out data indicies
	for(int d_x = 0; d_x < data->dim(0); d_x++) {
	  int u_x = offs.x + dg_x * data->dim(0) + d_x; // multiply out data indicies
	  Unit* un = FindUnitFmCoord(u_x, u_y);
	  if(un) {
	    float val = data->SafeElAsVar(d_x, d_y, dg_x, dg_y).toFloat();
	    un->ApplyInputData(val, ext_flags, ran, na_by_range);
	  }
	}
      }
    }
  }
}

void Layer::ApplyInputData_Gp4d(taMatrix* data, Unit::ExtType ext_flags, Random* ran,
				bool na_by_range) {
  // outer-loop is data-group (groups of x-y data items)
  for(int dg_y = 0; dg_y < data->dim(3); dg_y++) {
    for(int dg_x = 0; dg_x < data->dim(2); dg_x++) {

      for(int d_y = 0; d_y < data->dim(1); d_y++) {
	for(int d_x = 0; d_x < data->dim(0); d_x++) {
	  Unit* un = FindUnitFmGpCoord(dg_x, dg_y, d_x, d_y);
	  if(un) {
	    float val = data->SafeElAsVar(d_x, d_y, dg_x, dg_y).toFloat();
	    un->ApplyInputData(val, ext_flags, ran, na_by_range);
	  }
	}
      }
    }
  }
}

void Layer::ApplyLayerFlags(Unit::ExtType act_ext_flags) {
  SetExtFlag(act_ext_flags);
}

////////////////////////////////////////////////////////////////////////////////
//	Below are the primary computational interface to the Network Objects
//	for performing algorithm-specific activation and learning
//	Many functions operate directly on the units via threads, with
//	optional call through to the layers for any layer-level subsequent processing

void Layer::Init_InputData(Network* net) {
  if(ext_flag == Unit::NO_EXTERNAL)
    return;
  Unit* u;
  taLeafItr i;
  FOR_ITR_EL(Unit, u, units., i)
    u->Init_InputData();
  ext_flag = Unit::NO_EXTERNAL;
}

void  Layer::Init_Acts(Network* net) {
  ext_flag = Unit::NO_EXTERNAL;
  Unit* u;
  taLeafItr i;
  FOR_ITR_EL(Unit, u, units., i)
    u->Init_Acts(net);
}

void  Layer::Init_dWt(Network* net) {
  Unit* u;
  taLeafItr i;
  FOR_ITR_EL(Unit, u, units., i)
    u->Init_dWt(net);
}

void Layer::Init_Weights(Network* net) {
  Unit* u;
  taLeafItr i;
  FOR_ITR_EL(Unit, u, units., i)
    u->Init_Weights(net);
  sse = 0.0f;
}

void Layer::Init_Weights_post(Network* net) {
  Unit* u;
  taLeafItr i;
  FOR_ITR_EL(Unit, u, units., i)
    u->Init_Weights_post(net);
}

float Layer::Compute_SSE(Network* net, int& n_vals, bool unit_avg, bool sqrt) {
  n_vals = 0;
  sse = 0.0f;
  if(!(ext_flag & (Unit::TARG | Unit::COMP))) return 0.0f;
  Unit* u;
  taLeafItr i;
  bool has_targ;
  FOR_ITR_EL(Unit, u, units., i) {
    sse += u->Compute_SSE(net, has_targ);
    if(has_targ) n_vals++;
  }
  float rval = sse;
  if(unit_avg && n_vals > 0)
    sse /= (float)n_vals;
  if(sqrt)
    sse = sqrtf(sse);
  if(HasLayerFlag(NO_ADD_SSE) || ((ext_flag & Unit::COMP) && HasLayerFlag(NO_ADD_COMP_SSE))) {
    rval = 0.0f;
    n_vals = 0;
  }
  return rval;
}

////////////////////////////////////////////////////////////////////////////////
//	The following are misc functionality not required for primary computing

void Layer::Copy_Weights(const Layer* src) {
  units.Copy_Weights(&(src->units));
}
void Layer::SaveWeights_strm(ostream& strm, RecvCons::WtSaveFormat fmt) {
  // name etc is saved & processed by network level guy -- this is equiv to unit group
  units.SaveWeights_strm(strm, fmt);
}

int Layer::LoadWeights_strm(istream& strm, RecvCons::WtSaveFormat fmt, bool quiet) {
  return units.LoadWeights_strm(strm, fmt, quiet);
}

int Layer::SkipWeights_strm(istream& strm, RecvCons::WtSaveFormat fmt, bool quiet) {
  return Unit_Group::SkipWeights_strm(strm, fmt, quiet);
}

void Layer::SaveWeights(const String& fname, RecvCons::WtSaveFormat fmt) {
  taFiler* flr = GetSaveFiler(fname, ".wts", true);
  if(flr->ostrm)
    SaveWeights_strm(*flr->ostrm, fmt);
  flr->Close();
  taRefN::unRefDone(flr);
}

int Layer::LoadWeights(const String& fname, RecvCons::WtSaveFormat fmt, bool quiet) {
  taFiler* flr = GetLoadFiler(fname, ".wts", true);
  int rval = false;
  if(flr->istrm)
    rval = LoadWeights_strm(*flr->istrm, fmt, quiet);
  flr->Close();
  taRefN::unRefDone(flr);
  return rval;
}

void Layer::PropagateInputDistance() {
  int new_dist = dist.fm_input + 1;
  Projection* p;
  taLeafItr i;
  FOR_ITR_EL(Projection, p, send_prjns., i) {
    if(!p->layer || p->layer->lesioned()) continue;
    if(p->layer->dist.fm_input >= 0) { // already set
      if(new_dist < p->layer->dist.fm_input) { // but we're closer
	p->layer->dist.fm_input = new_dist;
	p->layer->PropagateInputDistance(); // note: this could lead back to us, but big deal.
	// the < sign prevents loops from continuing indefinitely.
      }
    }
    else { // not set yet
      p->layer->dist.fm_input = new_dist;
      p->layer->PropagateInputDistance();
    }
  }
}

void Layer::PropagateOutputDistance() {
  int new_dist = dist.fm_output + 1;
  Projection* p;
  taLeafItr i;
  FOR_ITR_EL(Projection, p, projections., i) {
    if(!p->from || p->from->lesioned()) continue;
    if(p->from->dist.fm_output >= 0) { // already set
      if(new_dist < p->from->dist.fm_output) { // but we're closer
	p->from->dist.fm_output = new_dist;
	p->from->PropagateOutputDistance(); // note: this could lead back to us, but big deal.
	// the < sign prevents loops from continuing indefinitely.
      }
    }
    else { // not set yet
      p->from->dist.fm_output = new_dist;
      p->from->PropagateOutputDistance();
    }
  }
}

void Layer::Compute_PrjnDirections() {
  Projection* p;
  taLeafItr i;
  FOR_ITR_EL(Projection, p, projections., i) {
    if(!p->from || p->from->lesioned()) {
      p->direction = Projection::DIR_UNKNOWN;
      continue;
    }
    // use the smallest value first..
    if(p->from->dist.fm_input <= p->from->dist.fm_output) {
      if(p->from->dist.fm_input < dist.fm_input) {
	p->direction = Projection::FM_INPUT;
      }
      else if(p->from->dist.fm_output < dist.fm_output) {
	p->direction = Projection::FM_OUTPUT;
      }
      else {
	p->direction = Projection::LATERAL;
      }
    }
    else {
      if(p->from->dist.fm_output < dist.fm_output) {
	p->direction = Projection::FM_OUTPUT;
      }
      else if(p->from->dist.fm_input < dist.fm_input) {
	p->direction = Projection::FM_INPUT;
      }
      else {
	p->direction = Projection::LATERAL;
      }
    }
  }  
}

bool Layer::SetUnitNames(bool force_use_unit_names) {
  if(!force_use_unit_names && unit_names.dims() == 0) return false;
  // first enforce geom, then do it.
  if(unit_groups) {
    if(unit_names.dims() == 2) {
      // if already populated and geom is larger than unit geom, go with flat geom
      if(unit_names.dim(0) * unit_names.dim(1) > un_geom.x * un_geom.y) {
	unit_names.SetGeom(2, flat_geom.x, flat_geom.y);
	int x, y;
	for (y = 0; y < flat_geom.y; ++y) {
	  for (x = 0; x < flat_geom.x; ++x) {
	    Unit* un = FindUnitFmCoord(x, y);
	    if (!un) continue;
	    un->SetName(unit_names.SafeElAsVar(x, y).toString());
	  }
	}
      }
      else {
	unit_names.SetGeom(2, un_geom.x, un_geom.y);
	if(units.gp.size > 0)
	  ((Unit_Group*)units.gp[0])->SetUnitNames(&unit_names);
      }
    }
    else { 
      unit_names.SetGeom(4, un_geom.x, un_geom.y, gp_geom.x, gp_geom.y);
      int gx, gy, ux, uy;
      for (gy = 0; gy < gp_geom.y; ++gy) {
	for (gx = 0; gx < gp_geom.x; ++gx) {
	  for (uy = 0; uy < un_geom.y; ++uy) {
	    for (ux = 0; ux < un_geom.x; ++ux) {
	      Unit* un = FindUnitFmGpCoord(gx, gy, ux, uy);
	      if (!un) continue;
	      un->SetName(unit_names.SafeElAsVar(ux, uy, gx, gy).toString());
	    }
	  }
	}
      }
    }
  }
  else {
    unit_names.SetGeom(2, un_geom.x, un_geom.y);
    units.SetUnitNames(&unit_names);
  }
  return true;
}

bool Layer::GetUnitNames(bool force_use_unit_names) {
  if(!force_use_unit_names && unit_names.dims() == 0) return false;
  // first enforce geom, then do it.
  if(unit_groups) {
    if(unit_names.dims() == 2) {
      unit_names.SetGeom(2, un_geom.x, un_geom.y);
      if(units.gp.size > 0)
	((Unit_Group*)units.gp[0])->GetUnitNames(&unit_names);
    }
    else { 
      unit_names.SetGeom(4, un_geom.x, un_geom.y, gp_geom.x, gp_geom.y);
      int gx, gy, ux, uy;
      for (gy = 0; gy < gp_geom.y; ++gy) {
	for (gx = 0; gx < gp_geom.x; ++gx) {
	  for (uy = 0; uy < un_geom.y; ++uy) {
	    for (ux = 0; ux < un_geom.x; ++ux) {
	      Unit* un = FindUnitFmGpCoord(gx, gy, ux, uy);
	      if (!un) continue;
	      unit_names.SetFmVar((Variant)un->GetName(), ux, uy, gx, gy);
	    }
	  }
	}
      }
    }
  }
  else {
    unit_names.SetGeom(2, un_geom.x, un_geom.y);
    units.GetUnitNames(&unit_names);
  }
  return true;
}

bool Layer::SetUnitNamesFromDataCol(const DataCol* unit_names_col, int max_un_chars) {
  if(TestError(!unit_names_col, "SetUnitNamesFromDataCol", "null unit_names_col"))
    return false;

  const MatrixGeom& cg = unit_names_col->cell_geom;
  taMatrix* nmat = (const_cast<DataCol*>(unit_names_col))->GetValAsMatrix(-1);
  if(!nmat) return false;
  taBase::Ref(nmat);

  if(unit_groups && cg.dims() == 4) { // check if all but first group is empty
    bool hugp_empty = true;
    int gx, gy, ux, uy;
    for(gy = 0; gy<cg.dim(3); gy++) {
      for(gx = 0; gx<cg.dim(2); gx++) {
	if(gx == 0 && gy == 0) continue; // skip 1st gp
	for(uy = 0; uy<cg.dim(1); uy++) {
	  for(ux = 0; ux<cg.dim(0); ux++) {
	    if(nmat->SafeElAsStr(ux,uy,gx,gy).nonempty()) {
	      hugp_empty = false;
	      break;
	    }
	  }
	}
      }
    }
    if(hugp_empty) {
      unit_names.SetGeom(2, cg.dim(0), cg.dim(1)); // just set for 1st gp
    }
    else {
      unit_names.SetGeomN(cg); // get our geom
    }
  }
  else {
    unit_names.SetGeomN(cg); // get our geom
  }
  for(int i=0;i<nmat->size && i<unit_names.size;i++) {
    String cnm = nmat->SafeElAsStr_Flat(i);
    unit_names.SetFmStr_Flat(cnm.elidedTo(max_un_chars), i);
  }
  taBase::unRefDone(nmat);
  SetUnitNames();		// actually set from these names
  return true;
}

void Layer::GetLocalistName() {
  Unit* u;
  taLeafItr i;
  FOR_ITR_EL(Unit, u, units., i) {
    u->GetLocalistName();
  }
  GetUnitNames(); // grab from units
}

void Layer::TransformWeights(const SimpleMathSpec& trans) {
  units.TransformWeights(trans);
}

void Layer::AddNoiseToWeights(const Random& noise_spec) {
  units.AddNoiseToWeights(noise_spec);
}

int Layer::PruneCons(const SimpleMathSpec& pre_proc,
			Relation::Relations rel, float cmp_val)
{
  return units.PruneCons(pre_proc, rel, cmp_val);
}

int Layer::ProbAddCons(float p_add_con, float init_wt) {
  int rval = 0;
  Projection* p;
  taLeafItr i;
  FOR_ITR_EL(Projection, p, projections., i)
    rval += p->ProbAddCons(p_add_con, init_wt);
  return rval;
}

int Layer::LesionCons(float p_lesion, bool permute) {
  return units.LesionCons(p_lesion, permute);
}

int Layer::LesionUnits(float p_lesion, bool permute) {
  StructUpdate(true);
  return units.LesionUnits(p_lesion, permute);
  StructUpdate(false);
  if (!Network_InStructUpdate(own_net)) {
    own_net->UpdtAfterNetMod();
  }
}

bool Layer::UpdateUnitSpecs(bool force) {
  if(!force && (unit_spec.SPtr() == m_prv_unit_spec)) return false;
  UnitSpec* sp = unit_spec.SPtr();
  if(!sp) return false;
  m_prv_unit_spec = sp;		// don't redo it
  Unit* u;
  taLeafItr i;
  FOR_ITR_EL(Unit, u, units., i) {
    if(sp->CheckObjectType(u))
      u->SetUnitSpec(sp);
    else
      return false;		// don't generate a bunch of redundant messages..
  }
  return true;
}

bool Layer::UpdateConSpecs(bool force) {
  bool rval = true;
  Projection* p;
  taLeafItr i;
  FOR_ITR_EL(Projection, p, projections., i) {
    if(!p->UpdateConSpecs(force))
      rval = false;
  }
  return rval;
}

bool Layer::SetUnitSpec(UnitSpec* sp) {
  if(!sp)	return false;
  if(!unit_spec.SetSpec(sp)) return false;
  if(!unit_spec.CheckSpec(units.el_typ, true)) { // quiet
    if(taMisc::Choice("The unit spec you are setting is not compatible with the unit type for units in this layer -- should I change the unit type to be: " +  unit_spec->min_obj_type->name
		      + " (if you answer No, you will continue to get errors until a compatible selection is made)",
		      "Yes", "No") == 0) {
      units.el_typ = unit_spec->min_obj_type;
    }
  }
  return UpdateUnitSpecs();
}

void Layer::SetUnitType(TypeDef* td) {
  if(td == NULL) return;
  units.el_typ = td;
  if(units.gp.size > 0) {
    int j;
    for(j=0;j<units.gp.size;j++) {
      ((Unit_Group*)units.gp.FastEl(j))->el_typ = td;
    }
  }
}

void Layer::MonitorVar(NetMonitor* net_mon, const String& variable) {
  if(!net_mon) return;
  net_mon->AddObject(this, variable);
}

bool Layer::Snapshot(const String& variable, SimpleMathSpec& math_op, bool arg_is_snap) {
  Unit* u;
  taLeafItr i;
  FOR_ITR_EL(Unit, u, units., i) {
    if(!u->Snapshot(variable, math_op, arg_is_snap)) return false;
  }
  return true;
}

int Layer::ReplaceUnitSpec(UnitSpec* old_sp, UnitSpec* new_sp) {
  int nchg = 0;
  if(unit_spec.SPtr() == old_sp) {
    unit_spec.SetSpec(new_sp);
    nchg++;
  }
  UpdateUnitSpecs();
  return nchg;
}

int Layer::ReplaceConSpec(ConSpec* old_sp, ConSpec* new_sp) {
  int nchg = 0;
  Projection* p;
  taLeafItr i;
  FOR_ITR_EL(Projection, p, projections., i)
    nchg += p->ReplaceConSpec(old_sp, new_sp);
  return nchg;
}

int Layer::ReplacePrjnSpec(ProjectionSpec* old_sp, ProjectionSpec* new_sp) {
  int nchg = 0;
  Projection* p;
  taLeafItr i;
  FOR_ITR_EL(Projection, p, projections., i)
    nchg += p->ReplacePrjnSpec(old_sp, new_sp);
  return nchg;
}

int Layer::ReplaceLayerSpec(LayerSpec* old_sp, LayerSpec* new_sp) {
  if(GetLayerSpec() != old_sp) return 0;
  SetLayerSpec(new_sp);
  return 1;
}

void Layer::WeightsToTable(DataTable* dt, Layer* send_lay) {
  if(send_lay == NULL) return;
  bool gotone = false;
  Projection* p;
  taLeafItr i;
  FOR_ITR_EL(Projection, p, projections., i) {
    if(p->from.ptr() != send_lay) continue;
    p->WeightsToTable(dt);
    gotone = true;
  }
  TestError(!gotone, "WeightsToTable", "No sending projection from:", send_lay->name);
}

void Layer::VarToTable(DataTable* dt, const String& variable) {
  if (!dt) {
    taProject* proj = GET_MY_OWNER(taProject);
    dt = proj->GetNewAnalysisDataTable(name + "_Var_" + variable, true);
  }

  Network* net = GET_MY_OWNER(Network);
  if(!net) return;

  NetMonitor nm;
  taBase::Own(nm, this);
  nm.AddLayer(this, variable);
  nm.items[0]->max_name_len = 20; // allow long names
  nm.SetDataNetwork(dt, net);
  nm.UpdateDataTable();
  dt->AddBlankRow();
  nm.GetMonVals();
  dt->WriteClose();
}

Unit* Layer::FindUnitFmCoord(int x, int y) {
  if(units.gp.size == 0)	// no group structure, just do it
    return units.FindUnitFmCoord(x,y);
  int un_x = x % un_geom.x;
  int un_y = y % un_geom.y;
  int gpidx = (y / un_geom.y) * gp_geom.x + (x / un_geom.x);
  if ((gpidx >= 0) && (gpidx < units.gp.size))
    return ((Unit_Group*)units.gp.FastEl(gpidx))->FindUnitFmCoord(un_x, un_y);
  return NULL;
}

Unit* Layer::FindUnitFmGpCoord(int gp_x, int gp_y, int un_x, int un_y) {
  Unit* rval = NULL;
  Unit_Group* ug = FindUnitGpFmCoord(gp_x, gp_y);
  if (ug) {
    rval = ug->FindUnitFmCoord(un_x, un_y);
  }
  return rval;
};

Unit_Group* Layer::FindUnitGpFmCoord(int gp_x, int gp_y) {
  if ((gp_x < 0) || (gp_x >= gp_geom.x) || (gp_y < 0) || (gp_y >= gp_geom.y) ) return NULL;
  int gidx = gp_y * gp_geom.x + gp_x;
  if (gidx >= units.gp.size) return NULL;
  return (Unit_Group*)units.gp.FastEl(gidx);
}

void Layer::GetActGeomNoSpc(TwoDCoord& nospc_geom) {
  nospc_geom = act_geom;
  if(!unit_groups) return;
  nospc_geom.x -= (gp_geom.x - 1) * gp_spc.x;
  nospc_geom.y -= (gp_geom.y - 1) * gp_spc.y;
}

void Layer::AddRelPos(TDCoord& rel_pos) {
  Layer_Group* lgp = dynamic_cast<Layer_Group*>(owner);
  if (lgp) {
    rel_pos += lgp->pos;
    lgp->AddRelPos(rel_pos);
  }
}
 
bool Layer::ChangeMyType(TypeDef* new_typ) {
  if(TestError(units.leaves > 0, "ChangeMyType", "You must first remove all units in the network before changing type of Layer -- otherwise it takes FOREVER -- do Network/Structure/Remove Units"))
    return false;
  return inherited::ChangeMyType(new_typ);
}

#ifdef DMEM_COMPILE
void Layer::DMem_DistributeUnits() {
  dmem_share_units.Reset();
  DMem_DistributeUnits_impl(dmem_share_units);
  dmem_share_units.Compile_ShareTypes();
}

bool Layer::DMem_DistributeUnits_impl(DMemShare& dms) {
  int np = 0; MPI_Comm_size(dmem_share_units.comm, &np);
  int this_proc = 0; MPI_Comm_rank(dmem_share_units.comm, &this_proc);
  if((dmem_dist == DMEM_DIST_DEFAULT) || (units.gp.size <= 0)) {
    int cnt = 0;
    taLeafItr ui;
    Unit* u;
    FOR_ITR_EL(Unit, u, units., ui) {
      u->DMem_SetLocalProc(cnt % np);
      u->DMem_SetThisProc(this_proc);
      dms.Link(u);
      cnt++;
    }
    return false;
  }
  else {
    int g;
    for(g=0; g<units.gp.size; g++) {
      Unit_Group* ug = (Unit_Group*)units.gp.FastEl(g);
      int cnt = 0;
      taLeafItr ui;
      Unit* u;
      FOR_ITR_EL(Unit, u, ug->, ui) {
	u->DMem_SetLocalProc(cnt % np);
	u->DMem_SetThisProc(this_proc);
	dms.Link(u);
	cnt++;
      }
    }
    return true;
  }
}

void Layer::DMem_SyncNRecvCons() {
  if(TestError(own_net->dmem_sync_level != Network::DMEM_SYNC_LAYER, "DMem_SyncNRecvCons",
	       "attempt to DMem sync at layer level, should only be at network level!")) {
    return;
  }
  dmem_share_units.Sync(0);
}

void Layer::DMem_SyncNet() {
  if(TestError(own_net->dmem_sync_level != Network::DMEM_SYNC_LAYER, "DMem_SyncNet",
	       "attempt to DMem sync layer level, should only be at network level!")) {
    return;
  }
  dmem_share_units.Sync(1);
}

void Layer::DMem_SyncAct() {
  if(TestError(own_net->dmem_sync_level != Network::DMEM_SYNC_LAYER, "DMem_SyncAct",
	       "attempt to DMem sync layer level, should only be at network level!")) {
    return;
  }
  dmem_share_units.Sync(2);
}
#endif


////////////////////////
//  Layer_Group	      //
////////////////////////

void Layer_Group::InitLinks() { 
  inherited::InitLinks(); 
  taBase::Own(pos,this);
}

void Layer_Group::CutLinks() { 
  pos.CutLinks();
  inherited::CutLinks(); 
}

void Layer_Group::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  // hack to disable the auto pos for nets
  if (!pos.isZero()) {
    Network* net = GET_MY_OWNER(Network);
    if (net && TestWarning(!(net->flags & Network::MANUAL_POS),
      "UpdateAfterEdit", 
      "Using Layer_Group.pos requires Network to use manual positioning of layers; so Network.MANUAL_POS has been set; you can reverse this by setting all Layer_Group.pos to 0 and then unchecking Network.MANUAL_POS")) 
    {
      net->flags = (Network::NetFlags)(net->flags | Network::MANUAL_POS);
      // don't do a UAE that is way too invasive
      net->DataChanged(DCR_ITEM_UPDATED);
    }
  }
}

void Layer_Group::AddRelPos(TDCoord& rel_pos) {
  Layer_Group* lg = GET_MY_OWNER(Layer_Group);
  if (lg) {
    rel_pos += lg->pos;
    lg->AddRelPos(rel_pos);
  }
}

void Layer_Group::DataChanged(int dcr, void* op1, void* op2) {
  inherited::DataChanged(dcr, op1, op2);
  if (dcr == DCR_LIST_ITEM_INSERT) {
    Network* net = GET_MY_OWNER(Network);
    if (net) 
      net->RebuildAllViews();
  }
}

void Layer_Group::BuildLayers() {
  BuildLayers_impl();
}

void Layer_Group::BuildLayers_impl() {
  Layer_Group* lg;
  for (int i = 0; i < gp.size; ++i) {
    lg = (Layer_Group*)gp.FastEl(i);
    lg->BuildLayers();
  }
}

void Layer_Group::BuildPrjns() {
  BuildPrjns_impl();
} 

void Layer_Group::BuildPrjns_impl() {
  Layer_Group* lg;
  for (int i = 0; i < gp.size; ++i) {
    lg = (Layer_Group*)gp.FastEl(i);
    lg->BuildPrjns();
  }
} 

void Layer_Group::Clean() {
  Layer_Group* lg;
  for (int i = gp.size - 1;  i >= 0; --i) {
    lg = (Layer_Group*)gp.FastEl(i);
    lg->Clean();
  }
  Clean_impl();
}

void Layer_Group::TriggerContextUpdate() {
  taLeafItr itr;
  Layer* lay;
  FOR_ITR_EL_REV(Layer, lay, this->, itr) {
    lay->TriggerContextUpdate();
  }
}

////////////////////////////////////////////////////////
//	Threading

void UnitCallTask::Initialize() {
  uidx_st = -1;
  uidx_ed = -1;
  uidx_inc = -1;
  unit_call = NULL;
}

void UnitCallTask::Destroy() {
  network.CutLinks();
  unit_call = NULL;
}

void UnitCallTask::run() {
  UnitCallThreadMgr* mg = mgr();
  const int nib_stop = mg->nibble_stop;

  if(uidx_inc > 0) {		// a forward run
    for(int i=uidx_st; i<uidx_ed; i+=uidx_inc) {
      Unit* un = network->units_flat[i];
      unit_call->call(un, network, task_id); // task id indicates threading, and which thread
      // debugging:
//       un->name = (String)task_id;
    }
  
    // then auto-nibble until done!
    const int nib_chnk = mg->nibble_chunk;
    while(true) {
      int nxt_uidx = mg->nibble_i.fetchAndAddOrdered(nib_chnk);
      if(nxt_uidx >= nib_stop) break;
      const int mx = MIN(nib_stop, nxt_uidx + nib_chnk);
      for(int i=nxt_uidx; i <mx; i++) {
	Unit* un = network->units_flat[i];
	unit_call->call(un, network, task_id); // task id indicates threading, and which thread
	// debugging:
// 	un->name = "n" + (String)task_id;
      }
      if(mx == nib_stop) break;		// we're the last guy
    }
  }
  else {			// backwards!
    for(int i=uidx_st; i>=uidx_ed; i+=uidx_inc) {
      Unit* un = network->units_flat[i];
      unit_call->call(un, network, task_id); // task id indicates threading, and which thread
	// debugging:
// 	un->name = (String)task_id;
    }
  
    // then auto-nibble until done!
    const int nib_chnk = -mg->nibble_chunk;
    while(true) {
      int nxt_uidx = mg->nibble_i.fetchAndAddOrdered(nib_chnk);
      if(nxt_uidx < nib_stop) break;
      const int mx = MAX(nib_stop, nxt_uidx + nib_chnk);
      for(int i=nxt_uidx; i>=mx; i--) {
	Unit* un = network->units_flat[i];
	unit_call->call(un, network, task_id); // task id indicates threading, and which thread
// 	// debugging:
//  	un->name = "n"+(String)task_id;
      }
      if(mx == nib_stop) break;		// we're the last guy
    }
  }
}

void UnitCallThreadMgr::Initialize() {
  chunk_pct = taMisc::thread_defaults.chunk_pct;
  nibble_chunk = taMisc::thread_defaults.nibble_chunk;
  compute_thr = taMisc::thread_defaults.compute_thr;
  min_units = taMisc::thread_defaults.min_units;
  send_netin = taMisc::thread_defaults.send_netin;
  interleave = true;
  ignore_lay_sync = false;
  nibble_i = -1;
  nibble_stop = 0;
  using_threads = false;
  n_threads_prev = n_threads;
}

void UnitCallThreadMgr::Destroy() {
}

void UnitCallThreadMgr::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(!taMisc::is_loading && n_threads != n_threads_prev) {
    network()->BuildUnits_Threads(); // calls InitAll where n_threads_prev is set..
  }
}

void UnitCallThreadMgr::InitAll() {
  n_threads_prev = n_threads;
  InitThreads();
  CreateTasks(&TA_UnitCallTask);
  SetTasksToThreads();
  Network* net = network();
  for(int i=0;i<tasks.size;i++) {
    UnitCallTask* uct = (UnitCallTask*)tasks[i];
    uct->network = net;
  }
}

void UnitCallThreadMgr::Run(ThreadUnitCall* unit_call, float comp_level,
			    bool backwards, bool layer_sync) {
  Network* net = network();
  if(n_threads == 1 || comp_level < compute_thr || net->units_flat.size < min_units
     || net->units_flat.size < tasks.size) {
    RunThread0(unit_call, backwards);
  }
  else {
//     { // debugging: clear names
//       const int nu = net->units_flat.size;
//       for(int i=1;i<nu;i++) {	// 0 = dummy idx
// 	net->units_flat[i]->name = "";
//       }
//     }
    if(backwards) {
      if(layer_sync && !ignore_lay_sync)
	RunThreads_BkwdLaySync(unit_call);
      else
	RunThreads_BkwdNetSync(unit_call);
    }
    else {
      if(layer_sync && !ignore_lay_sync)
	RunThreads_FwdLaySync(unit_call);
      else
	RunThreads_FwdNetSync(unit_call);
    }
  }
}

void UnitCallThreadMgr::RunThread0(ThreadUnitCall* unit_call, bool backwards) {
  using_threads = false;
  Network* net = network();
  const int nu = net->units_flat.size;
  if(backwards) {
    for(int i=nu-1;i>=1;i--) {	// 0 = dummy idx
      unit_call->call(net->units_flat[i], net, -1); // -1 indicates no threading
    }
  }
  else {			// forwards
    for(int i=1;i<nu;i++) {	// 0 = dummy idx
      unit_call->call(net->units_flat[i], net, -1); // -1 indicates no threading
    }
  }
}

void UnitCallThreadMgr::RunThreads_FwdNetSync(ThreadUnitCall* unit_call) {
  // note: units_flat[0] is a dummy so we have to add 1 to all idx's and skip over it
  using_threads = true;
  Network* net = network();
  const int nu = net->units_flat.size-1;	// 0 = dummy idx
  const int nt = tasks.size;
  int n_chunked = (int)((float)nu * chunk_pct);
  n_chunked = MAX(n_chunked, nt);

  int chnks = n_chunked / nt;
  n_chunked = chnks * nt; // must be even multiple of threads!
  while(n_chunked > nu)
    n_chunked -= nt;
  chnks = n_chunked / nt;

  if(interleave) {
    // sample task allocation: chnks = 3, nt = 2, n_chunked=15
    // un: 0123456789012345...
    // th  st       ed    nc
    // 0   0    5   10
    // 1    1    6   11
    // 2     2    7   12
    // 3      3    8   13
    // 4       4    9   14

    int end_base = 2 + n_chunked - nt; // add 1 b/c uses < ed and not <= ed, and 1 for dummy idx
    for(int i=0;i<nt;i++) {
      UnitCallTask* uct = (UnitCallTask*)tasks[i];
      uct->unit_call = unit_call;
      uct->uidx_st = 1+i;
      uct->uidx_ed = end_base + i;
      uct->uidx_inc = nt;
    }
  }
  else {
    for(int i=0;i<nt;i++) {
      UnitCallTask* uct = (UnitCallTask*)tasks[i];
      uct->unit_call = unit_call;
      uct->uidx_st = 1+i*chnks;
      uct->uidx_ed = 1+(i+1)*chnks;
      uct->uidx_inc = 1;
    }
  }

  nibble_i = 1+n_chunked;
  nibble_stop = 1+nu;

  // then run the subsidiary guys
  for(int i=0;i<threads.size;i++) {
    threads[i]->runTask();
  }
  tasks[0]->run();		// run our own set..

  // note: all the nibbling is automatic within the single run() deploy

  // finally, always need to sync at end to ensure that everyone is done!
  for(int i=0;i<threads.size;i++) {
    threads[i]->sync();
  }
}

void UnitCallThreadMgr::RunThreads_BkwdNetSync(ThreadUnitCall* unit_call) {
  // note: units_flat[0] is a dummy so we have to add 1 to all idx's and skip over it
  using_threads = true;
  Network* net = network();
  const int nu = net->units_flat.size-1;	// 0 = dummy idx
  const int nt = tasks.size;
  int n_chunked = (int)((float)nu * chunk_pct);
  n_chunked = MAX(n_chunked, nt);

  int chnks = n_chunked / nt;
  n_chunked = chnks * nt; // must be even multiple of threads!
  while(n_chunked > nu)
    n_chunked -= nt;
  chnks = n_chunked / nt;

  int st_base = nu;			     // starting index (-1 already taken above)
  int end_base = st_base - (n_chunked - nt); // no -1 b/c >= end_base

  for(int i=0;i<nt;i++) {
    UnitCallTask* uct = (UnitCallTask*)tasks[i];
    uct->unit_call = unit_call;
    uct->uidx_st = st_base - i;
    uct->uidx_ed = end_base - i;
    uct->uidx_inc = -nt;
  }
  nibble_i = nu - n_chunked;	// where to start nibbling
  nibble_stop = 1;	// 0 = dummy idx

  // then run the subsidiary guys
  for(int i=0;i<threads.size;i++) {
    threads[i]->runTask();
  }
  tasks[0]->run();		// run our own set..

  // note: all the nibbling is automatic within the single run() deploy

  // finally, always need to sync at end to ensure that everyone is done!
  for(int i=0;i<threads.size;i++) {
    threads[i]->sync();
  }
}

void UnitCallThreadMgr::RunThreads_FwdLaySync(ThreadUnitCall* unit_call) {
  using_threads = true;
  Network* net = network();
  const int nt = tasks.size;

  // note: this has same logic as net sync but all within each layer
  // IMPORTANT: lay sync guys MUST have all units.leaves units in units_flat --
  // the run code assumes this is true

  Layer* lay;
  taLeafItr li;
  FOR_ITR_EL(Layer, lay, net->layers., li) {
    if(lay->lesioned()) continue; // don't even add units from lesioned layers!!
    int st_idx = lay->units_flat_idx;
    const int nu = lay->units.leaves;

    if(nu < min_units || nu < nt) {
      // run locally
      for(int i=0;i<nu;i++) {
	unit_call->call(net->units_flat[st_idx + i], net, -1); // -1 indicates no threading
      }
    }
    else {
      int n_chunked = (int)((float)nu * chunk_pct);
      n_chunked = MAX(n_chunked, nt);

      int chnks = n_chunked / nt;
      n_chunked = chnks * nt; // must be even multiple of threads!
      while(n_chunked > nu)
	n_chunked -= nt;

      int end_base = st_idx + 1 + n_chunked - nt; // add 1 b/c uses < ed and not <= ed

      for(int i=0;i<nt;i++) {
	UnitCallTask* uct = (UnitCallTask*)tasks[i];
	uct->unit_call = unit_call;
	uct->uidx_st = st_idx + i;
	uct->uidx_ed = end_base + i;
	uct->uidx_inc = nt;
      }
      nibble_i = st_idx + n_chunked;
      nibble_stop = st_idx + nu;

      // then run the subsidiary guys
      for(int i=0;i<threads.size;i++) {
	threads[i]->runTask();
      }
      tasks[0]->run();		// run our own set..

      // note: all the nibbling is automatic within the single run() deploy

      // finally, always need to sync at end to ensure that everyone is done!
      for(int i=0;i<threads.size;i++) {
	threads[i]->sync();
      }
    }
  }
}

void UnitCallThreadMgr::RunThreads_BkwdLaySync(ThreadUnitCall* unit_call) {
  using_threads = true;
  Network* net = network();
  const int nt = tasks.size;

  // note: this has same logic as net sync but all within each layer
  // IMPORTANT: lay sync guys MUST have all units.leaves units in units_flat --
  // the run code assumes this is true

  int li;
  for(li=net->layers.leaves-1; li>=0; li--) {
    Layer* lay = net->layers.Leaf(li);
    if(lay->lesioned()) continue; // don't even add units from lesioned layers!!
    int st_idx = lay->units_flat_idx;
    const int nu = lay->units.leaves;

    if(nu < min_units || nu < nt) {
      // run locally
      for(int i=nu-1;i>=0;i--) {
	unit_call->call(net->units_flat[st_idx + i], net, -1); // -1 indicates no threading
      }
    }
    else {
      int n_chunked = (int)((float)nu * chunk_pct);
      n_chunked = MAX(n_chunked, nt);

      int chnks = n_chunked / nt;
      n_chunked = chnks * nt; // must be even multiple of threads!
      while(n_chunked > nu)
	n_chunked -= nt;

      int st_base = st_idx + nu-1;			     // starting index
      int end_base = st_base - (n_chunked - nt); // no -1 b/c >= end_base

      for(int i=0;i<nt;i++) {
	UnitCallTask* uct = (UnitCallTask*)tasks[i];
	uct->unit_call = unit_call;
	uct->uidx_st = st_base - i;
	uct->uidx_ed = end_base - i;
	uct->uidx_inc = -nt;
      }
      nibble_i = st_idx + nu-1 - n_chunked;
      nibble_stop = st_idx;

      // then run the subsidiary guys
      for(int i=0;i<threads.size;i++) {
	threads[i]->runTask();
      }
      tasks[0]->run();		// run our own set..

      // note: all the nibbling is automatic within the single run() deploy

      // finally, always need to sync at end to ensure that everyone is done!
      for(int i=0;i<threads.size;i++) {
	threads[i]->sync();
      }
    }
  }
}

////////////////////////
//	Network	 View //
////////////////////////

void NetViewFontSizes::Initialize() {
  net_name = .05f;
  net_vals = .05f;
  layer = .04f;
  layer_vals = .03f;
  prjn = .01f;
  unit = .02f;
  un_nm_len = 3;
}

void NetViewParams::Initialize() {
  xy_square = false;
  unit_spacing = .05f;
  prjn_disp = L_R_F;
  prjn_name = false;
  prjn_width = .002f;
  prjn_trans = .5f;
  lay_trans = .5f;
  unit_trans = 0.6f;
}

void NetViewObj::Initialize() {
  obj_type = TEXT;
  text = "Select, Context Menu to Edit";
  scale = 1.0f;
  font_size = .05f;
  set_color = false;
  color.Set(0.0f, 0.0f, 0.0f);
}

void NetViewObj::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  // anything?
}

void Network::Initialize() {
  specs.SetBaseType(&TA_BaseSpec);
  layers.SetBaseType(&TA_Layer);
  
  flags = NF_NONE;
  auto_build = AUTO_BUILD;

  train_mode = TRAIN;
  wt_update = ON_LINE;
  small_batch_n = 10;
  small_batch_n_eff = 10;

  batch = 0;
  epoch = 0;
  group = 0;
  trial = 0;
  tick = 0;
  cycle = 0;
  time = 0.0f;

  sse_unit_avg = false;
  sse_sqrt = false;
  sse = 0.0f;
  sum_sse = 0.0f;
  avg_sse = 0.0f;
  cnt_err = 0.0f;
  pct_err = 0.0f;
  pct_cor = 0.0f;

  cnt_err_tol = 0.0f;

  cur_sum_sse = 0.0f;
  avg_sse_n = 0;
  cur_cnt_err = 0.0f;

  dmem_sync_level = DMEM_SYNC_NETWORK;
  dmem_nprocs = 1;
  dmem_nprocs_actual = MIN(dmem_nprocs, taMisc::dmem_nprocs);
  usr1_save_fmt = FULL_NET;
  wt_save_fmt = TEXT;
  lay_layout = THREE_D;

  n_units = 0;
  n_cons = 0;
  max_size.x = 1;
  max_size.y = 1;
  max_size.z = 1;

  proj = NULL;
#ifdef DMEM_COMPILE
  dmem_share_units.comm = (MPI_Comm)MPI_COMM_WORLD;
  dmem_agg_sum.agg_op = MPI_SUM;
#endif
}

void Network::Destroy()	{ 
  CutLinks(); 
}

void Network::InitLinks() {
  GetDataLink(); // forces creation, so we track Updates
  proj = GET_MY_OWNER(ProjectBase);
  taBase::Own(specs, this);
  taBase::Own(layers, this);
  taBase::Own(view_objs, this);
  taBase::Own(max_size, this);
  taBase::Own(font_sizes, this);
  taBase::Own(view_params, this);

  taBase::Own(train_time, this);  //train_time.name = "train_time";
  taBase::Own(epoch_time, this);  //epoch_time.name = "epoch_time";
  taBase::Own(group_time, this);  //group_time.name = "group_time";
  taBase::Own(trial_time, this);  //trial_time.name = "trial_time";
  taBase::Own(settle_time, this);  //settle_time.name = "settle_time";
  taBase::Own(cycle_time, this);  //cycle_time.name = "cycle_time";
  taBase::Own(wt_sync_time, this); //wt_sync_time.name = "wt_sync_time";
  taBase::Own(misc_time, this);  //misc_time.name = "misc_time";

  taBase::Own(send_netin_tmp, this);
  taBase::Own(threads, this);

#ifdef DMEM_COMPILE
  taBase::Own(dmem_net_comm, this);
  taBase::Own(dmem_trl_comm, this);
  taBase::Own(dmem_share_units, this);
  taBase::Own(dmem_agg_sum, this);
  DMem_InitAggs();
#endif

  NetTextUserData();

  inherited::InitLinks();
}

void Network::CutLinks() {
  if(!owner) return; // already replacing or already dead
#ifdef DMEM_COMPILE
  dmem_net_comm.FreeComm();
  dmem_trl_comm.FreeComm();
#endif
  units_flat.Reset();
  send_netin_tmp.CutLinks();
  threads.CutLinks();
  RemoveCons();			// do this first in optimized way!
  RemoveUnitGroups();		// then units
  misc_time.CutLinks();
  wt_sync_time.CutLinks();
  cycle_time.CutLinks();
  settle_time.CutLinks();
  trial_time.CutLinks();
  group_time.CutLinks();
  epoch_time.CutLinks();
  train_time.CutLinks();
  view_params.CutLinks();
  font_sizes.CutLinks();
  max_size.CutLinks();
  view_objs.CutLinks();
  layers.CutLinks();		// then std kills
  specs.CutLinks();
  proj = NULL;
  inherited::CutLinks();
}

void Network::Copy_(const Network& cp) {
  specs = cp.specs;
  layers = cp.layers;
  view_objs = cp.view_objs;

  auto_build = cp.auto_build;

  train_mode = cp.train_mode;
  wt_update = cp.wt_update;
  small_batch_n = cp.small_batch_n;
  small_batch_n_eff = cp.small_batch_n_eff;

  batch = cp.batch;
  epoch = cp.epoch;
  group = cp.group;
  trial = cp.trial;
  tick = cp.tick;
  cycle = cp.cycle;
  time = cp.time;
  group_name = cp.group_name;
  trial_name = cp.trial_name;
  output_name = cp.output_name;

  sse_unit_avg = cp.sse_unit_avg;
  sse_sqrt = cp.sse_sqrt;
  sse = cp.sse;
  sum_sse = cp.sum_sse;
  avg_sse = cp.avg_sse;
  cnt_err_tol = cp.cnt_err_tol;
  cnt_err = cp.cnt_err;
  pct_err = cp.pct_err;
  pct_cor = cp.pct_cor;

  cur_sum_sse = cp.cur_sum_sse;
  avg_sse_n = cp.avg_sse_n;
  cur_cnt_err = cp.cur_cnt_err;

  dmem_sync_level = cp.dmem_sync_level;
  dmem_nprocs = cp.dmem_nprocs;
  dmem_nprocs_actual = cp.dmem_nprocs_actual;

  usr1_save_fmt = cp.usr1_save_fmt;
  wt_save_fmt = cp.wt_save_fmt;
  lay_layout = cp.lay_layout;

  max_size = cp.max_size;

  font_sizes = cp.font_sizes;
  view_params = cp.view_params;

  UpdatePointers_NewPar((taBase*)&cp, this); // update all the pointers
  SyncSendPrjns();
  FixPrjnIndexes();			     // fix the recv_idx and send_idx (not copied!)
  UpdateAllSpecs();
  LinkSendCons();		// set the send cons (not copied!)
  BuildUnits_Threads();
#ifdef DMEM_COMPILE
  DMem_DistributeUnits();
#endif
//   ((Network&)cp).SyncSendPrjns(); // these get screwed up in there somewhere..
  //note: batch update in tabase copy
}

void Network::UpdateAfterEdit_impl(){
  inherited::UpdateAfterEdit_impl();
  if(wt_save_fmt == NET_FMT)
    wt_save_fmt = TEXT;

  ClearNetFlag(SAVE_UNITS_FORCE); // might have been saved in on state from recover file or something!
}

void Network::UpdtAfterNetMod() {
  //  SyncSendPrjns();
  CountRecvCons();
  BuildUnits_Threads();
  small_batch_n_eff = small_batch_n;
#ifdef DMEM_COMPILE
  DMem_SyncNRecvCons();
  DMem_UpdtWtUpdt();
#endif
}

void Network::SetProjectionDefaultTypes(Projection* prjn) {
  // noop for base case: algorithms must override!
  prjn->spec.type = &TA_FullPrjnSpec; 
}

bool Network::ChangeMyType(TypeDef* new_typ) {
  if(TestError(n_units > 0, "ChangeMyType", "You must first remove all units in the network before changing its type -- otherwise it takes FOREVER -- do Network/Structure/Remove Units"))
    return false;
  return inherited::ChangeMyType(new_typ);
}

int Network::Dump_Load_Value(istream& strm, taBase* par) {
  int rval = inherited::Dump_Load_Value(strm, par);
  if(rval)
    LinkSendCons();

  ClearNetFlag(SAVE_UNITS_FORCE);	// might have been saved in on state from recover file or something!

  BuildUnits_Threads();
#ifdef DMEM_COMPILE
  DMem_DistributeUnits();
  DMem_PruneNonLocalCons();
  DMem_UpdtWtUpdt();
#endif

  return rval;
}

int Network::Save_strm(ostream& strm, TAPtr par, int indent) {
  SetNetFlag(SAVE_UNITS_FORCE); // override if !SAVE_UNITS
  int rval = inherited::Save_strm(strm, par, indent);
  ClearNetFlag(SAVE_UNITS_FORCE);
  return rval;
}

int Network::GetDefaultX(){
  int rval = 1;
  switch(lay_layout) {
  case THREE_D:	rval = DEFAULT_3D_NETSIZE_X; break;
  case TWO_D:	rval = DEFAULT_2D_NETSIZE_X; break;
  }
  return rval;
}

int Network::GetDefaultY(){
  int rval = 1;
  switch(lay_layout) {
  case THREE_D:	rval = DEFAULT_3D_NETSIZE_Y; break;
  case TWO_D:	rval = DEFAULT_2D_NETSIZE_Y; break;
  }
  return rval;
}

int Network::GetDefaultZ(){
  int rval = 1;
  switch(lay_layout) {
  case THREE_D:	rval = DEFAULT_3D_NETSIZE_Z; break;
  case TWO_D:	rval = DEFAULT_2D_NETSIZE_Z; break;
  }
  return rval;
}

void Network::Build() {
  taMisc::Busy();
  ++taMisc::no_auto_expand; // c'mon...!!! ;)
  StructUpdate(true);
  BuildLayers(); // note: for Area constructs
  BuildUnits();
  BuildPrjns(); // note: for Area constructs
  Connect();
  StructUpdate(false);
//   if (net_inst.ptr()) {
//     net_inst->OnBuild();
//   }
  --taMisc::no_auto_expand;
  taMisc::DoneBusy();
}

void Network::BuildLayers() {
  taMisc::Busy();
  ++taMisc::no_auto_expand;
  StructUpdate(true);
  layers.BuildLayers(); // recurses
  StructUpdate(false);
  --taMisc::no_auto_expand;
  taMisc::DoneBusy();
  if(!taMisc::gui_active)    return;
}

void Network::BuildUnits() {
  taMisc::Busy();
  StructUpdate(true);
  UpdateMax();
  Layer* l;
  taLeafItr i;
  FOR_ITR_EL(Layer, l, layers., i)
    l->BuildUnits();
  StructUpdate(false);
  taMisc::DoneBusy();
#ifdef DMEM_COMPILE
  DMem_DistributeUnits();
#endif
  UpdtAfterNetMod();		// calls BuildUnits_Threads
  if(!taMisc::gui_active)    return;
}

void Network::BuildUnits_Threads() {
  threads.InitAll();
  units_flat.Reset();
  // real indexes start at 1, to allow 0 to be a dummy case for inactive units that may
  // nevertheless get a send netin call to them -- all those just go to this 0 bin
  units_flat.Add(NULL);		// add a dummy null 
  Layer* lay;
  taLeafItr li;
  FOR_ITR_EL(Layer, lay, layers., li) {
    if(lay->lesioned()) {
      lay->units_flat_idx = 0;
      continue; // don't even add units from lesioned layers!!
    }
    lay->BuildUnits_Threads(this);
  }
  // temporary storage for sender-based netinput computation
  if(units_flat.size > 0 && threads.n_threads > 0) {
    send_netin_tmp.SetGeom(2, units_flat.size, threads.n_threads);
    send_netin_tmp.InitVals(0.0f);
  }
}

void Network::BuildPrjns() {
  taMisc::Busy();
  ++taMisc::no_auto_expand;
  StructUpdate(true);
  layers.BuildPrjns(); // recurses
  StructUpdate(false);
  taMisc::DoneBusy();
  --taMisc::no_auto_expand;
  if(!taMisc::gui_active)    return;
}

void Network::PreConnect() {
  Layer* l;
  taLeafItr i;
  FOR_ITR_EL(Layer, l, layers., i)
    l->PreConnect();
}

void Network::Connect() {
  taMisc::Busy();
  ++taMisc::no_auto_expand; // c'mon...!!! ;)
  StructUpdate(true);
  RemoveCons();
  SyncSendPrjns();
  // go in reverse order so that symmetric prjns can be made in
  // response to receiver-based projections
  Layer* l;
  int i;
  for(i=layers.leaves-1;i>=0;i--) {
    l = (Layer*)layers.Leaf(i);
    l->Connect();
  }
  UpdtAfterNetMod();
  StructUpdate(false);
  --taMisc::no_auto_expand;
  taMisc::DoneBusy();
}

void Network::CountRecvCons() {
  n_units = 0;
  n_cons = 0;
  Layer* l;
  taLeafItr i;
  FOR_ITR_EL(Layer, l, layers., i) {
    n_cons += l->CountRecvCons();
    n_units += l->units.leaves;
  }
}

bool Network::CheckBuild(bool quiet) {
  Layer* l;
  taLeafItr i;
  FOR_ITR_EL(Layer, l, layers., i) {
    if(!l->CheckBuild(quiet)) {
      if(!quiet)
	taMisc::CheckError("Network:",GetName(),"Needs the 'Build' command to be run");
      return false;
    }
  }
  return true;
}

bool Network::CheckConnect(bool quiet) {
  Layer* l;
  taLeafItr i;
  FOR_ITR_EL(Layer, l, layers., i) {
    if(!l->CheckConnect(quiet)) {
      if(!quiet)
	taMisc::CheckError("Network:",GetName(), "Needs the 'Connect' command to be run");
      return false;
    }
  }
  return true;
}

void Network::CheckThisConfig_impl(bool quiet, bool& rval) {
  //NOTE: slightly non-standard, because we bail on first detected issue
  if (!CheckBuild(quiet)) { rval = false; return; }
  if (!CheckConnect(quiet)) { rval = false; return; }
  UpdtAfterNetMod();		// just to be sure..
  inherited::CheckThisConfig_impl(quiet, rval);
}

void Network::CheckChildConfig_impl(bool quiet, bool& rval) {
  inherited::CheckChildConfig_impl(quiet, rval);
  specs.CheckConfig(quiet, rval); //note: this checks the specs themselves, not objs
  layers.CheckConfig(quiet, rval);
}

void Network::SetUnitType(TypeDef* td) {
  Layer* l;
  taLeafItr i;
  FOR_ITR_EL(Layer, l, layers., i) {
    l->SetUnitType(td);
  }
}

void Network::SyncSendPrjns() {
  Layer* l;
  taLeafItr i;
  FOR_ITR_EL(Layer, l, layers., i) {
    l->SyncSendPrjns();
  }
}

void Network::FixPrjnIndexes() {
  Layer* l;
  taLeafItr i;
  FOR_ITR_EL(Layer, l, layers., i) {
    l->FixPrjnIndexes();
  }
}

void Network::ConnectUnits(Unit* u_to, Unit* u_from, bool record, ConSpec* conspec) {
  if(u_to == NULL) return; // must have reciever
  if(u_from == NULL)    u_from = u_to; // assume self con if no from

  Layer* lay = GET_OWNER(u_to,Layer);
  Layer* l_from = GET_OWNER(u_from,Layer);
  Projection* pjn = NULL;
  taLeafItr p;
  // check to see if a pjrn already exists
  FOR_ITR_EL(Projection, pjn, lay->projections., p) {
    if((pjn->from.ptr() == l_from) &&
       (pjn->spec->InheritsFrom(&TA_CustomPrjnSpec)) &&
       ((conspec == NULL) || (pjn->con_spec == conspec)))
      break; // ok found one
  }
  if(pjn==NULL) { // no projection
#ifdef DMEM_COMPILE
    if(record && (taMisc::dmem_nprocs == 1)) { // don't actually run under gui in dmem mode
#endif
      pjn = (Projection*) lay->projections.New(1);
      pjn->SetCustomFrom(l_from);
      pjn->spec.type = &TA_CustomPrjnSpec;
      if(conspec)
	pjn->con_spec.SetSpec(conspec);
      pjn->spec.UpdateAfterEdit();
      pjn->projected = true;
      pjn->UpdateAfterEdit();
#ifdef DMEM_COMPILE
    }
#endif
    if (record) {
      taMisc::RecordScript(lay->projections.GetPath() + ".NewEl(1);\n");
      taMisc::SREAssignment(pjn,pjn->FindMember("from_type"));
      taMisc::ScriptRecordAssignment(pjn,pjn->FindMember("from"));
      taMisc::RecordScript(pjn->GetPath() + ".spec.type = CustomPrjnSpec;");
    }
  }

  u_to->ConnectFromCk(u_from,pjn);
  if (record)
    taMisc::RecordScript(u_to->GetPath() + ".ConnectFromCk(" + u_from->GetPath() +
					    ", " + pjn->GetPath() + ");\n");
  lay->UpdateAfterEdit();
}

void Network::LinkSendCons() {
  Layer* l;
  taLeafItr i;
  FOR_ITR_EL(Layer, l, layers., i)
    l->LinkSendCons();
}

#ifdef TA_GUI
NetView* Network::NewView(T3DataViewFrame* fr) {
  return NetView::New(this, fr);
}
#endif

NetViewObj* Network::NewViewText(const String& txt) {
  StructUpdate(true);
  NetViewObj* rval = view_objs.NewEl(1);
  rval->obj_type = NetViewObj::TEXT;
  if(txt.nonempty())
    rval->text = txt;
  StructUpdate(false);
  return rval;
}

NetViewObj* Network::NewGlassBrain() {
  StructUpdate(true);
  NetViewObj* lh = view_objs.NewEl(1);
  lh->name = "LeftHemisphere";
  lh->desc = "left hemisphere of human cortex (glass brain)";
  lh->obj_type = NetViewObj::OBJECT;
  lh->obj_fname = taMisc::app_dir + "/3dobj_lib/glass_brain_LH.iv";
  lh->set_color = true;
  lh->color.Set(.7f, .7f, .7f, .5f);
  lh->pos.x = 1.25f; lh->pos.y = .5f; lh->pos.z = -1.0f;

  NetViewObj* rh = view_objs.NewEl(1);
  rh->name = "RightHemisphere";
  rh->desc = "right hemisphere of human cortex (glass brain)";
  rh->obj_type = NetViewObj::OBJECT;
  rh->obj_fname = taMisc::app_dir + "/3dobj_lib/glass_brain_RH.iv";
  rh->set_color = true;
  rh->color.Set(.7f, .7f, .7f, .5f);
  rh->pos.x = -.25f; rh->pos.y = .5f; rh->pos.z = -1.0f;
  StructUpdate(false);
  return rh;
}

// PlaceNetText is in netstru_qtso.cpp

void Network::NetTextUserData() {
  TypeDef* td = GetTypeDef();
  Variant bool_on_val;
  bool_on_val.setBool(true);
  for(int i=td->members.size-1; i>=0; i--) {
    MemberDef* md = td->members[i];
    if(!md->HasOption("VIEW")) continue;
    if(HasUserData(md->name)) continue;
    SetUserData(md->name, bool_on_val);
  }  
}

Layer* Network::NewLayer() {
  return layers.NewEl(1);
}

void Network::RemoveUnits() {
  taMisc::Busy();
  StructUpdate(true);
  //  RemoveMonitors(); // not needed with smartref!
  Layer* l;
  taLeafItr i;
  FOR_ITR_EL(Layer, l, layers., i)
    l->RemoveUnits();
  n_cons = 0;
  StructUpdate(false);
  taMisc::DoneBusy();
}

void Network::RemoveUnitGroups() {
  taMisc::Busy();
  StructUpdate(true);
  // RemoveMonitors(); // not needed with smartref!
  Layer* l;
  taLeafItr i;
  FOR_ITR_EL(Layer, l, layers., i)
    l->RemoveUnitGroups();
  StructUpdate(false);
  taMisc::DoneBusy();
}

void Network::LayoutUnitGroups() {
  taMisc::Busy();
  StructUpdate(true);
  Layer* l;
  taLeafItr i;
  FOR_ITR_EL(Layer, l, layers., i)
    l->LayoutUnitGroups();
  StructUpdate(false);
  taMisc::DoneBusy();
}

void Network::RemoveCons() {
  taMisc::Busy();
  StructUpdate(true);
  //  RemoveMonitors(); // not needed with smartref!
  Layer* l;
  taLeafItr i;
  FOR_ITR_EL(Layer, l, layers., i)
    l->RemoveCons_Net();
  StructUpdate(false);
  taMisc::DoneBusy();
}

void Network::Init_InputData(){
  Layer* l;
  taLeafItr i;
  FOR_ITR_EL(Layer, l, layers., i) {
    if(!l->lesioned())
      l->Init_InputData(this);
  }
}

void Network::Init_Acts(){
  send_netin_tmp.InitVals(0.0f);
  Layer* l;
  taLeafItr i;
  FOR_ITR_EL(Layer, l, layers., i) {
    if(!l->lesioned())
      l->Init_Acts(this);
  }
}

void Network::Init_dWt(){
  Layer* l;
  taLeafItr i;
  FOR_ITR_EL(Layer, l, layers., i) {
    if(!l->lesioned())
      l->Init_dWt(this);
  }
}

void Network::Init_Weights() {
  // do lots of checking here to make sure, cuz often 1st thing that happens
  //NOTE: this will typically be nested inside a gui check
  if (!CheckConfig(true)) return;

  taMisc::Busy();
  Layer* l;
  taLeafItr i;
  FOR_ITR_EL(Layer, l, layers., i) {
    if(!l->lesioned())
      l->Init_Weights(this);
  }
  Init_Weights_post();		// done after all initialization (for scaling wts...)

#ifdef DMEM_COMPILE
  // do the dmem weight symmetrizing!
  DMem_SymmetrizeWts();
#endif

  Init_Acts();			// also re-init state at this point..
  Init_Metrics();
  UpdateAllViews();
  taMisc::DoneBusy();
}

void Network::Init_Weights_post() {
  Layer* l;
  taLeafItr i;
  FOR_ITR_EL(Layer, l, layers., i) {
    if(!l->lesioned())
      l->Init_Weights_post(this);
  }
}

void Network::Init_Metrics() {
  Init_Counters();
  Init_Stats();
  Init_Timers();
}

void Network::Init_Counters() {
  // this is one you do not reinit: loops over inits!
//   batch = 0;
  epoch = 0;
  group = 0;
  group_name = "";
  trial = 0;
  trial_name = "";
  tick = 0;
  cycle = 0;
  time = 0.0f;
}

void Network::Init_Stats() {
  sse = 0.0f;
  sum_sse = 0.0f;
  avg_sse = 0.0f;
  cnt_err = 0.0f;
  pct_err = 0.0f;
  pct_cor = 0.0f;

  output_name = "";

  cur_sum_sse = 0.0f;
  avg_sse_n = 0;
  cur_cnt_err = 0.0f;
}

void Network::Init_Timers() {
  train_time.ResetUsed();
  epoch_time.ResetUsed();
  trial_time.ResetUsed();
  settle_time.ResetUsed();
  cycle_time.ResetUsed();
  wt_sync_time.ResetUsed();
  misc_time.ResetUsed();
}

// NOTE on compute load levels for thread.Run function (2nd arg) --
// any function that goes over all the cons gets a 1.0 (netin, dwt, wts)
// others (Compute_Act) just have small guess value, which should be replaced

void Network::Compute_Netin() {
  ThreadUnitCall un_call(&Unit::Compute_Netin);
  threads.Run(&un_call, 1.0f);

#ifdef DMEM_COMPILE
  DMem_SyncNet();
#endif
}

void Network::Send_Netin() {
  ThreadUnitCall un_call(&Unit::Send_Netin);
  threads.Run(&un_call, 1.0f);

  // now need to roll up the netinput into unit vals
  const int nu = units_flat.size;
  const int nt = threads.tasks.size;
  if(threads.using_threads) {
    for(int i=1;i<nu;i++) {	// 0 = dummy idx
      Unit* un = units_flat[i];
      float nw_nt = 0.0f;
      for(int j=0;j<nt;j++) {
	nw_nt += send_netin_tmp.FastEl(i, j);
      }
      un->Compute_SentNetin(this, nw_nt);
    }
  }
  else {
    for(int i=1;i<nu;i++) {	// 0 = dummy idx
      Unit* un = units_flat[i];
      float nw_nt = send_netin_tmp.FastEl(i, 0); // use 0 thread
      un->Compute_SentNetin(this, nw_nt);
    }
  }
  send_netin_tmp.InitVals(0.0f); // reset for next time around

#ifdef DMEM_COMPILE
  DMem_SyncNet();
#endif
}

void Network::Compute_Act() {
  ThreadUnitCall un_call(&Unit::Compute_Act);
  threads.Run(&un_call, .2f);
}

void Network::Compute_NetinAct() {
  // important note: any algorithms using this for feedforward computation are not 
  // compatible with dmem computation on the network level (over connections)
  // because otherwise the netinput needs to be sync'd at the layer level prior to calling
  // the activation function at the layer level.  Threading should be much faster than
  // dmem in general so this takes precidence.  See BpNetwork::UpdateAfterEdit_impl for 
  // a warning message that should be included.
  ThreadUnitCall un_call(&Unit::Compute_NetinAct);
  threads.Run(&un_call, 1.0f, false, true); // backwards = false, layer_sync=true
}

void Network::Compute_dWt() {
  ThreadUnitCall un_call(&Unit::Compute_dWt);
  threads.Run(&un_call, 1.0f);
}

bool Network::Compute_Weights_Test(int trial_no) {
  if(train_mode == TEST) return false;
  if(wt_update == ON_LINE) return true;
  if(wt_update == BATCH) return false;
  if(wt_update == SMALL_BATCH) {
    int trial_no_eff = trial_no;
#ifdef DMEM_COMPILE
    if(dmem_trl_comm.nprocs > 1) {
      trial_no_eff = ((trial_no_eff-1) / dmem_trl_comm.nprocs) + 1;
      // subtract the 1 that was presumably added to trial_no, then add it back
    }
#endif
    return (trial_no_eff % small_batch_n_eff == 0);
  }
  return false;
}

void Network::Compute_Weights() {
#ifdef DMEM_COMPILE
  DMem_SumDWts(dmem_trl_comm.comm);
#endif
  Compute_Weights_impl();
}

void Network::Compute_Weights_impl() {
  ThreadUnitCall un_call(&Unit::Compute_Weights);
  threads.Run(&un_call, 1.0f);
}

void Network::Compute_SSE(bool unit_avg, bool sqrt) {
  sse = 0.0f;
  int n_vals = 0;
  int lay_vals = 0;
  Layer* l;
  taLeafItr i;
  FOR_ITR_EL(Layer, l, layers., i) {
    if(l->lesioned()) continue;
    sse += l->Compute_SSE(this, lay_vals, unit_avg, sqrt);
    n_vals += lay_vals;
  }
  if(unit_avg && n_vals > 0)
    sse /= (float)n_vals;
  if(sqrt)
    sse = sqrtf(sse);
  cur_sum_sse += sse;
  avg_sse_n++;
  if(sse > cnt_err_tol)
    cur_cnt_err += 1.0;
}

void Network::Compute_TrialStats() {
  Compute_SSE(sse_unit_avg, sse_sqrt);
}

void Network::DMem_ShareTrialData(DataTable* dt, int n_rows) {
#ifdef DMEM_COMPILE
  dt->DMem_ShareRows(dmem_trl_comm.comm, n_rows);
#endif  
}

void Network::Compute_EpochSSE() {
  sum_sse = cur_sum_sse;
  cnt_err = cur_cnt_err;
  if(avg_sse_n > 0) {
    avg_sse = cur_sum_sse / (float)avg_sse_n;
    pct_err = cnt_err / (float)avg_sse_n;
    pct_cor = 1.0f - pct_err;
  }
  
  cur_sum_sse = 0.0f;
  avg_sse_n = 0;
  cur_cnt_err = 0.0f;
}

void Network::Compute_EpochStats() {
#ifdef DMEM_COMPILE
  DMem_ComputeAggs(dmem_trl_comm.comm);
#endif
  Compute_EpochSSE();
}


#ifdef DMEM_COMPILE

void Network::DMem_SyncNRecvCons() {
  if(n_cons <= 0) return;
  if(dmem_sync_level == DMEM_SYNC_LAYER) {
    Layer* l;
    taLeafItr i;
    FOR_ITR_EL(Layer, l, layers., i) {
      if(!l->lesioned())
	l->DMem_SyncNRecvCons();
    }
  }
  else {
    dmem_share_units.Sync(0);
  }
  // need to re-agg all the cons after syncing!
  n_cons = 0;
  Layer* l;
  taLeafItr i;
  FOR_ITR_EL(Layer, l, layers., i) {
    if(l->lesioned()) continue;
    Unit* u;
    taLeafItr i;
    FOR_ITR_EL(Unit, u, l->units., i) {
      n_cons += u->n_recv_cons;
    }
  }
}

void Network::DMem_SyncNet() {
  if(TestError(dmem_sync_level != DMEM_SYNC_NETWORK, "DMem_SyncNet",
	       "attempt to DMem sync at network level, should only be at layer level!")) {
    return;
  }
  dmem_share_units.Sync(1);
}

void Network::DMem_SyncAct() {
  if(TestError(dmem_sync_level != DMEM_SYNC_NETWORK, "DMem_SyncAct",
	       "attempt to DMem sync at network level, should only be at layer level!")) {
    return;
  }
  dmem_share_units.Sync(2);
}

void Network::DMem_DistributeUnits() {
  //  cerr << "proc " << taMisc::dmem_proc << " in distribunits" << endl;
  dmem_nprocs_actual = MIN(dmem_nprocs, taMisc::dmem_nprocs);
  dmem_net_comm.CommSubGpInner(dmem_nprocs_actual);	// network is inner-group
  dmem_trl_comm.CommSubGpOuter(dmem_nprocs_actual);	// trial is outer-group
  dmem_share_units.comm = dmem_net_comm.comm;

  dmem_share_units.Reset();
  bool any_custom_distrib = false;
  Layer* lay;
  taLeafItr li;
  FOR_ITR_EL(Layer, lay, layers., li) {
    if(lay->lesioned()) continue;
    lay->dmem_share_units.comm = dmem_share_units.comm;
    if(dmem_sync_level == DMEM_SYNC_LAYER) {
      lay->DMem_DistributeUnits();
    }
    else {
      if(lay->DMem_DistributeUnits_impl(dmem_share_units))
	any_custom_distrib = true;
    }
  }
  if(dmem_sync_level == DMEM_SYNC_NETWORK) {
    if(!any_custom_distrib) {
      dmem_share_units.DistributeItems(); // use more efficient full distribution
    }
    else
      dmem_share_units.Compile_ShareTypes(); // use custom distribution: just compile it
  }
}

void Network::DMem_UpdtWtUpdt() {
  if(dmem_trl_comm.nprocs > 1) {
    TestWarning(wt_update != SMALL_BATCH, "DMem_UpdtWtUpdt",
		"changing wt_update to SMALL_BATCH because dmem trial nprocs > 1");
    wt_update = SMALL_BATCH;			  // must be small batch
    small_batch_n_eff = small_batch_n / dmem_trl_comm.nprocs; // effective small_batch_n
    if(small_batch_n_eff < 1) small_batch_n_eff = 1;
  }
}

void Network::DMem_InitAggs() {
  dmem_agg_sum.ScanMembers(GetTypeDef(), (void*)this);
  dmem_agg_sum.CompileVars();
}

void Network::DMem_PruneNonLocalCons() {
  if(dmem_nprocs_actual <= 1) return;
  taLeafItr li, ui;
  Unit* u;
  Layer *l;
  FOR_ITR_EL(Layer, l, layers., li) {
    if(l->lesioned()) continue;
    FOR_ITR_EL(Unit, u, l->units., ui) {
      if(u->DMem_IsLocal()) {
	continue;
      }
      // only non-local
      RecvCons* recv_gp;
      int g;
      for (g = 0; g < u->recv.size; g++) {
	recv_gp = (RecvCons *)u->recv.FastEl(g);
	if(recv_gp->GetConSpec()->DMem_AlwaysLocal()) continue;
	for (int sui = recv_gp->cons.size-1; sui >= 0; sui--) {
	  u->DisConnectFrom(recv_gp->Un(sui), NULL);
	}
      }
    }
  }
}

void Network::DMem_SumDWts(MPI_Comm comm) {
  wt_sync_time.StartTimer(false); // don't reset
  static float_Array values;
  static float_Array results;

  int np = 0; MPI_Comm_size(comm, &np);
  if(np <= 1) return;

  values.SetSize(n_cons + n_units);

  int cidx = 0;
  Layer* lay;
  taLeafItr li;
  FOR_ITR_EL(Layer, lay, layers., li) {
    if(lay->lesioned()) continue;
    Unit* un;
    taLeafItr ui;
    FOR_ITR_EL(Unit, un, lay->units., ui) {
      if(un->bias.cons.size)
	values.FastEl(cidx++) = un->bias.Cn(0)->dwt;
      for(int g = 0; g < un->recv.size; g++) {
	RecvCons* cg = un->recv.FastEl(g);
	for(int i = 0;i<cg->cons.size;i++)
	  values.FastEl(cidx++) = cg->Cn(i)->dwt;
      }
    }
  }

  results.SetSize(cidx);
  DMEM_MPICALL(MPI_Allreduce(values.el, results.el, cidx, MPI_FLOAT, MPI_SUM, comm),
	       "Network::SumDWts", "Allreduce");

  cidx = 0;
  FOR_ITR_EL(Layer, lay, layers., li) {
    if(lay->lesioned()) continue;
    Unit* un;
    taLeafItr ui;
    FOR_ITR_EL(Unit, un, lay->units., ui) {
      if(un->bias.cons.size)
	un->bias.Cn(0)->dwt = results.FastEl(cidx++);
      for(int g = 0; g < un->recv.size; g++) {
	RecvCons* cg = un->recv.FastEl(g);
	for(int i = 0;i<cg->cons.size;i++)
	  cg->Cn(i)->dwt = results.FastEl(cidx++);
      }
    }
  }
  wt_sync_time.EndTimer();
}

void Network::DMem_AvgWts(MPI_Comm comm) {
  static float_Array values;
  static float_Array results;

  int np = 0; MPI_Comm_size(comm, &np);
  if(np <= 1) return;

  values.SetSize(n_cons + n_units);

  int cidx = 0;
  Layer* lay;
  taLeafItr li;
  FOR_ITR_EL(Layer, lay, layers., li) {
    if(lay->lesioned()) continue;
    Unit* un;
    taLeafItr ui;
    FOR_ITR_EL(Unit, un, lay->units., ui) {
      if(un->bias.cons.size)
	values.FastEl(cidx++) = un->bias.Cn(0)->wt;
      for(int g = 0; g < un->recv.size; g++) {
	RecvCons* cg = un->recv.FastEl(g);
	for(int i = 0;i<cg->cons.size;i++)
	  values.FastEl(cidx++) = cg->Cn(i)->wt;
      }
    }
  }

  results.SetSize(cidx);
  DMEM_MPICALL(MPI_Allreduce(values.el, results.el, cidx, MPI_FLOAT, MPI_SUM, comm),
		     "Network::AvgWts", "Allreduce");

  float avg_mult = 1.0f / (float)np;
  cidx = 0;
  FOR_ITR_EL(Layer, lay, layers., li) {
    if(lay->lesioned()) continue;
    Unit* un;
    taLeafItr ui;
    FOR_ITR_EL(Unit, un, lay->units., ui) {
      if(un->bias.cons.size)
	un->bias.Cn(0)->wt = avg_mult * results.FastEl(cidx++);
      for(int g = 0; g < un->recv.size; g++) {
	RecvCons* cg = un->recv.FastEl(g);
	for(int i = 0;i<cg->cons.size;i++)
	  cg->Cn(i)->wt = avg_mult * results.FastEl(cidx++);
      }
    }
  }
}

void Network::DMem_ComputeAggs(MPI_Comm comm) {
  dmem_agg_sum.AggVar(comm, MPI_SUM);
}

void Network::DMem_SymmetrizeWts() {
  MPI_Comm comm = dmem_share_units.comm;
  int np = 0; MPI_Comm_size(comm, &np);

  if(np <= 1) return;

  static int_Array unit_idxs;
  static float_Array wt_vals;

  static int_Array all_unit_idxs;
  static float_Array all_wt_vals;

  Layer* lay;
  taLeafItr li;
  FOR_ITR_EL(Layer, lay, layers., li) {
    if(lay->lesioned()) continue;
    if(lay->projections.size == 0) continue;
    Unit* un;
    taLeafItr ui;
    FOR_ITR_EL(Unit, un, lay->units., ui) {
      int gi;
      for(gi=0;gi<un->recv.size;gi++) {
	RecvCons* cg = un->recv[gi];
	if(!cg->GetConSpec()->wt_limits.sym) continue;

	// check for presence of reciprocal connections in the first place..
	Layer* fmlay = cg->prjn->from;
	bool has_recip_prjn = false;
	Projection* fmpj;
	taLeafItr pji;
	FOR_ITR_EL(Projection, fmpj, fmlay->projections., pji) {
	  if(fmpj->from == lay) {
	    has_recip_prjn = true;
	    break;
	  }
	}
	if(!has_recip_prjn) continue; // no sym cons there anyway

	if(un->DMem_IsLocal()) {
	  // I'm local: I recv values from all other procs: each sends unit index and wt
	  all_unit_idxs.Reset();
	  all_wt_vals.Reset();
	  int proc;
	  for(proc = 0; proc < np; proc++) {
	    if(proc == un->dmem_local_proc) continue;
	    // recv the number of connection values obtained
	    int msgsize = 0;
	    MPI_Status status;
	    DMEM_MPICALL(MPI_Recv((void*)&msgsize, 1, MPI_INT, proc, 101, comm, &status),
			 "DMem_SymmetrizeWts", "MPI_Recv msgsize");
	    if(msgsize > 0) {
	      unit_idxs.SetSize(msgsize);
	      wt_vals.SetSize(msgsize);
	      DMEM_MPICALL(MPI_Recv(unit_idxs.el, msgsize, MPI_INT, proc, 102, comm, &status),
			   "DMem_SymmetrizeWts", "MPI_Recv unit_idxs");
	      DMEM_MPICALL(MPI_Recv(wt_vals.el, msgsize, MPI_FLOAT, proc, 103, comm, &status),
			   "DMem_SymmetrizeWts", "MPI_Recv wt_vals");

	      all_unit_idxs.CopyVals(unit_idxs, 0, -1, all_unit_idxs.size);
	      all_wt_vals.CopyVals(wt_vals, 0, -1, all_wt_vals.size);
	    }
	  }
	  // now have all the data collected, to through and get the sym values!
	  for(int i=0;i<cg->cons.size;i++) {
	    Unit* fm = cg->Un(i);
	    int uidx = fm->GetMyLeafIndex();
	    if(uidx < 0) continue;
	    int sidx = all_unit_idxs.FindEl(uidx);
	    if(sidx < 0) continue;
	    cg->Cn(i)->wt = all_wt_vals[sidx];
	  }
	}
	else {
	  // collect my data and send it off!
	  unit_idxs.Reset();
	  wt_vals.Reset();
	  int uni;
	  for(uni=0;uni<fmlay->units.leaves;uni++) {
	    Unit* fm = fmlay->units.Leaf(uni);
	    if(!fm->DMem_IsLocal()) continue;
	    for(int g = 0; g < fm->recv.size; g++) {
	      RecvCons* fmg = fm->recv.FastEl(g);
	      if(fmg->prjn->from != lay) continue;
	      Connection* con = fmg->FindConFrom(un);
	      if(con) {
		unit_idxs.Add(uni);
		wt_vals.Add(con->wt);
	      }
	    }
	  }
	  // send the number, then the data
	  int msgsize = unit_idxs.size;
	  DMEM_MPICALL(MPI_Send((void*)&msgsize, 1, MPI_INT, un->dmem_local_proc, 101, comm),
		       "DMem_SymmetrizeWts", "MPI_Send msgsize");

	  if(msgsize > 0) {
	    DMEM_MPICALL(MPI_Send(unit_idxs.el, msgsize, MPI_INT, un->dmem_local_proc, 102, comm),
			 "DMem_SymmetrizeWts", "MPI_Send unit_idxs");
	    DMEM_MPICALL(MPI_Send(wt_vals.el, msgsize, MPI_FLOAT, un->dmem_local_proc, 103, comm),
			 "DMem_SymmetrizeWts", "MPI_Send wt_vals");
	  }
	}
      }
    }
  }
//   if(taMisc::dmem_debug)
//     cerr << "proc: " << taMisc::dmem_proc << " at sym_wts barrier!" << endl;
//   DMEM_MPICALL(MPI_Barrier(comm),"Network::SymmetrizeWts", "Barrier");
//   if(taMisc::dmem_debug && (taMisc::dmem_proc == 0))
//     cerr << "---------- past sym_wts barrier ---------" << endl;
}

#endif	// DMEM

void Network::Copy_Weights(const Network* src) {
  taMisc::Busy();
  Layer* l, *sl;
  taLeafItr i,si;
  for(l = (Layer*)layers.FirstEl(i), sl = (Layer*)src->layers.FirstEl(si);
      (l) && (sl);
      l = (Layer*)layers.NextEl(i), sl = (Layer*)src->layers.NextEl(si))
  {
    if(!l->lesioned() && !sl->lesioned())
      l->Copy_Weights(sl);
  }
  UpdateAllViews();
  taMisc::DoneBusy();
}

void Network::SaveWeights_strm(ostream& strm, Network::WtSaveFormat fmt) {
  taMisc::Busy();
  if(fmt == NET_FMT) fmt = wt_save_fmt;

  strm << "<Fmt " << GetTypeDef()->GetEnumString("WtSaveFormat", fmt) << ">\n"
       << "<Name " << GetName() << ">\n"
       << "<Epoch " << epoch << ">\n";
  Layer* l;
  taLeafItr i;
  FOR_ITR_EL(Layer, l, layers., i) {
    if(l->lesioned()) continue;
    strm << "<Lay " << l->name << ">\n";
    l->SaveWeights_strm(strm, (RecvCons::WtSaveFormat)fmt);
    strm << "</Lay>\n";
  }
  taMisc::DoneBusy();
}

bool Network::LoadWeights_strm(istream& strm, bool quiet) {
  taMisc::Busy();
  int c = strm.peek();
  if(TestError(c == '#', "LoadWeights_strm",
	       "cannot read old formats from version 3.2 -- must use network save")) {
    return false;
  }
  String tag, val;
  int stat = taMisc::read_tag(strm, tag, val);
  if((stat != taMisc::TAG_GOT) || (tag != "Fmt")) return false;

  String enum_typ_nm;
  RecvCons::WtSaveFormat fmt = (RecvCons::WtSaveFormat)TA_RecvCons.GetEnumVal(val, enum_typ_nm);

  stat = taMisc::read_tag(strm, tag, val);
  if((stat != taMisc::TAG_GOT) || (tag != "Name")) return false;
  // don't set the name!!! this causes more trouble than it is worth!!
//   name = val;

  stat = taMisc::read_tag(strm, tag, val);
  if((stat != taMisc::TAG_GOT) || (tag != "Epoch")) return false;
  epoch = (int)val;

  while(true) {
    stat = taMisc::read_tag(strm, tag, val);
    if(stat != taMisc::TAG_GOT) break;		// *should* break at TAG_END
    if(tag != "Lay") { stat = taMisc::TAG_NONE;  break; } // bumping up against some other tag
    Layer* lay = layers.FindLeafName(val);
    if(lay) {
      stat = lay->LoadWeights_strm(strm, fmt, quiet);
    }
    else {
      TestWarning(!quiet, "LoadWeights", "Layer not found:", val);
      stat = Layer::SkipWeights_strm(strm, fmt, quiet);
    }
    if(stat != taMisc::TAG_END) break;
    stat = taMisc::TAG_NONE;	       // reset so EndTag will definitely read new tag
    RecvCons::LoadWeights_EndTag(strm, "Lay", tag, stat, quiet);
    if(stat != taMisc::TAG_END) break;
  }
  // could try to read end tag but what is the point?

  UpdateAllViews();
  taMisc::DoneBusy();
  return true;
}

void Network::SaveWeights(const String& fname, Network::WtSaveFormat fmt) {
  taFiler* flr = GetSaveFiler(fname, ".wts", true);
  if(flr->ostrm)
    SaveWeights_strm(*flr->ostrm, fmt);
  flr->Close();
  taRefN::unRefDone(flr);
}

bool Network::LoadWeights(const String& fname, bool quiet) {
  taFiler* flr = GetLoadFiler(fname, ".wts", true);
  bool rval = false;
  if(flr->istrm)
    rval = LoadWeights_strm(*flr->istrm, quiet);
  flr->Close();
  taRefN::unRefDone(flr);
  return rval;
}

void Network::LayerZPos_Unitize() {
  int_Array zvals;
  Layer* l;
  taLeafItr i;
  FOR_ITR_EL(Layer, l, layers., i) {
    zvals.AddUnique(l->pos.z);
  }
  zvals.Sort();
  FOR_ITR_EL(Layer, l, layers., i) {
    l->pos.z = zvals.FindEl(l->pos.z); // replace with its index on sorted list..
  }
  UpdateMax();
}

void Network::LayerPos_Cleanup() {
  if (flags & MANUAL_POS) return;
  bool moved = false;
  int n_itr = 0;
  do {
    moved = false;
    for(int i1=0;i1<layers.leaves;i1++) {
      Layer* l1 = layers.Leaf(i1);
      TwoDCoord l1e = (TwoDCoord)l1->pos + (TwoDCoord)l1->act_geom;
      for(int i2 = i1+1; i2<layers.leaves;i2++) {
	Layer* l2 = layers.Leaf(i2);
	TwoDCoord l2e = (TwoDCoord)l2->pos + (TwoDCoord)l2->act_geom;
	if(l2->pos.z != l1->pos.z) continue;
	if(l2->pos.x >= l1->pos.x && l2->pos.x < l1e.x &&
	    l2->pos.y >= l1->pos.y && l2->pos.y < l1e.y) { // l2 starts in l1; move l2 rt/back
	  if(l1e.x - l2->pos.x <= l1e.y - l2->pos.y) {	  // closer to x than y
	    l2->pos.x = l1e.x + 2;
	  }
	  else {
	    l2->pos.y = l1e.y + 2;
	  }
	  l2->DataChanged(DCR_ITEM_UPDATED);
	  moved = true;
	}
	else if(l1->pos.x >= l2->pos.x && l1->pos.x < l2e.x &&
		l1->pos.y >= l2->pos.y && l1->pos.y < l2e.y) { // l1 starts in l2; move l1 rt/back
	  if(l2e.x - l1->pos.x <= l2e.y - l1->pos.y) {	  // closer to x than y
	    l1->pos.x = l2e.x + 2;
	  }
	  else {
	    l1->pos.y = l2e.y + 2;
	  }
	  l1->DataChanged(DCR_ITEM_UPDATED);
	  moved = true;
	}
      }
    }
    n_itr++;
  } while(moved && n_itr < 10);
}


void Network::Compute_LayerDistances() {
  // first reset all 
  Layer* l;
  taLeafItr i;
  FOR_ITR_EL(Layer, l, layers., i) {
    if(l->lesioned()) continue;
    l->dist.fm_input = -1; l->dist.fm_output = -1;
  }

  // next go through and find inputs
  FOR_ITR_EL(Layer, l, layers., i) {
    if(l->lesioned()) continue;
    if(l->layer_type != Layer::INPUT) continue;
    l->dist.fm_input = 0;
    l->PropagateInputDistance();
  }
  // then outputs
  FOR_ITR_EL(Layer, l, layers., i) {
    if(l->lesioned()) continue;
    if(!((l->layer_type == Layer::OUTPUT) || (l->layer_type == Layer::TARGET))) continue;
    l->dist.fm_output = 0;
    l->PropagateOutputDistance();
  }
}

void Network::Compute_PrjnDirections() {
  Compute_LayerDistances();	// required data
  Layer* l;
  taLeafItr i;
  FOR_ITR_EL(Layer, l, layers., i) {
    if(l->lesioned()) continue;
    l->Compute_PrjnDirections();
  }
}

void Network::SetUnitNames(bool force_use_unit_names) {
  Layer* l;
  taLeafItr i;
  FOR_ITR_EL(Layer, l, layers., i) {
    if(!l->lesioned())
      l->SetUnitNames(force_use_unit_names);
  }
  UpdateAllViews();
}

void Network::SetUnitNamesFromDataTable(DataTable* undt, int max_unit_chars, 
					bool propagate_names) {
  if(TestError(!undt || undt->rows < 1, "SetUnitNamesFromDataTable", "null unit names table or doesn't have 1 or more rows!")) {
    return;
  }

  for(int i=0;i<undt->cols();i++) {
    DataCol* ndc = undt->data.FastEl(i);
    Layer* lay = (Layer*)layers.FindLeafName(ndc->name);
    if(!lay) continue;
    lay->SetUnitNamesFromDataCol(ndc, max_unit_chars);
  }
  if(propagate_names)
    GetLocalistName();	// propagate
  return;
}

void Network::GetUnitNames(bool force_use_unit_names) {
  Layer* l;
  taLeafItr i;
  FOR_ITR_EL(Layer, l, layers., i) {
    if(!l->lesioned())
      l->GetUnitNames(force_use_unit_names);
  }
  UpdateAllViews();
}

void Network::GetLocalistName() {
  Layer* l;
  taLeafItr i;
  FOR_ITR_EL(Layer, l, layers., i) {
    if(!l->lesioned())
      l->GetLocalistName();
  }
  UpdateAllViews();
}

bool Network::SnapVar() {
  SimpleMathSpec sm;
  sm.opr = SimpleMathSpec::NONE;
  return Snapshot("", sm, false); // empty var is retrieved
}

bool Network::SnapAnd() {
  SimpleMathSpec sm;
  sm.opr = SimpleMathSpec::MIN;
  return Snapshot("", sm, true); // empty var is retrieved
}

bool Network::SnapOr() {
  SimpleMathSpec sm;
  sm.opr = SimpleMathSpec::MAX;
  return Snapshot("", sm, true); // empty var is retrieved
}

bool Network::SnapThresh(float thresh_val) {
  SimpleMathSpec sm;
  sm.opr = SimpleMathSpec::THRESH;
  sm.arg = thresh_val;
  sm.lw = 0.0;
  sm.hi = 1.0;
  return Snapshot("", sm, false);
}

// Network::GetViewVar is in netstru_qtso.cpp

bool Network::Snapshot(const String& variable, SimpleMathSpec& math_op, bool arg_is_snap) {
  String var = variable;
  if(var.empty()) {
    var = GetViewVar();
    if(TestError(var.empty(), "Snapshot", "No view variable found!"))
      return false;
  }
  Layer* l;
  taLeafItr i;
  FOR_ITR_EL(Layer, l, layers., i) {
    if(l->lesioned()) continue;
    if(!l->Snapshot(var, math_op, arg_is_snap)) return false;
  }
  UpdateAllViews();
  return true;
}

void Network::MonitorVar(NetMonitor* net_mon, const String& variable) {
  if(!net_mon) return;
  net_mon->AddObject(this, variable);
}

void Network::RemoveMonitors() {
  if (!proj) return;
  TokenSpace& ts = TA_NetMonitor.tokens;
  for (int i = 0; i < ts.size; ++i) {
    NetMonitor* nm = (NetMonitor*)ts.FastEl(i);
    if(nm->network.ptr() != this) continue;
    nm->RemoveMonitors();
  }
}
void Network::UpdateMonitors() {
  if (!proj) return;
  TokenSpace& ts = TA_NetMonitor.tokens;
  for (int i = 0; i < ts.size; ++i) {
    NetMonitor* nm = (NetMonitor*)ts.FastEl(i);
    if(nm->network.ptr() != this) continue;
    nm->UpdateDataTable();
  }
}

void Network::NetControlPanel(SelectEdit* editor, const String& extra_label, const String& sub_gp_nm) {
  if(!editor) {
    taProject* proj = GET_MY_OWNER(taProject);
    if(TestError(!proj, "NetControlPanel", "cannot find project")) return;
    editor = (SelectEdit*)proj->edits.New(1);
  }
  TypeDef* td = GetTypeDef();
  for(int i=td->members.size-1; i>=0; i--) {
    MemberDef* md = td->members[i];
    if(!md->HasOption("VIEW")) continue;
    // filter by current guys..
    if(HasUserData(md->name) && !GetUserDataAsBool(md->name)) continue;
    editor->SelectMember(this, md, extra_label, "", sub_gp_nm);
  }
}

void Network::TransformWeights(const SimpleMathSpec& trans) {
  taMisc::Busy();
  Layer* l;
  taLeafItr i;
  FOR_ITR_EL(Layer, l, layers., i) {
    if(!l->lesioned())
      l->TransformWeights(trans);
  }
  UpdateAllViews();
  taMisc::DoneBusy();
}

void Network::AddNoiseToWeights(const Random& noise_spec) {
  taMisc::Busy();
  Layer* l;
  taLeafItr i;
  FOR_ITR_EL(Layer, l, layers., i) {
    if(!l->lesioned())
      l->AddNoiseToWeights(noise_spec);
  }
  UpdateAllViews();
  taMisc::DoneBusy();
}

int Network::PruneCons(const SimpleMathSpec& pre_proc,
			  Relation::Relations rel, float cmp_val)
{
  taMisc::Busy();
  StructUpdate(true);
  int rval = 0;
  Layer* l;
  taLeafItr i;
  FOR_ITR_EL(Layer, l, layers., i) {
    if(!l->lesioned())
      rval += l->PruneCons(pre_proc, rel, cmp_val);
  }
  StructUpdate(false);
  taMisc::DoneBusy();
  UpdtAfterNetMod();
  return rval;
}

int Network::ProbAddCons(float p_add_con, float init_wt) {
  taMisc::Busy();
  StructUpdate(true);
  int rval = 0;
  Layer* l;
  taLeafItr i;
  FOR_ITR_EL(Layer, l, layers., i) {
    if(!l->lesioned())
      rval += l->ProbAddCons(p_add_con, init_wt);
  }
  StructUpdate(false);
  taMisc::DoneBusy();
  UpdtAfterNetMod();
  return rval;
}

int Network::LesionCons(float p_lesion, bool permute) {
  taMisc::Busy();
  StructUpdate(true);
  int rval = 0;
  Layer* l;
  taLeafItr i;
  FOR_ITR_EL(Layer, l, layers., i) {
    if(!l->lesioned())
      rval += l->LesionCons(p_lesion, permute);
  }
  StructUpdate(false);
  taMisc::DoneBusy();
  UpdtAfterNetMod();
  return rval;
}

int Network::LesionUnits(float p_lesion, bool permute) {
  taMisc::Busy();
  StructUpdate(true);
  int rval = 0;
  Layer* l;
  taLeafItr i;
  FOR_ITR_EL(Layer, l, layers., i) {
    if(!l->lesioned())
      rval += l->LesionUnits(p_lesion, permute);
  }
  StructUpdate(false);
  taMisc::DoneBusy();
  UpdtAfterNetMod();
  return rval;
}

void Network::UpdateMax() {
  max_size.x = 1;  max_size.y = 1;  max_size.z = 1;

  Layer* l;
  taLeafItr i;
  TDCoord l_pos; // for abs_pos
  FOR_ITR_EL(Layer, l, layers., i) {
    l->GetAbsPos(l_pos);
    max_size.z = MAX(max_size.z, 1 + l_pos.z);
    if(l->Iconified()) {
      max_size.x = MAX(max_size.x, l_pos.x + 1);
      max_size.y = MAX(max_size.y, l_pos.y + 1);
    }
    else {
      max_size.x = MAX(max_size.x, l->act_geom.x + l_pos.x);
      max_size.y = MAX(max_size.y, l->act_geom.y + l_pos.y);
    }
  }
}

void Network::TwoD_Or_ThreeD(LayerLayout lo){
  lay_layout = lo;
  Layer * lay;
  taLeafItr j;
  FOR_ITR_EL(Layer, lay, layers., j){
    lay->SetDefaultPos();
  }
}

bool Network::UpdateUnitSpecs(bool force) {
  bool rval = true;
  Layer * lay;
  taLeafItr j;
  FOR_ITR_EL(Layer, lay, layers., j){
    if(!lay->UpdateUnitSpecs(force))
      rval = false;
  }
  return rval;
}

bool Network::UpdateConSpecs(bool force) {
  bool rval = true;
  Layer * lay;
  taLeafItr j;
  FOR_ITR_EL(Layer, lay, layers., j){
    if(!lay->UpdateConSpecs(force))
      rval = false;
  }
  return rval;
}

bool Network::UpdateAllSpecs(bool force) {
  bool rval = UpdateUnitSpecs(force);
  if(!UpdateConSpecs(force))
    rval = false;
  return rval;
}

void Network::ReplaceSpecs(BaseSpec* old_sp, BaseSpec* new_sp) {
  if(old_sp->InheritsFrom(&TA_UnitSpec))
    ReplaceUnitSpec((UnitSpec*)old_sp, (UnitSpec*)new_sp);
  else if(old_sp->InheritsFrom(&TA_ConSpec))
    ReplaceConSpec((ConSpec*)old_sp, (ConSpec*)new_sp);
  else if(old_sp->InheritsFrom(&TA_ProjectionSpec))
    ReplacePrjnSpec((ProjectionSpec*)old_sp, (ProjectionSpec*)new_sp);
  else if(old_sp->InheritsFrom(&TA_LayerSpec))
    ReplaceLayerSpec((LayerSpec*)old_sp, (LayerSpec*)new_sp);
  
  ReplaceSpecs_Gp(old_sp->children, new_sp->children);
}

void Network::ReplaceSpecs_Gp(const BaseSpec_Group& old_spg, BaseSpec_Group& new_spg) {
  taLeafItr spo, spn;
  BaseSpec* old_sp, *new_sp;
  for(old_sp = (BaseSpec*)old_spg.FirstEl(spo), new_sp = (BaseSpec*)new_spg.FirstEl(spn);
      old_sp && new_sp;
      old_sp = (BaseSpec*)old_spg.NextEl(spo), new_sp = (BaseSpec*)new_spg.NextEl(spn)) {
    ReplaceSpecs(old_sp, new_sp);
  }
}

int Network::ReplaceUnitSpec(UnitSpec* old_sp, UnitSpec* new_sp) {
  int nchg = 0;
  Layer* l;
  taLeafItr i;
  FOR_ITR_EL(Layer, l, layers., i) {
    if(!l->lesioned())
      nchg += l->ReplaceUnitSpec(old_sp, new_sp);
  }
  return nchg;
}

int Network::ReplaceConSpec(ConSpec* old_sp, ConSpec* new_sp) {
  int nchg = 0;
  Layer* l;
  taLeafItr i;
  FOR_ITR_EL(Layer, l, layers., i) {
    if(!l->lesioned())
      nchg += l->ReplaceConSpec(old_sp, new_sp);
  }
  return nchg;
}

int Network::ReplacePrjnSpec(ProjectionSpec* old_sp, ProjectionSpec* new_sp) {
  int nchg = 0;
  Layer* l;
  taLeafItr i;
  FOR_ITR_EL(Layer, l, layers., i) {
    if(!l->lesioned())
      nchg += l->ReplacePrjnSpec(old_sp, new_sp);
  }
  return nchg;
}

int Network::ReplaceLayerSpec(LayerSpec* old_sp, LayerSpec* new_sp) {
  int nchg = 0;
  Layer* l;
  taLeafItr i;
  FOR_ITR_EL(Layer, l, layers., i) {
    if(!l->lesioned())
      nchg += l->ReplaceLayerSpec(old_sp, new_sp);
  }
  return nchg;
}

void Network::WeightsToTable(DataTable* dt, Layer* recv_lay, Layer* send_lay)
{
  if(recv_lay == NULL) return;
  recv_lay->WeightsToTable(dt, send_lay);
}

void Network::VarToTable(DataTable* dt, const String& variable) {
  if (!dt) {
    taProject* proj = GET_MY_OWNER(taProject);
    dt = proj->GetNewAnalysisDataTable(name + "_Var_" + variable, true);
  }

  NetMonitor nm;
  taBase::Own(nm, this);
  nm.AddNetwork(this, variable);
  nm.items[0]->max_name_len = 20; // allow long names
  nm.SetDataNetwork(dt, this);
  nm.UpdateDataTable();
  dt->AddBlankRow();
  nm.GetMonVals();
  dt->WriteClose();
}

void Network::ProjectUnitWeights(Unit* src_u, float wt_thr, bool swt) {
  if(!src_u) return;

  // first initialize all vars
  Layer* lay;
  taLeafItr li;
  FOR_ITR_EL(Layer, lay, layers., li) {
    if(lay->lesioned()) continue;
    lay->ClearLayerFlag(Layer::PROJECT_WTS_NEXT);
    lay->ClearLayerFlag(Layer::PROJECT_WTS_DONE);
    Unit* u;
    taLeafItr ui;
    FOR_ITR_EL(Unit, u, lay->units., ui) {
      u->wt_prjn = u->tmp_calc1 = 0.0f;
    }
  }

  // do initial propagation
  for(int g = 0; g < (swt ? src_u->send.size : src_u->recv.size); g++) {
    taOBase* cg = (swt ? (taOBase*)src_u->send.FastEl(g) : (taOBase*)src_u->recv.FastEl(g));
    Projection* prjn = (swt ? ((SendCons*)cg)->prjn : ((RecvCons*)cg)->prjn);
    if(!prjn) continue;
    Layer* slay = (swt ? prjn->layer : prjn->from);

    if(slay->lesioned() || (prjn->from.ptr() == prjn->layer)) continue; // no self prjns!!
    slay->SetLayerFlag(Layer::PROJECT_WTS_NEXT);

    if(swt) {
      SendCons* scg = (SendCons*)cg;
      for(int ci = 0; ci < scg->cons.size; ci++) {
	float wtv = scg->Cn(ci)->wt;
	Unit* su = scg->Un(ci);
	su->wt_prjn += wtv;
	su->tmp_calc1 += 1.0f;	// sum to 1
      }
    }
    else {
      RecvCons* scg = (RecvCons*)cg;
      for(int ci = 0; ci < scg->cons.size; ci++) {
	float wtv = scg->Cn(ci)->wt;
	Unit* su = scg->Un(ci);
	su->wt_prjn += wtv;
	su->tmp_calc1 += 1.0f;	// sum to 1
      }
    }
  }

  // now it is just the same loop until there are no more guys!
  bool got_some = false;
  do {
    got_some = false;
    Layer* lay;
    taLeafItr li;
    FOR_ITR_EL(Layer, lay, layers., li) {
      if(lay->lesioned() || !lay->HasLayerFlag(Layer::PROJECT_WTS_NEXT)) continue;

      lay->SetLayerFlag(Layer::PROJECT_WTS_DONE); // we're done!
      
      // first normalize the weights on this guy
      float abs_max = 0.0f;
      Unit* u;
      taLeafItr ui;
      FOR_ITR_EL(Unit, u, lay->units., ui) {
	if(u->tmp_calc1 > 0.0f)
	  u->wt_prjn /= u->tmp_calc1;
	abs_max = MAX(abs_max, fabsf(u->wt_prjn));
      }

      if(abs_max == 0.0f) abs_max = 1.0f;

      FOR_ITR_EL(Unit, u, lay->units., ui) {
	u->wt_prjn /= abs_max;	// normalize
	if(u->wt_prjn < wt_thr) continue; // bail

	// propagate!
	for(int g = 0; g < (swt ? u->send.size : u->recv.size); g++) {
	  taOBase* cg = (swt ? (taOBase*)u->send.FastEl(g) : (taOBase*)u->recv.FastEl(g));
	  Projection* prjn = (swt ? ((SendCons*)cg)->prjn : ((RecvCons*)cg)->prjn);
	  if(!prjn) continue;
	  Layer* slay = (swt ? prjn->layer : prjn->from);

	  if(slay->lesioned() || (prjn->from.ptr() == prjn->layer) ||
	     slay->HasLayerFlag(Layer::PROJECT_WTS_DONE)) continue;
	  slay->SetLayerFlag(Layer::PROJECT_WTS_NEXT); // next..
	  got_some = true;			       // keep going..

	  if(swt) {
	    SendCons* scg = (SendCons*)cg;
	    for(int ci = 0; ci < scg->cons.size; ci++) {
	      float wtv = scg->Cn(ci)->wt;
	      Unit* su = scg->Un(ci);
	      su->wt_prjn += u->wt_prjn * wtv;
	      su->tmp_calc1 += u->wt_prjn;
	    }
	  }
	  else {
	    RecvCons* scg = (RecvCons*)cg;
	    for(int ci = 0; ci < scg->cons.size; ci++) {
	      float wtv = scg->Cn(ci)->wt;
	      Unit* su = scg->Un(ci);
	      su->wt_prjn += u->wt_prjn * wtv;
	      su->tmp_calc1 += u->wt_prjn;
	    }
	  }
	}
      }
    }
  } while(got_some);
}

// new monitor is in emergent_project.cc

////////////////////////////////////////////
//  	Wizard functions
////////////////////////////////////////////

bool Network::nw_itm_def_arg = false;

BaseSpec_Group* Network::FindMakeSpecGp(const char* nm, bool& nw_itm) {
  BaseSpec_Group* gp = (BaseSpec_Group*)specs.gp.FindName(nm);
  nw_itm = false;
  if(gp == NULL) {
    gp = (BaseSpec_Group*)specs.gp.New(1);
    gp->name = nm;
    nw_itm = true;
  }
  return gp;
}

BaseSpec* Network::FindMakeSpec(const char* nm, TypeDef* td, bool& nw_itm) {
  return (BaseSpec*)specs.FindMakeSpec(nm, td, nw_itm);
}

BaseSpec* Network::FindSpecName(const char* nm) {
  BaseSpec* rval = (BaseSpec*)specs.FindSpecName(nm);
  TestError(!rval, "FindSpecName", "could not find spec named:", nm);
  return rval;
}

BaseSpec* Network::FindSpecType(TypeDef* td) {
  BaseSpec* rval = (BaseSpec*)specs.FindSpecType(td);
  TestError(!rval, "FindSpecType", "could not find spec of type:", td->name);
  return rval;
}

Layer* Network::FindMakeLayer(const char* nm, TypeDef* td, bool& nw_itm, const char* alt_nm) {
  nw_itm = false;
  Layer* lay = (Layer*)layers.FindName(nm);
  if((lay == NULL) && (alt_nm)) {
    lay = (Layer*)layers.FindName(alt_nm);
    if(lay) lay->name = nm;
  }
  if(lay == NULL) {
    lay = (Layer*)layers.NewEl(1, td);
    lay->name = nm;
    nw_itm = true;
  }
  if((td) && !lay->InheritsFrom(td)) {
    layers.RemoveEl(lay);
    lay = (Layer*)layers.NewEl(1, td);
    lay->name = nm;
    nw_itm = true;
  }
  return lay;
}

Projection* Network::FindMakePrjn(Layer* recv, Layer* send, ProjectionSpec* ps, ConSpec* cs, bool& nw_itm) 
{
  nw_itm = false; // default, esp for early return
  Projection* use_prj = NULL;
  int i;
  for(i=0;i<recv->projections.size;i++) {
    Projection* prj = (Projection*)recv->projections[i];
    if(prj->from.ptr() == send) {
      if((ps == NULL) && (cs == NULL)) {
	return prj;
      }
      if((ps) && (prj->spec.spec.ptr() != ps)) {
	use_prj = prj;
	break;
      }
      if((cs) && (prj->con_spec.spec.ptr() != cs)) {
	use_prj = prj;
	break;
      }
      return prj;
    }
  }
  if (use_prj == NULL) {
    nw_itm = true;
    use_prj = (Projection*)recv->projections.NewEl(1);
  }
  use_prj->SetCustomFrom(send);
  if(ps) {
    use_prj->spec.SetSpec(ps);
  }
  if(cs) {
    use_prj->SetConType(cs->min_obj_type);
    use_prj->con_spec.SetSpec(cs);
  }
  return use_prj;
}

Projection* Network::FindMakePrjnAdd(Layer* recv, Layer* send, ProjectionSpec* ps, ConSpec* cs, bool& nw_itm) {
  int i;
  for(i=0;i<recv->projections.size;i++) {
    Projection* prj = (Projection*)recv->projections[i];
    if((prj->from.ptr() == send)
       && ((ps == NULL) || (prj->spec.spec.ptr() == ps) ||
	   (prj->spec.spec->InheritsFrom(TA_FullPrjnSpec) &&
	    ps->InheritsFrom(TA_FullPrjnSpec)))
       && ((cs == NULL) || (prj->con_spec.spec.ptr() == cs))) {
      nw_itm = false;
      return prj;
    }
  }
  nw_itm = true;
  Projection* prj = (Projection*)recv->projections.NewEl(1);
  prj->SetCustomFrom(send);
  if(ps) {
    prj->spec.SetSpec(ps);
  }
  if(cs) {
    prj->SetConType(cs->min_obj_type);
    prj->con_spec.SetSpec(cs);
  }
  return prj;
}

Projection* Network::FindMakeSelfPrjn(Layer* recv, ProjectionSpec* ps, ConSpec* cs, bool& nw_itm) {
  Projection* use_prj = NULL;
  int i;
  for(i=0;i<recv->projections.size;i++) {
    Projection* prj = (Projection*)recv->projections[i];
    if(prj->from.ptr() == recv) {
      if((ps == NULL) && (cs == NULL)) {
	nw_itm = false;
	return prj;
      }
      if((ps) && (prj->spec.spec.ptr() != ps)) {
	use_prj = prj;
	break;
      }
      if((cs) && (prj->con_spec.spec.ptr() != cs)) {
	use_prj = prj;
	break;
      }
      nw_itm = false;
      return prj;
    }
  }
  nw_itm = true;
  if(use_prj == NULL)
    use_prj = (Projection*)recv->projections.NewEl(1);
  use_prj->from_type = Projection::SELF;
//   taBase::SetPointer((taBase**)&(use_prj->from), recv);
  use_prj->from = recv;
  if(ps)
    use_prj->spec.SetSpec(ps);
  if(cs)
    use_prj->con_spec.SetSpec(cs);
  use_prj->DataChanged(DCR_ITEM_UPDATED);
  return use_prj;
}

Projection* Network::FindMakeSelfPrjnAdd(Layer* recv, ProjectionSpec* ps, ConSpec* cs, bool& nw_itm) {
  int i;
  for(i=0;i<recv->projections.size;i++) {
    Projection* prj = (Projection*)recv->projections[i];
    if((prj->from.ptr() == recv)
       && ((ps == NULL) || (prj->spec.spec.ptr() == ps))
       && ((cs == NULL) || (prj->con_spec.spec.ptr() == cs))) {
      nw_itm = false;
      return prj;
    }
  }
  nw_itm = true;
  Projection* prj = (Projection*)recv->projections.NewEl(1);
  prj->from_type = Projection::SELF;
//   taBase::SetPointer((taBase**)&(prj->from), recv);
  prj->from = recv;
  if(ps)
    prj->spec.SetSpec(ps);
  if(cs)
    prj->con_spec.SetSpec(cs);
  prj->DataChanged(DCR_ITEM_UPDATED);
  return prj;
}

bool Network::RemovePrjn(Layer* recv, Layer* send, ProjectionSpec* ps, ConSpec* cs) {
  int i;
  for(i=recv->projections.size-1;i>=0;i--) {
    Projection* prj = (Projection*)recv->projections[i];
    if((prj->from.ptr() == send)
       && ((ps == NULL) || (prj->spec.spec.ptr() == ps) ||
	   (prj->spec.spec->InheritsFrom(TA_FullPrjnSpec) &&
	    ps->InheritsFrom(TA_FullPrjnSpec)))
       && ((cs == NULL) || (prj->con_spec.spec.ptr() == cs))) {
      recv->projections.RemoveEl(prj);
      return true;
    }
  }
  return false;
}



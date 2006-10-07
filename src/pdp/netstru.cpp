// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/PDP++
//
//   TA/PDP++ is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   TA/PDP++ is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.



// netstru.cc
// Functions specific to structures defined in netstru.h


#include "netstru.h"
#include "pdpshell.h"
//#include "css_qt.h"		// for the cssiSession
#include "ta_data.h"
#include "ta_filer.h"

#ifdef TA_GUI
#include "ta_qt.h"
#include "ta_qtgroup.h"
#include "netstru_qtso.h"
#include "pdplog_qtso.h"
#endif

#ifdef DMEM_COMPILE
#include <mpi.h>
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

void SchedItem::Copy_(const SchedItem& cp) {
  start_ctr = cp.start_ctr;
  start_val = cp.start_val;
  duration = cp.duration;
  step = cp.step;
}

void Schedule::Initialize() {
  last_ctr = -1;
  default_val = 1.0f;
  cur_val = 0.0f;
  interpolate = true;
  SetBaseType(&TA_SchedItem);
}

void Schedule::Copy_(const Schedule& cp) {
  last_ctr = cp.last_ctr;
  default_val = cp.default_val;
  cur_val = cp.cur_val;
  interpolate = cp.interpolate;
}

void Schedule::UpdateAfterEdit() {
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
    temp.Remove(0);
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
  taList<SchedItem>::UpdateAfterEdit();
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
    if(u2->bias_spec.spec == old) {
      u2->bias_spec.SetSpec(nw); // update to new
      cnt++;
    }
    if(u2->children.leaves > 0)
      cnt += conspec_repl_bias_ptr(u2, old, nw);
  }
  return cnt;
}

void Connection::UpdateAfterEdit() {
  taBase::UpdateAfterEdit();
}

bool Connection::ChangeMyType(TypeDef*) {
  taMisc::Error("Cannot change type of connections -- change type setting in projection and reconnect network instead");
  return false;
}

void ConSpec::Initialize() {
  min_obj_type = &TA_Con_Group;
  min_con_type = &TA_Connection;
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
  Network* net = (Network *) GET_MY_OWNER(Network);
  if((net) && !net->deleting) {
    ConSpec* rsp = (ConSpec*)net->specs.FindSpecTypeNotMe(GetTypeDef(), this);
    if(rsp == NULL) {
      rsp = (ConSpec*)net->specs.FindSpecInheritsNotMe(&TA_ConSpec, this);
    }
    if(rsp) {
      int cnt = net->ReplaceConSpec(this, rsp);
      if(cnt > 0) {
	taMisc::Error("Warning: ConSpec",this->GetPath(),"was used in the network, replaced with",rsp->GetPath());
      }

      // now go through specs!
      BaseSpec* sp;
      taLeafItr si;
      FOR_ITR_EL(BaseSpec, sp, net->specs., si) {
	if(!sp->InheritsFrom(TA_UnitSpec)) continue;
	UnitSpec* us = (UnitSpec*)sp;
	if(us->bias_spec.spec == this) {
	  us->bias_spec.SetSpec(rsp); // update to new
	}
	if(us->children.leaves > 0)
	  conspec_repl_bias_ptr(us, this, rsp);
      }
    }
    else {
      taMisc::Error("Warning: Deleting ConSpec",this->GetPath(),"and couldn't find replacement - network will have NULL spec and crash!");
    }
  }
  BaseSpec::CutLinks();
}

void ConSpec::Copy_(const ConSpec& cp) {
  //  min_con_type = cp.min_con_type;  // don't do this -- could be going between types
  rnd = cp.rnd;
  wt_limits = cp.wt_limits;
}

int ConSpec::UseCount() {
  Network* net = (Network *) GET_MY_OWNER(Network);
  if(!net) return -1;
  int cnt = net->ReplaceConSpec(this, this);

  // now go through specs!
  BaseSpec* sp;
  taLeafItr si;
  FOR_ITR_EL(BaseSpec, sp, net->specs., si) {
    if(!sp->InheritsFrom(TA_UnitSpec)) continue;
    UnitSpec* us = (UnitSpec*)sp;
    if(us->bias_spec.spec == this)
      cnt++;
    if(us->children.leaves > 0)
      cnt += conspec_repl_bias_ptr(us, this, this);
  }
  return cnt;
}

void ConSpec::InitWeights() {
  Network* net = (Network *) GET_MY_OWNER(Network);
  if (!net) return;
  net->InitWtState();
  net->UpdateAllViews();
}

bool ConSpec::CheckObjectType_impl(TAPtr obj) {
  if(obj->InheritsFrom(TA_Con_Group)) {
    if((obj->GetOwner()) && obj->GetOwner()->InheritsFrom(TA_Unit))
      return true;		// don't do checking on 1st con group in unit..
  }
  else if(obj->InheritsFrom(TA_Connection)) {
    if(!obj->InheritsFrom(min_con_type))
      return false;
    return true;
  }
  else if(obj->InheritsFrom(TA_Projection)) {
    if(!((Projection*)obj)->con_type->InheritsFrom(min_con_type))
      return false;
    return true;
  }
  return BaseSpec::CheckObjectType_impl(obj);
}

void ConSpec::ApplySymmetry(Con_Group* cg, Unit* ru) {
  if(!wt_limits.sym) return;
  Connection* rc, *su_rc;
  int i;
  for(i=0; i<cg->size;i++) {
    rc = cg->Cn(i);
    su_rc = cg->FindRecipRecvCon(cg->Un(i), ru);
    if(su_rc)
      su_rc->wt = rc->wt;	// set other's weight to ours (otherwise no random..)
  }
}

void Con_Group::Initialize() {
  units.SetBaseType(&TA_Unit);
  prjn = NULL;
  other_idx = -1;
  own_cons = false;
  SetBaseType(&TA_Connection);
}

void Con_Group::InitLinks() {
  taBase_Group::InitLinks();
  spec.SetDefaultSpec(this);
  taBase::Own(units, this);
}

void Con_Group::CutLinks() {
  units.CutLinks();
  taBase::DelPointer((TAPtr*)&prjn);
  spec.CutLinks();
  taBase_Group::CutLinks();
}

void Con_Group::Copy(const Con_Group& cp) {
  Copy_Common(cp);		// just copy values across common connections
  Copy_(cp);			// then copy our stuff.
  gp.Copy(cp.gp);		// copy the sub-groups
}

void Con_Group::Copy_(const Con_Group& cp) {
  if(!cp.name.empty())
    name = cp.name;
  el_base = cp.el_base;
  el_typ = cp.el_typ;
  el_def = cp.el_def;
  spec = cp.spec;

  // stuff that doesn't get copied!
  // taBase::SetPointer((TAPtr*)&prjn, cp.prjn);
  // other_idx = cp.other_idx;
  // own_cons = cp.own_cons;
}

//static TimeUsed debug_time;

void Con_Group::CopyNetwork(Network* net, Network* cn, Con_Group* cp) {
  MemberDef* md;
  String path;
  if(!cp->own_cons) {		// just a link (i.e., send group)
    if(size > 0) {
      Reset();			// get rid of any existing actual cons
      units.Reset();
    }
    // don't copy any elements --- need to get pointers to *our* connections via ReConnect_Load
    own_cons = cp->own_cons;
    other_idx = cp->other_idx;
  }
  else {			// we actually own them
    if(!own_cons && (size > 0)) {
      Reset();
    }
    own_cons = cp->own_cons;
    other_idx = cp->other_idx;
    units.Reset();		// always reset the units, cuz they're going to be wrong..
    taList_impl::Copy(*cp);	// do a standard copy of the connections only
    int i;
    for(i=0; i<cp->units.size; i++) {
      Unit* au = cp->Un(i);
      if(au) {
	Layer* au_lay = GET_OWNER(au,Layer);
	if(au_lay->own_net) {
	  int lidx = au_lay->own_net->layers.FindLeaf(au_lay);
	  int uidx = au_lay->units.FindLeaf(au);
	  if((lidx >= 0) && (uidx >= 0)) {
	    Layer* nw_lay = (Layer*)net->layers.Leaf(lidx);
	    if(nw_lay) {
	      Unit* nw_un = (Unit*)nw_lay->units.Leaf(uidx);
	      if(nw_un)
		units.Link(nw_un);
	    }
	  }
	}
      }
    }
  }
  if(cp->prjn) {
    path = cp->prjn->GetPath(NULL, cn);
    Projection* nw_prjn = (Projection*)net->FindFromPath(path, md);
    if(nw_prjn)
      taBase::SetPointer((TAPtr*)&prjn, nw_prjn);
  }
}

void Con_Group::CopyPtrs(Con_Group* cp) {
  if(!cp->own_cons) {		// just a link (i.e., send group)
    if(size > 0) {
      Reset();			// get rid of any existing actual cons
      units.Reset();
    }
    // don't copy any elements --- need to get pointers to *our* connections via ReConnect_Load
    own_cons = cp->own_cons;
    other_idx = cp->other_idx;
    units.Borrow(cp->units);
    Borrow(*cp);
  }
  else {			// we actually own them
    if(!own_cons && (size > 0)) {
      Reset();
    }
    own_cons = cp->own_cons;
    other_idx = cp->other_idx;
    units.Reset();
    taList_impl::Copy(*cp);	// do a standard copy of the connections only
    units.Borrow(cp->units);
  }
  taBase::SetPointer((TAPtr*)&prjn, cp->prjn);
}

bool Con_Group::ChangeMyType(TypeDef*) {
  taMisc::Error("Cannot change type of con_groups -- change type setting in projection and reconnect network instead");
  return false;
}

void Con_Group::UpdateAfterEdit() {
  taBase_Group::UpdateAfterEdit();
  spec.CheckSpec();
}

Connection* Con_Group::NewCon(TypeDef* typ, Unit* un) {
  units.Link(un);
  Connection* rval;
  if(size > units.size-1)	// connections were preallocated
    rval = Cn(units.size-1);
  else
    rval = (Connection*)taBase_Group::NewEl(1, typ);
  return rval;
}

void Con_Group::LinkCon(Connection* cn, Unit* un) {
  units.Link(un);
  if(size > units.size-1)	// connections were preallocated
    ReplaceLink(units.size-1, cn);
  else
    Link(cn);
}

bool Con_Group::RemoveCon(Unit* un) {
  int idx;
  if((idx = units.Find(un)) < 0)
    return false;
  return Remove(idx);
}

void Con_Group::AllocCon(int no, TypeDef* typ) {
  if(no <= 0) return;
  Alloc(no);
  taBase_Group::NewEl(no, typ);	// subvert the redefinition, call old version of New
}

Connection* Con_Group::FindConFrom(Unit* un, int& idx) const {
  if((idx = units.Find(un)) < 0)
    return NULL;
  return (Connection*)SafeEl(idx);
}

void Con_Group::Alloc(int sz) {
  units.Alloc(sz);
  taBase_Group::Alloc(sz);
}

TAPtr Con_Group::New(int no, TypeDef* typ) {
  if(no == 0) {
#ifdef TA_GUI
    if(taMisc::gui_active)
      return gpiGroupNew::New(this,typ);
#endif
    return NULL;
  }
  if(typ == NULL)
    typ = el_typ;

  TAPtr rval = taBase_Group::New(no, typ);

  if(!typ->InheritsFrom(&TA_taGroup_impl)) {
    int i;
    for(i=0; i <no; i++)
      units.Add(NULL);
  }
  return rval;
}
TAPtr Con_Group::NewEl(int no, TypeDef* typ) {
  if(no == 0) {
#ifdef TA_GUI
    if(taMisc::gui_active)
      return gpiGroupNew::New(this,typ);
#endif
    return NULL;
  }
  int i;
  for(i=0; i <no; i++)
    units.Add(NULL);
  return taBase_Group::NewEl(no, typ);
}

bool Con_Group::Remove(int i) {
  units.Remove(i);
  return taBase_Group::Remove(i);
}

Con_Group* Con_Group::NewPrjn(Projection* aprjn, bool own) {
  Con_Group* rval = (Con_Group*)NewGp(1, aprjn->con_gp_type);
  taBase::SetPointer((TAPtr*)&(rval->prjn), aprjn);
  spec = aprjn->con_spec;	// set our spec to this one too..
  rval->el_typ = aprjn->con_type; // set type of connection to this type..
  el_typ = aprjn->con_type;	// ditto
  rval->spec = aprjn->con_spec;
  rval->own_cons = own;
  return rval;
}

Con_Group* Con_Group::FindPrjn(Projection* aprjn, int& idx) const {
  int g;
  for(g=0; g<gp.size; g++) {
    Con_Group* cg = (Con_Group*)gp.FastEl(g);
    if(cg->prjn == aprjn) {
      idx = g;
      return cg;
    }
  }
  idx = -1;
  return NULL;
}

Con_Group* Con_Group::FindFrom(Layer* from, int& idx) const {
  int g;
  for(g=0; g<gp.size; g++) {
    Con_Group* cg = (Con_Group*)gp.FastEl(g);
    if((cg->prjn) && (cg->prjn->from == from)) {
      idx = g;
      return cg;
    }
  }
  idx = -1;
  return NULL;
}

Con_Group* Con_Group::FindTypeFrom(TypeDef* prjn_td, Layer* from, int& idx) const {
  int g;
  for(g=0; g<gp.size; g++) {
    Con_Group* cg = (Con_Group*)gp.FastEl(g);
    if((cg->prjn) && (cg->prjn->from == from) &&
       (cg->prjn->InheritsFrom(prjn_td)))
    {
      idx = g;
      return cg;
    }
  }
  idx = -1;
  return NULL;
}


Con_Group* Con_Group::FindLayer(Layer* lay, int& idx) const {
  int g;
  for(g=0; g<gp.size; g++) {
    Con_Group* cg = (Con_Group*)gp.FastEl(g);
    if((cg->prjn) && (cg->prjn->layer == lay)) {
      idx = g;
      return cg;
    }
  }
  idx = -1;
  return NULL;
}

bool Con_Group::RemovePrjn(Projection* aprjn) {
  bool rval = false;
  int g;
  for(g=gp.size-1; g>=0; g--) {
    Con_Group* cg = (Con_Group*)gp.FastEl(g);
    if(cg->prjn == aprjn) {
      cg->prjn->projected = false;
      gp.Remove(cg);
      rval = true;
    }
  }
  return rval;
}

bool Con_Group::RemoveFrom(Layer* from) {
  bool rval = false;
  int g;
  for(g=gp.size-1; g>=0; g--) {
    Con_Group* cg = (Con_Group*)gp.FastEl(g);
    if((cg->prjn) && (cg->prjn->from == from)) {
      cg->prjn->projected = false;
      gp.Remove(cg);
      rval = true;
    }
  }
  return rval;
}

Connection* Con_Group::FindRecipRecvCon(Unit* su, Unit* ru) {
  Layer* my_lay = GET_OWNER(ru,Layer);
  int g;
  for(g=0; g<su->recv.gp.size; g++) {
    Con_Group* cg = (Con_Group*)su->recv.gp.FastEl(g);
    if((cg->prjn == NULL) && (cg->prjn->from != my_lay)) continue;
    Connection* con = cg->FindConFrom(ru);
    if(con) return con;
  }
  return NULL;
}

Connection* Con_Group::FindRecipSendCon(Unit* ru, Unit* su) {
  Layer* my_lay = GET_OWNER(su,Layer);
  int g;
  for(g=0; g<ru->send.gp.size; g++) {
    Con_Group* cg = (Con_Group*)ru->send.gp.FastEl(g);
    if((cg->prjn == NULL) && (cg->prjn->layer != my_lay)) continue;
    Connection* con = cg->FindConFrom(su);
    if(con) return con;
  }
  return NULL;
}

void Con_Group::Copy_Weights(const Con_Group* src) {
  int mx = MIN(size, src->size);
  int i;
  for(i=0; i < mx; i++)
    Cn(i)->wt = src->Cn(i)->wt;
}

void Con_Group::WriteWeights(ostream& strm, Unit*, Con_Group::WtSaveFormat fmt) {
  // this saves writing weights for recv-based linked weights
  if(!own_cons) return;
  int i;
  switch(fmt) {
  case Con_Group::TEXT:
    strm << "#Size " << size << "\n";
    for(i=0; i < size; i++) strm << Cn(i)->wt << "\n";
    break;
  case Con_Group::TEXT_IDX:
    if((prjn == NULL) || (prjn->from == NULL)) {
      taMisc::Error("*** Error in WriteWeights::BINARY_IDX: NULL prjn or prjn->from");
      return;
    }
    strm << "#Size " << size << "\n";
    for(i=0; i < size; i++) {
      int lidx = prjn->from->units.FindLeafEl(Un(i));
      if(lidx < 0) {
	lidx = 0;
	taMisc::Error("*** Error in WriteWeights::BINARY_IDX: can't find unit in connection: ", String(i),
		      "in layer:", prjn->layer->name);
      }
      strm << lidx << " " << Cn(i)->wt << "\n";
    }
    break;
  case Con_Group::BINARY:
    strm << size << "\n";
    for(i=0; i < size; i++) strm.write((char*)&(Cn(i)->wt), sizeof(Cn(i)->wt));
    break;
  case Con_Group::BINARY_IDX:
    if((prjn == NULL) || (prjn->from == NULL)) {
      taMisc::Error("*** Error in WriteWeights::BINARY_IDX: NULL prjn or prjn->from");
      return;
    }
    strm << size << "\n";
    for(i=0; i < size; i++) {
      int lidx = prjn->from->units.FindLeafEl(Un(i));
      if(lidx < 0) {
	lidx = 0;
	taMisc::Error("*** Error in WriteWeights::BINARY_IDX: can't find unit in connection: ", String(i),
		      "in layer:", prjn->layer->name);
      }
      strm.write((char*)&(lidx), sizeof(lidx));
      strm.write((char*)&(Cn(i)->wt), sizeof(Cn(i)->wt));
    }
    break;
  }
}

static int con_group_read_weights_text_size(istream& strm, int size) {
  int sz = size;
  int c = strm.peek();
  while(c == '#') {
    strm.get();
    c = taMisc::read_alnum_noeol(strm);
    if(c == EOF) break;
    if(taMisc::LexBuf == "Size") {
      c = taMisc::read_till_eol(strm);
      sz = (int)taMisc::LexBuf;
      break;
    }
    else {
      c = taMisc::read_till_eol(strm);
    }
    c = strm.peek();
  }
  if(c == EOF) sz = 0;
  if(sz < 0) {
    taMisc::Error("*** Con_Group::ReadWeights: Error in reading weights -- read size < 0");
  }
  return sz;
}

static bool con_group_read_weights_idx_con(Con_Group* ths, int& i, Unit* ru, int lidx ) {
  Unit* su = ths->prjn->from->units.Leaf(lidx);
  if(su == NULL) {
    taMisc::Error("*** Error in ReadWeights::_IDX: unit at leaf index: ",
		  String(lidx), "not found in layer:", ths->prjn->from->name);
    i--; ths->EnforceSize(ths->size-1);
    ru->n_recv_cons--;
    return false;
  }
  Con_Group* send_gp = (Con_Group*)su->send.SafeGp(ths->prjn->send_idx);
  if(send_gp == NULL) {
    taMisc::Error("*** Error in ReadWeights::_IDX: unit at leaf index: ",
		  String(lidx), "does not have proper send group:", String(ths->prjn->send_idx));
    i--; ths->EnforceSize(ths->size-1);
    ru->n_recv_cons--;
    return false;
  }
  if(ths->units.size <= i) {
    if(su == NULL) su = ths->prjn->from->units.Leaf(0);
    ths->units.Link(su);
    int sidx = send_gp->units.Find(ru);
    if(sidx >= 0) {
      send_gp->ReplaceLink(sidx, ths->Cn(i));
    }
    else {
      send_gp->LinkCon(ths->Cn(i), ru);
    }
  }
  else if(su != ths->Un(i)) {
    ths->units.ReplaceLink(i, su);
    int sidx = send_gp->units.Find(ru);
    if(sidx >= 0) {
      send_gp->ReplaceLink(sidx, ths->Cn(i));
    }
    else {
      send_gp->LinkCon(ths->Cn(i), ru);
    }
  }
  return true;
}

void Con_Group::ReadWeights(istream& strm, Unit* ru, Con_Group::WtSaveFormat fmt) {
  // this saves writing weights for recv-based linked weights
  if(!own_cons) return;
  int i;
  switch(fmt) {
  case Con_Group::TEXT:
    {
      int sz = con_group_read_weights_text_size(strm, size);
      if(sz <= 0) return ;	// zero size likely = DMEM save (or EOF) so just skip it!
      if(sz != size) {
	taMisc::Error("*** Error in reading weights: read size (", String(sz),
		      ") != current size (", String(size), ")");
      }
      int minsz = MIN(sz, size);
      for(i=0; i < minsz; i++) {
	int c = taMisc::read_alnum_noeol(strm);
	if(c == EOF) break;
	if(taMisc::LexBuf.empty()) {
	  i--; continue;		// re-read it
	}
	if(taMisc::LexBuf.firstchar() == '#') { // skip comments
	  if(c != '\n')
	    taMisc::read_till_eol(strm); // bag rest of line
	  i--;			// re-read this one
	  continue;
	}
	Cn(i)->wt = (float)taMisc::LexBuf;
      }
      if(sz > size) {
	for(;i < sz; i++) {
	  int c = taMisc::read_alnum_noeol(strm);
	  if(c == EOF) break;
	  if(taMisc::LexBuf.empty()) {
	    i--; continue;		// re-read it
	  }
	  if(taMisc::LexBuf.firstchar() == '#') { // skip comments
	    if(c != '\n')
	      taMisc::read_till_eol(strm); // bag rest of line
	    i--;			// re-read this one
	    continue;
	  }
	}
      }
    }
    break;
  case Con_Group::TEXT_IDX:
    {
      int sz = con_group_read_weights_text_size(strm, size);
      if(sz <= 0) return ;	// zero size likely = DMEM save (or EOF) so just skip it!
      ru->n_recv_cons += sz - size;
      EnforceSize(sz);
      for(i=0; i < size; i++) {
	int c = taMisc::read_alnum_noeol(strm);
	if(c == EOF) break;
	if(taMisc::LexBuf.empty()) {
	  i--; continue;		// re-read it
	}
	if(taMisc::LexBuf.firstchar() == '#') { // skip comments
	  if(c != '\n')
	    taMisc::read_till_eol(strm); // bag rest of line
	  i--;			// re-read this one
	  continue;
	}
	int lidx = (int)taMisc::LexBuf;
	c = taMisc::read_till_eol(strm);
	Cn(i)->wt = (float)taMisc::LexBuf;

	con_group_read_weights_idx_con(this, i, ru, lidx);
      }
    }
    break;
  case Con_Group::BINARY:
    {
      int sz;
      strm >> sz;  strm.get();
      if(sz == 0) return;	// zero size likely = DMEM save so just skip it!
      if(sz < 0) {
	taMisc::Error("*** Error in reading weights: read size < 0");
	return;
      }
      if(sz != size) {
	taMisc::Error("*** Error in reading weights: read size (", String(sz),
		      ") != current size (", String(size), ")");
      }
      int minsz = MIN(sz, size);
      for(i=0; i < minsz; i++) strm.read((char*)&(Cn(i)->wt), sizeof(Cn(i)->wt));
      if(sz > size) {
	float dumy;
	for(;i<sz;i++) strm.read((char*)&(dumy), sizeof(dumy));
      }
    }
    break;
  case Con_Group::BINARY_IDX:
    {
      if((prjn == NULL) || (prjn->from == NULL)) {
	taMisc::Error("*** Error in ReadWeights::BINARY_IDX: NULL prjn or prjn->from in WriteWeights::BINARY_IDX");
	return;
      }
      int sz;
      strm >> sz;  strm.get();
      if(sz == 0) return;	// zero size likely = DMEM save so just skip it!
      if(sz < 0) {
	taMisc::Error("*** Error in ReadWeights::BINARY_IDX: read size < 0");
	return;
      }
      ru->n_recv_cons += sz - size;
      EnforceSize(sz);
      for(i=0; i < size; i++) {
	int lidx;
	strm.read((char*)&(lidx), sizeof(lidx));
	strm.read((char*)&(Cn(i)->wt), sizeof(Cn(i)->wt));

	con_group_read_weights_idx_con(this, i, ru, lidx);
      }
    }
    break;
  }
}

void Con_Group::TransformWeights(const SimpleMathSpec& trans) {
  int i;
  for(i=0; i < size; i++)
    Cn(i)->wt = trans.Evaluate(Cn(i)->wt);
}

void Con_Group::AddNoiseToWeights(const Random& noise_spec) {
  int i;
  for(i=0; i < size; i++)
    Cn(i)->wt += noise_spec.Gen();
}

int Con_Group::PruneCons(Unit* un, const SimpleMathSpec& pre_proc,
			    CountParam::Relation rel, float cmp_val)
{
  CountParam cond;
  cond.rel = rel; cond.val = cmp_val;
  int rval = 0;
  int j;
  for(j=size-1; j>=0; j--) {
    if(cond.Evaluate(pre_proc.Evaluate(Cn(j)->wt))) {
      un->DisConnectFrom(Un(j), prjn);
      rval++;
    }
  }
  return rval;
}

int Con_Group::LesionCons(Unit* un, float p_lesion, bool permute) {
  int rval = 0;
  if(permute) {
    rval = (int) (p_lesion * (float)size);
    if(rval == 0) return 0;
    int_Array ary;
    int j;
    for(j=0; j<size; j++)
      ary.Add(j);
    ary.Permute();
    ary.size = rval;
    ary.Sort();
    for(j=ary.size-1; j>=0; j--)
      un->DisConnectFrom(Un(ary.FastEl(j)), prjn);
  }
  else {
    int j;
    for(j=size-1; j>=0; j--) {
      if(Random::ZeroOne() <= p_lesion) {
	un->DisConnectFrom(Un(j), prjn);
	rval++;
      }
    }
  }
  return rval;
}

// todo: replace with matrix versions!
void Con_Group::ConValuesToArray(float_RArray& ary, const char* variable) {
  MemberDef* md = el_typ->members.FindName(variable);
  if((md == NULL) || !md->type->InheritsFrom(TA_float)) {
    taMisc::Error("*** Variable:", variable, "not found or not a float on units of type:",
		   el_typ->name, "in ConValuesToArray()");
    return;
  }
  int i;
  for(i=0; i<size; i++) {
    float* val = (float*)md->GetOff((void*)Cn(i));
    ary.Add(*val);
  }
}

void Con_Group::ConValuesFromArray(float_RArray& ary, const char* variable) {
  MemberDef* md = el_typ->members.FindName(variable);
  if((md == NULL) || !md->type->InheritsFrom(TA_float)) {
    taMisc::Error("*** Variable:", variable, "not found or not a float on units of type:",
		   el_typ->name, "in ConValuesToArray()");
    return;
  }
  int mx = MIN(size, ary.size);
  int i;
  for(i=0; i<mx; i++) {
    float* val = (float*)md->GetOff((void*)Cn(i));
    *val = ary[i];
  }
}

int Con_Group::ReplaceConSpec(ConSpec* old_sp, ConSpec* new_sp) {
  if(spec.spec != old_sp) return 0;
  taBase::SetPointer((TAPtr*)&spec.spec, new_sp);
//   spec.SetSpec(new_sp);
  return 1;
}

bool Con_Group::CheckTypes(bool quiet) {
  if(!spec.CheckSpec())
    return false;
  if((prjn == NULL) && (size > 0)) {
    if(!quiet)
      taMisc::Error("ConGroup:",GetPath(),"has null projection!, do Connect All");
    return false;
  }
  if((size > 0) && (other_idx < 0)) {
    if(!quiet)
      taMisc::Error("ConGroup:", GetPath(), "has unset other_idx, do FixPrjnIndexes or Connect All");
    return false;
  }
  return true;
}

bool Con_Group::CheckOtherIdx_Recv() {
  if(size == 0) return true;
  if((other_idx < 0) || (other_idx != prjn->send_idx)) {
    taMisc::Error("recv ConGroup:", GetPath(), "has unset other_idx or doesn't match prjn, do FixPrjnIndexes or Connect All");
    return false;
  }
  Unit* su = Un(0);
  if(su->send.gp.size <= other_idx) {
    taMisc::Error("recv ConGroup:", GetPath(), "other_idx", String(other_idx),
		  "is out of range on sending unit. Do Actions/Remove Cons, then Build, Connect on Network");
    return false;
  }
  Con_Group* sucg = (Con_Group*)su->send.FastGp(other_idx);
  if(sucg->prjn != prjn) {
    taMisc::Error("recv ConGroup:", GetPath(), "other_idx", String(other_idx),
		  "doesn't have correct prjn on sending unit.  Do Actions/Remove Cons, then Build, Connect on Network");
    return false;
  }
  return true;
}

bool Con_Group::CheckOtherIdx_Send() {
  if(size == 0) return true;
  if((other_idx < 0) || (other_idx != prjn->recv_idx)) {
    taMisc::Error("send ConGroup:", GetPath(), "has unset other_idx or doesn't match prjn, do FixPrjnIndexes or Connect All");
    return false;
  }
  Unit* ru = Un(0);
  if(ru->recv.gp.size <= other_idx) {
    taMisc::Error("send ConGroup:", GetPath(), "other_idx", String(other_idx),
		  "is out of range on recv unit. Do Actions/Remove Cons, then Build, Connect on Network");
    return false;
  }
  Con_Group* rucg = (Con_Group*)ru->recv.FastGp(other_idx);
  if(rucg->prjn != prjn) {
    taMisc::Error("send ConGroup:", GetPath(), "other_idx", String(other_idx),
		  "doesn't have correct prjn on recv unit.  Do Actions/Remove Cons, then Build, Connect on Network");
    return false;
  }
  return true;
}

// only dump sub-group stuff
int Con_Group::Dump_Save_PathR(ostream& strm, TAPtr par, int indent) {
  return gp.Dump_Save_PathR(strm, par, indent);
}
int Con_Group::Dump_SaveR(ostream& strm, TAPtr par, int indent) {
  return gp.Dump_SaveR(strm, par, indent);
}

// have to implement after save_value because we're not saving a real
// path that can be loaded with Load

int Con_Group::Dump_Save_Value(ostream& strm, TAPtr par, int indent) {
  if(size == 0)			// don't own any if you don't got 'em
    own_cons = false;		// and, own_cons is used as a flag for loading..
  int rval = taBase_Group::Dump_Save_Value(strm, par, indent); // first dump members
  if((size == 0) || !own_cons || !rval || (prjn == NULL))
    return rval;

  // close off the regular members
  taMisc::indent(strm, indent,1) << "};\n";

  // output the units
  taMisc::indent(strm, indent, 1) << "{ units = {";

  int i;
  for(i=0; i<size; i++) {
    String un_path = Un(i)->GetPath(NULL, &(prjn->from->units));
    if(un_path == "root") {
      taMisc::Error("*** Units in projection:", prjn->name,"in layer:",prjn->layer->name,
		    "have a null path.  Do ConnectAll and save again.");
    }
    strm << un_path << "; ";
    if(((i+1) % 8) == 0)
      strm << "\n";
  }
  strm << "};\n";

  // output the connection values
  TypeDef* ctd = Cn(0)->GetTypeDef();
  int j;
  for(j=0; j<ctd->members.size; j++) {
    MemberDef* md = ctd->members.FastEl(j);
    if((md->type->ptr > 0) || (md->HasOption("NO_SAVE")))
      continue;
    taMisc::indent(strm, indent+1,1) << md->name << " = {";
    for(i=0; i<size; i++) {
      strm << md->type->GetValStr(md->GetOff((void*)Cn(i))) << "; ";
      if((((i+1) % 8) == 0) && (i < (size-1)))
	strm << "\n";
    }
    strm << "};\n";
  }
  return true;
}

int Con_Group::Dump_Load_Value(istream& strm, TAPtr) {
  int rval = taBase_Group::Dump_Load_Value(strm); // first dump members
  if(!own_cons || (rval == EOF) || (rval == 2))
    return rval;

  int c = taMisc::read_till_lbracket(strm);	// get past opening bracket
  if(c == EOF) return EOF;
  c = taMisc::read_word(strm);
  if(taMisc::LexBuf != "units") {
    taMisc::Error("*** Con_Group Expecting: 'units' in load file, got:",
		    taMisc::LexBuf,"instead");
    return false;
  }
  // skip =
  c = taMisc::skip_white(strm);
  if(c != '=') {
    taMisc::Error("*** Missing '=' in dump file for unit in Con_Group");
    return false;
  }
  // skip {
  c = taMisc::skip_white(strm);
  if(c != '{') {
    taMisc::Error("*** Missing '{' in dump file for unit in Con_Group");
    return false;
  }

  // prjn has to be loaded in advance of this!
  // (sender-based must have send_prjns own projection, else projection owns it)
  if((prjn == NULL) || (prjn->from == NULL) || (prjn->layer == NULL)) {
    taMisc::Error("*** projection or ->from or ->layer is NULL");
    return false;
  }
  Unit_Group* ug = &(prjn->from->units);

  TypeDef* ctd = prjn->con_type;

  // now read them all in..
  int c_count = 0;		// number of connections
  while(true) {
    c = taMisc::read_till_rb_or_semi(strm);
    if(c == EOF) return EOF;
    if(c == '}') break;

    // find ptr (optimized to search relative to ug_md..
    MemberDef* md;
    Unit* un = (Unit*)ug->FindFromPath(taMisc::LexBuf, md);
    if(un == NULL) {
      taMisc::Error("*** Connection unit not found:", taMisc::LexBuf,
		      "in connection type", ctd->name);
      c = taMisc::skip_past_err(strm); // scan past the error
      if(c == '}') break;
      continue;
    }
    else {
      if(units.size > c_count)
	units.ReplaceLink(c_count, un);
      else {
	units.Link(un);
	if(size < units.size)
	  Add(taBase::MakeToken(ctd)); // keep synced..
      }
    }
    c_count++;
  }

  // now read in the values
  while(true) {
    c = taMisc::read_word(strm);
    if(c == EOF) return EOF;
    if(c == '}') {
      if(strm.peek() == ';') strm.get(); // get the semi
      break;		// done
    }
    MemberDef* md = ctd->members.FindName(taMisc::LexBuf);
    if(md == NULL) {
      taMisc::Error("*** Connection member not found:", taMisc::LexBuf,
		      "in connection type", ctd->name);
      c = taMisc::skip_past_err(strm);
      if(c == '}') break;
      continue;
    }
    // skip =
    c = taMisc::skip_white(strm);
    if(c != '=') {
      taMisc::Error("*** Missing '=' in dump file for unit in Con_Group");
      c = taMisc::skip_past_err(strm);
      continue;
    }
    // skip {
    c = taMisc::skip_white(strm);
    if(c != '{') {
      taMisc::Error("*** Missing '{' in dump file for unit in Con_Group");
      c = taMisc::skip_past_err(strm);
      continue;
    }

    int i = 0;
    while(true) {
      c = taMisc::read_till_rb_or_semi(strm);
      if(c == EOF) return EOF;
      if(c == '}') break;
      if(i >= size) {
	c = taMisc::skip_past_err_rb(strm); // bail to ending rb
	break;
      }
      Connection* cn = Cn(i);
      md->type->SetValStr(taMisc::LexBuf, md->GetOff((void*)cn));
      i++;
    }
  }
  return true;
}

////////////////////////
//	Unit	      //
////////////////////////

void UnitSpec::Initialize() {
  min_obj_type = &TA_Unit;
  act_range.max = 1.0f; act_range.min = 0.0f;
  act_range.UpdateAfterEdit();
  bias_con_type = NULL;
  sse_tol = 0.0f;
}

void UnitSpec::InitLinks() {
  BaseSpec::InitLinks();
  children.SetBaseType(&TA_UnitSpec); // allow all of this general spec type to be created under here
  children.el_typ = GetTypeDef(); // but make the default to be me!
  taBase::Own(act_range, this);
  taBase::Own(bias_spec, this);
  bias_spec.owner = this;	// set the owner cuz setdefaultspec won't necc. do so.
  // don't do this if loading -- creates specs in all the wrong places..
  // specs that own specs have this problem
  Network* net = (Network *) GET_MY_OWNER(Network);
  if(net && !net->copying && !taMisc::is_loading)
    bias_spec.SetDefaultSpec(this);
}

void UnitSpec::Copy_(const UnitSpec& cp) {
  act_range = cp.act_range;
  bias_con_type = cp.bias_con_type;
  bias_spec = cp.bias_spec;
  sse_tol = cp.sse_tol;
}

bool UnitSpec::CheckConfig(Unit* un, Layer* lay, Network* net, bool quiet) {
  Con_Group* recv_gp;
  int g;
  FOR_ITR_GP(Con_Group, recv_gp, un->recv., g) {
    if(!recv_gp->CheckConfig(lay, un, net, quiet)) return false;
  }
  return true;
}

void UnitSpec::UpdateAfterEdit() {
  inherited::UpdateAfterEdit();
  act_range.UpdateAfterEdit();
  bias_spec.CheckSpec();
  if((bias_con_type) && (bias_spec.spec) && !bias_con_type->InheritsFrom(bias_spec.spec->min_con_type)) {
    taMisc::Error("Bias con type of:", bias_con_type->name,
		  "is not of the correct type for the bias con spec,"
		  "which needs at least a:", bias_spec.spec->min_con_type->name);
    return;
  }
  if(taMisc::is_loading) return;

  if(!taMisc::gui_active) return;
//   Network* net = (Network *) GET_MY_OWNER(Network);
//   if (!net) return;
/*TODO    NetView* vw;
    taLeafItr vi;
    FOR_ITR_EL(NetView, vw, net->views., vi) {
      vw->UpdateButtons();	// update buttons to reflect need to build or not..
    } */
}

int UnitSpec::UseCount() {
  Network* net = (Network *) GET_MY_OWNER(Network);
  if(!net) return -1;
  int cnt = net->ReplaceUnitSpec(this, this);
  return cnt;
}

void UnitSpec::BuildBiasCons() {
  Network* net = (Network *) GET_MY_OWNER(Network);
  if (!net) return;
  net->Build();
}

void UnitSpec::CutLinks() {
  Network* net = (Network *) GET_MY_OWNER(Network);
  if((net) && !net->deleting) {
    UnitSpec* rsp = (UnitSpec*)net->specs.FindSpecTypeNotMe(GetTypeDef(), this);
    if(rsp == NULL) {
      rsp = (UnitSpec*)net->specs.FindSpecInheritsNotMe(&TA_UnitSpec, this);
    }
    if(rsp) {
      int cnt = net->ReplaceUnitSpec(this, rsp);
      if(cnt > 0) {
	taMisc::Error("Warning: UnitSpec",this->GetPath(),"was used in the network, replaced with",rsp->GetPath());
      }
    }
    else {
      taMisc::Error("Warning: Deleting UnitSpec",this->GetPath(),"and couldn't find replacement - network will have NULL spec and crash!");
    }
  }
  BaseSpec::CutLinks();
  bias_spec.CutLinks();
}

void UnitSpec::InitState(Unit* u) {
  u->InitExterns();
  u->InitDelta();
  u->act = 0.0f;
}

void UnitSpec::InitWtDelta(Unit* u) {
  Con_Group* recv_gp;
  int g;
  FOR_ITR_GP(Con_Group, recv_gp, u->recv., g) {
    if(!recv_gp->prjn->from->lesion)
      recv_gp->InitWtDelta(u);
  }
  if(u->bias)
    bias_spec->C_InitWtDelta(NULL, u->bias, u, NULL); // this is a virtual fun
}

void UnitSpec::InitWtState(Unit* u) {
#ifdef DMEM_COMPILE
  if(!u->DMem_IsLocal()) {
    // make up for random numbers not being used for non-local connections.
    for(int i=0; i<u->n_recv_cons; i++) Random::ZeroOne();
  }
  else
#endif
    {
      Con_Group* recv_gp;
      int g;
      FOR_ITR_GP(Con_Group, recv_gp, u->recv., g) {
	// ignore lesion here because n_recv_cons does not take into account lesioned layers, so dmem would get out of sync
	//    if(!recv_gp->prjn->from->lesion)
	recv_gp->InitWtState(u);
      }
    }

  if(u->bias) {
    bias_spec->C_InitWtState(NULL, u->bias, u, NULL); // this is a virtual fun
    bias_spec->C_InitWtDelta(NULL, u->bias, u, NULL); // don't forget delta too!!
  }
}

void UnitSpec::InitWtState_post(Unit* u) {
  Con_Group* recv_gp;
  int g;
  FOR_ITR_GP(Con_Group, recv_gp, u->recv., g) {
    if(!recv_gp->prjn->from->lesion)
      recv_gp->InitWtState_post(u);
  }
}

void UnitSpec::Compute_Net(Unit* u) {
  Con_Group* recv_gp;
  u->net = 0.0f;
  int g;
  FOR_ITR_GP(Con_Group, recv_gp, u->recv., g) {
    if(!recv_gp->prjn->from->lesion)
      u->net += recv_gp->Compute_Net(u);
  }
  if(u->bias)
    u->net += u->bias->wt;
}

void UnitSpec::Send_Net(Unit* u) {
  // typically the whole point of using sender based net input is that you want to check
  // here if the sending unit's activation (i.e., this one) is above some threshold
  // so you don't send if it isn't above that threshold..  this isn't implemented here though.
  Con_Group* send_gp;
  int g;
  FOR_ITR_GP(Con_Group, send_gp, u->send., g) {
    Layer* tol = send_gp->prjn->layer;
    if(!tol->lesion)
      send_gp->Send_Net(u);
  }
  if(u->bias)
    u->net += u->bias->wt;
}

void UnitSpec::Send_NetToLay(Unit* u, Layer* tolay) {
  // typically the whole point of using sender based net input is that you want to check
  // here if the sending unit's activation (i.e., this one) is above some threshold
  // so you don't send if it isn't above that threshold..  this isn't implemented here though.
  Con_Group* send_gp;
  int g;
  FOR_ITR_GP(Con_Group, send_gp, u->send., g) {
    Layer* tol = send_gp->prjn->layer;
    if(!tol->lesion && (tol == tolay))
      send_gp->Send_Net(u);
  }
  if(u->bias)
    u->net += u->bias->wt;
}

void UnitSpec::Compute_Act(Unit* u) {
  if(u->ext_flag & Unit::EXT)
    u->act = u->ext;
  else
    u->act = u->net;
}

void UnitSpec::Compute_dWt(Unit* u) {
  Con_Group* recv_gp;
  int g;
  FOR_ITR_GP(Con_Group, recv_gp, u->recv., g) {
    if(!recv_gp->prjn->from->lesion)
      recv_gp->Compute_dWt(u);
  }
  // NOTE: derived classes must supply bias->Compute_dWt call because C_Compute_dWt
  // is not virtual, so if called here, only ConSpec version would be called.
  // This is not true of InitWtState and InitWtDelta, which are virtual.
}

void UnitSpec::UpdateWeights(Unit* u) {
  Con_Group* recv_gp;
  int g;
  FOR_ITR_GP(Con_Group, recv_gp, u->recv., g) {
    if(!recv_gp->prjn->from->lesion)
      recv_gp->UpdateWeights(u);
  }
  // NOTE: derived classes must supply bias->UpdateWeights call because C_UpdateWeights
  // is not virtual, so if called here, only ConSpec version would be called.
  // This is not true of InitWtState and InitWtDelta, which are virtual.
}

float UnitSpec::Compute_SSE(Unit* u) {
  float sse = 0.0f;
  if(u->ext_flag & Unit::TARG) {
    float uerr = u->targ - u->act;
    if(fabs(uerr) >= sse_tol)
      sse = uerr * uerr;
  }
  return sse;
}


/////// Unit ///////

void Unit::Initialize() {
  recv.SetBaseType(&TA_Connection);
  send.SetBaseType(&TA_Connection);
  bias = NULL;
  ext_flag = NO_EXTERNAL;
  targ = 0.0f;
  ext = 0.0f;
  act = 0.0f;
  net = 0.0f;
  n_recv_cons = 0;
}

void Unit::Destroy() {
  CutLinks();
}

void Unit::InitLinks() {
  taNBase::InitLinks();
  taBase::Own(recv, this);	// always own your constitutents
  taBase::Own(send, this);
  taBase::Own(pos, this);
  spec.SetDefaultSpec(this);
  Build();
  Layer* lay = GET_MY_OWNER(Layer);
  if(lay && !taMisc::is_loading)
    lay->LayoutUnits(this);
}

void Unit::CutLinks() {
  recv.CutLinks();
  send.CutLinks();
  taBase::DelPointer((TAPtr*)&bias);
  spec.CutLinks();
/*obs -- datalink mechanism automatically updates
  if((owner) && taMisc::gui_active && (pos.z >= 0)) { // if z = -1, then don't update display
    Layer* lay = ((Unit_Group*)owner)->own_lay;
    owner = NULL;
    if((lay) && (lay->own_net) && !lay->own_net->net_will_updt)
      lay->own_net->InitAllViews();
  } */
  inherited::CutLinks();
}

void Unit::Copy_(const Unit& cp) {
  if((bias) && (cp.bias))
    *bias = *(cp.bias);
  spec = cp.spec;
  pos = cp.pos;
  ext_flag = cp.ext_flag;
  targ = cp.targ;
  ext = cp.ext;
  act = cp.act;
  net = cp.net;
  recv = cp.recv;
  send = cp.send;
  n_recv_cons = cp.n_recv_cons;
}

void Unit::UpdateAfterEdit() {
  taNBase::UpdateAfterEdit();
  spec.CheckSpec();
  // this updates the unit's representation int the netviews
  // no negative positions
  pos.x = MAX(0,pos.x); pos.y = MAX(0,pos.y);  pos.z = MAX(0,pos.z);
  // stay within layer->geom
  Layer* lay = GET_MY_OWNER(Layer);
  if(lay == NULL) return;
  pos.x = MIN(lay->geom.x-1,pos.x); pos.y = MIN(lay->geom.y-1,pos.y);
  pos.z = MIN(lay->geom.z-1,pos.z);
  if(taMisc::is_loading || !taMisc::gui_active) return;
  Network* mynet = GET_MY_OWNER(Network);
  if(mynet == NULL)	return;
/*TODO  NetView* view;
  taLeafItr i;
  FOR_ITR_EL(NetView, view, mynet->views., i) {
    if(view->display_toggle)
      view->FixUnit(this,lay);
  }*/
}

#ifdef DMEM_COMPILE
int Unit::dmem_this_proc = 0;
#endif

// todo: change con_group's replacepointers fun to only op on cons or units
// and to properly get the units..

// void Unit::ReplacePointersHook(TAPtr old) {
//   Unit* ou = (Unit*)old;
//   CopyPtrs(ou);
//   // now go through and replace all pointers to unit
//   Network* own_net = GET_MY_OWNER(Network);
//   if(own_net) {
//     Layer* l;
//     taLeafItr li;
//     FOR_ITR_EL(Layer, l, own_net->layers., li) {
//       Unit* u;
//       taLeafItr ui;
//       FOR_ITR_EL(Unit, u, l->units., ui) {
// 	if(u == ou) continue;
// 	int g;
// 	for(g=0; g < u->recv.gp.size; g++) {
// 	  Con_Group* cg = (Con_Group*)u->recv.gp.FastEl(g);
// 	  int i;
// 	  for(i=0;i<cg->units.size;i++) {
// 	    if(cg->Un(i) == ou)
// 	      cg->units.ReplaceLink(i, this);
// 	  }
// 	}
// 	for(g=0; g < u->send.gp.size; g++) {
// 	  Con_Group* cg = (Con_Group*)u->send.gp.FastEl(g);
// 	  int i;
// 	  for(i=0;i<cg->units.size;i++) {
// 	    if(cg->Un(i) == ou)
// 	      cg->units.ReplaceLink(i, this);
// 	  }
// 	}
//       }
//     }
//   }
//   ou->recv.Reset();		// get rid of old connectivity
//   ou->send.Reset();
//   taNBase::ReplacePointersHook(old);
// }

void Unit::ApplyExternal(float val, ExtType act_ext_flags, Random* ran) {
  // note: not all flag values are valid, so following is a fuzzy cascade
  // ext is the default place, so we check for 
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

void Unit::CopyNetwork(Network* anet, Network* cn, Unit* cp) {
  int g;
  for(g=0; g<recv.gp.size && g<cp->recv.gp.size; g++) {
    ((Con_Group*)recv.gp.FastEl(g))->CopyNetwork(anet, cn, (Con_Group*)cp->recv.gp.FastEl(g));
  }
  for(g=0; g<send.gp.size && g<send.gp.size; g++) {
    ((Con_Group*)send.gp.FastEl(g))->CopyNetwork(anet, cn, (Con_Group*)cp->send.gp.FastEl(g));
  }
}

void Unit::CopyPtrs(Unit* cp) {
  int g;
  for(g=0; g<recv.gp.size && g<cp->recv.gp.size; g++) {
    ((Con_Group*)recv.gp.FastEl(g))->CopyPtrs((Con_Group*)cp->recv.gp.FastEl(g));
  }
  for(g=0; g<send.gp.size && g<send.gp.size; g++) {
    ((Con_Group*)send.gp.FastEl(g))->CopyPtrs((Con_Group*)cp->send.gp.FastEl(g));
  }
}

// the 'New' function is used during loading to create objects if they are
// not of the correct type, this will accomplish that
TAPtr Unit::New(int, TypeDef* type) {
  if(!type->InheritsFrom(TA_Connection))
    return NULL;
  Connection* new_bias = (Connection*)taBase::MakeToken(type);
  taBase::SetPointer((TAPtr*)&bias, new_bias);
  return new_bias;
}

bool Unit::Build() {
  bool rval = false;
  if(spec.spec == NULL)
    return false;
  TypeDef* bstd = spec->bias_con_type;
  if(bstd == NULL) {
    if(bias) {
      taBase::DelPointer((TAPtr*)&bias);
      rval = true;
    }
  }
  else {
    if((bias == NULL) || (bias->GetTypeDef() != bstd)) {
      rval = true;
      Connection* new_bias = (Connection*)taBase::MakeToken(bstd);
      taBase::SetPointer((TAPtr*)&bias, new_bias);
    }
  }
  return rval;
}

bool Unit::CheckBuild(bool quiet) {
  if(!spec.spec) {
    if(!quiet)
      taMisc::Error("Unit CheckBuild: no unit spec set for unit:", GetPath());
    return false;
  }
  if(spec->bias_con_type == NULL) {
    if(bias) {
      if(!quiet)
	taMisc::Error("Unit CheckBuild: bias weight exists but no type for unit:", GetPath());
      return false;		// todo: error messages
    }
  }
  else {
    if((bias == NULL) || (bias->GetTypeDef() != spec->bias_con_type)) {
      if(!quiet)
	taMisc::Error("Unit CheckBuild: bias weight null or not same type for unit:",
		      GetPath()," type should be:", spec->bias_con_type->name);
      return false;
    }
  }
  return true;
}

bool Unit::CheckTypes(bool quiet) {
  if(!spec.CheckSpec())
    return false;
  if(spec->bias_con_type) {
    if(!spec->bias_spec.CheckSpec(bias))
      return false;
  }
  Con_Group* cg;
  int i;
  FOR_ITR_GP(Con_Group, cg, recv., i) {
    if(!cg->CheckTypes(quiet)) return false;
  }
  FOR_ITR_GP(Con_Group, cg, send., i) {
    if(!cg->CheckTypes(quiet)) return false;
  }
  return true;
}

void Unit::RemoveCons() {
  recv.RemoveAll();		// blunt, but effective
  send.RemoveAll();
  n_recv_cons = 0;
}

bool Unit::SetConSpec(ConSpec* sp) {
  if(sp == NULL)	return false;
  int g;
  for(g=0; g<recv.gp.size; g++) {
    Con_Group* recv_gp = (Con_Group*)recv.gp.FastEl(g);
    if(sp->CheckObjectType(recv_gp))
      recv_gp->spec.SetSpec(sp);
    else
      return false;
  }
  return true;
}

int Unit::ReplaceUnitSpec(UnitSpec* old_sp, UnitSpec* new_sp) {
  if(spec.spec != old_sp) return 0;
  taBase::SetPointer((TAPtr*)&spec.spec, new_sp);
  //  spec.SetSpec(new_sp);
  return 1;
}

int Unit::ReplaceConSpec(ConSpec* old_sp, ConSpec* new_sp) {
  int nchg = 0;
  int g;
  nchg += recv.ReplaceConSpec(old_sp, new_sp);
  for(g=0; g<recv.gp.size; g++) {
    Con_Group* recv_gp = (Con_Group*)recv.gp.FastEl(g);
    nchg += recv_gp->ReplaceConSpec(old_sp, new_sp);
  }
  nchg += send.ReplaceConSpec(old_sp, new_sp);
  for(g=0; g<send.gp.size; g++) {
    Con_Group* send_gp = (Con_Group*)send.gp.FastEl(g);
    nchg += send_gp->ReplaceConSpec(old_sp, new_sp);
  }
  return nchg;
}

Con_Group* Unit::rcg_rval = NULL;
Con_Group* Unit::scg_rval = NULL;

void Unit::ConnectAlloc(int no, Projection* prjn, Con_Group*& cgp) {
#ifdef DMEM_COMPILE
  if(!DMem_IsLocal() && !prjn->con_spec->DMem_AlwaysLocal()) return;
#endif
  if((prjn->recv_idx < 0) || ((cgp = (Con_Group*)recv.SafeGp(prjn->recv_idx)) == NULL)) {
    cgp = recv.NewPrjn(prjn, true); // owns the connections
    prjn->recv_idx = recv.gp.size-1;
  }
  cgp->AllocCon(no, cgp->prjn->con_type);
}

Connection* Unit::ConnectFrom(Unit* su, Projection* prjn, Con_Group*& recv_gp,
			      Con_Group*& send_gp) {
#ifdef DMEM_COMPILE
  if(!DMem_IsLocal() && !prjn->con_spec->DMem_AlwaysLocal()) return NULL;
#endif
  if((prjn->recv_idx < 0) || ((recv_gp = (Con_Group*)recv.SafeGp(prjn->recv_idx)) == NULL)) {
    recv_gp = recv.NewPrjn(prjn, true);
    prjn->recv_idx = recv.gp.size-1;
  }
  if((prjn->send_idx < 0) || ((send_gp = (Con_Group*)su->send.SafeGp(prjn->send_idx)) == NULL)) {
    send_gp = su->send.NewPrjn(prjn, false);
    prjn->send_idx = su->send.gp.size-1;
  }
  if(recv_gp->other_idx < 0)
    recv_gp->other_idx = prjn->send_idx;
  if(send_gp->other_idx < 0)
    send_gp->other_idx = prjn->recv_idx;

  Connection* con = recv_gp->NewCon(prjn->con_type, su);
  send_gp->LinkCon(con, this);
  n_recv_cons++;
  return con;
}

Connection* Unit::ConnectFromLink(Unit* su, Projection* prjn, Connection* src_con,
				  Con_Group*& recv_gp, Con_Group*& send_gp) {
#ifdef DMEM_COMPILE
  if(!DMem_IsLocal() && !prjn->con_spec->DMem_AlwaysLocal()) return NULL;
#endif
  if((prjn->recv_idx < 0) || ((recv_gp = (Con_Group*)recv.SafeGp(prjn->recv_idx)) == NULL)) {
    recv_gp = recv.NewPrjn(prjn, false); // doesn't own the connections!
    prjn->recv_idx = recv.gp.size-1;
  }
  if((prjn->send_idx < 0) || ((send_gp = (Con_Group*)su->send.SafeGp(prjn->send_idx)) == NULL)) {
    send_gp = su->send.NewPrjn(prjn, false);
    prjn->send_idx = su->send.gp.size-1;
  }
  if(recv_gp->other_idx < 0)
    recv_gp->other_idx = prjn->send_idx;
  if(send_gp->other_idx < 0)
    send_gp->other_idx = prjn->recv_idx;

  recv_gp->LinkCon(src_con, su);
  send_gp->LinkCon(src_con, this);
  n_recv_cons++;
  return src_con;
}

Connection* Unit::ConnectFromCk(Unit* su, Projection* prjn, Con_Group*& recv_gp,
			      Con_Group*& send_gp) {
#ifdef DMEM_COMPILE
  if(!DMem_IsLocal() && !prjn->con_spec->DMem_AlwaysLocal()) return NULL;
#endif
  if((prjn->recv_idx < 0) || ((recv_gp = (Con_Group*)recv.SafeGp(prjn->recv_idx)) == NULL)) {
    recv_gp = recv.NewPrjn(prjn, true);
    prjn->recv_idx = recv.gp.size-1;
  }
  if((prjn->send_idx < 0) || ((send_gp = (Con_Group*)su->send.SafeGp(prjn->send_idx)) == NULL)) {
    send_gp = su->send.NewPrjn(prjn, false);
    prjn->send_idx = su->send.gp.size-1;
  }
  if(recv_gp->other_idx < 0)
    recv_gp->other_idx = prjn->send_idx;
  if(send_gp->other_idx < 0)
    send_gp->other_idx = prjn->recv_idx;

  if(recv_gp->units.Find(su) >= 0) // already connected!
    return NULL;

  Connection* con = recv_gp->NewCon(prjn->con_type, su);
  send_gp->LinkCon(con, this);
  n_recv_cons++;
  return con;
}

Connection* Unit::ConnectFromLinkCk(Unit* su, Projection* prjn, Connection* src_con,
				  Con_Group*& recv_gp, Con_Group*& send_gp) {
#ifdef DMEM_COMPILE
  if(!DMem_IsLocal() && !prjn->con_spec->DMem_AlwaysLocal()) return NULL;
#endif
  if((prjn->recv_idx < 0) || ((recv_gp = (Con_Group*)recv.SafeGp(prjn->recv_idx)) == NULL)) {
    recv_gp = recv.NewPrjn(prjn, false); // doesn't own the connections!
    prjn->recv_idx = recv.gp.size-1;
  }

  if((prjn->send_idx < 0) || ((send_gp = (Con_Group*)su->send.SafeGp(prjn->send_idx)) == NULL)) {
    send_gp = su->send.NewPrjn(prjn, false);
    prjn->send_idx = su->send.gp.size-1;
  }

  if(recv_gp->units.Find(su) >= 0) // already connected!
    return NULL;

  recv_gp->LinkCon(src_con, su);
  send_gp->LinkCon(src_con, this);
  return src_con;
}

bool Unit::DisConnectFrom(Unit* su, Projection* prjn) {
  Con_Group* recv_gp, *send_gp;
  if(prjn) {
    if((prjn->recv_idx < 0) || ((recv_gp = (Con_Group*)recv.SafeGp(prjn->recv_idx)) == NULL))
      return false;
    if((prjn->send_idx < 0) || ((send_gp = (Con_Group*)su->send.SafeGp(prjn->send_idx)) == NULL))
      return false;
  }
  else {
    Layer* su_lay = GET_OWNER(su,Layer);
    if((recv_gp = recv.FindFrom(su_lay)) == NULL)	return false;
    if(recv_gp->other_idx >= 0)
      send_gp = (Con_Group*)su->send.SafeGp(recv_gp->other_idx);
    else
      send_gp = NULL;
    if(send_gp == NULL)
      send_gp = su->send.FindPrjn(recv_gp->prjn);
    if(send_gp == NULL) return false;
    prjn = recv_gp->prjn;
  }

  recv_gp->RemoveCon(su);
  n_recv_cons--;
  return send_gp->RemoveCon(this);
}

void Unit::DisConnectAll() {
  Con_Group* recv_gp, *send_gp;
  int g;
  int i;
  for(g=0; g<recv.gp.size; g++) { // the removes cause the leaf_gp to crash..
    recv_gp = (Con_Group*)recv.gp.FastEl(g);
    for(i=recv_gp->size-1; i>=0; i--) {
      if(recv_gp->other_idx >= 0)
	send_gp = (Con_Group*)recv_gp->Un(i)->send.SafeGp(recv_gp->other_idx);
      else
	send_gp = NULL;
      if(send_gp == NULL)
	send_gp = recv_gp->Un(i)->send.FindPrjn(recv_gp->prjn);
      if(send_gp)
	send_gp->RemoveCon(this);
      recv_gp->Remove(i);
    }
    recv_gp->other_idx = -1;
  }
  for(g=0; g<send.gp.size; g++) { // the removes cause the leaf_gp to crash..
    send_gp = (Con_Group*)send.gp.FastEl(g);
    for(i=send_gp->size-1; i>=0; i--) {
      if(send_gp->other_idx >= 0)
	recv_gp = (Con_Group*)send_gp->Un(i)->recv.SafeGp(send_gp->other_idx);
      else
	recv_gp = NULL;
      if(recv_gp == NULL)
	recv_gp = send_gp->Un(i)->recv.FindPrjn(send_gp->prjn);
      if(recv_gp)
	recv_gp->RemoveCon(this);
      send_gp->Remove(i);
    }
    send_gp->other_idx = -1;
  }
  n_recv_cons = 0;
}

int Unit::CountRecvCons() {
  n_recv_cons = 0;
  Con_Group* cg;
  int g;
  FOR_ITR_GP(Con_Group, cg, recv., g) {
    n_recv_cons += cg->size;
  }
  return n_recv_cons;
}

void Unit::Copy_Weights(const Unit* src, Projection* prjn) {
  if((bias) && (src->bias)) {
    bias->wt = src->bias->wt;
  }
  Con_Group* cg, *scg;
  int i,si;
  for(cg = (Con_Group*)recv.FirstGp(i), scg = (Con_Group*)src->recv.FirstGp(si);
      (cg) && (scg);
      cg = (Con_Group*)recv.NextGp(i), scg = (Con_Group*)src->recv.NextGp(si))
  {
    if(cg->prjn->from->lesion || ((prjn) && (cg->prjn != prjn))) continue;
    cg->Copy_Weights(scg);
  }
}

void Unit::WriteWeights(ostream& strm, Projection* prjn, Con_Group::WtSaveFormat fmt) {
  if(fmt == Con_Group::TEXT)
    strm << "#Unit " << name << "\n";
  if(bias) {
    switch(fmt) {
    case Con_Group::TEXT:
    case Con_Group::TEXT_IDX:
      strm << bias->wt << "\n";
      break;
    case Con_Group::BINARY:
    case Con_Group::BINARY_IDX:
      strm.write((char*)&(bias->wt), sizeof(bias->wt));
      break;
    }
  }
  // not using ITR here in case of DMEM where we write separate files for
  // each process -- need to include size=0 place holders for non-local units
  int g;
  for(g = 0; g < recv.gp.size; g++) {
    Con_Group* cg = (Con_Group*)recv.gp.FastEl(g);
    if(cg->prjn->from->lesion || ((prjn) && (cg->prjn != prjn))) continue;
    if(fmt == Con_Group::TEXT)
      strm << "#Con_Group " << g << "\n";
    cg->WriteWeights(strm, this, fmt);
  }
}

void Unit::ReadWeights(istream& strm, Projection* prjn, Con_Group::WtSaveFormat fmt) {
  if(bias) {
    switch(fmt) {
    case Con_Group::TEXT:
    case Con_Group::TEXT_IDX:
      {
	int c;
	while(true) {
	  c = taMisc::read_alnum_noeol(strm);
	  if(c == EOF) return;
	  if(taMisc::LexBuf.empty()) {
	    continue;		// re-read it
	  }
	  if(taMisc::LexBuf.firstchar() == '#') { // skip comments
	    if(c != '\n')			      // only if didn't read last line
	      taMisc::read_till_eol(strm); // bag rest of line
	    continue;
	  }
	  bias->wt = (float)taMisc::LexBuf;
	  break;
	}
      }
      break;
    case Con_Group::BINARY:
    case Con_Group::BINARY_IDX:
      strm.read((char*)&(bias->wt), sizeof(bias->wt));
      break;
    }
  }
#ifdef DMEM_COMPILE
  if(!DMem_IsLocal()) {
    // bypass non-local connections!
    if(fmt == Con_Group::TEXT) {
      int tot_sz = n_recv_cons;
      int read_sz = 0;		// total sizes actually read from the file
      for(int i=0; i<tot_sz; i++) {
	int c = taMisc::read_alnum_noeol(strm);
	if(c == EOF) break;
	if(taMisc::LexBuf.empty()) {
	  i--; continue;		// re-read it
	}
	if(taMisc::LexBuf.firstchar() == '#') { // skip comments
	  if(taMisc::LexBuf == "#Size") {
	    taMisc::read_till_eol(strm);
	    int sz = (int)taMisc::LexBuf;
	    read_sz += sz;
	    if(read_sz > tot_sz) tot_sz = read_sz;
	  }
	  else if(taMisc::LexBuf == "#Unit") {
	    taMisc::read_till_eol(strm);
	    break; // we just went into the second unit, bail!
	  }
	  else {
	    if(c != '\n')
	      taMisc::read_till_eol(strm); // bag rest of line
	    i--;			// re-read this one
	    continue;
	  }
	}
      }
    }
    else {
      int g;
      for(g = 0; g < recv.gp.size; g++) {
	Con_Group* cg = (Con_Group*)recv.gp.FastEl(g);
	if(cg->prjn->from->lesion || ((prjn) && (cg->prjn != prjn))) continue;
	int sz;
	strm >> sz;  strm.get();
	float dumy;
	if(fmt == Con_Group::BINARY) {
	  for(int i=0;i<sz; i++) strm.read((char*)&(dumy), sizeof(dumy));
	}
	else {
	  float lidx;
	  for(int i=0;i<sz; i++) {
	    strm.read((char*)&(lidx), sizeof(lidx));
	    strm.read((char*)&(dumy), sizeof(dumy));
	  }
	}
      }
    }
    return;
  }
#endif
  if(strm.eof()) return;
  int g;
  for(g = 0; g < recv.gp.size; g++) {
    Con_Group* cg = (Con_Group*)recv.gp.FastEl(g);
    if(cg->prjn->from->lesion || ((prjn) && (cg->prjn != prjn))) continue;
    cg->ReadWeights(strm, this, fmt);
    if(strm.eof()) break;
  }
}

void Unit::TransformWeights(const SimpleMathSpec& trans, Projection* prjn) {
  Con_Group* cg;
  int g;
  FOR_ITR_GP(Con_Group, cg, recv., g) {
    if(cg->prjn->from->lesion || ((prjn) && (cg->prjn != prjn))) continue;
    cg->TransformWeights(trans);
  }
}

void Unit::AddNoiseToWeights(const Random& noise_spec, Projection* prjn) {
  Con_Group* cg;
  int g;
  FOR_ITR_GP(Con_Group, cg, recv., g) {
    if(cg->prjn->from->lesion || ((prjn) && (cg->prjn != prjn))) continue;
    cg->AddNoiseToWeights(noise_spec);
  }
}

int Unit::PruneCons(const SimpleMathSpec& pre_proc, CountParam::Relation rel,
		       float cmp_val, Projection* prjn)
{
  int rval = 0;
  int g;
  for(g=0; g<recv.gp.size; g++) {
    Con_Group* cg = (Con_Group*)recv.gp.FastEl(g);
    if(cg->prjn->from->lesion || ((prjn) && (cg->prjn != prjn))) continue;
    rval += cg->PruneCons(this, pre_proc, rel, cmp_val);
  }
  n_recv_cons -= rval;
  return rval;
}

int Unit::LesionCons(float p_lesion, bool permute, Projection* prjn) {
  int rval = 0;
  int g;
  for(g=0; g<recv.gp.size; g++) {
    Con_Group* cg = (Con_Group*)recv.gp.FastEl(g);
    if(cg->prjn->from->lesion || ((prjn) && (cg->prjn != prjn))) continue;
    rval += cg->LesionCons(this, p_lesion, permute);
  }
  n_recv_cons -= rval;
  return rval;
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

void ProjectionSpec::CutLinks() {
  Network* net = (Network *) GET_MY_OWNER(Network);
  if((net) && !net->deleting) {
    ProjectionSpec* rsp = (ProjectionSpec*)net->specs.FindSpecTypeNotMe(GetTypeDef(), this);
    if(rsp == NULL) {
      rsp = (ProjectionSpec*)net->specs.FindSpecInheritsNotMe(&TA_ProjectionSpec, this);
    }
    if(rsp) {
      int cnt = net->ReplacePrjnSpec(this, rsp);
      if(cnt > 0) {
	taMisc::Error("Warning: ProjectionSpec",this->GetPath(),"was used in the network, replaced with",rsp->GetPath());
      }
    }
    else {
      taMisc::Error("Warning: Deleting ProjectionSpec",this->GetPath(),"and couldn't find replacement - network will have NULL spec and crash!");
    }
  }
  BaseSpec::CutLinks();
}

int ProjectionSpec::UseCount() {
  Network* net = (Network *) GET_MY_OWNER(Network);
  if(!net) return -1;
  int cnt = net->ReplacePrjnSpec(this, this);
  return cnt;
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
  InitWtState(prjn);
  prjn->projected = true;
}

int ProjectionSpec::ProbAddCons(Projection*, float, float) {
  return 0;
}

void ProjectionSpec::InitWtState(Projection* prjn) {
  Unit* u;
  taLeafItr i;
  FOR_ITR_EL(Unit, u, prjn->layer->units., i) {
    int g;
    for(g=0; g < u->recv.gp.size; g++) {
      Con_Group* cg = (Con_Group*)u->recv.gp.FastEl(g);
      if(cg->prjn == prjn)
	cg->InitWtState(u);
    }
  }
}

void ProjectionSpec::InitWtState_post(Projection* prjn) {
  Unit* u;
  taLeafItr i;
  FOR_ITR_EL(Unit, u, prjn->layer->units., i) {
    int g;
    for(g=0; g < u->recv.gp.size; g++) {
      Con_Group* cg = (Con_Group*)u->recv.gp.FastEl(g);
      if(cg->prjn == prjn)
	cg->InitWtState_post(u);
    }
  }
}

void ProjectionSpec::C_InitWtState(Projection*, Con_Group* cg, Unit* ru) {
  // default is just to do same thing as the conspec would have done..
  CON_GROUP_LOOP(cg, cg->C_InitWtState(cg->Cn(i), ru, cg->Un(i)));
}

void ProjectionSpec::InitWtDelta(Projection* prjn) {
  Unit* u;
  taLeafItr i;
  FOR_ITR_EL(Unit, u, prjn->layer->units., i) {
    int g;
    for(g=0; g < u->recv.gp.size; g++) {
      Con_Group* cg = (Con_Group*)u->recv.gp.FastEl(g);
      if(cg->prjn == prjn)
	cg->InitWtDelta(u);
    }
  }
}

// reconnect based on data in the recv groups only (ie after loading)
void ProjectionSpec::ReConnect_Load(Projection* prjn) {
//  prjn->SetFrom();		// justin case

  if((prjn->from == NULL) || (prjn->layer == NULL))
    return;

  Con_Group* send_gp;
  Con_Group* recv_gp;
  Unit* ru, *su;
  taLeafItr ru_itr;
  FOR_ITR_EL(Unit, ru, prjn->layer->units., ru_itr) {
    if(ru->spec.spec == NULL)
      taMisc::Error("Spec is NULL in Unit:",ru->GetPath(),"will crash if not fixed!");
    int rg;
    FOR_ITR_GP(Con_Group, recv_gp, ru->recv., rg) {
      if(recv_gp->prjn != prjn)
	continue;
      if(recv_gp->spec.spec == NULL)
	taMisc::Error("Spec is NULL in Con_Group:",recv_gp->GetPath(),"will crash if not fixed!");
      int j;
      for(j=0; j<recv_gp->units.size; j++) {
	su = recv_gp->Un(j);
	if(su == NULL)
	  continue;

	if(recv_gp->other_idx >= 0)
	  send_gp = (Con_Group*)su->send.SafeGp(recv_gp->other_idx);
	else
	  send_gp = NULL;
	if(send_gp == NULL)
	  send_gp = su->send.FindPrjn(prjn);
	if(send_gp) {
	  if(send_gp->spec.spec == NULL)
	    taMisc::Error("Spec is NULL in Con_Group:",send_gp->GetPath(),"will crash if not fixed!");
	  send_gp->own_cons = false; // sender doesn't own
	  send_gp->units.LinkUnique(ru);
	  send_gp->LinkUnique(recv_gp->Cn(j));
	}
      }
    }
  }
}

void ProjectionSpec::CopyNetwork(Network* net, Network* cn, Projection* prjn, Projection* cp) {
  MemberDef* md;
  String path;
  if(cp->from) {
    path = cp->from->GetPath(NULL, cn); // path of old layer
    Layer* nw_lay = (Layer*)net->FindFromPath(path, md);	// find under nw net
    if(nw_lay)
      taBase::SetPointer((TAPtr*)&(prjn->from), nw_lay);
  }
  prjn->recv_idx = cp->recv_idx;
  prjn->send_idx = cp->send_idx;
  prjn->recv_n = cp->recv_n;
  prjn->send_n = cp->send_n;
  prjn->projected = cp->projected;
}

void Projection::CopyPtrs(Projection* cp) {
  taBase::SetPointer((TAPtr*)&from, cp->from);
  recv_idx = cp->recv_idx;
  send_idx = cp->send_idx;
  recv_n = cp->recv_n;
  send_n = cp->send_n;
  projected = cp->projected;
}

void ProjectionSpec::PreConnect(Projection* prjn) {
  if(prjn->from == NULL)	return;

  // make first set of congroups to get indicies
  Unit* first_ru = (Unit*)prjn->layer->units.Leaf(0);
  Unit* first_su = (Unit*)prjn->from->units.Leaf(0);
  if((first_ru == NULL) || (first_su == NULL))
    return;
  Con_Group* recv_gp = first_ru->recv.NewPrjn(prjn, true);
  prjn->recv_idx = first_ru->recv.gp.size - 1;
  Con_Group* send_gp = first_su->send.NewPrjn(prjn, false);
  prjn->send_idx = first_su->send.gp.size - 1;
  // set reciprocal indicies
  recv_gp->other_idx = prjn->send_idx;
  send_gp->other_idx = prjn->recv_idx;

  // then crank out for remainder of units..
  Unit* u;
  taLeafItr i;
  FOR_ITR_EL(Unit, u, prjn->layer->units., i) {
    if(u == first_ru)	continue; // skip over first one..
    recv_gp = u->recv.NewPrjn(prjn, true);
    recv_gp->other_idx = prjn->send_idx;
  }
  FOR_ITR_EL(Unit, u, prjn->from->units., i) {
    if(u == first_su)	continue; // skip over first one..
    send_gp = u->send.NewPrjn(prjn, false);
    send_gp->other_idx = prjn->recv_idx;
  }
}

bool ProjectionSpec::CheckConnect(Projection* prjn, bool quiet) {
  if(!prjn->projected) {
    if(!quiet)
      taMisc::Error("Projection CheckConnect: ", prjn->name, "is not connected!");
    return false;
  }
  if(prjn->con_spec.spec == NULL) {
    if(!quiet)
      taMisc::Error("Projection CheckConnect: ", prjn->name, "has null con_spec");
    return false;
  }
  if(!prjn->con_spec->CheckObjectType(prjn)) {
    if(!quiet)
      taMisc::Error("Projection CheckConnect: ", prjn->name, "does not have correct spec/object type");
    return false;
  }
  return true;
}

////////////////////////
//	Projection    //
////////////////////////

void Projection::Initialize() {
  layer = from = NULL;
  from_type = PREV;
  con_type = &TA_Connection;
  con_gp_type = &TA_Con_Group;
  recv_idx = -1;
  send_idx = -1;
  recv_n = 1;
  send_n = 1;
  projected = false;
/* obs #ifdef TA_GUI
  proj_points = NULL;
#endif */
}

void Projection::Destroy(){
  CutLinks();
}

void Projection::CutLinks() {
  if(owner == NULL) return;
  if(from) {
    // remove from sending links, being sure to protect against a spurious re-delete
    taBase::Ref(this);
    from->send_prjns.Remove(this);
    taBase::unRef(this);
  }
  RemoveCons();		// remove actual connections
  taBase::DelPointer((TAPtr*)&from);
/*obs #ifdef TA_GUI
  taBase::DelPointer((TAPtr*)&proj_points);
#endif */
  spec.CutLinks();
  con_spec.CutLinks();
  if((layer) && taMisc::gui_active) {
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

  spec.SetDefaultSpec(this);
  con_spec.SetDefaultSpec(this);
  layer = GET_MY_OWNER(Layer);
  if(mynet) {
    int myindex = mynet->layers.FindLeaf(layer);
    if(!(myindex == 0) && (from_type == PREV)) // is it not the first?
      UpdateAfterEdit();
  }
}

void Projection::Copy_(const Projection& cp) {
  from_type = cp.from_type;
  taBase::SetPointer((TAPtr*)&from, cp.from);
  spec = cp.spec;
  con_type = cp.con_type;
  con_gp_type = cp.con_gp_type;
  con_spec = cp.con_spec;
  // not to copy!
  // recv_idx = cp.recv_idx;
  // send_idx = cp.send_idx;
  // recv_n = cp.recv_n;
  // send_n = cp.send_n;
  // projected = cp.projected;
}

void Projection::UpdateAfterEdit() {
  inherited::UpdateAfterEdit();
  spec.CheckSpec();
  con_spec.CheckSpec();
  SetFrom();
  if(from)
    name = "Fm_" + from->name;
  if(taMisc::is_loading) return;
  ApplyConSpec();
  CheckTypes_impl(false);	// not quiet
  if(!taMisc::gui_active) return;
  Network* net = GET_MY_OWNER(Network);
  if(!net) return;
/*TODO  NetView* view;
  taLeafItr i;
  FOR_ITR_EL(NetView, view, net->views., i) {
    if(view->display_toggle) {
      view->FixProjection(this);
      view->UpdateButtons();
    }
  }*/
}

/*obs void Projection::GridViewWeights(GridLog* disp_log, bool use_swt, int un_x, int un_y, int wt_x, int wt_y) {
  if(disp_log == NULL) {
    disp_log = (GridLog*) pdpMisc::GetNewLog(GET_MY_OWNER(ProjectBase), &TA_GridLog);
    if(disp_log == NULL) return;
  }
  else {
    disp_log->Clear();
/ *TODO    LogView* lv = (LogView*)disp_log->views()->SafeEl(0);
    if((lv == NULL) || !lv->display_toggle || !lv->IsMapped())
      return; * /
  }
/ *TODO: replace subgroup usage with a Matrix-type col instead
  disp_log->SetName((String)"GridViewWeights: " + layer->GetName() + ", " + GetName());

  DataTable* md = &(disp_log->data);
  md->RemoveAll();

  md->NewColString("row");

  int rx = (un_x < 0) ? layer->act_geom.x : un_x;
  int ry = (un_y < 0) ? layer->act_geom.y : un_y;

  int wtx = (wt_x < 0) ? from->act_geom.x : wt_x;
  int wty = (wt_y < 0) ? from->act_geom.y : wt_y;

  int totn = wtx * wty;

  // these are the columns
  int i;
  for(i=0; i<rx; i++) {
    md->NewGroupFloat((String)"c" + (String)i, totn);
    md->AddColDispOpt(String("GEOM_X=") + String(wtx), 0, i); // column 0, subgp i
    md->AddColDispOpt(String("GEOM_Y=") + String(wty), 0, i); // column 0, subgp i
  }

  md->ResetData();

  for(i=0;i<ry;i++) {
    md->AddBlankRow();		// add the rows
    md->SetStringVal(String(ry - i - 1), 0, i);	// col 0, row i
  }

  int uni = 0;
  int ux, uy;
  for(uy=0;uy<ry; uy++) {
    if(uni >= layer->units.leaves)
      break;
    for(ux=0; ux<layer->act_geom.x; ux++, uni++) {
      if(uni >= layer->units.leaves)
	break;
      if(ux >= rx) {
	uni += layer->act_geom.x - rx; break;
      }
      Unit* un = (Unit*)layer->units.Leaf(uni);
      Con_Group* cg = un->recv.FindFrom(from);
      if(cg == NULL)
	break;
      int wi;
      for(wi=0;wi<cg->size;wi++) {
	Unit* su = cg->Un(wi);
	Unit_Group* ownr = ((Unit_Group*)su->owner);
	int sx, sy;
	sx = su->pos.x + ownr->pos.x;
	sy = su->pos.y + ownr->pos.y;
	if((sx >= wtx) || (sy >= wty)) continue;
	int sidx = (sy * wtx) + sx;
	float wtval = cg->Cn(wi)->wt;
	if(use_swt) {
	  Connection* cn = cg->FindRecipSendCon(un, su);
	  if(cn == NULL) continue;
	  wtval = cn->wt;
	}
	md->SetFloatVal(wtval, sidx, ry-uy-1, ux);
      }
    }
  }

  disp_log->ViewAllData();
* /
}*/

void Projection::WeightsToTable(DataTable* dt) {
  if(from == NULL) return;
/*TODO
  if (dt == NULL) {
    dt = pdpMisc::GetNewEnv(GET_MY_OWNER(ProjectBase));
  }
  if(env == NULL) return;

  env->events.Reset();
  env->SetName((String)"WeightsToEnv: " + layer->GetName() + ", " + GetName());

  EventSpec* es = env->GetAnEventSpec();
  es->UpdateAfterEdit();	// make sure its all done with internals..
  es->patterns.EnforceSize(1);

  PatternSpec* ps = (PatternSpec*)es->patterns[0];
  ps->n_vals = from->units.leaves;
  ps->geom = from->act_geom;
  ps->UpdateAfterEdit();	// this will sort out cases where nvals > geom
  es->UpdateAllEvents();	// get them all straightened out

  int idx = 0;
  taLeafItr ri;
  Unit* ru;
  FOR_ITR_EL(Unit, ru, layer->units., ri) {
    Con_Group* cg = ru->recv.FindFrom(from);
    if(cg == NULL)
      break;
    Event* ev = (Event*)env->events.NewEl(1);
    if(!ru->name.empty())
      ev->name = ru->name;
    else
      ev->name = layer->name + String("[") + String(idx) + String("]");
    Pattern* pat = (Pattern*)ev->patterns[0];
    int wi;
    for(wi=0;wi<cg->size;wi++) {
      pat->value.Set(wi, cg->Cn(wi)->wt);
    }
    idx++;
  }
  //TODO: in v4, clients like env must add their own datalink to net and act on the DataXXX events
//  env->InitAllViews(); */
}
/*obs #ifdef TA_GUI
void Projection::SetFromPoints(float x1,float y1){
  if(proj_points == NULL){
    proj_points = new Xform();
    taBase::Own(proj_points,this);
  }
  proj_points->a00 = x1;
  proj_points->a01 = y1;
}

void Projection::SetToPoints(float x1,float y1){
  if(proj_points == NULL){
    proj_points = new Xform();
    taBase::Own(proj_points,this);
  }
  proj_points->a10 = x1;
  proj_points->a11 = y1;
}
#endif */
void Projection::SetFrom() {
  if(layer == NULL) {
    from = NULL;
    return;
  }
  Network* mynet = layer->own_net;
  if(mynet == NULL)
    return;
  int myindex = mynet->layers.FindLeaf(layer);

  switch(from_type) { // this is where the projection is coming from
  case NEXT:
    if (myindex == (mynet->layers.leaves - 1)) { // is it the last layer
      taMisc::Error("*** Last Layer projects from NEXT layer in prjn:",
		    name);
      return;
    }
    else {
      Layer* nwly = (Layer*)mynet->layers.Leaf(myindex+1);
      if(from == nwly) return;
      taBase::SetPointer((TAPtr*)&from, nwly);
    }
    break;
  case PREV:
    if (myindex == 0) { // is it the first
      taMisc::Error("*** First Layer recieves projection from PREV layer in prjn:",
		    name);
      return;
    }
    else {
      Layer* nwly = (Layer*)mynet->layers.Leaf(myindex-1);
      if(from == nwly) return;
      taBase::SetPointer((TAPtr*)&from, nwly);
    }
    break;
  case SELF:
    if(from == layer) return;
    taBase::SetPointer((TAPtr*)&from, layer);
    break;
  case CUSTOM:
    if(from == NULL) {
      taMisc::Error("*** Warning: CUSTOM projection and from is NULL in prjn:",
		    name, "in layer:", layer->name);
    }
    break;
  }
  mynet->UpdtAfterNetMod();
}

void Projection::SetCustomFrom(Layer* fm_lay) {
  taBase::SetPointer((TAPtr*)&from, fm_lay);
  if(fm_lay == layer)
    from_type = SELF;
  else
    from_type = CUSTOM;
  UpdateAfterEdit();
}

bool Projection::SetConSpec(ConSpec* sp) {
  if(sp == NULL)	return false;
  con_spec.SetSpec(sp);
  return ApplyConSpec();
}

bool Projection::SetConType(TypeDef* td) {
  if(con_type == td) return false;
  projected = false;
  con_type = td;
  return true;
}

bool Projection::SetConGpType(TypeDef* td) {
  if(con_gp_type == td) return false;
  projected = false;
  con_gp_type = td;
  return true;
}

bool Projection::ApplyConSpec() {
  if((layer == NULL) || (from == NULL)) return false;
  Unit* u;
  taLeafItr i;
  FOR_ITR_EL(Unit, u, layer->units., i) {
    int g;
    for(g=0; g<u->recv.gp.size; g++) {
      Con_Group* recv_gp = (Con_Group*)u->recv.gp.FastEl(g);
      if(recv_gp->prjn == this) {
	if(con_spec.spec->CheckObjectType(recv_gp))
	  recv_gp->spec.SetSpec(con_spec.spec);
	else
	  return false;
      }
    }
  }
  // also do the from!
  FOR_ITR_EL(Unit, u, from->units., i) {
    int g;
    for(g=0; g<u->send.gp.size; g++) {
      Con_Group* send_gp = (Con_Group*)u->send.gp.FastEl(g);
      if(send_gp->prjn == this) {
	if(con_spec.spec->CheckObjectType(send_gp))
	  send_gp->spec.SetSpec(con_spec.spec);
	else
	  return false;
      }
    }
  }
  return true;
}

void Projection::FixIndexes() {
  if((layer == NULL) || (from == NULL)) return;
  Unit* u;
  taLeafItr i;
  FOR_ITR_EL(Unit, u, layer->units., i) {
    int g;
    for(g=0; g<u->recv.gp.size; g++) {
      Con_Group* recv_gp = (Con_Group*)u->recv.gp.FastEl(g);
      if(recv_gp->prjn == this) {
	recv_gp->other_idx = send_idx;
      }
    }
  }
  FOR_ITR_EL(Unit, u, from->units., i) {
    int g;
    for(g=0; g<u->send.gp.size; g++) {
      Con_Group* send_gp = (Con_Group*)u->send.gp.FastEl(g);
      if(send_gp->prjn == this) {
	send_gp->other_idx = recv_idx;
      }
    }
  }
}

int Projection::ReplaceConSpec(ConSpec* old_sp, ConSpec* new_sp) {
  if(con_spec.spec != old_sp) return 0;
  taBase::SetPointer((TAPtr*)&con_spec.spec, new_sp);
//   con_spec.SetSpec(new_sp);
  return 1;
}

int Projection::ReplacePrjnSpec(ProjectionSpec* old_sp, ProjectionSpec* new_sp) {
  if(spec.spec != old_sp) return 0;
  taBase::SetPointer((TAPtr*)&spec.spec, new_sp);
//   spec.SetSpec(new_sp);
  return 1;
}


void Projection::ReConnect_Load() {
  if(spec.spec == NULL) {
    taMisc::Error("Spec is NULL in projection:",GetPath(),"will crash if not fixed!");
    return;
  }
  spec->ReConnect_Load(this);
}

bool Projection::CheckTypes(bool quiet) {
  if(!spec.CheckSpec())
    return false;
  if(!CheckTypes_impl(quiet)) {
    if(!quiet)
      taMisc::Error("Projection CheckTypes: Connection, Con_Group, or spec types for projection:",GetPath(),
		    "are not correct, perform 'Connect' to rectify");
    return false;
  }
  return true;
}

bool Projection::CheckTypes_impl(bool quiet) {
  if(!layer || !from) return true; // silent bail for unconnected?
  if(!spec.spec) {
    if(!quiet)
      taMisc::Error("Projection CheckTypes: spec is null for projection:",GetPath());
    return false;
  }
  Unit* u;
  taLeafItr i;
  FOR_ITR_EL(Unit, u, layer->units., i) {
    int g;
    for(g=0; g<u->recv.gp.size; g++) {
      Con_Group* recv_gp = (Con_Group*)u->recv.gp.FastEl(g);
      if(recv_gp->prjn == this) {
	if(!con_spec.CheckSpec(recv_gp)) {
	  projected = false;
	  return false;
	}
	if(recv_gp->GetTypeDef() != con_gp_type) {
	  if(!quiet)
	    taMisc::Error("Projection CheckTypes: recv_gp type does not match con_gp_type for projection:",GetPath(), "type should be:", con_gp_type->name);
	  projected = false;
	  return false;
	}
	if(recv_gp->el_typ != con_type) {
	  if(!quiet)
	    taMisc::Error("Projection CheckTypes: recv connection type does not match con_type for projection:",GetPath(), "type should be:", con_type->name);
	  projected = false;
	  return false;
	}
      }
    }
  }
  // also do the from!
  FOR_ITR_EL(Unit, u, from->units., i) {
    int g;
    for(g=0; g<u->send.gp.size; g++) {
      Con_Group* send_gp = (Con_Group*)u->send.gp.FastEl(g);
      if(send_gp->prjn == this) {
	if(!con_spec.CheckSpec(send_gp)) {
	  projected = false;
	  return false;
	}
	if(send_gp->GetTypeDef() != con_gp_type) {
	  if(!quiet)
	    taMisc::Error("Projection CheckTypes: send_gp type does not match con_gp_type for projection:",GetPath(), "type should be:", con_gp_type->name);
	  projected = false;
	  return false;
	}
	if(send_gp->el_typ != con_type) {
	  if(!quiet)
	    taMisc::Error("Projection CheckTypes: send connection type does not match con_type for projection:",GetPath(), "type should be:", con_type->name);
	  projected = false;
	  return false;
	}
      }
    }
  }
  return true;
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

void Projection::WriteWeights(ostream& strm, Con_Group::WtSaveFormat fmt) {
  Unit* u;
  taLeafItr i;
  FOR_ITR_EL(Unit, u, layer->units., i)
    u->WriteWeights(strm, this, fmt);
}

void Projection::ReadWeights(istream& strm, Con_Group::WtSaveFormat fmt) {
  Unit* u;
  taLeafItr i;
  FOR_ITR_EL(Unit, u, layer->units., i) {
    u->ReadWeights(strm, this, fmt);
    if(strm.eof()) break;
  }
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
			      CountParam::Relation rel, float cmp_val)
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
  n_units = 0;
  geom.z = 1;
  units_lesioned = false;
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
  n_units = cp.n_units;
  pos = cp.pos;
  geom = cp.geom;
}

void Unit_Group::UpdateAfterEdit() {
  taGroup<Unit>::UpdateAfterEdit();
  if((own_lay == NULL) || (own_lay->own_net == NULL)) return;
  RecomputeGeometry();
  if(taMisc::is_loading || !taMisc::gui_active) return;
/*TODO  NetView* view;
  taLeafItr i;
  FOR_ITR_EL(NetView, view, own_lay->own_net->views., i) {
    if(view->display_toggle)
      view->FixUnitGroup(this, own_lay);
  }*/
}

void Unit_Group::RemoveAll() {
  units_lesioned = false;
  int i;
  for(i=0; i<size; i++) {
    if(FastEl(i)->owner == this)
      FastEl(i)->pos.z = -1;		// signal not to update display when units are removed
  }
  inherited::RemoveAll();
/*obs  // then update here
  if(taMisc::gui_active && (owner) && (own_lay) &&
     (own_lay->own_net) && !own_lay->own_net->net_will_updt)
    own_lay->own_net->InitAllViews(); */
}

void Unit_Group::RecomputeGeometry() {
  if((own_lay == NULL) || (own_lay->own_net == NULL)) return;
  int use_n_units = n_units;
  if(n_units == 0) {
    geom = own_lay->geom;
    geom.z = 1;
    use_n_units = own_lay->n_units;
  }
  if((use_n_units == 0) && (size > 0)) {
    use_n_units = size;
    n_units = size;
  }
  geom.FitNinXY(use_n_units);
  if (use_n_units == 0) {
    use_n_units = geom.x * geom.y;
    n_units = geom.x * geom.y;
  }
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
      if((un) && (u==NULL || un==u)) un->pos = mygeo;
      if(un==u) break;
    }
    if(un==u) break;
  }
}

bool Unit_Group::Build() {
  if((own_lay == NULL) || (owner == own_lay))	return false;
  StructUpdate(true);
  RecomputeGeometry();
  int use_n_units = n_units;
  if(n_units == 0)
    use_n_units = own_lay->n_units;
  bool units_changed = false;
  if(size != use_n_units)
    units_changed = true;
  EnforceSize(use_n_units);
  EnforceType();
  Unit* u;
  taLeafItr i;
  FOR_ITR_EL(Unit, u, this->, i)
    u->Build();
  units_lesioned = false;
  StructUpdate(false);
  return units_changed;
}

bool Unit_Group::CheckBuild(bool quiet) {
  if(n_units == 0) {
    if(own_lay == NULL) return false;		      // was true
    if(!units_lesioned && (size != own_lay->n_units)) {
      if(!quiet)
	taMisc::Error("Unit_Group CheckBuild: no units and this is not what is specified in group",
		      GetPath(),"in layer:", own_lay->name);
      return false;
    }
  }
  else if(!units_lesioned && (size != n_units)) {
    if(!quiet)
      taMisc::Error("Unit_Group CheckBuild: number of units != target in group",
		    GetPath(),"in layer:", own_lay->name);
    return false;
  }
  return true;
}

bool Unit_Group::SetUnitSpec(UnitSpec* sp) {
  if(sp == NULL)	return false;
  Unit* u;
  taLeafItr i;
  FOR_ITR_EL(Unit, u, this->, i) {
    if(sp->CheckObjectType(u))
      u->spec.SetSpec(sp);
    else
      return false;
  }
  return true;
}

bool Unit_Group::SetConSpec(ConSpec* sp) {
  if(sp == NULL)	return false;
  Unit* u;
  taLeafItr i;
  FOR_ITR_EL(Unit, u, this->, i) {
    if(!u->SetConSpec(sp))
      return false;
  }
  return true;
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

void Unit_Group::WriteWeights(ostream& strm, Con_Group::WtSaveFormat fmt) {
  Unit* u;
  taLeafItr i;
  FOR_ITR_EL(Unit, u, this->, i)
    u->WriteWeights(strm, NULL, fmt);
}

void Unit_Group::ReadWeights(istream& strm, Con_Group::WtSaveFormat fmt) {
  Unit* u;
  taLeafItr i;
  FOR_ITR_EL(Unit, u, this->, i) {
    if(strm.eof()) break;
    u->ReadWeights(strm, NULL, fmt);
  }
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
			CountParam::Relation rel, float cmp_val)
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
      un->pos.z = -1;		// don't update yet!
      RemoveLeaf(un);
    }
  }
  else {
    int j;
    for(j=leaves-1; j>=0; j--) {
      if(Random::ZeroOne() <= p_lesion) {
	Unit* un = (Unit*)Leaf(j);
	un->DisConnectAll();
	un->pos.z = -1;		// don't update yet!
	RemoveLeaf(j);
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
  return rval;
}

// todo: replace with matrix versions
void Unit_Group::UnitValuesToArray(float_RArray& ary, const char* variable) {
  MemberDef* md = el_typ->members.FindName(variable);
  if((md == NULL) || !md->type->InheritsFrom(TA_float)) {
    taMisc::Error("*** Variable:", variable, "not found or not a float on units of type:",
		   el_typ->name, "in UnitValuesToArray()");
    return;
  }
  Unit* u;
  taLeafItr i;
  FOR_ITR_EL(Unit, u, this->, i) {
    float* val = (float*)md->GetOff((void*)u);
    ary.Add(*val);
  }
}

void Unit_Group::UnitValuesFromArray(float_RArray& ary, const char* variable) {
  if(ary.size == 0) return;
  MemberDef* md = el_typ->members.FindName(variable);
  if((md == NULL) || !md->type->InheritsFrom(TA_float)) {
    taMisc::Error("*** Variable:", variable, "not found or not a float on units of type:",
		   el_typ->name, "in UnitValuesToArray()");
    return;
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
}

Unit* Unit_Group::FindUnitFmCoord(int x, int y) {
  if (( x < 0) || (x >= geom.x) || (y < 0) || (y >= geom.y)) return NULL;
  int idx = y * geom.x + x;
  if (idx < size)
    return FastEl(idx);
  return NULL;
}

bool Unit_Group::Dump_QuerySaveChildren() {
  ProjectBase* prj = GET_MY_OWNER(ProjectBase);
  if(prj && prj->save_rmv_units)
    return false;
  return true;
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
  Network* net = (Network *) GET_MY_OWNER(Network);
  if(net && !net->deleting) {
    LayerSpec* rsp = (LayerSpec*)net->specs.FindSpecTypeNotMe(GetTypeDef(), this);
    if(rsp == NULL) {
      rsp = (LayerSpec*)net->specs.FindSpecInheritsNotMe(&TA_LayerSpec, this);
    }
    if(rsp) {
      int cnt = net->ReplaceLayerSpec(this, rsp);
      if(cnt > 0) {
	taMisc::Error("Warning: LayerSpec",this->GetPath(),"was used in the network, replaced with",rsp->GetPath());
      }
    }
    else {
      taMisc::Error("Warning: Deleting LayerSpec",this->GetPath(),"and couldn't find replacement - network will have NULL spec and crash!");
    }
  }
  BaseSpec::CutLinks();
}

int LayerSpec::UseCount() {
  Network* net = (Network *) GET_MY_OWNER(Network);
  if(!net) return -1; // no networks!
  int cnt = net->ReplaceLayerSpec(this, this);
  return cnt;
}


void Layer::Initialize() {
  own_net = NULL;
  layer_type = HIDDEN;
  projections.SetBaseType(&TA_Projection);
  units.SetBaseType(&TA_Unit);
  n_units = 0;
  lesion = false;
  ext_flag = Unit::NO_EXTERNAL;
  geom.y =1;  geom.z =1;  geom.x =0;
  act_geom = geom;
  dmem_dist = DMEM_DIST_DEFAULT;
}

void Layer::InitLinks() {
  inherited::InitLinks();
  taBase::Own(unit_spec, this);
  taBase::Own(units, this);
  taBase::Own(projections, this);
  taBase::Own(send_prjns, this);
  taBase::Own(pos, this);
  taBase::Own(geom, this);
  taBase::Own(gp_geom, this);
  taBase::Own(gp_spc, this);
  taBase::Own(flat_geom, this);
  taBase::Own(act_geom, this);
  taBase::Own(sent_already, this);
#ifdef DMEM_COMPILE
  taBase::Own(dmem_share_units, this);
#endif
  unit_spec.SetDefaultSpec(this);
  own_net = GET_MY_OWNER(Network);
  SetDefaultPos();
  units.pos.z = 0;
}

void Layer::CutLinks() {
  static bool in_repl = false;
  if (in_repl || (owner == NULL)) return; // already replacing or already dead
  DisConnect();
  sent_already.CutLinks();
  act_geom.CutLinks();
  flat_geom.CutLinks();
  gp_spc.CutLinks();
  gp_geom.CutLinks();
  geom.CutLinks();
  pos.CutLinks();
  send_prjns.CutLinks();
  projections.CutLinks();
  units.CutLinks();
  unit_spec.CutLinks();
  inherited::CutLinks();
}


void Layer::Copy_(const Layer& cp) {
  layer_type = cp.layer_type;
  n_units = cp.n_units;
  geom = cp.geom;
  pos = cp.pos;
  pos.x+=2; // move it over so it can be seen!
  gp_geom = cp.gp_geom;
  gp_spc = cp.gp_spc;
  flat_geom = cp.flat_geom;
  act_geom = cp.act_geom;
  projections = cp.projections;
  units = cp.units;
  unit_spec = cp.unit_spec;
  lesion = cp.lesion;
  ext_flag = cp.ext_flag;

  // not copied
  //  send_prjns.BorrowUnique(cp.send_prjns); // link group
}

void Layer::UpdateAfterEdit() {
  //  spec.CheckSpec();  // ADD THIS TO ANY LAYERS WITH SPECS!
  unit_spec.CheckSpec();
  // no negative geoms., y,z must be 1 (for display)
  SyncSendPrjns();
  RecomputeGeometry();

  inherited::UpdateAfterEdit();
  if(taMisc::is_loading) return;
  if (own_net == NULL) return;
  own_net->UpdtAfterNetMod();

  if(!taMisc::gui_active) return;
/*TODO  NetView* view;
  taLeafItr i;
  FOR_ITR_EL(NetView, view, own_net->views., i) {
    if(view->display_toggle)
      view->FixLayer(this);
  }*/
}

void Layer::ApplyExternal(taMatrix* data, Unit::ExtType ext_flags,
    Random* ran, const PosTwoDCoord* offset) 
{
  // note: when use LayerWriters, we typically always just get a single frame of \
  // the exact dimensions, and so ignore 'frame'
  if (!data) return;
  // check correct geom of data
  if ((data->dims() != 2) && (data->dims() != 4)) {
    taMisc::Error("Layer::ApplyExternal: data->dims must be 2 (2-d) or 4 (4-d); is: ",
      String(data->dims()));
    return;
  }
  
  TxferDataStruct ads(data, ext_flags, ran, offset);
  // TODO determine effective offset
  
  // apply flags if we are the controller (zero offset)
  if ((ads.offs_x == 0) && (ads.offs_y == 0)) {
    ApplyLayerFlags(ext_flags);
  }
  // apply data according to the applicable model
  DataUpdate(true);
  if (uses_groups()) {
    if (data->dims() == 4) {
      ApplyExternal_Gp4d(ads);
    } else {
      ApplyExternal_Gp2d(ads);
    }
  } else {
    if (data->dims() == 4) {
      ApplyExternal_Flat4d(ads);
    } else {
      ApplyExternal_Flat2d(ads);
    }
  }
  DataUpdate(false);
}

void Layer::ApplyExternal_Flat2d(const TxferDataStruct& ads) {
  Unit* un; 	// current unit
  int u_x = ads.offs_x;  int u_y = ads.offs_y; // current unit coord
  int d_x = 0;  int d_y = 0; // current data coord
  Unit_Group* ug = &(this->units);
  float val;
  //note: a partially filled this will return a null unit when we reach the end of actual units
  while (u_y < this->geom.y) {
    while (u_x < this->geom.x) {
      // if we've run out of data for this row, go to next row
      if (d_x >= ads.data->dim(0)) break;
      un = ug->FindUnitFmCoord(u_x, u_y);
      // if we run out of units, there will be no more, period
      if (un == NULL) goto break1;
      val = ads.data->SafeElAsVar(d_x, d_y).toFloat();
      un->ApplyExternal(val, ads.ext_flags, ads.ran);
      ++d_x;
      ++u_x;
    }
    ++d_y;
    if (d_y >= ads.data->dim(1)) break;
    d_x = 0;
    u_x = ads.offs_x;
    ++u_y;
  }
break1:
  ;
}

void Layer::ApplyExternal_Flat4d(const TxferDataStruct& ads) {
  //TODO:
}

void Layer::ApplyExternal_Gp2d(const TxferDataStruct& ads) {
  //TODO:
}

void Layer::ApplyExternal_Gp4d(const TxferDataStruct& ads) {
  //TODO:
}


void Layer::ApplyLayerFlags(Unit::ExtType act_ext_flags) {
  SetExtFlag(act_ext_flags);
}

void Layer::ConnectFrom(Layer* from_lay) {
  Network* net = GET_MY_OWNER(Network);
  if (!net) return;
  //Projection* prjn =
  net->FindMakePrjn(this, from_lay);
}

void Layer::CopyNetwork(Network* net, Network* cn, Layer* lay) {
  Projection* p;
  taLeafItr pi;
  Projection* cp;
  taLeafItr cpi;
  for(p = (Projection*)projections.FirstEl(pi), cp = (Projection*)lay->projections.FirstEl(cpi);
      p && cp;
      p = (Projection*)projections.NextEl(pi), cp = (Projection*)lay->projections.NextEl(cpi)) {
    p->CopyNetwork(net, cn, cp);
  }
  Unit* u;
  taLeafItr ui;
  Unit* cu;
  taLeafItr cui;
  for(u = (Unit*)units.FirstEl(ui), cu = (Unit*)lay->units.FirstEl(cui);
      u && cu;
      u = (Unit*)units.NextEl(ui), cu = (Unit*)lay->units.NextEl(cui)) {
    u->CopyNetwork(net, cn, cu);
  }
}

void Layer::CopyPtrs(Layer* lay) {
  Projection* p;
  taLeafItr pi;
  Projection* cp;
  taLeafItr cpi;
  for(p = (Projection*)projections.FirstEl(pi), cp = (Projection*)lay->projections.FirstEl(cpi);
      p && cp;
      p = (Projection*)projections.NextEl(pi), cp = (Projection*)lay->projections.NextEl(cpi)) {
    p->CopyPtrs(cp);
  }
  int pip;
  for(pip=lay->send_prjns.size-1; pip>=0; pip--) {
    Projection* p = (Projection*)lay->send_prjns.FastEl(pip);
    if(p == NULL) continue;
    if(p->layer != lay)
      send_prjns.LinkUnique(p);
    else {
      int lfidx = lay->projections.FindEl(p);
      if((lfidx >= 0) && (lfidx < projections.size)) {
	send_prjns.LinkUnique(projections.FastEl(lfidx));
      }
    }
  }
  Unit* u;
  taLeafItr ui;
  Unit* cu;
  taLeafItr cui;
  for(u = (Unit*)units.FirstEl(ui), cu = (Unit*)lay->units.FirstEl(cui);
      u && cu;
      u = (Unit*)units.NextEl(ui), cu = (Unit*)lay->units.NextEl(cui)) {
    u->CopyPtrs(cu);
  }
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
    if((p->layer == NULL) || (p->from != this))
      send_prjns.Remove(pi);	// get rid of it!
  }
}

void Layer::RecomputeGeometry() {
  geom.y = MAX(1,geom.y);  geom.z = MAX(1,geom.z);
  if((n_units == 0) && (units.size > 0))
    n_units = units.size;
  geom.FitNinXY(n_units);
  if(n_units == 0)
    n_units = geom.x * geom.y;
  if (geom.z > 1) {
    gp_geom.FitNinXY(geom.z);	// fit all groups in there..
    flat_geom.x = geom.x * gp_geom.x;
    flat_geom.y = geom.y * gp_geom.y;
    //NOTE: act_geom must get computed later....
  } else {
    flat_geom.x = geom.x;
    flat_geom.y = geom.y;
    act_geom = geom;
  }
}

ProjectBase* Layer::project() {
  ProjectBase* rval = NULL;
  if (own_net)
    rval = GET_OWNER(own_net, ProjectBase);
  return rval;
}

bool Layer::SetLayerSpec(LayerSpec*) {
  return false;			// no layer spec for layers!
}

void Layer::SetDefaultPos() {
  if (own_net == NULL) return;
  int index = own_net->layers.FindLeaf(this);
  switch(own_net->lay_layout) {
  case Network::THREE_D:
    pos.z = index; pos.y=0;
    return;
  case Network::TWO_D:
    pos.z = 0;
    pos.y = 0;
    int i;
    for(i=0;i<index;i++) {
      pos.y =
	MAX(pos.y, ((Layer*) own_net->layers.Leaf(i))->pos.y +
	    ((Layer*) own_net->layers.Leaf(i))->geom.y) + 2;
    }
  }
}

void Layer::LayoutUnits(Unit* u) {
  StructUpdate(true);
  RecomputeGeometry();
  units.pos.z = 0;
  if(geom.z > 1) {		// create groups for each z dimension
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
	ungeo.x = ug->pos.x + MAX(ug->geom.x, geom.x) + gp_spc.x;
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
    act_geom = geom;
    units.geom = geom;		// default is to have layer's geom
    int i = 0;
    Unit* un = NULL;
    for(mygeo.y=0; mygeo.y < geom.y; mygeo.y++) {
      for(mygeo.x=0; mygeo.x <geom.x; mygeo.x++) {
	if(i >= units.leaves)
	  break;
	un = (Unit*)units.Leaf(i++);
	if((un) && (u==NULL || un==u)) un->pos = mygeo;
	if(un==u) break;
      }
      if(un==u) break;
    }
  }
  StructUpdate(false);
}

void Layer::Build() {
  taMisc::Busy();
  StructUpdate(true);
  RecomputeGeometry();
  bool units_changed = false;
  geom.z = MAX(geom.z, 1);
  if(geom.z > 1) {		// create groups for each z dimension
    while(units.size > 0) {
      ((Unit*)units.FastEl(units.size-1))->pos.z = -1; // do not update
      units.Remove(units.size-1); // get rid of any in top-level
    }
    units.gp.EnforceSize(geom.z);
    int k;
    for(k=0; k< units.gp.size; k++) {
      Unit_Group* ug = (Unit_Group*)units.gp.FastEl(k);
      ug->UpdateAfterEdit();
      units_changed = ug->Build();
    }
  }
  else {
    units.gp.RemoveAll();	// in case there were any subgroups..
    if(!units_changed && (units.leaves != n_units))
      units_changed = true;
    units.EnforceLeaves(n_units);
    units.EnforceType();
    Unit* u;
    taLeafItr i;
    FOR_ITR_EL(Unit, u, units., i)
      u->Build();
    units.units_lesioned = false;
  }

  LayoutUnits();
  // assign the spec
  taLeafItr i;
  Unit* u;
  FOR_ITR_EL(Unit, u, units.,i){
    u->spec = unit_spec;
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
  StructUpdate(false);
  taMisc::DoneBusy();
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
      ungeo.x = ug->pos.x + MAX(ug->geom.x, geom.x) + gp_spc.x;
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
  if(units.gp.size > 0) {
    int g;
    for(g=0; g<units.gp.size; g++) {
      Unit_Group* ug = (Unit_Group*)units.gp.FastEl(g);
      if(!ug->CheckBuild(quiet))
	return false;
    }
  }
  else {
    if(!units.units_lesioned && (units.size != n_units)) {
      if(!quiet)
	taMisc::Error("Layer CheckBuild: number of units != target in layer", name);
      return false;
    }
  }

  Unit* u;
  taLeafItr ui;
  FOR_ITR_EL(Unit, u, units., ui) {
    if(u->GetTypeDef() != units.el_typ) {
      if(!quiet)
	taMisc::Error("Layer CheckBuild: unit type not correct in layer", name,
		      "syould be:", units.el_typ->name);
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

bool Layer::CheckTypes(bool quiet) {
  // don't check layerspec by default, but layers that have them should!
  Unit* u;
  taLeafItr ui;
  FOR_ITR_EL(Unit, u, units., ui) {
    if(!u->CheckTypes(quiet))
      return false;
  }
  Projection* prjn;
  taLeafItr j;
  FOR_ITR_EL(Projection, prjn, projections.,j) {
    if(!prjn->CheckTypes(quiet)) 
      return false;
  }
  return true;
}

bool Layer::CheckConfig(Network* net, bool quiet) {
  // layerspec should take over this function in layers that have them!
  Unit* u;
  taLeafItr ui;
  FOR_ITR_EL(Unit, u, units., ui) {
    if(!u->CheckConfig(this, net, quiet))
      return false;
  }
  return true;
}

void Layer::FixPrjnIndexes() {
  Projection* p;
  taLeafItr i;
  FOR_ITR_EL(Projection, p, projections., i)
    p->FixIndexes();
}

bool Layer::isSparse() const {
  // not sparse if requesting geom
  if (n_units == 0) return false;
  // if 2d
  // we are sparse if num units not same as flat geom
  return (flat_geom.Product() != units.leaves);
}

int Layer::numUnits() const {
  return units.leaves;
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
    u->Build();			// this is for the bias connections!
  StructUpdate(false);
  taMisc::DoneBusy();
}

void Layer::DisConnect() {
  StructUpdate(true);
  int pi;
  for(pi=send_prjns.size-1; pi>=0; pi--) {
    Projection* p = (Projection*)send_prjns.FastEl(pi);
    if(p == NULL) continue;
    if(p->layer == NULL) {
      send_prjns.Remove(pi);
      continue;
    }
    p->layer->projections.RemoveLeaf(p);
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

void Layer::ReConnect_Load() {
  Projection* p;
  taLeafItr i;
  FOR_ITR_EL(Projection, p, projections., i)
    p->ReConnect_Load();
}

void Layer::InitExterns() {
  if(ext_flag == Unit::NO_EXTERNAL)
    return;
  Unit* u;
  taLeafItr i;
  FOR_ITR_EL(Unit, u, units., i)
    u->InitExterns();
  ext_flag = Unit::NO_EXTERNAL;
}

void  Layer::InitDelta() {
  Unit* u;
  taLeafItr i;
  FOR_ITR_EL(Unit, u, units., i)
    u->InitDelta();
}

void  Layer::InitState() {
  ext_flag = Unit::NO_EXTERNAL;
  Unit* u;
  taLeafItr i;
  FOR_ITR_EL(Unit, u, units., i)
    u->InitState();
}

void  Layer::ModifyState() {
  ext_flag = Unit::NO_EXTERNAL;
  Unit* u;
  taLeafItr i;
  FOR_ITR_EL(Unit, u, units., i)
    u->ModifyState();
}

void  Layer::InitWtDelta() {
  Unit* u;
  taLeafItr i;
  FOR_ITR_EL(Unit, u, units., i)
    u->InitWtDelta();
}

void Layer::InitWtState() {
  Unit* u;
  taLeafItr i;
  FOR_ITR_EL(Unit, u, units., i)
    u->InitWtState();
}

void Layer::InitWtState_post() {
  Unit* u;
  taLeafItr i;
  FOR_ITR_EL(Unit, u, units., i)
    u->InitWtState_post();
}

void Layer::Compute_Net() {
  if(projections.leaves == 0) return; // if no connections, don't do it!
  Unit* u;
  taLeafItr i;
  FOR_ITR_EL(Unit, u, units., i)
    u->Compute_Net();
}

void Layer::Send_Net() {
  if(send_prjns.leaves == 0) return; // no connections, don't do it!
  Unit* u;
  taLeafItr i;
  FOR_ITR_EL(Unit, u, units., i)
    u->Send_Net();
}

void Layer::Send_NetToLay(Layer* tolay) {
  if(send_prjns.leaves == 0) return; // no connections, don't do it!
  Unit* u;
  taLeafItr i;
  FOR_ITR_EL(Unit, u, units., i)
    u->Send_NetToLay(tolay);
}

void Layer::Send_NetToMe() {
  sent_already.Reset();
  Projection* p;
  taLeafItr i;
  FOR_ITR_EL(Projection, p, projections., i) {
    if(p->from->lesion) continue;
    int addr = (int)(long)p->from;
    if(sent_already.Find(addr) >= 0) continue;
    p->from->Send_NetToLay(this);
    sent_already.Add(addr);
  }
}

void Layer::Compute_Act() {
  Unit* u;
  taLeafItr i;
  FOR_ITR_EL(Unit, u, units., i)
    u->Compute_Act();
}
void Layer::UpdateWeights() {
  Unit* u;
  taLeafItr i;
  FOR_ITR_EL(Unit, u, units., i)
    u->UpdateWeights();
}
void Layer::Compute_dWt() {
  Unit* u;
  taLeafItr i;
  FOR_ITR_EL(Unit, u, units., i)
    u->Compute_dWt();
}
float Layer::Compute_SSE() {
  if(!(ext_flag & Unit::TARG)) return 0.0f;
  float sse = 0.0f;
  Unit* u;
  taLeafItr i;
  FOR_ITR_EL(Unit, u, units., i)
    sse += u->Compute_SSE();
  return sse;
}
void Layer::Copy_Weights(const Layer* src) {
  units.Copy_Weights(&(src->units));
}
void Layer::WriteWeights(ostream& strm, Con_Group::WtSaveFormat fmt) {
  if(fmt == Con_Group::TEXT)
    strm << "#Layer " << name << "\n";
  units.WriteWeights(strm, fmt);
}
void Layer::ReadWeights(istream& strm, Con_Group::WtSaveFormat fmt) {
  units.ReadWeights(strm, fmt);
}

void Layer::TransformWeights(const SimpleMathSpec& trans) {
  units.TransformWeights(trans);
}

void Layer::AddNoiseToWeights(const Random& noise_spec) {
  units.AddNoiseToWeights(noise_spec);
}

int Layer::PruneCons(const SimpleMathSpec& pre_proc,
			CountParam::Relation rel, float cmp_val)
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
  return units.LesionUnits(p_lesion, permute);
}

bool Layer::SetUnitSpec(UnitSpec* sp) {
  if(sp == NULL)	return false;
  unit_spec.SetSpec(sp);
  Unit* u;
  taLeafItr i;
  FOR_ITR_EL(Unit, u, units., i) {
    if(sp->CheckObjectType(u))
      u->spec.SetSpec(sp);
    else
      return false;
  }
  return true;
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

bool Layer::SetConSpec(ConSpec* sp) {
  if(sp == NULL)	return false;
  Unit* u;
  taLeafItr i;
  FOR_ITR_EL(Unit, u, units., i) {
    if(!u->SetConSpec(sp))
      return false;
  }
  return true;
}

int Layer::ReplaceUnitSpec(UnitSpec* old_sp, UnitSpec* new_sp) {
  int nchg = 0;
  if(unit_spec.spec == old_sp) {
    taBase::SetPointer((TAPtr*)&unit_spec.spec, new_sp);
    //    unit_spec.SetSpec(new_sp);
    nchg++;
  }
  Unit* u;
  taLeafItr i;
  FOR_ITR_EL(Unit, u, units., i)
    nchg += u->ReplaceUnitSpec(old_sp, new_sp);
  return nchg;
}

int Layer::ReplaceConSpec(ConSpec* old_sp, ConSpec* new_sp) {
  int nchg = 0;
  Unit* u;
  taLeafItr i;
  FOR_ITR_EL(Unit, u, units., i)
    nchg += u->ReplaceConSpec(old_sp, new_sp);
  Projection* p;
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

/*obs void Layer::GridViewWeights(GridLog* grid_log, Layer* send_lay, bool use_swt, int un_x, int un_y, int wt_x, int wt_y) {
  if(send_lay == NULL) return;
  bool gotone = false;
  Projection* p;
  taLeafItr i;
  FOR_ITR_EL(Projection, p, projections., i) {
    if(p->from != send_lay) continue;
    p->GridViewWeights(grid_log, use_swt, un_x, un_y, wt_x, wt_y);
    gotone = true;
  }
  if(!gotone) {
    taMisc::Error("GridViewWeights: No sending projection from: ", send_lay->name);
  }
}*/

void Layer::WeightsToTable(DataTable* dt, Layer* send_lay) {
  if(send_lay == NULL) return;
  bool gotone = false;
  Projection* p;
  taLeafItr i;
  FOR_ITR_EL(Projection, p, projections., i) {
    if(p->from != send_lay) continue;
    p->WeightsToTable(dt);
    gotone = true;
  }
  if(!gotone) {
    taMisc::Error("WeightsToEnv: No sending projection from: ", send_lay->name);
  }
}

Unit* Layer::FindUnitFmCoord(int x, int y) {
  if(units.gp.size == 0)	// no group structure, just do it
    return units.FindUnitFmCoord(x,y);
  int un_x = x % geom.x;
  int un_y = y % geom.y;
  int gpidx = (y / geom.y) * gp_geom.x + (x / geom.x);
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

void Layer::GetActGeomNoSpc(PosTDCoord& nospc_geom) {
  nospc_geom = act_geom;
  if(units.gp.size == 0) return;
  nospc_geom.x -= (gp_geom.x - 1) * gp_spc.x;
  nospc_geom.y -= (gp_geom.y - 1) * gp_spc.y;
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
  if(own_net->dmem_sync_level != Network::DMEM_SYNC_LAYER) {
    taMisc::Error("Error: attempt to DMem sync nrecv_cons at layer level, should only be at network level!");
    return;
  }
  dmem_share_units.Sync(0);
}

void Layer::DMem_SyncNet() {
  if(own_net->dmem_sync_level != Network::DMEM_SYNC_LAYER) {
    taMisc::Error("Error: attempt to DMem sync netin at layer level, should only be at network level!");
    return;
  }
  dmem_share_units.Sync(1);
}

void Layer::DMem_SyncAct() {
  if(own_net->dmem_sync_level != Network::DMEM_SYNC_LAYER) {
    taMisc::Error("Error: attempt to DMem sync act at layer level, should only be at network level!");
    return;
  }
  dmem_share_units.Sync(2);
}
#endif

////////////////////////
//	Network	      //
////////////////////////

void Network::Initialize() {
//TODO  views.SetBaseType(&TA_NetView);
  specs.SetBaseType(&TA_BaseSpec);
  layers.SetBaseType(&TA_Layer);

  context = TEST;
  wt_update = ON_LINE;
  batch_n = 10;
  batch_n_eff = 10;

  batch = 0;
  epoch = 0;
  trial = 0;
  cycle = 0;
  time = 0.0f;

  sse = 0.0f;
  sum_sse = 0.0f;
  avg_sse = 0.0f;
  cnt_err = 0.0f;

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
  max_size.x = GetDefaultX();
  max_size.y = GetDefaultY();
  max_size.z = GetDefaultZ();

  deleting = false;
  copying = false;
  proj = NULL;
#ifdef DMEM_COMPILE
  dmem_gp = -1;
  dmem_share_units.comm = (MPI_Comm)MPI_COMM_SELF;
#endif
}

void Network::InitLinks() {
  proj = GET_MY_OWNER(ProjectBase);
  taBase::Own(specs, this);
  taBase::Own(layers, this);
  taBase::Own(max_size, this);
#ifdef DMEM_COMPILE
  taBase::Own(dmem_share_units, this);
#endif
  inherited::InitLinks();
}

void Network::CutLinks() {
  static bool in_repl = false;
  if(in_repl || (owner == NULL)) return; // already replacing or already dead
  deleting = true;
#ifdef DMEM_COMPILE
  if(dmem_share_units.comm != MPI_COMM_SELF) {
    DMEM_MPICALL(MPI_Comm_free((MPI_Comm*)&dmem_share_units.comm),
		       "Network::CutLinks", "net Comm_free");
    DMEM_MPICALL(MPI_Group_free((MPI_Group*)&dmem_gp),
		       "Network::CutLinks", "net Group_free");
  }
#endif
//TODO  views.Reset();
  layers.CutLinks();
  specs.CutLinks();
  proj = NULL;
  inherited::CutLinks();
}

void Network::Copy_(const Network& cp) {
  copying = true;
  specs = cp.specs;
  layers = cp.layers;

  context = cp.context;
  wt_update = cp.wt_update;
  batch_n = cp.batch_n;
  batch_n_eff = cp.batch_n_eff;

  batch = cp.batch;
  epoch = cp.epoch;
  trial = cp.trial;
  cycle = cp.cycle;
  time = cp.time;
  group_name = cp.group_name;
  trial_name = cp.trial_name;

  sse = cp.sse;
  sum_sse = cp.sum_sse;
  avg_sse = cp.avg_sse;
  cnt_err_tol = cp.cnt_err_tol;
  cnt_err = cp.cnt_err;
  
  cur_sum_sse = cp.cur_sum_sse;
  avg_sse_n = cp.avg_sse_n;
  cur_cnt_err = cp.cur_cnt_err;

  dmem_sync_level = cp.dmem_sync_level;
  dmem_nprocs = cp.dmem_nprocs;
  dmem_nprocs_actual = cp.dmem_nprocs_actual;
  dmem_gp = cp.dmem_gp;

  usr1_save_fmt = cp.usr1_save_fmt;
  wt_save_fmt = cp.wt_save_fmt;
  lay_layout = cp.lay_layout;

  max_size = cp.max_size;

//    ReplaceSpecs_Gp(cp.specs, specs);
  //  CopyNetwork((Network*)&cp);
  
  UpdatePointers_NewPar((taBase*)&cp, this); // should do the work of the two above funs!

  ReConnect_Load();		// set the send cons
#ifdef DMEM_COMPILE
  DMem_DistributeUnits();
#endif
  ((Network&)cp).SyncSendPrjns(); // these get screwed up in there somewhere..
  //note: batch update in tabase copy
  copying = false;
}

void Network::UpdateAfterEdit(){
  inherited::UpdateAfterEdit();
  UpdtAfterNetMod();
}

void Network::SetProjectionDefaultTypes(Projection* prjn) {
  // noop for base case: algorithms must override!
  prjn->spec.type = &TA_FullPrjnSpec; 
}

void Network::RemoveMonitors() {
  if (!proj) return;
  TokenSpace& ts = TA_NetMonitor.tokens;
  for (int i = 0; i < ts.size; ++i) {
    NetMonitor* nm = (NetMonitor*)ts.FastEl(i);
    if (nm->GetOwner(&TA_ProjectBase) != proj) continue;
    nm->RemoveMonitors();
  }
}
void Network::UpdateMonitors() {
  if (!proj) return;
  TokenSpace& ts = TA_NetMonitor.tokens;
  for (int i = 0; i < ts.size; ++i) {
    NetMonitor* nm = (NetMonitor*)ts.FastEl(i);
    if (nm->GetOwner(&TA_ProjectBase) != proj) continue;
    nm->UpdateMonitors();
  }
}

// cfront requires this to be outside class function
enum NetSection {NS_NONE, NS_DEFINITIONS, NS_CONSTRAINTS,
		   NS_NETWORK, NS_BIASES};

ConSpec* GetNSConSpec(char name,BaseSpec_Group * bsmg,
		      BaseSpec_Group* master, TypeDef* conspec_type,
		      bool skip_dots=true){
  if((skip_dots == true) && (name == '.')) return NULL;
  int i;
  ConSpec* result;
  for(i=0;i<bsmg->leaves;i++){
    result = (ConSpec *) bsmg->Leaf(i);
    if(!result->name.empty() && (result->name[0] == name)) return result;
  }
  if((name == 'r') || (name == 'p') ||
     (name == 'n') || (name == '.')) { // pre-defined specs
    result  = (ConSpec *) master->New(1,conspec_type);
    bsmg->Link(result);
    result->name = name;
    result->rnd.type = Random::UNIFORM;
    if(name == 'r') {result->rnd.mean = 0.0; result->rnd.var = .5;}
    else if(name == 'p') {result->rnd.mean = 0.5; result->rnd.var = .5;}
    else if(name == 'n') {result->rnd.mean = -0.5; result->rnd.var = .5;}
    else if(name == '.') {result->rnd.mean = 0; result->rnd.var = 0;}
    return result;
  }
  // search for uppercase version
  for(i=0;i<bsmg->leaves;i++){
    result = (ConSpec *) bsmg->Leaf(i);
    String upnm = result->name;
    upnm.upcase();
    if(!upnm.empty() && (upnm[0] == name)){
      master->DuplicateEl(result);
      result = (ConSpec *) master->Leaf(master->leaves-1);
      result->name = name;
    }
    // todo set lrate or something to 0 to make upcase
    // version unmodifiable
  }
  taMisc::Error("Connection Specification letter \"", String(name) , "\" not found");
  return NULL;
}

void Network::UpdtAfterNetMod() {
  SyncSendPrjns();
  CountRecvCons();
#ifdef DMEM_COMPILE
  dmem_nprocs_actual = MIN(dmem_nprocs, taMisc::dmem_nprocs);
  DMem_SyncNRecvCons();
#endif
}

int Network::Dump_Load_Value(istream& strm, TAPtr par) {
  int rval = inherited::Dump_Load_Value(strm, par);
  if(rval)
    ReConnect_Load();

#ifdef DMEM_COMPILE
  DMem_DistributeUnits();
  DMem_PruneNonLocalCons();
#endif

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
  StructUpdate(true);
//obs  net_will_updt = true;
  UpdateMax();
  Layer* l;
  taLeafItr i;
  FOR_ITR_EL(Layer, l, layers., i)
    l->Build();
//obs  net_will_updt = false;
  UpdateMonitors();
  UpdtAfterNetMod();
  StructUpdate(false);
  taMisc::DoneBusy();
#ifdef DMEM_COMPILE
  DMem_DistributeUnits();
#endif
  if(!taMisc::gui_active)    return;
/*TODO   NetView* nv;
  FOR_ITR_EL(NetView, nv, views., i) {
    if(nv->ordered_uvg_list.size == 0) {
      nv->SelectVar("act");
//TEMP      if(nv->editor)
//	nv->editor->SelectActButton(Tool::select);
    }
  }*/

}

void Network::PreConnect() {
  Layer* l;
  taLeafItr i;
  FOR_ITR_EL(Layer, l, layers., i)
    l->PreConnect();
}

void Network::Connect() {
  taMisc::Busy();
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
  UpdateMonitors();
  StructUpdate(false);
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
	taMisc::Error("Network:",GetName(),"Needs the 'Build' command to be run");
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
	taMisc::Error("Network:",GetName(), "Needs the 'Connect' command to be run");
      return false;
    }
  }
  return true;
}

bool Network::CheckTypes(bool quiet) {
  Layer* l;
  taLeafItr i;
  FOR_ITR_EL(Layer, l, layers., i) {
    if(!l->CheckTypes(quiet))
      return false;
  }
  return true;
}

bool Network::CheckConfig(bool quiet) {
  if(!CheckTypes(quiet)) return false;
  if(!CheckBuild(quiet)) return false;
  if(!CheckConnect(quiet)) return false;

  Layer* l;
  taLeafItr i;
  FOR_ITR_EL(Layer, l, layers., i) {
    if(!l->CheckConfig(this, quiet))
      return false;
  }
  return true;
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

void Network::ConnectUnits(Unit* u_to, Unit* u_from, bool record,
			   ConSpec* conspec)
{
  if(u_to == NULL) return; // must have reciever
  if(u_from == NULL)    u_from = u_to; // assume self con if no from

  Layer* lay = GET_OWNER(u_to,Layer);
  Layer* l_from = GET_OWNER(u_from,Layer);
  Projection* pjn = NULL;
  taLeafItr p;
  // check to see if a pjrn already exists
  FOR_ITR_EL(Projection, pjn, lay->projections., p) {
    if((pjn->from == l_from) &&
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

void Network::ReConnect_Load() {
  Layer* l;
  taLeafItr i;
  FOR_ITR_EL(Layer, l, layers., i)
    l->ReConnect_Load();
}

void Network::CopyNetwork(Network* net) {
  Layer* l;
  taLeafItr li;
  Layer* cl;
  taLeafItr cli;
  for(l = (Layer*)layers.FirstEl(li), cl = (Layer*)net->layers.FirstEl(cli);
      l && cl;
      l = (Layer*)layers.NextEl(li), cl = (Layer*)net->layers.NextEl(cli)) {
    l->CopyNetwork(this, net, cl);
  }
  UpdtAfterNetMod();
}
#ifdef TA_GUI
void Network::ShowInViewer(T3DataViewer* vwr) {
  if (vwr) {
//TODO: need to verify it was from this project -- we should be able to specify this by directive...
        // check if already viewing this log there, warn user
    T3DataView* dv = vwr->FindRootViewOfData(this);
    if (dv) {
      if (taMisc::Choice("This network is already shown in that viewer -- are you sure you"
          " want to show it there again?", "&Ok", "&Cancel") != 0) return;
    }

  } else {
    // find the default viewer
//TODO: should use topmost viewer/selected t3 tab
    MainWindowViewer* mv = proj->GetDefaultProjectBrowser();
    if (mv) { // should exist if we can invoke this!
      vwr = (T3DataViewer*)mv->FindFrameByType(&TA_T3DataViewer);
    }
  }
  if (!vwr) return; // prob should give error msg
  
  // add the netview
  NetView* nv = NetView::New(vwr, this);
  nv->BuildAll();
  vwr->Render();
}
#endif
void Network::RemoveUnits() {
  taMisc::Busy();
  StructUpdate(true);
  RemoveMonitors();
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
  RemoveMonitors();
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
  RemoveMonitors();
  Layer* l;
  taLeafItr i;
  FOR_ITR_EL(Layer, l, layers., i)
    l->RemoveCons();
  StructUpdate(false);
  taMisc::DoneBusy();
}

void Network::InitExterns(){
  Layer* l;
  taLeafItr i;
  FOR_ITR_EL(Layer, l, layers., i) {
    if(!l->lesion)
      l->InitExterns();
  }
}

void Network::InitDelta(){
  Layer* l;
  taLeafItr i;
  FOR_ITR_EL(Layer, l, layers., i) {
    if(!l->lesion)
      l->InitDelta();
  }
}

void Network::InitState(){
  Layer* l;
  taLeafItr i;
  FOR_ITR_EL(Layer, l, layers., i) {
    if(!l->lesion)
      l->InitState();
  }
}

#ifdef DMEM_COMPILE

void Network::DMem_SyncNRecvCons() {
  if(n_cons <= 0) return;
  if(dmem_sync_level == DMEM_SYNC_LAYER) {
    Layer* l;
    taLeafItr i;
    FOR_ITR_EL(Layer, l, layers., i) {
      if(!l->lesion)
	l->DMem_SyncNRecvCons();
    }
  }
  else {
    dmem_share_units.Sync(0);
  }
}

void Network::DMem_SyncNet() {
  if(dmem_sync_level != DMEM_SYNC_NETWORK) {
    taMisc::Error("Error: attempt to DMem sync netin at network level, should only be at layer level!");
    return;
  }
  dmem_share_units.Sync(1);
}

void Network::DMem_SyncAct() {
  if(dmem_sync_level != DMEM_SYNC_NETWORK) {
    taMisc::Error("Error: attempt to DMem sync act at network level, should only be at layer level!");
    return;
  }
  dmem_share_units.Sync(2);
}

void Network::DMem_DistributeUnits() {
  //  cerr << "proc " << taMisc::dmem_proc << " in distribunits" << endl;
  dmem_nprocs_actual = MIN(dmem_nprocs, taMisc::dmem_nprocs);

  if(dmem_nprocs_actual > 1) {
    MPI_Group worldgp;
    DMEM_MPICALL(MPI_Comm_group(MPI_COMM_WORLD, &worldgp),
		       "Network::DMem_DistributeUnits", "Comm_group");

    if(dmem_share_units.comm != MPI_COMM_SELF) {
      DMEM_MPICALL(MPI_Comm_free((MPI_Comm*)&dmem_share_units.comm),
			 "Network::DMem_DistributeUnits", "net Comm_free");
      DMEM_MPICALL(MPI_Group_free((MPI_Group*)&dmem_gp),
			 "Network::DMem_DistributeUnits", "net Group_free");
    }

    int myepc = taMisc::dmem_proc / dmem_nprocs_actual;
    // outer-loop is epoch-wise: if there are extra procs they are for that
    int stnet = myepc * dmem_nprocs_actual;
    int net_ranks[dmem_nprocs_actual];
    for(int i = 0; i<dmem_nprocs_actual; i++)
      net_ranks[i] = stnet + i;

    DMEM_MPICALL(MPI_Group_incl(worldgp, dmem_nprocs_actual, net_ranks, (MPI_Group*)&dmem_gp),
		       "Network::DMem_DistributeUnits", "net Group_incl");
    DMEM_MPICALL(MPI_Comm_create(MPI_COMM_WORLD, (MPI_Group)dmem_gp, (MPI_Comm*)&dmem_share_units.comm),
		       "Network::DMem_DistributeUnits", "net Comm_create");
  }
  else {
    dmem_share_units.comm = MPI_COMM_SELF;
    dmem_gp = -1;
  }

  dmem_share_units.Reset();
  bool any_custom_distrib = false;
  Layer* lay;
  taLeafItr li;
  FOR_ITR_EL(Layer, lay, layers., li) {
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

void Network::DMem_PruneNonLocalCons() {
  if(dmem_nprocs_actual <= 1) return;
  taLeafItr li, ui;
  Unit* u;
  Layer *l;
  FOR_ITR_EL(Layer, l, layers., li) {
    FOR_ITR_EL(Unit, u, l->units., ui) {
      if(u->DMem_IsLocal()) {
	continue;
      }
      // only non-local
      Con_Group* recv_gp;
      int g;
      for (g = 0; g < u->recv.gp.size; g++) {
	recv_gp = (Con_Group *)u->recv.FastGp(g);
	if(recv_gp->spec->DMem_AlwaysLocal()) continue;
	for (int sui = recv_gp->size-1; sui >= 0; sui--) {
	  u->DisConnectFrom(recv_gp->Un(sui), NULL);
	}
      }
    }
  }
}

void Network::DMem_SyncWts(MPI_Comm comm, bool sum_dwts) {
  static float_Array values;
  static float_Array results;

  int np = 0; MPI_Comm_size(comm, &np);
  if(np <= 1) return;

//   if(taMisc::dmem_debug)
//     cerr << "proc: " << taMisc::dmem_proc << " at syncwts barrier!" << endl;
//   DMEM_MPICALL(MPI_Barrier(comm),"Network::SyncWts", "Barrier");
//   if(taMisc::dmem_debug && (taMisc::dmem_proc == 0))
//     cerr << "---------- past syncwts barrier ---------" << endl;

  values.EnforceSize(n_cons + n_units);

  int cidx = 0;
  Layer* lay;
  taLeafItr li;
  FOR_ITR_EL(Layer, lay, layers., li) {
    if(lay->projections.size == 0) continue;
    int dwt_off = 0;
    if(sum_dwts) {
      Projection* prjn = (Projection*)lay->projections.FastEl(0);
      MemberDef* md = prjn->con_spec->DMem_EpochShareDwtVar();
      if(md == NULL) continue;
      dwt_off = (int)md->GetOff((void*)0x100) - 0x100;
    }
    Unit* un;
    taLeafItr ui;
    FOR_ITR_EL(Unit, un, lay->units., ui) {
      if(un->bias) {
	if(sum_dwts) {
	  values.FastEl(cidx++) = *((float*)((char*)(un->bias) + dwt_off));
	}
	else {
	  values.FastEl(cidx++) = un->bias->wt;
	}
      }

      Con_Group* cg;
      int gi;
      FOR_ITR_GP(Con_Group, cg, un->recv., gi) {
	if(sum_dwts) {
	  for(int i = 0;i<cg->size;i++) values.FastEl(cidx++) = *((float*)((char*)(cg->Cn(i)) + dwt_off));
	}
	else {
	  for(int i = 0;i<cg->size;i++) values.FastEl(cidx++) = cg->Cn(i)->wt;
	}
      }
    }
  }

  results.EnforceSize(cidx);
  DMEM_MPICALL(MPI_Allreduce(values.el, results.el, cidx, MPI_FLOAT, MPI_SUM, comm),
		     "Network::SyncWts", "Allreduce");

  float avg_mult = 1.0f / (float)np;
  cidx = 0;
  FOR_ITR_EL(Layer, lay, layers., li) {
    if(lay->projections.size == 0) continue;
    int dwt_off = 0;
    if(sum_dwts) {
      Projection* prjn = (Projection*)lay->projections.FastEl(0);
      MemberDef* md = prjn->con_spec->DMem_EpochShareDwtVar();
      if(md == NULL) continue;
      dwt_off = (int)md->GetOff((void*)0x100) - 0x100;
    }
    Unit* un;
    taLeafItr ui;
    FOR_ITR_EL(Unit, un, lay->units., ui) {
      if(un->bias) {
	if(sum_dwts) {
	  *((float*)((char*)(un->bias) + dwt_off)) = results.FastEl(cidx++);
	}
	else {
	  un->bias->wt = avg_mult * results.FastEl(cidx++);
	}
      }
      Con_Group* cg;
      int gi;
      FOR_ITR_GP(Con_Group, cg, un->recv., gi) {
	if(sum_dwts) {
	  for(int i = 0;i<cg->size;i++) *((float*)((char*)(cg->Cn(i)) + dwt_off)) = results.FastEl(cidx++);
	}
	else {
	  for(int i = 0;i<cg->size;i++) cg->Cn(i)->wt = avg_mult * results.FastEl(cidx++);
	}
      }
    }
  }
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
    if(lay->projections.size == 0) continue;
    Unit* un;
    taLeafItr ui;
    FOR_ITR_EL(Unit, un, lay->units., ui) {
      int gi;
      for(gi=0;gi<un->recv.gp.size;gi++) {
	Con_Group* cg = (Con_Group*)un->recv.gp[gi];
	if(!cg->spec->wt_limits.sym) continue;

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
	      unit_idxs.EnforceSize(msgsize);
	      wt_vals.EnforceSize(msgsize);
	      DMEM_MPICALL(MPI_Recv(unit_idxs.el, msgsize, MPI_INT, proc, 102, comm, &status),
			   "DMem_SymmetrizeWts", "MPI_Recv unit_idxs");
	      DMEM_MPICALL(MPI_Recv(wt_vals.el, msgsize, MPI_FLOAT, proc, 103, comm, &status),
			   "DMem_SymmetrizeWts", "MPI_Recv wt_vals");

	      all_unit_idxs.CopyVals(unit_idxs, 0, -1, all_unit_idxs.size);
	      all_wt_vals.CopyVals(wt_vals, 0, -1, all_wt_vals.size);
	    }
	  }
	  // now have all the data collected, to through and get the sym values!
	  int i;
	  for(i=0;i<cg->size;i++) {
	    Unit* fm = cg->Un(i);
	    int uidx = cg->prjn->from->units.FindLeaf(fm);
	    if(uidx < 0) continue;
	    int sidx = all_unit_idxs.Find(uidx);
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
	    int fmgi;
	    Con_Group* fmg;
	    FOR_ITR_GP(Con_Group, fmg, fm->recv., fmgi) {
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

#endif

void Network::ModifyState(){
  Layer* l;
  taLeafItr i;
  FOR_ITR_EL(Layer, l, layers., i) {
    if(!l->lesion)
      l->ModifyState();
  }

}

void Network::InitWtDelta(){
  Layer* l;
  taLeafItr i;
  FOR_ITR_EL(Layer, l, layers., i) {
    if(!l->lesion)
      l->InitWtDelta();
  }
}

void Network::InitWtState() {
  // do lots of checking here to make sure, cuz often 1st thing that happens
  if(!CheckConfig()) return;

  taMisc::Busy();
  Layer* l;
  taLeafItr i;
  FOR_ITR_EL(Layer, l, layers., i) {
    if(!l->lesion)
      l->InitWtState();
  }
  InitWtState_post();		// done after all initialization (for scaling wts...)

#ifdef DMEM_COMPILE
  // do the dmem weight symmetrizing!
  DMem_SymmetrizeWts();
#endif

  InitState();			// also re-init state at this point..

  InitCounters();
  InitStats();

  sse = 0;
  UpdateAllViews();
  taMisc::DoneBusy();
}

void Network::InitCounters() {
  batch = 0;
  epoch = 0;
  trial = 0;
  cycle = 0;
  time = 0.0f;
}

void Network::InitStats() {
  sse = 0.0f;
  sum_sse = 0.0f;
  avg_sse = 0.0f;
  cnt_err = 0.0f;

  cur_sum_sse = 0.0f;
  avg_sse_n = 0;
  cur_cnt_err = 0.0f;
}

void Network::InitWtState_post() {
  Layer* l;
  taLeafItr i;
  FOR_ITR_EL(Layer, l, layers., i) {
    if(!l->lesion)
      l->InitWtState_post();
  }
}

void Network::Compute_Net() {
  Layer* l;
  taLeafItr i;
  FOR_ITR_EL(Layer, l, layers., i) {
    if(!l->lesion)
      l->Compute_Net();
  }
#ifdef DMEM_COMPILE
  DMem_SyncNet();
#endif
}

void Network::Send_Net() {
  Layer* l;
  taLeafItr i;
  FOR_ITR_EL(Layer, l, layers., i) {
    if(!l->lesion)
      l->Send_Net();
  }
#ifdef DMEM_COMPILE
  DMem_SyncNet();
#endif
}

void Network::Compute_Act_default() {
  Layer* l;
  taLeafItr i;
  FOR_ITR_EL(Layer, l, layers., i) {
    if(!l->lesion)
      l->Compute_Act();
  }
}

void Network::UpdateWeights() {
  Layer* l;
  taLeafItr i;
  FOR_ITR_EL(Layer, l, layers., i) {
    if(!l->lesion)
      l->UpdateWeights();
  }
}

void Network::Compute_dWt() {
  Layer* l;
  taLeafItr i;
  FOR_ITR_EL(Layer, l, layers., i) {
    if(!l->lesion)
      l->Compute_dWt();
  }
}

void Network::Compute_SSE() {
  sse = 0.0f;
  Layer* l;
  taLeafItr i;
  FOR_ITR_EL(Layer, l, layers., i) {
    if(!l->lesion)
      sse += l->Compute_SSE();
  }
  cur_sum_sse += sse;
  avg_sse_n++;
  if(sse > cnt_err_tol)
    cur_cnt_err += 1.0;
}

void Network::Compute_EpochSSE() {
  sum_sse = cur_sum_sse;
  if(avg_sse_n > 0)
    avg_sse = cur_sum_sse / (float)avg_sse_n;
  cnt_err = cur_cnt_err;

  cur_sum_sse = 0.0f;
  avg_sse_n = 0;
  cur_cnt_err = 0.0f;
}

void Network::Copy_Weights(const Network* src) {
  taMisc::Busy();
  Layer* l, *sl;
  taLeafItr i,si;
  for(l = (Layer*)layers.FirstEl(i), sl = (Layer*)src->layers.FirstEl(si);
      (l) && (sl);
      l = (Layer*)layers.NextEl(i), sl = (Layer*)src->layers.NextEl(si))
  {
    if(!l->lesion && !sl->lesion)
      l->Copy_Weights(sl);
  }
  UpdateAllViews();
  taMisc::DoneBusy();
}

void Network::WriteWeights(ostream& strm, Network::WtSaveFormat fmt) {
  taMisc::Busy();
  strm << "#Fmt " << fmt << "\n"
       << "#Name " << GetName() << "\n"
       << "#Epoch " << epoch << "\n"
       << "#ReInit " << 0 << "\n"; // todo: remove this!
  Layer* l;
  taLeafItr i;
  FOR_ITR_EL(Layer, l, layers., i) {
    if(!l->lesion)
      l->WriteWeights(strm, (Con_Group::WtSaveFormat)fmt);
  }
  taMisc::DoneBusy();
}

void Network::ReadWeights(istream& strm) {
  taMisc::Busy();
  int c = strm.peek();
  if(c != '#') {
    taMisc::Error("ReadWeights: file format is incorrect, expected #");
    return;
  }
  strm.get();
  c = strm.peek();
  if(!((c == 'F') || (c == 'N'))) {
    taMisc::Error("ReadWeights: file format is incorrect, expected Fmt or Name");
    return;
  }
  if(c == 'F') {
    c = taMisc::read_alnum_noeol(strm);
    c = taMisc::read_till_eol(strm);
    wt_save_fmt = (WtSaveFormat)(int)taMisc::LexBuf;
  }
  else {
    wt_save_fmt = TEXT;		// default is text for old files that don't have Fmt saved
  }
  c = taMisc::read_alnum_noeol(strm);
  c = taMisc::read_till_eol(strm);
  SetName(taMisc::LexBuf);
  c = taMisc::read_alnum_noeol(strm); // get #Epoch
  c = taMisc::read_till_eol(strm); // get epoch
  epoch = (int)taMisc::LexBuf;
  c = taMisc::read_alnum_noeol(strm); // get #ReInit
  c = taMisc::read_till_eol(strm); // get re_init
  int re_init = (int)taMisc::LexBuf;

  Layer* l;
  taLeafItr i;
  FOR_ITR_EL(Layer, l, layers., i) {
    if(!l->lesion)
      l->ReadWeights(strm, (Con_Group::WtSaveFormat)wt_save_fmt);
    if(strm.eof()) break;
  }
  UpdateAllViews();
  taMisc::DoneBusy();
}

void Network::StretchLayerPos(int add_to_z) {
  Layer* l;
  taLeafItr i;
  FOR_ITR_EL(Layer, l, layers., i) {
    l->pos.z += l->pos.z * add_to_z;
  }
}

void Network::TransformWeights(const SimpleMathSpec& trans) {
  taMisc::Busy();
  Layer* l;
  taLeafItr i;
  FOR_ITR_EL(Layer, l, layers., i) {
    if(!l->lesion)
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
    if(!l->lesion)
      l->AddNoiseToWeights(noise_spec);
  }
  UpdateAllViews();
  taMisc::DoneBusy();
}

int Network::PruneCons(const SimpleMathSpec& pre_proc,
			  CountParam::Relation rel, float cmp_val)
{
  taMisc::Busy();
  StructUpdate(true);
  int rval = 0;
  Layer* l;
  taLeafItr i;
  FOR_ITR_EL(Layer, l, layers., i) {
    if(!l->lesion)
      rval += l->PruneCons(pre_proc, rel, cmp_val);
  }
  StructUpdate(false);
  taMisc::DoneBusy();
  return rval;
}

int Network::ProbAddCons(float p_add_con, float init_wt) {
  taMisc::Busy();
  StructUpdate(true);
  int rval = 0;
  Layer* l;
  taLeafItr i;
  FOR_ITR_EL(Layer, l, layers., i) {
    if(!l->lesion)
      rval += l->ProbAddCons(p_add_con, init_wt);
  }
  StructUpdate(false);
  taMisc::DoneBusy();
  return rval;
}

int Network::LesionCons(float p_lesion, bool permute) {
  taMisc::Busy();
  StructUpdate(true);
  int rval = 0;
  Layer* l;
  taLeafItr i;
  FOR_ITR_EL(Layer, l, layers., i) {
    if(!l->lesion)
      rval += l->LesionCons(p_lesion, permute);
  }
  StructUpdate(false);
  taMisc::DoneBusy();
  return rval;
}

int Network::LesionUnits(float p_lesion, bool permute) {
  taMisc::Busy();
  StructUpdate(true);
  int rval = 0;
  Layer* l;
  taLeafItr i;
  FOR_ITR_EL(Layer, l, layers., i) {
    if(!l->lesion)
      rval += l->LesionUnits(p_lesion, permute);
  }
  StructUpdate(false);
  taMisc::DoneBusy();
  return rval;
}

void Network::UpdateMax() {
  max_size.x = 1;  max_size.y = 1;  max_size.z = 0;

  Layer* l;
  taLeafItr i;
  FOR_ITR_EL(Layer, l, layers., i) {
    max_size.x = MAX(max_size.x, l->act_geom.x + l->pos.x);
    max_size.y = MAX(max_size.y, l->act_geom.y + l->pos.y);
    max_size.z = MAX(max_size.z, 1 + l->pos.z);
  }
  max_size.y += 1;		// increment max y by one (for extra spacing)
  if(max_size.x == 1) max_size.x = GetDefaultX();
  if(max_size.y == 2) max_size.y = GetDefaultY();
  if(max_size.z == 0) max_size.z = GetDefaultZ();
}

void Network::FixLayerViews(Layer* lay){
/*TODO   NetView* view;
  taLeafItr i;
  FOR_ITR_EL(NetView, view, views., i) {
    view->FixLayer(lay);
  }*/
}

void Network::TwoD_Or_ThreeD(LayerLayout lo){
  lay_layout = lo;
  Layer * lay;
  taLeafItr j;
  FOR_ITR_EL(Layer, lay, layers., j){
    lay->SetDefaultPos();
  }
/*TODO   NetView* nv;
  taLeafItr i;
  FOR_ITR_EL(NetView, nv, views., i) {
    nv->SetDefaultSkew();
    nv->AutoPositionPrjnPoints();
    nv->UpdateAfterEdit();
  }*/
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
    if(!l->lesion)
      nchg += l->ReplaceUnitSpec(old_sp, new_sp);
  }
  return nchg;
}

int Network::ReplaceConSpec(ConSpec* old_sp, ConSpec* new_sp) {
  int nchg = 0;
  Layer* l;
  taLeafItr i;
  FOR_ITR_EL(Layer, l, layers., i) {
    if(!l->lesion)
      nchg += l->ReplaceConSpec(old_sp, new_sp);
  }
  return nchg;
}

int Network::ReplacePrjnSpec(ProjectionSpec* old_sp, ProjectionSpec* new_sp) {
  int nchg = 0;
  Layer* l;
  taLeafItr i;
  FOR_ITR_EL(Layer, l, layers., i) {
    if(!l->lesion)
      nchg += l->ReplacePrjnSpec(old_sp, new_sp);
  }
  return nchg;
}

int Network::ReplaceLayerSpec(LayerSpec* old_sp, LayerSpec* new_sp) {
  int nchg = 0;
  Layer* l;
  taLeafItr i;
  FOR_ITR_EL(Layer, l, layers., i) {
    if(!l->lesion)
      nchg += l->ReplaceLayerSpec(old_sp, new_sp);
  }
  return nchg;
}

/*obs void Network::GridViewWeights(GridLog* grid_log, Layer* recv_lay, Layer* send_lay,
			      bool use_swt, int un_x, int un_y, int wt_x, int wt_y)
{
  if(recv_lay == NULL) return;
  recv_lay->GridViewWeights(grid_log, send_lay, use_swt, un_x, un_y, wt_x, wt_y);
}*/

void Network::WeightsToTable(DataTable* dt, Layer* recv_lay, Layer* send_lay)
{
  if(recv_lay == NULL) return;
  recv_lay->WeightsToTable(dt, send_lay);
}


// new monitor is in pdpshell.cc

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
  if(rval == NULL) {
    taMisc::Error("Error: could not find spec named:", nm);
  }
  return rval;
}

BaseSpec* Network::FindSpecType(TypeDef* td) {
  BaseSpec* rval = (BaseSpec*)specs.FindSpecType(td);
  if(rval == NULL) {
    taMisc::Error("Error: could not find spec of type:", td->name);
  }
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
    layers.Remove(lay);
    lay = (Layer*)layers.NewEl(1, td);
    lay->name = nm;
    nw_itm = true;
  }
  return lay;
}

Projection* Network::FindMakePrjn(Layer* recv, Layer* send, ProjectionSpec* ps, ConSpec* cs, bool& nw_itm) {
  Projection* use_prj = NULL;
  int i;
  for(i=0;i<recv->projections.size;i++) {
    Projection* prj = (Projection*)recv->projections[i];
    if(prj->from == send) {
      if((ps == NULL) && (cs == NULL)) {
	nw_itm = false;
	return prj;
      }
      if((ps) && (prj->spec.spec != ps)) {
	use_prj = prj;
	break;
      }
      if((cs) && (prj->con_spec.spec != cs)) {
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
  use_prj->SetCustomFrom(send);
  if(ps) {
    use_prj->spec.SetSpec(ps);
  }
  if(cs) {
    use_prj->SetConType(cs->min_con_type);
    use_prj->con_spec.SetSpec(cs);
  }
  return use_prj;
}

Projection* Network::FindMakePrjnAdd(Layer* recv, Layer* send, ProjectionSpec* ps, ConSpec* cs, bool& nw_itm) {
  int i;
  for(i=0;i<recv->projections.size;i++) {
    Projection* prj = (Projection*)recv->projections[i];
    if((prj->from == send)
       && ((ps == NULL) || (prj->spec.spec == ps) ||
	   (prj->spec.spec->InheritsFrom(TA_FullPrjnSpec) &&
	    ps->InheritsFrom(TA_FullPrjnSpec)))
       && ((cs == NULL) || (prj->con_spec.spec == cs))) {
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
    prj->SetConType(cs->min_con_type);
    prj->con_spec.SetSpec(cs);
  }
  return prj;
}

Projection* Network::FindMakeSelfPrjn(Layer* recv, ProjectionSpec* ps, ConSpec* cs, bool& nw_itm) {
  Projection* use_prj = NULL;
  int i;
  for(i=0;i<recv->projections.size;i++) {
    Projection* prj = (Projection*)recv->projections[i];
    if(prj->from == recv) {
      if((ps == NULL) && (cs == NULL)) {
	nw_itm = false;
	return prj;
      }
      if((ps) && (prj->spec.spec != ps)) {
	use_prj = prj;
	break;
      }
      if((cs) && (prj->con_spec.spec != cs)) {
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
  taBase::SetPointer((TAPtr*)&(use_prj->from), recv);
  if(ps)
    use_prj->spec.SetSpec(ps);
  if(cs)
    use_prj->con_spec.SetSpec(cs);
  return use_prj;
}

Projection* Network::FindMakeSelfPrjnAdd(Layer* recv, ProjectionSpec* ps, ConSpec* cs, bool& nw_itm) {
  int i;
  for(i=0;i<recv->projections.size;i++) {
    Projection* prj = (Projection*)recv->projections[i];
    if((prj->from == recv)
       && ((ps == NULL) || (prj->spec.spec == ps))
       && ((cs == NULL) || (prj->con_spec.spec == cs))) {
      nw_itm = false;
      return prj;
    }
  }
  nw_itm = true;
  Projection* prj = (Projection*)recv->projections.NewEl(1);
  prj->from_type = Projection::SELF;
  taBase::SetPointer((TAPtr*)&(prj->from), recv);
  if(ps)
    prj->spec.SetSpec(ps);
  if(cs)
    prj->con_spec.SetSpec(cs);
  return prj;
}

bool Network::RemovePrjn(Layer* recv, Layer* send, ProjectionSpec* ps, ConSpec* cs) {
  int i;
  for(i=recv->projections.size-1;i>=0;i--) {
    Projection* prj = (Projection*)recv->projections[i];
    if((prj->from == send)
       && ((ps == NULL) || (prj->spec.spec == ps) ||
	   (prj->spec.spec->InheritsFrom(TA_FullPrjnSpec) &&
	    ps->InheritsFrom(TA_FullPrjnSpec)))
       && ((cs == NULL) || (prj->con_spec.spec == cs))) {
      recv->projections.Remove(prj);
      return true;
    }
  }
  return false;
}

#ifdef TA_GUI

//NOTE: we only look for T3DataLink, not the concrete type...
/*obs
taiDataLink* Projection::ConstrDataLink(DataViewer* viewer_, const TypeDef* link_type) {
  if (link_type->InheritsFrom(&TA_T3DataLink))
    return PrjnDataLink::New(this, viewer_);
  else return inherited::ConstrDataLink(viewer_, link_type);
}

taiDataLink* Unit_Group::ConstrDataLink(DataViewer* viewer_, const TypeDef* link_type) {
  if (link_type->InheritsFrom(&TA_T3DataLink))
    return UnitGroupDataLink::New(this, viewer_);
  else return taGroup_impl::ConstrDataLink(viewer_, link_type);
}

taiDataLink* Layer::ConstrDataLink(DataViewer* viewer_, const TypeDef* link_type) {
  if (link_type->InheritsFrom(&TA_T3DataLink))
    return LayerDataLink::New(this, viewer_);
  else return inherited::ConstrDataLink(viewer_, link_type);
}

taiDataLink* Network::ConstrDataLink(DataViewer* viewer_, const TypeDef* link_type) {
  if (link_type->InheritsFrom(&TA_T3DataLink))
    return NetDataLink::New(this, viewer_);
  else return inherited::ConstrDataLink(viewer_, link_type);
}
*/
#endif



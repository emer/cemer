// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of The Emergent Toolkit
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


// ta_defaults.cc

#include "ta_defaults.h"
#include "ta_TA_type.h"

void TypeDefault::Initialize() {
  old_type = NULL;
  default_type = &TA_taBase;
  token = NULL;
  active_membs.SetBaseType(&TA_NameValue);
  int i;
  for(i=0; i< MAX_DFT_LONGS; i++)
    active[i] = 0;
}

void TypeDefault::Destroy() {
  if(token != NULL)
    taBase::unRefDone(token);
  if(default_type != NULL) {
    taBase_List* gp = default_type->defaults;
    if(gp != NULL) {
      taBase::Ref(this);
      taBase::Ref(this);	// make sure that the remove doesn't cause a delete..
      gp->RemoveEl(this);
    }
  }
  token = NULL;
}

void TypeDefault::Copy_(const TypeDefault& cp) {
  old_type = cp.old_type;
  default_type = cp.default_type;
  memcpy(active, cp.active, sizeof(active));
  active_membs = cp.active_membs;
}

void TypeDefault::InitLinks() {
  inherited::InitLinks();
  taBase::Own(active_membs, this);
}

int TypeDefault::Dump_Load_Value(istream& strm, TAPtr par) {
  active_membs.Reset();		// remove all members before loading..
  int rval = taNBase::Dump_Load_Value(strm, par);
  if(rval == 1)
    UpdateAfterEdit();
  return rval;
}

void TypeDefault::SetActive(char* memb_nm, bool onoff) {
  MemberDef* md = FindMember(memb_nm);
  if(md != NULL)
    SetActive(md->idx, onoff);
}

bool TypeDefault::GetActive(char* memb_nm) {
  MemberDef* md = FindMember(memb_nm);
  if(md != NULL)
    return GetActive(md->idx);
  return false;
}

void TypeDefault::SetActive(int memb_no, bool onoff) {
  int lng_no = memb_no / (sizeof(long) * 8);
  int bit_no = memb_no % (sizeof(long) * 8);
  if(lng_no >= MAX_DFT_LONGS) {
    taMisc::Error("Number of members", String(memb_no), "exceeds member limit in",
		    GetTypeDef()->name, "of", String(256));
    return;
  }
  if(onoff)
    active[lng_no] |= 1 << bit_no;
  else
    active[lng_no] &= ~(1 << bit_no);
}

bool TypeDefault::GetActive(int memb_no) {
  int lng_no = memb_no / (sizeof(long) * 8);
  int bit_no = memb_no % (sizeof(long) * 8);
  if(lng_no >= MAX_DFT_LONGS) {
    taMisc::Error("Number of members", String(memb_no), "exceeds member limit in",
		    GetTypeDef()->name, "of", String(256));
    return false;
  }
  return active[lng_no] & (1 << bit_no) ? 1 : 0;
}

void TypeDefault::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(default_type == NULL)
    return;

  if((token == NULL) || (token->GetTypeDef() != default_type)) {

    if(old_type != NULL) {	// remove from previous typedef
      taBase_List* gp = old_type->defaults;
      if(gp != NULL) {
	taBase::Ref(this);	// make sure that the remove doesn't cause a delete..
	gp->RemoveEl(this);
      }
    }

    if(token != NULL) {
      taBase::unRefDone(token);
    }
    int i;
    for(i=0; i< MAX_DFT_LONGS; i++)
      active[i] = 0;
    token = taBase::MakeToken(default_type);

    if(token != NULL) {		// make may have failed so here we check
//      taBase::Own(token, this); // maybe shouldn't own this here..
      taBase::Ref(token);
      token->SetName("Default_" + default_type->name);
      SetName(default_type->name);
      default_type->tokens.RemoveEl(token); // don't count on list of tokens..
    }
    else {
      taMisc::Error("A default of", default_type->name," cannot be created",
		      "since that type does not generate tokens");
      default_type = &TA_taBase;
    }

    if(default_type->defaults == NULL) {
      default_type->defaults = new taBase_List;
      taBase::Ref(default_type->defaults);
      default_type->defaults->SetBaseType(&TA_TypeDefault);
    }
    taBase_List* gp = default_type->defaults;
    if(gp->FindEl(this) < 0) {	// add to the list
      gp->Link(this);
      taBase::unRef(this);	// don't inc the refcount, else it won't get killed..
      old_type = default_type;	// this is now the old type
    }
  }

  UpdateFromNameValue();
}

void TypeDefault::UpdateFromNameValue() {
  if((default_type == NULL) || (token == NULL))
    return;

  int i;
  for(i=0; i<active_membs.size; i++) {
    NameValue* nv = (NameValue*)active_membs.FastEl(i);
    MemberDef* md = default_type->members.FindName(nv->name);
    if(md != NULL) {
      SetActive(md->idx, true);
      md->type->SetValStr(nv->value, md->GetOff(token), token, md);
    }
  }
  token->UpdateAfterEdit();
}

void TypeDefault::UpdateToNameValue() {
  if(token == NULL)
    return;

  active_membs.RemoveAll();
  int i;
  for(i=0; i<default_type->members.size; i++) {
    if(GetActive(i)) {
      MemberDef* tmd = default_type->members.FastEl(i);
      NameValue* nv = (NameValue*)active_membs.New(1);
      nv->name = tmd->name;
      nv->value = tmd->type->GetValStr(tmd->GetOff(token), token, tmd);
    }
  }
}

void TypeDefault::SetTypeDefaults(TAPtr tok) {
  if((default_type == NULL) || (tok == NULL) || (token == NULL))
    return;

  if(!tok->InheritsFrom(default_type))
    return;

  int i;
  for(i=0; i<default_type->members.size; i++) {
    if(GetActive(i))
      tok->MemberCopyFrom(i, token);
  }
}


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



// spec.cc

#include "spec.h"
#include "pdp_project.h"
#ifdef TA_GUI
  #include "ta_qtgroup.h"
#endif

#ifdef V3_COMPAT
#include "v3_compat.h"
#endif

//////////////////////////////////
//	BaseSpec_Group		//
//////////////////////////////////

bool BaseSpec_Group::nw_itm_def_arg = false;


void BaseSpec_Group::Initialize() {
  SetBaseType(&TA_BaseSpec);
}

String BaseSpec_Group::GetTypeDecoKey() const {
  if(size > 0) return FastEl(0)->GetTypeDecoKey();
  if(GetOwner()) return GetOwner()->GetTypeDecoKey();
  return inherited::GetTypeDecoKey();
}

BaseSpec* BaseSpec_Group::FindSpecType(TypeDef* td) {
  // breadth-first search
  BaseSpec* bs;
  taLeafItr i;
  FOR_ITR_EL(BaseSpec, bs, this->, i) {
    if(!bs->isDestroying() && (bs->GetTypeDef() == td))
      return bs;    // use equals to find type of spec object
  }
  // then check the children
  FOR_ITR_EL(BaseSpec, bs, this->, i) {
    // this is not true -- bad!!
//     if(!(bs->InheritsFrom(td) || td->InheritsFrom(bs->GetTypeDef())))
//       continue;			// no hope..
    BaseSpec* rval = bs->children.FindSpecType(td);
    if(rval)
      return rval;
  }
  return NULL;
}

BaseSpec* BaseSpec_Group::FindSpecInherits(TypeDef* td, TAPtr for_obj) {
  // breadth-first search
  BaseSpec* bs;
  taLeafItr i;
  FOR_ITR_EL(BaseSpec, bs, this->, i) {
    if(bs->InheritsFrom(td)) {
      if((for_obj == NULL) || (bs->CheckObjectType_impl(for_obj)))
	return bs;    // object must also be sufficient..
    }
  }
  // then check the children
  FOR_ITR_EL(BaseSpec, bs, this->, i) {
    if(!(bs->InheritsFrom(td) || td->InheritsFrom(bs->GetTypeDef())))
      continue;			// no hope..
    BaseSpec* rval = bs->children.FindSpecInherits(td, for_obj);
    if(rval)
      return rval;
  }
  return NULL;
}

BaseSpec* BaseSpec_Group::FindSpecTypeNotMe(TypeDef* td, BaseSpec* not_me) {
  // breadth-first search
  BaseSpec* bs;
  taLeafItr i;
  FOR_ITR_EL(BaseSpec, bs, this->, i) {
    if((bs->GetTypeDef() == td) && (bs != not_me))
      return bs;    // use equals to find type of spec object
  }
  // then check the children
  FOR_ITR_EL(BaseSpec, bs, this->, i) {
    if(!(bs->InheritsFrom(td) || td->InheritsFrom(bs->GetTypeDef()) || (bs != not_me)))
      continue;			// no hope..
    BaseSpec* rval = bs->children.FindSpecTypeNotMe(td, not_me);
    if(rval)
      return rval;
  }
  return NULL;
}

BaseSpec* BaseSpec_Group::FindSpecInheritsNotMe(TypeDef* td, BaseSpec* not_me, TAPtr for_obj) {
  // breadth-first search
  BaseSpec* bs;
  taLeafItr i;
  FOR_ITR_EL(BaseSpec, bs, this->, i) {
    if(bs->InheritsFrom(td) && (bs != not_me)) {
      if((for_obj == NULL) || (bs->CheckObjectType_impl(for_obj)))
	return bs;    // object must also be sufficient..
    }
  }
  // then check the children
  FOR_ITR_EL(BaseSpec, bs, this->, i) {
    if(!(bs->InheritsFrom(td) || td->InheritsFrom(bs->GetTypeDef()) || (bs != not_me)))
      continue;			// no hope..
    BaseSpec* rval = bs->children.FindSpecInheritsNotMe(td, not_me, for_obj);
    if(rval)
      return rval;
  }
  return NULL;
}


BaseSpec* BaseSpec_Group::FindSpecName(const char* nm) {
  int idx;
  BaseSpec* rval = (BaseSpec*)FindLeafName((char*)nm, idx);
  if(rval)
    return rval;
  BaseSpec* bs;
  taLeafItr i;
  FOR_ITR_EL(BaseSpec, bs, this->, i) {
    rval = bs->children.FindSpecName(nm);
    if(rval)
      return rval;
  }
  return NULL;
}

BaseSpec* BaseSpec_Group::FindParent() {
  return GET_MY_OWNER(BaseSpec);
}

BaseSpec* BaseSpec_Group::FindMakeSpec(const char* nm, TypeDef* tp, bool& nw_itm, const char* alt_nm) {
  nw_itm = false;
  BaseSpec* sp = NULL;
  if(nm) {
    sp = (BaseSpec*)FindName(nm);
    if((sp == NULL) && (alt_nm)) {
      sp = (BaseSpec*)FindName(alt_nm);
      if(sp) sp->name = nm;
    }
  }
  else {
    sp = (BaseSpec*)FindType(tp);
  }
  if(sp == NULL) {
    sp = (BaseSpec*)NewEl(1, tp);
    if(nm)
      sp->name = nm;
    nw_itm = true;
  }
  else if(!sp->InheritsFrom(tp)) {
    RemoveEl(sp);
    sp = (BaseSpec*)NewEl(1, tp);
    if(nm)
      sp->name = nm;
    nw_itm = true;
  }
  return sp;
}

bool BaseSpec_Group::RemoveSpec(const char* nm, TypeDef* tp) {
  if(nm)
    return RemoveName(nm);

  int idx;
  FindType(tp, idx);
  if(idx >= 0)
    return RemoveIdx(idx);
  return false;
}

//////////////////////////////////
//	BaseSpec		//
//////////////////////////////////

bool BaseSpec::nw_itm_def_arg = false;

void BaseSpec::Initialize() {
  min_obj_type = &TA_taBase;
}

void BaseSpec::Copy_(const BaseSpec& cp) {
  //  min_obj_type = cp.min_obj_type;  // don't do this -- could be going between types
  unique = cp.unique;
  children = cp.children;
}

void BaseSpec::Destroy() {
  CutLinks();
}

void BaseSpec::InitLinks() {
  taNBase::InitLinks();
  taBase::Own(unique, this);
  taBase::Own(children, this);
  children.SetBaseType(GetTypeDef());
  if(!taMisc::is_loading)
    UpdateSpec();
}

void BaseSpec::CutLinks() {
  children.CutLinks();
  taNBase::CutLinks();
}

void BaseSpec::UpdateAfterEdit_impl() {
  taNBase::UpdateAfterEdit_impl();
  Network* net = GET_MY_OWNER(Network);
  if(isDestroying() || !net || net->isDestroying()) return;
  UpdateSpec();
}

BaseSpec* BaseSpec::NewChild() {
  BaseSpec* rval = (BaseSpec*)children.NewEl(1);
  rval->UpdateSpec();
  return rval;
}

BaseSpec* BaseSpec::FindMakeChild(const char* nm, TypeDef* td, bool& nw_itm, const char* alt_nm) {
  if(td == NULL) td = children.el_typ;
  return children.FindMakeSpec(nm, td, nw_itm, alt_nm);
}

BaseSpec* BaseSpec::FindParent() {
  return GET_MY_OWNER(BaseSpec);
}

bool BaseSpec::RemoveChild(const char* nm, TypeDef* td) {
  if(td == NULL) td = children.el_typ;
  return children.RemoveSpec(nm, td);
}

void BaseSpec::SetUnique(const char* memb_nm, bool on) {
  MemberDef* md = FindMember(memb_nm);
  if(md)
    SetUnique(md->idx, on);
  else {
    if(GetTypeDef()->members.size > 0)
      taMisc::Error("SetUnique: Member named:", memb_nm, "not found in Spec type:",
		    GetTypeDef()->name);
  }
}

void BaseSpec::SetUnique(int memb_no, bool on) {
  if(memb_no < TA_BaseSpec.members.size)
    return;
  MemberDef* md = GetTypeDef()->members[memb_no];
  if(!md) {
    taMisc::Error("SetUnique: Member number:", String(memb_no), "not found in Spec type:",
		  GetTypeDef()->name);
    return;
  }
  if(on)
    unique.AddUnique(md->name);
  else
    unique.RemoveEl(md->name);
}

bool BaseSpec::GetUnique(const char* memb_nm) {
  MemberDef* md = FindMember(memb_nm);
  if(md)
    return GetUnique(md->idx);
  taMisc::Error("GetUnique: Member named:", memb_nm, "not found in Spec type:",
		GetTypeDef()->name);
  return false;
}

bool BaseSpec::GetUnique(int memb_no) {
  if(memb_no < TA_BaseSpec.members.size)
    return false;
  MemberDef* md = GetTypeDef()->members[memb_no];
  if(!md) {
    taMisc::Error("GetUnique: Member number:", String(memb_no), "not found in Spec type:",
		  GetTypeDef()->name);
    return false;
  }
  if(unique.FindEl(md->name) >= 0) return true;
  return false;
}

void BaseSpec::UpdateSpec() {
  BaseSpec* parent = FindParent();
  if(parent) {
    TypeDef* td = GetTypeDef();
    int i;
    for(i=TA_BaseSpec.members.size; i< td->members.size; i++)
      UpdateMember(parent, i);
  }
  UpdateSubSpecs();
  UpdateChildren();
}

void BaseSpec::UpdateMember(BaseSpec* from, int memb_no) {
  if((from == NULL) || (memb_no < TA_BaseSpec.members.size))
    return;
  TypeDef* td = GetTypeDef();
  TypeDef* frm_td = from->GetTypeDef();
  if(memb_no < frm_td->members.size) {	// parent must have this member
    MemberDef* md = td->members[memb_no];
    if(frm_td->members[memb_no] == md) { 	// must be the same member
      // don't copy read only or hidden members! (usually set automatically
      // and might depend on local variables)
      if(!GetUnique(memb_no) &&
	 !(md->HasOption("READ_ONLY") || md->HasOption("HIDDEN") ||
	   md->HasOption("NO_INHERIT")))
      {
	if(md->type->InheritsFrom(TA_taList_impl)) {
	  ((taList_impl*)md->GetOff((void*)this))->SetSize
	    (((taList_impl*)md->GetOff((void*)from))->size);
	}
	if(md->type->InheritsFrom(TA_taArray_impl)) {
	  ((taArray_impl*)md->GetOff((void*)this))->SetSize
	    (((taArray_impl*)md->GetOff((void*)from))->size);
	}
	MemberCopyFrom(memb_no, from);
	DataChanged(DCR_ITEM_UPDATED); //obs tabMisc::NotifyEdits(this);
      }
    }
  }
}

void BaseSpec::UpdateChildren() {
  //NOTE: this routine may no longer be necessary, since the variable not_used_ok was removed
  BaseSpec* kid;
  taLeafItr i;
  FOR_ITR_EL(BaseSpec, kid, children., i) {
    kid->UpdateAfterEdit();
  }
}

bool BaseSpec::CheckType(TypeDef* td) {
  if(td == NULL) {
    taMisc::Warning("*** For spec:", name, ", NULL type.",
                  "should be at least:", min_obj_type->name);
    return false;
  }
  if(!CheckType_impl(td)) {
    taMisc::Warning("*** For spec:", name, ", incorrect type:", td->name,
                   "should be at least:", min_obj_type->name);
    return false;
  }
  return true;
}

bool BaseSpec::CheckObjectType(TAPtr obj) {
  if(obj == NULL) {
    taMisc::Warning("*** For spec:", name, ", NULL Object.",
		  "should be at least:", min_obj_type->name);
    return false;
  }
  if(!CheckObjectType_impl(obj)) {
    taMisc::Warning("*** For spec:", name, ", incorrect type of obj:", obj->GetPath(),
		   "of type:", obj->GetTypeDef()->name,
		   "should be at least:", min_obj_type->name);
    return false;
  }
  return true;
}

bool BaseSpec::CheckType_impl(TypeDef* td) {
  // other specs are allowed to own any kind of other spec,
  // and layers and projections also contain specs..
  if(td->InheritsFrom(TA_BaseSpec) ||
     (td->InheritsFrom(TA_Projection) && InheritsFrom(TA_ConSpec)) ||
     (td->InheritsFrom(TA_Layer) && InheritsFrom(TA_UnitSpec)))
    return true;

  if(!td->InheritsFrom(min_obj_type))
    return false;

  return true;
}

bool BaseSpec::CheckObjectType_impl(TAPtr obj) {
  // other specs are allowed to own any kind of other spec,
  // and layers and projections also contain specs..
  if(obj->InheritsFrom(TA_BaseSpec) ||
     (obj->InheritsFrom(TA_Projection) && InheritsFrom(TA_ConSpec)) ||
     (obj->InheritsFrom(TA_Layer) && InheritsFrom(TA_UnitSpec)))
    return true;

  if(!obj->InheritsFrom(min_obj_type))
    return false;

  return true;
}

//////////////////////////////////
//	BaseSubSpec		//
//////////////////////////////////

void BaseSubSpec::Initialize() {
}

void BaseSubSpec::Copy_(const BaseSubSpec& cp) {
  unique = cp.unique;
}

void BaseSubSpec::Destroy() {
}

void BaseSubSpec::InitLinks() {
  taNBase::InitLinks();
  taBase::Own(unique, this);
  if(!taMisc::is_loading)
    UpdateSpec();
}

void BaseSubSpec::UpdateAfterEdit_impl() {
  taNBase::UpdateAfterEdit_impl();
  UpdateSpec();
}

BaseSpec* BaseSubSpec::FindParentBaseSpec() {
  return GET_MY_OWNER(BaseSpec);
}

BaseSubSpec* BaseSubSpec::FindParent() {
  BaseSpec* bso = FindParentBaseSpec();
  if(bso == NULL)	return NULL;
  BaseSpec* bsoo = bso->FindParent(); // parent's owner
  if(bsoo == NULL)	return NULL;

  String my_path = GetPath(NULL, bso); // get my path to owner..
  MemberDef* md;
  BaseSubSpec* from = (BaseSubSpec*)bsoo->FindFromPath(my_path, md);
  if((from == NULL) || !from->InheritsFrom(TA_BaseSubSpec))
    return NULL;			// corresponding subspec object not found..
  return from;
}

void BaseSubSpec::SetUnique(const char* memb_nm, bool on) {
  MemberDef* md = FindMember(memb_nm);
  if(md)
    SetUnique(md->idx, on);
  else {
    if(GetTypeDef()->members.size > 0)
      taMisc::Error("SetUnique: Member named:", memb_nm, "not found in Spec type:",
		    GetTypeDef()->name);
  }
}

void BaseSubSpec::SetUnique(int memb_no, bool on) {
  if(memb_no < TA_BaseSubSpec.members.size)
    return;
  MemberDef* md = GetTypeDef()->members[memb_no];
  if(!md) {
    taMisc::Error("SetUnique: Member number:", String(memb_no), "not found in Spec type:",
		  GetTypeDef()->name);
    return;
  }
  if(on)
    unique.AddUnique(md->name);
  else
    unique.RemoveEl(md->name);
}

bool BaseSubSpec::GetUnique(const char* memb_nm) {
  MemberDef* md = FindMember(memb_nm);
  if(md)
    return GetUnique(md->idx);
  taMisc::Error("GetUnique: Member named:", memb_nm, "not found in Spec type:",
		GetTypeDef()->name);
  return false;
}

bool BaseSubSpec::GetUnique(int memb_no) {
  if(memb_no < TA_BaseSubSpec.members.size)
    return false;
  MemberDef* md = GetTypeDef()->members[memb_no];
  if(!md) {
    taMisc::Error("SetUnique: Member number:", String(memb_no), "not found in Spec type:",
		  GetTypeDef()->name);
    return false;
  }
  if(unique.FindEl(md->name) >= 0) return true;
  return false;
}

void BaseSubSpec::UpdateSpec() {
  BaseSubSpec* parent = FindParent();
  if(parent) {
    TypeDef* td = GetTypeDef();
    int i;
    for(i=TA_BaseSubSpec.members.size; i< td->members.size; i++)
      UpdateMember(parent, i);
  }
}

void BaseSubSpec::UpdateMember(BaseSubSpec* from, int memb_no) {
  if((from == NULL) || (memb_no < TA_BaseSubSpec.members.size))
    return;
  TypeDef* td = GetTypeDef();
  TypeDef* frm_td = from->GetTypeDef();
  if(memb_no < frm_td->members.size) {	// parent must have this member
    MemberDef* md = td->members[memb_no];
    if(frm_td->members[memb_no] == md) { 	// must be the same member
      // don't copy read only or hidden members! (usually set automatically
      // and might depend on local variables)
      if(!GetUnique(memb_no) &&
	 !(md->HasOption("READ_ONLY") || md->HasOption("HIDDEN") ||
	   md->HasOption("NO_INHERIT")))
      {
	MemberCopyFrom(memb_no, from);
	DataChanged(DCR_ITEM_UPDATED); //obstabMisc::NotifyEdits(this);
      }
    }
  }
}

//////////////////////////////////
//	SpecPtr_impl		//
//////////////////////////////////

void SpecPtr_impl::Initialize() {
  owner = NULL;
  type = NULL;
  base_type = &TA_BaseSpec;
  prv_spec = NULL;
}

void SpecPtr_impl::Copy_(const SpecPtr_impl& cp) {
  type = cp.type;
  base_type = cp.base_type;
}

int SpecPtr_impl::UpdatePointers_NewObj(taBase* old_ptr, taBase* new_ptr) {
  BaseSpec* sp = GetSpec();
  if(sp != old_ptr) return 0;
  if(GetOwner(old_ptr->GetTypeDef()) == old_ptr) return 0;
  // don't replace on children of the old object
  SetSpec((BaseSpec*)new_ptr);		// call set spec so type is updated!
  return 1;
}

void SpecPtr_impl::UpdateAfterEdit() {
  taBase::UpdateAfterEdit();

  if(!owner || !type || owner->isDestroying())
    return;

  Network* net = GET_OWNER(owner,Network);
  if(!net || net->isDestroying()) return;

  BaseSpec* sp = GetSpec();
  if(sp) {
    if(sp->isDestroying()) {	// shouldn't happen, but just in case
      SetSpec(NULL);
    }
    else if(sp->GetTypeDef() == type) {
      goto updt_spec;
    }
    else {
      SetSpec(NULL);		// get rid of existing spec
    }
  }
  GetSpecOfType();
 updt_spec:
  sp = GetSpec();
  if(prv_spec != sp) {		
    if(prv_spec)		// get rid of previous data client
      prv_spec->RemoveDataClient(this);
    prv_spec = sp;
  }
  if(sp && !sp->isDestroying())			// always make sure we are a data client
    sp->AddDataClient(this);
}

void SpecPtr_impl::SetBaseType(TypeDef* td) {
  type = td;
  base_type = td;		// this doesn't get set by defaults
}

void SpecPtr_impl::SetDefaultSpec(TAPtr ownr, TypeDef* td) {
  if(type == NULL)
    type = td;

  if(base_type == &TA_BaseSpec)
    base_type = td;

  owner = (TAPtr)ownr;
  if(taBase::GetRefn(this) == 0) {
    taBase::Ref(this);		// refer to this object..
  }

  BaseSpec* sp = GetSpec();
  if((sp) && (sp->GetTypeDef() == type))
    return;

  // this is just like GetSpecOfType(), except it uses inherits!
  // thus, this won't create a new object unless absolutely necessary
  BaseSpec_Group* spgp = GetSpecGroup();
  if(spgp == NULL)
    return;
  // pass the ownr to this, so that min_obj_type can be checked
  sp = spgp->FindSpecInherits(type, owner);
  if((sp) && sp->InheritsFrom(type)) {
    SetSpec(sp);
    return;
  }

  sp = (BaseSpec*)spgp->NewEl(1, type);
  if (sp) {
    SetSpec(sp);
  }
}

BaseSpec_Group* SpecPtr_impl::GetSpecGroup() {
#ifdef V3_COMPAT
  ProjectBase* prj = GET_OWNER(owner,ProjectBase);
  if(prj && prj->InheritsFrom(&TA_V3ProjectBase)) { // a v3 project
    return &(((V3ProjectBase*)prj)->specs);
  }
#endif
  Network* net = GET_OWNER(owner,Network);
  if(net == NULL)
    return NULL;
  return &(net->specs);
}

void SpecPtr_impl::GetSpecOfType() {
  BaseSpec_Group* spgp = GetSpecGroup();
  if(spgp == NULL)
    return;
  BaseSpec* sp = spgp->FindSpecType(type);
  if((sp) && (sp->GetTypeDef() == type)) {
    SetSpec(sp);
    return;
  }

  sp = (BaseSpec*)spgp->NewEl(1, type);
  if (sp) {
    SetSpec(sp);
  }
}

void SpecPtr_impl::DataDataChanged(taDataLink*, int dcr, void* op1, void* op2) {
  // no special calls for specs..
//   if (owner) {
//     owner->SmartRef_DataChanged(this, GetSpec(), dcr, op1, op2);
//   }
}
void SpecPtr_impl::DataLinkDestroying(taDataLink* dl) {
  if (owner && !owner->isDestroying()) {
//     owner->SmartRef_DataDestroying(this, GetSpec()); 
    SetSpec(NULL);
    owner->UpdateAfterEdit();
  }
}


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



// spec.cpp

#include "spec.h"

#ifdef V3_COMPAT
# include "v3_compat.h"
#else
# include "netstru.h"
#endif

//////////////////////////////////
//      BaseSpec_Group          //
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
  FOREACH_ELEM_IN_GROUP(BaseSpec, bs, *this) {
    if(!bs->isDestroying() && (bs->GetTypeDef() == td))
      return bs;    // use equals to find type of spec object
  }
  // then check the children
  FOREACH_ELEM_IN_GROUP(BaseSpec, bs, *this) {
    // this is not true -- bad!!
//     if(!(bs->InheritsFrom(td) || td->InheritsFrom(bs->GetTypeDef())))
//       continue;                      // no hope..
    BaseSpec* rval = bs->children.FindSpecType(td);
    if(rval)
      return rval;
  }
  return NULL;
}

BaseSpec* BaseSpec_Group::FindSpecInherits(TypeDef* td, taBase* for_obj) {
  // breadth-first search
  FOREACH_ELEM_IN_GROUP(BaseSpec, bs, *this) {
    if(bs->InheritsFrom(td)) {
      if((for_obj == NULL) || (bs->CheckObjectType_impl(for_obj)))
        return bs;    // object must also be sufficient..
    }
  }
  // then check the children
  FOREACH_ELEM_IN_GROUP(BaseSpec, bs, *this) {
    if(!(bs->InheritsFrom(td) || td->InheritsFrom(bs->GetTypeDef())))
      continue;                 // no hope..
    BaseSpec* rval = bs->children.FindSpecInherits(td, for_obj);
    if(rval)
      return rval;
  }
  return NULL;
}

BaseSpec* BaseSpec_Group::FindSpecTypeNotMe(TypeDef* td, BaseSpec* not_me) {
  // breadth-first search
  FOREACH_ELEM_IN_GROUP(BaseSpec, bs, *this) {
    if((bs->GetTypeDef() == td) && (bs != not_me))
      return bs;    // use equals to find type of spec object
  }
  // then check the children
  FOREACH_ELEM_IN_GROUP(BaseSpec, bs, *this) {
    if(!(bs->InheritsFrom(td) || td->InheritsFrom(bs->GetTypeDef()) || (bs != not_me)))
      continue;                 // no hope..
    BaseSpec* rval = bs->children.FindSpecTypeNotMe(td, not_me);
    if(rval)
      return rval;
  }
  return NULL;
}

BaseSpec* BaseSpec_Group::FindSpecInheritsNotMe(TypeDef* td, BaseSpec* not_me, taBase* for_obj) {
  // breadth-first search
  FOREACH_ELEM_IN_GROUP(BaseSpec, bs, *this) {
    if(bs->InheritsFrom(td) && (bs != not_me)) {
      if((for_obj == NULL) || (bs->CheckObjectType_impl(for_obj)))
        return bs;    // object must also be sufficient..
    }
  }
  // then check the children
  FOREACH_ELEM_IN_GROUP(BaseSpec, bs, *this) {
    if(!(bs->InheritsFrom(td) || td->InheritsFrom(bs->GetTypeDef()) || (bs != not_me)))
      continue;                 // no hope..
    BaseSpec* rval = bs->children.FindSpecInheritsNotMe(td, not_me, for_obj);
    if(rval)
      return rval;
  }
  return NULL;
}


BaseSpec* BaseSpec_Group::FindSpecName(const char* nm) {
  BaseSpec* rval = (BaseSpec*)FindLeafName((char*)nm);
  if(rval)
    return rval;
  FOREACH_ELEM_IN_GROUP(BaseSpec, bs, *this) {
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

  int idx = FindTypeIdx(tp);
  if(idx >= 0)
    return RemoveIdx(idx);
  return false;
}

void BaseSpec_Group::Defaults() {
  FOREACH_ELEM_IN_GROUP(BaseSpec, bs, *this) {
    bs->Defaults();
    bs->children.Defaults();
  }
}

//////////////////////////////////
//      SpecMemberBase          //
//////////////////////////////////

void SpecMemberBase::Defaults() {
  Defaults_impl();
  DefaultsMembers();
  UpdateAfterEdit();
}

void SpecMemberBase::DefaultsMembers() {
  TypeDef* td = GetTypeDef();
  for(int i=TA_SpecMemberBase.members.size; i< td->members.size; i++) {
    MemberDef* md = td->members[i];
    if(md->type->InheritsFrom(&TA_BaseSpec)) {
      ((BaseSpec*)md->GetOff((void*)this))->Defaults();
    }
    if(md->type->InheritsFrom(&TA_SpecMemberBase)) {
      ((SpecMemberBase*)md->GetOff((void*)this))->Defaults();
    }
  }
}

//////////////////////////////////
//      BaseSpec                //
//////////////////////////////////

bool BaseSpec::nw_itm_def_arg = false;

void BaseSpec::Initialize() {
  min_obj_type = &TA_taBase;
}

void BaseSpec::Copy_(const BaseSpec& cp) {
  //  min_obj_type = cp.min_obj_type;  // don't do this -- could be going between types
  desc = cp.desc;
  unique = cp.unique;
  children = cp.children;
}

void BaseSpec::Destroy() {
  CutLinks();
}

void BaseSpec::InitLinks() {
  inherited::InitLinks();
  taBase::Own(unique, this);
  taBase::Own(children, this);
  children.SetBaseType(GetTypeDef());
  // put in a struct bracket so thing does a full refresh when getting subspec'ed or root spec'ed
  if(!taMisc::is_loading) {
    StructUpdate(true);
    UpdateSpec();
    StructUpdate(false);
  }
}

void BaseSpec::CutLinks() {
  children.CutLinks();
  inherited::CutLinks();
}

void BaseSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  UpdateSpec();
}

void BaseSpec::Defaults() {
  DefaultsMembers();    // members has to come first, so they can be overridden by master!
  Defaults_impl();
  UpdateAfterEdit();
}

void BaseSpec::DefaultsMembers() {
  TypeDef* td = GetTypeDef();
  for(int i=TA_BaseSpec.members.size; i< td->members.size; i++) {
    MemberDef* md = td->members[i];
    if(md->type->InheritsFrom(&TA_BaseSpec)) {
      ((BaseSpec*)md->GetOff((void*)this))->Defaults();
    }
    if(md->type->InheritsFrom(&TA_SpecMemberBase)) {
      ((SpecMemberBase*)md->GetOff((void*)this))->Defaults();
    }
  }
}

void BaseSpec::CheckChildConfig_impl(bool quiet, bool& rval) {
  inherited::CheckChildConfig_impl(quiet, rval);
  children.CheckConfig(quiet, rval);
}

BaseSpec* BaseSpec::NewChild(TypeDef* child_type) {
  BaseSpec* rval = (BaseSpec*)children.New_gui(1, child_type);
  if(rval) rval->UpdateSpec();
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
    TestError(GetTypeDef()->members.size > 0, "SetUnique",
              "Member named:", memb_nm, "not found");
  }
}

void BaseSpec::SetUnique(int memb_no, bool on) {
  if(memb_no < TA_BaseSpec.members.size)
    return;
  MemberDef* md = GetTypeDef()->members[memb_no];
  if(TestError(!md, "SetUnique","Member number:", String(memb_no), "not found")) {
    return;
  }
  if(on)
    unique.AddUnique(md->name);
  else
    unique.RemoveEl(md->name);
}

bool BaseSpec::GetUnique(const char* memb_nm) {
  if(unique.FindEl(memb_nm) >= 0) return true;
  return false;
}

bool BaseSpec::GetUnique(int memb_no) {
  if(memb_no < TA_BaseSpec.members.size)
    return false;
  MemberDef* md = GetTypeDef()->members[memb_no];
  if(TestError(!md, "GetUnique", "Member number:", String(memb_no), "not found")) {
    return false;
  }
  if(unique.FindEl(md->name) >= 0) return true;
  return false;
}

bool BaseSpec::SpecInheritsFrom(BaseSpec* spec) const {
  if (this == spec) return true;
  // just iterate here, no need for recursion
  FOREACH_ELEM_IN_GROUP(BaseSpec, child, spec->children) {
    if (this == child) return true;
  }
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
  if(memb_no < frm_td->members.size) {  // parent must have this member
    MemberDef* md = td->members[memb_no];
    if(frm_td->members[memb_no] == md) {        // must be the same member
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
        // NO NO NO 1000 times no (literally!) -- this causes MASSIVE update chains
//      DataChanged(DCR_ITEM_UPDATED);
      }
    }
  }
}

void BaseSpec::UpdateChildren() {
  FOREACH_ELEM_IN_GROUP(BaseSpec, kid, children) {
    kid->UpdateAfterEdit();     // calls updatespec and updates gui too -- can be wasteful but also needed for e.g., ctrl panels to update properly, so no real way around it..
  }
}

bool BaseSpec::CheckType(TypeDef* td) {
  if(TestWarning(!td, "CheckType", "type is null, should be at least:", min_obj_type->name)) {
    return false;
  }
  if(TestWarning(!CheckType_impl(td),  "CheckType",
                 "incorrect type:", td->name,
                 "should be at least:", min_obj_type->name)) {
    return false;
  }
  return true;
}

bool BaseSpec::CheckObjectType(taBase* obj) {
  if(TestWarning(!obj, "CheckObjectType",
                 "object is null",
                 "should be at least:", min_obj_type->name)) {
    return false;
  }
  if(TestWarning(!CheckObjectType_impl(obj), "CheckObjectType",
                 "incorrect type of obj:", obj->GetTypeDef()->name,
                 "should be at least:", min_obj_type->name)) {
    return false;
  }
  return true;
}

bool BaseSpec::CheckType_impl(TypeDef* td) {
  // other specs are allowed to own any kind of other spec,
  // and layers and projections also contain specs..
  if (td->InheritsFrom(TA_BaseSpec))
    return true;

  if(!td->InheritsFrom(min_obj_type))
    return false;

  return true;
}

bool BaseSpec::CheckObjectType_impl(taBase* obj) {
  // other specs are allowed to own any kind of other spec,
  // and layers and projections also contain specs..
  if (obj->InheritsFrom(TA_BaseSpec))
    return true;

  if(!obj->InheritsFrom(min_obj_type))
    return false;

  return true;
}

String BaseSpec::WhereUsed() {
  String rval;
  taDataLink* dl = data_link();
  if(!dl) return rval;
  taSmartRef* sref;
  taDataLinkItr i;
  FOR_DLC_EL_OF_TYPE(taSmartRef, sref, dl, i) {
    taBase* sown = sref->GetOwner();
    if(!sown) continue;
    if(!sown->InheritsFrom(&TA_SpecPtr_impl)) continue;
    taBase* ownown = sown->GetOwner();
    if(ownown)
      rval += ownown->GetPathNames() + "\n";
  }
  return rval;
}


//////////////////////////////////
//      BaseSubSpec             //
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
  if(!taMisc::is_loading) {
    StructUpdate(true);
    UpdateSpec();
    StructUpdate(false);
  }
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
  if(bso == NULL)       return NULL;
  BaseSpec* bsoo = bso->FindParent(); // parent's owner
  if(bsoo == NULL)      return NULL;

  String my_path = GetPath(NULL, bso); // get my path to owner..
  MemberDef* md;
  BaseSubSpec* from = (BaseSubSpec*)bsoo->FindFromPath(my_path, md);
  if(!from || !from->InheritsFrom(TA_BaseSubSpec))
    return NULL;                        // corresponding subspec object not found..
  return from;
}

void BaseSubSpec::SetUnique(const char* memb_nm, bool on) {
  MemberDef* md = FindMember(memb_nm);
  if(md)
    SetUnique(md->idx, on);
  else {
    TestError(GetTypeDef()->members.size > 0, "SetUnique",
              "Member named:", memb_nm, "not found");
  }
}

void BaseSubSpec::SetUnique(int memb_no, bool on) {
  if(memb_no < TA_BaseSubSpec.members.size)
    return;
  MemberDef* md = GetTypeDef()->members[memb_no];
  if(TestError(!md, "SetUnique",
               "Member number:", String(memb_no), "not found")) {
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
  TestError(true, "GetUnique", "Member named:", memb_nm, "not found");
  return false;
}

bool BaseSubSpec::GetUnique(int memb_no) {
  if(memb_no < TA_BaseSubSpec.members.size)
    return false;
  MemberDef* md = GetTypeDef()->members[memb_no];
  if(TestError(!md, "GetUnique", "Member number:", String(memb_no), "not found")) {
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
  if(memb_no < frm_td->members.size) {  // parent must have this member
    MemberDef* md = td->members[memb_no];
    if(frm_td->members[memb_no] == md) {        // must be the same member
      // don't copy read only or hidden members! (usually set automatically
      // and might depend on local variables)
      if(!GetUnique(memb_no) &&
         !(md->HasOption("READ_ONLY") || md->HasOption("HIDDEN") ||
           md->HasOption("NO_INHERIT")))
      {
        MemberCopyFrom(memb_no, from);
        // NO NO NO 1000 times no (literally!) -- this causes MASSIVE update chains
//      DataChanged(DCR_ITEM_UPDATED);
      }
    }
  }
}

//////////////////////////////////
//      SpecPtr_impl            //
//////////////////////////////////

void SpecPtr_impl::Initialize() {
  owner = NULL;
  type = NULL;
  base_type = &TA_BaseSpec;
}

void SpecPtr_impl::Copy_(const SpecPtr_impl& cp) {
  type = cp.type;
  base_type = cp.base_type;
}

taBase* SpecPtr_impl::UpdatePointers_NewPar_FindNew(taBase* old_guy, taBase* old_par,
                                                    taBase* new_par) {
  taBase* new_guy = inherited::UpdatePointers_NewPar_FindNew(old_guy, old_par, new_par);
  String og_nm = old_guy->GetName();
  if(old_guy->InheritsFrom(&TA_BaseSpec) && old_guy->GetOwner() &&
     (!new_guy || (og_nm != new_guy->GetName()))) {
    // still not right -- look globally
    BaseSpec_Group* spgp = GetSpecGroup();
    if(spgp) {
      new_guy = spgp->FindSpecName(og_nm);
      if(!new_guy) {
        /// just make a new one!
        new_guy = old_guy->Clone();
        spgp->Add(new_guy);
        new_guy->SetName(og_nm);
        new_guy->DataChanged(DCR_ITEM_UPDATED);
        taMisc::Info("Note: copied Spec:", og_nm,
                     "into .specs on new network because it was not otherwise found");
      }
    }
  }
  return new_guy;
}

int SpecPtr_impl::UpdatePointers_NewObj(taBase* old_ptr, taBase* new_ptr) {
  BaseSpec* sp = GetSpec();
  if(sp != old_ptr) return 0;
  if(GetOwner(old_ptr->GetTypeDef()) == old_ptr) return 0;
  // don't replace on children of the old object
  SetSpec((BaseSpec*)new_ptr);          // call set spec so type is updated!
  return 1;
}

void SpecPtr_impl::CheckSpec(TypeDef* obj_td) {
  if (!owner || !type || !base_type || owner->isDestroying())
    return;

  if(!obj_td) obj_td = owner->GetTypeDef(); // use owner type

  Network* net = GET_MY_OWNER(Network);
  if(!net || net->isDestroying()) return;

  Network* owner_net = GET_OWNER(owner, Network);
  if(!owner_net || owner_net->isDestroying()) return;

  if(TestError(!type->InheritsFrom(base_type),
               "CheckSpec",
               "spec type:", type->name, "does not inherit from base_type:",
               base_type->name, "this should not happen (programmer error).",
               "Reverting type to base_type")) {
    type = base_type;
  }

  BaseSpec* sp = GetSpec();

  if(sp && (owner_net != net)) {        // oops!
    // try to find same name one first:
    BaseSpec_Group* spgp = GetSpecGroup();
    if(spgp) {
      BaseSpec* nsp = spgp->FindSpecName(sp->name);
      SetSpec(nsp);             // set -- either null or a candidate
      sp = nsp;                 // update our cur ptr guy
      DataChanged(DCR_ITEM_UPDATED);
    }
    else {
      SetSpec(NULL);            // get rid of existing -- will try to find new one
    }
  }

  // check again
  sp = GetSpec();
  if(sp) {
    if(sp->isDestroying()) {    // shouldn't happen, but just in case
      SetSpec(NULL);
    }
    else if(sp->GetTypeDef() != type) {
      SetSpec(NULL);            // get rid of existing spec
    }
  }

  // and again
  sp = GetSpec();
  if(!sp) {
    taMisc::CheckError("CheckSpec: spec is NULL in object:", owner->GetPathNames(),
                       "getting a new spec of type:", type->name);
    GetSpecOfType(true);        // verbose
  }

  // and again
  sp = GetSpec();
  if(sp && !obj_td->InheritsFrom(sp->min_obj_type)) {
    taMisc::CheckError("CheckSpec: incorrect type of object:", obj_td->name,
                       "for spec of type:", sp->GetTypeDef()->name,
                       "should be at least:", sp->min_obj_type->name,
                       "in object:",owner->GetPathNames(),
                       "DO NOT RUN NETWORK until fixed! (Use ChangeMyType on offending object)");
  }
}

bool SpecPtr_impl::CheckObjTypeForSpec(TypeDef* obj_td) {
  if (!owner || !type || !base_type || owner->isDestroying())
    return false;

  if(!obj_td) obj_td = owner->GetTypeDef(); // use owner type

  BaseSpec* sp = GetSpec();
  if(sp && !obj_td->InheritsFrom(sp->min_obj_type)) {
    return false;
  }
  return true;
}

void SpecPtr_impl::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  // don't do so much automatic -- just check for wrong owner and type stuff

  Network* net = GET_MY_OWNER(Network);
  if(!net || net->isDestroying()) return;

  Network* owner_net = GET_OWNER(owner, Network);
  if(!owner_net || owner_net->isDestroying()) return;

  if(TestError(type && base_type && !type->InheritsFrom(base_type),
               "UpdateAfterEdit",
               "spec type:", type->name, "does not inherit from base_type:",
               base_type->name, "this should not happen (programmer error).",
               "Reverting type to base_type")) {
    type = base_type;
  }

  BaseSpec* sp = GetSpec();

  if(sp && (owner_net != net)) {        // oops!
    // try to find same name one first:
    BaseSpec_Group* spgp = GetSpecGroup();
    if(spgp) {
      BaseSpec* nsp = spgp->FindSpecName(sp->name);
      SetSpec(nsp);             // set -- either null or a candidate
      DataChanged(DCR_ITEM_UPDATED);
    }
    else {
      SetSpec(NULL);            // get rid of existing -- will try to find new one later
    }
  }
}

void SpecPtr_impl::SetBaseType(TypeDef* td) {
  type = td;
  base_type = td;               // this doesn't get set by defaults
}

void SpecPtr_impl::SetDefaultSpec(taBase* ownr, TypeDef* td) {
  if(type == NULL)
    type = td;

  if(base_type == &TA_BaseSpec)
    base_type = td;

  owner = (taBase*)ownr;
  if(taBase::GetRefn(this) == 0) {
    taBase::Ref(this);          // refer to this object..
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

void SpecPtr_impl::GetSpecOfType(bool verbose) {
  BaseSpec_Group* spgp = GetSpecGroup();
  if(spgp == NULL)
    return;

  BaseSpec* sp = GetSpec();
  if(sp && (sp->GetTypeDef() == type))
    return;

  sp = spgp->FindSpecType(type);
  if((sp) && (sp->GetTypeDef() == type)) {
    SetSpec(sp);
    if(verbose) {
      taMisc::CheckError("GetSpecOfType for object:", owner->GetPathNames(),
                         "set spec pointer to existing spec named:", sp->name,
                         "of correct type:", type->name);
    }
    DataChanged(DCR_ITEM_UPDATED);
    return;
  }

  sp = (BaseSpec*)spgp->NewEl(1, type);
  if (sp) {
    SetSpec(sp);
    if(verbose) {
      taMisc::CheckError("GetSpecOfType for object:", owner->GetPathNames(),
                         "set spec pointer to NEW spec I just created, named:", sp->name,
                         "of type:", type->name);
    }
    DataChanged(DCR_ITEM_UPDATED);
  }
}

// todo: could add the UAE for the relevant smartref_destroy call..
// void SpecPtr_impl::DataLinkDestroying(taDataLink* dl) {
//   if (owner && !owner->isDestroying()) {
// //     owner->SmartRef_DataDestroying(this, GetSpec());
//     SetSpec(NULL);
//     owner->UpdateAfterEdit();
//   } else
//     SetSpec(NULL);
// }


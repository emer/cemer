// Copyright, 1995-2013, Regents of the University of Colorado,
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

#include "BaseSpec.h"
#include <SpecMemberBase>
#include <MemberDef>
#include <taSigLinkItr>
#include <SpecPtr>

#include <taMisc>

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

void BaseSpec::MemberUpdateAfterEdit(MemberDef* md, bool edit_dialog) {
  if(edit_dialog) return;	// no action
  SetUnique(md->name.chars(), true);	// always set to unique if it is being set somewhere
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
//      SigEmitUpdated();
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
  taSigLink* dl = sig_link();
  if(!dl) return rval;
  taSmartRef* sref;
  taSigLinkItr i;
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

void BaseSpec::SetParam(const String& param_path, const String& value) {
  TypeDef* td = GetTypeDef();
  ta_memb_ptr net_mbr_off = 0;
  int net_base_off = 0;
  MemberDef* smd = TypeDef::FindMemberPathStatic(td, net_base_off, net_mbr_off,
                                                 param_path, false); // no warn
  void* sbaddr = MemberDef::GetOff_static(this, net_base_off, net_mbr_off);

  smd->type->SetValStr(value, sbaddr, NULL, smd);
  UpdateAfterEdit();

  children.SetParam(&TA_BaseSpec, param_path, value); // use base spec here to not restrict type any further in children
}


taBase* BaseSpec::ChooseNew(taBase* origin) {
  BaseSpec* newSpec = NULL;

  if (origin->GetTypeDef()->DerivesFrom(&TA_SpecPtr_impl)) {
    SpecPtr_impl* sptr = (SpecPtr_impl*)origin;
    BaseSpec_Group* spgp = sptr->GetSpecGroup();
    if (spgp) {
      newSpec = (BaseSpec*)spgp->New(1, sptr->type); // get the type not the instance because it might be set to NULL
    }
  }
  return newSpec;
}

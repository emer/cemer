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
#include <taiWidgetTypeDefChooser>
#include <Network>


TA_BASEFUNS_CTORS_DEFN(BaseSpec);

bool BaseSpec::nw_itm_def_arg = false;

void BaseSpec::Initialize() {
  min_obj_type = &TA_taBase;
  is_used = false;
  is_new = false;
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
    if(taMisc::gui_active)      // only set as new if gui active
      is_new = true;
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
  UpdtIsUsed();
  if(taMisc::is_loading) {
    TypeDef* td = GetTypeDef();
    for(int i=TA_BaseSpec.members.size; i< td->members.size; i++) {
      MemberDef* md = td->members.FastEl(i);
      String aka = md->OptionAfter("AKA_");
      if(aka.empty()) continue;
      for(int j=0; j<unique.size; j++) {
        String& unq = unique.FastEl(j);
        if(unq == aka) {
          unq = md->name;       // translate
        }
      }
    }
  }
  UpdateSpec();
}

void BaseSpec::MemberUpdateAfterEdit(MemberDef* md, bool edit_dialog) {
  if(edit_dialog) return;	// no action
  SetUnique(md->name.chars(), true);	// always set to unique if it is being set somewhere
}

void BaseSpec::ApplyTo(SpecUser* obj1, SpecUser* obj2, SpecUser* obj3, SpecUser* obj4) {
  if(obj1 != NULL) {
    obj1->ApplySpecToMe(this);
  }
  if(obj2 != NULL) {
    obj2->ApplySpecToMe(this);
  }
  if(obj3 != NULL) {
    obj3->ApplySpecToMe(this);
  }
  if(obj4 != NULL) {
    obj4->ApplySpecToMe(this);
  }
  UpdtIsUsed();
  SigEmitUpdated();
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

void BaseSpec::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);

  UpdtIsUsed();
  
  if(is_new) {
    is_new = false;             // turn off!
    if(!is_used) {
      if(taMisc::Choice("A newly-created spec: " + name + " is not being used: do you want to apply it to some objects?", "Yes", "No") == 0) {
        CallFun("ApplyTo");
      }
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

BaseSpec* BaseSpec::FindMakeChild(const String& nm, TypeDef* td, bool& nw_itm, const String& alt_nm) {
  if(td == NULL) td = children.el_typ;
  return children.FindMakeSpec(nm, td, nw_itm, alt_nm);
}

BaseSpec* BaseSpec::FindParent() {
  return GET_MY_OWNER(BaseSpec);
}

bool BaseSpec::RemoveChild(const String& nm, TypeDef* td) {
  if(td == NULL) td = children.el_typ;
  return children.RemoveSpec(nm, td);
}

void BaseSpec::CompareWithChildren() {
  Network* network = (Network*)GetOwnerOfType(&TA_Network);
  network->SpecCompare(this);
}

void BaseSpec::SetUnique(const String& memb_nm, bool on) {
  MemberDef* md = FindMember(memb_nm);
  if(md) {
    SetUnique(md->idx, on);
  }
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

bool BaseSpec::GetUnique(const String& memb_nm) {
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

void BaseSpec::SpecSet(taBase* obj) {
  UpdtIsUsed();
  SigEmitUpdated();
}

void BaseSpec::SpecUnSet(taBase* obj) {
  UpdtIsUsed();
  SigEmitUpdated();
}

String BaseSpec::WhereUsed(bool child) {
  UpdtIsUsed();
  SigEmitUpdated();
  String rval;
  taSigLink* dl = sig_link();
  if(!dl) return rval;
  taSmartRef* sref;
  taSigLinkItr i;
  FOR_DLC_EL_OF_TYPE(taSmartRef, sref, dl, i) {
    taBase* sown = sref->GetOwner();
    if(!sown)
      continue;
    if(!sown->InheritsFrom(&TA_SpecPtr_impl))
      continue;
    taBase* ownown = sown->GetOwner();
    if(ownown) {
      SpecPtr_impl* sptr = dynamic_cast<SpecPtr_impl*>(sown);
      if (child) {
        rval += "[Child  " + sptr->GetSpec()->GetName() + "]  ";
      }
      else {
        rval += "[" + sptr->GetSpec()->GetName() + "]  ";
      }
      rval += ownown->GetPathNames() + " \n\n";
    }
  }
  
  for (int i=0; i<children.size; i++) {
    rval += children.SafeEl(i)->WhereUsed(true);
  }
  return rval;
}

bool BaseSpec::UpdtIsUsed() {
  is_used = false;
  taSigLink* dl = sig_link();
  if(!dl) {
    return is_used;
  }
  taSmartRef* sref;
  taSigLinkItr i;
  FOR_DLC_EL_OF_TYPE(taSmartRef, sref, dl, i) {
    taBase* sown = sref->GetOwner();
    if(!sown)
      continue;
    if(!sown->InheritsFrom(&TA_SpecPtr_impl))
      continue;
    taBase* ownown = sown->GetOwner();
    if(ownown) {
      is_used = true;
      return is_used;
    }
  }
  return is_used;
}

void BaseSpec::SetMember(const String& member, const String& value) {
  String mbr_eff = member;
  if(mbr_eff.contains('.'))
    mbr_eff = mbr_eff.before('.');
  SetUnique(mbr_eff, true);
  inherited::SetMember(member, value);
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

bool BaseSpec::ChooseMe() {
  taiWidgetTypeDefChooser* chooser = new taiWidgetTypeDefChooser(GetTypeDef(), NULL, NULL, NULL);
  chooser->GetImage(GetTypeDef(), GetTypeDef());
  TypeDef* new_td;
  bool okc = chooser->OpenChooser();
  if (okc) {
    new_td = chooser->td();
    ChangeMyType(new_td);
  }
  delete chooser;
  UpdateAfterEdit();
  return true;
}


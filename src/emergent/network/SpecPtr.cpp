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

#include "SpecPtr.h"
#include <Network>
#include <ProjectBase>

#include <taMisc>

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
        new_guy->SigEmitUpdated();
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
      SigEmitUpdated();
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
      SigEmitUpdated();
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
    SigEmitUpdated();
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
    SigEmitUpdated();
  }
}

// todo: could add the UAE for the relevant smartref_destroy call..
// void SpecPtr_impl::SigLinkDestroying(taSigLink* dl) {
//   if (owner && !owner->isDestroying()) {
// //     owner->SmartRef_SigDestroying(this, GetSpec());
//     SetSpec(NULL);
//     owner->UpdateAfterEdit();
//   } else
//     SetSpec(NULL);
// }

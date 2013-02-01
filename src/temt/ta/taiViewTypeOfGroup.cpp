// Copyright, 1995-2013, Regents of the University of Colorado,
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

#include "taiViewTypeOfGroup.h"
#include <taSigLinkGroup>
#include <taiType>
#include <taiEdit>
#include <taiMember>
#include <taiMethod>

#include <taMisc>
#include <taiMisc>

TypeDef_Of(taGroup_impl);
TypeDef_Of(taiArgType);


int taiViewTypeOfGroup::BidForView(TypeDef* td) {
  if (td->InheritsFrom(&TA_taGroup_impl))
    return (inherited::BidForView(td) +1);
  return 0;
}

taiSigLink* taiViewTypeOfGroup::CreateSigLink_impl(taBase* data_) {
  return new taSigLinkGroup((taGroup_impl*)data_);
}

void taiTypeBase::InitializeTypes(bool gui) {
  int i,j,k;
  TypeDef* td;

  TypeSpace i_type_space;
  TypeSpace v_type_space;
  TypeSpace i_memb_space;
  TypeSpace i_meth_space;
  TypeSpace i_edit_space;

  for (i=0; i < taMisc::types.size; ++i) {
    td = taMisc::types.FastEl(i);

    // make the hash tables to optimize css ops, path finding, etc
    td->CacheParents();
    td->members.BuildHashTable(td->members.size + td->members.size / 4);
    td->methods.BuildHashTable(td->methods.size + td->methods.size / 4);

    // generate a list of all the qt types
    if (td->instance) {
      if (gui && td->InheritsFrom(TA_taiType)
         && !(td->InheritsFrom(TA_taiMember) || td->InheritsFrom(TA_taiMethod) ||
              td->InheritsFrom(TA_taiArgType) || td->InheritsFrom(TA_taiEdit)))
        i_type_space.Link(td);

      // generate a list of all the view types
      if (td->InheritsFrom(TA_taiViewType))
        v_type_space.Link(td);

      // generate a list of all the member_i types
      if (gui && td->InheritsFrom(TA_taiMember))
        i_memb_space.Link(td);

      // generate a list of all the method_i types
      if (gui && td->InheritsFrom(TA_taiMethod))
        i_meth_space.Link(td);

      // generate a list of all the method arg types to be used later
      if (gui && td->InheritsFrom(TA_taiArgType))
        taiMisc::arg_types.Link(td);

      // generate a list of all the ie types (edit dialogs)
      if (gui && td->InheritsFrom(TA_taiEdit))
        i_edit_space.Link(td);
    }
  }

  if (gui && (i_type_space.size == 0))
    taMisc::Error("taiInit: warning: no taiType's found with instance != NULL");
  if (gui && (i_memb_space.size == 0))
    taMisc::Error("taiInit: warning: no taiMembers's found with instance != NULL");
  if (gui && (i_edit_space.size == 0))
    taMisc::Error("taiInit: warning: no taiEdit's found with instance != NULL");

  // go through all the types and assign the highest bid for
  //   the it, iv, and ie
  int bid;

  for (i=0; i < taMisc::types.size; ++i) {
    td = taMisc::types.FastEl(i);
    if (gui) {
      for (j=0; j <i_type_space.size; ++j) {
        taiType* tit_i = (taiType*) i_type_space.FastEl(j)->GetInstance();
        bid = tit_i->BidForType(td);
        if (bid > 0) {
          taiType* tit = tit_i->TypeInst(td); // make one
          tit->bid = bid;
          tit->AddToType(td);             // add it
        }
      }
    }

    for (j=0; j < v_type_space.size; ++j) {
      taiViewType* tit_v = (taiViewType*) v_type_space.FastEl(j)->GetInstance();
      bid = tit_v->BidForView(td);
      if (bid > 0) {
        taiViewType* tiv = tit_v->TypeInst(td); // make one
        tiv->bid = bid;
        tiv->AddView(td);               // add it
      }
    }

    if (gui) {
      for (j=0; j < i_edit_space.size; ++j) {
        taiEdit* tie_i = (taiEdit*) i_edit_space.FastEl(j)->GetInstance();
        bid = tie_i->BidForEdit(td);
        if (bid > 0) {
          taiEdit* tie = tie_i->TypeInst(td);
          tie->bid = bid;
          tie->AddEdit(td);
        }
      }
    }

    // go though all the types and find the ones that are classes
    // for each class type go through the members and assign
    // the highest bid for the member's it (may be based on opts field)
    // and do the enum types since they are not global and only on members

    if (gui && td->IsActualClassNoEff()) {
      for (j=0; j < td->members.size; ++j) {
        MemberDef* md = td->members.FastEl(j);
        if (md->owner->owner != td) continue; // if we do not own this mdef, skip
        for (k=0; k < i_memb_space.size; ++k) {
          taiMember* tim_i = (taiMember*) i_memb_space.FastEl(k)->GetInstance();
          bid = tim_i->BidForMember(md,td);
          if (bid > 0) {
            taiMember* tim = tim_i->MembInst(md,td);
            tim->bid = bid;
            tim->AddMember(md);
          }
        }
      }

      for(j=0; j < td->sub_types.size; ++j) {
        TypeDef* subt = td->sub_types.FastEl(j);
        for(k=0; k < i_type_space.size; ++k) {
          taiType* tit_i = (taiType*) i_type_space.FastEl(k)->GetInstance();
          bid = tit_i->BidForType(subt);
          if (bid > 0) {
            taiType* tit = tit_i->TypeInst(subt); // make one
            tit->bid = bid;
            tit->AddToType(subt);               // add it
          }
        }
      }

      // only assign method im's to those methods that do better than the default
      // (which has a value of 0).  Thus, most methods don't generate a new object here

      for (j=0; j < td->methods.size; ++j) {
        MethodDef* md = td->methods.FastEl(j);
        if (md->owner->owner != td) continue; // if we do not own this mdef, skip
        for (k=0; k < i_meth_space.size; ++k) {
          taiMethod* tim_i = (taiMethod*) i_meth_space.FastEl(k)->GetInstance();
          bid = tim_i->BidForMethod(md,td);
          if (bid > 0) {
            taiMethod* tim = tim_i->MethInst(md,td);
            tim->bid = bid;
            tim->AddMethod(md);
          }
        }
      }

    } // td->IsActualClassNoEff()

  } // for each Type

  // link in compatible members as properties of every type
  // "compatible" means that it isn't already a property, and is otherwise accessible
  for (i=0; i < taMisc::types.size; ++i){
    td = taMisc::types.FastEl(i);
    for (int j = 0; j < td->members.size; ++j) {
      MemberDef* md = td->members.FastEl(j);
      MemberDefBase* pd = td->properties.FindName(md->name);
      if (pd) continue; // already exists
      // ok, see if it is "compatible"
      if (!md->type->IsVarCompat()) continue; // can't read/write by Variant
      // TODO: anything????
      td->properties.Link(md);
    }
  }
}

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

#include "taSubGroup.h"
#include <taGroup_impl>

#include <SigLinkSignal>

TA_BASEFUNS_CTORS_DEFN(taSubGroup);

TA_BASEFUNS_CTORS_DEFN(taGroup_List);

void taSubGroup::SigEmit(int sls, void* op1, void* op2) {
  if (owner == NULL) return;
  // send LIST events to the owning group as a GROUP_ITEM event
  if ((sls >= SLS_LIST_ITEM_TO_GROUP_MIN) && (sls <= SLS_LIST_ITEM_TO_GROUP_MAX))
    ((taGroup_impl*)owner)->SigEmit(sls + SLS_ListItem_Group_Offset, op1, op2);
}

bool taSubGroup::Transfer(taBase* it) {
  // need to leaf count on parent group
  taGroup_impl* myown = (taGroup_impl*)owner;
  taGroup_impl* git = (taGroup_impl*)it;
  if((git->super_gp == myown) || (git->super_gp == NULL))
    return false;
  taGroup_impl* old_own = git->super_gp;
  bool rval = inherited::Transfer(git);
  //TODO: notification is not right, because counts are not rejigged yet!
  if (rval) {
    old_own->UpdateLeafCount_(-git->leaves);
    if(myown != NULL) {
      myown->UpdateLeafCount_(git->leaves);
    }
  }
  return rval;
}

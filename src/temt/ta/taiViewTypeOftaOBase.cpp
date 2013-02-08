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

#include "taiViewTypeOftaOBase.h"
#include <iPanelSet>
#include <UserDataItem_List>
#include <iPanelOfUserData>
#include <iPanelOfDocView>
#include <taDoc>
#include <taSigLinkTaOBase>
#include <iPanelOfList>



int taiViewTypeOftaOBase::BidForView(TypeDef* td) {
  if (td->InheritsFrom(&TA_taOBase))
    return (inherited::BidForView(td) +1);
  return 0;
}

void taiViewTypeOftaOBase::CheckUpdateDataPanelSet(iPanelSet* pan) {
  // TODO (4.1): check for new UserData
  // TODO: check for a new DocLink
  // if we have UserData, make a panel for it
  taiSigLink* dl = pan->link();
  if (!dl) return;
  taOBase* tab = dynamic_cast<taOBase*>(dl->taData());
  if (!tab) return; // shouldn't happen
  UserDataItem_List* udl = tab->GetUserDataList(false); // no force
  if (udl) { // note: if not, can't have DocLink either...
    // get or make a panel for Userdata, if any visible
    int start_idx = 0;
    bool udi_visible = udl->hasVisibleItems();
    iPanelOfUserData* udp = (iPanelOfUserData*)pan->GetDataPanelOfType(&TA_iPanelOfUserData, start_idx);
    // leave an existing panel if none visible, but don't make one if not
    if (!udp && udi_visible) {
      udp = new iPanelOfUserData((taiSigLink*)udl->GetSigLink());
      DataPanelCreated(udp);
    }

    // if we have a DocLink, get or make a panel for it
    // if the content changes or it gets deleted, the panel updates
    // if we set a new Doc or null it out, we get a USER_DATA_UPDATED notify, and reset it
    taDoc* doc = tab->GetDocLink();
    // get an existing DocPanel, if any -- don't force yet...
    start_idx = 0;
    iPanelOfDocView* dp = (iPanelOfDocView*)pan->GetDataPanelOfType(&TA_iPanelOfDocView, start_idx);
    if (doc) {
      if (!dp) { // need to create one
        dp = new iPanelOfDocView; // note special case: no ctor link
        DataPanelCreated(dp);
      }
      dp->setDoc(doc); // sets link
    } else {
      if (dp) {
        dp->setDoc(NULL);
        //NOTE: we currently don't support removing data panels
      }
    }
  }
}

taiSigLink* taiViewTypeOftaOBase::CreateSigLink_impl(taBase* data_) {
  return new taSigLinkTaOBase((taOBase*)data_);
}

void taiViewTypeOftaOBase::CreateDataPanel_impl(taiSigLink* dl_)
{
  // if we have defchildren, make a list panel
  // notes:
  // 1. taList will never have defchildren, so it will make the panel for itself
  // 2. for taOBase guys, the list goes second, since the obj is germane
  inherited::CreateDataPanel_impl(dl_);
  if (typ && typ->OptionAfter("DEF_CHILD_").nonempty()) {
    String custom_name = typ->OptionAfter("DEF_CHILDNAME_"); // optional
    custom_name.gsub("_", " ");
    dl_ = dynamic_cast<taSigLinkTaOBase*>(dl_)->listLink();
    if(dl_) {
      iPanelOfList* bldp = new iPanelOfList(dl_, custom_name);
      DataPanelCreated(bldp);
    }
  }
  // if we have UserData, make a panel for it
  taOBase* tab = dynamic_cast<taOBase*>(dl_->taData());
  if (!tab) return; // shouldn't happen
  UserDataItem_List* udl = tab->GetUserDataList(false); // no force
  if (udl) { // note: if not, can't have DocLink either...
    bool udi_visible = udl->hasVisibleItems();
    // only make one for UserData if any visible
    if (udi_visible) {
      iPanelOfUserData* udp = new iPanelOfUserData((taiSigLink*)udl->GetSigLink());
      DataPanelCreated(udp);
    }

    // if we have a DocLink, make a panel for it
    // if the content changes or it gets deleted, the panel updates
    // if we set a new Doc or null it out, we get a USER_DATA_UPDATED notify, and reset it
    taDoc* doc = tab->GetDocLink();
    if (doc) {
      iPanelOfDocView* dp = new iPanelOfDocView; // note special case: no ctor link
      DataPanelCreated(dp);
      dp->setDoc(doc); // sets link
    }
  }
}

taiSigLink* taiViewTypeOftaOBase::GetSigLink(void* data_, TypeDef* el_typ) {
  taOBase* data = (taOBase*)data_;
  taSigLink* dl = *(data->addr_sig_link());
  if (dl) return (taiSigLink*)dl;
  else return CreateSigLink_impl(data);
}

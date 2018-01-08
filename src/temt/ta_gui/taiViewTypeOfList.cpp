// Copyright 2013-2018, Regents of the University of Colorado,
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

#include "taiViewTypeOfList.h"
#include <taSigLinkList>
#include <iPanelOfList>



int taiViewTypeOfList::BidForView(TypeDef* td) {
  if (td->InheritsFrom(&TA_taList_impl))
    return (inherited::BidForView(td) +1);
  return 0;
}

taiSigLink* taiViewTypeOfList::CreateSigLink_impl(taBase* data_) {
  return new taSigLinkList((taList_impl*)data_);
}

void taiViewTypeOfList::CreateDataPanel_impl(taiSigLink* dl_)
{
  // we create ours first, because it should be the default
  iPanelOfList* bldp = new iPanelOfList(dl_);
  DataPanelCreated(bldp);
  inherited::CreateDataPanel_impl(dl_);
}

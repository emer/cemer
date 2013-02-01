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

#include "taiViewTypeOfProgramGroup.h"
#include <iTreeView>
#include <iProgramGroupPanel>

TypeDef_Of(Program_Group);

int taiViewTypeOfProgramGroup::BidForView(TypeDef* td) {
  if (td->InheritsFrom(&TA_Program_Group))
    return (inherited::BidForView(td) +1);
  return 0;
}

/*taiSigLink* taiViewTypeOfDataTable::CreateSigLink_impl(taBase* data_) {
  return new taSigLinkList((taList_impl*)data_);
} */

void taiViewTypeOfProgramGroup::CreateDataPanel_impl(taiSigLink* dl_)
{
  // we create ours first, because it should be the default
  iProgramGroupPanel* dp = new iProgramGroupPanel(dl_);
  DataPanelCreated(dp);
  inherited::CreateDataPanel_impl(dl_);
}

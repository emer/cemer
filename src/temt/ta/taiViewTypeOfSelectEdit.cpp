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

#include "taiViewTypeOfSelectEdit.h"
#include <iSelectEditPanel>
#include <ClusterRun>
#include <iDataTablePanel_Mbr>


int taiViewTypeOfSelectEdit::BidForView(TypeDef* td) {
  if (td->InheritsFrom(&TA_SelectEdit))
    return (inherited::BidForView(td) +1);
  return 0;
}

void taiViewTypeOfSelectEdit::CreateDataPanel_impl(taiDataLink* dl_)
{
  // we create ours first, because it should be the default
  iSelectEditPanel* bldp = new iSelectEditPanel(dl_);
  DataPanelCreated(bldp);

  if(ClusterRun* cr = dynamic_cast<ClusterRun *>(dl_->taData())) {
    if(taiDataLink *datatableLink = dynamic_cast<taiDataLink *>(cr->jobs_running.GetDataLink())) {
      iDataTablePanel_Mbr* dp = new iDataTablePanel_Mbr(datatableLink);
      DataPanelCreated(dp);
    }
    if (taiDataLink *datatableLink = dynamic_cast<taiDataLink *>(cr->jobs_done.GetDataLink())) {
      iDataTablePanel_Mbr* dp = new iDataTablePanel_Mbr(datatableLink);
      DataPanelCreated(dp);
    }
  }

  inherited::CreateDataPanel_impl(dl_);
}


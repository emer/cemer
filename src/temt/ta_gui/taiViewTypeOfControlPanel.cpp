// Copyright 2016, Regents of the University of Colorado,
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

#include "taiViewTypeOfControlPanel.h"
#include <iPanelOfControlPanel>
#include <ClusterRun>
#include <iPanelOfDataTable_Mbr>


int taiViewTypeOfControlPanel::BidForView(TypeDef* td) {
  if (td->InheritsFrom(&TA_ControlPanel))
    return (inherited::BidForView(td) +1);
  return 0;
}

void taiViewTypeOfControlPanel::CreateDataPanel_impl(taiSigLink* dl_)
{
  // we create ours first, because it should be the default
  iPanelOfControlPanel* bldp = new iPanelOfControlPanel(dl_);
  DataPanelCreated(bldp);

  if(ClusterRun* cr = dynamic_cast<ClusterRun *>(dl_->taData())) {
    if(taiSigLink *datatableLink = dynamic_cast<taiSigLink *>(cr->jobs_running.GetSigLink())) {
      iPanelOfDataTable_Mbr* dp = new iPanelOfDataTable_Mbr(datatableLink);
      DataPanelCreated(dp);
    }
    if (taiSigLink *datatableLink = dynamic_cast<taiSigLink *>(cr->jobs_done.GetSigLink())) {
      iPanelOfDataTable_Mbr* dp = new iPanelOfDataTable_Mbr(datatableLink);
      DataPanelCreated(dp);
    }
    if (taiSigLink *datatableLink = dynamic_cast<taiSigLink *>(cr->jobs_deleted.GetSigLink())) {
      iPanelOfDataTable_Mbr* dp = new iPanelOfDataTable_Mbr(datatableLink);
      DataPanelCreated(dp);
    }
    if (taiSigLink *datatableLink = dynamic_cast<taiSigLink *>(cr->jobs_archive.GetSigLink())) {
      iPanelOfDataTable_Mbr* dp = new iPanelOfDataTable_Mbr(datatableLink);
      DataPanelCreated(dp);
    }
    if (taiSigLink *datatableLink = dynamic_cast<taiSigLink *>(cr->file_list.GetSigLink())) {
      iPanelOfDataTable_Mbr* dp = new iPanelOfDataTable_Mbr(datatableLink);
      DataPanelCreated(dp);
    }
    if (taiSigLink *datatableLink = dynamic_cast<taiSigLink *>(cr->cluster_info.GetSigLink())) {
      iPanelOfDataTable_Mbr* dp = new iPanelOfDataTable_Mbr(datatableLink);
      DataPanelCreated(dp);
    }
  }

  inherited::CreateDataPanel_impl(dl_);
}


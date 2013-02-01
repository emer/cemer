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

#include "taiViewTypeOfProgram.h"
#include <iProgramCtrlPanel>
#include <iTreeView>
#include <iProgramPanel>
#include <iProgramViewScriptPanel>

TypeDef_Of(Program);

int taiViewTypeOfProgram::BidForView(TypeDef* td) {
  if (td->InheritsFrom(&TA_Program))
    return (inherited::BidForView(td) +1);
  return 0;
}

/*taiSigLink* taiViewTypeOfDataTable::CreateDataLink_impl(taBase* data_) {
  return new taSigLinkList((taList_impl*)data_);
} */

void taiViewTypeOfProgram::CreateDataPanel_impl(taiSigLink* dl_)
{
  // control panel is default
  iProgramCtrlPanel* cp = new iProgramCtrlPanel(dl_);
  DataPanelCreated(cp);

  // then editor
  iProgramPanel* dp = new iProgramPanel(dl_);
  DataPanelCreated(dp);

  // then view script
  iProgramViewScriptPanel* vp = new iProgramViewScriptPanel(dl_);
  DataPanelCreated(vp);
  inherited::CreateDataPanel_impl(dl_);
}

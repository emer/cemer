// Copyright 2015, Regents of the University of Colorado,
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

#include "taiViewTypeOfWizard.h"
#include <iPanelOfDocView>
#include <taWizard>

taTypeDef_Of(PluginWizard);
taTypeDef_Of(StartupWizard);


int taiViewTypeOfWizard::BidForView(TypeDef* td) {
  if(td->InheritsFrom(&TA_taWizard) && !td->InheritsFrom(&TA_PluginWizard)
     && !td->InheritsFrom(&TA_StartupWizard))
    return (inherited::BidForView(td) +1);
  return 0;
}

void taiViewTypeOfWizard::CreateDataPanel_impl(taiSigLink* dl_)
{
  // doc view is default
  iPanelOfDocView* cp = new iPanelOfDocView();
  cp->setUpdateOnShow(false); // no way -- user must refresh
  taWizard* wiz = (taWizard*)dl_->data();
  cp->setDoc(&(wiz->wiz_doc));
  DataPanelCreated(cp);

  // then standard properties
  inherited::CreateDataPanel_impl(dl_);
}


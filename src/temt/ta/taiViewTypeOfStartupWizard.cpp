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

#include "taiViewTypeOfStartupWizard.h"
#include <StartupWizard>
#include <iPanelOfStartupWizard>

int taiViewTypeOfStartupWizard::BidForView(TypeDef* td) {
  if(td->InheritsFrom(&TA_StartupWizard))
    return (inherited::BidForView(td)+2);
  return 0;
}

void taiViewTypeOfStartupWizard::CreateDataPanel_impl(taiSigLink* dl_)
{
  // doc view is default
  iPanelOfStartupWizard* cp = new iPanelOfStartupWizard(dl_);
  // cp->setUpdateOnShow(false); // no way -- user must refresh
  // taWizard* wiz = (taWizard*)dl_->data();
  // cp->setDoc(&(wiz->wiz_doc));
  DataPanelCreated(cp);

  // then standard properties
  inherited::CreateDataPanel_impl(dl_);
}




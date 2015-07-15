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

#include "taiViewTypeOfDoc.h"
#include <iPanelOfDocView>
#include <iPanelOfDocEdit>

taTypeDef_Of(taDoc);

int taiViewTypeOfDoc::BidForView(TypeDef* td) {
  if (td->InheritsFrom(&TA_taDoc))
    return (inherited::BidForView(td) +1);
  return 0;
}

void taiViewTypeOfDoc::CreateDataPanel_impl(taiSigLink* dl_)
{
  // doc view is default
  iPanelOfDocView* cp = new iPanelOfDocView();
  cp->setUpdateOnShow(false); // no way -- user must refresh
  cp->setDoc((taDoc*)dl_->data());
  DataPanelCreated(cp);

  // then source editor
  iPanelOfDocEdit* dp = new iPanelOfDocEdit(dl_);
  dp->setUpdateOnShow(false); // no way -- leave where it is
  DataPanelCreated(dp);

  // then standard properties
  inherited::CreateDataPanel_impl(dl_);
}


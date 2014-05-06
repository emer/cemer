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

#include "StartupWizard.h"
#include <taRootBase>

#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(StartupWizard);

void StartupWizard::Initialize() {
  auto_open = true;
  name = "StartupWizard";
}

void StartupWizard::InitLinks() {
  inherited::InitLinks();
  taBase::Own(rec_files_ref, this);
  taRootBase* root = GET_MY_OWNER(taRootBase);
  if(root) {
    rec_files_ref = &(root->recent_files);
  }
}

void StartupWizard::SmartRef_SigEmit(taSmartRef* ref, taBase* obj,
                                     int sls, void* op1_, void* op2_) override {
  if(!taMisc::gui_active) return;
  SigEmitUpdated();             // just update us
}

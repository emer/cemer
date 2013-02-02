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

#include "T3DataViewMain.h"
#include <T3Panel>
#include <T3ExaminerViewer>


void T3DataViewMain::InitLinks() {
  inherited::InitLinks();
  taBase::Own(main_xform, this);
}

void T3DataViewMain::Copy_(const T3DataViewMain& cp) {
  main_xform = cp.main_xform;
}


void T3DataViewMain::CopyFromViewFrame(T3DataViewPar* cp) {
  T3Panel* vwr = GetFrame();
  if(!vwr) return;
  T3Panel* cp_vwr = cp->GetFrame();
  if(!cp_vwr || vwr == cp_vwr) return;

  vwr->CopyFromViewFrame(cp_vwr);
}

void T3DataViewMain::setInteractionModeOn(bool on_off, bool re_render) {
  T3ExaminerViewer* vw = GetViewer();
  if(vw) {
    vw->setInteractionModeOn(on_off, re_render);
  }
}


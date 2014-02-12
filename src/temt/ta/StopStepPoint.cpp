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

#include "StopStepPoint.h"
#include <Program>
#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(StopStepPoint);


void StopStepPoint::Initialize() {
}

void StopStepPoint::GenCssBody_impl(Program* prog) {
  prog->AddLine(this, "StopCheck(); // check for Stop or Step button", ProgLine::MAIN_LINE);
  prog->AddVerboseLine(this);
}

String StopStepPoint::GetDisplayName() const {
  return "Stop/Step Point";
}

void StopStepPoint::InitLinks() {
  inherited::InitLinks();
  InitLinks_taAuto(&TA_StopStepPoint);
}

void StopStepPoint::PreGenMe_impl(int item_id) {
  // register as a subproc
  Program* prog = program();
  if (!prog) return; // shouldn't normally happen
  prog->SetProgFlag(Program::SELF_STEP);
}

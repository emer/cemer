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

#include "Loop.h"
#include <Program>
#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(Loop);


void Loop::CheckChildConfig_impl(bool quiet, bool& rval) {
  inherited::CheckChildConfig_impl(quiet, rval);
  loop_code.CheckConfig(quiet, rval);
}

void Loop::GenCssBody_impl(Program* prog) {
  loop_code.GenCss(prog);
}

const String Loop::GenListing_children(int indent_level) {
  return loop_code.GenListing(indent_level + 1);
}

void Loop::PreGenChildren_impl(int& item_id) {
  loop_code.PreGen(item_id);
}
ProgVar* Loop::FindVarName(const String& var_nm) const {
  return loop_code.FindVarName(var_nm);
}

bool Loop::BrowserEditTest() {
  bool rval = inherited::BrowserEditTest();
  bool rv2 = loop_code.BrowserEditTest();
  return rval | rv2;
}

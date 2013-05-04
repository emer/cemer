// Copyright, 1995-2013, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of Emergent
//
//   Emergent is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   Emergent is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.

#include "ActrModel.h"

#include <ActrDeclarativeModule>

void ActrModel::Initialize() {
  util_lrate = 0.02f;
  util_noise = 0.02f;
  prod_time = 0.050f;
  cur_time = 0.0f;
}

void ActrModel::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  DefaultConfig();
}

void ActrModel::DefaultConfig() {
  if(!buffers.FindName("goal")) {
    goal_buf = (ActrBuffer*)buffers.New(1);
    goal_buf->name = "goal";
  }
  if(!modules.FindName("declarative")) {
    decl_mod = (ActrModule*)modules.New(1, &TA_ActrDeclarativeModule);
    decl_mod->name = "declarative";
    decl_mod->InitBuffer(*this);
  }
}

// Copyright 2013-2018, Regents of the University of Colorado,
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

#include "taPluginBase.h"

TA_BASEFUNS_CTORS_DEFN(taPluginBase);

void taPluginBase::Initialize() {
}

void taPluginBase::Copy_(const taPluginBase& cp) {
  name = cp.name;
  desc = cp.desc;
  unique_id = cp.unique_id;
  version = cp.version;
  url = cp.url;
}

// Copyright 2017-2018, Regents of the University of Colorado,
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

#include "ArchivedParams_Group.h"
#include <ParamSet>

TA_BASEFUNS_CTORS_DEFN(ArchivedParams_Group);

ParamSet* ArchivedParams_Group::NewArchive() {
  ParamSet* rval = new ParamSet;
  Insert(rval, 0);
  rval->UpDate();
  return rval;
}

ParamSet* ArchivedParams_Group::Archive(ControlPanel* params) {
  if(params) {
    return params->Archive(_nilString);
  }
  return NULL;
}

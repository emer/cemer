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

#include "Network_Group.h"

TA_BASEFUNS_CTORS_DEFN(Network_Group);

taBrainAtlas_List Network_Group::brain_atlases;

void Network_Group::InitLinks() {
  inherited::InitLinks();
  if(brain_atlases.not_init) {
    taBase::Ref(brain_atlases);
    brain_atlases.LoadAtlases();
  }
}

void Network_Group::CutLinks() {
  inherited::CutLinks();
}


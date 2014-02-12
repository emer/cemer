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

#include "KwtaSortBuff.h"

TA_BASEFUNS_CTORS_DEFN(KwtaSortBuff);

void KwtaSortBuff::Initialize() {
}

void KwtaSortBuff::InitLinks() {
  inherited::InitLinks();
  taBase::Own(kbuff, this);
  taBase::Own(sizes, this);
}

void KwtaSortBuff::CutLinks() {
  kbuff.CutLinks();
  sizes.CutLinks();
  inherited::CutLinks();
}

void KwtaSortBuff::Copy_(const KwtaSortBuff& cp) {
  kbuff = cp.kbuff;
  sizes = cp.sizes;
}


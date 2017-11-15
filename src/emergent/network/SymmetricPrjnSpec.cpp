// Copyright 2017, Regents of the University of Colorado,
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

#include "SymmetricPrjnSpec.h"
#include <Network>

TA_BASEFUNS_CTORS_DEFN(SymmetricPrjnSpec);

void SymmetricPrjnSpec::Connect_impl(Projection* prjn, int make_cons) {
  TestError(true, "Connect_impl",
            "Sorry, this is obsolete and does not work with the new way that connections are made, due to optimizations.  Please replace with a projection that does the connectivity directly.  See TiledGpRFPrjnSpec for an example of a 'reciprocal' option that creates the symmetric version of a projection using the same params.");
}




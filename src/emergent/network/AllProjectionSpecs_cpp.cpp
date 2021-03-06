// Copyright 2017-2018, Regents of the University of Colorado,
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

#include "AllProjectionSpecs_cpp.h"

#include <ConSpec_cpp>
#include <PrjnState_cpp>

#include <State_cpp>

// impl

#include "FullPrjnSpec.cpp"

#include "OneToOnePrjnSpec.cpp"
#include "GpOneToOnePrjnSpec.cpp"
#include "MarkerGpOneToOnePrjnSpec.cpp"
#include "GpMapConvergePrjnSpec.cpp"
#include "GpMapDivergePrjnSpec.cpp"

#include "RandomPrjnSpec.cpp"
#include "UniformRndPrjnSpec.cpp"
#include "PolarRndPrjnSpec.cpp"
#include "SymmetricPrjnSpec.cpp"

#include "TesselPrjnSpec.cpp"
#include "GpTesselPrjnSpec.cpp"

#include "TiledGpRFPrjnSpec.cpp"
#include "TiledGpRFOneToOnePrjnSpec.cpp"
#include "TiledGpRFOneToOneWtsPrjnSpec.cpp"
#include "TiledSubGpRFPrjnSpec.cpp"

#include "TiledRFPrjnSpec.cpp"
#include "TiledNovlpPrjnSpec.cpp"
#include "TiledGpMapConvergePrjnSpec.cpp"

#include "GaussRFPrjnSpec.cpp"
#include "GradientWtsPrjnSpec.cpp"

#include "PFCPrjnSpec.cpp"
#include "BgPfcPrjnSpec.cpp"

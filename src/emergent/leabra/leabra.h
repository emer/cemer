// Copyright, 1995-2007, Regents of the University of Colorado,
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

// leabra.h 

#ifndef leabra_h
#define leabra_h

#include <Network>
#include <FunLookup>
#include <float_CircBuffer>
#include <voidptr_Matrix>
#include <int_Array>
#include <Schedule>
#include <SpecMemberBase>
#include <taMath_float>
#include <int_Matrix>
#include <ProjectBase>
#include <Wizard>


// these are needed for FLT_MAX
#include <math.h>
#include <limits.h>
#include <float.h>

// pre-declare

class LeabraCon;
class LeabraConSpec;
class LeabraBiasSpec;
class LeabraRecvCons;
class LeabraSendCons;

class LeabraUnitSpec;
class LeabraUnit;
class LeabraUnit_Group;

class LeabraPrjn;

class LeabraInhib;
class LeabraLayerSpec;
class LeabraLayer;

class KwtaSortBuff;
class KwtaSortBuff_List;

class LeabraNetwork;
class LeabraProject; 


#endif // leabra_h

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

#ifndef FastWtCon_h
#define FastWtCon_h 1

// parent includes:
#include <LeabraCon>

// member includes:

// declare all other types mentioned but not required to include:

TypeDef_Of(FastWtCon);

class LEABRA_API FastWtCon : public LeabraCon {
  // fast weight connection: standard wt learns fast, but decays toward slow weight value
public:
  float		swt;		// slow weight value
  float		sdwt;		// #NO_SAVE slow weight delta-weight change

  FastWtCon() { swt = sdwt = 0.0f; }
};

#endif // FastWtCon_h

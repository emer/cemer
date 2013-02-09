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

#ifndef MatrixCon_h
#define MatrixCon_h 1

// parent includes:
#include <LeabraCon>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(MatrixCon);

class MatrixCon : public LeabraCon {
  // connection for recv cons into matrix units -- stores the sender activation prior to any gating update signals, used for learning
public:
  float		sact_lrn;	// #NO_SAVE sending activation value used for learning -- prior to any gating-based update in activation states

  MatrixCon() { sact_lrn = 0.0f; }
};

#endif // MatrixCon_h

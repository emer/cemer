// Copyright 2013-2018, Regents of the University of Colorado,
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

#ifndef MSNCon_h
#define MSNCon_h 1

// parent includes:
#include <LeabraCon>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(MSNCon);

class E_API MSNCon : public LeabraCon {
  // connection for recv cons into matrix units -- stores the sender activation prior to any gating update signals, used for learning
public:
  float		ntr;	// #NO_SAVE new trace -- drives updates to trace value -- su * (1-ru_msn) for gated, or su * ru_msn for not-gated (or for non-thalamic cases)
  float         tr;     // #NO_SAVE current ongoing trace of activations, which drive learning -- adds ntr and clears after learning on current values -- includes both thal gated (+ and other nongated, - inputs)

  MSNCon() { ntr = tr = 0.0f; }
};

#endif // MSNCon_h

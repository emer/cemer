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

#ifndef bp_cuda_h
#define bp_cuda_h 1

// parent includes:
#include <Network_cuda.h>

// todo: replace function pointers with enums in base conspecs
// replace lots of standard unit spec options with an enum
// make custom sigmoid specs a special option

class BpUnitSpec_cuda {
  // NVIDIA CUDA backprop unit spec params -- only works for standard sig specs etc
public:
  float         err_tol;
};

class BpConSpec_cuda {
  // NVIDIA CUDA backprop con spec params -- 
public:
  float         cur_lrate;
  float         momentum;
  float         decay;
};


class Bp_cuda : public Network_cuda {
  // NVIDIA CUDA support for backprop computation
public:

  
  void  Compute_NetinAct();

};


#endif // bp_cuda_h

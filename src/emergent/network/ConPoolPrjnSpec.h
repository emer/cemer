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

#ifndef ConPoolPrjnSpec_h
#define ConPoolPrjnSpec_h 1

// parent includes:
#include <ProjectionSpec>

// member includes:
#include <PosVector2i>

// declare all other types mentioned but not required to include:

eTypeDef_Of(ConPoolPrjnSpec);

class E_API ConPoolPrjnSpec : public ProjectionSpec {
  // self-projection that interconnects neighboring pools of units with each other according to the pooling size in each dimension -- e.g., with pool_size = 2, every sequential pair of units is interconnected
INHERITED(ProjectionSpec)
public:
  PosVector2i   pool_size;              // how many units to interconnect together in a pool in each dimension
  PosVector2i   stride;                 // how many units to skip over for every pool connection step -- if stride == pool_size then pools are isolated from each other, whereas stride < pool_size causes the edges of each pool to overlap with others

  void Connect_impl(Projection* prjn, bool make_cons) override;
  
  TA_SIMPLE_BASEFUNS(ConPoolPrjnSpec);
private:
  void Initialize();
  void Destroy()     { };
};

#endif // ConPoolPrjnSpec_h

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

// declare all other types mentioned but not required to include:

eTypeDef_Of(ConPoolPrjnSpec);

class E_API ConPoolPrjnSpec : public ProjectionSpec {
  // self-projection that interconnects pools of units with each other, in sequential index order according to the pooling size -- e.g., with pool_size = 2, every sequential pair of units is interconnected
INHERITED(ProjectionSpec)
public:
  int   pool_size;              // how many units to interconnect together in a pool -- starts at offset 0 from current index and connects up to pool_size, skipping self if !self_con
  int   stride;                 // how many units to skip over for every pool connection step -- if stride == pool_size then pools are isolated from each other, whereas stride < pool_size causes the "edges" of each pool to overlap with others

  void Connect_impl(Projection* prjn, bool make_cons) override;
  // Connection function for full connectivity
  
  TA_SIMPLE_BASEFUNS(ConPoolPrjnSpec);
private:
  void Initialize();
  void Destroy()     { };
};

#endif // ConPoolPrjnSpec_h

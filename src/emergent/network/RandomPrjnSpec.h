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

#ifndef RandomPrjnSpec_h
#define RandomPrjnSpec_h 1

// parent includes:
#include <ProjectionSpec>

// member includes:
#include <RndSeed>

// declare all other types mentioned but not required to include:

eTypeDef_Of(RandomPrjnSpec);

class E_API RandomPrjnSpec : public ProjectionSpec {
  // Connects all units with probability p_con -- note it ALWAYS uses the same seed, because of the two-pass nature of the connection process -- you can update rndm_seed prior to connecting to get different patterns of connectivity
INHERITED(ProjectionSpec)
public:
  float		p_con;		// overall probability of connection
  bool		sym_self;	// if a self projection, make it symmetric (senders = receivers) otherwise it is not
  RndSeed	rndm_seed;	// random seed -- call NewSeed() to get a new random connectivity pattern

  void Connect_impl(Projection* prjn, bool make_cons) override;

  TA_SIMPLE_BASEFUNS(RandomPrjnSpec);
protected:
  void UpdateAfterEdit_impl() override;
private:
  void	Initialize();
  void 	Destroy()		{ };
};

#endif // RandomPrjnSpec_h

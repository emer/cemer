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
  // Connects all units with probability p_con.
INHERITED(ProjectionSpec)
public:
  float		p_con;		// overall probability of connection
  bool		sym_self;	// if a self projection, make it symmetric (senders = receivers) otherwise it is not
  bool		same_seed;	// use the same random seed each time (same connect pattern)
  RndSeed	rndm_seed;	// #HIDDEN random seed

  void Connect_impl(Projection* prjn) override;

  TA_SIMPLE_BASEFUNS(RandomPrjnSpec);
protected:
  void UpdateAfterEdit_impl() override;
private:
  void	Initialize();
  void 	Destroy()		{ };
};

#endif // RandomPrjnSpec_h

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

#ifndef RndGpOneToOnePrjnSpec_h
#define RndGpOneToOnePrjnSpec_h 1

// parent includes:
#include <GpOneToOnePrjnSpec>

// member includes:
#include <RndSeed>

// declare all other types mentioned but not required to include:

eTypeDef_Of(RndGpOneToOnePrjnSpec);

class E_API RndGpOneToOnePrjnSpec : public GpOneToOnePrjnSpec {
  // uniform random connectivity between one-to-one groups -- only 'permute' style random connectivity is supported (same number of connections across recv units)
INHERITED(GpOneToOnePrjnSpec)
public:
  float		p_con;		// overall probability of connection
  bool		same_seed;	// use the same random seed each time (same connect pattern)
  RndSeed	rndm_seed;	// #HIDDEN random seed

  void	Connect_impl(Projection* prjn) override;

  TA_SIMPLE_BASEFUNS(RndGpOneToOnePrjnSpec);
protected:
  void UpdateAfterEdit_impl() override;
private:
  void	Initialize();
  void 	Destroy()		{ };
};

#endif // RndGpOneToOnePrjnSpec_h

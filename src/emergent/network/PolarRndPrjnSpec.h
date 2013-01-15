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

#ifndef PolarRndPrjnSpec_h
#define PolarRndPrjnSpec_h 1

// parent includes:
#include <ProjectionSpec>
#include <RandomSpec>
#include <RndSeed>

// member includes:

// declare all other types mentioned but not required to include:

TypeDef_Of(PolarRndPrjnSpec);

class EMERGENT_API PolarRndPrjnSpec : public ProjectionSpec {
  // random connectivity defined as a function of distance and angle
INHERITED(ProjectionSpec)
public:
  enum UnitDistType {	// how to compute the distance between two units
    XY_DIST,		// X-Y axis distance between units
    XY_DIST_CENTER, 	// centered distance (layers centered over each other)
    XY_DIST_NORM,	// normalized range (0-1 for each layer) distance
    XY_DIST_CENTER_NORM  // normalized and centered (-1:1 for each layer) distance
  };

  float		p_con;		// overall probability of connection (number of samples)
  RandomSpec 	rnd_dist;	// prob density of connectivity as a fctn of distance
  RandomSpec	rnd_angle;	// prob density of connectivity as a fctn of angle (1 = 2pi)
  UnitDistType	dist_type; 	// type of distance function to use
  bool		wrap;		// wrap around layer coordinates (else clip at ends)
  int	       	max_retries;	// maximum number of times attempt to con same sender allowed
  bool		same_seed;	// use the same random seed each time (same connect pattern)
  RndSeed	rndm_seed;	// #HIDDEN random seed

  override void	Connect_impl(Projection* prjn);
  override void	C_Init_Weights(Projection* prjn, RecvCons* cg, Unit* ru);
  // uses weight values equal to the distance probability

  static float	UnitDist(UnitDistType typ, Projection* prjn,
			 const taVector2i& ru, const taVector2i& su);
  // computes the distance between two units according to distance type
  static Unit*	GetUnitFmOff(UnitDistType typ, bool wrap, Projection* prjn,
			     const taVector2i& ru, const taVector2f& su_off);
  // gets unit from real-valued offset scaled according to distance type

  virtual float	GetDistProb(Projection* prjn, Unit* ru, Unit* su);
  // compute the probability for connecting two units as a fctn of distance

  TA_SIMPLE_BASEFUNS(PolarRndPrjnSpec);
protected:
  override void UpdateAfterEdit_impl();
private:
  void	Initialize();
  void	Destroy()	{ };
};

#endif // PolarRndPrjnSpec_h

// Copyright 2017, Regents of the University of Colorado,
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

#ifndef AllProjectionSpecs_cpp_h
#define AllProjectionSpecs_cpp_h 1

// add all projection specs to this file!

// parent includes:
#include <ProjectionSpec_cpp>

#include <State_cpp>

#include <AllProjectionSpecs_mbrs>

#include <State_cpp>


class FullPrjnSpec_cpp : public ProjectionSpec_cpp {
  // Full connectivity between layers
INHERITED(ProjectionSpec)
public:

#include <FullPrjnSpec>

  FullPrjnSpec_cpp() { Initialize_core(); }
};


class OneToOnePrjnSpec_cpp : public ProjectionSpec_cpp {
  // one-to-one connectivity (1st unit to 1st unit, etc)
INHERITED(ProjectionSpec)
public:

#include <OneToOnePrjnSpec>

  OneToOnePrjnSpec_cpp() { Initialize_core(); }
};


class GpOneToOnePrjnSpec_cpp : public OneToOnePrjnSpec_cpp {
  // unit_group based one-to-one connectivity, with full connectivity within unit groups (all in 1st group to all in 1st group, etc) -- if one layer has same number of units as the other does unit groups, then each unit connects to entire unit group
INHERITED(OneToOnePrjnSpec)
public:

#include <GpOneToOnePrjnSpec>

  GpOneToOnePrjnSpec_cpp() { Initialize_core(); }
};


class MarkerGpOneToOnePrjnSpec_cpp : public OneToOnePrjnSpec_cpp {
  // unit_group based one-to-one connectivity for marking a projection -- all of the recv units receive from just the first unit in the sending unit group, thus providing a marker for where to receive information from the sending group
INHERITED(OneToOnePrjnSpec)
public:

#include <MarkerGpOneToOnePrjnSpec>  
  
  MarkerGpOneToOnePrjnSpec_cpp() { Initialize_core(); }
};


class GpMapConvergePrjnSpec_cpp : public ProjectionSpec_cpp {
  // generates a converging map of the units within a sending layer that has unit groups into a receiving layer that has the same geometry as one of the unit groups -- each recv unit receives from the corresponding unit in all of the sending unit groups
INHERITED(ProjectionSpec)
public:

#include <GpMapConvergePrjnSpec>

  GpMapConvergePrjnSpec_cpp() { Initialize_core(); }
};


class GpMapDivergePrjnSpec_cpp : public ProjectionSpec_cpp {
  // projects from a layer without unit groups into a receiving layer with unit groups and that has the same unit geometry in each of its unit groups as the sending layer -- each unit projects to the corresponding unit in all of the receiving unit groups
INHERITED(ProjectionSpec)
public:
  
#include <GpMapDivergePrjnSpec>

  GpMapDivergePrjnSpec_cpp()      { Initialize_core(); }
};


class RandomPrjnSpec_cpp : public ProjectionSpec_cpp {
  // Connects all units with probability p_con -- note it ALWAYS uses the same seed, because of the two-pass nature of the connection process -- you can update rndm_seed prior to connecting to get different patterns of connectivity
INHERITED(ProjectionSpec)
public:

#include <RandomPrjnSpec>
  
  RandomPrjnSpec_cpp() { Initialize_core(); }
};


class UniformRndPrjnSpec_cpp : public ProjectionSpec_cpp {
  // Uniform random connectivity between layers -- only 'permute' stye randomness is supported, creates same number of connections per unit
INHERITED(ProjectionSpec)
public:

#include <UniformRndPrjnSpec>
  
  UniformRndPrjnSpec_cpp() { Initialize_core(); }
};


class PolarRndPrjnSpec_cpp : public ProjectionSpec_cpp {
  // random connectivity defined as a function of distance and angle
INHERITED(ProjectionSpec)
public:

#include <PolarRndPrjnSpec>
  
  PolarRndPrjnSpec_cpp() { Initialize_core(); }
};


class TesselPrjnSpec_cpp : public ProjectionSpec_cpp {
  // arbitrary tesselations (repeating patterns) of connectivity -- sweeps over receiving units and connects with sending units based on projection of recv unit position into sending layer, plus sending offsets that specify the connectivity pattern
INHERITED(ProjectionSpec)
public:

#include <TesselPrjnSpec>
  
  TesselPrjnSpec_cpp() { Initialize_core(); }
};


class GpTesselPrjnSpec_cpp : public ProjectionSpec_cpp {
  // specifies tesselated patterns of groups to connect with (both recv and send layers must have unit groups), optionally with random connectivity within each group (also very useful for full connectivity -- has optimized support for that) -- only 'permute' style randomness is supported, producing same number of recv connections per unit
INHERITED(ProjectionSpec)
public:

#include <GpTesselPrjnSpec>
  
  GpTesselPrjnSpec_cpp() { Initialize_core(); }
};


class TiledGpRFPrjnSpec_cpp : public ProjectionSpec_cpp {
  // Tiled receptive field projection spec for entirely group-to-group connections: connects entire receiving layer unit groups with overlapping tiled regions of sending layer groups -- if init_wts is on, gaussian or sigmoid topographic weights are initialized
INHERITED(ProjectionSpec)
public:

#include <TiledGpRFPrjnSpec>  

  TiledGpRFPrjnSpec_cpp() { Initialize_core(); }
};



#endif // AllProjectionSpecs_cpp_h

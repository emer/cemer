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


class E_API FullPrjnSpec_cpp : public ProjectionSpec_cpp {
  // Full connectivity between layers
INHERITED(ProjectionSpec)
public:

#include <FullPrjnSpec>

  FullPrjnSpec_cpp() { Initialize_core(); }
};


///////////////////////////////////////////////////////////
//      OneToOne

class E_API OneToOnePrjnSpec_cpp : public ProjectionSpec_cpp {
  // one-to-one connectivity (1st unit to 1st unit, etc)
INHERITED(ProjectionSpec)
public:

#include <OneToOnePrjnSpec>

  OneToOnePrjnSpec_cpp() { Initialize_core(); }
};


class E_API GpOneToOnePrjnSpec_cpp : public OneToOnePrjnSpec_cpp {
  // unit_group based one-to-one connectivity, with full connectivity within unit groups (all in 1st group to all in 1st group, etc) -- if one layer has same number of units as the other does unit groups, then each unit connects to entire unit group
INHERITED(OneToOnePrjnSpec)
public:

#include <GpOneToOnePrjnSpec>

  GpOneToOnePrjnSpec_cpp() { Initialize_core(); }
};


class E_API MarkerGpOneToOnePrjnSpec_cpp : public OneToOnePrjnSpec_cpp {
  // unit_group based one-to-one connectivity for marking a projection -- all of the recv units receive from just the first unit in the sending unit group, thus providing a marker for where to receive information from the sending group
INHERITED(OneToOnePrjnSpec)
public:

#include <MarkerGpOneToOnePrjnSpec>  
  
  MarkerGpOneToOnePrjnSpec_cpp() { Initialize_core(); }
};


class E_API GpMapConvergePrjnSpec_cpp : public ProjectionSpec_cpp {
  // generates a converging map of the units within a sending layer that has unit groups into a receiving layer that has the same geometry as one of the unit groups -- each recv unit receives from the corresponding unit in all of the sending unit groups
INHERITED(ProjectionSpec)
public:

#include <GpMapConvergePrjnSpec>

  GpMapConvergePrjnSpec_cpp() { Initialize_core(); }
};


class E_API GpMapDivergePrjnSpec_cpp : public ProjectionSpec_cpp {
  // projects from a layer without unit groups into a receiving layer with unit groups and that has the same unit geometry in each of its unit groups as the sending layer -- each unit projects to the corresponding unit in all of the receiving unit groups
INHERITED(ProjectionSpec)
public:
  
#include <GpMapDivergePrjnSpec>

  GpMapDivergePrjnSpec_cpp()      { Initialize_core(); }
};


///////////////////////////////////////////////////////////
//      Random

class E_API RandomPrjnSpec_cpp : public ProjectionSpec_cpp {
  // Connects all units with probability p_con -- note it ALWAYS uses the same seed, because of the two-pass nature of the connection process -- you can update rndm_seed prior to connecting to get different patterns of connectivity
INHERITED(ProjectionSpec)
public:

#include <RandomPrjnSpec>
  
  RandomPrjnSpec_cpp() { Initialize_core(); }
};


class E_API UniformRndPrjnSpec_cpp : public ProjectionSpec_cpp {
  // Uniform random connectivity between layers -- only 'permute' stye randomness is supported, creates same number of connections per unit
INHERITED(ProjectionSpec)
public:

#include <UniformRndPrjnSpec>
  
  UniformRndPrjnSpec_cpp() { Initialize_core(); }
};


class E_API PolarRndPrjnSpec_cpp : public ProjectionSpec_cpp {
  // random connectivity defined as a function of distance and angle
INHERITED(ProjectionSpec)
public:

#include <PolarRndPrjnSpec>
  
  PolarRndPrjnSpec_cpp() { Initialize_core(); }
};


class E_API SymmetricPrjnSpec_cpp : public ProjectionSpec_cpp {
  // connect with the same symmetric pattern that exists in a projection going the other direction -- especially useful for random projections but works for any type of projection -- runs in a second pass so no constraints on layer ordering etc -- somewhat slow but not too bad
INHERITED(ProjectionSpec)
public:
  
#include <SymmetricPrjnSpec>
  
  SymmetricPrjnSpec_cpp() { Initialize_core(); }
};


///////////////////////////////////////////////////////////
//      Tessel 

class E_API TesselPrjnSpec_cpp : public ProjectionSpec_cpp {
  // arbitrary tesselations (repeating patterns) of connectivity -- sweeps over receiving units and connects with sending units based on projection of recv unit position into sending layer, plus sending offsets that specify the connectivity pattern
INHERITED(ProjectionSpec)
public:

#include <TesselPrjnSpec>
  
  TesselPrjnSpec_cpp() { Initialize_core(); }
  ~TesselPrjnSpec_cpp() { FreeSendOffs(); }
};


class E_API GpTesselPrjnSpec_cpp : public ProjectionSpec_cpp {
  // specifies tesselated patterns of groups to connect with (both recv and send layers must have unit groups), optionally with random connectivity within each group (also very useful for full connectivity -- has optimized support for that) -- only 'permute' style randomness is supported, producing same number of recv connections per unit
INHERITED(ProjectionSpec)
public:

#include <GpTesselPrjnSpec>
  
  GpTesselPrjnSpec_cpp() { Initialize_core(); }
  ~GpTesselPrjnSpec_cpp() { FreeSendOffs(); }
};


///////////////////////////////////////////////////////////
//      Tiled..

class E_API TiledGpRFPrjnSpec_cpp : public ProjectionSpec_cpp {
  // Tiled receptive field projection spec for entirely group-to-group connections: connects entire receiving layer unit groups with overlapping tiled regions of sending layer groups -- if init_wts is on, gaussian or sigmoid topographic weights are initialized
INHERITED(ProjectionSpec)
public:

#include <TiledGpRFPrjnSpec>  

  TiledGpRFPrjnSpec_cpp() { Initialize_core(); }
};


class E_API TiledGpRFOneToOnePrjnSpec_cpp : public TiledGpRFPrjnSpec_cpp {
  // TiledGpRFPrjnSpec connectvity with one-to-one connections for units with the same index within a unit group -- useful for establishing connections among layers with the same unit-group structure (see also TiledGpRFOneToOneWtsPrjnSpec for a softer version where only weights are set)
INHERITED(TiledGpRFPrjnSpec)
public:

#include <TiledGpRFOneToOnePrjnSpec>
  
  TiledGpRFOneToOnePrjnSpec_cpp() { Initialize_core(); }
};


class E_API TiledGpRFOneToOneWtsPrjnSpec_cpp : public TiledGpRFPrjnSpec_cpp {
  // TiledGpRFPrjnSpec connectvity with initial weights (when init_wts is set) that have differential weights for units with the same index within a unit group vs. differential weights -- useful for establishing connections among layers with the same unit-group structure (see also TiledGpRFOnetToOnePrjnSpec for harder version where connections are only made among units with same index within group)
INHERITED(TiledGpRFPrjnSpec)
public:

#include <TiledGpRFOneToOneWtsPrjnSpec>

  TiledGpRFOneToOneWtsPrjnSpec_cpp()  { Initialize_core(); }
};


class E_API TiledSubGpRFPrjnSpec_cpp : public ProjectionSpec_cpp {
  // Tiled receptive field projection spec for topographic tiled group-to-group connections, with a sub-tiling of unit groups within the larger receptive field tiling, to divide the larger problem into more managable subsets: connects entire receiving layer unit groups with overlapping tiled regions of sending layer groups -- if init_wts is on, gaussian topographic weights are initialized
INHERITED(ProjectionSpec)
public:

#include <TiledSubGpRFPrjnSpec>
  
  TiledSubGpRFPrjnSpec_cpp() { Initialize_core(); }
};


class E_API TiledRFPrjnSpec_cpp : public ProjectionSpec_cpp {
  // Tiled receptive field projection spec: connects entire receiving layer unit groups with overlapping tiled regions of sending layers
INHERITED(ProjectionSpec)
public:

#include <TiledRFPrjnSpec>

  TiledRFPrjnSpec_cpp() { Initialize_core(); }
};


class E_API TiledNovlpPrjnSpec_cpp : public ProjectionSpec_cpp {
  // Tiled non-overlapping projection spec: connects entire receiving layer unit groups with non-overlapping tiled regions of sending units
INHERITED(ProjectionSpec)
public:

#include <TiledNovlpPrjnSpec>

  TiledNovlpPrjnSpec_cpp()  { Initialize_core(); }
};


class E_API TiledGpMapConvergePrjnSpec_cpp : public ProjectionSpec_cpp {
  // generates a converging map of the units within a sending layer with unit groups, using tiled overlapping receptive fields within each unit group -- each recv unit receives from the corresponding unit in all of the sending unit groups, with the recv units organized into unit groups that each recv from one tiled subset of sending units within all the sending unit groups -- there must be the same number of recv unit groups as tiled subsets within the sending unit groups
INHERITED(ProjectionSpec)
public:

#include <TiledGpMapConvergePrjnSpec>

  TiledGpMapConvergePrjnSpec_cpp()  { Initialize_core(); }
};


///////////////////////////////////////////////////////////
//      Gaussian, Gradient..

class E_API GaussRFPrjnSpec_cpp : public ProjectionSpec_cpp {
  // a simple receptive-field (RF) projection spec with gaussian weight values over a receptive-field window onto the sending layer that moves as a function of the receiving unit's position (like TesselPrjnSpec and other RF prjn specs, but does NOT use unit groups) -- useful for reducing larger layers to smaller ones for example
INHERITED(ProjectionSpec)
public:

#include <GaussRFPrjnSpec>

  GaussRFPrjnSpec_cpp() { Initialize_core(); }
};


class E_API GradientWtsPrjnSpec_cpp : public FullPrjnSpec_cpp {
  // full connectivity with a gradient of weight strengths (requires init_wts = true, otherwise is just like Full Prjn), where weights are strongest from sending units in same relative location as the receiving unit, and fall off from there (either linearly or as a Guassian) -- if recv layer has unit groups, then it is the unit group position that counts, and all units within the recv group have the same connectivity (can override with use_gps flag)
INHERITED(FullPrjnSpec)
public:

#include <GradientWtsPrjnSpec>
  
  GradientWtsPrjnSpec_cpp() { Initialize_core(); }
};


///////////////////////////////////////////////////////////
//      PFC / BG


class E_API PFCPrjnSpec_cpp : public ProjectionSpec_cpp {
  // projections involving a PFC layer with unit groups organized by rows into alternating transient and maintaining units, with the first two rows described as INPUT, and the last two rows as OUTPUT
INHERITED(ProjectionSpec)
public:

#include <PFCPrjnSpec>  
  
  PFCPrjnSpec_cpp() { Initialize_core(); }
};


class E_API BgPfcPrjnSpec_cpp : public ProjectionSpec_cpp {
  // for connecting BG and PFC layers, where there are separate PFC layers that interconnect with a single BG layer (Matrix, GPi, etc), allowing competition within the BG -- has a customizable data table of the different PFC layers that all map to the same BG layer -- also supports connections from a Patch layer with same name root as PFC
INHERITED(ProjectionSpec)
public:

#include <BgPfcPrjnSpec>

  BgPfcPrjnSpec_cpp() { Initialize_core(); }
  ~BgPfcPrjnSpec_cpp() { FreePfcLayers(); }
};


#endif // AllProjectionSpecs_cpp_h

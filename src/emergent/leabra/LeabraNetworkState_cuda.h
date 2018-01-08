// Copyright 2017-2017, Regents of the University of Colorado,
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

#ifndef LeabraNetworkState_cuda_h
#define LeabraNetworkState_cuda_h 1

// parent includes:
#include <NetworkState_cuda> // replace with actual parent

#include <LeabraNetwork_mbrs>

#include <State_cuda>

// member includes:

// declare all other types mentioned but not required to include:

class STATE_CLASS(LeabraLayerState); //
class STATE_CLASS(LeabraPrjnState); //
class STATE_CLASS(LeabraUnGpState); //
class STATE_CLASS(LeabraUnitState); //
class STATE_CLASS(LeabraConState); // 
class STATE_CLASS(LeabraLayerSpec); //
class STATE_CLASS(LeabraUnitSpec); //
class STATE_CLASS(LeabraConSpec); //


class LeabraNetworkState_cuda : public NetworkState_cuda {
  // #STEM_BASE ##CAT_Leabra network that uses the Leabra algorithms and objects
INHERITED(NetworkState)
public:

#include <LeabraNetwork_core>
#include <LeabraNetworkState_core>

  LeabraNetworkState_cuda() { Initialize_core(); }
};

#endif // LeabraNetworkState_cuda_h

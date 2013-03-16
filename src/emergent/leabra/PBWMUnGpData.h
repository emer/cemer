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

#ifndef PBWMUnGpData_h
#define PBWMUnGpData_h 1

// parent includes:
#include <LeabraUnGpData>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(PBWMUnGpData);

class E_API PBWMUnGpData : public LeabraUnGpData {
  // PBWM version of data to maintain for independent unit groups of competing units within a single layer -- contains extra information for PBWM state
INHERITED(LeabraUnGpData)
public:
  bool		go_fired_now;	// #CAT_Activation has Go fired on this cycle?  only true for one cycle
  bool		go_fired_trial;	// #CAT_Activation has Go fired on this trial -- false until it fires
  int		go_cycle;	// #CAT_Activation cycle on which Go fired, if it did on this trial (-1 if no Go)
  int		mnt_count;	// #CAT_Activation current counter of number of trials in maint or empty state -- 0 = gating just happened -- will only be true on trial when go_fired_trial is true -- 1+ = maint for a trial or a more (increments at start of each trial, 1 = trial right after gating, etc) -- -1- = empty for a trial or more (also increments at start of each trial)
  int		prv_mnt_count;	// #CAT_Activation previous counter of number of trials in maint or empty state, just prior to last gating event (only updated at time of gating) -- enables determination of whether a current Go signal was to a maintaining stripe (go_fired_trial && prv_mnt_count > 0) or an empty stripe (otherwise)
  float		nogo_inhib;	// #CAT_Activation amount of nogo inhibition for this stripe -- only valid in Matrix Go layers
  float		refract_inhib;	// #CAT_Activation amount of refractory inhibition for this stripe -- only valid in Matrix Go layers
  float		pvr_inhib;	// #CAT_Activation amount of pvr activity inhibition for this stripe -- only valid in Matrix Go layers

  override void	Init_State();

  virtual void	CopyPBWMData(const PBWMUnGpData& cp);
  // copy just the pbwm data

  void	Copy_(const PBWMUnGpData& cp);
  TA_BASEFUNS(PBWMUnGpData);
private:
  void	Initialize();
  void	Destroy()		{ };
};

#endif // PBWMUnGpData_h

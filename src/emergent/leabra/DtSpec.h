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

#ifndef DtSpec_h
#define DtSpec_h 1

// parent includes:
#include "network_def.h"
#include <SpecMemberBase>
#include <BaseSpec>

// member includes:

// declare all other types mentioned but not required to include:

// NOTE: DtSpec is actually not that useful in practice -- better to just add
// equivalent logic and _time or _rate values directly into spec of interest
// definitely good to show both reps!

eTypeDef_Of(DtSpec);

class E_API DtSpec : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Math time constant specification -- shows both multiplier and time constant (inverse) value 
INHERITED(SpecMemberBase)
public:
  bool		set_time;	// if true, time constant is entered in terms of time, otherwise, in terms of rate
  float		rate;		// #CONDSHOW_OFF_set_time rate factor = 1/time -- used for multiplicative update equations
  float		time;		// #CONDSHOW_ON_set_time temporal duration for the time constant -- how many msec or sec long (typically to reach a 1/e level with exponential dynamics) = 1/rate

  String       GetTypeDecoKey() const CPP11_OVERRIDE { return "UnitSpec"; }

  TA_SIMPLE_BASEFUNS(DtSpec);
protected:
  SPEC_DEFAULTS;
  void	UpdateAfterEdit_impl() { if(set_time) rate = 1.0f / time; else time = 1.0f / rate; }
private:
  void	Initialize()    { set_time = false; rate = 1.0f; time = 1.0f; }
  void	Destroy()	{ };
  void	Defaults_init() { }
};


#endif // DtSpec_h

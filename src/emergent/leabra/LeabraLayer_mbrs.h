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

// this contains all full classes that appear as members of _core specs
// it must be included directly in LeabraLayer* _cpp.h, _cuda.h
// the STATE_CLASS macro will define a _cpp _cuda or plain (no suffix) version

// this pragma ensures that maketa properly grabs this type information even though
// this file is included in the other files -- we get ta info for main and _cpp, not cuda

#ifdef __MAKETA__
#pragma maketa_file_is_target LeabraLayer
#pragma maketa_file_is_target LeabraLayerState
#endif

class STATE_CLASS(LeabraMarginVals) : public STATE_CLASS(taOBase) {
  // ##INLINE ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra holds values for margin activation levels
INHERITED(taOBase)
public:
  float         low_thr;   // #CAT_Activation #SAVE_WTS low threshold for marginal activation, in terms of v_m_eq -- adapts so that roughly acts_p_avg units on average are above this low threshold
  float         med_thr;   // #CAT_Activation #SAVE_WTS medium threshold for marginal activation, in terms of v_m_eq -- adapts so that marginal units are roughly split in half by this threshold
  float         hi_thr;   // #CAT_Activation #SAVE_WTS high threshold for marginal activation, in terms of v_m_eq -- adapts so that roughly margin.pct_marg * acts_p_avg units on average are between low and high threshold
  float         low_avg; // #CAT_Activation #SAVE_WTS running-average computed proportion of units above the low_thr
  float         med_avg; // #CAT_Activation #SAVE_WTS running-average computed proportion of units between the low and medium thresholds
  float         hi_avg; // #CAT_Activation #SAVE_WTS running-average computed proportion of units above the hi_thr

  INLINE void   InitVals() {
    low_thr = 0.495f;    med_thr = 0.503f;    hi_thr = 0.508f;    low_avg = .15f;
    med_avg = 0.0375f;   hi_avg = 0.075f;
  }
  // initialize the inhibition values 

  STATE_DECO_KEY("Layer");
  STATE_TA_STD_CODE(LeabraMarginVals);
private:
  void	Initialize()    { InitVals(); }
};


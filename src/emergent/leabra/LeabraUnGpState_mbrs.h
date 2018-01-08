// Copyright 2017-20188 Regents of the University of Colorado,
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
// it must be included directly in LeabraUnGpState.h, _cpp.h, _cuda.h
// the STATE_CLASS macro will define a _cpp _cuda or plain (no suffix) version

// this pragma ensures that maketa properly grabs this type information even though
// this file is included in the other files -- we get ta info for main and _cpp, not cuda

#ifdef __MAKETA__
#pragma maketa_file_is_target LeabraUnGpState
#pragma maketa_file_is_target LeabraLayer
#endif


class STATE_CLASS(LeabraTwoDVals) : public STATE_CLASS(taOBase) {
  // ##INLINE ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra holds two-dimensional decoding / encoding values for spatial representational layers (TwoDValLayerSpec)
INHERITED(taOBase)
public:
  float ext_x;         // external inputs
  float ext_y;         // external inputs
  float targ_x;        // target values
  float targ_y;        // target values
  float act_x;         // current activation
  float act_y;         // current activation
  float act_m_x;       // minus phase activations
  float act_m_y;       // minus phase activations
  float act_p_x;       // plus phase activations
  float act_p_y;       // plus phase activations
  float act_dif_x;     // difference between plus and minus phase activations
  float act_dif_y;     // difference between plus and minus phase activations
  float act_m2_x;      // second minus phase activations
  float act_m2_y;      // second minus phase activations
  float act_p2_x;      // second plus phase activations
  float act_p2_y;      // second plus phase activations
  float act_dif2_x;    // difference between second plus and minus phase activations
  float act_dif2_y;    // difference between second plus and minus phase activations
  float err_x;         // error from target: targ - act_m
  float err_y;         // error from target: targ - act_m
  float sqerr_x;       // squared error from target: (targ - act_m)^2
  float sqerr_y;       // squared error from target: (targ - act_m)^2

  INLINE void   InitVals() {
    ext_x = ext_y = targ_x = targ_y = act_x = act_y = 0.0f;
    act_m_x = act_m_y =  act_p_x = act_p_y =  act_dif_x = act_dif_y = 0.0f;
    act_m2_x = act_m2_y = act_p2_x = act_p2_y = act_dif2_x = act_dif2_y = 0.0f;
    err_x = err_y = sqerr_x = sqerr_y = 0.0f;
  }
  // initialize the values

  STATE_DECO_KEY("Layer");
  STATE_TA_STD_CODE(LeabraTwoDVals);
private:
  void	Initialize()    { InitVals(); }
};


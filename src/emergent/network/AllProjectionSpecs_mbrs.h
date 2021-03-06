// Copyright 2017-2018, Regents of the University of Colorado,
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

// this must be included in a defined State_core/main/cuda context with appropriate
// #ifndef multiple-include protection AT THAT LEVEL not here..

#ifdef __MAKETA__
#pragma maketa_file_is_target AllProjectionSpecs
#endif

class STATE_CLASS(TessEl) : public STATE_CLASS(taOBase) {
  // ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Spec one element of a tesselation specification
INHERITED(taOBase)
public:
  TAVECTOR2I    send_off;       // offset from current receiving unit
  float         wt_val;         // value to assign to weight

  STATE_DECO_KEY("ProjectionSpec");
  STATE_TA_STD_CODE(TessEl);
private:
  INLINE void   Initialize() { wt_val = 1; }
};


class STATE_CLASS(GpTessEl) : public STATE_CLASS(taOBase) {
  // ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Spec one element of a tesselation specification for groups
INHERITED(taOBase)
public:
  TAVECTOR2I    send_gp_off;    // offset of group from current receiving group
  float         p_con;          // proportion connectivity from this group -- negative value means make symmetric cons within the same layer -- also optimized for full connectivity when = 1

  STATE_DECO_KEY("ProjectionSpec");
  STATE_TA_STD_CODE(GpTessEl);
private:
  void  Initialize() { p_con = 1.0f; }
};


class STATE_CLASS(GaussInitWtsSpec) : public STATE_CLASS(taOBase) {
  // #STEM_BASE ##INLINE ##CAT_Projection parameters for initializing gaussian projection weights
INHERITED(taOBase)
public:
  bool          on;             // initialize gaussian weights at this level scale
  float         sigma;          // #CONDSHOW_ON_on gaussian sigma (width) in normalized units where entire distance across relevant dimension is 1.0 -- typical useful values range from .3 to 1.5
  bool          wrap_wts;       // #CONDSHOW_ON_on wrap the gaussian around on other sides of the receptive field -- this removes strict topography but ensures a more uniform distribution of weight values so edge units don't have weaker overall weights -- often useful to use for unit-group level but not for full level
  float         ctr_mv;         // #CONDSHOW_ON_on #DEF_0.8;1 how much the center of the gaussian moves with respect to the position of the receiving unit within its unit group -- 1.0 = centers span the entire range of the receptive field -- typically want to use 1.0 for wrap_wts = true, and 0.8 for wrap_wts = false

  STATE_DECO_KEY("ProjectionSpec");
  STATE_TA_STD_CODE(GaussInitWtsSpec);
private:
  void  Initialize() {  on = true;  sigma = 0.6f;  wrap_wts = false;  ctr_mv = 0.8f; }
};


class STATE_CLASS(SigmoidInitWtsSpec) : public STATE_CLASS(taOBase) {
  // #STEM_BASE ##INLINE ##CAT_Projection parameters for initializing sigmoid projection weights
INHERITED(taOBase)
public:
  bool          on;             // initialize sigmoid weights at this level scale
  float         gain;           // #CONDSHOW_ON_on sigmoid gain (contrast) in normalized units where entire distance across relevant dimension is 1.0 -- typical useful values range from 0.01..0.1
  float         ctr_mv;         // #CONDSHOW_ON_on #DEF_0.5;1 how much the center of the sigmoid moves with respect to the position of the receiving unit within its unit group -- 1.0 = centers span the entire range of the receptive field -- typically 0.5 to 0.8 is best

  STATE_DECO_KEY("ProjectionSpec");
  STATE_TA_STD_CODE(SigmoidInitWtsSpec);
private:
  void  Initialize()  {  on = true;  gain = 0.02f;  ctr_mv = 0.5f; }
};



class STATE_CLASS(BgPfcPrjnEl) : public STATE_CLASS(taOBase) {
  // ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Spec one element of a BgPfcPrjnSpec projection spec
INHERITED(taOBase)
public:
  static const int LAY_NAME_MAX_LEN=256; // #IGNORE max length of layer name -- update from Layer_core.h if ever need to change
 
  char          pfc_name[LAY_NAME_MAX_LEN]; // name of the PFC layer -- layer must uniquely contain this name -- case sensitive maching is used
  TAVECTOR2I    size; // size of this pfc layer in terms of unit groups (stripes) -- only needed if the PFC layers are NOT all the same size -- a -1 indicates that all pfc layers are the same size
  TAVECTOR2I    start; // starting coordinates of the unit group of stripes for this pfc within the bg layer -- a -1 for x indicates that PFC layers are organized horizontally across, and position computed automatically from sizes

  INLINE void   SetPfcName(const char* pfc_nm) {
    strncpy(pfc_name, pfc_nm, LAY_NAME_MAX_LEN-1);
    pfc_name[LAY_NAME_MAX_LEN-1] = '\0'; // C is so lame!
  }
  // #CAT_State set pfc name to given name
  INLINE bool   PfcNameIs(const char* pfc_nm) const {
    return (strncmp(pfc_name, pfc_nm, LAY_NAME_MAX_LEN) == 0);
  }
  // #CAT_State return true if that is the name of the pfc
  INLINE bool   PfcNameContains(const char* pfc_nm) const {
    return (strstr(pfc_name, pfc_nm) != NULL);
  }
  // #CAT_State return true if the pfc name contains the given string

  STATE_DECO_KEY("ProjectionSpec");
  STATE_TA_STD_CODE(BgPfcPrjnEl);
private:
  INLINE void   Initialize() { size = -1; start.x = -1; start.y = 0; }
};



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

#ifndef CtSRAvgVals_h
#define CtSRAvgVals_h 1

// parent includes:
#include "network_def.h"
#include <taOBase>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(CtSRAvgVals);

class E_API CtSRAvgVals : public taOBase {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra sender-receiver average values accumulated during Ct processing -- e.g., overall vals used for normalization
INHERITED(taOBase)
public:
  enum 	SRAvgState {		// what state are we in for computing sravg
    NO_SRAVG,			// don't compute sravg right now
    SRAVG_M,			// update ONLY medium time-scale (minus phase) sravg values
    SRAVG_S,			// update ONLY short time-scale (plus phase) sravg values
    SRAVG_SM,			// update BOTH short and medium time-scale sravg values -- this is the default for plus phase
  };

  SRAvgState	state;		// #GUI_READ_ONLY #SHOW what state of sravg computation are we currently in?  This is usually computed from the CtSRAvgSpec values on the network, but can be overridden and determined programmatically instead -- set the network CtSRAvgSpec manual flag on to do so..
  float		s_sum;	// #READ_ONLY #SHOW sum of sravg_s (short time-scale, plus phase) weightings (count of number of times sravg has been computed) -- used for normalizing the overall average
  float		m_sum;	// #READ_ONLY #SHOW sum of sravg_m (medium time-scale, trial) weightings (count of number of times sravg has been computed) -- used for normalizing the overall average

  float		s_nrm;	// #READ_ONLY #SHOW normalization term = 1 / sravg_s_sum -- multiply connection-level sravg_s by this value -- only for Compute_dWt_CtLeabraCAL 
  float		m_nrm;	// #READ_ONLY #SHOW normalization term = 1 / sravg_m_sum -- multiply connection-level sravg_m by this value -- only for Compute_dWt_CtLeabraCAL 

  void		InitVals() { state = NO_SRAVG; s_sum = m_sum = 0.0f; s_nrm = m_nrm = 1.0f; }

  String       GetTypeDecoKey() const override { return "Network"; }

  SIMPLE_COPY(CtSRAvgVals);
  TA_BASEFUNS(CtSRAvgVals);
// protected:
//   void UpdateAfterEdit_impl();

private:
  void	Initialize()	{ InitVals(); }
  void 	Destroy()	{ };
};

#endif // CtSRAvgVals_h

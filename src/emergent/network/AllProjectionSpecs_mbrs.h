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

// this must be included in a defined State_core/main/cuda context with appropriate
// #ifndef multiple-include protection AT THAT LEVEL not here..

#pragma maketa_file_is_target AllProjectionSpecs

class STATE_CLASS(TessEl) : public STATE_CLASS(taOBase) {
  // ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Spec one element of a tesselation specification
INHERITED(taOBase)
public:
  TAVECTOR2I	send_off;	// offset from current receiving unit
  float		wt_val;		// value to assign to weight

  STATE_DECO_KEY("ProjectionSpec");
  STATE_TA_STD_CODE(TessEl);
private:
  INLINE void	Initialize() {
    wt_val = 1;
  }
};





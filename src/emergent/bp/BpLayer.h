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

#ifndef BpLayer_h
#define BpLayer_h 1

// parent includes:
#include <Layer>

// member includes:
#include <BpUnitState_cpp>

#include <State_main>

// declare all other types mentioned but not required to include:

eTypeDef_Of(BpLayer);

class E_API BpLayer : public Layer {
  // #STEM_BASE ##CAT_Bp A feedforward backpropagation layer
INHERITED(Layer)
public:

  inline BpUnit* GetUnitIdx(int un_idx) const
  { return (BpUnit*)inherited::GetUnitIdx(un_idx); }
  // #CAT_Access get unit state at given unit index (0..n_units) -- preferred Program interface as no NetworkState arg is required
  inline BpUnit* GetUnitFlatXY(int flat_x, int flat_y) const
  { return (BpUnit*)inherited::GetUnitFlatXY(flat_x, flat_y);  }
  // #CAT_Access get unit state at given flat X,Y coordinates -- preferred Program interface as no NetworkState arg is required
  inline BpUnit* GetUnitGpUnIdx(int gp_idx, int un_idx) const
  { return (BpUnit*)inherited::GetUnitGpUnIdx(gp_idx, un_idx); }
  // #CAT_Access get unit state at given group and unit indexes -- preferred Program interface as no NetworkState arg is required
  inline BpUnit* GetUnitGpXYUnIdx(int gp_x, int gp_y, int un_idx) const
  { return (BpUnit*)inherited::GetUnitGpXYUnIdx(gp_x, gp_y, un_idx); }
  // #CAT_Access get the unit state at given group X,Y coordinate and unit indexes -- preferred Program interface as no NetworkState arg is required
  inline BpUnit* GetUnitGpIdxUnXY(int gp_idx, int un_x, int un_y) const
  { return (BpUnit*)inherited::GetUnitGpIdxUnXY(gp_idx, un_x, un_y); }
  // #CAT_Access get the unit state at given group index and unit X,Y coordinate -- preferred Program interface as no NetworkState arg is required
  inline BpUnit* GetUnitGpUnXY(int gp_x, int gp_y, int un_x, int un_y) const
  { return (BpUnit*)inherited::GetUnitGpUnXY(gp_x, gp_y, un_x, un_y);  }
  // #CAT_Access get the unit state at given group X,Y and unit X,Y coordinates -- preferred Program interface as no NetworkState arg is required
  inline BpUnit*  MostActiveUnit(int& idx) const
  { return (BpUnit*)inherited::MostActiveUnit(idx); }
  // #CAT_Statistic Return the unit with the highest activation (act) value -- index of unit is returned in idx

  TA_BASEFUNS_NOCOPY(BpLayer);
private:
  void	Initialize();
  void 	Destroy()		{ };
};

#endif // BpLayer_h

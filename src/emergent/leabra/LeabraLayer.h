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

#ifndef LeabraLayer_h
#define LeabraLayer_h 1

// parent includes:
#include <Layer>

// member includes:
#include <LeabraLayerSpec>


eTypeDef_Of(LeabraAvgMax);
eTypeDef_Of(LeabraInhibVals);
eTypeDef_Of(LeabraTwoDVals);
eTypeDef_Of(LeabraMarginVals);
// declare all other types mentioned but not required to include:
class LeabraNetwork; // 

eTypeDef_Of(LeabraLayer);

// State update: in general we now avoid forwarding directly to the spec -- creates double-effort
// and requires inclusion of the spec in the state -- but rules of include are that state
// be fully independent of spec -- can include any state in any other state and not worry
// about specs

#include <State_main>

#include <LeabraAvgMax_core>
#include <LeabraLayer_mbrs>


class E_API LeabraLayer : public Layer {
  // #STEM_BASE ##CAT_Leabra #AKA_TwoDValLeabraLayer a Leabra Layer, which defines the primary scope of inhibitory competition among the units and unit groups that it contains
INHERITED(Layer)
public:

  LeabraLayerSpec_SPtr	spec;	// #CAT_Structure the spec for this layer: controls all functions of layer

#include <LeabraLayer_core>
 
  String        minus_output_name;    // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic #VIEW name for the output produced by the network in the minus phase -- for recording in logs as network's response (output_name in plus phase is clamped target value)

  inline LeabraUnit* GetUnitIdx(int un_idx) const
  { return (LeabraUnit*)inherited::GetUnitIdx(un_idx); }
  // #CAT_Access get unit state at given unit index (0..n_units) -- preferred Program interface as no NetworkState arg is required
  inline LeabraUnit* GetUnitFlatXY(int flat_x, int flat_y) const
  { return (LeabraUnit*)inherited::GetUnitFlatXY(flat_x, flat_y);  }
  // #CAT_Access get unit state at given flat X,Y coordinates -- preferred Program interface as no NetworkState arg is required
  inline LeabraUnit* GetUnitGpUnIdx(int gp_idx, int un_idx) const
  { return (LeabraUnit*)inherited::GetUnitGpUnIdx(gp_idx, un_idx); }
  // #CAT_Access get unit state at given group and unit indexes -- preferred Program interface as no NetworkState arg is required
  inline LeabraUnit* GetUnitGpXYUnIdx(int gp_x, int gp_y, int un_idx) const
  { return (LeabraUnit*)inherited::GetUnitGpXYUnIdx(gp_x, gp_y, un_idx); }
  // #CAT_Access get the unit state at given group X,Y coordinate and unit indexes -- preferred Program interface as no NetworkState arg is required
  inline LeabraUnit* GetUnitGpIdxUnXY(int gp_idx, int un_x, int un_y) const
  { return (LeabraUnit*)inherited::GetUnitGpIdxUnXY(gp_idx, un_x, un_y); }
  // #CAT_Access get the unit state at given group index and unit X,Y coordinate -- preferred Program interface as no NetworkState arg is required
  inline LeabraUnit* GetUnitGpUnXY(int gp_x, int gp_y, int un_x, int un_y) const
  { return (LeabraUnit*)inherited::GetUnitGpUnXY(gp_x, gp_y, un_x, un_y);  }
  // #CAT_Access get the unit state at given group X,Y and unit X,Y coordinates -- preferred Program interface as no NetworkState arg is required
  inline LeabraUnit*  MostActiveUnit(int& idx) const
  { return (LeabraUnit*)inherited::MostActiveUnit(idx); }
  // #CAT_Statistic Return the unit with the highest activation (act) value -- index of unit is returned in idx


  void  CheckSpecs() override;
  void	CheckInhibCons(LeabraNetwork* net);
  // #CAT_Structure check for inhibitory connections -- sets flag on network

  ////////////////////////////////////////////
  //	TwoD Misc structural routines

  virtual bool   TwoDValMode();
  // #CAT_TwoD are we operating in TwoD value spec mode?  i.e., the layerspec is set to a TwoDValLayerSpec
  
  void	ApplyInputData_2d(NETWORK_STATE* net, taMatrix* data, ExtFlags ext_flags,
                          Random* ran, const taVector2i& offs, bool na_by_range=false) override;
  void	ApplyInputData_Gp4d(NETWORK_STATE* net, taMatrix* data, ExtFlags ext_flags,
                            Random* ran, bool na_by_range=false) override;
  
  ////////////////////////////////////////////
  //	Misc structural routines

  bool          SetLayerSpec(LayerSpec* sp) override;
  LayerSpec*    GetMainLayerSpec() const override { return (LayerSpec*)spec.SPtr(); }
  
  void	InitLinks() override;
  void	CutLinks() override;
  SIMPLE_COPY(LeabraLayer);
  TA_BASEFUNS(LeabraLayer);
protected:
  void  CheckThisConfig_impl(bool quiet, bool& rval) override;
private:
  void	Initialize();
  void	Destroy()		{ CutLinks(); }
};

TA_SMART_PTRS(E_API, LeabraLayer);

#endif // LeabraLayer_h

// Copyright 2013-2017, Regents of the University of Colorado,
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

#ifndef LeabraLayerSpec_h
#define LeabraLayerSpec_h 1

// parent includes:
#include <LayerSpec>
#include <SpecMemberBase>

// member includes:
#include <Layer>

eTypeDef_Of(LeabraAvgMax);
eTypeDef_Of(LeabraInhibSpec);
eTypeDef_Of(LayerAvgActSpec);
eTypeDef_Of(LeabraAdaptInhib);
eTypeDef_Of(LeabraActMargin);
eTypeDef_Of(LeabraInhibMisc);
eTypeDef_Of(LeabraClampSpec);
eTypeDef_Of(LayerDecaySpec);
eTypeDef_Of(LeabraDelInhib);
eTypeDef_Of(LeabraCosDiffMod);
eTypeDef_Of(LeabraLayStats);

#include <LeabraAvgMax_cpp>

#include <LeabraNetworkState_cpp>
#include <LeabraLayerState_cpp>
#include <LeabraUnGpState_cpp>
#include <LeabraPrjnState_cpp>
#include <LeabraUnitState_cpp>

#include <State_main>

// declare all other types mentioned but not required to include:
class LeabraLayer; // 
class LeabraNetwork; // 
class LeabraUnit; //

eTypeDef_Of(LeabraLayerSpec);


#include <LeabraLayerSpec_mbrs>

class E_API LeabraLayerSpec : public LayerSpec {
  // #STEM_BASE ##CAT_Leabra Leabra layer specs, computes inhibitory input for all units in layer
INHERITED(LayerSpec)
public:

#include <LeabraLayerSpec_core>

  virtual void Compute_OutputName(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Statistic compute the output_name field from the layer acts.max_i (only for OUTPUT or TARGET layers)
  virtual void Compute_OutputName_ugp(LeabraLayer* lay, LeabraNetwork* net, int gpidx);
  // #IGNORE compute the output_name field from the layer acts.max_i (only for OUTPUT or TARGET layers)

  virtual void  LayerAvgAct(DataTable* report_table = NULL);
  // #BUTTON #NULL_OK #NULL_TEXT_NewReportData create a data table with the current layer average activations (acts_m_avg, acts_p_avg, acts_p_avg_eff) and the values specified in the layerspec avg_act.init -- this is useful for setting the .init values accurately based on actual levels 

  ////////////////////////////////////////////
  //	Misc structural routines

  virtual LeabraLayer* FindLayerFmSpec(LeabraLayer* lay, int& prjn_idx, TypeDef* layer_spec);
  // #CAT_Structure find a layer that given layer receives from based on the type of layer spec
  virtual LeabraLayer* FindLayerFmSpecExact(LeabraLayer* lay, int& prjn_idx, TypeDef* layer_spec);
  // #CAT_Structure find a layer that given layer receives from based on the type of layer spec: uses exact type match, not inherits!
  static  LeabraLayer* FindLayerFmSpecNet(Network* net, TypeDef* layer_spec);
  // #CAT_Structure find a layer in network based on the type of layer spec

  virtual void	HelpConfig();	// #BUTTON #CAT_Structure get help message for configuring this spec
  bool CheckConfig_Layer(Layer* lay, bool quiet=false) override;
  // check for for misc configuration settings required by different algorithms, including settings on the processes NOTE: this routine augments the default layer checks, it doesn't replace them

  String        GetToolbarName() const override { return "layer spec"; }

  void	InitLinks() override;
  SIMPLE_COPY(LeabraLayerSpec);
  TA_BASEFUNS(LeabraLayerSpec);
protected:
  SPEC_DEFAULTS;
  void	UpdateAfterEdit_impl() override;
private:
  void 	Initialize();
  void	Destroy()		{ CutLinks(); }
  void	Defaults_init();
};

SPECPTR_OF(LeabraLayerSpec);

#endif // LeabraLayerSpec_h

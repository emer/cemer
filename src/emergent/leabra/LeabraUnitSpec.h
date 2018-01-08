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

#ifndef LeabraUnitSpec_h
#define LeabraUnitSpec_h 1

// parent includes:
#include "network_def.h"
#include <UnitSpec>
#include <SpecMemberBase>

// member includes:
#include <MinMaxRange>
#include <RandomSpec>
#include <Schedule>
#include <taMath_float>

eTypeDef_Of(LeabraInhibVals);
eTypeDef_Of(LeabraTwoDVals);

#include <LeabraNetworkState_cpp>
#include <LeabraUnitState_cpp>
#include <LeabraUnGpState_cpp>
#include <LeabraLayerState_cpp>
#include <LeabraConState_cpp>

#include <State_main>

// declare all other types mentioned but not required to include:
class Unit; // 
class Network; // 
class LeabraNetwork; // 
class DataTable; //
class LeabraLayer; //
class LeabraLayerSpec; //
class LeabraInhib; //
class FunLookup; // 

eTypeDef_Of(LeabraActFunSpec);
eTypeDef_Of(LeabraActMiscSpec);
eTypeDef_Of(SpikeFunSpec);
eTypeDef_Of(SpikeMiscSpec);
eTypeDef_Of(OptThreshSpec);
eTypeDef_Of(LeabraInitSpec);
eTypeDef_Of(LeabraDtSpec);
eTypeDef_Of(LeabraActAvgSpec);
eTypeDef_Of(LeabraAvgLSpec);
eTypeDef_Of(LeabraAvgL2Spec);
eTypeDef_Of(LeabraChannels);
eTypeDef_Of(KNaAdaptSpec);
eTypeDef_Of(KNaAdaptMiscSpec);
eTypeDef_Of(ShortPlastSpec);
eTypeDef_Of(SynDelaySpec);
eTypeDef_Of(DeepSpec);
eTypeDef_Of(TRCSpec);
eTypeDef_Of(DaModSpec);
eTypeDef_Of(LeabraNoiseSpec);
eTypeDef_Of(LeabraConSpec);


eTypeDef_Of(LeabraUnitSpec);


#include <LeabraUnitSpec_mbrs>

class E_API LeabraUnitSpec : public UnitSpec {
  // #STEM_BASE ##CAT_Leabra Leabra unit specifications, point-neuron approximation
INHERITED(UnitSpec)
public:

#include <LeabraUnitSpec_core>
  
  ///////////////////////////////////////////////////////////////////////
  //        Misc Housekeeping, non Compute functions

  virtual void        CreateNXX1Fun(LeabraActFunSpec& act_spec, FunLookup& nxx1_fl,
                              FunLookup& noise_fl);
  // #CAT_Activation create convolved gaussian and x/x+1 function lookup tables

  virtual void         BioParams
    (float norm_sec=0.001f, float norm_volt=0.1f, float volt_off=-0.1f,
     float norm_amp=1.0e-8f, float C_pF=281.0f, float gbar_l_nS=10.0f,
     float gbar_e_nS=100.0f, float gbar_i_nS=100.0f,
     float erev_l_mV=-70.0f, float erev_e_mV=0.0f, float erev_i_mV=-75.0f,
     float act_thr_mV=-50.0f, float spk_thr_mV=20.0f, float exp_slope_mV=2.0f);
  // #BUTTON set parameters based on biologically-based values, using normalization scaling to convert into typical Leabra standard parameters.  norm_x are normalization values to convert from SI units to normalized values (defaults are 1ms = .001 s, 100mV with -100 mV offset to bring into 0-1 range between -100..0 mV, 1e-8 amps (makes g_bar, C, etc params nice).  other defaults are based on the AdEx model of Brette & Gurstner (2005), which the SPIKE mode implements exactly with these default parameters -- last bit of name indicates the units in which this value must be provided (mV = millivolts, ms = milliseconds, pF = picofarads, nS = nanosiemens, nA = nanoamps)

  virtual void        GraphVmFun(DataTable* graph_data, float g_i = .5, float min = 0.0,
                           float max = 1.0, float incr = .01);
  // #MENU_BUTTON #MENU_ON_Graph #NULL_OK #NULL_TEXT_NewGraphData graph membrane potential (v_m) as a function of excitatory net input (net) for given inhib conductance (g_i) (NULL = new graph data)
  virtual void        GraphActFmNetFun(DataTable* graph_data, float g_i = .5, float min = 0.0,
                                 float max = 1.0, float incr = .001, float g_e_thr = 0.5,
                                 float lin_gain = 10);
  // #MENU_BUTTON #MENU_ON_Graph #NULL_OK #NULL_TEXT_NewGraphData graph the activation function as a function of net input -- this is the direct activation function, computed relative to the g_e_thr threshold value provided -- a linear comparison with lin_gain slope is also provided for reference -- always computed as lin_gain * (net - g_e_thr) (NULL = new graph data)
  virtual void        GraphSpikeAlphaFun(DataTable* graph_data, bool force_alpha=false);
  // #MENU_BUTTON #MENU_ON_Graph #NULL_OK #NULL_TEXT_NewGraphData graph the spike alpha function for conductance integration over time window given in spike parameters -- last data point is the sum over the whole window (total conductance of a single spike) -- force_alpha means use explicit alpha function even when rise=0 (otherewise it simulates actual recursive exp decay used in optimized code)
//   virtual void        GraphSLNoiseAdaptFun(DataTable* graph_data, float incr = 0.05f);
//   // #MENU_BUTTON #MENU_ON_Graph #NULL_OK #NULL_TEXT_NewGraphData graph the short and long-term noise adaptation function, which integrates both short-term and long-term performance values
  virtual void        TimeExp(int mode, int nreps=100000000);
  // #EXPERT time how long it takes to compute various forms of exp() function: mode=0 = double sum ctrl (baseline), mode=1 = std double exp(), mode=2 = taMath_double::exp_fast, mode=3 = float sum ctrl (float baseline), mode=4 = expf, mode=5 = taMath_float::exp_fast -- this is the dominant cost in spike alpha function computation, so we're interested in optimizing it..

  bool          CheckConfig_Unit(Layer* lay, bool quiet=false) override;
  TypeDef*      MinBiasSpecType() const override { return &TA_LeabraConSpec; }

  String        GetToolbarName() const override { return "unit spec"; }

  void          InitLinks() override;
  SIMPLE_COPY(LeabraUnitSpec);
  TA_BASEFUNS(LeabraUnitSpec);
protected:
  SPEC_DEFAULTS;
  void         UpdateAfterEdit_impl() override;        // to set _impl sig
  void         CheckThisConfig_impl(bool quiet, bool& rval) override;

private:
  void        Initialize();
  void        Destroy()                { };
  void        Defaults_init();
};

TA_SMART_PTRS(E_API, LeabraUnitSpec);

#endif // LeabraUnitSpec_h

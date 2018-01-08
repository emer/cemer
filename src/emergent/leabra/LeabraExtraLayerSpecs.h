// Copyright 2017-2017, Regents of the University of Colorado,
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

#ifndef LeabraExtraLayerSpecs_h
#define LeabraExtraLayerSpecs_h 1

// parent includes:
#include <LeabraLayerSpec>

#include <State_main>

// member includes:

eTypeDef_Of(ScalarValSpec);
eTypeDef_Of(ScalarValBias);
eTypeDef_Of(OutErrSpec);
eTypeDef_Of(ExtRewSpec);
eTypeDef_Of(TwoDValSpec);
eTypeDef_Of(TwoDValBias);

#include <LeabraExtraLayerSpecs_mbrs>

// declare all other types mentioned but not required to include:

eTypeDef_Of(ScalarValLayerSpec);

class E_API ScalarValLayerSpec : public LeabraLayerSpec {
  // represents a scalar value using a coarse-coded distributed code over units.  the external input to the first unit is used to generate distributed inputs to the rest of the units, but unlike in earlier versions, all the units represent the distributed representation - the first unit is not just for display anymore, though it does contain the scalar readout val in misc_1 unit variable, and misc_2 has the minus phase read out
INHERITED(LeabraLayerSpec)
public:

#include <ScalarValLayerSpec>
  
  virtual void	LabelUnits(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net);
  // #CAT_ScalarVal label units in given layer with their underlying values
    virtual void LabelUnits_ugp(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net,
                                int gpidx);
    // #CAT_ScalarVal label units with their underlying values
  virtual void	LabelUnitsNet();
  // #BUTTON #CAT_ScalarVal label all layers in given network using this spec

  void	HelpConfig() override;	// #BUTTON get help message for configuring this spec
  bool  CheckConfig_Layer(Layer* lay, bool quiet=false) override;

  TA_SIMPLE_BASEFUNS(ScalarValLayerSpec);
protected:
  SPEC_DEFAULTS;
  void	UpdateAfterEdit_impl() override;
private:
  void 	Initialize();
  void	Defaults_init();
  void	Destroy()		{ };
};


eTypeDef_Of(ExtRewLayerSpec);

class E_API ExtRewLayerSpec : public ScalarValLayerSpec {
  // computes external reward feedback: minus phase is zero, plus phase is reward value derived from network performance or other inputs (computed at start of 1+)
INHERITED(ScalarValLayerSpec)
public:

#include <ExtRewLayerSpec>

  void	HelpConfig() override;	// #BUTTON get help message for configuring this spec
  bool  CheckConfig_Layer(Layer* lay, bool quiet=false) override;

  TA_SIMPLE_BASEFUNS(ExtRewLayerSpec);
protected:
  SPEC_DEFAULTS;
  void	UpdateAfterEdit_impl() override;
private:
  void 	Initialize();
  void	Defaults_init();
  void	Destroy()		{ };
};


eTypeDef_Of(TwoDValLayerSpec);

class E_API TwoDValLayerSpec : public LeabraLayerSpec {
  // represents one or more two-d value(s) using a coarse-coded distributed code over units.  one val readout is weighted-average; multiple vals = max bumps over 3x3 local grid
INHERITED(LeabraLayerSpec)
public:

#include <TwoDValLayerSpec>

  virtual void	LabelUnits(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net);
  // #CAT_TwoDVal label units in given layer with their underlying values
  virtual void   LabelUnits_ugp(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net, int gpidx);
  // #CAT_TwoDVal label units with their underlying values
  virtual void	LabelUnitsNet();
  // #BUTTON #CAT_TwoDVal label all layers in given network using this spec

  virtual void	ReConfig(Network* net, int n_units = -1);
  // #BUTTON reconfigure layer and associated specs for current scalar.rep type; if n_units > 0, changes number of units in layer to specified value

  void	HelpConfig() override;	// #BUTTON get help message for configuring this spec
  bool  CheckConfig_Layer(Layer* lay, bool quiet=false) override;

  TA_SIMPLE_BASEFUNS(TwoDValLayerSpec);
protected:
  SPEC_DEFAULTS;
  void	UpdateAfterEdit_impl() override;
private:
  void 	Initialize();
  void	Defaults_init() 	{ };
  void	Destroy()		{ };
};


eTypeDef_Of(DecodeTwoDValLayerSpec);

class E_API DecodeTwoDValLayerSpec : public TwoDValLayerSpec {
  // a two-d-value layer spec that copies its activations from one-to-one input prjns, to act as a decoder of another layer
INHERITED(TwoDValLayerSpec)
public:

#include <DecodeTwoDValLayerSpec>

  TA_BASEFUNS_NOCOPY(DecodeTwoDValLayerSpec);
private:
 void 	Initialize()            { };
  void	Destroy()		{ };
};


// not converting below:
// #include <MotorForceLayerSpec>
// #include <LeabraV1LayerSpec>
// #include <GPiSoftMaxLayerSpec>
// #include <SubiculumLayerSpec>

#endif // LeabraExtraLayerSpecs_h

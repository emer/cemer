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

#ifndef LeabraTILayerSpec_h
#define LeabraTILayerSpec_h 1

// parent includes:
#include <LeabraLayerSpec>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(LeabraTILayerSpec);

class E_API LeabraTILayerSpec : public LeabraLayerSpec {
  // #AKA_DeepContextLayerSpec Leabra Temporal Integration algorithm -- effectively a simple recurrent network (SRN) context layer at this point, with a flexible api for future expansion -- only really need to use this for DEEP (Context) layers -- supers can be any layerspec -- deep should also have LayerActUnitSpec unit spec to optimize computation
INHERITED(LeabraLayerSpec)
public:
  enum LaminaType {
    SUPER,			// superficial layer (neocortical layers 2,3) -- runs free at all times and receives context info from the deep layer
    DEEP,			// deep layer (neocortical layers 5,6) -- copies the corresponding one-to-one super layer activations and holds them for a trial (~100 msec, alpha frequency updating)
  };

  LaminaType		lamina;	// which neocortical lamina is this -- superficial or deep?

    virtual void Clear_Maint(LeabraLayer* lay, LeabraNetwork* net);
  // clear maint_h maintenance values in layer

  virtual void Compute_MaintFmSuper(LeabraLayer* lay, LeabraNetwork* net);
  // set maint_h of deep unit from current sending super unit activation
  virtual void Compute_ActFmSuper(LeabraLayer* lay, LeabraNetwork* net);
  // set current act of deep unit to sending super unit activation
  virtual void Compute_ActFmMaint(LeabraLayer* lay, LeabraNetwork* net);
  // set current act of deep unit to maint_h

  virtual void Compute_TIAct(LeabraLayer* lay, LeabraNetwork* net);
  // compute activation in units appropriate for current time layer type and lamina 

  override void Compute_CycleStats(LeabraLayer* lay, LeabraNetwork* net);
  override void	PostSettle(LeabraLayer* lay, LeabraNetwork* net);

  override bool	Compute_dWt_FirstPlus_Test(LeabraLayer* lay, LeabraNetwork* net)
  { if(lamina == DEEP) return false; return inherited::Compute_dWt_FirstPlus_Test(lay, net); }
  override bool	Compute_dWt_Nothing_Test(LeabraLayer* lay, LeabraNetwork* net)
  { if(lamina == DEEP) return false; return inherited::Compute_dWt_Nothing_Test(lay, net); }

  override bool  CheckConfig_Layer(Layer* lay, bool quiet=false);

  TA_SIMPLE_BASEFUNS(LeabraTILayerSpec);
protected:
  SPEC_DEFAULTS;
  void  UpdateAfterEdit_impl();

private:
  void 	Initialize();
  void	Destroy()		{ };
  void	Defaults_init();
};

#endif // LeabraTILayerSpec_h

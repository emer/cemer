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

#ifndef LeabraWizard_h
#define LeabraWizard_h 1

// parent includes:
#include "network_def.h"
#include <Wizard>

// member includes:

// declare all other types mentioned but not required to include:
class Network; // 
class DataTable; // 
class Program; // 
class LeabraNetwork; // 
class LeabraLayer; // 

eTypeDef_Of(LeabraWizard);

class E_API LeabraWizard : public Wizard {
  // #STEM_BASE ##CAT_Leabra Leabra-specific wizard for automating construction of simulation objects
INHERITED(Wizard)
public:

  bool  StdNetwork() override;

  bool	UpdateInputDataFmNet(Network* net, DataTable* data_table) override;

  virtual bool	StdLeabraSpecs(LeabraNetwork* net);
  // #MENU_BUTTON #MENU_ON_Network #MENU_SEP_BEFORE make standard layer specs for a basic Leabra network (FFFB inhib for hiddens, KWTA PAT_K for input/output), and TopDownCons con spec for connections from higher layers

  virtual bool	DeepLeabra(LeabraNetwork* net, const String& lay_name_contains = "");
  // #MENU_BUTTON configure DeepLeabra specs and layers, for hidden layers in the network (all or optionally those that contain given string) -- creates corresponding deep cortical layer and thalamic TRC layers for predictive auto-encoder learning, driven from deep raw driver projections coming from lower layers

  virtual bool	DeepLeabraCopy(LeabraNetwork* net, const String& lay_name_contains,
                               LeabraLayer* source_deep_layer);
  // #MENU_BUTTON #PROJ_SCOPE_2 configure DeepLeabra layer(s) with name containing given string, copying specs from given source deep layer which is already configured -- creates corresponding deep cortical layer and thalamic TRC layers for predictive auto-encoder learning, driven from deep raw driver projections coming from lower layers
  
  virtual bool	SRNContext(LeabraNetwork* net, const String& lay_name_contains = "");
  // #MENU_BUTTON configure a simple-recurrent-network context layer in the network -- specify name for layers to create context of -- if empty, will do all HIDDEN layers

  virtual bool	UnitInhib(LeabraNetwork* net, int n_inhib_units=10);
  // #MENU_BUTTON configures unit-based inhibition for all layers in the network

  virtual bool 	Hippo(LeabraNetwork* net, int n_ec_slots = 8);
  // #MENU_BUTTON #MENU_SEP_BEFORE configure standard Hippocampus system, using specialized QuadPhase learning mechanisms

  virtual bool 	TD(LeabraNetwork* net, bool td_mod_all = false);
  // #MENU_BUTTON #MENU_SEP_BEFORE configure standard TD reinforcement learning layers; td_mod_all = have td value modulate all the regular units in the network

  virtual bool 	PVLV(LeabraNetwork* net, int n_pos_pv=1, int n_neg_pv=1,
                     bool da_mod_all = false);
  // #MENU_BUTTON #MENU_SEP_BEFORE configure PVLV (pavlovian primary value and learned value) learning layers in a network -- provides a simulated dopamine signal that reflects unexpected primary rewards through interaction of PV (primary value) and LV (learned value) systems -- this is the bi-valent (bv) version that includes both appetitive and aversive (punishments) USs; n_pos_pv = number of positive PV states (rewards) available; n_neg_pv = number of negative PV states; da_mod_all = have vta dopamine value modulate all the regular units in the network (not typically used). The PVLV module takes three different kinds of input layers: 1) CS-type inputs, e.g., Stim_In in bvPVLV.proj; one-to-one prjn to lateral amygdala (LA) layer and full projections to all VSMatrix layers; 2) Context-type inputs, e.g., Context_In in bvPVLV.proj; full projection to basal amygdalar EXTINCTION layers; and, 3) Time-evolving inputs, e.g., USTime_In in bvPVLV.proj; full projection to all VSPatch layers HELPFUL NOTE: If PRIOR to running the PVLV wizard there are already three or more layers ALL of layer_type == INPUT, and some of their names contain_ci (case insensitive) the key character strings ``context'', and/or ``time'', the wizard will try to hook everything up appropriately - but that should definitely be checked by the user to make sure things end up as intended. Otherwise, the wizard will connect INPUT layers indiscriminantly and the user will have to correct manually. ALSO, since the LA layer requires a one-to-one projection from the input representing CS-type information, its unit geometry can/should be edited manually to match the corresponding input layer in the larger model.    

  virtual bool 	PVLV_Specs(LeabraNetwork* net);
  // create and configure PVLV (pavlovian primary value and learned value) specs -- these are always created in a group called PVLV

  virtual bool 	PVLV_Defaults(LeabraNetwork* net);
  // #MENU_BUTTON set the parameters in the specs of the network to the latest default values for the PVLV model, and also ensures that the standard control panels are built and contain relevant parameters -- this is only for a model that already has PVLV configured

  virtual bool PVLV_SetLrate(LeabraNetwork* net, float base_lrate=0.04);
  // #MENU_BUTTON set the learning rate for PVLV network -- does a coordinated update across the different learning rates, to keep default dynamics balanced -- given value is the base lrate; actual lrates applied to Patch, Matrix, and BAAcq, BAext layers are different multiples of base lrate
  virtual bool PVLV_ConnectCSLayer(LeabraNetwork* net, LeabraLayer* sending_layer,
				 bool disconnect = false);
  // #MENU_BUTTON #PROJ_SCOPE_1 make (or break if disconnect = true) connections between given CS-encoding sending_layer in given network and the relevant PVLV layers (Lateral Amygdala, VSMatrix)
  virtual bool PVLV_OutToExtRew(LeabraNetwork* net, LeabraLayer* output_layer,
				 bool disconnect = false);
  // #MENU_BUTTON #PROJ_SCOPE_1 make (or break if disconnect = true) connection between given output_layer in given network and the ExtRew layer, which uses this output layer together with the RewTarg layer input to automatically compute reward value based on performance

  virtual bool 	PBWM(LeabraNetwork* net, int pfc_gp_x = 2, int pfc_gp_y = 2,
                     bool add_on = false, const String& prefix = "PBWM");
  // #MENU_BUTTON #MENU_SEP_BEFORE configure all the layers and specs for the prefrontal-cortex basal ganglia working memory system (PBWM) -- does a PVLV configuration first (see PVLV for details) and then adds a basal ganglia gating system that is trained by PVLV dopamine signals.  The gating system determines when the PFC working memory representations are updated; unit group (stripe) geometry specified for paired maint and output stripes;  add_on = this is an additional PBWM system on top of one that is already there -- be sure to also change prefix name so it doesn't overwrite

  virtual bool 	PBWM_Specs(LeabraNetwork* net, 
                           const String& prefix = "PBWM", bool set_defs = false);
  // create and configure the PBWM specs using the given prefix -- if set_defs it is being called from Defaults function -- try not to overwrite user-settable parameters

  virtual bool 	PBWM_Defaults(LeabraNetwork* net, 
                              const String& prefix = "PBWM");
  // #MENU_BUTTON set the parameters in the specs of the network to the latest default values for the PBWM model, and also ensures that the standard control panels are built and contain relevant parameters -- this is only for a model that already has PBWM configured

  virtual bool PBWM_SetNStripes
    (LeabraNetwork* net, int pfc_gp_x = 2, int pfc_gp_y = 2,
     int n_matrix_units_x=-1, int n_matrix_units_y=-1,
     int n_pfc_units_x=-1, int n_pfc_units_y=-1, const String& prefix = "PBWM");
  // #MENU_BUTTON #MENU_SEP_BEFORE set number of "stripes" (unit groups) for maint and output gating in the network -- and number of units per unit group in matrix and pfc along each dimension (-1 = use current # of units -- must specify both x and y if specifying)

  virtual bool PBWM_Remove(LeabraNetwork* net,
                           const String& prefix = "PBWM");
  // #MENU_BUTTON remove all the PBWM (and PVLV) specific items from the network (specs and layers) -- can be useful for converting between PBWM versions -- ONLY works when layers are organized into groups

  bool	StdProgs() override;
  bool	TestProgs(Program* call_test_from, bool call_in_loop=true, int call_modulus=1) override;

  TA_BASEFUNS_NOCOPY(LeabraWizard);
protected:
  String RenderWizDoc_network() override;
private:
  void 	Initialize();
  void 	Destroy()	{ };
};

#endif // LeabraWizard_h

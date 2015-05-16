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

  bool StdNetwork() override;
  bool	UpdateInputDataFmNet(Network* net, DataTable* data_table) override;

  virtual bool	StdLeabraSpecs(LeabraNetwork* net);
  // #MENU_BUTTON #MENU_ON_Network #MENU_SEP_BEFORE make standard layer specs for a basic Leabra network (FFFB inhib for hiddens, KWTA PAT_K for input/output), and TopDownCons con spec for connections from higher layers

  virtual bool	LeabraTI(LeabraNetwork* net);
  // #MENU_BUTTON configure temporal integration (LeabraTI) specs and connections, for all hidden layers in the network -- creates new conspec and unit spec under the basic LeabraConSpec_0 and LeabraUnitSpec_0 specs, and new SELF projections with this conspec set -- requires unitspec deep.on = true and deep_qtr = Q4

  virtual bool	DeepLeabra(LeabraNetwork* net, bool ti_ctxt = false, bool top_down_attn = false);
  // #MENU_BUTTON configure DeepLeabra specs and layers, for all hidden layers in the network -- creates corresponding trc layers for predictive auto-encoder learning from deep layer driver projections coming from lower layers -- optionaly create TI deep context self projections, and top-down deep attentional projections from higher layers (latter implies ti_context as well)

  virtual bool	SRNContext(LeabraNetwork* net);
  // #MENU_BUTTON configure a simple-recurrent-network context layer in the network

  virtual bool	UnitInhib(LeabraNetwork* net, int n_inhib_units=10);
  // #MENU_BUTTON configures unit-based inhibition for all layers in the network

  virtual bool 	Hippo(LeabraNetwork* net, int n_ec_slots = 8);
  // #MENU_BUTTON #MENU_SEP_BEFORE configure standard Hippocampus system, using specialized QuadPhase learning mechanisms

  virtual bool 	TD(LeabraNetwork* net, bool bio_labels = false, bool td_mod_all = false);
  // #MENU_BUTTON #MENU_SEP_BEFORE configure standard TD reinforcement learning layers; bio_labels = use biologically-based labels for layers, else functional; td_mod_all = have td value modulate all the regular units in the network

  virtual bool 	PVLV(LeabraNetwork* net, int n_pos_pv=1, int n_neg_pv=1,
                     bool da_mod_all = false);
  // #MENU_BUTTON #MENU_SEP_BEFORE configure PVLV (pavlovian primary value and learned value) learning layers in a network -- provides a simulated dopamine signal that reflects unexpected primary rewards through interaction of PV (primary value) and LV (learned value) systems -- this is the goal-driven version; n_pos_pv = number of positive PV states (positive outcomes); n_neg_pv = number of negative PV states (negative outcomes); da_mod_all = have vta dopamine value modulate all the regular units in the network

  virtual bool 	PVLV_Specs(LeabraNetwork* net);
  // create and configure PVLV (pavlovian primary value and learned value) specs -- these are always created in a group called PVLV

  virtual bool PVLV_ConnectLayer(LeabraNetwork* net, LeabraLayer* sending_layer,
				 bool disconnect = false);
  // #MENU_BUTTON #PROJ_SCOPE_1 make (or break if disconnect = true) connections between given sending_layer in given network and the learning PVLV layers (PVr, PVi, LVe, LVi, NV), each of which should typically receive from the same sending layers
  virtual bool PVLV_OutToPVe(LeabraNetwork* net, LeabraLayer* output_layer,
				 bool disconnect = false);
  // #MENU_BUTTON #PROJ_SCOPE_1 make (or break if disconnect = true) connection between given output_layer in given network and the PVe layer, which uses this output layer together with the RewTarg layer input to automatically compute reward value based on performance

  virtual bool 	PBWM(LeabraNetwork* net, int pfc_gp_x = 2, int pfc_gp_y = 2,
                     bool add_on = false, const String& prefix = "PBWM");
  // #MENU_BUTTON #MENU_SEP_BEFORE configure all the layers and specs for the prefrontal-cortex basal ganglia working memory system (PBWM) -- does a PVLV configuration first (see PVLV for details) and then adds a basal ganglia gating system that is trained by PVLV dopamine signals.  The gating system determines when the PFC working memory representations are updated; unit group (stripe) geometry specified for paired maint and output stripes;  add_on = this is an additional PBWM system on top of one that is already there -- be sure to also change prefix name so it doesn't overwrite

  virtual bool 	PBWM_Specs(LeabraNetwork* net, 
                           const String& prefix = "PBWM", bool set_defs = false);
  // create and configure the PBWM specs using the given prefix -- if set_defs it is being called from Defaults function -- try not to overwrite user-settable parameters

  virtual bool 	PBWM_Defaults(LeabraNetwork* net, 
                              const String& prefix = "PBWM");
  // #MENU_BUTTON set the parameters in the specs of the network to the latest default values for the PBWM model, and also ensures that the standard select edits are built and contain relevant parameters -- this is only for a model that already has PBWM configured

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

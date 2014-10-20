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

#include "LeabraWizard.h"
#include <LeabraProject>
#include <LeabraNetwork>
#include <StdNetWizDlg>
#include <DataTable>
#include <ControlPanel>

#include <FullPrjnSpec>
#include <OneToOnePrjnSpec>
#include <ScalarValLayerSpec>
#include <GpOneToOnePrjnSpec>
#include <MarkerGpOneToOnePrjnSpec>
#include <TesselPrjnSpec>
#include <UniformRndPrjnSpec>
#include <RowColPrjnSpec>
#include <GpRndTesselPrjnSpec>

#include <TwoDValLayerSpec>
#include <LeabraTICtxtConSpec>
#include <MarkerConSpec>
#include <LayerActUnitSpec>
#include <LeabraContextLayerSpec>

#include <ExtRewLayerSpec>
#include <LeabraTdUnit>
#include <LeabraTdUnitSpec>
#include <TDRewPredConSpec>
#include <TDRewPredLayerSpec>
#include <TDRewIntegLayerSpec>
#include <TdLayerSpec>

#include <PPTgUnitSpec>
#include <LHbRMTgUnitSpec>
#include <LearnModUnitSpec>
#include <VTAUnitSpec>
#include <DRNUnitSpec>
#include <VSPatchUnitSpec>
#include <LeabraDeltaConSpec>
#include <LearnModDeltaConSpec>
#include <LearnModHebbConSpec>

#include <PatchUnitSpec>
#include <GPiUnitSpec>
#include <InvertUnitSpec>
#include <ThalUnitSpec>
#include <PFCUnitSpec>
#include <Deep5bLayerSpec>
#include <MatrixConSpec>
#include <Deep5bConSpec>

#include <TopoWtsPrjnSpec>

#include <HippoEncoderConSpec>
#include <XCalCHLConSpec>
#include <ECinLayerSpec>
#include <ECoutLayerSpec>
#include <CA3LayerSpec>
#include <CA1LayerSpec>
#include <SubiculumLayerSpec>

#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(LeabraWizard);

void LeabraWizard::Initialize() {
}

// use these macros to make specs
#define FMSpec(T, var, par, nm)\
  T* var = (T*)par->FindMakeSpec(nm, &TA_##T)
#define FMChild(T, var, par, nm)\
  T* var = (T*)par->FindMakeChild(nm, &TA_##T)

// for previously-defined vars:
#define FMSpecPD(T, var, par, nm)\
  var = (T*)par->FindMakeSpec(nm, &TA_##T)
#define FMChildPD(T, var, par, nm)\
  var = (T*)par->FindMakeChild(nm, &TA_##T)

String LeabraWizard::RenderWizDoc_network() {
  String rval = inherited::RenderWizDoc_network();
  rval += String("\
* [[<this>.LeabraTI()|LeabraTI]] -- configure specs and layers for LeabraTI -- temporal integration of information over time, based on biology -- functionally similar to an SRN but auto-encoding and predictive\n\
* [[<this>.SRNContext()|SRN Context]] -- configure a network with a simple-recurrent-network (SRN) context layer\n\
* [[<this>.UnitInhib()|Unit Inhib]] -- configure unit-based inhibition for all layers in selected network (as compared with standard kWTA inhibition) ('''NOTE: parameters are out of date''').\n\
* [[<this>.Hippo()|Hippo]] -- configure a Hippocampus using quad-phase specs -- high functioning hippocampal episodic memory system.\n\
* [[<this>.TD()|Temporal Differences (TD)]] -- configure temporal-differences (TD) reinforcement learning layers.\n\
* [[<this>.PVLV()|PVLV]] -- configure PVLV (Primary Value, Learned Value) biologically-motivated reinforcement learning layers -- provides a simulated dopamine signal that reflects unexpected primary rewards (PV = primary value system) and unexpected learned reward assocations (conditioned stimuli; LV = learned value = system).\n\
:* [[<this>.PVLV_ConnectLayer()|PVLV Connect Layer]] -- connect or disconnect a layer as an input to the PVLV system -- multiple PVLV layers should be connected together so this automates that process and is strongly recommended.\n\
:* [[<this>.PVLV_OutToPVe()|PVLV Connect Output to PVe]] -- connect or disconnect an Output layer to the PVe (primary value, excitatory) layer, which then computes reward based on network success in producing target outputs.\n\
* [[<this>.PBWM()|PBWM]] -- create and configure prefrontal cortex basal ganglia working memory (PBWM) layers in the network -- also does a PVLV configuration, which does the reinforcement learning for PBWM.\n\
:* [[<this>.PBWM_Defaults()|PBWM Defaults]] -- set the parameters in the specs of the network to the latest default values for the PBWM model, and also ensures that the standard select edits are built and contain relevant parameters -- this is only for a model that already has PBWM configured and in a standard current format (i.e., everything in groups).\n\
:* [[<this>.PBWM_SetNStripes()|PBWM Set N Stripes]] -- set the number of stripes (unit groups) throughout the set of PFC and BG layers that have stripes -- easier than doing it manually for each layer.\n\
:* [[<this>.PBWM_Remove()|PBWM Remove]] -- Remove PBWM layers and specs from a network -- can be useful for converting between PBWM versions -- ONLY works when layers are organized into groups.\n\
");
  return rval;
}

bool LeabraWizard::StdNetwork() {
  ProjectBase* proj = GET_MY_OWNER(ProjectBase);
  if(proj->networks.size == 0) {        // make a new one for starters always
    LeabraNetwork* net = (LeabraNetwork*)proj->networks.New(1);
    if(net)
      StdLeabraSpecs(net);
  }
  if(!std_net_dlg) {
    taBase::SetPointer((taBase**)&std_net_dlg, new StdNetWizDlg);
  }
  taBase::Own(std_net_dlg, this);
  bool rval = std_net_dlg->DoDialog();
  if(std_net_dlg && std_net_dlg->network)
    StdLeabraSpecs((LeabraNetwork*)std_net_dlg->network.ptr()); // re-run to organize things better now that stuff has happened
  return rval;
}

bool LeabraWizard::StdLeabraSpecs(LeabraNetwork* net) {
  if(!net) {
    LeabraProject* proj = GET_MY_OWNER(LeabraProject);
    net = (LeabraNetwork*)proj->GetNewNetwork();
    if(TestError(!net, "StdLayerSpecs", "network is NULL and could not make a new one -- aborting!")) return false;
  }
  LeabraLayerSpec* hid = (LeabraLayerSpec*)net->specs.FindType(&TA_LeabraLayerSpec);
  if(!hid) {
    hid = (LeabraLayerSpec*)net->FindMakeSpec("HiddenLayer", &TA_LeabraLayerSpec);
  }
  else {
    hid->name = "HiddenLayer";
  }
  LeabraLayerSpec* inout;
  inout = (LeabraLayerSpec*)hid->children.FindMakeSpec("Input_Output",
                                                       &TA_LeabraLayerSpec);

  // move the bias spec under the con spec
  LeabraBiasSpec* bs;
    bs = (LeabraBiasSpec*)net->specs.FindType(&TA_LeabraBiasSpec);
  if(bs != NULL) {
    LeabraConSpec* ps = (LeabraConSpec*)bs->FindParent();
    if(ps != NULL) return false;
    ps = (LeabraConSpec*)net->specs.FindSpecTypeNotMe(&TA_LeabraConSpec, bs);
    if(ps != NULL) {
      ps->children.Transfer(bs);
    }
  }

  LeabraConSpec* ps = (LeabraConSpec*)net->specs.FindType(&TA_LeabraConSpec);
  if(ps) {
    FMChild(LeabraConSpec, td, ps, "TopDownCons");
    td->desc = "Leabra (particularly the XCAL learning rule) requires top-down connections to be weaker than bottom-up ones -- this spec achieves that by setting wt_scale.rel = .2 -- set this for any connections coming from higher-level TARGET layers";
    td->SetUnique("wt_scale", true);
    td->wt_scale.rel = 0.2f;
  }

  return true;
}

///////////////////////////////////////////////////////////////
//                      Unit Inhib
///////////////////////////////////////////////////////////////

bool LeabraWizard::UnitInhib(LeabraNetwork* net, int n_inhib_units) {
  LeabraProject* proj = GET_MY_OWNER(LeabraProject);
  if(!net) {
    net = (LeabraNetwork*)proj->GetNewNetwork();
    if(TestError(!net, "UnitInhib", "network is NULL and could not make a new one -- aborting!")) return false;
  }

  net->RemoveUnits();

  LeabraUnitSpec* basic_us = (LeabraUnitSpec*)net->FindSpecType(&TA_LeabraUnitSpec);
  if(TestError(!basic_us, "UnitInhib", "basic LeabraUnitSpec not found, bailing!")) {
    return false;
  }
  FMChild(LeabraUnitSpec, inhib_us, basic_us, "InhibUnits");

  LeabraConSpec* basic_cs = (LeabraConSpec*)net->FindSpecType(&TA_LeabraConSpec);
  if(TestError(!basic_cs, "UnitInhib", "basic LeabraConSpec not found, bailing!")) {
    return false;
  }
  FMChild(LeabraConSpec, inhib_cs, basic_cs, "InhibCons");

  FMChild(LeabraConSpec, fb_inhib_cs, basic_cs, "FBtoInhib");
  FMChild(LeabraConSpec, ff_inhib_cs, fb_inhib_cs, "FFtoInhib");

  LeabraLayerSpec* basic_ls = (LeabraLayerSpec*)net->FindSpecType(&TA_LeabraLayerSpec);
  if(TestError(!basic_ls, "UnitInhib", "basic LeabraLayerSpec not found, bailing!")) {
    return false;
  }
  FMChild(LeabraLayerSpec, inhib_ls, basic_ls, "InhibLayers");

  FullPrjnSpec* fullprjn = (FullPrjnSpec*)net->FindSpecType(&TA_FullPrjnSpec);
  if(TestError(!fullprjn, "UnitInhib", "basic FullPrjnSpec not found, bailing!")) {
    return false;
  }

  // todo: optimize these params..
  //  basic_us->dt.vm_tau = ??
  basic_us->g_bar.i = 10.0f;
  inhib_us->SetUnique("dt", true);
  //  inhib_us->dt.vm_tau = ??

  inhib_cs->SetUnique("rnd", true);
  inhib_cs->rnd.mean = 1.0f;  inhib_cs->rnd.var = 0.0f;
  inhib_cs->SetUnique("wt_limits", true);
  inhib_cs->wt_limits.sym = false;
  inhib_cs->SetUnique("inhib", true);
  inhib_cs->inhib = true;

  fb_inhib_cs->SetUnique("wt_limits", true);
  fb_inhib_cs->wt_limits.sym = false;
  fb_inhib_cs->SetUnique("rnd", true);
  fb_inhib_cs->rnd.mean = .5f;  fb_inhib_cs->rnd.var = 0.05f;
  fb_inhib_cs->SetUnique("lrate", true);
  fb_inhib_cs->lrate = 0.0f;

  // todo: optimize
  ff_inhib_cs->SetUnique("wt_scale", true);
  ff_inhib_cs->wt_scale.abs = .4f;

  basic_us->UpdateAfterEdit();
  basic_cs->UpdateAfterEdit();
  basic_ls->UpdateAfterEdit();

  int i;
  for(i=0;i<net->layers.size;i++) {
    LeabraLayer* lay = (LeabraLayer*)net->layers[i];
    if(lay->layer_type == Layer::INPUT) continue;
    String nm = lay->name;
    nm.downcase();
    if(nm.contains("_inhib")) continue;

    String inm = lay->name + "_Inhib";
    LeabraLayer* ilay = (LeabraLayer*)net->layers.FindName(inm);
    if(ilay == NULL) {
      ilay = (LeabraLayer*)net->layers.NewEl(1);
      ilay->name = inm;
      ilay->pos.z = lay->pos.z;
      ilay->pos.x = lay->pos.x + lay->disp_geom.x + 1;
      ilay->un_geom.n = n_inhib_units;
      if(n_inhib_units <= 20) {
        ilay->un_geom.x = 2; ilay->un_geom.y = n_inhib_units / 2;
        while(ilay->un_geom.x * ilay->un_geom.y < n_inhib_units) ilay->un_geom.y++;
      }
      else if(n_inhib_units <= 40) {
        ilay->un_geom.x = 4; ilay->un_geom.y = n_inhib_units / 4;
        while(ilay->un_geom.x * ilay->un_geom.y < n_inhib_units) ilay->un_geom.y++;
      }
    }
    ilay->SetLayerSpec(inhib_ls);
    ilay->SetUnitSpec(inhib_us);

    int j;
    for(j=0;j<lay->projections.size;j++) {
      LeabraLayer* fmlay = (LeabraLayer*)((Projection*)lay->projections[j])->from.ptr();
      if(fmlay->name.contains("_Inhib")) continue;
      if(fmlay == lay) continue;
      net->FindMakePrjn(ilay, fmlay, fullprjn, ff_inhib_cs);
    }
    net->FindMakePrjn(ilay, lay, fullprjn, fb_inhib_cs);
    net->FindMakePrjn(lay, ilay, fullprjn, inhib_cs);
    net->FindMakePrjn(ilay, ilay, fullprjn, inhib_cs);
  }

  net->UpdateAfterEdit();
  // todo:
//   net->InitAllViews();

  // todo: !!!
//   winbMisc::DelayedMenuUpdate(net);

  // set settle cycles to 300

  ControlPanel* edit = proj->FindMakeControlPanel("UnitInhib");
  if(edit != NULL) {
    basic_us->AddToControlPanelNm("dt", edit, "excite");
    inhib_us->AddToControlPanelNm("dt", edit, "inhib");
    basic_us->AddToControlPanelNm("g_bar", edit, "excite");
    ff_inhib_cs->AddToControlPanelNm("wt_scale", edit, "ff_inhib");
    fb_inhib_cs->AddToControlPanelNm("rnd", edit, "to_inhib");
    inhib_cs->AddToControlPanelNm("rnd", edit, "fm_inhib");
    basic_ls->AddToControlPanelNm("adapt_i", edit, "layers");

    net->AddToControlPanelNm("cycle_max", edit, net->name);
    net->AddToControlPanelNm("min_cycles", edit, net->name);
  }

  if(proj) {
    proj->undo_mgr.SaveUndo(net, "Wizard::UnitInhib -- actually saves network specifically");
  }
  return true;
}

bool LeabraWizard::StdProgs() {
  // todo: could check for subgroups and do LeabraAll_GpData instead
  //Program_Group* pg =
  if(!StdProgs_impl("LeabraAll_Std")) return false;
  // todo: could do something more here..
  return true;
}

bool LeabraWizard::TestProgs(Program* call_test_from, bool call_in_loop, int call_modulus) {
  if(!TestProgs_impl("LeabraAll_Test", call_test_from, call_in_loop, call_modulus))
    return false;
  return true;
}

bool LeabraWizard::UpdateInputDataFmNet(Network* net, DataTable* data_table) {
  if(TestError(!data_table || !net, "UpdateInputDataFmNet",
               "must specify both a network and a data table")) return false;
  data_table->StructUpdate(true);
  FOREACH_ELEM_IN_GROUP(LeabraLayer, lay, net->layers) {
    if(lay->layer_type == Layer::HIDDEN) continue;
    int lay_idx = 0;

    LeabraLayerSpec* ls = (LeabraLayerSpec*)lay->GetLayerSpec();
    if(ls->InheritsFrom(&TA_ScalarValLayerSpec) && !((ScalarValLayerSpec*)ls)->scalar.clamp_pat) {
      if(lay->unit_groups) {
        data_table->FindMakeColName
          (lay->name, lay_idx, DataTable::VT_FLOAT, 4, 1, 1,
           MAX(lay->gp_geom.x,1), MAX(lay->gp_geom.y,1));
      }
      else {
        data_table->FindMakeColName
          (lay->name, lay_idx, DataTable::VT_FLOAT, 2, 1, 1);
      }
    }
    else if(ls->InheritsFrom(&TA_TwoDValLayerSpec) && !((TwoDValLayerSpec*)ls)->twod.clamp_pat) {
      TwoDValLayerSpec* tdls = (TwoDValLayerSpec*)ls;
      int nx = tdls->twod.n_vals * 2;
      if(lay->unit_groups) {
        data_table->FindMakeColName
          (lay->name, lay_idx, DataTable::VT_FLOAT, 4, nx, 1,
           MAX(lay->gp_geom.x,1), MAX(lay->gp_geom.y,1));
      }
      else {
        data_table->FindMakeColName
          (lay->name, lay_idx, DataTable::VT_FLOAT, 2, nx, 1);
      }
    }
    else {
      if(lay->unit_groups) {
        data_table->FindMakeColName
          (lay->name, lay_idx, DataTable::VT_FLOAT, 4,
           MAX(lay->un_geom.x,1), MAX(lay->un_geom.y,1),
           MAX(lay->gp_geom.x,1), MAX(lay->gp_geom.y,1));
      }
      else {
        data_table->FindMakeColName
          (lay->name, lay_idx, DataTable::VT_FLOAT, 2,
           MAX(lay->un_geom.x,1), MAX(lay->un_geom.y,1));
      }
    }
  }
  data_table->StructUpdate(false);
//   if(taMisc::gui_active) {
//     tabMisc::DelayedFunCall_gui(data_table, "BrowserSelectMe");
//   }

  UpdateLayerWriters(net, data_table);

  return true;
}

///////////////////////////////////////////////////////////////
//                LeabraTI -- temporal integration wizard
///////////////////////////////////////////////////////////////

bool LeabraWizard::LeabraTI(LeabraNetwork* net) {
  if(TestError(!net, "LeabraTI", "must have basic constructed network first")) {
    return false;
  }
  FMSpec(LeabraConSpec, stdcons, net, "LeabraConSpec_0");
  FMChild(LeabraConSpec, ti_ctxt, stdcons, "LeabraTICtxt");
  FMSpec(LeabraUnitSpec, stduns, net, "LeabraUnitSpec_0");
  FMSpec(FullPrjnSpec, full_prjn, net, "FullPrjnSpec_0");

  net->net_misc.ti = true;

  ti_ctxt->SetUnique("wt_scale", true);
  ti_ctxt->wt_scale.rel = 1.0f;

  for(int li=net->layers.leaves-1; li >= 0; li--) {
    LeabraLayer* lay = (LeabraLayer*)net->layers.Leaf(li);
    if(lay->layer_type != Layer::HIDDEN) continue;

    LeabraLayerSpec* ls = (LeabraLayerSpec*)lay->GetLayerSpec();
    if(ls->InheritsFrom(&TA_LeabraContextLayerSpec)) continue; // skip existing srn's

    //	  	 	   to		 from		prjn_spec	con_spec
    net->FindMakePrjn(lay, lay, full_prjn,  ti_ctxt);
  }

  net->Build();
  return true;
}

///////////////////////////////////////////////////////////////
//                      SRN Context
///////////////////////////////////////////////////////////////

bool LeabraWizard::SRNContext(LeabraNetwork* net) {
  if(TestError(!net, "SRNContext", "must have basic constructed network first")) {
    return false;
  }
  FMSpec(OneToOnePrjnSpec, otop, net, "CtxtPrjn");
  FMSpec(LeabraContextLayerSpec, ctxts, net, "CtxtLayerSpec");

  if((otop == NULL) || (ctxts == NULL)) {
    return false;
  }

  LeabraLayer* hidden = (LeabraLayer*)net->FindLayer("Hidden");
  LeabraLayer* ctxt = (LeabraLayer*)net->FindMakeLayer("Context");

  if((hidden == NULL) || (ctxt == NULL)) return false;

  ctxt->SetLayerSpec(ctxts);
  ctxt->un_geom = hidden->un_geom;

  net->layers.MoveAfter(hidden, ctxt);
  net->FindMakePrjn(ctxt, hidden, otop); // one-to-one into the ctxt layer
  net->FindMakePrjn(hidden, ctxt);       // std prjn back into the hidden from context
  net->Build();
  return true;
}


///////////////////////////////////////////////////////////////
//                      TD
///////////////////////////////////////////////////////////////

// todo: set td_mod.on = true for td_mod_all; need to get UnitSpec..

bool LeabraWizard::TD(LeabraNetwork* net, bool bio_labels, bool td_mod_all) {
  if(!net) {
    LeabraProject* proj = GET_MY_OWNER(LeabraProject);
    net = (LeabraNetwork*)proj->GetNewNetwork();
    if(TestError(!net, "TD", "network is NULL and could not make a new one -- aborting!"))
      return false;
    if(!StdNetwork()) return false;
  }

  String msg = "Configuring TD Temporal Differences Layers:\n\n\
 There is one thing you will need to check manually after this automatic configuration\
 process completes (this note will be repeated when things complete --- there may be some\
 messages in the interim):\n\n";

  String man_msg = "1. Check that connection(s) were made from all appropriate output layers\
 to the ExtRew layer, using the MarkerConSpec (MarkerCons) Con spec.\
 This will provide the error signal to the system based on output error performance.\n\n";

  msg += man_msg + "\n\nThe configuration will now be checked and a number of default parameters\
 will be set.  If there are any actual errors which must be corrected before\
 the network will run, you will see a message to that effect --- you will then need to\
 re-run this configuration process to make sure everything is OK.  When you press\
 Re/New/Init on the control process these same checks will be performed, so you\
 can be sure everything is ok.";
  taMisc::Confirm(msg);

  net->RemoveUnits();

  //////////////////////////////////////////////////////////////////////////////////
  // make layers

  bool  tdrp_new = false;
  String tdrpnm = "ABL";  String tdintnm = "NAc";  String tddanm = "VTA";
  if(!bio_labels) {
    tdrpnm = "TDRewPred";    tdintnm = "TDRewInteg";    tddanm = "TD";
  }

  LeabraLayer* rew_targ_lay = (LeabraLayer*)net->FindMakeLayer("RewTarg");
  LeabraLayer* extrew = (LeabraLayer*)net->FindMakeLayer("ExtRew");
  LeabraLayer* tdrp = (LeabraLayer*)net->FindMakeLayer(tdrpnm, NULL, tdrp_new);
  LeabraLayer* tdint = (LeabraLayer*)net->FindMakeLayer(tdintnm);
  LeabraLayer* tdda = (LeabraLayer*)net->FindMakeLayer(tddanm);
  if(rew_targ_lay == NULL || tdrp == NULL || extrew == NULL || tdint == NULL || tdda == NULL) return false;
  if(tdrp_new) {
    extrew->pos.z = 0; extrew->pos.y = 4; extrew->pos.x = 0;
    tdrp->pos.z = 0; tdrp->pos.y = 2; tdrp->pos.x = 0;
    tdint->pos.z = 0; tdint->pos.y = 0; tdint->pos.x = 0;
    tdda->pos.z = 0; tdda->pos.y = 4; tdda->pos.x = 10;
  }

  //////////////////////////////////////////////////////////////////////////////////
  // sort layers

  rew_targ_lay->name = "0000";  extrew->name = "0001"; tdrp->name = "0002";
  tdint->name = "0003";  tdda->name = "0004";

  net->layers.Sort();

  rew_targ_lay->name = "RewTarg";  extrew->name = "ExtRew"; tdrp->name = tdrpnm;
  tdint->name = tdintnm;  tdda->name = tddanm;

  //////////////////////////////////////////////////////////////////////////////////
  // collect layer groups

  Layer_Group other_lays;
  Layer_Group hidden_lays;
  Layer_Group output_lays;
  Layer_Group input_lays;
  int i;
  for(i=0;i<net->layers.size;i++) {
    LeabraLayer* lay = (LeabraLayer*)net->layers[i];
    LeabraLayerSpec* laysp = (LeabraLayerSpec*)lay->spec.SPtr();
    lay->SetUnitType(&TA_LeabraTdUnit);
    // todo: add any new bg layer exclusions here!
    if(lay != rew_targ_lay && lay != tdrp && lay != extrew && lay != tdint && lay != tdda) {
      other_lays.Link(lay);
      if(lay->layer_type == Layer::HIDDEN)
        hidden_lays.Link(lay);
      else if(lay->layer_type == Layer::INPUT)
        input_lays.Link(lay);
      else
        output_lays.Link(lay);
      LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.SPtr();
      if(us == NULL || !us->InheritsFrom(TA_LeabraTdUnitSpec)) {
        us->ChangeMyType(&TA_LeabraTdUnitSpec);
      }
    }
  }

  //////////////////////////////////////////////////////////////////////////////////
  // make specs

  String gpprfx = "TD_";

  // todo: switch to single master-group here, like in PVLV etc
  BaseSpec_Group* units = net->FindMakeSpecGp(gpprfx + "Units");
  BaseSpec_Group* cons = net->FindMakeSpecGp(gpprfx + "Cons");
  BaseSpec_Group* layers = net->FindMakeSpecGp(gpprfx + "Layers");
  BaseSpec_Group* prjns = net->FindMakeSpecGp(gpprfx + "Prjns");
  if(units == NULL || cons == NULL || layers == NULL || prjns == NULL) return false;

  FMSpec(LeabraUnitSpec, rewpred_units, units, "TDRewPredUnits");
  FMSpec(LeabraUnitSpec, td_units, units, "TdUnits");

  FMSpec(LeabraConSpec, learn_cons, cons, "LearnCons");

  FMChild(TDRewPredConSpec, rewpred_cons, learn_cons, "TDRewPredCons");
  FMChild(LeabraBiasSpec, bg_bias, learn_cons, "BgBias");
  FMChild(LeabraBiasSpec, fixed_bias, bg_bias, "FixedBias");

  FMSpec(MarkerConSpec, marker_cons, cons, "MarkerCons");

  FMSpec(ExtRewLayerSpec, ersp, layers, "ExtRewLayer");
  FMSpec(TDRewPredLayerSpec, tdrpsp, layers, "Layer");
  FMSpec(TDRewIntegLayerSpec, tdintsp, layers, "Layer");
  FMSpec(TdLayerSpec, tdsp, layers, "Layer");

  FMSpec(ProjectionSpec, fullprjn, prjns, "FullPrjn");
  FMSpec(OneToOnePrjnSpec, onetoone, prjns, "OneToOne");

  //////////////////////////////////////////////////////////////////////////////////
  // set default spec parameters

  onetoone->send_start = 1;     // needed for new index based connections

  // learn_cons->lmix.hebb = .01f; // .01 hebb on learn cons
//   learn_cons->not_used_ok = true;
  learn_cons->UpdateAfterEdit();
  bg_bias->SetUnique("lrate", true);
  bg_bias->lrate = 0.0f;
  bg_bias->SetUnique("learn", true);
  bg_bias->learn = false;
  fixed_bias->SetUnique("lrate", true);
  fixed_bias->lrate = 0.0f;
  fixed_bias->SetUnique("learn", true);
  fixed_bias->learn = false;
  rewpred_cons->SetUnique("rnd", true);
  rewpred_cons->rnd.mean = 0.1f; rewpred_cons->rnd.var = 0.0f;
  rewpred_cons->SetUnique("wt_sig", true);
  rewpred_cons->wt_sig.gain = 1.0f;  rewpred_cons->wt_sig.off = 1.0f;
  // rewpred_cons->SetUnique("lmix", true);
  // rewpred_cons->lmix.hebb = 0.0f;

  if(output_lays.size > 0)
    ersp->rew_type = ExtRewLayerSpec::OUT_ERR_REW;
  else
    ersp->rew_type = ExtRewLayerSpec::EXT_REW;

  int n_rp_u = 22;              // number of rewpred-type units
  ScalarValLayerSpec* valspecs[3] = {tdrpsp, tdintsp, ersp};
  for(int i=0;i<2;i++) {
    ScalarValLayerSpec* lsp = valspecs[i];
    lsp->scalar.rep = ScalarValSpec::GAUSSIAN;
    lsp->scalar.min_sum_act = .2f;
    // lsp->inhib.type = LeabraInhibSpec::KWTA_INHIB; lsp->inhib.kwta_pt = 0.25f;
    // lsp->kwta.k_from = KWTASpec::USE_K;    lsp->kwta.k = 3;
    // lsp->gp_kwta.k_from = KWTASpec::USE_K; lsp->gp_kwta.k = 3;
    lsp->unit_range.min = -0.5f;  lsp->unit_range.max = 3.5f;
    lsp->unit_range.UpdateAfterEdit();
    lsp->val_range = lsp->unit_range;
  }
  ersp->unit_range.max = 1.5f;
  ersp->unit_range.UpdateAfterEdit();

  //////////////////////////////////////////////////////////////////////////////////
  // set geometries

  if(tdrp->un_geom.n != n_rp_u) { tdrp->un_geom.n = n_rp_u; tdrp->un_geom.x = n_rp_u; tdrp->un_geom.y = 1; }
  if(extrew->un_geom.n != 12) { extrew->un_geom.n = 12; extrew->un_geom.x = 12; extrew->un_geom.y = 1; }
  if(tdint->un_geom.n != n_rp_u) { tdint->un_geom.n = n_rp_u; tdint->un_geom.x = n_rp_u; tdint->un_geom.y = 1; }
  tdda->un_geom.n = 1;
  rew_targ_lay->un_geom.n = 1;

  //////////////////////////////////////////////////////////////////////////////////
  // apply specs to objects

  tdrp->SetLayerSpec(tdrpsp);   tdrp->SetUnitSpec(rewpred_units);
  extrew->SetLayerSpec(ersp);   extrew->SetUnitSpec(rewpred_units);
  tdint->SetLayerSpec(tdintsp); tdint->SetUnitSpec(rewpred_units);
  tdda->SetLayerSpec(tdsp);     tdda->SetUnitSpec(td_units);

  rewpred_units->bias_spec.SetSpec(bg_bias);
  td_units->bias_spec.SetSpec(fixed_bias);

  //////////////////////////////////////////////////////////////////////////////////
  // make projections

  // FindMakePrjn(Layer* recv, Layer* send,
  net->FindMakePrjn(extrew, rew_targ_lay, onetoone, marker_cons);
  net->FindMakePrjn(tdint, tdrp, onetoone, marker_cons);
  net->FindMakePrjn(tdda, tdint, onetoone, marker_cons);
  net->FindMakePrjn(tdint, extrew, onetoone, marker_cons);
  net->FindMakePrjn(tdrp, tdda, onetoone, marker_cons);

  for(i=0;i<other_lays.size;i++) {
    Layer* ol = (Layer*)other_lays[i];
    if(tdrp_new)
      net->FindMakePrjn(tdrp, ol, fullprjn, rewpred_cons);
    if(td_mod_all)
      net->FindMakePrjn(ol, tdda, fullprjn, marker_cons);
  }

  for(i=0;i<output_lays.size;i++) {
    Layer* ol = (Layer*)output_lays[i];
    net->FindMakePrjn(extrew, ol, onetoone, marker_cons);
  }

  //////////////////////////////////////////////////////////////////////////////////
  // build and check

  net->Build();

  bool ok = tdrpsp->CheckConfig_Layer(tdrp, false) && tdintsp->CheckConfig_Layer(tdint, false)
    && tdsp->CheckConfig_Layer(tdda, false) && ersp->CheckConfig_Layer(extrew, false);

  if(!ok) {
    msg =
      "TD: An error in the configuration has occurred (it should be the last message\
 you received prior to this one).  The network will not run until this is fixed.\
 In addition, the configuration process may not be complete, so you should run this\
 function again after you have corrected the source of the error.";
  }
  else {
    msg =
    "TD configuration is now complete.  Do not forget the one remaining thing\
 you need to do manually:\n\n" + man_msg;
  }
  taMisc::Confirm(msg);

  tdrpsp->UpdateAfterEdit();
  ersp->UpdateAfterEdit();
  tdintsp->UpdateAfterEdit();

  for(int j=0;j<net->specs.leaves;j++) {
    BaseSpec* sp = (BaseSpec*)net->specs.Leaf(j);
    sp->UpdateAfterEdit();
  }

  // todo: !!!
//   winbMisc::DelayedMenuUpdate(net);

  //////////////////////////////////////////////////////////////////////////////////
  // select edit

  LeabraProject* proj = GET_MY_OWNER(LeabraProject);
  ControlPanel* edit = proj->FindMakeControlPanel("TD");
  if(edit != NULL) {
    rewpred_cons->AddToControlPanelNm("lrate", edit, "rewpred");
    ersp->AddToControlPanelNm("rew", edit, "extrew");
    tdrpsp->AddToControlPanelNm("rew_pred", edit, "tdrp");
    tdintsp->AddToControlPanelNm("rew_integ", edit, "tdint");
  }
  return true;
}

///////////////////////////////////////////////////////////////
//                      PVLV
///////////////////////////////////////////////////////////////

// this is how to access a pvlv spec of a given type, by name:
#define PvlvSp(pth,T) ((T*)pvlvspgp->ElemPath(pth, T::StatTypeDef(0), true))

bool LeabraWizard::PVLV_Specs(LeabraNetwork* net) {
  if(!net) {
    if(TestError(!net, "PVLV_Specs", "network is NULL -- only makes sense to run on an existing network -- aborting!"))
      return false;
  }

  String pvlvprefix = "PVLV";
  BaseSpec_Group* pvlvspgp = net->FindMakeSpecGp(pvlvprefix);
  if(!pvlvspgp) return false;

  FMSpec(LeabraUnitSpec, pvlv_units, pvlvspgp, "PVLVUnits");
  FMChild(LearnModUnitSpec, pv_units, pvlv_units, "PVUnits");
  FMChild(LHbRMTgUnitSpec, lhbrmtg_units, pvlv_units, "LHbRMTgUnits");
  FMChild(PPTgUnitSpec, pptg_units, pvlv_units, "PPTgUnits");
  FMChild(VTAUnitSpec, vta_units, pvlv_units, "VTAUnits");
  FMChild(DRNUnitSpec, drn_units, pvlv_units, "DRNUnits");
  FMChild(LeabraUnitSpec, cem_units, pvlv_units, "CeMUnits");
  FMChild(LeabraUnitSpec, bla_units, pvlv_units, "BLAUnits");
  FMChild(VSPatchUnitSpec, vspd_units, pvlv_units, "VSPatchDirectUnits");
  FMChild(VSPatchUnitSpec, vspi_units, pvlv_units, "VSPatchIndirUnits");
  FMChild(LeabraUnitSpec, vsmd_units, pvlv_units, "VSMatrixDirectUnits");
  FMChild(LeabraUnitSpec, vsmi_units, pvlv_units, "VSMatrixIndirUnits");

  FMSpec(LeabraConSpec, pvlv_cons, pvlvspgp, "PVLVLrnCons");
  FMChild(LeabraBiasSpec, lrnbias_cons, pvlv_cons, "PVLVLearnBias");
  FMChild(LearnModDeltaConSpec, lrnfmpv_cons, pvlv_cons, "LearnFromPV");
  FMChild(LearnModHebbConSpec, lrnfmpv_hebb_cons, pvlv_cons, "LearnFromPVHebb");
  FMChild(LeabraDeltaConSpec, vspatch_cons, pvlv_cons, "VSPatchCons");
  FMChild(LeabraDeltaConSpec, vsmatrix_cons, pvlv_cons, "VSMatrixCons");

  FMSpec(LeabraConSpec, fix_cons, pvlvspgp, "PVLVFixedCons");
  FMChild(LeabraBiasSpec, fix_bias, fix_cons, "PVLVFixedBias");
  FMChild(LeabraConSpec, fix_strong, fix_cons, "PVLVFixedStrong");

  FMSpec(MarkerConSpec, marker_con, pvlvspgp, "PVLVMarkerCons");

  FMSpec(LeabraLayerSpec, laysp, pvlvspgp, "PVLVLayers");
  FMChild(LeabraLayerSpec, pvsp, laysp, "PVLayers");
  FMChild(LeabraLayerSpec, dasp, laysp, "DALayers");
  FMChild(LeabraLayerSpec, amgysp, laysp, "AmygLayer");
  FMChild(LeabraLayerSpec, vspsp, laysp, "VSPatchLayer");
  FMChild(LeabraLayerSpec, vsmsp, laysp, "VSMatrixLayer");
  FMChild(ExtRewLayerSpec, ersp, laysp, "ExtRewLayer");

  FMSpec(FullPrjnSpec, fullprjn, pvlvspgp, "PVLVFullPrjn");
  FMSpec(OneToOnePrjnSpec, onetoone, pvlvspgp, "PVLVOneToOne");
  FMSpec(GpOneToOnePrjnSpec, gponetoone, pvlvspgp, "PVLVGpOneToOne");
  FMSpec(TesselPrjnSpec, pv_fm_er, pvlvspgp, "PVFmExtRew");

  //////////////////////////////////////////////////////////////////////////////////
  // set default spec parameters

  pvlvspgp->Defaults();

  //////  Units
  pvlv_units->UpdateAfterEdit();
  pvlv_units->bias_spec.SetSpec(fix_bias);

  cem_units->SetUnique("da_mod", true);
  cem_units->da_mod.on = true;
  cem_units->da_mod.minus = 0.0f;
  cem_units->da_mod.plus = 0.0f; // todo: could add da modulation..

  bla_units->SetUnique("da_mod", true);
  bla_units->da_mod.on = true;
  bla_units->da_mod.minus = 0.0f;
  bla_units->da_mod.plus = 0.0f; // todo: could add da modulation..

  // todo: could add vspatch da mod

  vsmd_units->SetUnique("da_mod", true);
  vsmd_units->da_mod.on = true;
  vsmd_units->da_mod.minus = 0.0f;
  vsmd_units->da_mod.plus = 0.01f;

  vsmi_units->SetUnique("da_mod", true);
  vsmi_units->da_mod.on = true;
  vsmi_units->da_mod.minus = 0.0f;
  vsmi_units->da_mod.plus = -0.01f; // nogo / opposite sign

  //////  Cons
  pvlv_cons->UpdateAfterEdit();
  pvlv_cons->lrate = 0.002f;    // best for pbwm
  pvlv_cons->xcal.raw_l_mix = true;
  pvlv_cons->xcal.thr_l_mix = 0.0f; // no hebbian at all..
  pvlv_cons->rnd.mean = 0.01f;
  pvlv_cons->rnd.var = 0.0f;
  pvlv_cons->wt_limits.sym = false;

  vspatch_cons->SetUnique("wt_scale", true);
  vspatch_cons->wt_scale.abs = 4.0f;
  vsmatrix_cons->SetUnique("wt_scale", true);
  vsmatrix_cons->wt_scale.abs = 4.0f;

  fix_cons->UpdateAfterEdit();
  fix_cons->xcal.raw_l_mix = true;
  fix_cons->xcal.thr_l_mix = 0.0f; // no hebbian at all..
  fix_cons->learn = false;
  fix_cons->rnd.mean = 0.9f;
  fix_cons->rnd.var = 0.0f;
  fix_cons->wt_limits.sym = false;
  fix_cons->lrate = 0.0f;

  fix_strong->SetUnique("wt_scale", true);
  fix_strong->wt_scale.abs = 3.0f;

  ////// Layers
  laysp->UpdateAfterEdit();
  laysp->lay_inhib.gi = 1.0f;
  laysp->lay_inhib.ff = 0.8f;
  laysp->lay_inhib.fb = 0.0f;
  laysp->avg_act.init = 0.25f;
  laysp->avg_act.fixed = true;
  laysp->inhib_misc.self_fb = 0.3f;
  laysp->decay.event = 0.0f;

  vsmsp->SetUnique("del_inhib", true);
  vsmsp->del_inhib.on = true;
  vsmsp->del_inhib.prv_trl = 4.0f;
  vsmsp->del_inhib.prv_q = 0.0f;

  ersp->unit_range.min = 0.0f;
  ersp->unit_range.max = 1.0f;
  ersp->rew_type = ExtRewLayerSpec::OUT_ERR_REW;

  /////// Prjns
  
  pv_fm_er->MakeRectangle(1,1,0,0);
  pv_fm_er->send_off.x = 3.0;
  pv_fm_er->wrap = false;

  //////////////////
  // Update Everyone

  pvlv_units->UpdateAfterEdit();
  pvlv_cons->UpdateAfterEdit();
  laysp->UpdateAfterEdit();

  for(int j=0;j<pvlvspgp->leaves;j++) {
    BaseSpec* sp = (BaseSpec*)pvlvspgp->Leaf(j);
    sp->UpdateAfterEdit();
  }

  //////////////////////////////////////////////////////////////////////////////////
  // control panel

  LeabraProject* proj = GET_MY_OWNER(LeabraProject);
  ControlPanel* cp = proj->FindMakeControlPanel("PVLV");
  if(cp) {
     // cp->SetUserData("user_pinned", true);

    pvlv_cons->AddToControlPanelNm("lrate", cp, "pvlv");
    vsmd_units->AddToControlPanelNm("da_mod", cp, "vs_matrix_dir");
    vsmi_units->AddToControlPanelNm("da_mod", cp, "vs_matrix_ind");
    
    cp->EditPanel(true, true);
  }

  return true;
}

// todo: set td_mod.on = true for td_mod_all; need to get UnitSpec..

bool LeabraWizard::PVLV(LeabraNetwork* net, int n_pos_pv, int n_neg_pv, bool da_mod_all) {
  LeabraProject* proj = GET_MY_OWNER(LeabraProject);
  if(!net) {
    net = (LeabraNetwork*)proj->GetNewNetwork();
    if(TestError(!net, "PVLV", "network is NULL and could not make a new one -- aborting!"))
      return false;
    if(!StdNetwork()) return false;
  }
  if(proj) {
    proj->undo_mgr.SaveUndo(net, "Wizard::PVLV before -- actually saves network specifically");
  }

  String msg = "Configuring Pavlov (PVLV) Layers:\n\n";
 // There is one thing you will need to check manually after this automatic configuration\
 // process completes (this note will be repeated when things complete --- there may be some\
 // messages in the interim):\n\n";

 //  String man_msg = "1. Check that connection(s) were made from all appropriate output layers\
 // to the PVe (ExtRewLayerSpec) layer, using the MarkerConSpec (MarkerCons) Con spec.\
 // This will provide the error signal to the system based on output error performance.\n\n";

  msg += "\n\nThe configuration will now be checked and a number of default parameters\
 will be set.  If there are any actual errors which must be corrected before\
 the network will run, you will see a message to that effect --- you will then need to\
 re-run this configuration process to make sure everything is OK.  When you press\
 Re/New/Init on the control process these same checks will be performed, so you\
 can be sure everything is ok.";
  taMisc::Confirm(msg);

  net->RemoveUnits();

  //////////////////////////////////////////////////////////////////////////////////
  // make layers

  bool new_laygp = false;
  Layer_Group* pv_gp = net->FindMakeLayerGroup("PVLV_PV", NULL, new_laygp);
  Layer_Group* amyg_gp = net->FindMakeLayerGroup("PVLV_Amyg", NULL, new_laygp);
  Layer_Group* vs_gp = net->FindMakeLayerGroup("PVLV_VS", NULL, new_laygp);
  Layer_Group* da_gp = net->FindMakeLayerGroup("PVLV_DA", NULL, new_laygp);

  bool new_pv = false;
  LeabraLayer* pos_pv = (LeabraLayer*)pv_gp->FindMakeLayer("PosPV", NULL, new_pv);
  LeabraLayer* neg_pv = (LeabraLayer*)pv_gp->FindMakeLayer("NegPV");
  LeabraLayer* pos_bs = (LeabraLayer*)pv_gp->FindMakeLayer("PosBodyState");
  LeabraLayer* neg_bs = (LeabraLayer*)pv_gp->FindMakeLayer("NegBodyState");
  LeabraLayer* rew_targ = (LeabraLayer*)pv_gp->FindMakeLayer("RewTarg");
  LeabraLayer* ext_rew = (LeabraLayer*)pv_gp->FindMakeLayer("ExtRew");
  pos_pv->layer_type = Layer::INPUT;
  neg_pv->layer_type = Layer::INPUT;
  pos_bs->layer_type = Layer::INPUT;
  neg_bs->layer_type = Layer::INPUT;
  ext_rew->layer_type = Layer::INPUT;

  bool new_amyg = false;
  LeabraLayer* poslv_cem = (LeabraLayer*)amyg_gp->FindMakeLayer("PosLV_CeM", NULL,
                                                                new_amyg);
  LeabraLayer* poslv_bla = (LeabraLayer*)amyg_gp->FindMakeLayer("PosLV_BLA");
  LeabraLayer* neglv_bla = (LeabraLayer*)amyg_gp->FindMakeLayer("NegLV_BLA");

  bool new_vs = false;
  LeabraLayer* vspi = (LeabraLayer*)vs_gp->FindMakeLayer("VSPatchIndir_Pos", NULL, new_vs);
  LeabraLayer* vspd = (LeabraLayer*)vs_gp->FindMakeLayer("VSPatchDirect_Neg");
  LeabraLayer* vsmd = (LeabraLayer*)vs_gp->FindMakeLayer("VSMatrixDirect_Pos");
  LeabraLayer* vsmi = (LeabraLayer*)vs_gp->FindMakeLayer("VSMatrixIndir_Neg");

  bool new_da = false;
  LeabraLayer* pptg = (LeabraLayer*)da_gp->FindMakeLayer("PPTg", NULL, new_da);
  LeabraLayer* lhb = (LeabraLayer*)da_gp->FindMakeLayer("LHbRMTg");
  LeabraLayer* vta = (LeabraLayer*)da_gp->FindMakeLayer("VTA");
  LeabraLayer* drn = (LeabraLayer*)da_gp->FindMakeLayer("DRN");
 
  //////////////////////////////////////////////////////////////////////////////////
  // collect layer groups

  Layer_Group other_lays;
  Layer_Group hidden_lays;
  Layer_Group output_lays;
  Layer_Group input_lays;
  int i;
  for(i=0;i<net->layers.leaves;i++) {
    LeabraLayer* lay = (LeabraLayer*)net->layers.Leaf(i);
    LeabraLayerSpec* laysp = (LeabraLayerSpec*)lay->spec.SPtr();
    if(lay->owner->GetName().contains("PVLV_")) continue;
    other_lays.Link(lay);
    if(lay->pos.z == 0) lay->pos.z = 2; // nobody allowed in 0!
    if(lay->layer_type == Layer::HIDDEN)
      hidden_lays.Link(lay);
    else if((lay->layer_type == Layer::INPUT) || lay->name.contains("In"))
      input_lays.Link(lay);
    else if(lay->name.contains("Out"))
      output_lays.Link(lay);
    else
      input_lays.Link(lay);   // default to input -- many are now TARGET in TI
  }

  //////////////////////////////////////////////////////////////////////////////////
  // make specs

  if(!PVLV_Specs(net)) return false;

  String pvlvprefix = "PVLV";
  BaseSpec_Group* pvlvspgp = net->FindMakeSpecGp(pvlvprefix);

//   if(output_lays.size > 0)
//     PvlvSp("PVeLayer",ExtRewLayerSpec)->rew_type = ExtRewLayerSpec::OUT_ERR_REW;
//   else
//     PvlvSp("PVeLayer",ExtRewLayerSpec)->rew_type = ExtRewLayerSpec::EXT_REW;

  //////////////////////////////////////////////////////////////////////////////////
  // set positions & geometries

  // todo: see about better options here..
  pos_pv->brain_area = ".*/.*/.*/.*/Lateral Hypothalamic area LHA";
  neg_pv->brain_area = ".*/.*/.*/.*/Lateral Hypothalamic area LHA";
  pos_bs->brain_area = ".*/.*/.*/.*/Lateral Hypothalamic area LHA";
  neg_bs->brain_area = ".*/.*/.*/.*/Lateral Hypothalamic area LHA";
  ext_rew->brain_area = ".*/.*/.*/.*/Lateral Hypothalamic area LHA";
  rew_targ->brain_area = ".*/.*/.*/.*/Lateral Hypothalamic area LHA";

  poslv_cem->brain_area = ".*/.*/.*/.*/Amygdala Central Nucleus CNA";
  poslv_bla->brain_area = ".*/.*/.*/.*/Amygdala";
  neglv_bla->brain_area = ".*/.*/.*/.*/Amygdala";

  vspd->brain_area = ".*/.*/.*/.*/Nucleus Accumbens NAc";
  vspi->brain_area = ".*/.*/.*/.*/Nucleus Accumbens NAc";
  vsmd->brain_area = ".*/.*/.*/.*/Nucleus Accumbens NAc";
  vsmi->brain_area = ".*/.*/.*/.*/Nucleus Accumbens NAc";

  pptg->brain_area = ".*/.*/.*/.*/Ventral Tegmental Area VTA"; // todo
  lhb->brain_area = ".*/.*/.*/.*/Lateral Habenula LHB";
  vta->brain_area = ".*/.*/.*/.*/Ventral Tegmental Area VTA";
  drn->brain_area = ".*/.*/.*/.*/Ventral Tegmental Area VTA"; // todo

  int sp = 2;
  int neg_st = n_pos_pv * 2 + sp;
  int da_st = neg_st + n_neg_pv * 2 + sp;

  if(new_pv) {
    pv_gp->pos.SetXYZ(0,0,0);
    pos_pv->pos.SetXYZ(0,0,0);
    neg_pv->pos.SetXYZ(neg_st,0,0);
    pos_bs->pos.SetXYZ(0,sp,0);
    neg_bs->pos.SetXYZ(neg_st,sp,0);
    ext_rew->pos.SetXYZ(da_st,sp,0);
    rew_targ->pos.SetXYZ(da_st + 6,sp,0);
  }

  if(new_amyg) {
    amyg_gp->pos.SetXYZ(0, 0, 0);
    poslv_cem->pos.SetXYZ(0,2*sp,0);
    poslv_bla->pos.SetXYZ(0,3*sp,0);
    neglv_bla->pos.SetXYZ(neg_st,3*sp,0);
  }

  if(new_vs) {
    vs_gp->pos.SetXYZ(da_st, 0, 0);
    vspi->pos.SetXYZ(0, 2*sp, 0);
    vspd->pos.SetXYZ(0, 3*sp, 0);

    vsmd->pos.SetXYZ(neg_st, 2*sp, 0);
    vsmi->pos.SetXYZ(neg_st, 3*sp, 0);
  }

  if(new_da) { 
    da_gp->pos.z = 0;
    pptg->pos.SetXYZ(da_st, 0, 0);
    vta->pos.SetXYZ(da_st + 1 + sp, 0, 0);
    lhb->pos.SetXYZ(da_st + 2*(sp+1), 0, 0);
    drn->pos.SetXYZ(da_st + 3*(sp+1), 0, 0);
  }

  pos_pv->un_geom.SetXYN(1,1,1);
  pos_pv->gp_geom.SetXY(n_pos_pv, 1);  pos_pv->unit_groups = true;

  neg_pv->un_geom.SetXYN(1,1,1);
  neg_pv->gp_geom.SetXY(n_neg_pv, 1);  neg_pv->unit_groups = true;

  pos_bs->un_geom.SetXYN(1,1,1);
  pos_bs->gp_geom.SetXY(n_pos_pv, 1);  pos_bs->unit_groups = true;

  neg_bs->un_geom.SetXYN(1,1,1);
  neg_bs->gp_geom.SetXY(n_neg_pv, 1);  neg_bs->unit_groups = true;

  ext_rew->un_geom.SetXYN(4,1,4);
  rew_targ->un_geom.SetXYN(4,1,4);

  poslv_cem->un_geom.SetXYN(1,1,1);
  poslv_cem->gp_geom.SetXY(n_pos_pv, 1);  poslv_cem->unit_groups = true;

  poslv_bla->un_geom.SetXYN(1,1,1);
  poslv_bla->gp_geom.SetXY(n_pos_pv, 1);  poslv_bla->unit_groups = true;

  neglv_bla->un_geom.SetXYN(1,1,1);
  neglv_bla->gp_geom.SetXY(n_neg_pv, 1);  neglv_bla->unit_groups = true;

  vspi->un_geom.SetXYN(1,1,1);
  vspi->gp_geom.SetXY(n_pos_pv, 1);  vspi->unit_groups = true;

  vsmd->un_geom.SetXYN(1,1,1);
  vsmd->gp_geom.SetXY(n_pos_pv, 1);  vsmd->unit_groups = true;

  vspd->un_geom.SetXYN(1,1,1);
  vspd->gp_geom.SetXY(n_neg_pv, 1);  vspd->unit_groups = true;

  vsmi->un_geom.SetXYN(1,1,1);
  vsmi->gp_geom.SetXY(n_neg_pv, 1);  vsmi->unit_groups = true;

  pptg->un_geom.SetXYN(1,1,1);
  lhb->un_geom.SetXYN(1,1,1);
  vta->un_geom.SetXYN(1,1,1);
  drn->un_geom.SetXYN(1,1,1);

  //////////////////////////////////////////////////////////////////////////////////
  // apply specs to objects

  LeabraUnitSpec* pvlv_units = PvlvSp("PVLVUnits",LeabraUnitSpec);
  LearnModUnitSpec* pv_units = PvlvSp("PVUnits",LearnModUnitSpec);
  pos_pv->SetUnitSpec(pv_units);
  neg_pv->SetUnitSpec(pv_units);
  pos_bs->SetUnitSpec(pv_units);
  neg_bs->SetUnitSpec(pv_units);

  LeabraLayerSpec* pvsp = PvlvSp("PVLayers", LeabraLayerSpec);
  pos_pv->SetLayerSpec(pvsp);
  neg_pv->SetLayerSpec(pvsp);
  pos_bs->SetLayerSpec(pvsp);
  neg_bs->SetLayerSpec(pvsp);

  poslv_cem->SetUnitSpec(PvlvSp("CeMUnits", LeabraUnitSpec));
  poslv_bla->SetUnitSpec(PvlvSp("BLAUnits", LeabraUnitSpec));
  neglv_bla->SetUnitSpec(PvlvSp("BLAUnits", LeabraUnitSpec));

  LeabraLayerSpec* amygsp = PvlvSp("AmygLayer", LeabraLayerSpec);
  poslv_cem->SetLayerSpec(amygsp);
  poslv_bla->SetLayerSpec(amygsp);
  neglv_bla->SetLayerSpec(amygsp);

  vspi->SetUnitSpec(PvlvSp("VSPatchIndirUnits", VSPatchUnitSpec));
  vspd->SetUnitSpec(PvlvSp("VSPatchDirectUnits", VSPatchUnitSpec));
  vsmi->SetUnitSpec(PvlvSp("VSMatrixIndirUnits", LeabraUnitSpec));
  vsmd->SetUnitSpec(PvlvSp("VSPatchDirectUnits", LeabraUnitSpec));

  vspi->SetLayerSpec(PvlvSp("VSPatchLayer", LeabraLayerSpec));
  vspd->SetLayerSpec(PvlvSp("VSPatchLayer", LeabraLayerSpec));
  vsmi->SetLayerSpec(PvlvSp("VSMatrixLayer", LeabraLayerSpec));
  vsmd->SetLayerSpec(PvlvSp("VSMatrixLayer", LeabraLayerSpec));

  LeabraLayerSpec* dasp = PvlvSp("DALayers", LeabraLayerSpec);
  pptg->SetUnitSpec(PvlvSp("PPTgUnits", PPTgUnitSpec));
  pptg->SetLayerSpec(dasp);

  lhb->SetUnitSpec(PvlvSp("LHbRMTgUnits", LHbRMTgUnitSpec));
  lhb->SetLayerSpec(dasp);

  vta->SetUnitSpec(PvlvSp("VTAUnits", VTAUnitSpec));
  vta->SetLayerSpec(dasp);

  drn->SetUnitSpec(PvlvSp("DRNUnits", DRNUnitSpec));
  drn->SetLayerSpec(dasp);

  //////////////////////////////////////////////////////////////////////////////////
  // make projections

  MarkerConSpec* marker_cons = PvlvSp("PVLVMarkerCons", MarkerConSpec);
  LeabraConSpec* fix_cons = PvlvSp("PVLVFixedCons", LeabraConSpec);
  OneToOnePrjnSpec* onetoone = PvlvSp("PVLVOneToOne", OneToOnePrjnSpec);
  GpOneToOnePrjnSpec* gponetoone = PvlvSp("PVLVGpOneToOne", GpOneToOnePrjnSpec);
  FullPrjnSpec* fullprjn = PvlvSp("PVLVFullPrjn", FullPrjnSpec);

  // net->FindMakePrjn(Layer* recv, Layer* send, prjn, conspec)

  // all basic PV driver projections:
  net->FindMakePrjn(poslv_cem, pos_pv, gponetoone, fix_cons);
  net->FindMakePrjn(poslv_bla, pos_pv, gponetoone, fix_cons);
  net->FindMakePrjn(neglv_bla, neg_pv, gponetoone, fix_cons);

  // user can lesion this if they want..
  net->FindMakePrjn(pos_pv, ext_rew, PvlvSp("PVFmExtRew", TesselPrjnSpec), fix_cons);

  net->FindMakePrjn(vspi, pos_pv, gponetoone, marker_cons);
  net->FindMakePrjn(vspd, neg_pv, gponetoone, marker_cons);
  net->FindMakePrjn(vsmd, poslv_bla, gponetoone, fix_cons);
  net->FindMakePrjn(vsmi, neglv_bla, gponetoone, fix_cons);

  // patch gets both direct PV drivers and bla-drivers..?
  LeabraDeltaConSpec* vsp_cons = PvlvSp("VSPatchCons", LeabraDeltaConSpec);
  net->FindMakePrjn(vspi, poslv_bla, gponetoone, vsp_cons);
  net->FindMakePrjn(vsmi, neglv_bla, gponetoone, vsp_cons);

  // vta neuromodulation -- todo: add bla, cem??
  net->FindMakePrjn(vspi, vta, fullprjn, marker_cons);
  net->FindMakePrjn(vspd, vta, fullprjn, marker_cons);
  net->FindMakePrjn(vsmd, vta, fullprjn, marker_cons);
  net->FindMakePrjn(vsmi, vta, fullprjn, marker_cons);

  LearnModDeltaConSpec* lfmpv_cons = PvlvSp("LearnFromPV", LearnModDeltaConSpec);
  LeabraDeltaConSpec* vsm_cons = PvlvSp("VSMatrixCons", LeabraDeltaConSpec);
  for(i=0;i<input_lays.size;i++) {
    Layer* il = (Layer*)input_lays[i];
    net->FindMakePrjn(poslv_cem, il, fullprjn, lfmpv_cons);
    net->FindMakePrjn(poslv_bla, il, fullprjn, lfmpv_cons);
    net->FindMakePrjn(neglv_bla, il, fullprjn, lfmpv_cons);

    net->FindMakePrjn(vspi, il, fullprjn, vsp_cons);
    net->FindMakePrjn(vspd, il, fullprjn, vsp_cons);
    net->FindMakePrjn(vsmd, il, fullprjn, vsm_cons);
    net->FindMakePrjn(vsmi, il, fullprjn, vsm_cons);
  }

  // core da nuclei
  net->FindMakePrjn(pptg, poslv_cem, fullprjn, fix_cons);

  net->FindMakePrjn(lhb, vsmd, fullprjn, fix_cons);
  net->FindMakePrjn(lhb, vsmi, fullprjn, fix_cons);
  net->FindMakePrjn(lhb, vspd, fullprjn, fix_cons);
  net->FindMakePrjn(lhb, vspi, fullprjn, fix_cons);
  net->FindMakePrjn(lhb, pos_pv, fullprjn, fix_cons);
  net->FindMakePrjn(lhb, neg_pv, fullprjn, fix_cons);
  
  net->FindMakePrjn(vta, pptg, fullprjn, marker_cons);
  net->FindMakePrjn(vta, lhb, fullprjn, marker_cons);
  net->FindMakePrjn(vta, pos_pv, fullprjn, marker_cons);
  net->FindMakePrjn(vta, vspi, fullprjn, marker_cons);

  net->FindMakePrjn(drn, pos_pv, fullprjn, marker_cons);
  net->FindMakePrjn(drn, neg_pv, fullprjn, marker_cons);

//   if(da_mod_all) {
//     for(i=0;i<other_lays.size;i++) {
//       Layer* ol = (Layer*)other_lays[i];
//       net->FindMakePrjn(ol, vta, fullprjn, marker_cons);
//       //net->FindMakePrjn(ol, vta, vtaonetoone, marker_cons);
//     }
//   }

//   for(i=0;i<output_lays.size;i++) {
//     Layer* ol = (Layer*)output_lays[i];
//     net->FindMakePrjn(pve, ol, onetoone, marker_cons);
//   }

  //////////////////////////////////////////////////////////////////////////////////
  // build and check

  net->Build();
  net->LayerPos_Cleanup();

//   if(new_laygp) {
//     laygp->pos.z = 0;           // move back!
//     net->RebuildAllViews();     // trigger update
//   }

  bool ok = net->CheckConfig();

  if(!ok) {
    msg =
      "PVLV: An error in the configuration has occurred (it should be the last message\
 you received prior to this one).  The network will not run until this is fixed.\
 In addition, the configuration process may not be complete, so you should run this\
 function again after you have corrected the source of the error.";
  }
  else {
    msg =
      "PVLV configuration is now complete.\n";
// "  Do not forget the one remaining thing \
//  you need to do manually:\n\n" + man_msg;
  }
  taMisc::Confirm(msg);

  if(proj) {
    proj->undo_mgr.SaveUndo(net, "Wizard::PVLV after -- actually saves network specifically");
  }
  return true;
}

bool LeabraWizard::PVLV_ConnectLayer(LeabraNetwork* net, LeabraLayer* sending_layer,
                                      bool disconnect) {
//   if(TestError(!net || !sending_layer, "PVLV_ConnectLayer", "must specify a network and a sending layer!")) return false;

//   LeabraProject* proj = GET_MY_OWNER(LeabraProject);
//   if(proj) {
//     proj->undo_mgr.SaveUndo(net, "Wizard::PVLV_ConnectLayer before -- actually saves network specifically");
//   }

//   // String pvenm = "PVe";
//   String pvinm = "PVi";  String pvrnm = "PVr";
//   String lvenm = "LVe";  String lvinm = "LVi";  String nvnm = "NV";

// //   LeabraLayer* pve = (LeabraLayer*)net->FindLayer(pvenm);
//   LeabraLayer* pvr = (LeabraLayer*)net->FindLayer(pvrnm);
//   LeabraLayer* pvi = (LeabraLayer*)net->FindLayer(pvinm);
//   LeabraLayer* lve = (LeabraLayer*)net->FindLayer(lvenm);
//   LeabraLayer* lvi = (LeabraLayer*)net->FindLayer(lvinm);
//   LeabraLayer* nv =  (LeabraLayer*)net->FindLayer(nvnm);

//   String pvlvprefix = "PVLV";
//   BaseSpec_Group* pvlvspgp = net->FindMakeSpecGp(pvlvprefix);

//   FullPrjnSpec* fullprjn = PvlvSp("PvlvFullPrjn", FullPrjnSpec);
//   if(TestError(!fullprjn, "PVLV_ConnectLayer",
//                "fullprjn not found -- PVLV was not properly configured -- everything has been updated since version 6.4.0, so you need to reconfigure using the PVLV wizard")) {
//     return false;
//   }

//   if(disconnect) {
//     if(pvr)
//       net->RemovePrjn(pvr, sending_layer);
//     if(pvi)
//       net->RemovePrjn(pvi, sending_layer);
//     if(lve)
//       net->RemovePrjn(lve, sending_layer);
//     if(lvi)
//       net->RemovePrjn(lvi, sending_layer);
//     if(nv)
//       net->RemovePrjn(nv,  sending_layer);
//   }
//   else {
//     if(pvr)
//       net->FindMakePrjn(pvr, sending_layer, fullprjn, PvlvSp("PVrCons", PVrConSpec));
//     if(pvi)
//       net->FindMakePrjn(pvi, sending_layer, fullprjn, PvlvSp("PViCons", PVConSpec));
//     if(lve)
//       net->FindMakePrjn(lve, sending_layer, fullprjn, PvlvSp("LVeCons", PVConSpec));
//     // if(lvi && lvi_cons)
//     //   net->FindMakePrjn(lvi, sending_layer, fullprjn, lvi_cons);
//     if(nv)
//       net->FindMakePrjn(nv,  sending_layer, fullprjn, PvlvSp("NVCons", PVConSpec));
//   }
//   if(proj) {
//     proj->undo_mgr.SaveUndo(net, "Wizard::PVLV_ConnectLayer before -- actually saves network specifically");
//   }
  return true;
}

bool LeabraWizard::PVLV_OutToPVe(LeabraNetwork* net, LeabraLayer* output_layer,
                                     bool disconnect) {
  if(TestError(!net || !output_layer, "PVLV_OutToPVe", "must specify a network and an output layer!")) return false;

  LeabraProject* proj = GET_MY_OWNER(LeabraProject);
  if(proj) {
    proj->undo_mgr.SaveUndo(net, "Wizard::PVLV_OutToPVe before -- actually saves network specifically");
  }

  // String pvenm = "PVe";
  // LeabraLayer* pve = (LeabraLayer*)net->FindLayer(pvenm);

  // String pvlvprefix = "PVLV";
  // BaseSpec_Group* pvlvspgp = net->FindMakeSpecGp(pvlvprefix);

  // MarkerConSpec* marker_cons = PvlvSp("PvlvMarker", MarkerConSpec);
  // OneToOnePrjnSpec* onetoone = PvlvSp("PvlvOneToOne", OneToOnePrjnSpec);
  // if(TestError(!marker_cons, "PVLV_OutToPVe",
  //              "marker_cons not found -- PVLV was not properly configured -- everything has been updated since version 6.4.0, so you need to reconfigure using the PVLV wizard")) {
  //   return false;
  // }

  // if(disconnect) {
  //   net->RemovePrjn(pve, output_layer);
  // }
  // else {
  //   net->FindMakePrjn(pve, output_layer, onetoone, marker_cons);
  // }
  // if(proj) {
  //   proj->undo_mgr.SaveUndo(net, "Wizard::PVLV_OutToPVe after -- actually saves network specifically");
  // }
  return true;
}


///////////////////////////////////////////////////////////////
//                      PBWM
///////////////////////////////////////////////////////////////

bool LeabraWizard::PBWM_Defaults(LeabraNetwork* net, const String& prefix) {
  if(!net) {
    if(TestError(!net, "PBWM", "network is NULL -- must be passed and already PBWM configured -- aborting!"))
      return false;
  }

  return PBWM_Specs(net, prefix, true); // true = set defaults
}

// this is how to access a pvlv spec of a given type, by name:
#define PbwmSp(pth,T) ((T*)pbwmspgp->ElemPath(pth, T::StatTypeDef(0), true))

bool LeabraWizard::PBWM_Specs(LeabraNetwork* net, const String& prefix, bool set_defs) {
  if(!net) {
    if(TestError(!net, "PBWM_Specs", "network is NULL -- only makes sense to run on an existing network -- aborting!"))
      return false;
  }

  String pvlvprefix = "PVLV";
  BaseSpec_Group* pvlvspgp = (BaseSpec_Group*)net->specs.gp.FindName(pvlvprefix);
  BaseSpec_Group* pbwmspgp = net->FindMakeSpecGp(prefix);
  if(!pbwmspgp) return false;

  ////////////	UnitSpecs

  FMSpec(LeabraUnitSpec, pbwm_units, pbwmspgp, prefix + "Units");
  FMChild(LeabraUnitSpec, matrix_units, pbwm_units, "MatrixUnits");
  FMChild(PatchUnitSpec, patch_units, pbwm_units, "PatchUnits");
  FMChild(GPiUnitSpec, gpi_units, pbwm_units, "GPiUnits");
  FMChild(InvertUnitSpec, gpinv_units, pbwm_units, "GPInvert");
  FMChild(ThalUnitSpec, thal_units, pbwm_units, "ThalUnits");
  FMChild(PFCUnitSpec, pfc_units, pbwm_units, "PFCUnits");
  FMChild(LayerActUnitSpec, pfcd_units, pbwm_units, "PFCdUnits");

  ////////////	ConSpecs

  FMSpec(LeabraConSpec, lrn_cons, pbwmspgp, prefix + "LrnCons");
  FMChild(MatrixConSpec, mtx_cons_go, lrn_cons, "MatrixConsGo");
  FMChild(MatrixConSpec, mtx_cons_nogo, mtx_cons_go, "MatrixConsNoGo");
  FMChild(Deep5bConSpec, d5b_lrn_cons, lrn_cons, "Deep5bLrn");
  FMChild(LeabraConSpec, to_pfc, lrn_cons, "ToPFC");
  FMChild(Deep5bConSpec, pfcd_mnt_out, lrn_cons, "PFCdMntToOut");

  FMSpec(LeabraConSpec, fix_cons, pbwmspgp, prefix + "FixedCons");
  FMChild(LeabraBiasSpec, fix_bias, fix_cons, prefix + "FixedBias");
  FMChild(MarkerConSpec, marker_cons, fix_cons, prefix + "MarkerCons");
  FMChild(LeabraConSpec, in_to_mtx, fix_cons, "InputToMatrixBias");
  FMChild(LeabraConSpec, pfc_to_mtx, fix_cons, "PFCToMatrix");
  FMChild(Deep5bConSpec, pfcd_out_bias_mtx, fix_cons, "PFCdOutBiasMtx");
  FMChild(LeabraConSpec, in_to_thal, fix_cons, "InputToThal");
  FMChild(LeabraConSpec, gp_inhib_to_thal, fix_cons, "GPinhibToThal");
  FMChild(LeabraConSpec, pfc_to_thal, fix_cons, "PFCToThal");
  FMChild(Deep5bConSpec, pfcd_mnt_bias_thal, fix_cons, "PFCdMntBiasThal");
  FMChild(Deep5bConSpec, pfcd_out_bias_thal, fix_cons, "PFCdOutBiasThal");
  FMChild(Deep5bConSpec, pfcd_patch, fix_cons, "PFCdPatch");

  ////////////	LayerSpecs

  FMSpec(LeabraLayerSpec, pbwm_sp, pbwmspgp, prefix + "Layers");
  FMChild(LeabraLayerSpec, matrix_sp, pbwm_sp, "MatrixLayer");
  FMChild(LeabraLayerSpec, patch_sp, pbwm_sp, "PatchLayer");
  FMChild(LeabraLayerSpec, gpi_sp, pbwm_sp, "GPiLayer");
  FMChild(LeabraLayerSpec, thal_sp, pbwm_sp, "ThalLayer");
  FMChild(LeabraLayerSpec, pfc_sp, pbwm_sp, "PFCLayer");
  FMChild(Deep5bLayerSpec, pfcd_sp, pfc_sp, "PFCdLayer");

  ////////////	PrjnSpecs

  FMSpec(FullPrjnSpec, fullprjn, pbwmspgp, prefix + "FullPrjn");
  FMSpec(OneToOnePrjnSpec, onetoone, pbwmspgp, prefix + "OneToOne");
  FMSpec(GpOneToOnePrjnSpec, gponetoone, pbwmspgp, prefix + "GpOneToOne");
  FMSpec(RowColPrjnSpec, input_to_pfc_in_prjn, pbwmspgp, "InputToPFCinPrjn");
  FMChild(RowColPrjnSpec, input_to_thal_in_prjn, input_to_pfc_in_prjn,
          "InputToThalInPrjn");
  FMSpec(RowColPrjnSpec, pfc_out_to_out_prjn, pbwmspgp, "PFCoutToOutPrjn");
  FMSpec(RowColPrjnSpec, pfcd_mnt_to_out_prjn, pbwmspgp, "PFCdMntToOutPrjn");
  FMChild(RowColPrjnSpec, pfcd_mnt_to_out_thal_prjn, pfcd_mnt_to_out_prjn,
          "PFCdMntToOutPrjn_thal");
  FMSpec(RowColPrjnSpec, pfc_mnt_to_patch_prjn, pbwmspgp, "PFCMntToPatchPrjn");
  FMSpec(GpRndTesselPrjnSpec, patch_to_matrix_prjn, pbwmspgp, "PatchToMatrixPrjn");

  //////////////////////////////////////////////////////////////////////////////////
  // first: all the basic defaults from specs

  pbwmspgp->Defaults();

  //////////////////////////////////////////////////////////////////////////////////
  // set default spec parameters

  ////////////	UnitSpecs

  pbwm_units->bias_spec.SetSpec(fix_bias);

  matrix_units->SetUnique("cifer", true);
  matrix_units->cifer.on = true;
  matrix_units->cifer.thal_thr = 0.1f;
  matrix_units->cifer.thal_bin = false;

  matrix_units->SetUnique("noise_type", true);
  matrix_units->noise_type = LeabraUnitSpec::NETIN_NOISE;
  matrix_units->SetUnique("noise", true);
  matrix_units->noise.var = 0.0005f;
  matrix_units->SetUnique("noise_adapt", true);
  matrix_units->noise_adapt.trial_fixed = true;

  pfc_units->SetUnique("cifer", true);
  pfc_units->cifer.on = true;
  pfc_units->cifer.thal_thr = 0.1f;
  pfc_units->cifer.act5b_thr = 0.2f;
  pfc_units->cifer.thal_bin = true;

  ////////////	ConSpecs

  lrn_cons->lrate = 0.01f;

  mtx_cons_go->SetUnique("wt_limits", true);
  mtx_cons_go->wt_limits.sym = false;
  mtx_cons_go->SetUnique("lrate", true);
  mtx_cons_go->lrate = 0.001f;

  mtx_cons_nogo->SetUnique("nogo", true);
  mtx_cons_nogo->nogo = true;

  fix_cons->SetUnique("rnd", true);
  fix_cons->rnd.mean = 0.8f;
  fix_cons->rnd.var = 0.0f;
  fix_cons->SetUnique("wt_limits", true);
  fix_cons->wt_limits.sym = false;
  fix_cons->rnd.var = 0.0f;
  fix_cons->SetUnique("lrate", true);
  fix_cons->lrate = 0.0f;
  fix_cons->SetUnique("xcal", true);
  fix_cons->xcal.raw_l_mix = true;
  fix_cons->xcal.thr_l_mix = 0.0f;
  fix_cons->SetUnique("learn", true);
  fix_cons->learn = false;

  in_to_mtx->SetUnique("wt_scale", true);
  in_to_mtx->wt_scale.rel = 0.1f;
  pfc_to_mtx->SetUnique("wt_scale", true);
  pfc_to_mtx->wt_scale.rel = 0.1f;
  pfcd_out_bias_mtx->SetUnique("wt_scale", true);
  pfcd_out_bias_mtx->wt_scale.rel = 0.1f;

  in_to_thal->SetUnique("wt_scale", true);
  in_to_thal->wt_scale.rel = 1.0f;
  gp_inhib_to_thal->SetUnique("wt_scale", true);
  gp_inhib_to_thal->wt_scale.abs = 0.8f;
  gp_inhib_to_thal->SetUnique("inhib", true);
  gp_inhib_to_thal->inhib = true;
  pfc_to_thal->SetUnique("wt_scale", true);
  pfc_to_thal->wt_scale.rel = 1.0f;
  pfcd_mnt_bias_thal->SetUnique("wt_scale", true);
  pfcd_mnt_bias_thal->wt_scale.abs = 1.75f;
  pfcd_out_bias_thal->SetUnique("wt_scale", true);
  pfcd_out_bias_thal->wt_scale.abs = 1.75f;

  ////////////	LayerSpecs

  matrix_sp->SetUnique("lay_inhib", true);
  matrix_sp->lay_inhib.on = false;
  matrix_sp->SetUnique("unit_gp_inhib", true);
  matrix_sp->unit_gp_inhib.on = true;
  matrix_sp->unit_gp_inhib.gi = 2.0f;
  matrix_sp->unit_gp_inhib.ff = 1.0f;
  matrix_sp->unit_gp_inhib.fb = 0.0f;
  matrix_sp->SetUnique("avg_act", true);
  matrix_sp->avg_act.init = 0.4f;
  matrix_sp->avg_act.fixed = true;
  matrix_sp->SetUnique("inhib_misc", true);
  matrix_sp->inhib_misc.self_fb = 0.3f;
  matrix_sp->SetUnique("del_inhib", true);
  matrix_sp->del_inhib.on = true;
  matrix_sp->del_inhib.prv_trl = 0.001f; // todo: prv_phs instead..

  patch_sp->SetUnique("lay_inhib", true);
  patch_sp->lay_inhib.on = false;
  patch_sp->SetUnique("unit_gp_inhib", true);
  patch_sp->unit_gp_inhib.on = true;
  patch_sp->unit_gp_inhib.gi = 1.0f;
  patch_sp->unit_gp_inhib.ff = 1.0f;
  patch_sp->unit_gp_inhib.fb = 0.0f;
  patch_sp->SetUnique("avg_act", true);
  patch_sp->avg_act.init = 0.2f;
  patch_sp->avg_act.fixed = true;
  patch_sp->SetUnique("inhib_misc", true);
  patch_sp->inhib_misc.self_fb = 0.4f;

  gpi_sp->SetUnique("lay_inhib", true);
  gpi_sp->lay_inhib.on = true;
  gpi_sp->lay_inhib.gi = 1.5f;
  gpi_sp->lay_inhib.ff = 1.0f;
  gpi_sp->lay_inhib.fb = 0.5f;
  gpi_sp->SetUnique("unit_gp_inhib", true);
  gpi_sp->unit_gp_inhib.on = true;
  gpi_sp->unit_gp_inhib.gi = 1.5f;
  gpi_sp->unit_gp_inhib.ff = 1.0f;
  gpi_sp->unit_gp_inhib.fb = 0.5f;
  gpi_sp->SetUnique("avg_act", true);
  gpi_sp->avg_act.init = 0.2f;
  gpi_sp->avg_act.fixed = true;

  thal_sp->SetUnique("lay_inhib", true);
  thal_sp->lay_inhib.on = true;
  thal_sp->lay_inhib.gi = 0.8f;
  thal_sp->lay_inhib.ff = 1.0f;
  thal_sp->lay_inhib.fb = 0.5f;
  thal_sp->SetUnique("unit_gp_inhib", true);
  thal_sp->unit_gp_inhib.on = true;
  thal_sp->unit_gp_inhib.gi = 1.0f;
  thal_sp->unit_gp_inhib.ff = 1.0f;
  thal_sp->unit_gp_inhib.fb = 0.5f;
  thal_sp->SetUnique("avg_act", true);
  thal_sp->avg_act.init = 0.2f;
  thal_sp->avg_act.fixed = true;

  pfc_sp->SetUnique("lay_inhib", true);
  pfc_sp->lay_inhib.on = false;
  pfc_sp->SetUnique("unit_gp_inhib", true);
  pfc_sp->unit_gp_inhib.on = true;
  pfc_sp->unit_gp_inhib.gi = 2.0f;
  pfc_sp->unit_gp_inhib.ff = 1.0f;
  pfc_sp->unit_gp_inhib.fb = 0.5f;
  pfc_sp->SetUnique("avg_act", true);
  pfc_sp->avg_act.init = 0.2f;
  pfc_sp->avg_act.fixed = true; // use fixed..
  pfc_sp->SetUnique("decay", true);
  pfc_sp->decay.event = 0.0f;

  ////////////	PrjnSpecs

  input_to_pfc_in_prjn->recv_row_col = RowColPrjnSpec::ROWS;
  input_to_pfc_in_prjn->recv_gp = true;
  input_to_pfc_in_prjn->recv_start = 0;
  input_to_pfc_in_prjn->recv_end = 0; // todo: need to set in main fun!
  input_to_pfc_in_prjn->send_row_col = RowColPrjnSpec::ALL;
  
  input_to_thal_in_prjn->SetUnique("recv_gp", true);
  input_to_thal_in_prjn->recv_gp = false;

  pfc_out_to_out_prjn->recv_row_col = RowColPrjnSpec::ALL;
  pfc_out_to_out_prjn->send_row_col = RowColPrjnSpec::ROWS;
  pfc_out_to_out_prjn->send_gp = true;
  pfc_out_to_out_prjn->send_start = -1; // todo: need to set in main fun!
  pfc_out_to_out_prjn->send_end = -1; 

  pfcd_mnt_to_out_prjn->recv_row_col = RowColPrjnSpec::ROWS;
  pfcd_mnt_to_out_prjn->recv_gp = true;
  pfcd_mnt_to_out_prjn->recv_start = -1; // todo: need to set in main fun!
  pfcd_mnt_to_out_prjn->recv_end = -1; 
  pfcd_mnt_to_out_prjn->send_row_col = RowColPrjnSpec::ROWS;
  pfcd_mnt_to_out_prjn->send_gp = true;
  pfcd_mnt_to_out_prjn->send_start = 0; 
  pfcd_mnt_to_out_prjn->send_end = 0; // todo: need to set in main fun!
  pfcd_mnt_to_out_prjn->one_to_one = true;

  pfcd_mnt_to_out_thal_prjn->SetUnique("recv_gp", true);
  pfcd_mnt_to_out_thal_prjn->recv_gp = false;

  patch_to_matrix_prjn->def_p_con = 1.0f;
  patch_to_matrix_prjn->MakeRectangle(1, 1, 0, 0);

  pfc_mnt_to_patch_prjn->recv_row_col = RowColPrjnSpec::ALL;
  pfc_mnt_to_patch_prjn->send_row_col = RowColPrjnSpec::ROWS;
  pfc_mnt_to_patch_prjn->send_gp = true;
  pfc_mnt_to_patch_prjn->send_start = 0; 
  pfc_mnt_to_patch_prjn->send_end = 0; // todo: need to set in main fun!

  ///////// Update All!

  for(int j=0;j<pbwmspgp->leaves;j++) {
    BaseSpec* sp = (BaseSpec*)net->specs.Leaf(j);
    sp->UpdateAfterEdit();
  }

  //////////////////////////////////////////////////////////////////////////////////
  // control panel

  LeabraProject* proj = GET_MY_OWNER(LeabraProject);
  ControlPanel* cp = proj->FindMakeControlPanel(prefix);
  if(cp != NULL) {
    // cp->SetUserData("user_pinned", true);

    String subgp;
    subgp = "PFC";
    pfc_units->AddToControlPanelNm("pfc_maint", cp, "pfc", subgp);
    pfc_units->AddToControlPanelNm("cifer", cp, "pfc", subgp);

    pfc_sp->AddToControlPanelNm("unit_gp_inhib", cp, "pfc", subgp);
    pfc_sp->AddToControlPanelNm("lay_inhib", cp, "pfc", subgp);
    pfc_sp->AddToControlPanelNm("avg_act", cp, "pfc", subgp);

    subgp = "Matrix";
    matrix_units->AddToControlPanelNm("cifer", cp, "matrix", subgp);
    matrix_sp->AddToControlPanelNm("del_inhib", cp, "matrix", subgp);
    mtx_cons_go->AddToControlPanelNm("lrate", cp, "matrix", subgp,
                                     "Default Matrix lrate is .001");
    mtx_cons_go->AddToControlPanelNm("matrix", cp, "matrix", subgp);

    in_to_mtx->AddToControlPanelNm("wt_scale", cp, "in_to_matrix_bias", subgp);
    pfc_to_mtx->AddToControlPanelNm("wt_scale", cp, "pfc_to_matrix", subgp);

    subgp = "GPi_Thal";
    gpi_units->AddToControlPanelNm("gpi", cp, "gpi", subgp);
    gpi_sp->AddToControlPanelNm("lay_inhib", cp, "gpi", subgp);
    gpi_sp->AddToControlPanelNm("unit_gp_inhib", cp, "gpi", subgp);

    gp_inhib_to_thal->AddToControlPanelNm("wt_scale", cp, "gp_inhib_to_thal", subgp);
    pfcd_mnt_bias_thal->AddToControlPanelNm("wt_scale", cp, "pfcd_mnt_bias_thal", subgp);
    pfcd_out_bias_thal->AddToControlPanelNm("wt_scale", cp, "pfcd_out_bias_thal", subgp);

    cp->EditPanel(true, true);
  }
  return true;
}

static void lay_set_geom(LeabraLayer* lay, int n_stripes, int n_channels, 
                         bool gpi_thal, int n_units = -1, bool sp = true) {
  lay->unit_groups = true;
  if(gpi_thal) {
    lay->un_geom.SetXY(1, n_stripes);
    lay->gp_geom.SetXY(n_channels, 1);
  }
  else {
    lay->gp_geom.SetXY(n_channels, n_stripes);
    if(n_units > 0) {
      lay->SetNUnits(n_units);
    }
  }
  if(sp) {
    lay->gp_spc.x = 1;
    lay->gp_spc.y = 1;
  }
  lay->UpdateAfterEdit();
}

static void set_n_stripes(LeabraNetwork* net, const String& gpnm, const String& nm,
                          int n_stripes, int n_channels, bool gpi_thal,
                          int n_units, bool sp)
{
  Layer_Group* laygp = (Layer_Group*)net->layers.gp.FindName(gpnm);
  if(!laygp) return;
  LeabraLayer* lay = (LeabraLayer*)laygp->FindLeafName(nm);
  if(!lay) return;
  lay_set_geom(lay, n_stripes, n_channels, gpi_thal, n_units, sp);
}

bool LeabraWizard::PBWM_SetNStripes(LeabraNetwork* net, int in_stripes, int mnt_stripes,
				    int out_stripes, int n_channels, int n_matrix_units,
				    int n_pfc_units, const String& prefix) {
  if(TestError(!net, "PBWM_SetNStripes", "network is NULL -- only makes sense to run on an existing network -- aborting!"))
    return false;

  // this is called as a subroutine a lot too so don't save here -- could do impl but
  // not really worth it..
//   LeabraProject* proj = GET_MY_OWNER(LeabraProject);
//   if(proj) {
//     proj->undo_mgr.SaveUndo(net, "Wizard::PBWM_SetNStripes -- actually saves network specifically");
//   }

  int tot_stripes = in_stripes + mnt_stripes + out_stripes;

  set_n_stripes(net, prefix, "MatrixGo",  tot_stripes, n_channels, false,
                n_matrix_units, true);
  set_n_stripes(net, prefix, "MatrixNoGo",  tot_stripes, n_channels, false,
                n_matrix_units, true);
  set_n_stripes(net, prefix, "Patch",  1, n_channels, false, 1, true);

  set_n_stripes(net, prefix, "GPi", tot_stripes, n_channels, true, -1, true);
  set_n_stripes(net, prefix, "GPInvert", tot_stripes, n_channels, true, -1, true);
  set_n_stripes(net, prefix, "MDThal", tot_stripes, n_channels, true, -1, true);

  set_n_stripes(net, prefix, "PFC",  tot_stripes, n_channels, false, n_pfc_units, true);
  set_n_stripes(net, prefix, "PFCdeep",  tot_stripes, n_channels, false, n_pfc_units, true);

  net->Build();
  return true;
}

bool LeabraWizard::PBWM(LeabraNetwork* net, int in_stripes, int mnt_stripes,
			int out_stripes, int n_channels,
                        bool add_on, const String& prefix) {
  if(!net) {
    LeabraProject* proj = GET_MY_OWNER(LeabraProject);
    net = (LeabraNetwork*)proj->GetNewNetwork();
    if(TestError(!net, "PBWM", "network is NULL and could not make a new one -- aborting!"))
      return false;
    if(!StdNetwork()) return false;
  }

  int tot_stripes = in_stripes + mnt_stripes + out_stripes;

  // first configure PVLV system..
  if(!add_on) {
    if(TestError(!PVLV(net, 1, 1, false), "PBWM", "could not make PVLV")) return false;
  }

  String msg = "Configuring PBWM (Prefrontal-cortex Basal-ganglia Working Memory) Layers:\n\n\
There is one thing you will need to check manually after this automatic configuration\
process completes (this note will be repeated when things complete --- there may be some\
messages in the interim):\n\n";

  String man_msg = "1. Check the bidirectional connections between the PFC and all appropriate layers.\
Except for special intra-PFC connections, the conspecs INTO ALL superficial PFC layers should be ToPFC conspecs; those out from PFC layers should be regular learning conspecs.";

  msg += man_msg + "\n\nThe configuration will now be checked and a number of default parameters\
will be set.  If there are any actual errors which must be corrected before\
the network will run, you will see a message to that effect --- you will then need to\
re-run this configuration process to make sure everything is OK.  When you press\
Re/New/Init on the control process these same checks will be performed, so you\
can be sure everything is ok.";
  taMisc::Confirm(msg);

  net->RemoveUnits();

  // TODO: Note below gets rid of LeabraWizard PBWM CHECK ERROR message -- works and seems safe to do
  // net->phases.no_plus_test = false;


  //////////////////////////////////////////////////////////////////////////////////
  // make layers

  // Harvest from the PVLV function..
  Layer_Group* pvlv_laygp_da = (Layer_Group*)net->layers.gp.FindName("PVLV_DA");
  Layer_Group* pvlv_laygp_pv = (Layer_Group*)net->layers.gp.FindName("PVLV_PV");

  LeabraLayer* rew_targ_lay = NULL;
  LeabraLayer* ext_rew = NULL;
  LeabraLayer* vta = NULL;

  if(pvlv_laygp_pv) {
    rew_targ_lay = (LeabraLayer*)pvlv_laygp_pv->FindName("RewTarg");
    ext_rew = (LeabraLayer*)pvlv_laygp_pv->FindName("ExtRew");
  }
  if(pvlv_laygp_da) {
    vta = (LeabraLayer*)pvlv_laygp_da->FindName("VTA");
  }

  bool new_pbwm_laygp = false;
  Layer_Group* pbwm_laygp = net->FindMakeLayerGroup(prefix, NULL, new_pbwm_laygp);

  LeabraLayer* matrix_go = NULL;
  LeabraLayer* matrix_nogo = NULL;
  LeabraLayer* patch = NULL;
  LeabraLayer* gpi = NULL;
  LeabraLayer* gpinvert = NULL;
  LeabraLayer* thal= NULL;
  LeabraLayer* pfc = NULL;
  LeabraLayer* pfcd = NULL;

  bool new_matrix = false;
  matrix_go = (LeabraLayer*)pbwm_laygp->FindMakeLayer("MatrixGo", NULL, new_matrix);
  matrix_nogo = (LeabraLayer*)pbwm_laygp->FindMakeLayer("MatrixNoGo", NULL);
  patch = (LeabraLayer*)pbwm_laygp->FindMakeLayer("Patch", NULL);
  gpi = (LeabraLayer*)pbwm_laygp->FindMakeLayer("GPi", NULL);
  gpinvert = (LeabraLayer*)pbwm_laygp->FindMakeLayer("GPInvert", NULL);
  bool new_thal = false;
  thal = (LeabraLayer*)pbwm_laygp->FindMakeLayer("MDThal", NULL, new_thal);
  bool new_pfc  = false;
  pfc = (LeabraLayer*)pbwm_laygp->FindMakeLayer("PFC", NULL, new_pfc);
  pfcd = (LeabraLayer*)pbwm_laygp->FindMakeLayer("PFCdeep", NULL);

  //////////////////////////////////////////////////////////////////////////////////
  // collect layer groups

  int mx_z1 = 0;         // max x coordinate on layer z=1
  int mx_z2 = 0;         // z=2
  Layer_Group other_lays;   Layer_Group hidden_lays;
  Layer_Group output_lays;  Layer_Group input_lays;
  taVector3i lpos;
  int i;
  for(i=0;i<net->layers.leaves;i++) {
    LeabraLayer* lay = (LeabraLayer*)net->layers.Leaf(i);
    if(lay->owner->GetName().contains("PVLV") || 
       lay->owner->GetName().contains("PBWM") ||
       lay->owner->GetName() == prefix) continue;
    other_lays.Link(lay);
    lay->GetAbsPos(lpos);
    if(!add_on && lpos.z == 0) {
      lay->pos.z+=2; // only for non-pbwm: nobody allowed in 0!
    }
    int xm = lpos.x + lay->scaled_disp_geom.x + 1;
    if(lpos.z == 1) mx_z1 = MAX(mx_z1, xm);
    if(lpos.z == 2) mx_z2 = MAX(mx_z2, xm);
    if(lay->layer_type == Layer::HIDDEN)
      hidden_lays.Link(lay);
    else if((lay->layer_type == Layer::INPUT) || lay->name.contains("In"))
      input_lays.Link(lay);
    else if(lay->name.contains("Out"))
      output_lays.Link(lay);
    else
      input_lays.Link(lay);   // default to input -- many are now TARGET in TI
  }

  //////////////////////////////////////////////////////////////////////////////////
  // get specs

  String pvlvprefix = "PVLV";
  BaseSpec_Group* pvlvspgp = (BaseSpec_Group*)net->specs.gp.FindName(pvlvprefix);

  BaseSpec_Group* pbwmspgp = net->FindMakeSpecGp(prefix);
  if(!pbwmspgp) return false;

  if(!PBWM_Specs(net, prefix)) return false;

  //////////////////////////////////////////////////////////////////////////////////
  // apply specs to objects

  matrix_go->SetUnitSpec(PbwmSp("MatrixUnits",LeabraUnitSpec));
  matrix_go->SetLayerSpec(PbwmSp("MatrixLayer",LeabraLayerSpec));

  matrix_nogo->SetUnitSpec(PbwmSp("MatrixUnits",LeabraUnitSpec));
  matrix_nogo->SetLayerSpec(PbwmSp("MatrixLayer",LeabraLayerSpec));

  patch->SetUnitSpec(PbwmSp("PatchUnits",PatchUnitSpec));
  patch->SetLayerSpec(PbwmSp("PatchLayer",LeabraLayerSpec));

  gpi->SetUnitSpec(PbwmSp("GPiUnits",GPiUnitSpec));
  gpi->SetLayerSpec(PbwmSp("GPiLayer",LeabraLayerSpec));

  gpinvert->SetUnitSpec(PbwmSp("GPInvert",InvertUnitSpec));
  gpinvert->SetLayerSpec(PbwmSp("GPiLayer",LeabraLayerSpec));

  thal->SetUnitSpec(PbwmSp("ThalUnits",ThalUnitSpec));
  thal->SetLayerSpec(PbwmSp("ThalLayer",LeabraLayerSpec));

  pfc->SetUnitSpec(PbwmSp("PFCUnits",PFCUnitSpec));
  pfc->SetLayerSpec(PbwmSp("PFCLayer",LeabraLayerSpec));

  pfcd->SetUnitSpec(PbwmSp("PFCdUnits",LayerActUnitSpec));
  pfcd->SetLayerSpec(PbwmSp("PFCdLayer",Deep5bLayerSpec));

  //////////////////////////////////////////////////////////////////////////////////
  // make projections

  //// fix prjn params for stripes
  { RowColPrjnSpec* rcps = PbwmSp("InputToPFCinPrjn",RowColPrjnSpec);
    rcps->recv_end = in_stripes;
    rcps->UpdateAfterEdit();
  }
  { RowColPrjnSpec* rcps = PbwmSp("PFCoutToOutPrjn",RowColPrjnSpec);
    rcps->send_start = -out_stripes;
    rcps->UpdateAfterEdit();
  }
  { RowColPrjnSpec* rcps = PbwmSp("PFCdMntToOutPrjn",RowColPrjnSpec);
    rcps->recv_start = -out_stripes;
    rcps->send_end = in_stripes + mnt_stripes; // todo..
    rcps->UpdateAfterEdit();
  }
  { RowColPrjnSpec* rcps = PbwmSp("PFCMntToPatchPrjn",RowColPrjnSpec);
    rcps->send_start = in_stripes;
    rcps->send_end = in_stripes + mnt_stripes;
    rcps->UpdateAfterEdit();
  }
  { PFCUnitSpec* pfcus = PbwmSp("PFCUnits",PFCUnitSpec);
    pfcus->pfc_maint.maint_last_row = -(out_stripes + 1);
  }


  MarkerConSpec* marker_cons = PbwmSp(prefix + "MarkerCons", MarkerConSpec);
  GpOneToOnePrjnSpec* gponetoone = PbwmSp(prefix + "GpOneToOne",GpOneToOnePrjnSpec);
  FullPrjnSpec* fullprjn = PbwmSp(prefix + "FullPrjn", FullPrjnSpec);
  OneToOnePrjnSpec* onetoone = PbwmSp(prefix + "OneToOne", OneToOnePrjnSpec);

  LeabraConSpec* fix_cons = PbwmSp(prefix + "FixedCons", LeabraConSpec);

 //	  	 	   to		 from		prjn_spec	con_spec

  net->FindMakePrjn(matrix_go, gpi, gponetoone, marker_cons);
  net->FindMakePrjn(matrix_go, vta, fullprjn, marker_cons);
  net->FindMakePrjn(matrix_go, patch, PbwmSp("PatchToMatrixPrjn", GpRndTesselPrjnSpec),
                    marker_cons);
  net->FindMakePrjn(matrix_go, pfc, gponetoone,
                    PbwmSp("PFCToMatrix", LeabraConSpec));
  net->FindMakePrjn(matrix_go, pfc, PbwmSp("PFCdMntToOutPrjn",RowColPrjnSpec),
                    PbwmSp("PFCdOutBiasMtx", Deep5bConSpec));

  net->FindMakePrjn(matrix_nogo, gpi, gponetoone, marker_cons);
  net->FindMakePrjn(matrix_nogo, vta, fullprjn, marker_cons);
  net->FindMakePrjn(matrix_nogo, patch, PbwmSp("PatchToMatrixPrjn", GpRndTesselPrjnSpec),
                    marker_cons);
  net->FindMakePrjn(matrix_nogo, pfc, gponetoone,
                    PbwmSp("PFCToMatrix", LeabraConSpec));
  net->FindMakePrjn(matrix_nogo, pfc, PbwmSp("PFCdMntToOutPrjn",RowColPrjnSpec),
                    PbwmSp("PFCdOutBiasMtx", Deep5bConSpec));

  net->FindMakePrjn(patch, pfc, gponetoone, PbwmSp("PFCdPatch", Deep5bConSpec));

  net->FindMakePrjn(gpi, matrix_go, gponetoone, fix_cons);
  net->FindMakePrjn(gpi, matrix_nogo, gponetoone, fix_cons);

  net->FindMakePrjn(gpinvert, gpi, onetoone, marker_cons);

  net->FindMakePrjn(thal, gpinvert, onetoone, marker_cons);
  net->FindMakePrjn(thal, pfc, gponetoone, PbwmSp("PFCToThal", LeabraConSpec));
  net->FindMakePrjn(thal, pfc, gponetoone, PbwmSp("PFCdMntBiasThal", Deep5bConSpec));
  net->FindMakePrjn(thal, pfc, PbwmSp("PFCdMntToOutPrjn_thal",RowColPrjnSpec),
                    PbwmSp("PFCdOutBiasThal", Deep5bConSpec));

  net->FindMakePrjn(pfc, thal, gponetoone, marker_cons);
  net->FindMakePrjn(pfc, pfc, PbwmSp("PFCdMntToOutPrjn",RowColPrjnSpec),
                    PbwmSp("PFCdMntToOut", Deep5bConSpec));

  net->FindMakePrjn(pfcd, pfc, onetoone, marker_cons);

  // connect input layers
  for(i=0;i<input_lays.size;i++) {
    Layer* il = (Layer*)input_lays[i];
    if(il->name.contains("Ctrl") || il->name.contains("Task")) {
      // control or task inputs go to everyone
      net->FindMakePrjn(matrix_go, il, fullprjn, PbwmSp("MatrixConsGo", MatrixConSpec));
      net->FindMakePrjn(matrix_nogo, il, fullprjn,
                        PbwmSp("MatrixConsNoGo", MatrixConSpec));
    }
    else {
      net->FindMakePrjn(matrix_go, il, PbwmSp("InputToPFCinPrjn", RowColPrjnSpec),
                        PbwmSp("InputToMatrixBias", LeabraConSpec));
      net->FindMakePrjn(matrix_nogo, il, PbwmSp("InputToPFCinPrjn", RowColPrjnSpec),
                        PbwmSp("InputToMatrixBias", LeabraConSpec));
      net->FindMakePrjn(thal, il, PbwmSp("InputToThalInPrjn", RowColPrjnSpec),
                        PbwmSp("InputToThal", LeabraConSpec));
      net->FindMakePrjn(pfc, il, fullprjn, PbwmSp("ToPFC", LeabraConSpec));
    }
  }

  // connect output layers
  for(i=0;i<output_lays.size;i++) {
    Layer* ol = (Layer*)output_lays[i];

    net->FindMakePrjn(ol, pfc, PbwmSp("PFCoutToOutPrjn", RowColPrjnSpec), 
                      PbwmSp("Deep5bLrn", Deep5bConSpec));
    // todo: need reciprocal connection!
    // net->FindMakePrjn(pfc, ol, PbwmSp("PFCoutToOutPrjn", RowColPrjnSpec), 
    //                   PbwmSp("ToPFC", LeabraConSpec));
  }

  // connect hiden layers
  for(i=0;i<hidden_lays.size;i++) {
    Layer* hl = (Layer*)hidden_lays[i];

    net->FindMakePrjn(hl, pfc, PbwmSp("PFCoutToOutPrjn", RowColPrjnSpec), 
                      PbwmSp("Deep5bLrn", Deep5bConSpec));
    // todo: need reciprocal connection!
    // net->FindMakePrjn(pfc, hl, PbwmSp("PFCoutToOutPrjn", RowColPrjnSpec), 
    //                   PbwmSp("ToPFC", LeabraConSpec));
  }

  //////////////////////////////////////////////////////////////////////////////////
  // set positions & geometries

  if(matrix_go->brain_area.empty()) 
    matrix_go->brain_area = ".*/.*/.*/.*/Caudate Body";
  if(matrix_nogo->brain_area.empty()) 
    matrix_nogo->brain_area = ".*/.*/.*/.*/Caudate Body";
  if(patch->brain_area.empty()) 
    patch->brain_area = ".*/.*/.*/.*/Caudate Body";
  if(gpi->brain_area.empty())
    gpi->brain_area = ".*/.*/.*/.*/Substantia Nigra";
  if(gpinvert->brain_area.empty())
    gpinvert->brain_area = ".*/.*/.*/.*/Substantia Nigra";
  if(thal->brain_area.empty())
    thal->brain_area = ".*/.*/Thalamus/.*/Medial Dorsal Nucleus";
  if(pfc->brain_area.empty())
    pfc->brain_area = ".*/.*/.*/.*/BA9";
  if(pfcd->brain_area.empty())
    pfcd->brain_area = ".*/.*/.*/.*/BA9";

  int lay_spc = 2;

  if(new_pbwm_laygp) {
    pbwm_laygp->pos.z = 0;
    pbwm_laygp->pos.x = 20;
    pbwm_laygp->pos.y = 0;
  }

  ///////////////	Matrix Layout

  int mtx_st_x = 0;
  int mtx_st_y = 0;
  int mtx_nogo_y = mtx_st_y + 3 * lay_spc;
  int mtx_z = 0;

  int mtx_x_sz = 4;
  int mtx_y_sz = 4;
  int mtx_n = mtx_x_sz * mtx_y_sz;

  if(new_matrix) {
    matrix_go->pos.SetXYZ(mtx_st_x, mtx_st_y, mtx_z);
    matrix_go->un_geom.SetXYN(mtx_x_sz, mtx_y_sz, mtx_n);
    lay_set_geom(matrix_go, tot_stripes, n_channels, false);

    mtx_nogo_y = MAX(mtx_nogo_y, mtx_st_y + matrix_go->disp_geom.y + 3 * lay_spc);
    matrix_nogo->pos.SetXYZ(mtx_st_x, mtx_nogo_y, mtx_z);
    matrix_nogo->un_geom.SetXYN(mtx_x_sz, mtx_y_sz, mtx_n);

    lay_set_geom(matrix_nogo, tot_stripes, n_channels, false);

    mtx_st_x += matrix_nogo->disp_geom.x + lay_spc; // move over..
  }

  ///////////////	GPi / Thal

  int gpi_st_y = 0;

  if(new_matrix) {
    gpi->pos.SetXYZ(mtx_st_x, gpi_st_y, mtx_z);
    lay_set_geom(gpi, tot_stripes, n_channels, true);
    gpi_st_y += gpi->disp_geom.y + lay_spc;

    gpinvert->pos.SetXYZ(mtx_st_x, gpi_st_y, mtx_z);
    lay_set_geom(gpinvert, tot_stripes, n_channels, true);
    gpi_st_y += gpinvert->disp_geom.y + lay_spc;
    
    thal->pos.SetXYZ(mtx_st_x, gpi_st_y, mtx_z);
    lay_set_geom(thal, tot_stripes, n_channels, true);
    gpi_st_y += thal->disp_geom.y + lay_spc;
    
    patch->pos.SetXYZ(mtx_st_x, gpi_st_y, mtx_z);
    lay_set_geom(patch, 1, n_channels, false, 1, true);
    gpi_st_y += patch->disp_geom.y + lay_spc;
  }

  ///////////////	PFC Layout first -- get into z = 1

  int pfcu_n = 49; int pfcu_x = 7; int pfcu_y = 7;
  int pfc_st_x = 0;
  int pfc_st_y = 0;
  int pfc_z = 1;
  if(new_pfc) {
    pfc->pos.SetXYZ(pfc_st_x, pfc_st_y, pfc_z);
    pfc->un_geom.SetXYN(pfcu_x, pfcu_y, pfcu_n);
    lay_set_geom(pfc, tot_stripes, n_channels, false);

    pfc_st_y += pfc->disp_geom.y + lay_spc;

    pfcd->pos.SetXYZ(pfc_st_x, pfc_st_y, pfc_z);
    pfcd->un_geom.SetXYN(pfcu_x, pfcu_y, pfcu_n);
    lay_set_geom(pfcd, tot_stripes, n_channels, false);
  }

  // here to allow it to get disp_geom for laying out the pfc and matrix guys!
  PBWM_SetNStripes(net, in_stripes, mnt_stripes, out_stripes, n_channels);

  if(new_pbwm_laygp) {
    pbwm_laygp->pos.z = 0;
    pbwm_laygp->pos.x = 20;
    pbwm_laygp->pos.y = 0;
  }

  //////////////////////////////////////////////////////////////////////////////////
  // build and check

  net->Build();			// rebuild after defaults in place
  net->LayerPos_Cleanup();

  // move back!
  if(new_pbwm_laygp) {
    pbwm_laygp->pos.z = 0;
    net->RebuildAllViews();     // trigger update
  }

  bool ok = net->CheckConfig();

  if(!ok) {
    msg =
      "PBWM: An error in the configuration has occurred (it should be the last message\
you received prior to this one).  The network will not run until this is fixed.\
In addition, the configuration process may not be complete, so you should run this\
function again after you have corrected the source of the error.";
  }
  else {
    msg =
      "PBWM configuration is now complete.  Do not forget the one remaining thing\
you need to do manually:\n\n" + man_msg;
  }
  taMisc::Confirm(msg);

  LeabraProject* proj = GET_MY_OWNER(LeabraProject);
  if(proj) {
    proj->undo_mgr.SaveUndo(net, "Wizard::PBWM -- actually saves network specifically");
  }
  return true;
}

//////////////////////////////
//      Remove!!!

bool LeabraWizard::PBWM_Remove(LeabraNetwork* net, const String& prefix) {
  if(TestError(!net, "PBWM_Remove", "network is NULL -- only makes sense to run on an existing network -- aborting!"))
    return false;

  LeabraProject* proj = GET_MY_OWNER(LeabraProject);
  if(proj) {
    proj->undo_mgr.SaveUndo(net, "Wizard::PBWM_Remove before -- actually saves network specifically");
  }

  net->StructUpdate(true);

  net->RemoveUnits();
  net->layers.gp.RemoveName(prefix);
  //  net->layers.gp.RemoveName("PVLV");

  net->specs.gp.RemoveName(prefix);

  net->CheckSpecs();            // could have nuked dependent specs!

  net->StructUpdate(false);

  if(proj) {
    proj->undo_mgr.SaveUndo(net, "Wizard::PBWM_Remove after -- actually saves network specifically");
  }
  return true;
}


//////////////////////////////////
//              Wizard          //
//////////////////////////////////

bool LeabraWizard::Hippo(LeabraNetwork* net, int n_ec_slots) {
  if(!net) {
    LeabraProject* proj = GET_MY_OWNER(LeabraProject);
    net = (LeabraNetwork*)proj->GetNewNetwork();
    if(TestError(!net, "Hippo", "network is NULL and could not make a new one -- aborting!"))
      return false;
    if(!StdNetwork()) return false;
  }

  String msg = "Configuring ThetaPhase Hippocampus:\n\n\
 You will have to configure inputs/outputs to/from the EC layers after the configuration:\n\n";

  taMisc::Confirm(msg);

  net->RemoveUnits();

  //////////////////////////////////////////////////////////////////////////////////
  // make layers

  bool new_lay;
  Layer_Group* hip_laygp = net->FindMakeLayerGroup("Hippocampus");
  LeabraLayer* ecin = (LeabraLayer*)hip_laygp->FindMakeLayer("EC_in", NULL, new_lay);
  LeabraLayer* ecout = (LeabraLayer*)hip_laygp->FindMakeLayer("EC_out", NULL, new_lay);
  LeabraLayer* dg = (LeabraLayer*)hip_laygp->FindMakeLayer("DG", NULL, new_lay);
  LeabraLayer* ca3 = (LeabraLayer*)hip_laygp->FindMakeLayer("CA3", NULL, new_lay);
  LeabraLayer* ca1 = (LeabraLayer*)hip_laygp->FindMakeLayer("CA1", NULL, new_lay);
  LeabraLayer* subic = (LeabraLayer*)hip_laygp->FindMakeLayer("Subiculum", NULL, new_lay);

  //////////////////////////////////////////////////////////////////////////////////
  // make specs

  BaseSpec_Group* hipspec = net->FindMakeSpecGp("HippoSpecs");

  // unit specs
  FMSpec(LeabraUnitSpec, hip_units, hipspec, "HippoUnitSpec");
  // FMChild(LeabraUnitSpec, dg_units, hip_units, "DGUnits");
  // FMSpec(LeabraUnitSpec, ecout_units, units, "ECOutUnits");

  FMSpec(HippoEncoderConSpec, ecca1_cons, hipspec, "EC_CA1ConSpecs");
  FMChild(HippoEncoderConSpec, ecin_ca1_cons, ecca1_cons, "EC_in_CA1");
  FMChild(HippoEncoderConSpec, ca1_ecout_cons, ecca1_cons, "CA1_EC_out");
  FMChild(HippoEncoderConSpec, ecout_ca1_cons, ecca1_cons, "EC_out_CA1");
  FMChild(HippoEncoderConSpec, ecin_ecout_cons, ecca1_cons, "EC_in_EC_out");
  FMChild(HippoEncoderConSpec, ecout_ecin_cons, ecca1_cons, "EC_out_EC_in");
  FMChild(LeabraConSpec, in_ecin_cons, ecca1_cons, "Input_EC_in");
  FMChild(LeabraConSpec, ecout_out_cons, ecca1_cons, "EC_out_Output");
  FMChild(LeabraConSpec, tosubic_cons, ecca1_cons, "ToSubic");

  // connection specs
  FMSpec(XCalCHLConSpec, hip_cons, hipspec, "HippoConSpecs");
  FMChild(LeabraBiasSpec, hip_bias, hip_cons, "HippoBiasSpec");
  FMChild(XCalCHLConSpec, ppath_cons, hip_cons, "PerfPath");
  FMChild(XCalCHLConSpec, mossy_cons, hip_cons, "Mossy");
  FMChild(XCalCHLConSpec, ca3ca3_cons, hip_cons, "CA3_CA3");
  FMChild(XCalCHLConSpec, ca3ca1_cons, hip_cons, "CA3_CA1");

  // layer specs
  FMSpec(ThetaPhaseLayerSpec, hip_laysp, hipspec, "HippoLayerSpec");
  FMChild(ECoutLayerSpec, ecout_laysp, hip_laysp, "EC_out");
  FMChild(ECinLayerSpec, ecin_laysp, ecout_laysp, "EC_in");
  FMChild(LeabraLayerSpec, dg_laysp, hip_laysp, "DG");
  FMChild(CA3LayerSpec, ca3_laysp, hip_laysp, "CA3");
  FMChild(CA1LayerSpec, ca1_laysp, hip_laysp, "CA1");
  FMSpec(SubiculumLayerSpec, subic_laysp, hipspec, "Subiculum");

  // prjn specs
  BaseSpec_Group* prjns = (BaseSpec_Group*)hipspec->FindMakeGpName("HippoPrjns");

  FMSpec(ProjectionSpec, fullprjn, prjns, "FullPrjn");
  FMSpec(ProjectionSpec, onetoone, prjns, "OneToOne");
  FMSpec(ProjectionSpec, gponetoone, prjns, "GpOneToOne");
  FMSpec(UniformRndPrjnSpec, ppath_prjn, prjns, "RandomPerfPath");
  FMSpec(UniformRndPrjnSpec, mossy_prjn, prjns, "UniformRndMossy");


  //////////////////////////////////////////////////////////////////////////////////
  // apply specs to objects

  // set bias specs for unit specs
  hip_units->bias_spec.SetSpec(hip_bias);

  ecin->SetLayerSpec(ecin_laysp);
  ecout->SetLayerSpec(ecout_laysp);
  dg->SetLayerSpec(dg_laysp);
  ca3->SetLayerSpec(ca3_laysp);
  ca1->SetLayerSpec(ca1_laysp);
  subic->SetLayerSpec(subic_laysp);

  ecin->SetUnitSpec(hip_units);
  ecout->SetUnitSpec(hip_units);
  dg->SetUnitSpec(hip_units);
  ca3->SetUnitSpec(hip_units);
  ca1->SetUnitSpec(hip_units);
  subic->SetUnitSpec(hip_units);

  //////////////////////////////////////////////////////////////////////////////////
  // make projections

  // FindMakePrjn(Layer* recv, Layer* send,

  net->FindMakePrjn(ecin, ecout, onetoone, ecout_ecin_cons);

//   net->FindMakePrjn(ecout, ecin, onetoone, ecin_ecout_cons);
  net->FindMakePrjn(ecout, ca1, gponetoone, ca1_ecout_cons);

  net->FindMakePrjn(dg, ecin, ppath_prjn, ppath_cons);

  net->FindMakePrjn(ca3, ecin, ppath_prjn, ppath_cons);
  net->FindMakePrjn(ca3, dg, mossy_prjn, mossy_cons);
  net->FindMakePrjn(ca3, ca3, fullprjn, ca3ca3_cons);

  net->FindMakePrjn(ca1, ecin, gponetoone, ecin_ca1_cons);
  net->FindMakePrjn(ca1, ecout, gponetoone, ecout_ca1_cons);
  net->FindMakePrjn(ca1, ca3, fullprjn, ca3ca1_cons);

  net->FindMakePrjn(subic, ecin, onetoone, tosubic_cons);
  net->FindMakePrjn(subic, ecout, onetoone, tosubic_cons);

  //////////////////////////////////////////////////////////////////////////////////
  // set positions & geometries

  ecin->brain_area = ".*/.*/.*/.*/Hippocampus entorhinal cortex";
  ecout->brain_area = ".*/.*/.*/.*/Hippocampus entorhinal cortex";
  dg->brain_area = ".*/.*/.*/.*/Hippocampus dentate gyrus";
  ca3->brain_area = ".*/.*/.*/.*/Hippocampus cornu ammonis";
  ca1->brain_area = ".*/.*/.*/.*/Hippocampus cornu ammonis";
  subic->brain_area = ".*/.*/.*/.*/Hippocampus subiculum";

  ecin->unit_groups = true;
  ecin->SetNUnitGroups(n_ec_slots);
  ecin->SetNUnits(49);

  ecout->unit_groups = true;
  ecout->SetNUnitGroups(n_ec_slots);
  ecout->SetNUnits(49);

  ca1->unit_groups = true;
  ca1->SetNUnitGroups(n_ec_slots);
  ca1->SetNUnits(100);

  dg->SetNUnits(1000);

  ca3->SetNUnits(225);

  subic->SetNUnits(12);
  subic->un_geom.x = 12;
  subic->un_geom.y = 1;

  hip_laygp->pos.SetXYZ(0, 0, 1);
  ecin->pos.SetXYZ(0, 0, 0);
  ecout->pos.SetXYZ(35, 0, 0);
  subic->pos.SetXYZ(70, 0, 0);
  dg->pos.SetXYZ(0, 0, 1);
  ca3->pos.SetXYZ(0, 0, 2);
  ca1->pos.SetXYZ(35, 0, 2);

  //////////////////////////////////////////////////////////////////////////////////
  // params

  // EC_CA1ConSpecs, wt_sig.gain = 6, off 1.25, cor = 1 (not .4)

  // EC_in_CA1, abs = 2
  ecin_ca1_cons->SetUnique("wt_scale", true);
  ecin_ca1_cons->wt_scale.abs = 2.0f;

  // CA1_EC_out, abs = 4
  ca1_ecout_cons->SetUnique("wt_scale", true);
  ca1_ecout_cons->wt_scale.abs = 4.0f;

  // EC_in_EC_out mean/var = .9, .01, rel = 0, lrate = 0
  ecin_ecout_cons->SetUnique("wt_scale", true);
  ecin_ecout_cons->wt_scale.rel = 0.0f;
  ecin_ecout_cons->SetUnique("lrate", true);
  ecin_ecout_cons->lrate = 0.0f;
  ecin_ecout_cons->SetUnique("rnd", true);
  ecin_ecout_cons->rnd.mean = 0.9f;
  ecin_ecout_cons->rnd.var = 0.01f;

  ecout_ecin_cons->SetUnique("lrate", true);
  ecout_ecin_cons->lrate = 0.0f;
  ecout_ecin_cons->SetUnique("wt_scale", true);
  ecout_ecin_cons->wt_scale.rel = 0.5f;
  ecout_ecin_cons->SetUnique("rnd", true);
  ecout_ecin_cons->rnd.mean = 0.5f;
  ecout_ecin_cons->rnd.var = 0.01f;

  // HippoConSpecs, lrate = .2, hebb = 0.05
  hip_cons->SetUnique("lrate", true);
  hip_cons->lrate = 0.2f;
  // hip_cons->SetUnique("lmix", true);
  // hip_cons->lmix.hebb = 0.05f;
  
  // mossy mean = 0.9 var = 0.01, rel = 8 , lrate = 0
  mossy_cons->SetUnique("rnd", true);
  mossy_cons->rnd.mean = 0.9f;
  mossy_cons->rnd.var = 0.01f;
  mossy_cons->SetUnique("wt_scale", true);
  mossy_cons->wt_scale.rel = 8.0f;
  mossy_cons->SetUnique("lrate", true);
  mossy_cons->lrate = 0.0f;
  // mossy_cons->SetUnique("savg_cor", true);
  // mossy_cons->savg_cor.cor = 1.0f;
  
  // ca3_ca3 rel = 2, 
  ca3ca3_cons->SetUnique("wt_scale", true);
  ca3ca3_cons->wt_scale.rel = 2.0f;
  // ca3ca3_cons->SetUnique("savg_cor", true);
  // ca3ca3_cons->savg_cor.cor = 1.0f;

  // ca3_ca1 lrate = 0.05
  ca3ca1_cons->SetUnique("lrate", true);
  ca3ca1_cons->lrate = 0.05f;
  // ca3ca1_cons->SetUnique("lmix", true);
  // ca3ca1_cons->lmix.hebb = 0.005f;

  // sparse hippocampal layers!

  // dg_laysp->SetUnique("kwta", true);
  // dg_laysp->kwta.pct = 0.01f;

  // ca3_laysp->SetUnique("kwta", true);
  // ca3_laysp->kwta.pct = 0.02f;

  // ca1_laysp->SetUnique("inhib_group", true);
  // ca1_laysp->inhib_group = LeabraLayerSpec::UNIT_GROUPS;
  // ca1_laysp->SetUnique("gp_kwta", true);
  // ca1_laysp->gp_kwta.pct = 0.1f;
 

  subic_laysp->lrate_mod_con_spec.SetSpec(ca3ca1_cons);

  // todo; lrate schedule!
  
  //////////////////////////////////////////////////////////////////////////////////
  // build and check

  net->Build();
  net->LayerPos_Cleanup();

//   taMisc::CheckConfigStart(false, false);

//   bool ok = patchsp->CheckConfig_Layer(patch, false);
//   ok &= sncsp->CheckConfig_Layer(snc, false);
//   ok &= pfcmsp->CheckConfig_Layer(pfc_m, false);
//   ok &= matrixsp->CheckConfig_Layer(matrix_m, false);
//   ok &= snrthalsp->CheckConfig_Layer(snrthal_m, false);
//   if(out_gate) {
//     ok &= pfcosp->CheckConfig_Layer(pfc_o, false);
//     ok &= matrixosp->CheckConfig_Layer(matrix_o, false);
//     ok &= snrthalosp->CheckConfig_Layer(snrthal_o, false);
//   }

//   taMisc::CheckConfigEnd(ok);

//   if(!ok) {
//     msg =
//       "BG/PFC: An error in the configuration has occurred (it should be the last message\
//  you received prior to this one).  The network will not run until this is fixed.\
//  In addition, the configuration process may not be complete, so you should run this\
//  function again after you have corrected the source of the error.";
//   }
//   else {
//     msg =
//     "BG/PFC configuration is now complete.  Do not forget the one remaining thing\
//  you need to do manually:\n\n" + man_msg;
//   }
//   taMisc::Confirm(msg);

  for(int j=0;j<net->specs.leaves;j++) {
    BaseSpec* sp = (BaseSpec*)net->specs.Leaf(j);
    sp->UpdateAfterEdit();
  }

  LeabraProject* proj = GET_MY_OWNER(LeabraProject);
  if(proj) {
    proj->undo_mgr.SaveUndo(net, "Wizard::Hippo -- actually saves network specifically");
  }
  return true;
}


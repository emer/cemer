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

TA_BASEFUNS_CTORS_DEFN(LeabraWizard);

void LeabraWizard::Initialize() {
}

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
#include <GpRndTesselPrjnSpec>
#include <TiledGpRFPrjnSpec>
#include <TiledGpRFOneToOnePrjnSpec>

#include <TwoDValLayerSpec>
#include <DeepCtxtConSpec>
#include <MarkerConSpec>
#include <LayerActUnitSpec>
#include <LeabraContextLayerSpec>

#include <ExtRewLayerSpec>
// #include <LeabraTdUnit>
// #include <LeabraTdUnitSpec>
// #include <TDRewPredConSpec>
// #include <TDRewPredLayerSpec>
// #include <TDRewIntegLayerSpec>
// #include <TdLayerSpec>

#include <PPTgUnitSpec>
#include <LHbRMTgUnitSpec>
#include <VTAUnitSpec>
#include <DRNUnitSpec>
#include <LeabraDeltaConSpec>
#include <BasAmygConSpec>
#include <LatAmygConSpec>
#include <TANUnitSpec>

#include <MSNUnitSpec>
#include <PatchUnitSpec>
#include <GPiInvUnitSpec>
#include <PFCUnitSpec>
#include <DeepCopyUnitSpec>
#include <MSNConSpec>
#include <SendDeepRawConSpec>
#include <SendDeepModConSpec>

#include <BgPfcPrjnSpec>
#include <TopoWtsPrjnSpec>

#include <HippoEncoderConSpec>
#include <CHLConSpec>
#include <ECoutUnitSpec>
#include <CA1UnitSpec>
//#include <SubiculumLayerSpec>

#include <taMisc>

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
* [[<this>.LeabraTI()|LeabraTI]] -- configure specs and layers for LeabraTI -- temporal integration of information over time, based on deep neocortical layer biology -- functionally similar to an SRN but auto-encoding and predictive\n\
* [[<this>.DeepLeabra()|DeepLeabra]] -- configure DeepLeabra specs and layers, for all hidden layers in the network -- creates corresponding deep and trc layers for predictive auto-encoder learning from deep layer driver projections coming from lower layers\n\
* [[<this>.SRNContext()|SRN Context]] -- configure a network with a simple-recurrent-network (SRN) context layer\n\
* [[<this>.UnitInhib()|Unit Inhib]] -- configure unit-based inhibition for all layers in selected network (as compared with standard kWTA inhibition) ('''NOTE: parameters are out of date''').\n\
* [[<this>.Hippo()|Hippo]] -- configure a Hippocampus using theta-phase specs -- high functioning hippocampal episodic memory system.\n\
* [[<this>.TD()|Temporal Differences (TD)]] -- configure temporal-differences (TD) reinforcement learning layers.\n\
* [[<this>.PVLV()|PVLV]] -- configure PVLV (Primary Value, Learned Value) biologically-motivated reinforcement learning layers -- provides a simulated dopamine signal that reflects unexpected primary rewards (PV = primary value system) and unexpected learned reward assocations (conditioned stimuli; LV = learned value = system).\n\
:* [[<this>.PVLV_Defaults()|PVLV Defaults]] -- set the parameters in the specs of the network to the latest default values for the PVLV model, and also ensures that the standard control panels are built and contain relevant parameters -- this is only for a model that already has PVLV configured and in a standard current format.\n\
:* [[<this>.PVLV_SetLrate()|PVLV Set Lrate]] -- set the learning rate for PVLV network -- does a coordinated update across the different learning rates, to keep default dynamics balanced -- given value is the base lrate that applies to Patch, Matrix, and BAext layers -- rest of Amyg layers use 10x that value.\n\
:* [[<this>.PVLV_ConnectCSLayer()|PVLV Connect CS Layer]] -- connect or disconnect a CS layer as an input to the PVLV system -- connects to the relevant PVLV layers (Lateral Amygdala, VSMatrix).\n\
:* [[<this>.PVLV_OutToExtRew()|PVLV Connect Output to ExtRew]] -- connect or disconnect an Output layer to the ExtRew layer, which uses this output layer together with the RewTarg layer input to automatically compute reward value based on performance.\n\
* [[<this>.PBWM()|PBWM]] -- create and configure prefrontal cortex basal ganglia working memory (PBWM) layers in the network -- also does a PVLV configuration, which does the reinforcement learning for PBWM.\n\
:* [[<this>.PBWM_Defaults()|PBWM Defaults]] -- set the parameters in the specs of the network to the latest default values for the PBWM model, and also ensures that the standard control panels are built and contain relevant parameters -- this is only for a model that already has PBWM configured and in a standard current format.\n\
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
  FMChild(DeepCtxtConSpec, ti_ctxt, stdcons, "DeepTICtxt");
  FMSpec(LeabraUnitSpec, stduns, net, "LeabraUnitSpec_0");
  FMChild(LeabraUnitSpec, ti_uns, stduns, "TICtxtUnits");
  FMSpec(FullPrjnSpec, full_prjn, net, "FullPrjnSpec_0");
  FMSpec(TiledGpRFPrjnSpec, ctxt_prjn, net, "RF3x3skp1");

  ti_ctxt->SetUnique("wt_scale", true);
  ti_ctxt->wt_scale.rel = 1.0f;
  ti_uns->SetUnique("deep", true);
  ti_uns->deep.on = true;
  ti_uns->SetUnique("deep_raw_qtr", true);
  ti_uns->deep_raw_qtr = LeabraUnitSpec::Q4;

  ctxt_prjn->send_gp_size = 3;
  ctxt_prjn->send_gp_skip = 1;
  ctxt_prjn->send_gp_start = -1;

  net->specs.UpdateAllSpecs();
  
  for(int li=net->layers.leaves-1; li >= 0; li--) {
    LeabraLayer* lay = (LeabraLayer*)net->layers.Leaf(li);
    if(lay->layer_type != Layer::HIDDEN) continue;

    LeabraLayerSpec* ls = (LeabraLayerSpec*)lay->GetLayerSpec();
    if(ls->InheritsFrom(&TA_LeabraContextLayerSpec)) continue; // skip existing srn's

    //	  	 	   to		 from		prjn_spec	con_spec
    if(lay->unit_groups) {
      net->FindMakePrjn(lay, lay, ctxt_prjn,  ti_ctxt);
    }
    else {
      net->FindMakePrjn(lay, lay, full_prjn,  ti_ctxt);
    }
    lay->SetUnitSpec(ti_uns);
  }

  net->Build();
  return true;
}

bool LeabraWizard::DeepLeabra(LeabraNetwork* net) {
  if(TestError(!net, "DeepLeabra", "must have basic constructed network first")) {
    return false;
  }
  FMSpec(LeabraUnitSpec, stduns, net, "LeabraUnitSpec_0");
  FMChild(LeabraUnitSpec, s_uns, stduns, "SuperUnits");
  FMChild(LeabraUnitSpec, d_uns, stduns, "DeepUnits");
  FMChild(LeabraUnitSpec, trc_uns, stduns, "TRCUnits");
  FMSpec(LeabraConSpec, stdcons, net, "LeabraConSpec_0");
  FMChild(DeepCtxtConSpec, ti_ctxt, stdcons, "DeepTICtxt");
  FMChild(LeabraConSpec, fm_trc, stdcons, "FmTRC");
  FMChild(LeabraConSpec, to_trc, stdcons, "ToTRC");
  FMChild(LeabraConSpec, deep_td, stdcons, "DeepTopDown");
  FMChild(SendDeepRawConSpec, d_to_trc, stdcons, "DeepToTRC");
  FMChild(SendDeepModConSpec, dmod, stdcons, "SendDeepMod_fixed");
  FMSpec(FullPrjnSpec, full_prjn, net, "FullPrjnSpec_0");
  FMSpec(GpOneToOnePrjnSpec, gp_one_to_one, net, "GpOneToOne");
  FMSpec(OneToOnePrjnSpec, one_to_one, net, "OneToOne");
  FMSpec(TiledGpRFPrjnSpec, deep_prjn, net, "DeepToTRC");
  FMSpec(TiledGpRFPrjnSpec, trc_prjn, net, "RF3x3skp1");
  FMSpec(TiledGpRFPrjnSpec, ctxt_prjn, net, "RF5x5skp1");

  stduns->deep.on = true;
  
  s_uns->SetUnique("deep", true);
  s_uns->deep.role = DeepSpec::SUPER;
  d_uns->SetUnique("deep", true);
  d_uns->deep.role = DeepSpec::DEEP;
  trc_uns->SetUnique("deep", true);
  trc_uns->deep.role = DeepSpec::TRC;
  
  fm_trc->SetUnique("wt_scale", true);
  fm_trc->wt_scale.rel = 0.1f;
  d_to_trc->SetUnique("learn", true);
  d_to_trc->learn = false;
  d_to_trc->SetUnique("rnd", true);
  d_to_trc->rnd.var = 0.0f;

  deep_td->SetUnique("wt_scale", true);
  deep_td->wt_scale.rel = 0.2f;

  dmod->SetUnique("learn", true);
  dmod->learn = false;
  dmod->SetUnique("rnd", true);
  dmod->rnd.var = 0.0f;

  deep_prjn->send_gp_size = 2;
  deep_prjn->send_gp_skip = 2;
  deep_prjn->send_gp_start = 0;
  deep_prjn->wrap = true;
  deep_prjn->init_wts = true;
  deep_prjn->wts_type = TiledGpRFPrjnSpec::GAUSSIAN;
  deep_prjn->full_gauss.on = true;
  deep_prjn->full_gauss.sigma = 1.2f;
  deep_prjn->full_gauss.ctr_mv = 0.8f;
  deep_prjn->full_gauss.wrap_wts = false;
  deep_prjn->gp_gauss.on = true;
  deep_prjn->gp_gauss.sigma = 1.2f;
  deep_prjn->gp_gauss.ctr_mv = 0.8f;
  deep_prjn->gp_gauss.wrap_wts = false;
  deep_prjn->wt_range.min = 0.3f;
  deep_prjn->wt_range.max = 0.7f;

  trc_prjn->send_gp_size = 3;
  trc_prjn->send_gp_skip = 1;
  trc_prjn->send_gp_start = -1;
  trc_prjn->init_wts = true;
  trc_prjn->wrap = true;
  trc_prjn->full_gauss.on = true;
  trc_prjn->full_gauss.sigma = 0.6f;
  trc_prjn->full_gauss.wrap_wts = false;
  trc_prjn->full_gauss.ctr_mv = 0.8f;
  trc_prjn->gp_gauss.on = true;
  trc_prjn->gp_gauss.sigma = 0.6f;
  trc_prjn->gp_gauss.wrap_wts = false;
  trc_prjn->gp_gauss.ctr_mv = 0.8f;

  ctxt_prjn->send_gp_size = 5;
  ctxt_prjn->send_gp_skip = 1;
  ctxt_prjn->send_gp_start = -2;
  ctxt_prjn->init_wts = true;
  ctxt_prjn->wrap = true;
  ctxt_prjn->full_gauss.on = true;
  ctxt_prjn->full_gauss.sigma = 0.6f;
  ctxt_prjn->full_gauss.wrap_wts = false;
  ctxt_prjn->full_gauss.ctr_mv = 0.8f;
  ctxt_prjn->gp_gauss.on = true;
  ctxt_prjn->gp_gauss.sigma = 0.6f;
  ctxt_prjn->gp_gauss.wrap_wts = false;
  ctxt_prjn->gp_gauss.ctr_mv = 0.8f;

  // Layer_Group input_lays;
  // for(int li=net->layers.leaves-1; li >= 0; li--) {
  //   LeabraLayer* lay = (LeabraLayer*)net->layers.Leaf(li);
  //   if(lay->layer_type == Layer::INPUT) {
  //     input_lays.Link(lay);
  //   }
  // }

  net->specs.UpdateAllSpecs();

  net->Build();
  // build first so we can use directional information from existing connections

  for(int li=net->layers.leaves-1; li >= 0; li--) {
    LeabraLayer* lay = (LeabraLayer*)net->layers.Leaf(li);
    if(lay->layer_type != Layer::HIDDEN) continue;
    if(lay->name.contains("trc")) continue;
    if(lay->name.endsWith("d")) continue;

    lay->SetUnitSpec(s_uns);
      
    LeabraLayer* deep = (LeabraLayer*)net->FindMakeLayer(lay->name + "d");
    deep->un_geom = lay->un_geom;
    deep->unit_groups = lay->unit_groups;
    deep->gp_geom = lay->gp_geom;
    net->layers.MoveAfter(lay, deep);
    deep->PositionBehind(lay, 2);
    deep->SetUnitSpec(d_uns);

    LeabraLayer* trc = (LeabraLayer*)net->FindMakeLayer(lay->name + "trc");
    trc->un_geom = 4;
    trc->unit_groups = lay->unit_groups;
    trc->gp_geom = lay->gp_geom;
    net->layers.MoveAfter(deep, trc);
    trc->PositionBehind(deep, 2);
    trc->SetUnitSpec(trc_uns);

    Layer* fm_in = NULL;
    Layer* fm_out = NULL;
    for(int pi = 0; pi < lay->projections.size; pi++) {
      Projection* prjn = lay->projections[pi];
      if(!fm_in && prjn->direction == Projection::FM_INPUT)
        fm_in = prjn->from.ptr();
      if(!fm_out && prjn->direction == Projection::FM_OUTPUT)
        fm_out = prjn->from.ptr();
    }

    //	  	 	   to		 from		prjn_spec	con_spec
    if(lay->unit_groups) {
      net->FindMakePrjn(lay, trc, trc_prjn,  fm_trc);
      net->FindMakePrjn(lay, deep, gp_one_to_one,  dmod);
      net->FindMakePrjn(trc, deep, trc_prjn,  to_trc);
      net->FindMakePrjn(deep, trc, trc_prjn,  fm_trc);
      net->FindMakePrjn(deep, deep, ctxt_prjn,  ti_ctxt);
      net->FindMakePrjn(deep, lay, ctxt_prjn,  ti_ctxt);
      if(fm_out) {
        net->FindMakePrjn(deep, fm_out, gp_one_to_one,  deep_td);
      }
    }
    else {
      net->FindMakePrjn(lay, trc, full_prjn,  fm_trc);
      net->FindMakePrjn(lay, deep, one_to_one,  dmod);
      net->FindMakePrjn(trc, deep, full_prjn,  to_trc);
      net->FindMakePrjn(deep, trc, full_prjn,  fm_trc);
      net->FindMakePrjn(deep, deep, full_prjn,  ti_ctxt);
      net->FindMakePrjn(deep, lay, full_prjn,  ti_ctxt);
      if(fm_out) {
        net->FindMakePrjn(deep, fm_out, full_prjn,  deep_td);
      }
    }
    if(fm_in) {
      net->FindMakePrjn(trc, fm_in, deep_prjn,  d_to_trc);
    }
  }
  
  net->Build();

  String msg = "DeepLeabra Configuration complete!";
  taMisc::Confirm(msg);
  
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

  net->specs.UpdateAllSpecs();
  
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
#if 0
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

  net->specs.UpdateAllSpecs();
  
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

  net->specs.UpdateAllSpecs();

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
#endif
  return true;
}

///////////////////////////////////////////////////////////////
//                      PVLV
///////////////////////////////////////////////////////////////

bool LeabraWizard::PVLV_Defaults(LeabraNetwork* net) {
  if(!net) {
    if(TestError(!net, "PVLV", "network is NULL -- must be passed and already PVLV configured -- aborting!"))
      return false;
  }

  return PVLV_Specs(net); // true = set defaults
}

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
  FMChild(LeabraUnitSpec, pv_units, pvlv_units, "PVUnits");
  FMChild(LHbRMTgUnitSpec, lhbrmtg_units, pvlv_units, "LHbRMTgUnits");
  FMChild(PPTgUnitSpec, pptg_units, pvlv_units, "PPTgUnits");
  FMChild(VTAUnitSpec, vtap_units, pvlv_units, "VTAUnits_p");
  FMChild(VTAUnitSpec, vtan_units, vtap_units, "VTAUnits_n");
  FMChild(DRNUnitSpec, drn_units, pvlv_units, "DRNUnits");
  FMChild(TANUnitSpec, tan_units, pvlv_units, "TANUnits");
  FMChild(LeabraUnitSpec, cem_units, pvlv_units, "CeMUnits");
  FMChild(LeabraUnitSpec, la_units, pvlv_units, "LatAmygUnits");
  FMChild(BasAmygUnitSpec, baapd1_units, pvlv_units, "BAAcqPosD1Units");
  FMChild(BasAmygUnitSpec, baepd2_units, baapd1_units, "BAExtPosD2Units");
  FMChild(BasAmygUnitSpec, baand2_units, baapd1_units, "BAAcqNegD2Units");
  FMChild(BasAmygUnitSpec, baend1_units, baapd1_units, "BAExtNegD1Units");

  FMChild(MSNUnitSpec, vsppd1_units, pvlv_units, "VSPatchPosD1Units");
  FMChild(MSNUnitSpec, vsppd2_units, vsppd1_units, "VSPatchPosD2Units");
  FMChild(MSNUnitSpec, vspnd2_units, vsppd1_units, "VSPatchNegD2Units");
  FMChild(MSNUnitSpec, vspnd1_units, vsppd1_units, "VSPatchNegD1Units");

  FMChild(MSNUnitSpec, vsmpd1_units, pvlv_units, "VSMatrixPosD1Units");
  FMChild(MSNUnitSpec, vsmpd2_units, vsmpd1_units, "VSMatrixPosD2Units");
  FMChild(MSNUnitSpec, vsmnd2_units, vsmpd1_units, "VSMatrixNegD2Units");
  FMChild(MSNUnitSpec, vsmnd1_units, vsmpd1_units, "VSMatrixNegD1Units");

  FMSpec(LeabraConSpec, pvlv_cons, pvlvspgp, "PVLVLrnCons");
  FMChild(LatAmygConSpec, la_cons, pvlv_cons, "LatAmygCons");
  FMChild(BasAmygConSpec, baap_cons, pvlv_cons, "BasAmygCons_acq_pos");
  FMChild(BasAmygConSpec, baan_cons, baap_cons, "BasAmygCons_acq_neg");
  FMChild(BasAmygConSpec, baep_cons, baap_cons, "BasAmygCons_ext_pos");
  FMChild(BasAmygConSpec, baen_cons, baap_cons, "BasAmygCons_ext_neg");
  FMChild(MSNConSpec, vspatch_cons_pd1, pvlv_cons, "VSPatchCons_ToPosD1");
  FMChild(MSNConSpec, vspatch_cons_pd2, vspatch_cons_pd1,
          "VSPatchCons_ToPosD2");
  FMChild(MSNConSpec, vspatch_cons_nd1, vspatch_cons_pd1,
          "VSPatchCons_ToNegD1");
  FMChild(MSNConSpec, vspatch_cons_nd2, vspatch_cons_pd1,
          "VSPatchCons_ToNegD2");
  FMChild(MSNConSpec, vsmatrix_cons_pd1, pvlv_cons, "VSMatrixCons_ToPosD1");
  FMChild(MSNConSpec, vsmatrix_cons_nd2, vsmatrix_cons_pd1, "VSMatrixCons_ToNegD2");
  FMChild(MSNConSpec, vsmatrix_cons_pd2, vsmatrix_cons_pd1, "VSMatrixCons_ToPosD2");
  FMChild(MSNConSpec, vsmatrix_cons_nd1, vsmatrix_cons_pd1, "VSMatrixCons_ToNegD1");

  FMSpec(LeabraConSpec, fix_cons, pvlvspgp, "PVLVFixedCons");
  FMChild(LeabraBiasSpec, fix_bias, fix_cons, "PVLVFixedBias");
  FMChild(LeabraConSpec, ba_to_vsm, fix_cons, "BAtoVSMatrix");
  FMChild(LeabraConSpec, bae_to_cem_inh, fix_cons, "BAExtToCeM_Inhib");
  FMChild(LeabraConSpec, bae_to_baa_inh, fix_cons, "BAExtToBAAcq_Inhib");
  FMChild(SendDeepModConSpec, baa_to_bae_dmod, fix_cons, "BAAcqToBAExt_DeepMod");
  FMChild(SendDeepModConSpec, baa_to_vs_dmod, baa_to_bae_dmod, "BAAcqToVS_DeepMod");
  FMChild(SendDeepModConSpec, vsm_to_vsm_dmod, fix_cons, "VSMatrixToVSMatrix_DeepMod");
  FMChild(SendDeepRawConSpec, pv_to_ba_draw, fix_cons, "PVtoBA_DeepRaw");
  FMChild(LeabraConSpec, ba_to_ca, fix_cons, "BAtoCA_Fixed");
  FMChild(LeabraConSpec, fm_pv, fix_cons, "FmPV_Fixed");

  FMSpec(MarkerConSpec, marker_con, pvlvspgp, "PVLVMarkerCons");

  FMSpec(LeabraLayerSpec, laysp, pvlvspgp, "PVLVLayers");
  FMChild(LeabraLayerSpec, pvsp, laysp, "PVLayers");
  FMChild(LeabraLayerSpec, dasp, laysp, "DALayers");
  FMChild(LeabraLayerSpec, amygsp, laysp, "AmygLayer");
  FMChild(LeabraLayerSpec, amyextp, amygsp, "AmygExtLayer");
  FMChild(LeabraLayerSpec, vspsp, laysp, "VSPatchLayer");
  FMChild(LeabraLayerSpec, vsmsp, laysp, "VSMatrixLayer");
  FMChild(LeabraLayerSpec, dmsmsp, laysp, "DMSMatrixLayer");
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

  pv_units->SetUnique("deep", true);
  pv_units->deep.on = true;
  pv_units->deep.role = DeepSpec::SUPER;
  pv_units->deep.mod_min = 1.0f;

  vtap_units->SetUnique("deep_raw_qtr", true);
  vtap_units->deep_raw_qtr = LeabraUnitSpec::Q4;
  vtap_units->SetUnique("da_val", true);
  vtap_units->da_val = VTAUnitSpec::DA_P;
  vtan_units->SetUnique("da_val", true);
  vtan_units->da_val = VTAUnitSpec::DA_N;

  baapd1_units->SetUnique("deep", true);
  baapd1_units->deep.role = DeepSpec::DEEP;
  baapd1_units->SetUnique("acq_ext", true);
  baapd1_units->acq_ext = BasAmygUnitSpec::ACQ;
  baapd1_units->SetUnique("valence", true);
  baapd1_units->valence = BasAmygUnitSpec::APPETITIVE;
  
  baepd2_units->SetUnique("deep", true);
  baepd2_units->deep.role = DeepSpec::SUPER;
  baepd2_units->deep.mod_min = 0.0f;
  baepd2_units->SetUnique("acq_ext", true);
  baepd2_units->acq_ext = BasAmygUnitSpec::EXT;
  baepd2_units->SetUnique("valence", true);
  baepd2_units->valence = BasAmygUnitSpec::APPETITIVE;
  baepd2_units->SetUnique("g_bar", true);
  baepd2_units->g_bar.l = 0.3f; // todo: control panel!
  
  baand2_units->SetUnique("deep", true);
  baand2_units->deep.role = DeepSpec::DEEP;
  baand2_units->SetUnique("acq_ext", true);
  baand2_units->acq_ext = BasAmygUnitSpec::ACQ;
  baand2_units->SetUnique("valence", true);
  baand2_units->valence = BasAmygUnitSpec::AVERSIVE;
  
  baend1_units->SetUnique("deep", true);
  baend1_units->deep.role = DeepSpec::SUPER;
  baend1_units->deep.mod_min = 0.0f;
  baend1_units->SetUnique("acq_ext", true);
  baend1_units->acq_ext = BasAmygUnitSpec::EXT;
  baend1_units->SetUnique("valence", true);
  baend1_units->valence = BasAmygUnitSpec::AVERSIVE;
  baend1_units->SetUnique("g_bar", true);
  baend1_units->g_bar.l = 0.3f; // todo: control panel!
  
  cem_units->SetUnique("act", true);
  cem_units->act.gain = 400.0f;

  vsppd1_units->SetUnique("deep", true);
  vsppd1_units->deep.on = true;
  vsppd1_units->deep.role = DeepSpec::SUPER;
  vsppd1_units->deep.raw_thr_rel = 0.1f;
  vsppd1_units->deep.raw_thr_abs = 0.1f;
  vsppd1_units->deep.mod_min = 0.0f;
  vsppd1_units->SetUnique("dar", true);
  vsppd1_units->dar = MSNUnitSpec::D1R;
  vsppd1_units->SetUnique("matrix_patch", true);
  vsppd1_units->matrix_patch = MSNUnitSpec::PATCH;
  vsppd1_units->SetUnique("dorsal_ventral", true);
  vsppd1_units->dorsal_ventral = MSNUnitSpec::VENTRAL;
  vsppd1_units->SetUnique("valence", true);
  vsppd1_units->valence = MSNUnitSpec::APPETITIVE;
  
  vsppd2_units->SetUnique("deep", false);
  vsppd2_units->SetUnique("dar", true);
  vsppd2_units->dar = MSNUnitSpec::D2R;
  vsppd2_units->SetUnique("matrix_patch", true);
  vsppd2_units->matrix_patch = MSNUnitSpec::PATCH;
  vsppd2_units->SetUnique("dorsal_ventral", true);
  vsppd2_units->dorsal_ventral = MSNUnitSpec::VENTRAL;
  vsppd2_units->SetUnique("valence", true);
  vsppd2_units->valence = MSNUnitSpec::APPETITIVE;
  
  vspnd2_units->SetUnique("deep", false);
  vspnd2_units->SetUnique("dar", true);
  vspnd2_units->dar = MSNUnitSpec::D2R;
  vspnd2_units->SetUnique("matrix_patch", true);
  vspnd2_units->matrix_patch = MSNUnitSpec::PATCH;
  vspnd2_units->SetUnique("dorsal_ventral", true);
  vspnd2_units->dorsal_ventral = MSNUnitSpec::VENTRAL;
  vspnd2_units->SetUnique("valence", true);
  vspnd2_units->valence = MSNUnitSpec::AVERSIVE;
  
  vspnd1_units->SetUnique("deep", false);
  vspnd1_units->SetUnique("dar", true);
  vspnd1_units->dar = MSNUnitSpec::D1R;
  vspnd1_units->SetUnique("matrix_patch", true);
  vspnd1_units->matrix_patch = MSNUnitSpec::PATCH;
  vspnd1_units->SetUnique("dorsal_ventral", true);
  vspnd1_units->dorsal_ventral = MSNUnitSpec::VENTRAL;
  vspnd1_units->SetUnique("valence", true);
  vspnd1_units->valence = MSNUnitSpec::AVERSIVE;
  
  vsmpd1_units->SetUnique("deep", true);
  vsmpd1_units->deep.on = true;
  vsmpd1_units->deep.role = DeepSpec::SUPER;
  vsmpd1_units->deep.raw_thr_rel = 0.1f;
  vsmpd1_units->deep.raw_thr_abs = 0.1f;
  vsmpd1_units->deep.mod_min = 0.0f;
  vsmpd1_units->SetUnique("dar", true);
  vsmpd1_units->dar = MSNUnitSpec::D1R;
  vsmpd1_units->SetUnique("matrix_patch", true);
  vsmpd1_units->matrix_patch = MSNUnitSpec::MATRIX;
  vsmpd1_units->SetUnique("dorsal_ventral", true);
  vsmpd1_units->dorsal_ventral = MSNUnitSpec::VENTRAL;
  vsmpd1_units->SetUnique("valence", true);
  vsmpd1_units->valence = MSNUnitSpec::APPETITIVE;
  
  vsmnd2_units->SetUnique("deep", false);
  vsmnd2_units->SetUnique("dar", true);
  vsmnd2_units->dar = MSNUnitSpec::D2R;
  vsmnd2_units->SetUnique("matrix_patch", true);
  vsmnd2_units->matrix_patch = MSNUnitSpec::MATRIX;
  vsmnd2_units->SetUnique("dorsal_ventral", true);
  vsmnd2_units->dorsal_ventral = MSNUnitSpec::VENTRAL;
  vsmnd2_units->SetUnique("valence", true);
  vsmnd2_units->valence = MSNUnitSpec::AVERSIVE;

  vsmpd2_units->SetUnique("deep", false);
  vsmpd2_units->SetUnique("dar", true);
  vsmpd2_units->dar = MSNUnitSpec::D2R;
  vsmpd2_units->SetUnique("matrix_patch", true);
  vsmpd2_units->matrix_patch = MSNUnitSpec::MATRIX;
  vsmpd2_units->SetUnique("dorsal_ventral", true);
  vsmpd2_units->dorsal_ventral = MSNUnitSpec::VENTRAL;
  vsmpd2_units->SetUnique("valence", true);
  vsmpd2_units->valence = MSNUnitSpec::APPETITIVE;
  
  vsmnd1_units->SetUnique("deep", false);
  vsmnd1_units->SetUnique("dar", true);
  vsmnd1_units->dar = MSNUnitSpec::D1R;
  vsmnd1_units->SetUnique("matrix_patch", true);
  vsmnd1_units->matrix_patch = MSNUnitSpec::MATRIX;
  vsmnd1_units->SetUnique("dorsal_ventral", true);
  vsmnd1_units->dorsal_ventral = MSNUnitSpec::VENTRAL;
  vsmnd1_units->SetUnique("valence", true);
  vsmnd1_units->valence = MSNUnitSpec::AVERSIVE;

  //////  Cons

  const float base_lrate = 0.02f;
  const float base_da_gain = 1.0f;
  
  pvlv_cons->UpdateAfterEdit();
  pvlv_cons->lrate = base_lrate;
  pvlv_cons->rnd.mean = 0.01f;
  pvlv_cons->rnd.var = 0.0f;
  pvlv_cons->wt_limits.sym = false;

  la_cons->SetUnique("lrate", true);
  la_cons->lrate = 10.0f * base_lrate;
  la_cons->neg_da_gain = 0.05f * base_da_gain;
  baap_cons->SetUnique("lrate", true);
  baap_cons->lrate = 10.0f * base_lrate;
  baap_cons->SetUnique("wt_scale", true);
  baap_cons->wt_scale.abs = 0.95f;
  baap_cons->SetUnique("wt_sig", true);
  baap_cons->wt_sig.gain = 1.0f;
  baap_cons->SetUnique("ba_learn", true);
  baap_cons->ba_learn.burst_da_gain = base_da_gain;
  baap_cons->ba_learn.dip_da_gain = 0.05f * base_da_gain;

//  baan_cons->SetUnique("lrate", false);
//  baan_cons->SetUnique("wt_sig", false);
  
  baan_cons->SetUnique("lrate", true);
  baan_cons->lrate = 10.0f * base_lrate;
  baan_cons->SetUnique("wt_scale", true);
  baan_cons->wt_scale.abs = 0.95f;
  baan_cons->SetUnique("wt_sig", true);
  baan_cons->wt_sig.gain = 1.0f;
  baan_cons->SetUnique("ba_learn", true);
  baan_cons->ba_learn.burst_da_gain = 0.05f * base_da_gain;
  baan_cons->ba_learn.dip_da_gain = base_da_gain;
  
  baep_cons->SetUnique("wt_sig", false);
  baep_cons->SetUnique("rnd", true);
  baep_cons->rnd.mean = 0.1f;
  baep_cons->rnd.var = 0.0f;
  baep_cons->SetUnique("lrate", true);
  baep_cons->lrate = base_lrate; // todo: was 2x base..
  baep_cons->SetUnique("wt_scale", true);
  baep_cons->wt_scale.abs = 1.2f;
  baep_cons->SetUnique("deep", true);
  baep_cons->deep.on = true;
  baep_cons->deep.bg_lrate = 0.0f;
  baep_cons->deep.fg_lrate = 1.0f;
  baep_cons->SetUnique("ba_learn", true);
  baep_cons->ba_learn.burst_da_gain = 1.0f;
  baep_cons->ba_learn.dip_da_gain = 1.0f;
  baep_cons->ba_learn.us_delta = false;

  baen_cons->SetUnique("wt_sig", false);
  baen_cons->SetUnique("rnd", true);
  baen_cons->rnd.mean = 0.1f;
  baen_cons->rnd.var = 0.0f;
  baen_cons->SetUnique("lrate", true);
  baen_cons->lrate = base_lrate; // todo: was 2x base..
  baen_cons->SetUnique("wt_scale", true);
  baen_cons->wt_scale.abs = 1.2f;
  baen_cons->SetUnique("deep", true);
  baen_cons->deep.on = true;
  baen_cons->deep.bg_lrate = 0.0f;
  baen_cons->deep.fg_lrate = 1.0f;
  baen_cons->SetUnique("ba_learn", true);
  baen_cons->ba_learn.burst_da_gain = 1.0f;
  baen_cons->ba_learn.dip_da_gain = 1.0f;
  baen_cons->ba_learn.us_delta = false;
  
  vspatch_cons_pd1->SetUnique("rnd", true);
  vspatch_cons_pd1->rnd.mean = 0.01f;
  vspatch_cons_pd1->rnd.var = 0.0f;
  vspatch_cons_pd1->SetUnique("wt_scale", true);
  vspatch_cons_pd1->wt_scale.abs = 1.0f;
  vspatch_cons_pd1->SetUnique("lrate", true);
  vspatch_cons_pd1->lrate = base_lrate;
  vspatch_cons_pd1->SetUnique("wt_sig", true);
  vspatch_cons_pd1->wt_sig.gain = 1.0f;
  vspatch_cons_pd1->SetUnique("deep", true);
  vspatch_cons_pd1->deep.on = true;
  vspatch_cons_pd1->deep.bg_lrate = 0.0f;
  vspatch_cons_pd1->deep.fg_lrate = 1.0f;
  vspatch_cons_pd1->SetUnique("su_act_var", true);
  vspatch_cons_pd1->su_act_var = MSNConSpec::PREV_TRIAL;
  vspatch_cons_pd1->SetUnique("ru_act_var", true);
  vspatch_cons_pd1->ru_act_var = MSNConSpec::PREV_TRIAL;
  vspatch_cons_pd1->SetUnique("learn_rule", true);
  vspatch_cons_pd1->learn_rule = MSNConSpec::DA_HEBB_VS;
  vspatch_cons_pd1->SetUnique("burst_da_gain", true);
  vspatch_cons_pd1->burst_da_gain = base_da_gain;
  vspatch_cons_pd1->SetUnique("dip_da_gain", true);
  vspatch_cons_pd1->dip_da_gain = 0.2f * base_da_gain;

  vspatch_cons_pd2->SetUnique("rnd", false);
  vspatch_cons_pd2->SetUnique("wt_scale", false);
  vspatch_cons_pd2->SetUnique("lrate", true);
  vspatch_cons_pd2->lrate = 0.5f * base_lrate;
  vspatch_cons_pd2->SetUnique("wt_sig", false);
  vspatch_cons_pd2->SetUnique("deep", false);
  vspatch_cons_pd2->SetUnique("su_act_var", false);
  vspatch_cons_pd2->SetUnique("ru_act_var", false);
  vspatch_cons_pd2->SetUnique("learn_rule", false);
  vspatch_cons_pd2->SetUnique("burst_da_gain", true);
  vspatch_cons_pd2->burst_da_gain = base_da_gain;
  vspatch_cons_pd2->SetUnique("dip_da_gain", true);
  vspatch_cons_pd2->dip_da_gain = base_da_gain;
  
  vspatch_cons_nd1->SetUnique("rnd", false);
  vspatch_cons_nd1->SetUnique("wt_scale", false);
  vspatch_cons_nd1->SetUnique("lrate", true);
  vspatch_cons_nd1->lrate = 0.5f * base_lrate;
  vspatch_cons_nd1->SetUnique("wt_sig", false);
  vspatch_cons_nd1->SetUnique("deep", false);
  vspatch_cons_nd1->SetUnique("su_act_var", false);
  vspatch_cons_nd1->SetUnique("ru_act_var", false);
  vspatch_cons_nd1->SetUnique("learn_rule", false);
  vspatch_cons_nd1->SetUnique("burst_da_gain", true);
  vspatch_cons_nd1->burst_da_gain = base_da_gain;
  vspatch_cons_nd1->SetUnique("dip_da_gain", true);
  vspatch_cons_nd1->dip_da_gain = base_da_gain;
  
  vspatch_cons_nd2->SetUnique("rnd", false);
  vspatch_cons_nd2->SetUnique("wt_scale", false);
  vspatch_cons_nd2->SetUnique("lrate", true);
  vspatch_cons_nd2->lrate = base_lrate;
  vspatch_cons_nd2->SetUnique("wt_sig", false);
  vspatch_cons_nd2->SetUnique("deep", false);
  vspatch_cons_nd2->SetUnique("su_act_var", false);
  vspatch_cons_nd2->SetUnique("ru_act_var", false);
  vspatch_cons_nd2->SetUnique("learn_rule", false);
  vspatch_cons_nd2->SetUnique("burst_da_gain", true);
  vspatch_cons_nd2->burst_da_gain = 0.2f * base_da_gain;
  vspatch_cons_nd2->SetUnique("dip_da_gain", true);
  vspatch_cons_nd2->dip_da_gain = base_da_gain;
  
  vsmatrix_cons_pd1->SetUnique("rnd", true);
  vsmatrix_cons_pd1->rnd.mean = 0.01f;
  vsmatrix_cons_pd1->rnd.var = 0.0f;
  vsmatrix_cons_pd1->SetUnique("wt_scale", true);
  vsmatrix_cons_pd1->wt_scale.abs = 0.5f;
  vsmatrix_cons_pd1->SetUnique("lrate", true);
  vsmatrix_cons_pd1->lrate = base_lrate;
  vsmatrix_cons_pd1->SetUnique("wt_sig", true);
  vsmatrix_cons_pd1->wt_sig.gain = 1.0f;
  vsmatrix_cons_pd1->SetUnique("deep", true);
  vsmatrix_cons_pd1->deep.on = true;
  vsmatrix_cons_pd1->deep.bg_lrate = 0.0f;
  vsmatrix_cons_pd1->deep.fg_lrate = 1.0f;
  vsmatrix_cons_pd1->SetUnique("su_act_var", true);
  vsmatrix_cons_pd1->su_act_var = MSNConSpec::ACT_P;
  vsmatrix_cons_pd1->SetUnique("ru_act_var", true);
  vsmatrix_cons_pd1->ru_act_var = MSNConSpec::ACT_P;
  vsmatrix_cons_pd1->SetUnique("learn_rule", true);
  vsmatrix_cons_pd1->learn_rule = MSNConSpec::TRACE_NO_THAL_VS;
  vsmatrix_cons_pd1->SetUnique("burst_da_gain", true);
  vsmatrix_cons_pd1->burst_da_gain = base_da_gain;
  vsmatrix_cons_pd1->SetUnique("dip_da_gain", true);
  vsmatrix_cons_pd1->dip_da_gain = 0.2f * base_da_gain;
  
  vsmatrix_cons_nd2->SetUnique("rnd", false);
  vsmatrix_cons_nd2->SetUnique("wt_scale", false);
  vsmatrix_cons_nd2->SetUnique("lrate", true);
  vsmatrix_cons_nd2->lrate = 2.0f * base_lrate;
  vsmatrix_cons_nd2->SetUnique("wt_sig", false);
  vsmatrix_cons_nd2->SetUnique("deep", false);
  vsmatrix_cons_nd2->SetUnique("su_act_var", false);
  vsmatrix_cons_nd2->SetUnique("ru_act_var", false);
  vsmatrix_cons_nd2->SetUnique("learn_rule", false);
  vsmatrix_cons_nd2->SetUnique("burst_da_gain", true);
  vsmatrix_cons_nd2->burst_da_gain = 0.2f * base_da_gain;
  vsmatrix_cons_nd2->SetUnique("dip_da_gain", true);
  vsmatrix_cons_nd2->dip_da_gain = base_da_gain;
  
  vsmatrix_cons_pd2->SetUnique("rnd", false);
  vsmatrix_cons_pd2->SetUnique("wt_scale", false);
  vsmatrix_cons_pd2->SetUnique("lrate", true);
  vsmatrix_cons_pd2->lrate = base_lrate;
  vsmatrix_cons_pd2->SetUnique("wt_sig", false);
  vsmatrix_cons_pd2->SetUnique("deep", false);
  vsmatrix_cons_pd2->SetUnique("su_act_var", false);
  vsmatrix_cons_pd2->SetUnique("ru_act_var", false);
  vsmatrix_cons_pd2->SetUnique("learn_rule", false);
  vsmatrix_cons_pd2->SetUnique("burst_da_gain", true);
  vsmatrix_cons_pd2->burst_da_gain = base_da_gain;
  vsmatrix_cons_pd2->SetUnique("dip_da_gain", true);
  vsmatrix_cons_pd2->dip_da_gain = base_da_gain;

  vsmatrix_cons_nd1->SetUnique("rnd", false);
  vsmatrix_cons_nd1->SetUnique("wt_scale", false);
  vsmatrix_cons_nd1->SetUnique("lrate", true);
  vsmatrix_cons_nd1->lrate = 2.0f * base_lrate;
  vsmatrix_cons_nd1->SetUnique("wt_sig", false);
  vsmatrix_cons_nd1->SetUnique("deep", false);
  vsmatrix_cons_nd1->SetUnique("su_act_var", false);
  vsmatrix_cons_nd1->SetUnique("ru_act_var", false);
  vsmatrix_cons_nd1->SetUnique("learn_rule", false);
  vsmatrix_cons_nd1->SetUnique("burst_da_gain", true);
  vsmatrix_cons_nd1->burst_da_gain = base_da_gain;
  vsmatrix_cons_nd1->SetUnique("dip_da_gain", true);
  vsmatrix_cons_nd1->dip_da_gain = base_da_gain;

  fix_cons->UpdateAfterEdit();
  fix_cons->learn = false;
  fix_cons->rnd.mean = 0.9f;
  fix_cons->rnd.var = 0.0f;
  fix_cons->wt_limits.sym = false;
  fix_cons->lrate = 0.0f;

  ba_to_vsm->SetUnique("wt_scale", true);
  ba_to_vsm->wt_scale.abs = 0.15f;

  bae_to_cem_inh->SetUnique("wt_scale", true);
  bae_to_cem_inh->wt_scale.abs = 0.5f;
  bae_to_cem_inh->SetUnique("inhib", true);
  bae_to_cem_inh->inhib = true;

  bae_to_baa_inh->SetUnique("wt_scale", true);
  bae_to_baa_inh->wt_scale.abs = 1.1f;
  bae_to_baa_inh->SetUnique("inhib", true);
  bae_to_baa_inh->inhib = true;

  baa_to_bae_dmod->SetUnique("wt_scale", true);
  baa_to_bae_dmod->wt_scale.abs = 0.1f; // todo: does this do anything??
  baa_to_bae_dmod->SetUnique("rnd", false);

  baa_to_vs_dmod->SetUnique("wt_scale", true);
  baa_to_vs_dmod->wt_scale.abs = 1.0f;
  baa_to_vs_dmod->SetUnique("rnd", false);

  vsm_to_vsm_dmod->SetUnique("wt_scale", true);
  vsm_to_vsm_dmod->wt_scale.abs = 0.1f; // todo: does this do anything??
  vsm_to_vsm_dmod->SetUnique("rnd", false);

  pv_to_ba_draw->SetUnique("rnd", true);
  pv_to_ba_draw->rnd.mean = 1.0f;
  pv_to_ba_draw->rnd.var = 0.0f;
  
  ba_to_ca->SetUnique("wt_scale", true);
  ba_to_ca->wt_scale.abs = 0.7f;
  ba_to_ca->SetUnique("rnd", false);
  
  fm_pv->SetUnique("wt_scale", true);
  fm_pv->wt_scale.abs = 0.5f;
  fm_pv->SetUnique("rnd", false);
  
  ////// Layers
  laysp->UpdateAfterEdit();
  laysp->lay_inhib.gi = 1.0f;
  laysp->lay_inhib.ff = 1.0f;
  laysp->lay_inhib.fb = 0.0f;
  laysp->avg_act.init = 0.25f;
  laysp->avg_act.fixed = true;
  laysp->inhib_misc.self_fb = 0.3f;
  laysp->decay.trial = 0.0f;

  dasp->SetUnique("inhib_misc", true);
  dasp->inhib_misc.self_fb = 0.8f; // only relevant for PPTg units

  amygsp->SetUnique("inhib_misc", true);
  amygsp->inhib_misc.self_fb = 0.5f;

  vspsp->SetUnique("inhib_misc", true);
  vspsp->inhib_misc.self_fb = 0.5f;

  vsmsp->SetUnique("del_inhib", true);
  vsmsp->del_inhib.on = true;
  vsmsp->del_inhib.prv_trl = 6.0f;
  vsmsp->del_inhib.prv_q = 0.0f;

  dmsmsp->SetUnique("del_inhib", true);
  dmsmsp->del_inhib.on = true;
  dmsmsp->del_inhib.prv_trl = 6.0f;
  dmsmsp->del_inhib.prv_q = 0.0f;

  ersp->unit_range.min = 0.0f;
  ersp->unit_range.max = 1.0f;
  ersp->rew_type = ExtRewLayerSpec::EXT_REW; // will be updated later

  /////// Prjns
  
  pv_fm_er->MakeRectangle(1,1,0,0);
  pv_fm_er->send_off.x = 2.0;
  pv_fm_er->wrap = false;

  //////////////////
  // Update Everyone

  pvlv_units->UpdateAfterEdit();
  pvlv_cons->UpdateAfterEdit();
  laysp->UpdateAfterEdit();

  net->specs.UpdateAllSpecs();
  pvlvspgp->UpdateAllSpecs();

  //////////////////////////////////////////////////////////////////////////////////
  // control panel

  LeabraProject* proj = GET_MY_OWNER(LeabraProject);
  ControlPanel* cp = proj->FindMakeControlPanel("PVLV");
  if(cp) {
     // cp->SetUserData("user_pinned", true);
    // pvlv_cons->AddToControlPanelNm("lrate", cp, "pvlv"); 
    la_cons->AddToControlPanelNm("lrate", cp, "lat_amyg"); 
    baap_cons->AddToControlPanelNm("lrate", cp, "bas_amyg_acq_pos");
    baap_cons->AddToControlPanelNm("dip_da_gain", cp, "bas_amyg_acq_pos");
    baan_cons->AddToControlPanelNm("lrate", cp, "bas_amyg_acq_neg");
    baan_cons->AddToControlPanelNm("dip_da_gain", cp, "bas_amyg_acq_neg");
    baep_cons->AddToControlPanelNm("lrate", cp, "bas_amyg_ext_pos");
    baep_cons->AddToControlPanelNm("dip_da_gain", cp, "bas_amyg_ext_pos");
    baen_cons->AddToControlPanelNm("lrate", cp, "bas_amyg_ext_neg");
    baen_cons->AddToControlPanelNm("dip_da_gain", cp, "bas_amyg_ext_neg");
    vspatch_cons_pd1->AddToControlPanelNm("lrate", cp, "vs_patch");
    vsmatrix_cons_pd1->AddToControlPanelNm("lrate", cp, "vs_matrix");

    baepd2_units->AddToControlPanelNm("g_bar", cp, "bas_amyg_ext_pos");
    baend1_units->AddToControlPanelNm("g_bar", cp, "bas_amyg_ext_neg");
    lhbrmtg_units->AddToControlPanelNm("lhb", cp, "lhb_rmtg");
    lhbrmtg_units->AddToControlPanelNm("gains", cp, "lhb_rmtg");
    pptg_units->AddToControlPanelNm("d_net_gain", cp, "pptg");
    vtap_units->AddToControlPanelNm("da", cp, "vta_p");
    vtap_units->AddToControlPanelNm("gains", cp, "vta_p");
    vtap_units->AddToControlPanelNm("lv_block", cp, "vta_p");
    drn_units->AddToControlPanelNm("se", cp, "drn");

    laysp->AddToControlPanelNm("lay_inhib", cp, "pvlv");
    laysp->AddToControlPanelNm("inhib_misc", cp, "pvlv");

    cp->EditPanel(true, true);
  }

  return true;
}


bool LeabraWizard::PVLV_SetLrate(LeabraNetwork* net, float base_lrate) {
  if(!net) {
    if(TestError(!net, "PVLV_SetLrate", "network is NULL -- only makes sense to run on an existing network -- aborting!"))
      return false;
  }

  LeabraProject* proj = GET_MY_OWNER(LeabraProject);
  if(proj) {
    proj->undo_mgr.SaveUndo(net, "Wizard::PVLV_SetLrate before -- actually saves network specifically");
  }

  String pvlvprefix = "PVLV";
  BaseSpec_Group* pvlvspgp = net->FindMakeSpecGp(pvlvprefix);
  if(!pvlvspgp) return false;

  PvlvSp("PVLVLrnCons", LeabraConSpec)->lrate = base_lrate;
  PvlvSp("LatAmygCons", LatAmygConSpec)->lrate = 10.0f * base_lrate;
  PvlvSp("BasAmygCons_acq_pos", BasAmygConSpec)->lrate = 10.0f * base_lrate;
  PvlvSp("BasAmygCons_ext", BasAmygConSpec)->lrate = base_lrate; // todo: was 2x base..

  PvlvSp("VSPatchCons_ToPosD1", MSNConSpec)->lrate = base_lrate;
  PvlvSp("VSMatrixCons_ToPosD1", MSNConSpec)->lrate = base_lrate;

  if(proj) {
    proj->undo_mgr.SaveUndo(net, "Wizard::PVLV_SetLrate after -- actually saves network specifically");
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
  rew_targ->layer_type = Layer::INPUT;
  ext_rew->layer_type = Layer::HIDDEN;

  bool new_amyg = false;
  LeabraLayer* lat_amyg = (LeabraLayer*)amyg_gp->FindMakeLayer("LatAmyg", NULL, new_amyg);
  LeabraLayer* baapd1 = (LeabraLayer*)amyg_gp->FindMakeLayer("BAAcqPosD1");
  LeabraLayer* baepd2 = (LeabraLayer*)amyg_gp->FindMakeLayer("BAExtPosD2");
  LeabraLayer* capos = (LeabraLayer*)amyg_gp->FindMakeLayer("CAPos");
  LeabraLayer* baand2 = (LeabraLayer*)amyg_gp->FindMakeLayer("BAAcqNegD2");
  LeabraLayer* baend1 = (LeabraLayer*)amyg_gp->FindMakeLayer("BAExtNegD1");
  LeabraLayer* caneg = (LeabraLayer*)amyg_gp->FindMakeLayer("CANeg");

  bool new_vs = false;
  LeabraLayer* vsppd1 = (LeabraLayer*)vs_gp->FindMakeLayer("VSPatchPosD1", NULL, new_vs);
  LeabraLayer* vsppd2 = (LeabraLayer*)vs_gp->FindMakeLayer("VSPatchPosD2");
  LeabraLayer* vspnd2 = (LeabraLayer*)vs_gp->FindMakeLayer("VSPatchNegD2");
  LeabraLayer* vspnd1 = (LeabraLayer*)vs_gp->FindMakeLayer("VSPatchNegD1");

  LeabraLayer* vsmpd1 = (LeabraLayer*)vs_gp->FindMakeLayer("VSMatrixPosD1");
  LeabraLayer* vsmpd2 = (LeabraLayer*)vs_gp->FindMakeLayer("VSMatrixPosD2");
  LeabraLayer* vsmnd2 = (LeabraLayer*)vs_gp->FindMakeLayer("VSMatrixNegD2");
  LeabraLayer* vsmnd1 = (LeabraLayer*)vs_gp->FindMakeLayer("VSMatrixNegD1");

  bool new_da = false;
  LeabraLayer* pptg_p = (LeabraLayer*)da_gp->FindMakeLayer("PPTg_p", NULL, new_da);
  LeabraLayer* vtap = (LeabraLayer*)da_gp->FindMakeLayer("VTAp");
  LeabraLayer* lhb = (LeabraLayer*)da_gp->FindMakeLayer("LHbRMTg");

  LeabraLayer* pptg_n = (LeabraLayer*)da_gp->FindMakeLayer("PPTg_n");
  LeabraLayer* vtan = (LeabraLayer*)da_gp->FindMakeLayer("VTAn");
  LeabraLayer* drn = (LeabraLayer*)da_gp->FindMakeLayer("DRN");
 
  //////////////////////////////////////////////////////////////////////////////////
  // collect layer groups

  Layer_Group other_lays;
  Layer_Group hidden_lays;
  Layer_Group output_lays;
  Layer_Group input_lays;
  Layer_Group time_in_lays;
  Layer_Group ctxt_in_lays;
  int i;
  for(i=0;i<net->layers.leaves;i++) {
    LeabraLayer* lay = (LeabraLayer*)net->layers.Leaf(i);
    LeabraLayerSpec* laysp = (LeabraLayerSpec*)lay->spec.SPtr();
    if(lay->owner->GetName().contains("PVLV_")) continue;
    other_lays.Link(lay);
    if(lay->pos.z == 0) lay->pos.z = 2; // nobody allowed in 0!
    if(lay->layer_type == Layer::HIDDEN)
      hidden_lays.Link(lay);
    else if((lay->layer_type == Layer::INPUT) || lay->name.contains_ci("in")) {
      if(lay->name.contains_ci("time"))
        time_in_lays.Link(lay);
      else if(lay->name.contains_ci("context") || lay->name.contains_ci("ctxt"))
        ctxt_in_lays.Link(lay);
      else
        input_lays.Link(lay);
    }
    else if((lay->layer_type == Layer::TARGET) || (lay->layer_type == Layer::OUTPUT)
            || lay->name.contains_ci("out"))
      output_lays.Link(lay);
    else
      input_lays.Link(lay);   // default to input -- who knows..
  }

  //////////////////////////////////////////////////////////////////////////////////
  // make specs

  if(!PVLV_Specs(net)) return false;

  String pvlvprefix = "PVLV";
  BaseSpec_Group* pvlvspgp = net->FindMakeSpecGp(pvlvprefix);

  if(output_lays.size > 0)
    PvlvSp("ExtRewLayer",ExtRewLayerSpec)->rew_type = ExtRewLayerSpec::OUT_ERR_REW;
  else
    PvlvSp("ExtRewLayer",ExtRewLayerSpec)->rew_type = ExtRewLayerSpec::EXT_REW;

  //////////////////////////////////////////////////////////////////////////////////
  // set positions & geometries

  net->SetNetFlag(Network::ABS_POS); // only works with absolute positioning
  net->SetNetFlag(Network::MANUAL_POS); // and turn off automatic..
  
  // todo: see about better options here..
  pos_pv->brain_area = ".*/.*/.*/.*/Lateral Hypothalamic area LHA";
  neg_pv->brain_area = ".*/.*/.*/.*/Lateral Hypothalamic area LHA";
  pos_bs->brain_area = ".*/.*/.*/.*/Lateral Hypothalamic area LHA";
  neg_bs->brain_area = ".*/.*/.*/.*/Lateral Hypothalamic area LHA";
  ext_rew->brain_area = ".*/.*/.*/.*/Lateral Hypothalamic area LHA";
  rew_targ->brain_area = ".*/.*/.*/.*/Lateral Hypothalamic area LHA";

  lat_amyg->brain_area = ".*/.*/.*/.*/Amygdala";
  baapd1->brain_area = ".*/.*/.*/.*/Amygdala";
  baepd2->brain_area = ".*/.*/.*/.*/Amygdala";
  capos->brain_area = ".*/.*/.*/.*/Amygdala Central Nucleus CNA";
  baand2->brain_area = ".*/.*/.*/.*/Amygdala";
  baend1->brain_area = ".*/.*/.*/.*/Amygdala";
  caneg->brain_area = ".*/.*/.*/.*/Amygdala Central Nucleus CNA";

  vsppd1->brain_area = ".*/.*/.*/.*/Nucleus Accumbens NAc";
  vsppd2->brain_area = ".*/.*/.*/.*/Nucleus Accumbens NAc";
  vspnd2->brain_area = ".*/.*/.*/.*/Nucleus Accumbens NAc";
  vspnd1->brain_area = ".*/.*/.*/.*/Nucleus Accumbens NAc";

  vsmpd1->brain_area = ".*/.*/.*/.*/Nucleus Accumbens NAc";
  vsmpd2->brain_area = ".*/.*/.*/.*/Nucleus Accumbens NAc";
  vsmnd2->brain_area = ".*/.*/.*/.*/Nucleus Accumbens NAc";
  vsmnd1->brain_area = ".*/.*/.*/.*/Nucleus Accumbens NAc";

  pptg_p->brain_area = ".*/.*/.*/.*/Ventral Tegmental Area VTA"; // todo
  vtap->brain_area = ".*/.*/.*/.*/Ventral Tegmental Area VTA";
  lhb->brain_area = ".*/.*/.*/.*/Lateral Habenula LHB";

  pptg_n->brain_area = ".*/.*/.*/.*/Ventral Tegmental Area VTA"; // todo
  vtan->brain_area = ".*/.*/.*/.*/Ventral Tegmental Area VTA";
  drn->brain_area = ".*/.*/.*/.*/Ventral Tegmental Area VTA"; // todo

  pos_pv->un_geom.SetXYN(1,1,1);
  pos_pv->gp_geom.SetXY(n_pos_pv, 1);  pos_pv->unit_groups = true;

  neg_pv->un_geom.SetXYN(1,1,1);
  neg_pv->gp_geom.SetXY(n_neg_pv, 1);  neg_pv->unit_groups = true;

  pos_bs->un_geom.SetXYN(1,1,1);
  pos_bs->gp_geom.SetXY(n_pos_pv, 1);  pos_bs->unit_groups = true;

  neg_bs->un_geom.SetXYN(1,1,1);
  neg_bs->gp_geom.SetXY(n_neg_pv, 1);  neg_bs->unit_groups = true;

  ext_rew->un_geom.SetXYN(3,1,3);
  rew_targ->un_geom.SetXYN(1,1,1);

  lat_amyg->un_geom.SetXYN(n_pos_pv + n_neg_pv,1,n_pos_pv + n_neg_pv);
  lat_amyg->unit_groups = true;

  baapd1->un_geom.SetXYN(1,1,1);
  baapd1->gp_geom.SetXY(n_pos_pv, 1);  baapd1->unit_groups = true;

  baepd2->un_geom.SetXYN(1,1,1);
  baepd2->gp_geom.SetXY(n_pos_pv, 1);  baepd2->unit_groups = true;

  capos->un_geom.SetXYN(1,1,1);
  capos->gp_geom.SetXY(n_pos_pv, 1);  capos->unit_groups = true;

  caneg->un_geom.SetXYN(1,1,1);
  caneg->gp_geom.SetXY(n_neg_pv, 1);  caneg->unit_groups = true;

  baand2->un_geom.SetXYN(1,1,1);
  baand2->gp_geom.SetXY(n_neg_pv, 1);  baand2->unit_groups = true;

  baend1->un_geom.SetXYN(1,1,1);
  baend1->gp_geom.SetXY(n_neg_pv, 1);  baend1->unit_groups = true;

  vsppd1->un_geom.SetXYN(1,1,1);
  vsppd1->gp_geom.SetXY(n_pos_pv, 1);  vsppd1->unit_groups = true;

  vsppd2->un_geom.SetXYN(1,1,1);
  vsppd2->gp_geom.SetXY(n_pos_pv, 1);  vsppd2->unit_groups = true;

  vspnd2->un_geom.SetXYN(1,1,1);
  vspnd2->gp_geom.SetXY(n_neg_pv, 1);  vspnd2->unit_groups = true;

  vspnd1->un_geom.SetXYN(1,1,1);
  vspnd1->gp_geom.SetXY(n_neg_pv, 1);  vspnd1->unit_groups = true;

  vsmpd1->un_geom.SetXYN(1,1,1);
  vsmpd1->gp_geom.SetXY(n_pos_pv, 1);  vsmpd1->unit_groups = true;

  vsmpd2->un_geom.SetXYN(1,1,1);
  vsmpd2->gp_geom.SetXY(n_pos_pv, 1);  vsmpd2->unit_groups = true;

  vsmnd2->un_geom.SetXYN(1,1,1);
  vsmnd2->gp_geom.SetXY(n_neg_pv, 1);  vsmnd2->unit_groups = true;

  vsmnd1->un_geom.SetXYN(1,1,1);
  vsmnd1->gp_geom.SetXY(n_neg_pv, 1);  vsmnd1->unit_groups = true;

  pptg_p->un_geom.SetXYN(1,1,1);
  vtap->un_geom.SetXYN(1,1,1);
  lhb->un_geom.SetXYN(1,1,1);
  pptg_n->un_geom.SetXYN(1,1,1);
  vtan->un_geom.SetXYN(1,1,1);
  drn->un_geom.SetXYN(1,1,1);

  int sp = 3;
  int neg_st = n_pos_pv * 2 + sp;
  int da_st = neg_st + n_neg_pv * 2 + sp;

  if(new_pv) {
    pv_gp->pos.SetXYZ(0,0,0);
    pos_pv->pos_abs.SetXYZ(0,0,0);
    neg_pv->PositionRightOf(pos_pv, sp);
    pos_bs->PositionBehind(pos_pv, sp);
    neg_bs->PositionBehind(neg_pv, sp);
    ext_rew->PositionBehind(pos_bs, sp);
    rew_targ->PositionBehind(neg_bs, sp);
    if(pv_gp->pos.z > 0 || pv_gp->pos.x > 0)
      pv_gp->MovePos(-pv_gp->pos.x, -pv_gp->pos.y, -pv_gp->pos.z);
  }

  if(new_amyg) {
    capos->PositionBehind(ext_rew, sp);
    caneg->PositionBehind(rew_targ, sp);
    baepd2->PositionBehind(capos, sp);
    baend1->PositionBehind(caneg, sp);
    baapd1->PositionBehind(baepd2, sp);
    baand2->PositionBehind(baend1, sp);
    lat_amyg->PositionBehind(baapd1, sp);
  }

  if(new_da) { 
    pptg_p->PositionRightOf(neg_pv, sp);
    vtap->PositionRightOf(pptg_p, sp);
    lhb->PositionRightOf(vtap, sp);
    pptg_n->PositionBehind(pptg_p, sp);
    vtan->PositionRightOf(pptg_n, sp);
    drn->PositionRightOf(vtan, sp);
  }

  if(new_vs) {
    vsppd1->PositionBehind(pptg_p, 2*sp);
    vspnd1->PositionRightOf(vsppd1, sp);
    vsppd2->PositionBehind(vsppd1, sp);
    vspnd2->PositionRightOf(vsppd2, sp);

    vsmpd1->PositionBehind(vsppd2, 2*sp);
    vsmnd1->PositionRightOf(vsmpd1, sp);
    vsmpd2->PositionBehind(vsmpd1, sp);
    vsmnd2->PositionRightOf(vsmpd2, sp);
  }

  //////////////////////////////////////////////////////////////////////////////////
  // apply specs to objects

  LeabraUnitSpec* pvlv_units = PvlvSp("PVLVUnits",LeabraUnitSpec);
  LeabraUnitSpec* pv_units = PvlvSp("PVUnits",LeabraUnitSpec);
  pos_pv->SetUnitSpec(pv_units);
  neg_pv->SetUnitSpec(pv_units);
  pos_bs->SetUnitSpec(pv_units);
  neg_bs->SetUnitSpec(pv_units);
  ext_rew->SetUnitSpec(pvlv_units);
  rew_targ->SetUnitSpec(pvlv_units);
  
  LeabraLayerSpec* pvsp = PvlvSp("PVLayers", LeabraLayerSpec);
  pos_pv->SetLayerSpec(pvsp);
  neg_pv->SetLayerSpec(pvsp);
  pos_bs->SetLayerSpec(pvsp);
  neg_bs->SetLayerSpec(pvsp);
  rew_targ->SetLayerSpec(pvsp);
  ext_rew->SetLayerSpec(PvlvSp("ExtRewLayer", ExtRewLayerSpec));

  lat_amyg->SetUnitSpec(PvlvSp("LatAmygUnits", LeabraUnitSpec));
  baapd1->SetUnitSpec(PvlvSp("BAAcqPosD1Units", BasAmygUnitSpec));
  baepd2->SetUnitSpec(PvlvSp("BAExtPosD2Units", BasAmygUnitSpec));
  capos->SetUnitSpec(PvlvSp("CeMUnits", LeabraUnitSpec));
  baand2->SetUnitSpec(PvlvSp("BAAcqNegD2Units", BasAmygUnitSpec));
  baend1->SetUnitSpec(PvlvSp("BAExtNegD1Units", BasAmygUnitSpec));
  caneg->SetUnitSpec(PvlvSp("CeMUnits", LeabraUnitSpec));

  LeabraLayerSpec* amygsp = PvlvSp("AmygLayer", LeabraLayerSpec);
  lat_amyg->SetLayerSpec(amygsp);
  baapd1->SetLayerSpec(amygsp);
  baepd2->SetLayerSpec(PvlvSp("AmygExtLayer", LeabraLayerSpec));
  capos->SetLayerSpec(amygsp);
  baand2->SetLayerSpec(amygsp);
  baend1->SetLayerSpec(PvlvSp("AmygExtLayer", LeabraLayerSpec));
  caneg->SetLayerSpec(amygsp);

  vsppd1->SetUnitSpec(PvlvSp("VSPatchPosD1Units", MSNUnitSpec));
  vsppd2->SetUnitSpec(PvlvSp("VSPatchPosD2Units", MSNUnitSpec));
  vspnd2->SetUnitSpec(PvlvSp("VSPatchNegD2Units", MSNUnitSpec));
  vspnd1->SetUnitSpec(PvlvSp("VSPatchNegD1Units", MSNUnitSpec));

  vsmpd1->SetUnitSpec(PvlvSp("VSMatrixPosD1Units", MSNUnitSpec));
  vsmpd2->SetUnitSpec(PvlvSp("VSMatrixPosD2Units", MSNUnitSpec));
  vsmnd2->SetUnitSpec(PvlvSp("VSMatrixNegD2Units", MSNUnitSpec));
  vsmnd1->SetUnitSpec(PvlvSp("VSMatrixNegD1Units", MSNUnitSpec));

  vsppd1->SetLayerSpec(PvlvSp("VSPatchLayer", LeabraLayerSpec));
  vsppd2->SetLayerSpec(PvlvSp("VSPatchLayer", LeabraLayerSpec));
  vspnd2->SetLayerSpec(PvlvSp("VSPatchLayer", LeabraLayerSpec));
  vspnd1->SetLayerSpec(PvlvSp("VSPatchLayer", LeabraLayerSpec));

  vsmpd1->SetLayerSpec(PvlvSp("VSMatrixLayer", LeabraLayerSpec));
  vsmpd2->SetLayerSpec(PvlvSp("VSMatrixLayer", LeabraLayerSpec));
  vsmnd2->SetLayerSpec(PvlvSp("VSMatrixLayer", LeabraLayerSpec));
  vsmnd1->SetLayerSpec(PvlvSp("VSMatrixLayer", LeabraLayerSpec));

  LeabraLayerSpec* dasp = PvlvSp("DALayers", LeabraLayerSpec);
  pptg_p->SetUnitSpec(PvlvSp("PPTgUnits", PPTgUnitSpec));
  pptg_p->SetLayerSpec(dasp);

  vtap->SetUnitSpec(PvlvSp("VTAUnits_p", VTAUnitSpec));
  vtap->SetLayerSpec(dasp);

  lhb->SetUnitSpec(PvlvSp("LHbRMTgUnits", LHbRMTgUnitSpec));
  lhb->SetLayerSpec(dasp);

  pptg_n->SetUnitSpec(PvlvSp("PPTgUnits", PPTgUnitSpec));
  pptg_n->SetLayerSpec(dasp);

  vtan->SetUnitSpec(PvlvSp("VTAUnits_n", VTAUnitSpec));
  vtan->SetLayerSpec(dasp);

  drn->SetUnitSpec(PvlvSp("DRNUnits", DRNUnitSpec));
  drn->SetLayerSpec(dasp);

  //////////////////////////////////////////////////////////////////////////////////
  // make projections

  MarkerConSpec* marker_cons = PvlvSp("PVLVMarkerCons", MarkerConSpec);
  LeabraConSpec* fix_cons = PvlvSp("PVLVFixedCons", LeabraConSpec);
  OneToOnePrjnSpec* onetoone = PvlvSp("PVLVOneToOne", OneToOnePrjnSpec);
  GpOneToOnePrjnSpec* gponetoone = PvlvSp("PVLVGpOneToOne", GpOneToOnePrjnSpec);
  FullPrjnSpec* fullprjn = PvlvSp("PVLVFullPrjn", FullPrjnSpec);
  SendDeepRawConSpec* pvtoba = PvlvSp("PVtoBA_DeepRaw", SendDeepRawConSpec);

  // net->FindMakePrjn(Layer* recv, Layer* send, prjn, conspec)

  net->FindMakePrjn(lat_amyg, vtap, fullprjn, marker_cons);
  net->FindMakePrjn(lat_amyg, vtan, fullprjn, marker_cons);
  // also stim in

  net->FindMakePrjn(baapd1, pos_pv, gponetoone, pvtoba);
  net->FindMakePrjn(baapd1, vtap, fullprjn, marker_cons);
  net->FindMakePrjn(baapd1, lat_amyg, fullprjn, PvlvSp("BasAmygCons_acq_pos",
                                                       BasAmygConSpec));
  net->FindMakePrjn(baapd1, baepd2, gponetoone, PvlvSp("BAExtToBAAcq_Inhib",
                                                       LeabraConSpec));
  
  net->FindMakePrjn(baepd2, baapd1, gponetoone, PvlvSp("BAAcqToBAExt_DeepMod",
                                                       SendDeepModConSpec));
  net->FindMakePrjn(baepd2, vtap, fullprjn, marker_cons);
  net->FindMakePrjn(baepd2, lat_amyg, fullprjn, PvlvSp("BasAmygCons_ext_pos",                                           BasAmygConSpec));
   // TODO: also fm context in..
  
  net->FindMakePrjn(capos, pos_pv, gponetoone, PvlvSp("FmPV_Fixed", LeabraConSpec));
  net->FindMakePrjn(capos, baapd1, gponetoone, PvlvSp("BAtoCA_Fixed", LeabraConSpec));

  net->FindMakePrjn(baand2, neg_pv, gponetoone, pvtoba);
  net->FindMakePrjn(baand2, vtap, fullprjn, marker_cons);
  net->FindMakePrjn(baand2, lat_amyg, fullprjn, PvlvSp("BasAmygCons_acq_neg", BasAmygConSpec));
  net->FindMakePrjn(baand2, baend1, gponetoone, PvlvSp("BAExtToBAAcq_Inhib", LeabraConSpec));
  
  net->FindMakePrjn(baend1, baand2, gponetoone, PvlvSp("BAAcqToBAExt_DeepMod", SendDeepModConSpec));
  net->FindMakePrjn(baend1, neg_pv, gponetoone, pvtoba);
  net->FindMakePrjn(baend1, vtap, fullprjn, marker_cons);
  net->FindMakePrjn(baend1, lat_amyg, fullprjn, PvlvSp("BasAmygCons_ext_neg", BasAmygConSpec));
  // TODO: also fm context in..
  
  net->FindMakePrjn(caneg, neg_pv, gponetoone, PvlvSp("FmPV_Fixed", LeabraConSpec));
  net->FindMakePrjn(caneg, baand2, gponetoone, fix_cons); // todo BAtoCA_Fixed??
  
  // user can lesion this if they want..
  net->FindMakePrjn(pos_pv, ext_rew, PvlvSp("PVFmExtRew", TesselPrjnSpec), fix_cons);
  net->FindMakePrjn(ext_rew, rew_targ, fullprjn, marker_cons);

  SendDeepModConSpec* bavsmod_cons = PvlvSp("BAAcqToVS_DeepMod", SendDeepModConSpec);
  SendDeepModConSpec* vsvsmod_cons = PvlvSp("VSMatrixToVSMatrix_DeepMod", SendDeepModConSpec);


  // patch, matrix:
  net->FindMakePrjn(vsppd1, vtap, fullprjn, marker_cons);
  net->FindMakePrjn(vsppd2, vtap, fullprjn, marker_cons);
  net->FindMakePrjn(vspnd2, vtap, fullprjn, marker_cons);
  net->FindMakePrjn(vspnd1, vtap, fullprjn, marker_cons);
  
  net->FindMakePrjn(vsmpd1, vtap, fullprjn, marker_cons);
  net->FindMakePrjn(vsmpd2, vtap, fullprjn, marker_cons);
  net->FindMakePrjn(vsmnd2, vtap, fullprjn, marker_cons);
  net->FindMakePrjn(vsmnd1, vtap, fullprjn, marker_cons);

  net->FindMakePrjn(vsppd1, baapd1, gponetoone, bavsmod_cons);
  net->FindMakePrjn(vsppd2, baapd1, gponetoone, bavsmod_cons);
  net->FindMakePrjn(vspnd2, baand2, gponetoone, bavsmod_cons);
  net->FindMakePrjn(vspnd1, baand2, gponetoone, bavsmod_cons);

  net->FindMakePrjn(vsmpd1, baapd1, gponetoone, bavsmod_cons);
  net->FindMakePrjn(vsmpd2, vsmpd1, gponetoone, vsvsmod_cons);
  net->FindMakePrjn(vsmnd2, baand2, gponetoone, bavsmod_cons);
  net->FindMakePrjn(vsmnd1, vsmnd2, gponetoone, vsvsmod_cons);
  // only stimtime (should be OFC) projections into patch

  LatAmygConSpec* la_cons = PvlvSp("LatAmygCons", LatAmygConSpec);
  BasAmygConSpec* baep_cons = PvlvSp("BasAmygCons_ext_pos", BasAmygConSpec);
  BasAmygConSpec* baen_cons = PvlvSp("BasAmygCons_ext_neg", BasAmygConSpec);
  
  for(i=0;i<time_in_lays.size;i++) {
    Layer* il = (Layer*)time_in_lays[i];
    net->FindMakePrjn(vsppd1, il, fullprjn, PvlvSp("VSPatchCons_ToPosD1", MSNConSpec));
    net->FindMakePrjn(vsppd2, il, fullprjn, PvlvSp("VSPatchCons_ToPosD2", MSNConSpec));
    net->FindMakePrjn(vspnd2, il, fullprjn, PvlvSp("VSPatchCons_ToNegD2", MSNConSpec));
    net->FindMakePrjn(vspnd1, il, fullprjn, PvlvSp("VSPatchCons_ToNegD1", MSNConSpec));
  }
  
  for(i=0;i<ctxt_in_lays.size;i++) {
    Layer* il = (Layer*)ctxt_in_lays[i];
    net->FindMakePrjn(baepd2, il, fullprjn, baep_cons);
    net->FindMakePrjn(baend1, il, fullprjn, baen_cons);
  }
  
  for(i=0;i<input_lays.size;i++) {
    Layer* il = (Layer*)input_lays[i];
    net->FindMakePrjn(lat_amyg, il, fullprjn, la_cons);

    net->FindMakePrjn(vsmpd1, il, fullprjn, PvlvSp("VSMatrixCons_ToPosD1", MSNConSpec));
    net->FindMakePrjn(vsmpd2, il, fullprjn, PvlvSp("VSMatrixCons_ToPosD2", MSNConSpec));
    net->FindMakePrjn(vsmnd2, il, fullprjn, PvlvSp("VSMatrixCons_ToNegD2", MSNConSpec));
    net->FindMakePrjn(vsmnd1, il, fullprjn, PvlvSp("VSMatrixCons_ToNegD1", MSNConSpec));
    
    if(time_in_lays.size == 0) { // take what we can..
      net->FindMakePrjn(vsppd1, il, fullprjn, PvlvSp("VSPatchCons_ToPosD1", MSNConSpec));
      net->FindMakePrjn(vsppd2, il, fullprjn, PvlvSp("VSPatchCons_ToPosD2", MSNConSpec));
      net->FindMakePrjn(vspnd2, il, fullprjn, PvlvSp("VSPatchCons_ToNegD2", MSNConSpec));
      net->FindMakePrjn(vspnd1, il, fullprjn, PvlvSp("VSPatchCons_ToNegD1", MSNConSpec));
    }
    if(ctxt_in_lays.size == 0) {
      net->FindMakePrjn(baepd2, il, fullprjn, baep_cons);
      net->FindMakePrjn(baend1, il, fullprjn, baen_cons);
    }
  }

  for(i=0;i<output_lays.size;i++) {
    Layer* ol = (Layer*)output_lays[i];
    net->FindMakePrjn(ext_rew, ol, fullprjn, marker_cons);
  }
  
  // core da nuclei
  net->FindMakePrjn(pptg_p, capos, fullprjn, fix_cons);

  net->FindMakePrjn(vtap, pptg_p, fullprjn, marker_cons);
  net->FindMakePrjn(vtap, lhb, fullprjn, marker_cons);
  net->FindMakePrjn(vtap, pos_pv, fullprjn, marker_cons);
  net->FindMakePrjn(vtap, vsppd1, fullprjn, marker_cons);
  net->FindMakePrjn(vtap, vsppd2, fullprjn, marker_cons);

  net->FindMakePrjn(lhb, pos_pv, fullprjn, marker_cons);
  net->FindMakePrjn(lhb, neg_pv, fullprjn, marker_cons);

  net->FindMakePrjn(lhb, vsppd1, fullprjn, marker_cons);
  net->FindMakePrjn(lhb, vsppd2, fullprjn, marker_cons);
  net->FindMakePrjn(lhb, vspnd2, fullprjn, marker_cons);
  net->FindMakePrjn(lhb, vspnd1, fullprjn, marker_cons);

  net->FindMakePrjn(lhb, vsmpd1, fullprjn, marker_cons);
  net->FindMakePrjn(lhb, vsmpd2, fullprjn, marker_cons);
  net->FindMakePrjn(lhb, vsmnd2, fullprjn, marker_cons);
  net->FindMakePrjn(lhb, vsmnd1, fullprjn, marker_cons);
  
  net->FindMakePrjn(pptg_n, caneg, fullprjn, fix_cons);

  net->FindMakePrjn(vtan, pptg_n, fullprjn, marker_cons);
  net->FindMakePrjn(vtan, lhb, fullprjn, marker_cons);
  net->FindMakePrjn(vtan, neg_pv, fullprjn, marker_cons);
  net->FindMakePrjn(vtan, vspnd2, fullprjn, marker_cons);
  net->FindMakePrjn(vtan, vspnd1, fullprjn, marker_cons);

  net->FindMakePrjn(drn, neg_pv, fullprjn, marker_cons);
  net->FindMakePrjn(drn, pos_pv, fullprjn, marker_cons);

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

bool LeabraWizard::PVLV_ConnectCSLayer(LeabraNetwork* net, LeabraLayer* sending_layer,
                                       bool disconnect) {
  taMisc::Error("PVLV_ConnectLayer: not yet implemented for latest PVLV version");
  if(TestError(!net || !sending_layer, "PVLV_ConnectLayer", "must specify a network and a sending layer!")) return false;

  LeabraProject* proj = GET_MY_OWNER(LeabraProject);
  if(proj) {
    proj->undo_mgr.SaveUndo(net, "Wizard::PVLV_ConnectCSLayer before -- actually saves network specifically");
  }

  Layer_Group* amyg_gp = net->FindLayerGroup("PVLV_Amyg");
  Layer_Group* vs_gp = net->FindLayerGroup("PVLV_VS");
  if(!amyg_gp || !vs_gp) {
    taMisc::Error("PVLV_Amyg or PVLV_VS layer groups not found!");
    return false;
  }

  LeabraLayer* lat_amyg = (LeabraLayer*)amyg_gp->FindName("LatAmyg");
  LeabraLayer* vsmpd1 = (LeabraLayer*)vs_gp->FindName("VSMatrixPosD1");
  LeabraLayer* vsmpd2 = (LeabraLayer*)vs_gp->FindName("VSMatrixPosD2");
  LeabraLayer* vsmnd2 = (LeabraLayer*)vs_gp->FindName("VSMatrixNegD2");
  LeabraLayer* vsmnd1 = (LeabraLayer*)vs_gp->FindName("VSMatrixNegD1");

  String pvlvprefix = "PVLV";
  BaseSpec_Group* pvlvspgp = net->FindMakeSpecGp(pvlvprefix);

  FullPrjnSpec* fullprjn = PvlvSp("PVLVFullPrjn", FullPrjnSpec);

  if(disconnect) {
    net->RemovePrjn(lat_amyg, sending_layer);
    net->RemovePrjn(vsmpd1, sending_layer);
    net->RemovePrjn(vsmpd2, sending_layer);
    net->RemovePrjn(vsmnd2, sending_layer);
    net->RemovePrjn(vsmnd1, sending_layer);
  }
  else {
    net->FindMakePrjn(lat_amyg, sending_layer, fullprjn, PvlvSp("LatAmygCons", LatAmygConSpec));

    net->FindMakePrjn(vsmpd1, sending_layer, fullprjn, PvlvSp("VSMatrixCons_ToPosD1", MSNConSpec));
    net->FindMakePrjn(vsmpd2, sending_layer, fullprjn, PvlvSp("VSMatrixCons_ToPosD2", MSNConSpec));
    net->FindMakePrjn(vsmnd2, sending_layer, fullprjn, PvlvSp("VSMatrixCons_ToNegD2", MSNConSpec));
    net->FindMakePrjn(vsmnd1, sending_layer, fullprjn, PvlvSp("VSMatrixCons_ToNegD1", MSNConSpec));
  }

  if(proj) {
    proj->undo_mgr.SaveUndo(net, "Wizard::PVLV_ConnectCSLayer after -- actually saves network specifically");
  }
  
  return true;
}

bool LeabraWizard::PVLV_OutToExtRew(LeabraNetwork* net, LeabraLayer* output_layer,
                                     bool disconnect) {
  if(TestError(!net || !output_layer, "PVLV_OutToExtRew", "must specify a network and an output layer!")) return false;

  LeabraProject* proj = GET_MY_OWNER(LeabraProject);
  if(proj) {
    proj->undo_mgr.SaveUndo(net, "Wizard::PVLV_OutToExtRew before -- actually saves network specifically");
  }

  Layer_Group* pv_gp = net->FindLayerGroup("PVLV_PV");

  if(!pv_gp) {
    taMisc::Error("PVLV_PV layer group not found!");
    return false;
  }

  LeabraLayer* ext_rew = (LeabraLayer*)pv_gp->FindName("ExtRew");

  String pvlvprefix = "PVLV";
  BaseSpec_Group* pvlvspgp = net->FindMakeSpecGp(pvlvprefix);

  FullPrjnSpec* fullprjn = PvlvSp("PVLVFullPrjn", FullPrjnSpec);
  MarkerConSpec* marker_cons = PvlvSp("PVLVMarkerCons", MarkerConSpec);
  if(TestError(!marker_cons, "PVLV_OutToExtRew",
               "marker_cons not found -- PVLV was not properly configured")) {
    return false;
  }

  if(disconnect) {
    net->RemovePrjn(ext_rew, output_layer);
  }
  else {
    net->FindMakePrjn(ext_rew, output_layer, fullprjn, marker_cons);
  }
  if(proj) {
    proj->undo_mgr.SaveUndo(net, "Wizard::PVLV_OutToExtRew after -- actually saves network specifically");
  }
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
  FMChild(MSNUnitSpec, matrix_go_units, pbwm_units, "MatrixGoUnits");
  FMChild(MSNUnitSpec, matrix_no_units, matrix_go_units, "MatrixNoGoUnits");
  FMChild(GPiInvUnitSpec, gpi_units, pbwm_units, "GPiUnits");
  FMChild(LeabraUnitSpec, gpe_units, pbwm_units, "GPeNoGoUnits");
  FMChild(PatchUnitSpec, patch_units, pbwm_units, "PatchUnits");
  FMChild(PFCUnitSpec, pfc_mnt_units, pbwm_units, "PFCmntUnits");
  FMChild(PFCUnitSpec, pfc_mnt_d_units, pfc_mnt_units, "PFCmntdUnits");
  FMChild(PFCUnitSpec, pfc_out_units, pbwm_units, "PFCoutUnits");
  FMChild(PFCUnitSpec, pfc_out_d_units, pfc_out_units, "PFCoutdUnits");
  FMChild(LeabraUnitSpec, pfc_trc_units, pbwm_units, "PFCtrcUnits");
  FMChild(LeabraUnitSpec, input_units, pbwm_units, "PFCInputUnits");

  ////////////	ConSpecs

  FMSpec(LeabraConSpec, bg_lrn_cons, pbwmspgp, "BgLrnCons");
  FMChild(MSNConSpec, mtx_cons_go, bg_lrn_cons, "MatrixConsGo");
  FMChild(MSNConSpec, mtx_cons_no, mtx_cons_go, "MatrixConsNoGo");
  FMChild(MSNConSpec, mtx_cons_fm_pfc, mtx_cons_go, "MatrixConsFmPFC");
  FMChild(LeabraDeltaConSpec, to_tans, bg_lrn_cons, "ToTANs");

  FMSpec(LeabraConSpec, pfc_lrn_cons, pbwmspgp, "PfcLrnCons");
  FMChild(DeepCtxtConSpec, deep_ctxt, pfc_lrn_cons, "PFCDeepCtxt");
  FMChild(LeabraConSpec, to_pfc, pfc_lrn_cons, "ToPFC");
  FMChild(LeabraConSpec, pfc_fm_trc, pfc_lrn_cons, "PFCfmTRC");
  FMChild(LeabraConSpec, pfc_to_trc, pfc_lrn_cons, "PFCtoTRC");
  FMChild(LeabraConSpec, to_out_cons, pfc_lrn_cons, "PFCtoOutput");

  FMSpec(LeabraConSpec, fix_cons, pbwmspgp, prefix + "FixedCons");
  FMChild(LeabraBiasSpec, fix_bias, fix_cons, prefix + "FixedBias");
  FMChild(MarkerConSpec, marker_cons, fix_cons, prefix + "MarkerCons");
  FMChild(SendDeepModConSpec, pfc_deep_mod, fix_cons, "PFCSendDeepMod");
  FMChild(LeabraConSpec, pfcd_mnt_out, fix_cons, "PFCdMntToOut");
  FMChild(SendDeepRawConSpec, d5b_lrn_cons, fix_cons, prefix + "DeepRawPlus");

  ////////////	LayerSpecs

  FMSpec(LeabraLayerSpec, pbwm_sp, pbwmspgp, prefix + "Layers");
  FMChild(LeabraLayerSpec, matrix_sp, pbwm_sp, "MatrixLayer");
  FMChild(LeabraLayerSpec, patch_sp, pbwm_sp, "PatchLayer");
  FMChild(LeabraLayerSpec, gpi_sp, pbwm_sp, "GPiLayer");
  FMChild(LeabraLayerSpec, gp_nogo_sp, gpi_sp, "GPeNoGoLayer");
  FMChild(LeabraLayerSpec, pfc_sp, pbwm_sp, "PFCLayer");
  FMChild(LeabraLayerSpec, pfc_trc_sp, pfc_sp, "PFCtrcLayer");

  ////////////	PrjnSpecs

  FMSpec(FullPrjnSpec, fullprjn, pbwmspgp, prefix + "FullPrjn");
  FMSpec(OneToOnePrjnSpec, onetoone, pbwmspgp, prefix + "OneToOne");
  FMSpec(GpOneToOnePrjnSpec, gponetoone, pbwmspgp, prefix + "GpOneToOne");

  FMSpec(BgPfcPrjnSpec, bgpfcprjn, pbwmspgp, "BgPfcPrjn");
  FMChild(BgPfcPrjnSpec, bgpfcprjn_toout, bgpfcprjn, "BgPfcPrjnToOut");
  FMChild(BgPfcPrjnSpec, bgpfcprjn_tomnt, bgpfcprjn, "BgPfcPrjnToMnt");

  FMSpec(TiledGpRFPrjnSpec, deep_prjn, pbwmspgp, "DeepToTRC");

  //////////////////////////////////////////////////////////////////////////////////
  // first: all the basic defaults from specs

  pbwmspgp->Defaults();

  //////////////////////////////////////////////////////////////////////////////////
  // set default spec parameters

  ////////////	UnitSpecs

  pbwm_units->bias_spec.SetSpec(fix_bias);
  pbwm_units->deep_raw_qtr = LeabraUnitSpec::Q2_Q4; // beta by default
  
  matrix_go_units->SetUnique("noise_type", true);
  matrix_go_units->noise_type = LeabraUnitSpec::NETIN_NOISE;
  matrix_go_units->SetUnique("noise", true);
  matrix_go_units->noise.var = 0.0005f;
  matrix_go_units->SetUnique("noise_adapt", true);
  matrix_go_units->noise_adapt.trial_fixed = true;
  matrix_go_units->SetUnique("dar", true);
  matrix_go_units->dar = MSNUnitSpec::D1R;
  matrix_go_units->SetUnique("matrix_patch", true);
  matrix_go_units->matrix_patch = MSNUnitSpec::MATRIX;
  matrix_go_units->SetUnique("dorsal_ventral", true);
  matrix_go_units->dorsal_ventral = MSNUnitSpec::DORSAL;
  matrix_go_units->SetUnique("deep", true);
  matrix_go_units->deep.on = true;
  matrix_go_units->deep.role = DeepSpec::SUPER;
  matrix_go_units->deep.mod_min = 0.97f; // key!

  matrix_no_units->SetUnique("noise_type", false);
  matrix_no_units->SetUnique("noise", false);
  matrix_no_units->SetUnique("noise_adapt", false);
  matrix_no_units->SetUnique("deep", false);
  matrix_no_units->SetUnique("dar", true);
  matrix_no_units->dar = MSNUnitSpec::D2R;
  matrix_no_units->SetUnique("matrix_patch", true);
  matrix_no_units->matrix_patch = MSNUnitSpec::MATRIX;
  matrix_no_units->SetUnique("dorsal_ventral", true);
  matrix_no_units->dorsal_ventral = MSNUnitSpec::DORSAL;

  gpe_units->SetUnique("deep", true);
  gpe_units->deep.on = true;
  gpe_units->deep.role = DeepSpec::SUPER;

  pfc_mnt_units->SetUnique("deep", true);
  pfc_mnt_units->deep.on = true;
  pfc_mnt_units->deep.role = DeepSpec::SUPER;
  pfc_mnt_units->maint.s_mnt_min = 0.3f;
  pfc_mnt_units->maint.s_mnt_max = 0.3f;
  pfc_mnt_units->maint.max_mnt = 100;

  pfc_mnt_d_units->SetUnique("deep", true);
  pfc_mnt_d_units->deep.on = true;
  pfc_mnt_d_units->deep.role = DeepSpec::DEEP;
  
  pfc_out_units->SetUnique("deep", true);
  pfc_mnt_units->deep.on = true;
  pfc_mnt_units->deep.role = DeepSpec::SUPER;
  pfc_out_units->SetUnique("gate", true);
  pfc_out_units->SetUnique("maint", true);
  pfc_out_units->gate.out_gate = true;
  pfc_out_units->maint.max_mnt = 1;
  // todo: many other params
  pfc_out_units->n_dyns = 1;

  pfc_out_d_units->SetUnique("deep", true);
  pfc_out_d_units->deep.on = true;
  pfc_out_d_units->deep.role = DeepSpec::DEEP;
  
  pfc_trc_units->SetUnique("deep", true);
  pfc_trc_units->deep.on = true;
  pfc_trc_units->deep.role = DeepSpec::TRC;
  pfc_trc_units->deep.trc_p_only_m = true;
  pfc_trc_units->deep.trc_thal_gate = true;

  input_units->SetUnique("deep", true);
  input_units->deep.on = true;
  input_units->deep.role = DeepSpec::SUPER;
  input_units->deep.raw_thr_rel = 0.1f;
  

  ////////////	ConSpecs

  bg_lrn_cons->SetUnique("lrate", true);
  bg_lrn_cons->lrate = 0.01f;
  bg_lrn_cons->SetUnique("learn_qtr", true);
  bg_lrn_cons->learn_qtr = LeabraConSpec::Q2_Q4; // beta by default
  bg_lrn_cons->SetUnique("wt_limits", true);
  bg_lrn_cons->wt_limits.sym = false;

  mtx_cons_go->SetUnique("wt_limits", false);
  mtx_cons_go->SetUnique("lrate", true);
  mtx_cons_go->lrate = .01f;
  mtx_cons_go->SetUnique("wt_sig", true);
  mtx_cons_go->wt_sig.gain = 1.0f;
  mtx_cons_go->SetUnique("su_act_var", true);
  mtx_cons_go->su_act_var = MSNConSpec::ACT_EQ;
  mtx_cons_go->SetUnique("ru_act_var", true);
  mtx_cons_go->ru_act_var = MSNConSpec::ACT_EQ;
  mtx_cons_go->SetUnique("learn_rule", true);
  mtx_cons_go->learn_rule = MSNConSpec::TRACE_THAL;

  mtx_cons_no->SetUnique("wt_limits", false);
  mtx_cons_no->SetUnique("lrate", false);
  mtx_cons_no->SetUnique("wt_sig", false);
  mtx_cons_no->SetUnique("su_act_var", false);
  mtx_cons_no->SetUnique("ru_act_var", false);
  mtx_cons_no->SetUnique("learn_rule", false);

  mtx_cons_fm_pfc->SetUnique("wt_limits", false);
  mtx_cons_fm_pfc->SetUnique("lrate", false);
  mtx_cons_fm_pfc->SetUnique("wt_sig", false);
  mtx_cons_fm_pfc->SetUnique("su_act_var", false);
  mtx_cons_fm_pfc->SetUnique("ru_act_var", false);
  mtx_cons_fm_pfc->SetUnique("learn_rule", false);
  mtx_cons_fm_pfc->SetUnique("wt_scale", true);
  mtx_cons_fm_pfc->wt_scale.rel = 0.1f;
  
  to_tans->SetUnique("wt_limits", false);
  to_tans->SetUnique("learn_qtr", true);
  to_tans->learn_qtr = LeabraConSpec::Q4; // only get learn at end
  to_tans->SetUnique("lrate", true);
  to_tans->lrate = 0.1f;

  pfc_lrn_cons->SetUnique("lrate", true);
  pfc_lrn_cons->lrate = 0.004f;
  pfc_lrn_cons->learn_qtr = LeabraConSpec::Q2_Q4;

  deep_ctxt->SetUnique("wt_scale", true); // just make sure 1,1

  to_pfc->SetUnique("wt_scale", true); // just make sure 1,1

  pfc_fm_trc->SetUnique("wt_scale", true);
  pfc_fm_trc->wt_scale.rel = 0.2f;

  // pfc_to_trc = nothing
  
  to_out_cons->SetUnique("lrate", true);
  to_out_cons->lrate = 0.02f;
  to_out_cons->SetUnique("learn_qtr", true);
  to_out_cons->learn_qtr = LeabraConSpec::Q4;
  to_out_cons->SetUnique("wt_scale", true);
  to_out_cons->wt_scale.abs = 4.0f;
  
  fix_cons->SetUnique("rnd", true);
  fix_cons->rnd.mean = 0.8f;
  fix_cons->rnd.var = 0.0f;
  fix_cons->SetUnique("wt_limits", true);
  fix_cons->wt_limits.sym = false;
  fix_cons->SetUnique("lrate", true);
  fix_cons->lrate = 0.0f;
  fix_cons->SetUnique("learn", true);
  fix_cons->learn = false;

  pfc_deep_mod->rnd.mean = 0.8f;
  pfc_deep_mod->rnd.var = 0.0f;
  pfc_deep_mod->wt_limits.sym = false;

  ////////////	LayerSpecs

  matrix_sp->SetUnique("lay_inhib", true);
  matrix_sp->lay_inhib.on = false;
  matrix_sp->SetUnique("unit_gp_inhib", true);
  matrix_sp->unit_gp_inhib.on = true;
  matrix_sp->unit_gp_inhib.gi = 2.3f;
  matrix_sp->unit_gp_inhib.ff = 1.0f;
  matrix_sp->unit_gp_inhib.fb = 0.0f;
  matrix_sp->SetUnique("lay_inhib", true);
  matrix_sp->lay_inhib.on = true;
  matrix_sp->lay_inhib.gi = 1.9f;
  matrix_sp->SetUnique("avg_act", true);
  matrix_sp->avg_act.init = 0.4f;
  matrix_sp->avg_act.fixed = true;
  matrix_sp->SetUnique("inhib_misc", true);
  matrix_sp->inhib_misc.self_fb = 0.3f;
  matrix_sp->SetUnique("del_inhib", true);
  matrix_sp->del_inhib.on = true;
  matrix_sp->del_inhib.prv_trl = 0.0f;
  matrix_sp->del_inhib.prv_q = 0.03f;

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
  gpi_sp->lay_inhib.gi = 2.1f;
  gpi_sp->lay_inhib.ff = 1.0f;
  gpi_sp->lay_inhib.fb = 0.5f;
  gpi_sp->SetUnique("unit_gp_inhib", true);
  gpi_sp->unit_gp_inhib.on = false;
  gpi_sp->SetUnique("avg_act", true);
  gpi_sp->avg_act.init = 0.2f;
  gpi_sp->avg_act.fixed = true;
  gpi_sp->UpdateAfterEdit();       // spread before override

  gp_nogo_sp->SetUnique("lay_inhib", true);
  gp_nogo_sp->lay_inhib.gi = 2.2f;
  
  pfc_sp->SetUnique("lay_inhib", true);
  pfc_sp->lay_inhib.on = false;
  pfc_sp->SetUnique("unit_gp_inhib", true);
  pfc_sp->unit_gp_inhib.on = true;
  pfc_sp->unit_gp_inhib.gi = 2.1f;
  pfc_sp->unit_gp_inhib.ff = 1.0f;
  pfc_sp->unit_gp_inhib.fb = 0.5f;
  pfc_sp->SetUnique("avg_act", true);
  pfc_sp->avg_act.init = 0.2f;
  pfc_sp->avg_act.fixed = true; // use fixed..
  pfc_sp->SetUnique("decay", true);
  pfc_sp->decay.trial = 0.0f;

  ////////////	PrjnSpecs

  bgpfcprjn_toout->SetUnique("cross_connect", true);
  bgpfcprjn_toout->cross_connect = true;
  bgpfcprjn_toout->SetUnique("connect_as", true);
  bgpfcprjn_toout->connect_as = "PFCout";

  bgpfcprjn_tomnt->SetUnique("cross_connect", true);
  bgpfcprjn_tomnt->cross_connect = true;
  bgpfcprjn_tomnt->SetUnique("connect_as", true);
  bgpfcprjn_tomnt->connect_as = "PFCmnt";

  deep_prjn->send_gp_size = 1;
  deep_prjn->send_gp_skip = 1;
  deep_prjn->send_gp_start = 0;
  deep_prjn->wrap = true;
  deep_prjn->init_wts = true;
  deep_prjn->wts_type = TiledGpRFPrjnSpec::GAUSSIAN;
  deep_prjn->full_gauss.on = true;
  deep_prjn->full_gauss.sigma = 1.2f;
  deep_prjn->full_gauss.ctr_mv = 0.8f;
  deep_prjn->full_gauss.wrap_wts = false;
  deep_prjn->gp_gauss.on = true;
  deep_prjn->gp_gauss.sigma = 1.2f;
  deep_prjn->gp_gauss.ctr_mv = 0.8f;
  deep_prjn->gp_gauss.wrap_wts = false;
  deep_prjn->wt_range.min = 0.3f;
  deep_prjn->wt_range.max = 0.7f;
  
  ////////////  Fix PVLV Specs!

  MSNConSpec* vspatch_cons = PvlvSp("VSPatchCons_ToPosD1", MSNConSpec);
  vspatch_cons->su_act_var = MSNConSpec::ACT_P;
  vspatch_cons->ru_act_var = MSNConSpec::ACT_P;
  vspatch_cons->learn_rule = MSNConSpec::DA_HEBB_VS; // def needs vs
  vspatch_cons->rnd.mean = 0.01f;
  vspatch_cons->lrate = 0.04f;

  LHbRMTgUnitSpec* lhbrmtg_units = PvlvSp("LHbRMTgUnits", LHbRMTgUnitSpec);
  lhbrmtg_units->lhb.patch_cur = true;

  VTAUnitSpec* vtap_units = PvlvSp("VTAUnits_p", VTAUnitSpec);
  vtap_units->da.patch_cur = true;

  ///////// Update All!

  net->specs.UpdateAllSpecs();
  pbwmspgp->UpdateAllSpecs();

  //////////////////////////////////////////////////////////////////////////////////
  // control panel

  LeabraProject* proj = GET_MY_OWNER(LeabraProject);
  ControlPanel* cp = proj->FindMakeControlPanel(prefix);
  if(cp != NULL) {
    // cp->SetUserData("user_pinned", true);

    String subgp;
    subgp = "";

    pbwm_units->AddToControlPanelNm("deep_raw_qtr", cp, "pbwm", subgp,
                                    "set to Q2, Q4 for beta frequency updating -- Q4 for just alpha -- coordinate with bg_lrn_learn_qtr setting!");
    bg_lrn_cons->AddToControlPanelNm("learn_qtr", cp, "bg_lrn", subgp);
    
    subgp = "PFC";

    pfc_mnt_units->AddToControlPanelNm("pfc", cp, "pfc_mnt", subgp);
    // pfc_mnt_units->AddToControlPanelNm("deep", cp, "pfc_mnt", subgp);

    pfc_out_units->AddToControlPanelNm("pfc", cp, "pfc_out", subgp);
    // pfc_out_units->AddToControlPanelNm("deep", cp, "pfc_out", subgp);
    
    pfc_sp->AddToControlPanelNm("unit_gp_inhib", cp, "pfc", subgp);
    pfc_sp->AddToControlPanelNm("lay_inhib", cp, "pfc", subgp);
    pfc_sp->AddToControlPanelNm("avg_act", cp, "pfc", subgp);

    pfc_lrn_cons->AddToControlPanelNm("lrate", cp, "pfc_lrn", subgp);
    pfc_lrn_cons->AddToControlPanelNm("learn_qtr", cp, "pfc_lrn", subgp);
    // to_pfc->AddToControlPanelNm("wt_scale", cp, "to_pfc", subgp);

    pfc_fm_trc->AddToControlPanelNm("wt_scale", cp, "pfc_fm_trc", subgp);
    
    subgp = "BG";
    matrix_sp->AddToControlPanelNm("unit_gp_inhib", cp, "matrix", subgp);
    matrix_sp->AddToControlPanelNm("lay_inhib", cp, "matrix", subgp);
    matrix_sp->AddToControlPanelNm("inhib_misc", cp, "matrix", subgp);
    matrix_sp->AddToControlPanelNm("del_inhib", cp, "matrix", subgp);

    matrix_go_units->deep.AddToControlPanelNm("mod_min", cp, "matrix_deep", subgp,
                                              "Controls strength of bias for output gating to occur in stripes that are already maintaining information -- PFCmnt deep modulation of output gating");

    mtx_cons_go->AddToControlPanelNm("lrate", cp, "matrix", subgp,
                                     "Default Matrix lrate is .01");
    mtx_cons_go->AddToControlPanelNm("trace", cp, "matrix", subgp);
    mtx_cons_go->AddToControlPanelNm("matrix", cp, "matrix", subgp);
    mtx_cons_fm_pfc->AddToControlPanelNm("wt_scale", cp, "matrix", subgp);

    gpi_units->AddToControlPanelNm("gpi", cp, "gpi", subgp);

    gpi_sp->AddToControlPanelNm("lay_inhib", cp, "gpi", subgp);
    gp_nogo_sp->AddToControlPanelNm("lay_inhib", cp, "gpe_nogo", subgp);

    cp->EditPanel(true, true);
  }
  return true;
}

static void lay_set_geom(LeabraLayer* lay, int gp_x, int gp_y, int n_un_x = -1,
                         int n_un_y = -1, bool sp = true) {
  lay->unit_groups = true;
  lay->gp_geom.SetXY(gp_x, gp_y);
  if(n_un_x > 0) {
    lay->un_geom.SetXY(n_un_x, n_un_y);
  }
  if(sp) {
    lay->gp_spc.x = 1;
    lay->gp_spc.y = 1;
  }
  lay->UpdateAfterEdit();
}

static void set_n_stripes(LeabraNetwork* net, const String& gpnm, const String& nm,
                          int pfc_gp_x, int pfc_gp_y, int n_un_x, int n_un_y, bool sp)
{
  Layer_Group* laygp = (Layer_Group*)net->layers.gp.FindName(gpnm);
  if(!laygp) return;
  LeabraLayer* lay = (LeabraLayer*)laygp->FindLeafName(nm);
  if(!lay) return;
  lay_set_geom(lay, pfc_gp_x, pfc_gp_y, n_un_x, n_un_y, sp);
}

bool LeabraWizard::PBWM_SetNStripes(LeabraNetwork* net, int pfc_gp_x, int pfc_gp_y,
                                    int n_matrix_units_x, int n_matrix_units_y,
				    int n_pfc_units_x, int n_pfc_units_y,
                                    const String& prefix) {
  if(TestError(!net, "PBWM_SetNStripes", "network is NULL -- only makes sense to run on an existing network -- aborting!"))
    return false;

  // this is called as a subroutine a lot too so don't save here -- could do impl but
  // not really worth it..
//   LeabraProject* proj = GET_MY_OWNER(LeabraProject);
//   if(proj) {
//     proj->undo_mgr.SaveUndo(net, "Wizard::PBWM_SetNStripes -- actually saves network specifically");
//   }

  int bg_gp_x = 2 * pfc_gp_x;
  int bg_gp_y = pfc_gp_y;

  set_n_stripes(net, prefix, "MatrixGo", bg_gp_x, bg_gp_y,
                n_matrix_units_x, n_matrix_units_y, true);
  set_n_stripes(net, prefix, "MatrixNoGo", bg_gp_x, bg_gp_y,
                n_matrix_units_x, n_matrix_units_y, true);
  set_n_stripes(net, prefix, "PFCmnt_patch",  pfc_gp_x, pfc_gp_y, 1, 1,
                true);

  set_n_stripes(net, prefix, "GPi", bg_gp_x, bg_gp_y, -1, -1, true);
  set_n_stripes(net, prefix, "GPeNoGo", bg_gp_x, bg_gp_y, -1, -1, true);

  set_n_stripes(net, prefix, "PFCmnt",  pfc_gp_x, pfc_gp_y,
                n_pfc_units_x, n_pfc_units_y, true);
  set_n_stripes(net, prefix, "PFCmnt_trc",  pfc_gp_x, pfc_gp_y,
                -1, -1, true);
  set_n_stripes(net, prefix, "PFCout",  pfc_gp_x, pfc_gp_y,
                n_pfc_units_x, n_pfc_units_y, true);
  set_n_stripes(net, prefix, "PFCout_deep",  pfc_gp_x, pfc_gp_y,
                n_pfc_units_x, n_pfc_units_y, true);
  set_n_stripes(net, prefix, "PFCmnt_deep",  pfc_gp_x, pfc_gp_y,
                n_pfc_units_x, n_pfc_units_y, true);

  net->Build();
  return true;
}

bool LeabraWizard::PBWM(LeabraNetwork* net, int pfc_gp_x, int pfc_gp_y,
                        bool add_on, const String& prefix) {
  if(!net) {
    LeabraProject* proj = GET_MY_OWNER(LeabraProject);
    net = (LeabraNetwork*)proj->GetNewNetwork();
    if(TestError(!net, "PBWM", "network is NULL and could not make a new one -- aborting!"))
      return false;
    if(!StdNetwork()) return false;
  }

  // first configure PVLV system..
  if(!add_on) {
    if(TestError(!PVLV(net, 1, 1, false), "PBWM", "could not make PVLV")) return false;
  }

  String msg = "Configuring PBWM (Prefrontal-cortex Basal-ganglia Working Memory)       Layers:\n\n\
There are some things you should check after this automatic configuration \
process completes (this note will be repeated when things complete --- there may be some \
messages in the interim):\n\n";

  String man_msg = "1. Check the bidirectional connections between the PFC and all appropriate layers.  \
Except for special intra-PFC connections, the conspecs into all superficial PFC layers \
should usually be ToPFC conspecs; those out from PFC layers should be PFCtoOutput or regular learning conspecs.\n\n\
2. Check the projections into the Matrix layers -- both Go and NoGo should typically \
receive the same patterns of connections, and the wizard just connects everything to \
everything -- more selective connection patterns usually work better overall.";

  msg += man_msg + "\n\nThe configuration will now be checked and a number of default parameters \
will be set.  If there are any actual errors which must be corrected before \
the network will run, you will see a message to that effect --- you will then need to \
re-run this configuration process to make sure everything is OK.  When you press \
Re/New/Init on the control process these same checks will be performed, so you \
can be sure everything is ok.";
  taMisc::Confirm(msg);

  net->RemoveUnits();

  //////////////////////////////////////////////////////////////////////////////////
  // make layers

  // Harvest from the PVLV function..
  Layer_Group* pvlv_laygp_da = (Layer_Group*)net->layers.gp.FindName("PVLV_DA");
  Layer_Group* pvlv_laygp_pv = (Layer_Group*)net->layers.gp.FindName("PVLV_PV");

  LeabraLayer* rew_targ_lay = NULL;
  LeabraLayer* ext_rew = NULL;
  LeabraLayer* vta = NULL;
  LeabraLayer* pos_pv = NULL;
  LeabraLayer* vspatch_posd1 = NULL;

  if(pvlv_laygp_pv) {
    pos_pv = (LeabraLayer*)pvlv_laygp_pv->FindName("PosPV");
    rew_targ_lay = (LeabraLayer*)pvlv_laygp_pv->FindName("RewTarg");
    ext_rew = (LeabraLayer*)pvlv_laygp_pv->FindName("ExtRew");
    vspatch_posd1 = (LeabraLayer*)pvlv_laygp_pv->FindName("VSPatchPosD1");
  }
  if(pvlv_laygp_da) {
    vta = (LeabraLayer*)pvlv_laygp_da->FindName("VTAp");
  }

  pos_pv->layer_type = Layer::HIDDEN; // not an input layer anymore
  
  bool new_pbwm_laygp = false;
  Layer_Group* pbwm_laygp = net->FindMakeLayerGroup(prefix, NULL, new_pbwm_laygp);

  LeabraLayer* matrix_go = NULL;
  LeabraLayer* matrix_nogo = NULL;
  LeabraLayer* matrix_tan = NULL;
  LeabraLayer* patch = NULL;
  LeabraLayer* gpi = NULL;
  LeabraLayer* gpenogo = NULL;
  LeabraLayer* pfc_mnt = NULL;
  LeabraLayer* pfc_mnt_trc = NULL;
  LeabraLayer* pfc_out = NULL;
  LeabraLayer* pfc_mnt_d = NULL;
  LeabraLayer* pfc_out_d = NULL;

  bool new_matrix = false;
  gpenogo = (LeabraLayer*)pbwm_laygp->FindMakeLayer("GPeNoGo", NULL);
  gpi = (LeabraLayer*)pbwm_laygp->FindMakeLayer("GPi", NULL);
  matrix_go = (LeabraLayer*)pbwm_laygp->FindMakeLayer("MatrixGo", NULL, new_matrix);
  matrix_nogo = (LeabraLayer*)pbwm_laygp->FindMakeLayer("MatrixNoGo", NULL);
  matrix_tan = (LeabraLayer*)pbwm_laygp->FindMakeLayer("MatrixTAN", NULL);
  patch = (LeabraLayer*)pbwm_laygp->FindMakeLayer("PFCmnt_patch", NULL);
  bool new_pfc  = false;
  pfc_mnt = (LeabraLayer*)pbwm_laygp->FindMakeLayer("PFCmnt", NULL, new_pfc);
  pfc_mnt_d = (LeabraLayer*)pbwm_laygp->FindMakeLayer("PFCmnt_deep", NULL);
  pfc_mnt_trc = (LeabraLayer*)pbwm_laygp->FindMakeLayer("PFCmnt_trc", NULL, new_pfc);
  pfc_out = (LeabraLayer*)pbwm_laygp->FindMakeLayer("PFCout", NULL);
  pfc_out_d = (LeabraLayer*)pbwm_laygp->FindMakeLayer("PFCout_deep", NULL);

  //////////////////////////////////////////////////////////////////////////////////
  // collect layer groups

  int mx_z1 = 0;         // max x coordinate on layer z=1
  int mx_z2 = 0;         // z=2
  Layer_Group other_lays;   Layer_Group hidden_lays;
  Layer_Group output_lays;  Layer_Group stim_input_lays;
  Layer_Group task_input_lays;
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
    if(lay->layer_type == Layer::HIDDEN) {
      hidden_lays.Link(lay);
    }
    else if((lay->layer_type == Layer::INPUT) || lay->name.contains("In")) {
      if(lay->name.contains("Ctrl") || lay->name.contains("Task")) {
        task_input_lays.Link(lay);
      }
      else {
        stim_input_lays.Link(lay);   // default to input
      }
    }
    else if(lay->name.contains("Out")) {
      output_lays.Link(lay);
    }
    else if(lay->name.contains("Ctrl") || lay->name.contains("Task")) {
      task_input_lays.Link(lay);
    }
    else {
      stim_input_lays.Link(lay);   // default to input
    }
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

  matrix_go->SetUnitSpec(PbwmSp("MatrixGoUnits",LeabraUnitSpec));
  matrix_go->SetLayerSpec(PbwmSp("MatrixLayer",LeabraLayerSpec));

  matrix_nogo->SetUnitSpec(PbwmSp("MatrixNoGoUnits",LeabraUnitSpec));
  matrix_nogo->SetLayerSpec(PbwmSp("MatrixLayer",LeabraLayerSpec));

  matrix_tan->SetUnitSpec(PvlvSp("TANUnits",TANUnitSpec));
  matrix_tan->SetLayerSpec(PvlvSp("PVLayers",LeabraLayerSpec));

  patch->SetUnitSpec(PbwmSp("PatchUnits",PatchUnitSpec));
  patch->SetLayerSpec(PbwmSp("PatchLayer",LeabraLayerSpec));

  gpi->SetUnitSpec(PbwmSp("GPiUnits",GPiInvUnitSpec));
  gpi->SetLayerSpec(PbwmSp("GPiLayer",LeabraLayerSpec));

  gpenogo->SetUnitSpec(PbwmSp("GPeNoGoUnits",LeabraUnitSpec));
  gpenogo->SetLayerSpec(PbwmSp("GPeNoGoLayer",LeabraLayerSpec));

  pfc_mnt->SetUnitSpec(PbwmSp("PFCmntUnits",PFCUnitSpec));
  pfc_mnt->SetLayerSpec(PbwmSp("PFCLayer",LeabraLayerSpec));

  pfc_mnt_d->SetUnitSpec(PbwmSp("PFCmntdUnits",PFCUnitSpec));
  pfc_mnt_d->SetLayerSpec(PbwmSp("PFCLayer",LeabraLayerSpec));

  pfc_mnt_trc->SetUnitSpec(PbwmSp("PFCtrcUnits",LeabraUnitSpec));
  pfc_mnt_trc->SetLayerSpec(PbwmSp("PFCtrcLayer",LeabraLayerSpec));

  pfc_out->SetUnitSpec(PbwmSp("PFCoutUnits",PFCUnitSpec));
  pfc_out->SetLayerSpec(PbwmSp("PFCLayer",LeabraLayerSpec));

  pfc_out_d->SetUnitSpec(PbwmSp("PFCoutdUnits",PFCUnitSpec));
  pfc_out_d->SetLayerSpec(PbwmSp("PFCLayer",LeabraLayerSpec));

  //////////////////////////////////////////////////////////////////////////////////
  // make projections

  Projection* prjn = NULL;
  MarkerConSpec* marker_cons = PbwmSp(prefix + "MarkerCons", MarkerConSpec);
  GpOneToOnePrjnSpec* gponetoone = PbwmSp(prefix + "GpOneToOne",GpOneToOnePrjnSpec);
  FullPrjnSpec* fullprjn = PbwmSp(prefix + "FullPrjn", FullPrjnSpec);
  OneToOnePrjnSpec* onetoone = PbwmSp(prefix + "OneToOne", OneToOnePrjnSpec);
  BgPfcPrjnSpec* bgpfcprjn = PbwmSp("BgPfcPrjn", BgPfcPrjnSpec);

  LeabraConSpec* fix_cons = PbwmSp(prefix + "FixedCons", LeabraConSpec);
  SendDeepModConSpec* pfc_send_deep = PbwmSp("PFCSendDeepMod", SendDeepModConSpec);

 //	  	 	   to		 from		prjn_spec	con_spec

  net->FindMakePrjn(matrix_go, gpi, gponetoone, marker_cons);
  net->FindMakePrjn(matrix_go, vta, fullprjn, marker_cons);
  net->FindMakePrjn(matrix_go, matrix_tan, fullprjn, marker_cons);
  net->FindMakePrjn(matrix_go, patch, bgpfcprjn, marker_cons);
  net->FindMakePrjn(matrix_go, pfc_mnt_d, PbwmSp("BgPfcPrjnToOut", BgPfcPrjnSpec),
                    pfc_send_deep);
  net->FindMakePrjnAdd(matrix_go, pfc_mnt_d, fullprjn, PbwmSp("MatrixConsFmPFC", MSNConSpec));
  // add makes 2nd prjn from same layer!!
  // also ctrl input

  net->FindMakePrjn(matrix_nogo, gpi, gponetoone, marker_cons);
  net->FindMakePrjn(matrix_nogo, vta, fullprjn, marker_cons);
  net->FindMakePrjn(matrix_nogo, matrix_tan, fullprjn, marker_cons);
  net->FindMakePrjn(matrix_nogo, patch, bgpfcprjn, marker_cons);
  net->FindMakePrjn(matrix_nogo, pfc_mnt_d, PbwmSp("BgPfcPrjnToOut", BgPfcPrjnSpec),
                    pfc_send_deep);
  net->FindMakePrjnAdd(matrix_nogo, pfc_mnt_d, fullprjn, PbwmSp("MatrixConsFmPFC", MSNConSpec));
  // add makes 2nd prjn from same layer!!
  // also ctrl input

  if(pos_pv) {
    prjn = net->FindMakePrjn(matrix_tan, pos_pv, fullprjn, marker_cons);
    if(rew_targ_lay) 
      prjn->off = true;
  }
  if(vspatch_posd1) {
    prjn = net->FindMakePrjn(matrix_tan, vspatch_posd1, fullprjn, marker_cons);
    if(rew_targ_lay) 
      prjn->off = true;
  }
  if(rew_targ_lay) {
    net->FindMakePrjn(matrix_tan, rew_targ_lay, fullprjn, marker_cons);
  }
  // also stim, ctrl

  net->FindMakePrjn(patch, pfc_mnt_d, gponetoone, fix_cons);

  net->FindMakePrjn(gpenogo, matrix_nogo, gponetoone, fix_cons);

  net->FindMakePrjn(gpi, matrix_go, gponetoone, fix_cons);
  net->FindMakePrjn(gpi, gpenogo, gponetoone, fix_cons);

  net->FindMakePrjn(pfc_mnt, gpi, bgpfcprjn, marker_cons);
  net->FindMakePrjn(pfc_mnt, pfc_mnt_d, onetoone, pfc_send_deep);
  net->FindMakePrjn(pfc_mnt, pfc_out, onetoone, marker_cons);
  net->FindMakePrjn(pfc_mnt, pfc_mnt_trc, gponetoone,
                    PbwmSp("PFCfmTRC", LeabraConSpec));
  // also stim input

  net->FindMakePrjn(pfc_mnt_d, pfc_mnt, onetoone, PbwmSp("PFCDeepCtxt", DeepCtxtConSpec));

  net->FindMakePrjn(pfc_mnt_trc, gpi, bgpfcprjn, marker_cons);
  net->FindMakePrjn(pfc_mnt_trc, pfc_mnt, gponetoone,
                    PbwmSp("PFCtoTRC", LeabraConSpec));
  // todo: technically should be from pfc_mnt_d
  // also stim input

  net->FindMakePrjn(pfc_out, gpi, bgpfcprjn, marker_cons);
  net->FindMakePrjn(pfc_out, pfc_out_d, onetoone,
                    PbwmSp("PFCSendDeepMod", SendDeepModConSpec));
  net->FindMakePrjn(pfc_out, pfc_mnt_d, onetoone,
                    PbwmSp("PFCdMntToOut", LeabraConSpec));

  net->FindMakePrjn(pfc_out_d, pfc_out, onetoone, PbwmSp("PFCDeepCtxt", DeepCtxtConSpec));
  
  // connect input layers
  for(i=0;i<task_input_lays.size;i++) {
    Layer* il = (Layer*)task_input_lays[i];
    net->FindMakePrjn(matrix_go, il, fullprjn, PbwmSp("MatrixConsGo", MSNConSpec));
    net->FindMakePrjn(matrix_nogo, il, fullprjn,
                      PbwmSp("MatrixConsNoGo", MSNConSpec));
    net->FindMakePrjn(matrix_tan, il, fullprjn, PbwmSp("ToTANs", LeabraDeltaConSpec));
  }
  
  LeabraUnitSpec* input_units = PbwmSp("PFCInputUnits", LeabraUnitSpec);
  
  for(i=0;i<stim_input_lays.size;i++) {
    Layer* il = (Layer*)stim_input_lays[i];

    il->SetUnitSpec(input_units);
    
    if(task_input_lays.size == 0) {
      net->FindMakePrjn(matrix_go, il, fullprjn, PbwmSp("MatrixConsGo", MSNConSpec));
      net->FindMakePrjn(matrix_nogo, il, fullprjn,
                        PbwmSp("MatrixConsNoGo", MSNConSpec));
    }
    net->FindMakePrjn(matrix_tan, il, fullprjn, PbwmSp("ToTANs", LeabraDeltaConSpec));
                      
    net->FindMakePrjn(pfc_mnt, il, fullprjn, PbwmSp("ToPFC", LeabraConSpec));
    net->FindMakePrjn(pfc_mnt_trc, il, PbwmSp("DeepToTRC", TiledGpRFPrjnSpec),
                      PbwmSp(prefix + "DeepRawPlus", SendDeepRawConSpec));
    
  }

  // connect output layers
  for(i=0;i<output_lays.size;i++) {
    Layer* ol = (Layer*)output_lays[i];
    net->FindMakePrjn(ol, pfc_out_d, fullprjn, PbwmSp("PFCtoOutput", LeabraConSpec));
    //    net->FindMakePrjn(ol, pfc_mnt_d, fullprjn, PbwmSp(prefix + "LrnCons", LeabraConSpec));
    // todo: no reciprocal connection!?
  }

  // connect hiden layers
  for(i=0;i<hidden_lays.size;i++) {
    Layer* hl = (Layer*)hidden_lays[i];
    net->FindMakePrjn(hl, pfc_out_d, fullprjn, PbwmSp("PFCtoOutput", LeabraConSpec));
    //    net->FindMakePrjn(hl, pfc_mnt_d, fullprjn, PbwmSp(prefix + "LrnCons", LeabraConSpec));
  }

  //////////////////////////////////////////////////////////////////////////////////
  // set positions & geometries

  net->SetNetFlag(Network::ABS_POS); // only works with absolute positioning
  net->SetNetFlag(Network::MANUAL_POS); // and turn off automatic..
  
  if(matrix_go->brain_area.empty()) 
    matrix_go->brain_area = ".*/.*/.*/.*/Caudate Body";
  if(matrix_nogo->brain_area.empty()) 
    matrix_nogo->brain_area = ".*/.*/.*/.*/Caudate Body";
  if(patch->brain_area.empty()) 
    patch->brain_area = ".*/.*/.*/.*/Caudate Body";
  if(gpi->brain_area.empty())
    gpi->brain_area = ".*/.*/.*/.*/Substantia Nigra";
  if(gpenogo->brain_area.empty())
    gpenogo->brain_area = ".*/.*/.*/.*/Substantia Nigra";
  if(pfc_mnt->brain_area.empty())
    pfc_mnt->brain_area = ".*/.*/.*/.*/BA9";
  if(pfc_mnt_d->brain_area.empty())
    pfc_mnt_d->brain_area = ".*/.*/.*/.*/BA9";

  // here to allow it to get disp_geom for laying out the pfc and matrix guys!
  PBWM_SetNStripes(net, pfc_gp_x, pfc_gp_y, -1, -1, -1, -1, prefix);

  int lay_spc = 2;

  if(new_pbwm_laygp) {
    pbwm_laygp->pos.z = 0;
    pbwm_laygp->pos.x = 0;
    pbwm_laygp->pos.y = 0;
  }

  ///////////////	Matrix Layout

  int bg_gp_x = pfc_gp_x * 2;
  int bg_gp_y = pfc_gp_y;
  
  int mtx_st_x = pvlv_laygp_da->pos.x + pvlv_laygp_da->max_disp_size.x + lay_spc;
  int mtx_st_y = 0;
  int mtx_nogo_y = mtx_st_y + 3 * lay_spc;
  int mtx_z = 0;

  int mtx_x_sz = 4;
  int mtx_y_sz = 4;
  int mtx_n = mtx_x_sz * mtx_y_sz;

  if(new_matrix) {
    matrix_go->pos_abs.SetXYZ(mtx_st_x, mtx_st_y, mtx_z);
    matrix_go->un_geom.SetXYN(mtx_x_sz, mtx_y_sz, mtx_n);
    lay_set_geom(matrix_go, bg_gp_x, bg_gp_y);

    matrix_nogo->PositionBehind(matrix_go, lay_spc);
    matrix_nogo->un_geom.SetXYN(mtx_x_sz, mtx_y_sz, mtx_n);
    lay_set_geom(matrix_nogo, bg_gp_x, bg_gp_y);
  }

  ///////////////	GPi / Thal

  int gpi_st_y = 0;

  if(new_matrix) {
    gpi->PositionRightOf(matrix_go, lay_spc);
    lay_set_geom(gpi, bg_gp_x, bg_gp_y);

    gpenogo->PositionBehind(gpi, lay_spc);
    lay_set_geom(gpenogo, bg_gp_x, bg_gp_y);
    
    patch->PositionBehind(gpenogo, lay_spc);
    lay_set_geom(patch, pfc_gp_x, pfc_gp_y, 1);
    
    matrix_tan->PositionBehind(patch, lay_spc);
    lay_set_geom(matrix_tan, 1, 1, 1);
    matrix_tan->unit_groups = false;
  }

  ///////////////	PFC Layout first -- get into z = 1

  int pfcu_n = 35; int pfcu_x = 5; int pfcu_y = 7;
  int pfc_st_x = mtx_st_x;
  int pfc_st_y = 0;
  int pfc_z = 1;
  if(new_pfc) {
    pfc_mnt_trc->pos_abs.SetXYZ(pfc_st_x, pfc_st_y, pfc_z); 
    pfc_mnt_trc->un_geom.SetXYN(4, 4, 16);
    lay_set_geom(pfc_mnt_trc, pfc_gp_x, pfc_gp_y);

    pfc_mnt->PositionRightOf(pfc_mnt_trc, lay_spc);
    pfc_mnt->un_geom.SetXYN(pfcu_x, pfcu_y, pfcu_n);
    lay_set_geom(pfc_mnt, pfc_gp_x, pfc_gp_y);
    
    pfc_mnt_d->PositionBehind(pfc_mnt, lay_spc);
    pfc_mnt_d->un_geom.SetXYN(pfcu_x, pfcu_y, pfcu_n);
    lay_set_geom(pfc_mnt_d, pfc_gp_x, pfc_gp_y);

    pfc_out->PositionRightOf(pfc_mnt, lay_spc);
    pfc_out->un_geom.SetXYN(pfcu_x, pfcu_y, pfcu_n);
    lay_set_geom(pfc_out, pfc_gp_x, pfc_gp_y);

    pfc_out_d->PositionBehind(pfc_out, lay_spc);
    pfc_out_d->un_geom.SetXYN(pfcu_x, pfcu_y, pfcu_n);
    lay_set_geom(pfc_out_d, pfc_gp_x, pfc_gp_y);
  }

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
      "PBWM configuration is now complete.  Do not forget the remaining things \
you need to check:\n\n" + man_msg;
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

  String man_msg = "You will have to configure inputs/outputs to/from the EC layers after the configuration";
  String msg = "Configuring ThetaPhase Hippocampus:\n\n" + man_msg + "\n\n";

  taMisc::Confirm(msg);

  net->RemoveUnits();

  //////////////////////////////////////////////////////////////////////////////////
  // make layers

  bool new_lay;
  Layer_Group* hip_laygp = net->FindMakeLayerGroup("Hippocampus");
  LeabraLayer* ecin = (LeabraLayer*)hip_laygp->FindMakeLayer("ECin", NULL, new_lay);
  LeabraLayer* ecout = (LeabraLayer*)hip_laygp->FindMakeLayer("ECout", NULL, new_lay);
  LeabraLayer* dg = (LeabraLayer*)hip_laygp->FindMakeLayer("DG", NULL, new_lay);
  LeabraLayer* ca3 = (LeabraLayer*)hip_laygp->FindMakeLayer("CA3", NULL, new_lay);
  LeabraLayer* ca1 = (LeabraLayer*)hip_laygp->FindMakeLayer("CA1", NULL, new_lay);
  //  LeabraLayer* subic = (LeabraLayer*)hip_laygp->FindMakeLayer("Subiculum", NULL, new_lay);

  //////////////////////////////////////////////////////////////////////////////////
  // make specs

  BaseSpec_Group* hipspec = net->FindMakeSpecGp("HippoSpecs");

  // unit specs
  FMSpec(LeabraUnitSpec, hip_units, hipspec, "HippoUnitSpec");
  FMChild(ECoutUnitSpec, ecout_units, hip_units, "ECoutUnits");
  FMChild(CA1UnitSpec, ca1_units, hip_units, "CA1Units");
  // FMChild(LeabraUnitSpec, dg_units, hip_units, "DGUnits");

  FMSpec(HippoEncoderConSpec, ecca1_cons, hipspec, "EC_CA1ConSpecs");
  FMChild(HippoEncoderConSpec, ecin_ca1_cons, ecca1_cons, "ECin_CA1");
  FMChild(HippoEncoderConSpec, ca1_ecout_cons, ecca1_cons, "CA1_ECout");
  FMChild(HippoEncoderConSpec, ecout_ca1_cons, ecca1_cons, "ECout_CA1");
  FMChild(HippoEncoderConSpec, ecin_ecout_cons, ecca1_cons, "ECin_ECout");
  FMChild(HippoEncoderConSpec, ecout_ecin_cons, ecca1_cons, "ECout_ECin");
  FMChild(LeabraConSpec, in_ecin_cons, ecca1_cons, "Input_ECin");
  FMChild(LeabraConSpec, ecout_out_cons, ecca1_cons, "ECout_Output");
  FMChild(LeabraConSpec, tosubic_cons, ecca1_cons, "ToSubic");
  FMChild(MarkerConSpec, marker_cons, ecca1_cons, "HippoMarker");

  // connection specs
  FMSpec(CHLConSpec, hip_cons, hipspec, "HippoConSpecs");
  FMChild(LeabraBiasSpec, hip_bias, hip_cons, "HippoBiasSpec");
  FMChild(CHLConSpec, ppath_cons, hip_cons, "PerfPath");
  FMChild(CHLConSpec, mossy_cons, hip_cons, "Mossy");
  FMChild(CHLConSpec, ca3ca3_cons, hip_cons, "CA3_CA3");
  FMChild(CHLConSpec, ca3ca1_cons, hip_cons, "CA3_CA1");

  // layer specs
  FMSpec(LeabraLayerSpec, hip_laysp, hipspec, "HippoLayerSpec");
  FMChild(LeabraLayerSpec, ecout_laysp, hip_laysp, "ECout");
  FMChild(LeabraLayerSpec, ecin_laysp, ecout_laysp, "ECin");
  FMChild(LeabraLayerSpec, dg_laysp, hip_laysp, "DG");
  FMChild(LeabraLayerSpec, ca3_laysp, hip_laysp, "CA3");
  FMChild(LeabraLayerSpec, ca1_laysp, hip_laysp, "CA1");
  // FMSpec(SubiculumLayerSpec, subic_laysp, hipspec, "Subiculum");

  // prjn specs
  BaseSpec_Group* prjns = (BaseSpec_Group*)hipspec->FindMakeGpName("HippoPrjns");

  FMSpec(FullPrjnSpec, fullprjn, prjns, "FullPrjn");
  FMSpec(OneToOnePrjnSpec, onetoone, prjns, "OneToOne");
  FMSpec(GpOneToOnePrjnSpec, gponetoone, prjns, "GpOneToOne");
  FMSpec(UniformRndPrjnSpec, ppath_prjn, prjns, "RandomPerfPath");
  FMSpec(UniformRndPrjnSpec, mossy_prjn, prjns, "UniformRndMossy");

  net->specs.UpdateAllSpecs();
  hipspec->UpdateAllSpecs();

  //////////////////////////////////////////////////////////////////////////////////
  // apply specs to objects

  // set bias specs for unit specs
  hip_units->bias_spec.SetSpec(hip_bias);

  ecin->SetLayerSpec(ecin_laysp);
  ecout->SetLayerSpec(ecout_laysp);
  dg->SetLayerSpec(dg_laysp);
  ca3->SetLayerSpec(ca3_laysp);
  ca1->SetLayerSpec(ca1_laysp);
  // subic->SetLayerSpec(subic_laysp);

  ecin->SetUnitSpec(hip_units);
  ecout->SetUnitSpec(ecout_units);
  dg->SetUnitSpec(hip_units);
  ca3->SetUnitSpec(hip_units);
  ca1->SetUnitSpec(ca1_units);
  // subic->SetUnitSpec(hip_units);

  //////////////////////////////////////////////////////////////////////////////////
  // make projections

  // FindMakePrjn(Layer* recv, Layer* send,

  net->FindMakePrjn(ecin, ecout, onetoone, ecout_ecin_cons);

  net->FindMakePrjn(ecout, ca1, gponetoone, ca1_ecout_cons);
  net->FindMakePrjn(ecout, ecin, onetoone, marker_cons);

  net->FindMakePrjn(dg, ecin, ppath_prjn, ppath_cons);

  net->FindMakePrjn(ca3, ecin, ppath_prjn, ppath_cons);
  net->FindMakePrjn(ca3, dg, mossy_prjn, mossy_cons);
  net->FindMakePrjn(ca3, ca3, fullprjn, ca3ca3_cons);

  net->FindMakePrjn(ca1, ecin, gponetoone, ecin_ca1_cons);
  net->FindMakePrjn(ca1, ecout, gponetoone, ecout_ca1_cons);
  net->FindMakePrjn(ca1, ca3, fullprjn, ca3ca1_cons);

  // net->FindMakePrjn(subic, ecin, onetoone, tosubic_cons);
  // net->FindMakePrjn(subic, ecout, onetoone, tosubic_cons);

  //////////////////////////////////////////////////////////////////////////////////
  // set positions & geometries

  ecin->brain_area = ".*/.*/.*/.*/Hippocampus entorhinal cortex";
  ecout->brain_area = ".*/.*/.*/.*/Hippocampus entorhinal cortex";
  dg->brain_area = ".*/.*/.*/.*/Hippocampus dentate gyrus";
  ca3->brain_area = ".*/.*/.*/.*/Hippocampus cornu ammonis";
  ca1->brain_area = ".*/.*/.*/.*/Hippocampus cornu ammonis";
  // subic->brain_area = ".*/.*/.*/.*/Hippocampus subiculum";

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

  // subic->SetNUnits(12);
  // subic->un_geom.x = 12;
  // subic->un_geom.y = 1;

  hip_laygp->pos.SetXYZ(0, 0, 1);
  ecin->SetAbsPos(0, 0, 1);
  ecout->SetAbsPos(35, 0, 1);
  // subic->pos.SetXYZ(70, 0, 0);
  dg->SetAbsPos(0, 0, 2);
  ca3->SetAbsPos(0, 0, 3);
  ca1->SetAbsPos(35, 0, 3);

  //////////////////////////////////////////////////////////////////////////////////
  // params

  mossy_prjn->p_con = 0.05f;
  
  // EC_CA1ConSpecs, wt_sig.gain = 6, off 1.25, cor = 1 (not .4)

  // ECin_CA1, abs = 2
  ecin_ca1_cons->SetUnique("wt_scale", true);
  // ecin_ca1_cons->wt_scale.abs = 2.0f;

  // CA1_ECout, abs = 4
  ca1_ecout_cons->SetUnique("wt_scale", true);
  ca1_ecout_cons->wt_scale.abs = 4.0f;

  // ECin_ECout mean/var = .9, .01, rel = 0, lrate = 0
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
  hip_cons->SetUnique("chl", true);
  hip_cons->chl.hebb = 0.05f;
  
  // mossy mean = 0.9 var = 0.01, rel = 8 , lrate = 0
  mossy_cons->SetUnique("rnd", true);
  mossy_cons->rnd.mean = 0.9f;
  mossy_cons->rnd.var = 0.01f;
  mossy_cons->SetUnique("wt_scale", true);
  mossy_cons->wt_scale.rel = 8.0f;
  mossy_cons->SetUnique("lrate", true);
  mossy_cons->lrate = 0.0f;
  mossy_cons->SetUnique("chl", true);
  mossy_cons->chl.savg_cor = 1.0f;
  
  // ca3_ca3 rel = 2, 
  ca3ca3_cons->SetUnique("wt_scale", true);
  ca3ca3_cons->wt_scale.rel = 2.0f;
  ca3ca3_cons->SetUnique("chl", true);
  ca3ca3_cons->chl.savg_cor = 1.0f;
  ca3ca3_cons->chl.hebb = 0.01f; // todo: need to explore..

  // ca3_ca1 lrate = 0.05
  ca3ca1_cons->SetUnique("lrate", true);
  ca3ca1_cons->lrate = 0.05f;
  ca3ca1_cons->SetUnique("chl", true);
  ca3ca1_cons->chl.hebb = 0.005f;

  // sparse hippocampal layers!

  dg_laysp->SetUnique("lay_inhib", true);
  dg_laysp->lay_inhib.gi = 3.8f;
  dg_laysp->SetUnique("avg_act", true);
  dg_laysp->avg_act.init = 0.01f;

  ca3_laysp->SetUnique("lay_inhib", true);
  ca3_laysp->lay_inhib.gi = 3.0f;
  ca3_laysp->SetUnique("avg_act", true);
  ca3_laysp->avg_act.init = 0.02f;

  ca1_laysp->SetUnique("lay_inhib", true);
  ca1_laysp->lay_inhib.on = false;
  ca1_laysp->SetUnique("unit_gp_inhib", true);
  ca1_laysp->unit_gp_inhib.on = true;
  ca1_laysp->unit_gp_inhib.gi = 2.4f;
  ca1_laysp->SetUnique("avg_act", true);
  ca1_laysp->avg_act.init = 0.1f;

  ecout_laysp->SetUnique("lay_inhib", true);
  ecout_laysp->lay_inhib.on = false;
  ecout_laysp->SetUnique("unit_gp_inhib", true);
  ecout_laysp->unit_gp_inhib.on = true;
  ecout_laysp->unit_gp_inhib.gi = 2.1f;
  ecout_laysp->SetUnique("avg_act", true);
  ecout_laysp->avg_act.init = 0.2f;

  // subic_laysp->lrate_mod_con_spec.SetSpec(ca3ca1_cons);

  // todo; lrate schedule!
  
  //////////////////////////////////////////////////////////////////////////////////
  // build and check

  net->Build();
  net->LayerPos_Cleanup();

  bool ok = net->CheckConfig();

  if(!ok) {
    msg =
      "Hippo: An error in the configuration has occurred (it should be the last message\
 you received prior to this one).  The network will not run until this is fixed.\
 In addition, the configuration process may not be complete, so you should run this\
 function again after you have corrected the source of the error.";
  }
  else {
    msg =
      "Hippo configuration is now complete.  Do not forget the one remaining thing\
 you need to do manually:\n\n" + man_msg;
  }
  taMisc::Confirm(msg);

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


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
#include <SelectEdit>

#include <FullPrjnSpec>
#include <OneToOnePrjnSpec>
#include <ScalarValLayerSpec>
#include <GpOneToOnePrjnSpec>
#include <MarkerGpOneToOnePrjnSpec>
#include <TesselPrjnSpec>
#include <UniformRndPrjnSpec>

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

#include <PVConSpec>
#include <PVrConSpec>
#include <NVConSpec>
#include <PViLayerSpec>
#include <PVrLayerSpec>
#include <LVeLayerSpec>
#include <NVLayerSpec>
#include <PVLVDaLayerSpec>

#include <SNrThalLayerSpec>
#include <MatrixUnitSpec>
#include <MatrixLayerSpec>
#include <MatrixConSpec>
#include <MatrixBiasSpec>
#include <PFCUnitSpec>
#include <PFCLayerSpec>
#include <PFCConSpec>
#include <PFCDeepGatedConSpec>
#include <LeabraTICtxtLayerSpec>

#include <SNrPrjnSpec>
#include <TopoWtsPrjnSpec>
#include <PVrToMatrixGoPrjnSpec>

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
      StdLayerSpecs(net);
  }
  if(!std_net_dlg) {
    taBase::SetPointer((taBase**)&std_net_dlg, new StdNetWizDlg);
  }
  bool rval = std_net_dlg->DoDialog();
  return rval;
}

bool LeabraWizard::StdLayerSpecs(LeabraNetwork* net) {
  if(!net) {
    LeabraProject* proj = GET_MY_OWNER(LeabraProject);
    net = (LeabraNetwork*)proj->GetNewNetwork();
    if(TestError(!net, "StdLayerSpecs", "network is NULL and could not make a new one -- aborting!")) return false;
  }
  LeabraLayerSpec* hid = (LeabraLayerSpec*)net->FindMakeSpec("", &TA_LeabraLayerSpec);
  hid->name = "HiddenLayer";
  LeabraLayerSpec* inout;
  inout = (LeabraLayerSpec*)hid->children.FindMakeSpec("Input_Output", &TA_LeabraLayerSpec);
  hid->inhib.type = LeabraInhibSpec::KWTA_AVG_INHIB;
  hid->inhib.kwta_pt = .6f;
  inout->SetUnique("inhib", true);
  inout->SetUnique("kwta", true);
  inout->inhib.type = LeabraInhibSpec::KWTA_INHIB;
  inout->inhib.kwta_pt = .25f;
  inout->kwta.k_from = KWTASpec::USE_PAT_K;

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
  basic_us->dt.vm = .04f;
  basic_us->g_bar.i = 10.0f;
  inhib_us->SetUnique("dt", true);
  inhib_us->dt.vm = .07f;

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

  basic_ls->inhib.type = LeabraInhibSpec::UNIT_INHIB;
  basic_ls->adapt_i.type = AdaptISpec::G_BAR_I;
  basic_ls->adapt_i.tol = .02f; // these params sometimes get off..
  basic_ls->adapt_i.p_dt = .1f;

  inhib_ls->SetUnique("kwta", true);
  inhib_ls->kwta.k_from = KWTASpec::USE_PCT;
  inhib_ls->kwta.pct = .34f;

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
  net->cycle_max = 300;
  net->min_cycles = 150;

  SelectEdit* edit = proj->FindMakeSelectEdit("UnitInhib");
  if(edit != NULL) {
    basic_us->SelectForEditNm("dt", edit, "excite");
    inhib_us->SelectForEditNm("dt", edit, "inhib");
    basic_us->SelectForEditNm("g_bar", edit, "excite");
    ff_inhib_cs->SelectForEditNm("wt_scale", edit, "ff_inhib");
    fb_inhib_cs->SelectForEditNm("rnd", edit, "to_inhib");
    inhib_cs->SelectForEditNm("rnd", edit, "fm_inhib");
    basic_ls->SelectForEditNm("adapt_i", edit, "layers");

    net->SelectForEditNm("cycle_max", edit, net->name);
    net->SelectForEditNm("min_cycles", edit, net->name);
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

  net->learn_rule = LeabraNetwork::CTLEABRA_XCAL; // make sure
  net->UpdateAfterEdit();	// trigger update
  net->ti_mode = true;

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
    if(lay != rew_targ_lay && lay != tdrp && lay != extrew && lay != tdint && lay != tdda
       && !laysp->InheritsFrom(&TA_PFCLayerSpec)
       && !laysp->InheritsFrom(&TA_MatrixLayerSpec)
       && !laysp->InheritsFrom(&TA_SNrThalLayerSpec)) {
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

  learn_cons->lmix.hebb = .01f; // .01 hebb on learn cons
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
  rewpred_cons->SetUnique("lmix", true);
  rewpred_cons->lmix.hebb = 0.0f;

  rewpred_units->SetUnique("g_bar", true);
  rewpred_units->g_bar.h = .015f;
  rewpred_units->g_bar.a = .045f;

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
    lsp->inhib.type = LeabraInhibSpec::KWTA_INHIB; lsp->inhib.kwta_pt = 0.25f;
    lsp->kwta.k_from = KWTASpec::USE_K;    lsp->kwta.k = 3;
    lsp->gp_kwta.k_from = KWTASpec::USE_K; lsp->gp_kwta.k = 3;
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
  SelectEdit* edit = proj->FindMakeSelectEdit("TD");
  if(edit != NULL) {
    rewpred_cons->SelectForEditNm("lrate", edit, "rewpred");
    ersp->SelectForEditNm("rew", edit, "extrew");
    tdrpsp->SelectForEditNm("rew_pred", edit, "tdrp");
    tdintsp->SelectForEditNm("rew_integ", edit, "tdint");
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

  FMSpec(LeabraUnitSpec, pv_units, pvlvspgp, "PVUnits");
  FMChild(LeabraUnitSpec, lv_units, pv_units, "LVUnits");
  FMSpec(LeabraUnitSpec, da_units, pvlvspgp, "DaUnits");

  FMSpec(PVConSpec, pvi_cons, pvlvspgp, "PViCons");
  FMChild(PVrConSpec, pvr_cons, pvi_cons, "PVrCons");
  FMChild(PVConSpec, lve_cons, pvi_cons, "LVeCons");
  FMChild(PVConSpec, lvi_cons, lve_cons, "LViCons");
  FMChild(NVConSpec, nv_cons, pvi_cons, "NVCons");
  FMSpec(LeabraBiasSpec, bg_bias, pvlvspgp, "BgBias");
  FMSpec(MarkerConSpec, marker_cons, pvlvspgp, "PvlvMarker");

  FMSpec(ScalarValLayerSpec, laysp, pvlvspgp, "PvlvLayer");
  FMChild(LeabraLayerSpec, rewtargsp, laysp, "RewTargLayer");
  FMChild(ExtRewLayerSpec, pvesp, laysp, "PVeLayer");
  FMChild(PVrLayerSpec, pvrsp, laysp, "PVrLayer");
  FMChild(PViLayerSpec, pvisp, laysp, "PViLayer");
  FMChild(LVeLayerSpec, lvesp, laysp, "LVeLayer");
  FMChild(NVLayerSpec, nvsp, laysp, "NVLayer");
  FMChild(PVLVDaLayerSpec, dasp, laysp, "VTALayer");

  FMSpec(FullPrjnSpec, fullprjn, pvlvspgp, "PvlvFullPrjn");
  FMSpec(OneToOnePrjnSpec, onetoone, pvlvspgp, "PvlvOneToOne");
  FMSpec(OneToOnePrjnSpec, vtaonetoone, pvlvspgp, "VTAOneToOnePrjn");

  //////////////////////////////////////////////////////////////////////////////////
  // set default spec parameters

  pvlvspgp->Defaults();

  pvi_cons->UpdateAfterEdit();
  bg_bias->SetUnique("lrate", true);
  bg_bias->lrate = 0.0f;
  bg_bias->SetUnique("learn", true);
  bg_bias->learn = false;

  pvr_cons->SetUnique("lrate", true);
  pvi_cons->SetUnique("lrate", true);
  nv_cons->SetUnique("lrate", true);
  lve_cons->SetUnique("lrate", true);
  lvi_cons->SetUnique("lrate", true);

  pvi_cons->SetUnique("lmix", true);
  pvi_cons->lmix.err_sb = false;
  pvi_cons->SetUnique("rnd", true);
  pvi_cons->rnd.mean = 0.1f;
  pvi_cons->rnd.var = 0.0f;

  pvi_cons->lrate = .01f;
  pvr_cons->lrate = .1f;
  nv_cons->lrate = .0005f;
  lve_cons->lrate = .05f;
  lvi_cons->lrate = .001f;

  pv_units->SetUnique("g_bar", true);

  //vtaonetoone->send_start = 1; // this scheme compiled, but didn't get rid of error message so abandoning

  // default scalar val for all types
  laysp->scalar.rep = ScalarValSpec::LOCALIST;
  laysp->bias_val.wt = ScalarValBias::NO_WT;
  laysp->bias_val.val = 0.5f;
  laysp->scalar.min_sum_act = .2f;
  laysp->inhib.type = LeabraInhibSpec::KWTA_AVG_INHIB; laysp->inhib.kwta_pt = 0.9f;
  laysp->kwta.k_from = KWTASpec::USE_K;    laysp->kwta.k = 1;
  laysp->gp_kwta.k_from = KWTASpec::USE_K; laysp->gp_kwta.k = 1;
  laysp->unit_range.min = 0.0f;  laysp->unit_range.max = 1.0f;
  laysp->unit_range.UpdateAfterEdit();
  laysp->val_range = laysp->unit_range;

  nvsp->SetUnique("bias_val", true);
  nvsp->bias_val.un = ScalarValBias::GC;
  nvsp->bias_val.wt = ScalarValBias::NO_WT;
  nvsp->bias_val.val = 1.0f;

  lv_units->SetUnique("maxda", false);
  pv_units->SetUnique("act", true);
  pv_units->SetUnique("act_fun", true);
  pv_units->SetUnique("dt", true);
  pv_units->act_fun = LeabraUnitSpec::NOISY_LINEAR;
  pv_units->act.gelin = false;
  pv_units->act.thr = .17f;
  pv_units->act.gain = 220.0f;
  pv_units->act.nvar = .01f;
  pv_units->v_m_init.mean = 0.15f;
  pv_units->e_rev.l = 0.15f;
  pv_units->e_rev.i = 0.15f;
  pv_units->g_bar.l = .1f;
  pv_units->g_bar.h = .03f;  pv_units->g_bar.a = .09f;
  pv_units->dt.vm_eq_cyc = 100; // go straight to equilibrium!
  pv_units->SetUnique("maxda", true);
  pv_units->maxda.val = MaxDaSpec::NO_MAX_DA;
  pv_units->act.avg_dt = 0.0f;

  da_units->SetUnique("act_range", true);
  da_units->act_range.max = 2.0f;
  da_units->act_range.min = -2.0f;
  da_units->act_range.UpdateAfterEdit();
  da_units->SetUnique("clamp_range", true);
  da_units->clamp_range.max = 2.0f;
  da_units->clamp_range.min = -2.0f;
  da_units->clamp_range.UpdateAfterEdit();
  da_units->SetUnique("maxda", true);
  da_units->maxda.val = MaxDaSpec::NO_MAX_DA;
  da_units->SetUnique("act", true);
  da_units->act.avg_dt = 0.0f;

  vtaonetoone->send_start = 1;  // key feature

  //////////////////
  // Update Everyone

  pvesp->UpdateAfterEdit();
  pvisp->UpdateAfterEdit();
  lvesp->UpdateAfterEdit();
  // lvisp->UpdateAfterEdit();
  pvrsp->UpdateAfterEdit();
  nvsp->UpdateAfterEdit();

  for(int j=0;j<pvlvspgp->leaves;j++) {
    BaseSpec* sp = (BaseSpec*)pvlvspgp->Leaf(j);
    sp->UpdateAfterEdit();
  }

  //////////////////////////////////////////////////////////////////////////////////
  // select edit

  LeabraProject* proj = GET_MY_OWNER(LeabraProject);
  SelectEdit* edit = proj->FindMakeSelectEdit("PVLV");
  if(edit) {
    edit->auto_edit = true;
    pvr_cons->SelectForEditNm("lrate", edit, "pvr");
    pvi_cons->SelectForEditNm("lrate", edit, "pvi");
    lve_cons->SelectForEditNm("lrate", edit, "lve");
    // lvi_cons->SelectForEditNm("lrate", edit, "lvi");
    nv_cons->SelectForEditNm("lrate", edit, "nv");
    pvesp->SelectForEditNm("rew", edit, "pve");
    pvisp->SelectForEditNm("pv", edit, "pvi");
    lvesp->SelectForEditNm("lv", edit, "lve");
    pvrsp->SelectForEditNm("pv_detect", edit, "pvr");
    nvsp->SelectForEditNm("nv", edit, "nv");
//     pvisp->SelectForEditNm("scalar", edit, "pvi");
//     lvesp->SelectForEditNm("scalar", edit, "lve");
//     pvisp->SelectForEditNm("bias_val", edit, "pvi");
//     lvesp->SelectForEditNm("bias_val", edit, "lve");
//    dasp->SelectForEditNm("avg_da", edit, "vta");
    dasp->SelectForEditNm("da", edit, "vta");
  }

  return true;
}

// todo: set td_mod.on = true for td_mod_all; need to get UnitSpec..

bool LeabraWizard::PVLV(LeabraNetwork* net, bool da_mod_all) {
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

  String msg = "Configuring Pavlov (PVLV) Layers:\n\n\
 There is one thing you will need to check manually after this automatic configuration\
 process completes (this note will be repeated when things complete --- there may be some\
 messages in the interim):\n\n";

  String man_msg = "1. Check that connection(s) were made from all appropriate output layers\
 to the PVe (ExtRewLayerSpec) layer, using the MarkerConSpec (MarkerCons) Con spec.\
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

  bool  lve_new = false;
  bool  pvr_new = false;
  bool  nv_new = false;
  String pvenm = "PVe";  String pvinm = "PVi";  String pvrnm = "PVr";
  String lvenm = "LVe";  String lvinm = "LVi";  String nvnm = "NV";
  String vtanm = "VTA";

  bool new_laygp = false;
  Layer_Group* laygp = net->FindMakeLayerGroup("PVLV", NULL, new_laygp);

  LeabraLayer* rew_targ_lay;
  LeabraLayer* pve;  LeabraLayer* pvr; LeabraLayer* pvi; LeabraLayer* lve;
  LeabraLayer* lvi = NULL;
  LeabraLayer* nv;   LeabraLayer* vta;

  bool dumbo;
  rew_targ_lay = (LeabraLayer*)laygp->FindMakeLayer("RewTarg");
  pve = (LeabraLayer*)laygp->FindMakeLayer(pvenm, NULL, dumbo);
  pvr = (LeabraLayer*)laygp->FindMakeLayer(pvrnm, NULL, pvr_new);
  pvi = (LeabraLayer*)laygp->FindMakeLayer(pvinm, NULL, dumbo);
  lve = (LeabraLayer*)laygp->FindMakeLayer(lvenm, NULL, lve_new);
  // lvi = (LeabraLayer*)laygp->FindMakeLayer(lvinm, NULL, dumbo);
  nv =  (LeabraLayer*)laygp->FindMakeLayer(nvnm, NULL, nv_new);
  vta = (LeabraLayer*)laygp->FindMakeLayer(vtanm, NULL, dumbo, "DA");
  if(rew_targ_lay == NULL || lve == NULL || pve == NULL || pvi == NULL || vta == NULL) return false;

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
    // todo: add any new bg layer exclusions here!
    if(lay != rew_targ_lay && lay != lve && lay != pve && lay != pvr && lay != pvi &&
       lay != lvi && lay != nv && lay != vta
       && !laysp->InheritsFrom(&TA_PFCLayerSpec)
       && !laysp->InheritsFrom(&TA_MatrixLayerSpec)
       && !laysp->InheritsFrom(&TA_SNrThalLayerSpec)) {
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
  }

  //////////////////////////////////////////////////////////////////////////////////
  // make specs

  if(!PVLV_Specs(net)) return false;

  String pvlvprefix = "PVLV";
  BaseSpec_Group* pvlvspgp = net->FindMakeSpecGp(pvlvprefix);

  if(output_lays.size > 0)
    PvlvSp("PVeLayer",ExtRewLayerSpec)->rew_type = ExtRewLayerSpec::OUT_ERR_REW;
  else
    PvlvSp("PVeLayer",ExtRewLayerSpec)->rew_type = ExtRewLayerSpec::EXT_REW;

  int n_lv_u;           // number of pvlv-type units
  if(PvlvSp("PViLayer",PViLayerSpec)->scalar.rep == ScalarValSpec::LOCALIST)
    n_lv_u = 4;
  else if(PvlvSp("PViLayer",PViLayerSpec)->scalar.rep == ScalarValSpec::GAUSSIAN)
    n_lv_u = 12;
  else
    n_lv_u = 21;

  //////////////////////////////////////////////////////////////////////////////////
  // set positions & geometries

  pve->brain_area = ".*/.*/.*/.*/Lateral Hypothalamic area LHA";
  pvi->brain_area = ".*/.*/.*/.*/Nucleus Accumbens NAc";
  pvr->brain_area = ".*/.*/.*/.*/Caudate Head";
  lve->brain_area = ".*/.*/.*/.*/Amygdala Central Nucleus CNA";
  // lvi->brain_area = ".*/.*/.*/.*/Lateral Habenula LHB";
  vta->brain_area = ".*/.*/.*/.*/Ventral Tegmental Area VTA";

  if(new_laygp) {
    laygp->pos.z = 0;
  }

  if(pvr_new) {
    pvr->pos.SetXYZ(0,10,0);
  }
  if(nv_new) {
    nv->pos.SetXYZ(8,10,0);
  }

  if(lve_new) {
    pve->pos.SetXYZ(0,0,0);
    pvi->pos.SetXYZ(0,5,0);

    lve->pos.SetXYZ(8,0,0);
    // lvi->pos.SetXYZ(8,5,0);

    vta->pos.SetXYZ(15,0,0);
    rew_targ_lay->pos.SetXYZ(15,5,0);
  }

  if(pvi->un_geom.n != n_lv_u) {
    pvi->un_geom.n = n_lv_u; pvi->un_geom.x = n_lv_u; pvi->un_geom.y = 1; }
  if(lve->un_geom.n != n_lv_u) {
    lve->un_geom.n = n_lv_u; lve->un_geom.x = n_lv_u; lve->un_geom.y = 1; }
  // if(lvi->un_geom.n != n_lv_u) {
  //   lvi->un_geom.n = n_lv_u; lvi->un_geom.x = n_lv_u; lvi->un_geom.y = 1; }
  if(pve->un_geom.n != n_lv_u) {
    pve->un_geom.n = n_lv_u; pve->un_geom.x = n_lv_u; pve->un_geom.y = 1; }
  if(pvr->un_geom.n != n_lv_u) {
    pvr->un_geom.n = n_lv_u; pvr->un_geom.x = n_lv_u; pvr->un_geom.y = 1; }
  if(nv->un_geom.n != n_lv_u) {
    nv->un_geom.n = n_lv_u; nv->un_geom.x = n_lv_u; nv->un_geom.y = 1; }
  vta->un_geom.n = 1;
  rew_targ_lay->un_geom.n = 1;
  rew_targ_lay->layer_type = Layer::INPUT;

  //////////////////////////////////////////////////////////////////////////////////
  // apply specs to objects

  LeabraUnitSpec* pv_units = PvlvSp("PVUnits",LeabraUnitSpec);

  rew_targ_lay->SetLayerSpec(PvlvSp("RewTargLayer",LeabraLayerSpec));
  pve->SetLayerSpec(PvlvSp("PVeLayer",ExtRewLayerSpec));
  pve->SetUnitSpec(pv_units);
  pvi->SetLayerSpec(PvlvSp("PViLayer", PViLayerSpec));
  pvi->SetUnitSpec(pv_units);
  lve->SetLayerSpec(PvlvSp("LVeLayer", LVeLayerSpec));
  lve->SetUnitSpec(PvlvSp("LVUnits",LeabraUnitSpec));
  vta->SetLayerSpec(PvlvSp("VTALayer", PVLVDaLayerSpec));
  vta->SetUnitSpec(PvlvSp("DaUnits",LeabraUnitSpec));
  pvr->SetLayerSpec(PvlvSp("PVrLayer", PVrLayerSpec));
  pvr->SetUnitSpec(pv_units);
  nv->SetLayerSpec(PvlvSp("NVLayer", NVLayerSpec));
  nv->SetUnitSpec(pv_units);

  LeabraBiasSpec* bg_bias = PvlvSp("BgBias", LeabraBiasSpec);
  pv_units->bias_spec.SetSpec(bg_bias);
  PvlvSp("LVUnits",LeabraUnitSpec)->bias_spec.SetSpec(bg_bias);
  PvlvSp("DaUnits",LeabraUnitSpec)->bias_spec.SetSpec(bg_bias);

  //////////////////////////////////////////////////////////////////////////////////
  // make projections

  MarkerConSpec* marker_cons = PvlvSp("PvlvMarker", MarkerConSpec);
  OneToOnePrjnSpec* onetoone = PvlvSp("PvlvOneToOne", OneToOnePrjnSpec);
  FullPrjnSpec* fullprjn = PvlvSp("PvlvFullPrjn", FullPrjnSpec);

  // FindMakePrjn(Layer* recv, Layer* send,
  net->FindMakePrjn(pve, rew_targ_lay, onetoone, marker_cons);
  net->FindMakePrjn(pvr, pve, onetoone, marker_cons);
  net->FindMakePrjn(pvi, pve, onetoone, marker_cons);

  net->FindMakePrjn(lve, pvr, onetoone, marker_cons);
  // net->FindMakePrjn(lvi, pvr, onetoone, marker_cons);

  OneToOnePrjnSpec* vtaonetoone = PvlvSp("VTAOneToOnePrjn", OneToOnePrjnSpec);

  net->FindMakePrjn(vta, pvi, vtaonetoone, marker_cons);
  net->FindMakePrjn(vta, lve, vtaonetoone, marker_cons);
  // net->FindMakePrjn(vta, lvi, vtaonetoone, marker_cons);
  net->FindMakePrjn(vta, pvr, vtaonetoone, marker_cons);
  net->FindMakePrjn(vta, nv,  vtaonetoone, marker_cons);

  if(lve_new) {
    for(i=0;i<input_lays.size;i++) {
      Layer* il = (Layer*)input_lays[i];
      net->FindMakePrjn(pvr, il, fullprjn, PvlvSp("PVrCons", PVrConSpec));
      net->FindMakePrjn(pvi, il, fullprjn, PvlvSp("PViCons", PVConSpec));
      net->FindMakePrjn(lve, il, fullprjn, PvlvSp("LVeCons", PVConSpec));
      // net->FindMakePrjn(lvi, il, fullprjn, lvi_cons);
      net->FindMakePrjn(nv,  il, fullprjn, PvlvSp("NVCons", PVConSpec));
    }
  }

  if(da_mod_all) {
    for(i=0;i<other_lays.size;i++) {
      Layer* ol = (Layer*)other_lays[i];
      net->FindMakePrjn(ol, vta, fullprjn, marker_cons);
      //net->FindMakePrjn(ol, vta, vtaonetoone, marker_cons);
    }
  }

  for(i=0;i<output_lays.size;i++) {
    Layer* ol = (Layer*)output_lays[i];
    net->FindMakePrjn(pve, ol, onetoone, marker_cons);
  }

  //////////////////////////////////////////////////////////////////////////////////
  // build and check

  net->Build();
  net->LayerPos_Cleanup();

  if(new_laygp) {
    laygp->pos.z = 0;           // move back!
    net->RebuildAllViews();     // trigger update
  }

  taMisc::CheckConfigStart(false, false);

  bool ok = PvlvSp("PViLayer",PViLayerSpec)->CheckConfig_Layer(pvi, false);
  ok &= PvlvSp("LVeLayer",LVeLayerSpec)->CheckConfig_Layer(lve, false);
  // ok &= lvisp->CheckConfig_Layer(lve, false);
  ok &= PvlvSp("VTALayer",PVLVDaLayerSpec)->CheckConfig_Layer(vta, false);
  ok &= PvlvSp("PVeLayer",ExtRewLayerSpec)->CheckConfig_Layer(pve, false);
  ok &= PvlvSp("PVrLayer",PVrLayerSpec)->CheckConfig_Layer(pvr, false);
  ok &= PvlvSp("NVLayer",NVLayerSpec)->CheckConfig_Layer(nv, false);

  taMisc::CheckConfigEnd(ok);

  if(!ok) {
    msg =
      "PVLV: An error in the configuration has occurred (it should be the last message\
 you received prior to this one).  The network will not run until this is fixed.\
 In addition, the configuration process may not be complete, so you should run this\
 function again after you have corrected the source of the error.";
  }
  else {
    msg =
    "PVLV configuration is now complete.  Do not forget the one remaining thing\
 you need to do manually:\n\n" + man_msg;
  }
  taMisc::Confirm(msg);

  if(proj) {
    proj->undo_mgr.SaveUndo(net, "Wizard::PVLV after -- actually saves network specifically");
  }
  return true;
}

bool LeabraWizard::PVLV_ConnectLayer(LeabraNetwork* net, LeabraLayer* sending_layer,
                                     bool disconnect) {
  if(TestError(!net || !sending_layer, "PVLV_ConnectLayer", "must specify a network and a sending layer!")) return false;

  LeabraProject* proj = GET_MY_OWNER(LeabraProject);
  if(proj) {
    proj->undo_mgr.SaveUndo(net, "Wizard::PVLV_ConnectLayer before -- actually saves network specifically");
  }

  // String pvenm = "PVe";
  String pvinm = "PVi";  String pvrnm = "PVr";
  String lvenm = "LVe";  String lvinm = "LVi";  String nvnm = "NV";

//   LeabraLayer* pve = (LeabraLayer*)net->FindLayer(pvenm);
  LeabraLayer* pvr = (LeabraLayer*)net->FindLayer(pvrnm);
  LeabraLayer* pvi = (LeabraLayer*)net->FindLayer(pvinm);
  LeabraLayer* lve = (LeabraLayer*)net->FindLayer(lvenm);
  LeabraLayer* lvi = (LeabraLayer*)net->FindLayer(lvinm);
  LeabraLayer* nv =  (LeabraLayer*)net->FindLayer(nvnm);

  String pvlvprefix = "PVLV";
  BaseSpec_Group* pvlvspgp = net->FindMakeSpecGp(pvlvprefix);

  FullPrjnSpec* fullprjn = PvlvSp("PvlvFullPrjn", FullPrjnSpec);
  if(TestError(!fullprjn, "PVLV_ConnectLayer",
               "fullprjn not found -- PVLV was not properly configured -- everything has been updated since version 6.4.0, so you need to reconfigure using the PVLV wizard")) {
    return false;
  }

  if(disconnect) {
    if(pvr)
      net->RemovePrjn(pvr, sending_layer);
    if(pvi)
      net->RemovePrjn(pvi, sending_layer);
    if(lve)
      net->RemovePrjn(lve, sending_layer);
    if(lvi)
      net->RemovePrjn(lvi, sending_layer);
    if(nv)
      net->RemovePrjn(nv,  sending_layer);
  }
  else {
    if(pvr)
      net->FindMakePrjn(pvr, sending_layer, fullprjn, PvlvSp("PVrCons", PVrConSpec));
    if(pvi)
      net->FindMakePrjn(pvi, sending_layer, fullprjn, PvlvSp("PViCons", PVConSpec));
    if(lve)
      net->FindMakePrjn(lve, sending_layer, fullprjn, PvlvSp("LVeCons", PVConSpec));
    // if(lvi && lvi_cons)
    //   net->FindMakePrjn(lvi, sending_layer, fullprjn, lvi_cons);
    if(nv)
      net->FindMakePrjn(nv,  sending_layer, fullprjn, PvlvSp("NVCons", PVConSpec));
  }
  if(proj) {
    proj->undo_mgr.SaveUndo(net, "Wizard::PVLV_ConnectLayer before -- actually saves network specifically");
  }
  return true;
}

bool LeabraWizard::PVLV_OutToPVe(LeabraNetwork* net, LeabraLayer* output_layer,
                                     bool disconnect) {
  if(TestError(!net || !output_layer, "PVLV_OutToPVe", "must specify a network and an output layer!")) return false;

  LeabraProject* proj = GET_MY_OWNER(LeabraProject);
  if(proj) {
    proj->undo_mgr.SaveUndo(net, "Wizard::PVLV_OutToPVe before -- actually saves network specifically");
  }

  String pvenm = "PVe";
  LeabraLayer* pve = (LeabraLayer*)net->FindLayer(pvenm);

  String pvlvprefix = "PVLV";
  BaseSpec_Group* pvlvspgp = net->FindMakeSpecGp(pvlvprefix);

  MarkerConSpec* marker_cons = PvlvSp("PvlvMarker", MarkerConSpec);
  OneToOnePrjnSpec* onetoone = PvlvSp("PvlvOneToOne", OneToOnePrjnSpec);
  if(TestError(!marker_cons, "PVLV_OutToPVe",
               "marker_cons not found -- PVLV was not properly configured -- everything has been updated since version 6.4.0, so you need to reconfigure using the PVLV wizard")) {
    return false;
  }

  if(disconnect) {
    net->RemovePrjn(pve, output_layer);
  }
  else {
    net->FindMakePrjn(pve, output_layer, onetoone, marker_cons);
  }
  if(proj) {
    proj->undo_mgr.SaveUndo(net, "Wizard::PVLV_OutToPVe after -- actually saves network specifically");
  }
  return true;
}


///////////////////////////////////////////////////////////////
//                      PBWM
///////////////////////////////////////////////////////////////

bool LeabraWizard::PBWM_Defaults(LeabraNetwork* net, bool topo_prjns,
                                 const String& prefix) {
  if(!net) {
    if(TestError(!net, "PBWM", "network is NULL -- must be passed and already PBWM configured -- aborting!"))
      return false;
  }

  return PBWM_Specs(net, topo_prjns, prefix, true); // true = set defaults
}

// this is how to access a pvlv spec of a given type, by name:
#define PbwmSp(pth,T) ((T*)pbwmspgp->ElemPath(pth, T::StatTypeDef(0), true))

bool LeabraWizard::PBWM_Specs(LeabraNetwork* net, bool topo_prjns,
                              const String& prefix, bool set_defs) {
  if(!net) {
    if(TestError(!net, "PBWM_Specs", "network is NULL -- only makes sense to run on an existing network -- aborting!"))
      return false;
  }

  String pvlvprefix = "PVLV";
  BaseSpec_Group* pvlvspgp = net->FindMakeSpecGp(pvlvprefix);
  if(!pvlvspgp) return false;
  BaseSpec_Group* pbwmspgp = net->FindMakeSpecGp(prefix);
  if(!pbwmspgp) return false;

  net->ti_mode = true;

  ////////////	UnitSpecs

  FMSpec(PFCUnitSpec, pfc_units, pbwmspgp, "PFCUnits");
  FMChild(LayerActUnitSpec, pfcd_units, pfc_units, "PFCDeepUnits");
  FMSpec(MatrixUnitSpec, matrix_units, pbwmspgp, "MatrixUnits");
  FMChild(MatrixUnitSpec, matrix_nogo_units, matrix_units, "MatrixNoGo");
  FMSpec(LeabraUnitSpec, snrthal_units, pbwmspgp, "SNrThalUnits");

  ////////////	ConSpecs

  FMSpec(PFCConSpec, topfc_cons, pbwmspgp, "ToPFC");
  FMChild(LeabraBiasSpec, pfc_bias, topfc_cons, "PFCBias");
  FMChild(LeabraBiasSpec, fixed_bias, topfc_cons, "FixedBias");

  FMChild(LeabraTICtxtConSpec, pfc_ctxt_cons, topfc_cons, "PfcTICtxt");

  FMChild(PFCConSpec, pfctopfc_cons, topfc_cons, "PFCtoPFC");

  FMChild(PFCConSpec, topfcfmin_cons, topfc_cons, "ToPFCFmInput");
  FMChild(PFCConSpec, topfcfmout_cons, topfc_cons, "ToPFCFmOutput");

  FMSpec(MatrixConSpec, matrix_cons, pbwmspgp, "MatrixCons");
  FMChild(MatrixBiasSpec, matrix_bias, matrix_cons, "MatrixBias");
  FMChild(MatrixConSpec, matrix_cons_topo, matrix_cons, "MatrixConsTopo");
  FMChild(MatrixConSpec, matrix_cons_topo_weak, matrix_cons_topo, "MatrixConsTopoWeak");
  FMChild(MatrixConSpec, matrix_cons_topo_strong, matrix_cons_topo, "MatrixConsTopoStrong");
  FMChild(MatrixConSpec, matrix_cons_nogo, matrix_cons, "MatrixConsNoGo");
  FMChild(MatrixConSpec, matrix_cons_nogofmgo, matrix_cons_nogo, "MatrixConsNoGoFmGo");
  FMChild(MatrixConSpec, matrix_cons_out, matrix_cons, "MatrixConsOut");

  FMSpec(LeabraConSpec, fmpfc_out, pbwmspgp, "FmPFC_out");

  FMSpec(MarkerConSpec, marker_cons, pbwmspgp, "PbwmMarker");
  FMSpec(LeabraConSpec, matrix_to_snrthal, pbwmspgp, "MatrixToSNrThal");
  // matrix nogo to snrthal just a marker con

  ////////////	LayerSpecs

  FMSpec(PFCLayerSpec, pfc_mnt_sp, pbwmspgp, "PFC_mnt");
  FMChild(PFCLayerSpec, pfc_in_sp, pfc_mnt_sp, "PFC_in");
  FMChild(PFCLayerSpec, pfc_out_sp, pfc_mnt_sp, "PFC_out");
  FMChild(LeabraTICtxtLayerSpec, pfc_deep_sp, pfc_mnt_sp, "PFC_deep");

  MatrixLayerSpec* matrix_go_mnt_out_sp = NULL;
  MatrixLayerSpec* matrix_go_out_mnt_sp = NULL;

  MatrixLayerSpec* matrix_nogo_mnt_out_sp = NULL;
  MatrixLayerSpec* matrix_nogo_out_mnt_sp = NULL;

  FMSpec(MatrixLayerSpec, matrix_go_mnt_sp, pbwmspgp, "Matrix_Go_mnt");

  FMChild(MatrixLayerSpec, matrix_go_in_sp, matrix_go_mnt_sp, "Matrix_Go_in");
  FMChild(MatrixLayerSpec, matrix_go_out_sp, matrix_go_mnt_sp, "Matrix_Go_out");
  if(topo_prjns) {
    FMChildPD(MatrixLayerSpec, matrix_go_mnt_out_sp, matrix_go_mnt_sp,
              "Matrix_Go_mnt_out");
    FMChildPD(MatrixLayerSpec, matrix_go_out_mnt_sp, matrix_go_mnt_sp,
              "Matrix_Go_out_mnt");  
  }

  FMChild(MatrixLayerSpec, matrix_nogo_mnt_sp, matrix_go_mnt_sp, "Matrix_NoGo_mnt");
  FMChild(MatrixLayerSpec, matrix_nogo_in_sp, matrix_nogo_mnt_sp, "Matrix_NoGo_in");
  FMChild(MatrixLayerSpec, matrix_nogo_out_sp, matrix_nogo_mnt_sp, "Matrix_NoGo_out");
  if(topo_prjns) {
    FMChildPD(MatrixLayerSpec, matrix_nogo_mnt_out_sp, matrix_nogo_mnt_sp,
              "Matrix_NoGo_mnt_out");
    FMChildPD(MatrixLayerSpec, matrix_nogo_out_mnt_sp, matrix_nogo_mnt_sp,
              "Matrix_NoGo_out_mnt");
  }

  FMSpec(SNrThalLayerSpec, snrthalsp, pbwmspgp, "SNrThalLayer");
  FMChild(SNrThalLayerSpec, snrthalsp_out, snrthalsp, "SNrThalLayer_out");

  ////////////	PrjnSpecs

  FMSpec(FullPrjnSpec, fullprjn, pbwmspgp, "PbwmFullPrjn");
  FMSpec(OneToOnePrjnSpec, onetoone, pbwmspgp, "PbwmOneToOne");
  FMSpec(GpOneToOnePrjnSpec, gponetoone, pbwmspgp, "PbwmGpOneToOne");
  FMSpec(MarkerGpOneToOnePrjnSpec, markergponetoone, pbwmspgp, "MarkerGpOneToOne");

  FMSpec(SNrPrjnSpec, snr_prjn, pbwmspgp, "SNrPrjn");

  FMSpec(TopoWtsPrjnSpec, topomaster, pbwmspgp, "TopoMaster");
  FMChild(TopoWtsPrjnSpec, topofminput, topomaster, "TopoFmInput");
  FMChild(TopoWtsPrjnSpec, intrapfctopo, topomaster, "TopoIntraPFC");
  FMChild(TopoWtsPrjnSpec, topomatrixpfc_self, topomaster, "TopoMatrixPFC_Self");
  FMChild(TopoWtsPrjnSpec, topomatrixpfc_other, topomaster, "TopoMatrixPFC_Other");

  FMSpec(TesselPrjnSpec, input_pfc, pbwmspgp, "Input_PFC");
  input_pfc->send_offs.SetSize(1); // this is all it takes!

  //////////////////////////////////////////////////////////////////////////////////
  // first: all the basic defaults from specs

  if(net->mid_minus_cycle < 5) {
    net->mid_minus_cycle = 40;
    net->min_cycles = net->mid_minus_cycle + 15;
  }

  pbwmspgp->Defaults();

  //////////////////////////////////////////////////////////////////////////////////
  // set default spec parameters

  pfc_units->act_avg.l_up_dt = 0.1f;

  // lr sched:
  topfc_cons->lrs_value = LeabraConSpec::NO_LRS;
  //learn_cons->lrate_sched.SetSize(2);
  //SchedItem* si = (SchedItem*)learn_cons->lrate_sched.FastEl(0);
  //si->start_val = 1.0f;
  //si = (SchedItem*)learn_cons->lrate_sched.FastEl(1);
  //si->start_ctr = 90;
  //si->start_val = .1f;

  // slow learning rate on to pfc cons!
  topfc_cons->SetUnique("lrate", true);
  topfc_cons->learn = true;
  topfc_cons->lrate = .005f;
  topfc_cons->SetUnique("rnd", false);
  topfc_cons->rnd.var = 0.25f;

  topfc_cons->SetUnique("lmix", true);
  topfc_cons->lmix.hebb = .001f;
  topfc_cons->stable_mix.stable_pct = 0.8f;

  pfc_ctxt_cons->SetUnique("wt_scale", true);
  pfc_ctxt_cons->wt_scale.rel = 1.5f;

  if(topo_prjns) {
    pfctopfc_cons->SetUnique("rnd", true);
    pfctopfc_cons->rnd.mean = 0.0f;
    pfctopfc_cons->rnd.var = 0.25f;
  }
  else {
    pfctopfc_cons->SetUnique("rnd", false);
  }

  pfctopfc_cons->SetUnique("wt_scale", true);
  pfctopfc_cons->wt_scale.rel = 0.5f;

  if(topo_prjns) {
    topfcfmin_cons->SetUnique("rnd", true);
    topfcfmin_cons->rnd.mean = 0.0f;
    topfcfmin_cons->rnd.var = 0.25f;
    // todo: wt_scale??
  }
  else {
    topfcfmin_cons->SetUnique("rnd", false);
  }

  topfcfmout_cons->SetUnique("wt_scale", true);
  topfcfmout_cons->wt_scale.rel = 0.5f;

  if(topo_prjns) {
    matrix_cons->SetUnique("rnd", true);
    matrix_cons->rnd.mean = 0.5f; 
    matrix_cons->rnd.var = .25f; 
  }
  else {
    matrix_cons->SetUnique("rnd", false);
  }
  matrix_cons->SetUnique("lrate", true);
  matrix_cons->lrate = .005f;
  matrix_cons->SetUnique("wt_sig", true);
  matrix_cons->wt_sig.gain = 6.0f;
  matrix_cons->wt_sig.off = 1.0f;
  matrix_cons->stable_mix.stable_pct = 0.0f; // better without

  matrix_cons->SetUnique("ignore_unlearnable", true);
  matrix_cons->ignore_unlearnable = false; 

  matrix_cons->lrate_sched.SetSize(2);
  matrix_cons->lrate_sched.default_val = 0.0f; // this is the value that happens prior to stats being collected
  matrix_cons->lrate_sched[0]->start_ctr = 0;
  matrix_cons->lrate_sched[0]->start_val = 0.0f;
  matrix_cons->lrate_sched[1]->start_ctr = 10; // enough time for reps pretrain
  matrix_cons->lrate_sched[1]->start_val = 1.0f;
  matrix_cons->lrs_value = LeabraConSpec::NO_LRS;

  matrix_cons_topo->SetUnique("rnd", true);
  matrix_cons_topo->rnd.mean = 0.0f;
  matrix_cons_topo->rnd.var = .25f;
  matrix_cons_topo->SetUnique("wt_sig", false);
  matrix_cons_topo->SetUnique("lmix", false);

  if(topo_prjns) {
    matrix_cons_nogo->SetUnique("rnd", true);
    matrix_cons_nogo->rnd.mean = 0.0f;
    matrix_cons_nogo->rnd.var = .25f;
  }
  else {
    matrix_cons_nogo->SetUnique("rnd", false);
  }
  matrix_cons_nogo->SetUnique("wt_scale", true);
  matrix_cons_nogo->wt_scale.abs = 1.0f;
  matrix_cons_nogo->SetUnique("wt_sig", false);
  matrix_cons_nogo->SetUnique("lmix", false);

  matrix_cons_nogofmgo->SetUnique("wt_scale", true);
  matrix_cons_nogofmgo->wt_scale.rel = 0.2f;

  matrix_cons_topo_weak->SetUnique("wt_scale", true);
  matrix_cons_topo_weak->wt_scale.rel = .2f;

  matrix_cons_topo_strong->SetUnique("wt_scale", true);
  matrix_cons_topo_strong->wt_scale.rel = 2.0f;

  matrix_cons_out->SetUnique("immed_trace", true);
  matrix_cons_out->immed_trace = true;
  matrix_cons_out->SetUnique("mnt_decay", true);
  matrix_cons_out->mnt_decay = 1.0f;
  matrix_cons_out->SetUnique("no_mnt_decay", true);
  matrix_cons_out->no_mnt_decay = 1.0f;

  matrix_to_snrthal->SetUnique("rnd", true);
  matrix_to_snrthal->rnd.mean = 0.7f;
  matrix_to_snrthal->rnd.var = 0.0f;
  matrix_to_snrthal->SetUnique("learn", true);
  matrix_to_snrthal->learn = false;
  matrix_to_snrthal->SetUnique("lrate", true);
  matrix_to_snrthal->lrate = 0.0f;

  matrix_bias->SetUnique("lrate", true);
  matrix_bias->lrate = 0.0f;            // default is no bias learning
  matrix_bias->SetUnique("learn", true);
  matrix_bias->learn = false;

  fixed_bias->SetUnique("lrate", true);
  fixed_bias->lrate = 0.0f;
  fixed_bias->SetUnique("learn", true);
  fixed_bias->learn = false;

  // fmpfc_out->SetUnique("wt_scale", true);
  fmpfc_out->stable_mix.stable_pct = 0.8f;

  matrix_go_mnt_sp->go_nogo = MatrixLayerSpec::GO;
  matrix_go_mnt_sp->gating_type = SNrThalLayerSpec::MNT;
  matrix_go_in_sp->SetUnique("gating_type",true);
  matrix_go_in_sp->gating_type = SNrThalLayerSpec::INPUT;
  matrix_go_out_sp->SetUnique("gating_type",true);
  matrix_go_out_sp->gating_type = SNrThalLayerSpec::OUTPUT;
  if(matrix_go_mnt_out_sp) {
    matrix_go_mnt_out_sp->SetUnique("gating_type",true);
    matrix_go_mnt_out_sp->gating_type = SNrThalLayerSpec::MNT_OUT;
  }
  if(matrix_go_out_mnt_sp) {
    matrix_go_out_mnt_sp->SetUnique("gating_type",true);
    matrix_go_out_mnt_sp->gating_type = SNrThalLayerSpec::OUT_MNT;  
  }

  matrix_nogo_mnt_sp->SetUnique("go_nogo",true);
  matrix_nogo_mnt_sp->go_nogo = MatrixLayerSpec::NOGO;
  matrix_nogo_mnt_sp->gating_type = SNrThalLayerSpec::MNT;
  matrix_nogo_in_sp->SetUnique("gating_type",true);
  matrix_nogo_in_sp->gating_type = SNrThalLayerSpec::INPUT;
  matrix_nogo_out_sp->SetUnique("gating_type",true);
  matrix_nogo_out_sp->gating_type = SNrThalLayerSpec::OUTPUT;
  if(matrix_nogo_mnt_out_sp) {
    matrix_nogo_mnt_out_sp->SetUnique("gating_type",true);
    matrix_nogo_mnt_out_sp->gating_type = SNrThalLayerSpec::MNT_OUT;  
  }
  if(matrix_nogo_out_mnt_sp) {
    matrix_nogo_out_mnt_sp->SetUnique("gating_type",true);
    matrix_nogo_out_mnt_sp->gating_type = SNrThalLayerSpec::OUT_MNT;  
  }

  pfc_mnt_sp->pfc_type = SNrThalLayerSpec::MNT;
  pfc_mnt_sp->gate.pregate_gain = 1.0f;

  pfc_in_sp->SetUnique("pfc_type",true);
  pfc_in_sp->pfc_type = SNrThalLayerSpec::INPUT;
  pfc_in_sp->SetUnique("gate",true);
  pfc_in_sp->gate.pregate_gain = 1.0f;
  pfc_in_sp->gate.max_maint = 1;

  pfc_out_sp->SetUnique("pfc_type",true);
  pfc_out_sp->pfc_type = SNrThalLayerSpec::OUTPUT;
  pfc_out_sp->SetUnique("gate",true);
  pfc_out_sp->gate.pregate_gain = 0.0f;
  pfc_out_sp->gate.max_maint = 1;

  pfc_out_sp->SetUnique("gp_kwta",true);
  pfc_out_sp->gp_kwta.diff_act_pct = true;
  pfc_out_sp->gp_kwta.act_pct = 0.05f;

  // pfc_mnt_out_sp->SetUnique("pfc_type",true);
  // pfc_mnt_out_sp->pfc_type = SNrThalLayerSpec::MNT_OUT;
  // pfc_mnt_out_sp->SetUnique("gate",true);
  // pfc_mnt_out_sp->gate.pregate_gain = 1.0f;

  // pfc_out_mnt_sp->SetUnique("pfc_type",true);
  // pfc_out_mnt_sp->pfc_type = SNrThalLayerSpec::OUT_MNT;
  // pfc_out_mnt_sp->SetUnique("gate",true);
  // pfc_out_mnt_sp->gate.pregate_gain = 1.0f;

  // unit_gp_inhib.act_denom params set in basic config b/c depends on n stripes

  snrthal_units->SetUnique("maxda", true);
  snrthal_units->maxda.val = MaxDaSpec::NO_MAX_DA;

  snrthalsp_out->SetUnique("kwta", true);
  snrthalsp_out->kwta.k = 2;
  snrthalsp_out->SetUnique("snrthal", true);
  snrthalsp_out->snrthal.go_thr = 0.1f;
  snrthalsp_out->snrthal.min_cycle = 15;
  snrthalsp_out->snrthal.max_cycle = 40;

  fullprjn->self_con = true;

  //////////	Prjns

  topomaster->wt_range.min = 0.1f;
  topomaster->wt_range.max = 0.5f;
  topomaster->grad_type = TopoWtsPrjnSpec::GAUSSIAN;
  topomaster->gauss_sig = 0.1f;
  topomaster->index_by_gps_recv.on = false;
  topomaster->index_by_gps_send.on = false;

  if(!set_defs) {
    topofminput->SetUnique("topo_pattern", true);
    topofminput->topo_pattern = TopoWtsPrjnSpec::Y2X;
    topofminput->SetUnique("wrap_reflect", true);
    topofminput->wrap_reflect = TopoWtsPrjnSpec::NONE;
    topofminput->SetUnique("index_by_gps_recv", true);
    topofminput->index_by_gps_recv.on = true;
    topofminput->SetUnique("index_by_gps_send", true);
    topofminput->index_by_gps_send.on = false;
    topofminput->SetUnique("custom_send_range", true);
    topofminput->custom_send_range = true;
    topofminput->SetUnique("send_range_start", true);
    topofminput->send_range_start.x = 0;
    topofminput->send_range_start.y = 0;
    topofminput->SetUnique("send_range_end", true);
    topofminput->send_range_end.x = -1;
    topofminput->SetUnique("custom_recv_range", true);
    topofminput->custom_recv_range = false;

    topomatrixpfc_self->SetUnique("topo_pattern", true);
    topomatrixpfc_self->topo_pattern = TopoWtsPrjnSpec::X2X_Y2Y;
    topomatrixpfc_self->SetUnique("wrap_reflect", true);
    topomatrixpfc_self->wrap_reflect = TopoWtsPrjnSpec::NONE;
    topomatrixpfc_self->SetUnique("index_by_gps_recv", true);
    topomatrixpfc_self->index_by_gps_recv.on = true;
    topomatrixpfc_self->SetUnique("index_by_gps_send", true);
    topomatrixpfc_self->index_by_gps_send.on = true;
    topomatrixpfc_self->SetUnique("custom_send_range", true);
    topomatrixpfc_self->custom_send_range = false;
    topomatrixpfc_self->SetUnique("custom_recv_range", true);
    topomatrixpfc_self->custom_recv_range = false;

    topomatrixpfc_other->SetUnique("topo_pattern", true);
    topomatrixpfc_other->topo_pattern = TopoWtsPrjnSpec::X2X_Y2Y;
    topomatrixpfc_other->SetUnique("wrap_reflect", true);
    topomatrixpfc_other->wrap_reflect = TopoWtsPrjnSpec::NONE;
    topomatrixpfc_other->SetUnique("index_by_gps_recv", true);
    topomatrixpfc_other->index_by_gps_recv.on = true;
    topomatrixpfc_other->SetUnique("index_by_gps_send", true);
    topomatrixpfc_other->index_by_gps_send.on = true;
    topomatrixpfc_other->SetUnique("custom_send_range", true);
    topomatrixpfc_other->custom_send_range = false;
    topomatrixpfc_other->SetUnique("custom_recv_range", true);
    topomatrixpfc_other->custom_recv_range = false;

    intrapfctopo->SetUnique("topo_pattern", true);
    intrapfctopo->topo_pattern = TopoWtsPrjnSpec::X2X_Y2Y;
    intrapfctopo->SetUnique("wrap_reflect", true);
    intrapfctopo->wrap_reflect = TopoWtsPrjnSpec::NONE;
    intrapfctopo->SetUnique("index_by_gps_recv", true);
    intrapfctopo->index_by_gps_recv.on = true;
    intrapfctopo->SetUnique("index_by_gps_send", true);
    intrapfctopo->index_by_gps_send.on = true;
    intrapfctopo->SetUnique("custom_send_range", true);
    intrapfctopo->custom_send_range = false;
    intrapfctopo->SetUnique("custom_recv_range", true);
    intrapfctopo->custom_recv_range = false;
  }

  for(int j=0;j<pbwmspgp->leaves;j++) {
    BaseSpec* sp = (BaseSpec*)net->specs.Leaf(j);
    sp->UpdateAfterEdit();
  }

  //////////////////////////////////////////////////////////////////////////////////
  // select edit

  LeabraProject* proj = GET_MY_OWNER(LeabraProject);
  SelectEdit* edit = proj->FindMakeSelectEdit("PBWM");
  if(edit != NULL) {
    edit->auto_edit = true;
    String subgp;
    subgp = "PFC";
    pfc_mnt_sp->SelectForEditNm("gate", edit, "pfc_mnt", subgp);
    pfc_mnt_sp->SelectForEditNm("gp_kwta", edit, "pfc_mnt", subgp,
      "pfc kwta parameters -- pct is main param for pfc dynamics, and act_pct for balancing excitation to other layers");
    topfc_cons->SelectForEditNm("lrate", edit, "to_pfc", subgp,
        "PFC requires a slower learning rate in general, around .002");

    subgp = "Matrix";
    matrix_go_mnt_sp->SelectForEditNm("matrix", edit, "matrix", subgp);
    matrix_go_mnt_sp->SelectForEditNm("gp_kwta", edit, "matrix", subgp,
      "matrix kwta parameters -- pct, gp_g are main for matrix dynamics (gp_g = 1 almost always best)");

    matrix_units->SelectForEditNm("noise", edit, "matrix", subgp,
      "matrix noise -- variance around .001 seems best overall");
    matrix_units->SelectForEditNm("noise_adapt", edit, "matrix", subgp);
    matrix_cons->SelectForEditNm("lrate", edit, "matrix", subgp,
     "Default Matrix lrate is .002");

    subgp = "SNrThal";
    snrthalsp->SelectForEditNm("kwta", edit, "snrthal", subgp,
      "snrthal kwta parameter -- how many stripes can gate at once");
    snrthalsp->SelectForEditNm("inhib", edit, "snrthal", subgp,
      "Default is KWTA_AVG_INHIB with kwta_pt = .7 -- more competition but with some flexibility from avg-based computation");
    snrthalsp->SelectForEditNm("snrthal", edit, "snrthal", subgp);
  }
  return true;
}

static void lay_set_geom(LeabraLayer* lay, int n_stripes, int n_units = -1, bool sp = true,
                         int gp_geom_x=-1, int gp_geom_y=-1) {
  lay->unit_groups = true;
  lay->SetNUnitGroups(n_stripes);
  if(gp_geom_x > 0 && gp_geom_y > 0) {
    lay->gp_geom.x = gp_geom_x;
    lay->gp_geom.y = gp_geom_y;
  }
  else {
    if(n_stripes <= 5 || n_stripes == 7 || n_stripes == 11 || n_stripes >= 13) {
      lay->gp_geom.x = n_stripes;
      lay->gp_geom.y = 1;
    }
    else{
      if(n_stripes == 6 || n_stripes == 8 || n_stripes == 10) {
        lay->gp_geom.x = n_stripes / 2;
        lay->gp_geom.y = 2;
      }
      else {
        if(n_stripes == 9 || n_stripes == 12) {
          lay->gp_geom.x = n_stripes / 3;
          lay->gp_geom.y = 3;
        }
      }
    }
  } // default whatever SetNUnitGroups() set..
  if(n_units > 0) {
    lay->SetNUnits(n_units);
  }
  if(sp) {
    lay->gp_spc.x = 1;
    lay->gp_spc.y = 1;
  }
  lay->UpdateAfterEdit();
}

static void set_n_stripes(LeabraNetwork* net, const String& gpnm, const String& nm,
                          int n_stripes, int n_units, bool sp, int gp_geom_x=-1,
                          int gp_geom_y=-1)
{
  Layer_Group* laygp = (Layer_Group*)net->layers.gp.FindName(gpnm);
  if(!laygp) return;
  LeabraLayer* lay = (LeabraLayer*)laygp->FindLeafName(nm);
  if(!lay) return;
  lay_set_geom(lay, n_stripes, n_units, sp, gp_geom_x, gp_geom_y);
}

bool LeabraWizard::PBWM_SetNStripes(LeabraNetwork* net, int in_stripes, int mnt_stripes,
				    int out_stripes, bool one_snr, int n_matrix_units,
				    int n_pfc_units, const String& prefix) {
  if(TestError(!net, "PBWM_SetNStripes", "network is NULL -- only makes sense to run on an existing network -- aborting!"))
    return false;

  // this is called as a subroutine a lot too so don't save here -- could do impl but
  // not really worth it..
//   LeabraProject* proj = GET_MY_OWNER(LeabraProject);
//   if(proj) {
//     proj->undo_mgr.SaveUndo(net, "Wizard::PBWM_SetNStripes -- actually saves network specifically");
//   }

  set_n_stripes(net, prefix + "_PFC", "PFC_in",  in_stripes, n_pfc_units, true);
  set_n_stripes(net, prefix + "_PFC", "PFCd_in",  in_stripes, n_pfc_units, true);
  set_n_stripes(net, prefix + "_PFC", "PFC_mnt", mnt_stripes, n_pfc_units, true);
  set_n_stripes(net, prefix + "_PFC", "PFCd_mnt", mnt_stripes, n_pfc_units, true);
  set_n_stripes(net, prefix + "_PFC", "PFC_out", out_stripes, n_pfc_units, true);

  set_n_stripes(net, prefix + "_Go", "Matrix_Go_in",   in_stripes, n_matrix_units, true);
  set_n_stripes(net, prefix + "_NoGo", "Matrix_NoGo_in", in_stripes, n_matrix_units, true);
  set_n_stripes(net, prefix + "_Go", "Matrix_Go_mnt",  mnt_stripes, n_matrix_units, true);
  set_n_stripes(net, prefix + "_NoGo", "Matrix_NoGo_mnt",mnt_stripes, n_matrix_units, true);
  set_n_stripes(net, prefix + "_Go", "Matrix_Go_out",  out_stripes, n_matrix_units, true);
  set_n_stripes(net, prefix + "_NoGo", "Matrix_NoGo_out",out_stripes, n_matrix_units, true);

  int snr_stripes = in_stripes + mnt_stripes;
  if(!one_snr) {
    if(out_stripes > 0) {
      set_n_stripes(net, prefix + "_Go", "SNrThal_out", out_stripes, 1, true);
    }
  }
  else {
    snr_stripes += out_stripes;
  }
  if(snr_stripes > in_stripes && snr_stripes > mnt_stripes &&
     (!one_snr || snr_stripes > out_stripes)) {
    set_n_stripes(net, prefix + "_Go", "SNrThal", snr_stripes, 1, true, snr_stripes, 1);
  }
  else {
    set_n_stripes(net, prefix + "_Go", "SNrThal", snr_stripes, 1, true);
  }

  net->Build();
  return true;
}

bool LeabraWizard::PBWM(LeabraNetwork* net, int in_stripes, int mnt_stripes,
			int out_stripes, bool one_snr, bool topo_prjns,
                        bool add_on, const String& prefix) {
  if(!net) {
    LeabraProject* proj = GET_MY_OWNER(LeabraProject);
    net = (LeabraNetwork*)proj->GetNewNetwork();
    if(TestError(!net, "PBWM", "network is NULL and could not make a new one -- aborting!"))
      return false;
    if(!StdNetwork()) return false;
  }

  // first configure PVLV system..
  if(TestError(!PVLV(net, false), "PBWM", "could not make PVLV")) return false;

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

  net->ti_mode = true;
  net->RemoveUnits();

  // TODO: Note below gets rid of LeabraWizard PBWM CHECK ERROR message -- works and seems safe to do
  net->no_plus_test = false;

  String pvenm = "PVe";  String pvinm = "PVi";  String pvrnm = "PVr";
  String lvenm = "LVe";  String lvinm = "LVi";  String nvnm = "NV";
  String vtanm = "VTA";

  //////////////////////////////////////////////////////////////////////////////////
  // make layers

  // Harvest from the PVLV function..
  Layer_Group* pvlv_laygp = net->FindMakeLayerGroup("PVLV");
  LeabraLayer* rew_targ_lay = (LeabraLayer*)pvlv_laygp->FindName("RewTarg");
  LeabraLayer* pve = (LeabraLayer*)pvlv_laygp->FindName(pvenm);
  LeabraLayer* pvr = (LeabraLayer*)pvlv_laygp->FindName(pvrnm);
  LeabraLayer* pvi = (LeabraLayer*)pvlv_laygp->FindName(pvinm);
  LeabraLayer* lve = (LeabraLayer*)pvlv_laygp->FindName(lvenm);
  // LeabraLayer* lvi = (LeabraLayer*)pvlv_laygp->FindName(lvinm);
  LeabraLayer* nv =  (LeabraLayer*)pvlv_laygp->FindName(nvnm);
  LeabraLayer* vta = (LeabraLayer*)pvlv_laygp->FindName(vtanm);
  if(!vta)
    vta = (LeabraLayer*)pvlv_laygp->FindName("DA");

  bool new_pbwm_laygp = false;
  Layer_Group* pbwm_laygp_go = net->FindMakeLayerGroup
    (prefix + "_Go", NULL,     new_pbwm_laygp);
  Layer_Group* pbwm_laygp_nogo = net->FindMakeLayerGroup
    (prefix + "_NoGo", NULL, new_pbwm_laygp);
  Layer_Group* pbwm_laygp_pfc = net->FindMakeLayerGroup
    (prefix + "_PFC", NULL, new_pbwm_laygp);

  // new gets full update, otherwise more just params
  bool matrix_new =   false; 
  bool pfc_new = false; 
  bool pfcd_new = false; 
  bool snrthal_new = false; 
  bool snrthal_out_new = false; 

  LeabraLayer* pfc_mnt = NULL;
  LeabraLayer* pfc_out = NULL;
  LeabraLayer* pfc_in = NULL;
  LeabraLayer* pfc_mnt_d = NULL;
  LeabraLayer* pfc_out_d = NULL;
  LeabraLayer* pfc_in_d = NULL;

  LeabraLayer* matrix_go_in = NULL;
  LeabraLayer* matrix_go_mnt = NULL;
  LeabraLayer* matrix_go_out = NULL;
  LeabraLayer* matrix_nogo_in = NULL;
  LeabraLayer* matrix_nogo_mnt = NULL;
  LeabraLayer* matrix_nogo_out = NULL;
  LeabraLayer* snrthal = NULL;
  LeabraLayer* snrthal_out = NULL;

  // stick this in go -- must be first!
  snrthal = (LeabraLayer*)pbwm_laygp_go->FindMakeLayer("SNrThal", NULL, snrthal_new);
  if(!one_snr && out_stripes > 0) {
    snrthal_out = (LeabraLayer*)pbwm_laygp_go->FindMakeLayer("SNrThal_out", NULL,
                                                             snrthal_out_new);
  }

  if(in_stripes > 0) {
    matrix_go_in = (LeabraLayer*)pbwm_laygp_go->FindMakeLayer("Matrix_Go_in", NULL,
							      matrix_new);
    matrix_nogo_in = (LeabraLayer*)pbwm_laygp_nogo->FindMakeLayer("Matrix_NoGo_in", NULL,
								  matrix_new);
    pfc_in =  (LeabraLayer*)pbwm_laygp_pfc->FindMakeLayer("PFC_in",  NULL, pfc_new);
    pfc_in_d =  (LeabraLayer*)pbwm_laygp_pfc->FindMakeLayer("PFCd_in",  NULL, pfcd_new);
  }

  if(mnt_stripes > 0) {
    matrix_go_mnt = (LeabraLayer*)pbwm_laygp_go->FindMakeLayer("Matrix_Go_mnt", NULL,
							      matrix_new, "Matrix");
    matrix_nogo_mnt = (LeabraLayer*)pbwm_laygp_nogo->FindMakeLayer("Matrix_NoGo_mnt", NULL,
								  matrix_new);
    pfc_mnt = (LeabraLayer*)pbwm_laygp_pfc->FindMakeLayer("PFC_mnt", NULL, pfc_new);
    pfc_mnt_d = (LeabraLayer*)pbwm_laygp_pfc->FindMakeLayer("PFCd_mnt", NULL, pfcd_new);
  }

  if(out_stripes > 0) {
    matrix_go_out = (LeabraLayer*)pbwm_laygp_go->FindMakeLayer("Matrix_Go_out", NULL,
							      matrix_new, "Matrix");
    matrix_nogo_out = (LeabraLayer*)pbwm_laygp_nogo->FindMakeLayer("Matrix_NoGo_out", NULL,
								  matrix_new);
    pfc_out = (LeabraLayer*)pbwm_laygp_pfc->FindMakeLayer("PFC_out", NULL, pfc_new);
    // if(make_deep_pfc) {
    //   pfc_out_d = (LeabraLayer*)pbwm_laygp_pfc->FindMakeLayer("PFCd_out", NULL, pfcd_new);
    // }
  }

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
    if(lay != rew_targ_lay && lay != pve && lay != pvr && lay != pvi
       && lay != lve && lay != nv && lay != vta
       && lay != snrthal && lay != snrthal_out 
       && lay != matrix_go_in && lay != matrix_go_mnt && lay != matrix_go_out
       && lay != matrix_nogo_in && lay != matrix_nogo_mnt && lay != matrix_nogo_out
       && lay != pfc_mnt && lay != pfc_out && lay != pfc_in
       && lay != pfc_mnt_d && lay != pfc_in_d && lay != pfc_out_d) {
      other_lays.Link(lay);
      lay->GetAbsPos(lpos);
      if(lpos.z == 0) {
        if(!(lay->name.contains("Matrix") || lay->name.contains("PFC") ||
             lay->name.contains("SNr")))
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
  }

  //////////////////////////////////////////////////////////////////////////////////
  // get specs

  String pvlvprefix = "PVLV";
  BaseSpec_Group* pvlvspgp = net->FindMakeSpecGp(pvlvprefix);
  if(!pvlvspgp) return false;

  if(!add_on || pvlvspgp->leaves == 0) { // only if main guy or nothing yet
    if(!PVLV_Specs(net)) return false;
  }

  BaseSpec_Group* pbwmspgp = net->FindMakeSpecGp(prefix);
  if(!pbwmspgp) return false;

  if(!PBWM_Specs(net, topo_prjns)) return false;

  // different PVLV defaults -- not relevant anyway
  PvlvSp("LVeLayer",LVeLayerSpec)->lv.min_lvi = 0.4f;

  PvlvSp("NVLayer",NVLayerSpec)->nv.da_gain = 0.0f;      // no nv by default
  PvlvSp("VTALayer",PVLVDaLayerSpec)->da.da_gain = 1.0f;
  PvlvSp("VTALayer",PVLVDaLayerSpec)->da.pv_gain = 0.1f;

  //////////////////////////////////////////////////////////////////////////////////
  // apply specs to objects

  LeabraUnitSpec* pfc_units = PbwmSp("PFCUnits",PFCUnitSpec);
  LeabraUnitSpec* pfcd_units = PbwmSp("PFCDeepUnits",LayerActUnitSpec);
  LeabraUnitSpec* matrix_units = PbwmSp("MatrixUnits",LeabraUnitSpec);
  LeabraUnitSpec* matrix_nogo_units = PbwmSp("MatrixNoGo",LeabraUnitSpec);
  LeabraTICtxtLayerSpec* pfc_deep_sp = PbwmSp("PFC_deep", LeabraTICtxtLayerSpec);

  // set bias specs for unit specs
  pfc_units->bias_spec.SetSpec(PbwmSp("PFCBias",LeabraBiasSpec));
  matrix_units->bias_spec.SetSpec(PbwmSp("MatrixBias", MatrixBiasSpec));
  matrix_nogo_units->bias_spec.SetSpec(PbwmSp("MatrixBias", MatrixBiasSpec));
  PbwmSp("SNrThalUnits",LeabraUnitSpec)->bias_spec.SetSpec(PbwmSp("FixedBias",
                                                                  LeabraBiasSpec));

  snrthal->SetLayerSpec(PbwmSp("SNrThalLayer",SNrThalLayerSpec));
  snrthal->SetUnitSpec(PbwmSp("SNrThalUnits",LeabraUnitSpec));

  if(in_stripes > 0) {
    matrix_go_in->SetLayerSpec(PbwmSp("Matrix_Go_in",MatrixLayerSpec));
    matrix_go_in->SetUnitSpec(matrix_units);
    matrix_nogo_in->SetLayerSpec(PbwmSp("Matrix_NoGo_in",MatrixLayerSpec)); 
    matrix_nogo_in->SetUnitSpec(matrix_nogo_units);

    pfc_in->SetLayerSpec(PbwmSp("PFC_in",PFCLayerSpec));
    pfc_in->SetUnitSpec(pfc_units);
    if(pfc_in_d) {
      pfc_in_d->SetLayerSpec(pfc_deep_sp);
      pfc_in_d->SetUnitSpec(pfcd_units);
    }
  }
  if(mnt_stripes > 0) {
    matrix_go_mnt->SetLayerSpec(PbwmSp("Matrix_Go_mnt", MatrixLayerSpec));
    matrix_go_mnt->SetUnitSpec(matrix_units);
    matrix_nogo_mnt->SetLayerSpec(PbwmSp("Matrix_NoGo_mnt", MatrixLayerSpec)); 
    matrix_nogo_mnt->SetUnitSpec(matrix_nogo_units);

    pfc_mnt->SetLayerSpec(PbwmSp("PFC_mnt",PFCLayerSpec));
    pfc_mnt->SetUnitSpec(pfc_units);
    if(pfc_mnt_d) {
      pfc_mnt_d->SetLayerSpec(pfc_deep_sp);
      pfc_mnt_d->SetUnitSpec(pfcd_units);
    }
  }
  if(out_stripes > 0) {
    matrix_go_out->SetLayerSpec(PbwmSp("Matrix_Go_out",MatrixLayerSpec));
    matrix_go_out->SetUnitSpec(matrix_units);
    matrix_nogo_out->SetLayerSpec(PbwmSp("Matrix_NoGo_out",MatrixLayerSpec)); 
    matrix_nogo_out->SetUnitSpec(matrix_nogo_units);

    pfc_out->SetLayerSpec(PbwmSp("PFC_out",PFCLayerSpec));
    pfc_out->SetUnitSpec(pfc_units);
    if(pfc_out_d) {
      pfc_out_d->SetLayerSpec(pfc_deep_sp);
      pfc_out_d->SetUnitSpec(pfcd_units);
    }
    if(snrthal_out) {
      snrthal_out->SetLayerSpec(PbwmSp("SNrThalLayer_out",SNrThalLayerSpec));
      snrthal_out->SetUnitSpec(PbwmSp("SNrThalUnits",LeabraUnitSpec));
    }
  }

  //////////////////////////////////////////////////////////////////////////////////
  // make projections

  Projection* prjn = NULL;


  MarkerConSpec* marker_cons = PbwmSp("PbwmMarker", MarkerConSpec);
  OneToOnePrjnSpec* onetoone = PbwmSp("PbwmOneToOne", OneToOnePrjnSpec);
  FullPrjnSpec* fullprjn = PbwmSp("PbwmFullPrjn", FullPrjnSpec);
  GpOneToOnePrjnSpec* gponetoone = PbwmSp("PbwmGpOneToOne",GpOneToOnePrjnSpec);
  MarkerGpOneToOnePrjnSpec* markergponetoone =
    PbwmSp("MarkerGpOneToOne",MarkerGpOneToOnePrjnSpec);
  SNrPrjnSpec* snr_prjn = PbwmSp("SNrPrjn",SNrPrjnSpec);
  
  LeabraConSpec* matrix_to_snrthal = PbwmSp("MatrixToSNrThal",LeabraConSpec);
  MatrixConSpec* matrix_cons_nogofmgo = PbwmSp("MatrixConsNoGoFmGo", MatrixConSpec);

  //	  	 	   to		 from		prjn_spec	con_spec

  // matrix <-> snrthal
  if(in_stripes > 0) {
    net->FindMakePrjn(snrthal, matrix_go_in, snr_prjn, matrix_to_snrthal);
    net->FindMakePrjn(matrix_go_in, matrix_nogo_in, markergponetoone, marker_cons);
    net->FindMakePrjn(matrix_go_in, snrthal, snr_prjn, marker_cons);
    net->FindMakePrjn(matrix_go_in, vta, fullprjn, marker_cons);

    net->FindMakePrjn(matrix_nogo_in, snrthal, snr_prjn, marker_cons);
    net->FindMakePrjn(matrix_nogo_in, vta, fullprjn, marker_cons);
    net->FindMakePrjn(matrix_nogo_in, matrix_go_in, gponetoone, matrix_cons_nogofmgo);
  }
  if(mnt_stripes > 0) {
    net->FindMakePrjn(snrthal, matrix_go_mnt, snr_prjn, matrix_to_snrthal);
    net->FindMakePrjn(matrix_go_mnt, matrix_nogo_mnt, markergponetoone, marker_cons);
    net->FindMakePrjn(matrix_go_mnt, snrthal, snr_prjn, marker_cons);
    net->FindMakePrjn(matrix_go_mnt, vta, fullprjn, marker_cons);

    net->FindMakePrjn(matrix_nogo_mnt, snrthal, snr_prjn, marker_cons);
    net->FindMakePrjn(matrix_nogo_mnt, vta, fullprjn, marker_cons);
    net->FindMakePrjn(matrix_nogo_mnt, matrix_go_mnt, gponetoone,
		      matrix_cons_nogofmgo);
  }
  if(out_stripes > 0) {
    if(snrthal_out) {
      net->FindMakePrjn(snrthal_out, matrix_go_out, snr_prjn, matrix_to_snrthal);
      net->FindMakePrjn(matrix_go_out, snrthal_out, snr_prjn, marker_cons);
      net->FindMakePrjn(matrix_nogo_out, snrthal_out, snr_prjn, marker_cons);
    }
    else {
      net->FindMakePrjn(snrthal, matrix_go_out, snr_prjn, matrix_to_snrthal);
      net->FindMakePrjn(matrix_go_out, snrthal, snr_prjn, marker_cons);
      net->FindMakePrjn(matrix_nogo_out, snrthal, snr_prjn, marker_cons);
    }
    net->FindMakePrjn(matrix_go_out, matrix_nogo_out, markergponetoone, marker_cons);
    net->FindMakePrjn(matrix_go_out, vta, fullprjn, marker_cons);

    net->FindMakePrjn(matrix_nogo_out, vta, fullprjn, marker_cons);
    net->FindMakePrjn(matrix_nogo_out, matrix_go_out, gponetoone, matrix_cons_nogofmgo);
  }

  // matrix <-> pfc and pfc <-> pfc

  LeabraTICtxtConSpec* pfc_ctxt_cons = PbwmSp("PfcTICtxt", LeabraTICtxtConSpec);
  TopoWtsPrjnSpec* topomatrixpfc_self = PbwmSp("TopoMatrixPFC_Self",TopoWtsPrjnSpec);
  TopoWtsPrjnSpec* intrapfctopo = PbwmSp("TopoIntraPFC", TopoWtsPrjnSpec);
  PFCConSpec* topfc_cons = PbwmSp("ToPFC", PFCConSpec);
  MatrixConSpec* matrix_cons = PbwmSp("MatrixCons", MatrixConSpec);
  MatrixConSpec* matrix_cons_out = PbwmSp("MatrixConsOut", MatrixConSpec);
  MatrixConSpec* matrix_cons_nogo = PbwmSp("MatrixConsNoGo", MatrixConSpec);

  if(in_stripes > 0) {
    if(topo_prjns) {
      net->FindMakePrjn(matrix_go_in, pfc_in, topomatrixpfc_self,
                        PbwmSp("MatrixConsTopo",MatrixConSpec));
      net->FindMakePrjn(matrix_nogo_in, pfc_in, topomatrixpfc_self,
                        matrix_cons_nogo);
    }
    else {
      net->FindMakePrjn(matrix_go_in, pfc_in, gponetoone, matrix_cons);
      net->FindMakePrjn(matrix_nogo_in, pfc_in, gponetoone,
                        matrix_cons_nogo);
    }
    net->FindMakePrjn(pfc_in, snrthal, snr_prjn, marker_cons);
    net->FindMakePrjn(pfc_in, pfc_in, onetoone, pfc_ctxt_cons);

    if(pfc_in_d) {
      net->FindMakePrjn(pfc_in_d, pfc_in, onetoone, marker_cons);
    }
  }

  if(mnt_stripes > 0) {
    if(topo_prjns) {
      net->FindMakePrjn(matrix_go_mnt, pfc_mnt, topomatrixpfc_self,
                        PbwmSp("MatrixConsTopoWeak",MatrixConSpec));
      net->FindMakePrjn(matrix_nogo_mnt, pfc_mnt, topomatrixpfc_self,
                        matrix_cons_nogo);
    }
    else {
      net->FindMakePrjn(matrix_go_mnt, pfc_mnt, gponetoone, matrix_cons);
      net->FindMakePrjn(matrix_nogo_mnt, pfc_mnt, markergponetoone,
                        marker_cons);
    }
    net->FindMakePrjn(pfc_mnt, snrthal, snr_prjn, marker_cons);
    net->FindMakePrjn(pfc_mnt, pfc_mnt, onetoone, pfc_ctxt_cons);

    if(pfc_mnt_d) {
      net->FindMakePrjn(pfc_mnt_d, pfc_mnt, onetoone, marker_cons);
    }

    if(in_stripes > 0) {
      if(topo_prjns) {
        net->FindMakePrjn(pfc_mnt, pfc_in, intrapfctopo, topfc_cons);
        // net->FindMakePrjn(matrix_nogo_mnt, pfc_in, topomatrixpfc_other,
        //                   matrix_cons_nogo);
      }
      else {
        net->FindMakePrjn(pfc_mnt, pfc_in, fullprjn, topfc_cons);
        // net->FindMakePrjn(matrix_nogo_mnt, pfc_in, fullprjn,
        //                   matrix_cons_nogo); // why is this prjn here?
      }
    }
    if(out_stripes > 0) {
      if(topo_prjns) {
        prjn = net->FindMakePrjn(pfc_mnt, pfc_out, intrapfctopo,
                                 PbwmSp("PFCtoPFC",PFCConSpec));
      }
      else {
        prjn = net->FindMakePrjn(pfc_mnt, pfc_out, fullprjn, 
                                 PbwmSp("PFCtoPFC",PFCConSpec));
      }
    }
  }

  if(out_stripes > 0) {
    if(snrthal_out) {
      net->FindMakePrjn(pfc_out, snrthal_out, snr_prjn, marker_cons);
    }
    else {
      net->FindMakePrjn(pfc_out, snrthal, snr_prjn, marker_cons);
    }

    if(in_stripes > 0) {
      if(topo_prjns) {
        net->FindMakePrjn(pfc_out, pfc_in, intrapfctopo, topfc_cons);
        // net->FindMakePrjn(matrix_nogo_out, pfc_in, topomatrixpfc_other,
        //                   matrix_cons_nogo);
      }
      else {
        net->FindMakePrjn(pfc_out, pfc_in, fullprjn, topfc_cons);
        // net->FindMakePrjn(matrix_nogo_out, pfc_in, fullprjn,
        //                   matrix_cons_nogo);
      }
    }
    if(mnt_stripes > 0) {
      if(topo_prjns) {
        net->FindMakePrjn(pfc_out, pfc_mnt, intrapfctopo, topfc_cons);
        net->FindMakePrjn(pfc_out, pfc_mnt_d, intrapfctopo, topfc_cons);
        // net->FindMakePrjn(matrix_nogo_out, pfc_mnt, topomatrixpfc_other,
        //                   matrix_cons_nogo);
      }
      else {
        net->FindMakePrjn(pfc_out, pfc_mnt, fullprjn, topfc_cons);
        net->FindMakePrjn(pfc_out, pfc_mnt_d, fullprjn, topfc_cons);
        // net->FindMakePrjn(matrix_nogo_out, pfc_mnt, fullprjn,
        //                   matrix_cons_nogo);
      }
    }
  }

  // connections from PFC to pvlv seem to actually be bad when dealing with distributed
  // TI-like representations, as compared to the more symbolic kind -- parameterize
  if(pvi) {
    if(in_stripes > 0) {
      net->FindMakePrjn(pvi, pfc_in, fullprjn, PvlvSp("PViCons",PVConSpec));
      // net->FindMakePrjn(lve, pfc_in, fullprjn, lve_cons);
      // net->FindMakePrjn(nv,  pfc_in, fullprjn, nv_cons);
    }
    if(mnt_stripes > 0) {
      net->FindMakePrjn(pvi, pfc_mnt_d, fullprjn, PvlvSp("PViCons",PVConSpec));
      if(lve) {
        net->FindMakePrjn(lve, pfc_mnt_d, fullprjn, PvlvSp("LVeCons",PVConSpec));
      }
      // net->FindMakePrjn(nv,  pfc_mnt, fullprjn, nv_cons);
    }
  }
  if(pvr) {
    // if(in_stripes > 0) {
    //   net->FindMakePrjn(pvi, pfc_in, fullprjn, PvlvSp("PViCons",PVConSpec));
    //   // net->FindMakePrjn(lve, pfc_in, fullprjn, lve_cons);
    //   // net->FindMakePrjn(nv,  pfc_in, fullprjn, nv_cons);
    // }
    if(mnt_stripes > 0) {
      net->FindMakePrjn(pvr, pfc_mnt_d, fullprjn, PvlvSp("PVrCons",PVrConSpec));
      // net->FindMakePrjn(nv,  pfc_mnt, fullprjn, nv_cons);
    }
  }

  TopoWtsPrjnSpec* topofminput = PbwmSp("TopoFmInput", TopoWtsPrjnSpec);
  MatrixConSpec* matrix_cons_topo = PbwmSp("MatrixConsTopo", MatrixConSpec);
  LeabraConSpec* fmpfc_out = PbwmSp("FmPFC_out", LeabraConSpec);

  for(i=0;i<input_lays.size;i++) {
    Layer* il = (Layer*)input_lays[i];

    if(matrix_new) {  // posterior cortex presumably also projects from superficial..
      if(in_stripes > 0) {
        if(topo_prjns) {
          net->FindMakePrjn(matrix_go_in, il, topofminput, matrix_cons_topo);
          net->FindMakePrjn(matrix_nogo_in, il, topofminput, matrix_cons_nogo);
        }
        else {
          net->FindMakePrjn(matrix_go_in, il, fullprjn, matrix_cons);
          net->FindMakePrjn(matrix_nogo_in, il, fullprjn, matrix_cons_nogo);
        }
      }
      if(mnt_stripes > 0) {
        if(topo_prjns) {
          net->FindMakePrjn(matrix_go_mnt, il, topofminput, matrix_cons_topo);
          net->FindMakePrjn(matrix_nogo_mnt, il, topofminput, matrix_cons_nogo);
        }
        else {
          net->FindMakePrjn(matrix_go_mnt, il, fullprjn, matrix_cons);
          net->FindMakePrjn(matrix_nogo_mnt, il, fullprjn, matrix_cons_nogo);
        }
      }
      if(out_stripes > 0) {
        if(topo_prjns) {
          net->FindMakePrjn(matrix_go_out, il, topofminput, matrix_cons_out); // not right but..
          net->FindMakePrjn(matrix_nogo_out, il, topofminput, matrix_cons_out);
        }
        else {
          net->FindMakePrjn(matrix_go_out, il, fullprjn, matrix_cons_out);
          net->FindMakePrjn(matrix_nogo_out, il, fullprjn, matrix_cons_out); // out trumps nogo
        }
      }
    }

    if(pfc_new) {
      TopoWtsPrjnSpec* topofminput = PbwmSp("TopoFmInput", TopoWtsPrjnSpec);
      PFCConSpec * topfcfmin_cons = PbwmSp("ToPFCFmInput", PFCConSpec);
      if(in_stripes > 0) {
        if(topo_prjns) {
          net->FindMakePrjn(pfc_in, il, topofminput, topfcfmin_cons);
        }
        else {
          net->FindMakePrjn(pfc_in, il, fullprjn, topfcfmin_cons);
        }
        if(il->layer_type == Layer::TARGET || il->layer_type == Layer::OUTPUT) {
          net->FindMakePrjn(il, pfc_in, fullprjn, fmpfc_out);
        }
      }
      else if(mnt_stripes > 0) { // only maint if no input
        if(topo_prjns) {
          net->FindMakePrjn(pfc_mnt, il, topofminput, topfcfmin_cons);
        }
        else {
          net->FindMakePrjn(pfc_mnt, il, fullprjn, topfcfmin_cons);
        }
        if(il->layer_type == Layer::TARGET || il->layer_type == Layer::OUTPUT) {
          net->FindMakePrjn(il, pfc_mnt, fullprjn, fmpfc_out);
        }
      }
    }
  }

  PFCConSpec* topfcfmout_cons = PbwmSp("ToPFCFmOutput", PFCConSpec);
  for(i=0;i<output_lays.size;i++) {
    Layer* ol = (Layer*)output_lays[i];

    if(pfc_new) {
      if(in_stripes > 0) {
        net->FindMakePrjn(pfc_in, ol, fullprjn, topfcfmout_cons);
      }
      if(out_stripes > 0) {
	net->FindMakePrjn(ol, pfc_out, fullprjn, fmpfc_out);
        net->FindMakePrjn(pfc_out, ol, fullprjn, topfcfmout_cons);
      }
      if(mnt_stripes > 0) {
	if(!(out_stripes > 0)) {
	  net->FindMakePrjn(ol, pfc_mnt, fullprjn, fmpfc_out);
	}
        net->FindMakePrjn(pfc_mnt, ol, fullprjn, topfcfmout_cons);
      }
    }
  }

  //////////////////////////////////////////////////////////////////////////////////
  // set positions & geometries

  snrthal->brain_area = ".*/.*/.*/.*/Substantia Nigra";
  if(snrthal_out)
    snrthal_out->brain_area = ".*/.*/.*/.*/Substantia Nigra";
  // these are just random suggestions:
  if(in_stripes > 0) {
    if(matrix_go_in->brain_area.empty()) 
      matrix_go_in->brain_area = ".*/.*/.*/.*/Caudate Body";
    if(matrix_nogo_in->brain_area.empty()) 
      matrix_nogo_in->brain_area = ".*/.*/.*/.*/Caudate Body";
    if(pfc_in->brain_area.empty()) {
      pfc_in->brain_area = ".*/.*/.*/.*/BA45";
    }
    if(pfc_in_d && pfc_in_d->brain_area.empty()) {
      pfc_in_d->brain_area = ".*/.*/.*/.*/BA45";
    }
  }
  if(mnt_stripes > 0) {
    if(matrix_go_mnt->brain_area.empty()) 
      matrix_go_mnt->brain_area = ".*/.*/.*/.*/Caudate Body";
    if(matrix_nogo_mnt->brain_area.empty()) 
      matrix_nogo_mnt->brain_area = ".*/.*/.*/.*/Caudate Body";
    if(pfc_mnt->brain_area.empty()) {
      pfc_mnt->brain_area = ".*/.*/.*/.*/BA9";
    }
    if(pfc_mnt_d && pfc_mnt_d->brain_area.empty()) {
      pfc_mnt_d->brain_area = ".*/.*/.*/.*/BA9";
    }
  }
  if(out_stripes > 0) {
    if(matrix_go_out->brain_area.empty()) 
      matrix_go_out->brain_area = ".*/.*/.*/.*/Caudate Body";
    if(matrix_nogo_out->brain_area.empty()) 
      matrix_nogo_out->brain_area = ".*/.*/.*/.*/Caudate Body";
    if(pfc_out->brain_area.empty()) {
      pfc_out->brain_area = ".*/.*/.*/.*/BA44";
    }
    if(pfc_out_d && pfc_mnt_d->brain_area.empty()) {
    	pfc_out_d->brain_area = ".*/.*/.*/.*/BA44";
    }
  }

  int lay_spc = 2;

  int n_lv_u;           // number of pvlv-type units
  // if(lvesp->scalar.rep == ScalarValSpec::LOCALIST)
    n_lv_u = 4;
  // else if(lvesp->scalar.rep == ScalarValSpec::GAUSSIAN)
  //   n_lv_u = 12;
  // else
  //   n_lv_u = 21;


  if(new_pbwm_laygp) {
    pbwm_laygp_go->pos.z = 0;
    pbwm_laygp_go->pos.x = 20;
    pbwm_laygp_go->pos.y = 0;
    pbwm_laygp_nogo->pos.z = 0;
    pbwm_laygp_nogo->pos.x = 20;
    pbwm_laygp_nogo->pos.y = 50;
    pbwm_laygp_pfc->pos.z = 1;
    pbwm_laygp_pfc->pos.x = 20;
    pbwm_laygp_pfc->pos.y = 0;
  }

  ///////////////	Matrix Layout

  int mtx_st_x = 0;
  int mtx_st_y = 0;
  int mtx_nogo_y = mtx_st_y + matrix_go_mnt->disp_geom.y + 3 * lay_spc;
  int mtx_go_y = 3 * lay_spc;
  int mtx_z = 0;
  int mtx_x_sz = 7;
  int mtx_y_sz = 4;
  int mtx_n = mtx_x_sz * mtx_y_sz;

  if(in_stripes > 0) {
    if(matrix_new) {
      matrix_go_in->pos.SetXYZ(mtx_st_x, mtx_st_y + mtx_go_y, mtx_z);
      matrix_nogo_in->pos.SetXYZ(mtx_st_x, mtx_st_y, mtx_z);

      matrix_go_in->un_geom.n = mtx_n; matrix_go_in->un_geom.x = mtx_x_sz;
      matrix_go_in->un_geom.y = mtx_y_sz;
      matrix_nogo_in->un_geom.n = mtx_n; matrix_nogo_in->un_geom.x = mtx_x_sz;
      matrix_nogo_in->un_geom.y = mtx_y_sz;
    }
    lay_set_geom(matrix_go_in, in_stripes);
    lay_set_geom(matrix_nogo_in, in_stripes);

    mtx_nogo_y = MAX(mtx_nogo_y, mtx_st_y + matrix_go_in->disp_geom.y + 3 * lay_spc);
    mtx_st_x += matrix_go_in->disp_geom.x + lay_spc; // move over..
  }
  if(mnt_stripes > 0) {
    if(matrix_new) {
      matrix_go_mnt->pos.SetXYZ(mtx_st_x, mtx_st_y + mtx_go_y, mtx_z);
      matrix_nogo_mnt->pos.SetXYZ(mtx_st_x, mtx_st_y, mtx_z);

      matrix_go_mnt->un_geom.n = mtx_n; matrix_go_mnt->un_geom.x = mtx_x_sz;
      matrix_go_mnt->un_geom.y = mtx_y_sz;
      matrix_nogo_mnt->un_geom.n = mtx_n; matrix_nogo_mnt->un_geom.x = mtx_x_sz;
      matrix_nogo_mnt->un_geom.y = mtx_y_sz;
    }
    lay_set_geom(matrix_go_mnt, mnt_stripes);
    lay_set_geom(matrix_nogo_mnt, mnt_stripes);

    mtx_nogo_y = MAX(mtx_nogo_y, mtx_st_y + matrix_go_mnt->disp_geom.y + 3 * lay_spc);
    mtx_st_x += matrix_go_mnt->disp_geom.x + lay_spc; // move over..
  }
  if(out_stripes > 0) {
    if(matrix_new) {
      matrix_go_out->pos.SetXYZ(mtx_st_x, mtx_st_y + mtx_go_y, mtx_z);
      matrix_nogo_out->pos.SetXYZ(mtx_st_x, mtx_st_y, mtx_z);

      matrix_go_out->un_geom.n = mtx_n; matrix_go_out->un_geom.x = mtx_x_sz;
      matrix_go_out->un_geom.y = mtx_y_sz;
      matrix_nogo_out->un_geom.n = mtx_n; matrix_nogo_out->un_geom.x = mtx_x_sz;
      matrix_nogo_out->un_geom.y = mtx_y_sz;
    }
    lay_set_geom(matrix_go_out, out_stripes);
    lay_set_geom(matrix_nogo_out, out_stripes);

    mtx_nogo_y = MAX(mtx_nogo_y, mtx_st_y + matrix_go_out->disp_geom.y + 3 * lay_spc);
    mtx_st_x += matrix_go_out->disp_geom.x + lay_spc; // move over..
  }

  if(new_pbwm_laygp) {
    pbwm_laygp_nogo->pos.y = mtx_nogo_y; // move over!
  }

  ///////////////	PFC Layout first -- get into z = 1

  int pfcu_n = 49; int pfcu_x = 7; int pfcu_y = 7;
  int pfc_st_x = 0;
  int pfc_st_y = 0;
  int pfc_z = 0;
  if(in_stripes > 0) {
    if(pfc_new) {
      pfc_in->pos.SetXYZ(pfc_st_x, pfc_st_y, pfc_z);
      pfc_in->un_geom.SetXYN(pfcu_x, pfcu_y, pfcu_n);
    }
    lay_set_geom(pfc_in, in_stripes);

    if(pfc_in_d) {
      if(pfcd_new) {
        pfc_in_d->pos.SetXYZ(pfc_st_x, pfc_st_y + pfc_in->disp_geom.y + lay_spc, pfc_z);
        pfc_in_d->un_geom.SetXYN(pfcu_x, pfcu_y, pfcu_n);
      }
      lay_set_geom(pfc_in_d, in_stripes);
    }
    pfc_st_x += pfc_in->disp_geom.x + lay_spc; // move starting x over for next type
  }

  if(mnt_stripes > 0) {
    if(pfc_new) {
      pfc_mnt->pos.SetXYZ(pfc_st_x, pfc_st_y, pfc_z);
      pfc_mnt->un_geom.SetXYN(pfcu_x, pfcu_y, pfcu_n);
    }
    lay_set_geom(pfc_mnt, mnt_stripes);

    if(pfc_mnt_d) {
      if(pfcd_new) {
        pfc_mnt_d->pos.SetXYZ(pfc_st_x, pfc_st_y + pfc_mnt->disp_geom.y + lay_spc, pfc_z);
        pfc_mnt_d->un_geom.SetXYN(pfcu_x, pfcu_y, pfcu_n);
      }
      lay_set_geom(pfc_mnt_d, mnt_stripes);
    }
    pfc_st_x += pfc_mnt->disp_geom.x + lay_spc;
  }

  if(out_stripes > 0) {
    if(pfc_new) {
      pfc_out->pos.SetXYZ(pfc_st_x, pfc_st_y, pfc_z);
      pfc_out->un_geom.SetXYN(pfcu_x, pfcu_y, pfcu_n);
    }
    lay_set_geom(pfc_out, out_stripes);
  }
  if(pfc_out_d) {
    if(pfcd_new) {
      pfc_out_d->pos.SetXYZ(pfc_st_x, pfc_st_y + pfc_out->disp_geom.y + lay_spc, pfc_z);
      pfc_out_d->un_geom.SetXYN(pfcu_x, pfcu_y, pfcu_n);
    }
    lay_set_geom(pfc_out_d, out_stripes);
  }
  pfc_st_x += pfc_out->disp_geom.x + lay_spc;

  ///////////////	Now SNrThal

  int snr_stripes = in_stripes + mnt_stripes;
  if(snrthal_out) {
    lay_set_geom(snrthal_out, out_stripes, 1);
  }
  else {
    snr_stripes += out_stripes;
  }
  lay_set_geom(snrthal, snr_stripes, 1);

  if(snrthal_new) { // put at front of go
    snrthal->pos.SetXYZ(0, 0, mtx_z);
  }
  if(snrthal_out && snrthal_out_new) {
    snrthal_out->pos.SetXYZ(snr_stripes*2 + lay_spc, 0, mtx_z);
  }

  // here to allow it to get disp_geom for laying out the pfc and matrix guys!
  PBWM_SetNStripes(net, in_stripes, mnt_stripes, out_stripes, one_snr);

  if(new_pbwm_laygp) {
    pbwm_laygp_go->pos.z = 0;
    pbwm_laygp_go->pos.x = 20;
    pbwm_laygp_go->pos.y = 0;
    pbwm_laygp_nogo->pos.z = 0;
    pbwm_laygp_nogo->pos.x = 20;
    pbwm_laygp_nogo->pos.y = mtx_nogo_y;
    pbwm_laygp_pfc->pos.z = 1;
    pbwm_laygp_pfc->pos.x = 20;
    pbwm_laygp_pfc->pos.y = 0;
  }

  //////////////////////////////////////////////////////////////////////////////////
  // build and check

  // Need to update act_denoms based on stripe counts

  PFCLayerSpec* pfc_in_sp = PbwmSp("PFC_in",PFCLayerSpec);
  PFCLayerSpec* pfc_mnt_sp = PbwmSp("PFC_mnt",PFCLayerSpec);
  PFCLayerSpec* pfc_out_sp = PbwmSp("PFC_out",PFCLayerSpec);

  if(in_stripes > 0)
    pfc_mnt_sp->unit_gp_inhib.act_denom = in_stripes;
  else if(mnt_stripes > 0)
    pfc_mnt_sp->unit_gp_inhib.act_denom = mnt_stripes;
  else if(out_stripes > 0)
    pfc_mnt_sp->unit_gp_inhib.act_denom = out_stripes;

  pfc_mnt_sp->UpdateAfterEdit(); // update
  
  pfc_out_sp->SetUnique("unit_gp_inhib",true);
  if(out_stripes > 0)
    pfc_out_sp->unit_gp_inhib.act_denom = out_stripes;
  else if(in_stripes > 0)
    pfc_out_sp->unit_gp_inhib.act_denom = in_stripes;
  else if(mnt_stripes > 0)
    pfc_out_sp->unit_gp_inhib.act_denom = mnt_stripes;
  // pfc_out_sp->unit_gp_inhib.gp_g = 0.8f; // special case
  pfc_out_sp->UpdateAfterEdit(); // update

  net->LayerPos_Cleanup();

  net->Build();			// rebuild after defaults in place

  // move back!
  if(new_pbwm_laygp) {
    pbwm_laygp_go->pos.z = 0;
    net->RebuildAllViews();     // trigger update
  }

  taMisc::CheckConfigStart(false, false);

  bool ok = false;
  ok = PbwmSp("SNrThalLayer",SNrThalLayerSpec)->CheckConfig_Layer(snrthal, false);
  if(in_stripes > 0) {
    ok &= pfc_in_sp->CheckConfig_Layer(pfc_in, false);
    ok &= PbwmSp("Matrix_Go_in",MatrixLayerSpec)->CheckConfig_Layer(matrix_go_in, false);
    ok &= PbwmSp("Matrix_NoGo_in",MatrixLayerSpec)->CheckConfig_Layer(matrix_nogo_in, false);
  }
  if(mnt_stripes > 0) {
    ok &= pfc_mnt_sp->CheckConfig_Layer(pfc_mnt, false);
    ok &= PbwmSp("Matrix_Go_mnt",MatrixLayerSpec)->CheckConfig_Layer(matrix_go_mnt, false);
    ok &= PbwmSp("Matrix_NoGo_mnt",MatrixLayerSpec)->CheckConfig_Layer(matrix_nogo_mnt, false);
  }
  if(out_stripes > 0) {
    ok &= pfc_out_sp->CheckConfig_Layer(pfc_out, false);
    ok &= PbwmSp("Matrix_Go_out",MatrixLayerSpec)->CheckConfig_Layer(matrix_go_out, false);
    ok &= PbwmSp("Matrix_NoGo_out",MatrixLayerSpec)->CheckConfig_Layer(matrix_nogo_out, false);
  }

  taMisc::CheckConfigEnd(ok);

  if(!ok) {
    msg =
      "BG/PFC: An error in the configuration has occurred (it should be the last message\
 you received prior to this one).  The network will not run until this is fixed.\
 In addition, the configuration process may not be complete, so you should run this\
 function again after you have corrected the source of the error.";
  }
  else {
    msg =
    "BG/PFC configuration is now complete.  Do not forget the one remaining thing\
 you need to do manually:\n\n" + man_msg;
  }
  taMisc::Confirm(msg);

  for(int j=0;j<net->specs.leaves;j++) {
    BaseSpec* sp = (BaseSpec*)net->specs.Leaf(j);
    sp->UpdateAfterEdit();
  }

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
  net->layers.gp.RemoveName(prefix + "_PFC");
  net->layers.gp.RemoveName(prefix + "_BG");
  net->layers.gp.RemoveName(prefix + "_Go");
  net->layers.gp.RemoveName(prefix + "_NoGo");
  net->layers.gp.RemoveName(prefix + "_PFC");
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
  hip_cons->SetUnique("lmix", true);
  hip_cons->lmix.hebb = 0.05f;
  
  // mossy mean = 0.9 var = 0.01, rel = 8 , lrate = 0
  mossy_cons->SetUnique("rnd", true);
  mossy_cons->rnd.mean = 0.9f;
  mossy_cons->rnd.var = 0.01f;
  mossy_cons->SetUnique("wt_scale", true);
  mossy_cons->wt_scale.rel = 8.0f;
  mossy_cons->SetUnique("lrate", true);
  mossy_cons->lrate = 0.0f;
  mossy_cons->SetUnique("savg_cor", true);
  mossy_cons->savg_cor.cor = 1.0f;
  
  // ca3_ca3 rel = 2, 
  ca3ca3_cons->SetUnique("wt_scale", true);
  ca3ca3_cons->wt_scale.rel = 2.0f;
  ca3ca3_cons->SetUnique("savg_cor", true);
  ca3ca3_cons->savg_cor.cor = 1.0f;

  // ca3_ca1 lrate = 0.05
  ca3ca1_cons->SetUnique("lrate", true);
  ca3ca1_cons->lrate = 0.05f;
  ca3ca1_cons->SetUnique("lmix", true);
  ca3ca1_cons->lmix.hebb = 0.005f;

  // sparse hippocampal layers!

  dg_laysp->SetUnique("kwta", true);
  dg_laysp->kwta.pct = 0.01f;

  ca3_laysp->SetUnique("kwta", true);
  ca3_laysp->kwta.pct = 0.02f;

  ca1_laysp->SetUnique("inhib_group", true);
  ca1_laysp->inhib_group = LeabraLayerSpec::UNIT_GROUPS;
  ca1_laysp->SetUnique("gp_kwta", true);
  ca1_laysp->gp_kwta.pct = 0.1f;
 

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


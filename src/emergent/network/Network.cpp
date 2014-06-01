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

#include "Network.h"
#include <Network_Group>
#include <NetView>
#include <NetworkVoxelMapper>
#include <BrainView>
#include <MemberDef>
#include <UserDataItem>
#include <ProjectBase>
#include <ControlPanel>
#include <DataTable>
#include <taFiler>
#include <SimpleMathSpec>
#include <NetMonitor>
#include <taMath_float>

#include <tabMisc>
#include <taMisc>

eTypeDef_Of(FullPrjnSpec);
eTypeDef_Of(CustomPrjnSpec);

#include <sstream>

TA_BASEFUNS_CTORS_DEFN(Network);

using namespace std;


taBrainAtlas_List* Network::brain_atlases = NULL;

void Network::Initialize() {
  specs.SetBaseType(&TA_BaseSpec);
  layers.SetBaseType(&TA_Layer);

  if(!brain_atlases)
    brain_atlases = &Network_Group::brain_atlases;

  flags = NF_NONE;
  auto_build = AUTO_BUILD;

  train_mode = TRAIN;
  wt_update = ON_LINE;
  small_batch_n = 10;
  small_batch_n_eff = 10;

  batch = 0;
  epoch = 0;
  group = 0;
  trial = 0;
  tick = 0;
  cycle = 0;
  time = 0.0f;

  // group_name = ??
  // trial_name = ??
  // output_name = ??

  sse_unit_avg = false;
  sse_sqrt = false;
  sse = 0.0f;
  sum_sse = 0.0f;
  avg_sse = 0.0f;
  cnt_err = 0.0f;
  pct_err = 0.0f;
  pct_cor = 0.0f;

  cnt_err_tol = 0.0f;

  cur_sum_sse = 0.0f;
  avg_sse_n = 0;
  cur_cnt_err = 0.0f;

  compute_prerr = false;

  // prerr = ??
  // sum_prerr = ??
  // epc_prerr = ??
  // train_time = ??
  // epoch_time = ??
  // group_time = ??
  // trial_time = ??
  // settle_time = ??
  // cycle_time = ??
  // wt_sync_time = ??
  // misc_time = ??
  // threads = ??
  // units_flat = ??
  // send_netin_tmp = ??

  dmem_sync_level = DMEM_SYNC_NETWORK;
  dmem_nprocs = 1;
  dmem_nprocs_actual = MIN(dmem_nprocs, taMisc::dmem_nprocs);
  usr1_save_fmt = FULL_NET;
  wt_save_fmt = TEXT;

  n_units = 0;
  n_cons = 0;
  max_prjns = 1;
  max_disp_size.x = 1;
  max_disp_size.y = 1;
  max_disp_size.z = 1;

  max_disp_size2d.x = 1;
  max_disp_size2d.y = 1;

  proj = NULL;
  old_load_cons = false;

  null_unit = NULL;

#ifdef DMEM_COMPILE
  // dmem_net_comm = ??
  // dmem_trl_comm = ??
  dmem_share_units.comm = (MPI_Comm)MPI_COMM_WORLD;
  dmem_agg_sum.agg_op = MPI_SUM;
#endif
}

void Network::Destroy() {
  CutLinks();
}

void Network::InitLinks() {
  GetSigLink(); // forces creation, so we track Updates
  proj = GET_MY_OWNER(ProjectBase);
  taBase::Own(specs, this);
  taBase::Own(layers, this);
  taBase::Own(weights, this);
  taBase::Own(max_disp_size, this);
  taBase::Own(max_disp_size2d, this);

  taBase::Own(prerr, this);
  taBase::Own(sum_prerr, this);
  taBase::Own(epc_prerr, this);

  taBase::Own(train_time, this);  //train_time.name = "train_time";
  taBase::Own(epoch_time, this);  //epoch_time.name = "epoch_time";
  taBase::Own(group_time, this);  //group_time.name = "group_time";
  taBase::Own(trial_time, this);  //trial_time.name = "trial_time";
  taBase::Own(settle_time, this);  //settle_time.name = "settle_time";
  taBase::Own(cycle_time, this);  //cycle_time.name = "cycle_time";
  taBase::Own(wt_sync_time, this); //wt_sync_time.name = "wt_sync_time";
  taBase::Own(misc_time, this);  //misc_time.name = "misc_time";

  taBase::Own(send_netin_tmp, this);
  taBase::Own(threads, this);

  brain_atlas = brain_atlases->FindNameContains("Talairach"); // default
  
#ifdef DMEM_COMPILE
  taBase::Own(dmem_net_comm, this);
  taBase::Own(dmem_trl_comm, this);
  taBase::Own(dmem_share_units, this);
  taBase::Own(dmem_agg_sum, this);
  DMem_InitAggs();
#endif

  NetTextUserData();

  inherited::InitLinks();
}

void Network::CutLinks() {
  if(!owner) return; // already replacing or already dead
#ifdef DMEM_COMPILE
  dmem_net_comm.FreeComm();
  dmem_trl_comm.FreeComm();
#endif
  units_flat.Reset();
  send_netin_tmp.CutLinks();
  threads.CutLinks();
  RemoveCons();                 // do this first in optimized way!
  RemoveUnitGroups();           // then units
  misc_time.CutLinks();
  wt_sync_time.CutLinks();
  cycle_time.CutLinks();
  settle_time.CutLinks();
  trial_time.CutLinks();
  group_time.CutLinks();
  epoch_time.CutLinks();
  train_time.CutLinks();
  max_disp_size.CutLinks();
  max_disp_size2d.CutLinks();
  layers.CutLinks();            // then std kills
  specs.CutLinks();
  proj = NULL;
  taBase::DelPointer((taBase**)&null_unit);
  inherited::CutLinks();
}

void Network::Copy_(const Network& cp) {
  specs = cp.specs;
  layers = cp.layers;

  auto_build = cp.auto_build;

  train_mode = cp.train_mode;
  wt_update = cp.wt_update;
  small_batch_n = cp.small_batch_n;
  small_batch_n_eff = cp.small_batch_n_eff;

  batch = cp.batch;
  epoch = cp.epoch;
  group = cp.group;
  trial = cp.trial;
  tick = cp.tick;
  cycle = cp.cycle;
  time = cp.time;
  group_name = cp.group_name;
  trial_name = cp.trial_name;
  output_name = cp.output_name;

  sse_unit_avg = cp.sse_unit_avg;
  sse_sqrt = cp.sse_sqrt;
  sse = cp.sse;
  sum_sse = cp.sum_sse;
  avg_sse = cp.avg_sse;
  cnt_err_tol = cp.cnt_err_tol;
  cnt_err = cp.cnt_err;
  pct_err = cp.pct_err;
  pct_cor = cp.pct_cor;

  cur_sum_sse = cp.cur_sum_sse;
  avg_sse_n = cp.avg_sse_n;
  cur_cnt_err = cp.cur_cnt_err;

  compute_prerr = cp.compute_prerr;
  prerr = cp.prerr;
  sum_prerr = cp.sum_prerr;
  epc_prerr = cp.epc_prerr;

  dmem_sync_level = cp.dmem_sync_level;
  dmem_nprocs = cp.dmem_nprocs;
  dmem_nprocs_actual = cp.dmem_nprocs_actual;

  usr1_save_fmt = cp.usr1_save_fmt;
  wt_save_fmt = cp.wt_save_fmt;

  max_disp_size = cp.max_disp_size;
  max_disp_size2d = cp.max_disp_size2d;

  UpdatePointers_NewPar((taBase*)&cp, this); // update all the pointers
  SyncSendPrjns();
  FixPrjnIndexes();                          // fix the recv_idx and send_idx (not copied!)
  UpdateAllSpecs();
  BuildUnits_Threads();
#ifdef DMEM_COMPILE
  DMem_DistributeUnits();
#endif
//   ((Network&)cp).SyncSendPrjns(); // these get screwed up in there somewhere..
  //note: batch update in tabase copy
}

void Network::UpdateAfterEdit_impl(){
  inherited::UpdateAfterEdit_impl();
  if(wt_save_fmt == NET_FMT)
    wt_save_fmt = TEXT;

//  if(name.empty()) {
//    taMisc::Error("name should not be empty -- this indicates the dreaded null dialog bug!!  triggering div zero now -- please report the trace to Randy!");
//    int zero = 0;
//    String msg = String(1 / zero); // trigger error
//  }

  if(taMisc::is_loading) {
    brain_atlas = brain_atlases->FindName(brain_atlas_name);
  }
  else {
    if(brain_atlas)
      brain_atlas_name = brain_atlas->name; // for later saving..
  }

  ClearNetFlag(SAVE_UNITS_FORCE); // might have been saved in on state from recover file or something!
}

void Network::UpdtAfterNetMod() {
  //  SyncSendPrjns();
  CountRecvCons();
  BuildUnits_Threads();
  small_batch_n_eff = small_batch_n;
  if(small_batch_n_eff < 1) small_batch_n_eff = 1;
#ifdef DMEM_COMPILE
  DMem_SyncNRecvCons();
  DMem_UpdtWtUpdt();
#endif
}

void Network::SetProjectionDefaultTypes(Projection* prjn) {
  // noop for base case: algorithms must override!
  prjn->spec.type = &TA_FullPrjnSpec;
}

bool Network::ChangeMyType(TypeDef* new_typ) {
  if(TestError(n_units > 0, "ChangeMyType", "You must first remove all units in the network before changing its type -- otherwise it takes FOREVER -- do Network/Structure/Remove Units"))
    return false;
  return inherited::ChangeMyType(new_typ);
}

int Network::Dump_Load_Value(istream& strm, taBase* par) {
  old_load_cons = false;
  int rval = inherited::Dump_Load_Value(strm, par);

  if(old_load_cons) { // old dump format
    Connect();                    // needs an explicit connect to make everything
    FOREACH_ELEM_IN_GROUP(Layer, lay, layers) {
      FOREACH_ELEM_IN_GROUP(Unit, u, lay->units) {
        for(int g=0; g<u->recv.size; g++) {
          RecvCons* cg = u->recv.FastEl(g);
          cg->Dump_Load_Old_Cons(u, g);
        }
      }
    }
    old_load_cons = false;
  }

  ClearNetFlag(SAVE_UNITS_FORCE);       // might have been saved in on state from recover file or something!

  BuildUnits_Threads();
#ifdef DMEM_COMPILE
  DMem_DistributeUnits();
  DMem_PruneNonLocalCons();
  DMem_UpdtWtUpdt();
#endif

  return rval;
}

int Network::Dump_Save_impl(ostream& strm, taBase* par, int indent) {
  if(taMisc::is_undo_saving) {
    if(!tabMisc::cur_undo_save_owner || !IsChildOf(tabMisc::cur_undo_save_owner)) {
      // no need to save b/c unaffected by changes elsewhere..
      if(!tabMisc::cur_undo_mod_obj || !tabMisc::cur_undo_mod_obj->IsChildOf(this)) {
        // affected object is not under us
	if(taMisc::undo_debug) {
	  taMisc::Info("NOT saving for Undo network named:", name);
	}
	return 1;
      }
    }
    if(taMisc::undo_debug) {
      taMisc::Info("YES saving for Undo network named:", name);
    }
  }
  return inherited::Dump_Save_impl(strm, par, indent);
}

int Network::Save_strm(ostream& strm, taBase* par, int indent) {
  if(!taMisc::is_undo_saving)     // don't do it for undo!!
    SetNetFlag(SAVE_UNITS_FORCE); // override if !SAVE_UNITS
  int rval = inherited::Save_strm(strm, par, indent);
  ClearNetFlag(SAVE_UNITS_FORCE);
  return rval;
}

void Network::Build() {
  taMisc::Busy();
  ++taMisc::no_auto_expand; // c'mon...!!! ;)
  StructUpdate(true);
  CheckSpecs();
  BuildLayers(); // note: for Area constructs
  BuildUnits();
  BuildPrjns(); // note: for Area constructs
  Connect();
  if(taMisc::gui_active)	// only when gui is active..
    AssignVoxels();
  StructUpdate(false);
//   if (net_inst.ptr()) {
//     net_inst->OnBuild();
//   }
  --taMisc::no_auto_expand;
  taMisc::DoneBusy();
}

void Network::CheckSpecs() {
  FOREACH_ELEM_IN_GROUP(Layer, l, layers)
    l->CheckSpecs();
}

void Network::BuildLayers() {
  taMisc::Busy();
  ++taMisc::no_auto_expand;
  StructUpdate(true);
  LayerPos_Cleanup();
  layers.BuildLayers(); // recurses
  StructUpdate(false);
  --taMisc::no_auto_expand;
  taMisc::DoneBusy();
  if(!taMisc::gui_active)    return;
}

void Network::BuildUnits() {
  taMisc::Busy();
  StructUpdate(true);
  BuildNullUnit();
  FOREACH_ELEM_IN_GROUP(Layer, l, layers) {
    if(l->lesioned()) continue;
    l->BuildUnits();
  }
  StructUpdate(false);
  taMisc::DoneBusy();
#ifdef DMEM_COMPILE
  DMem_DistributeUnits();
#endif
  UpdtAfterNetMod();            // calls BuildUnits_Threads
  if(!taMisc::gui_active)    return;
}

void Network::BuildNullUnit() {
  // in derived classes, just replace the unit type with appropriate one -- do NOT call
  // inherited!
  if(!null_unit) {
    taBase::OwnPointer((taBase**)&null_unit, new Unit, this);
  }
}

void Network::BuildUnits_Threads() {
  threads.InitAll();
  units_flat.Reset();
  // real indexes start at 1, to allow 0 to be a dummy case for inactive units that may
  // nevertheless get a send netin call to them -- all those just go to this 0 bin
  BuildNullUnit();              // double sure
  units_flat.Add(null_unit);         // add a dummy null
  FOREACH_ELEM_IN_GROUP(Layer, l, layers) {
    if(l->lesioned()) {
      l->units_flat_idx = 0;
      continue; // don't even add units from lesioned layers!!
    }
    l->BuildUnits_Threads(this);
  }
  // temporary storage for sender-based netinput computation
  if(units_flat.size > 0 && threads.n_threads > 0) {
    if(NetinPerPrjn()) {
      send_netin_tmp.SetGeom(3, units_flat.size, max_prjns, threads.n_threads);
    }
    else {
      send_netin_tmp.SetGeom(2, units_flat.size, threads.n_threads);
    }
    send_netin_tmp.InitVals(0.0f);
  }
}

void Network::BuildPrjns() {
  taMisc::Busy();
  ++taMisc::no_auto_expand;
  StructUpdate(true);
  layers.BuildPrjns(); // recurses
  StructUpdate(false);
  taMisc::DoneBusy();
  --taMisc::no_auto_expand;
  if(!taMisc::gui_active)    return;
}

void Network::Connect() {
  taMisc::Busy();
  ++taMisc::no_auto_expand; // c'mon...!!! ;)
  StructUpdate(true);
  CheckSpecs();
  RemoveCons();
  SyncSendPrjns();
  // go in reverse order so that symmetric prjns can be made in
  // response to receiver-based projections
  Layer* l;
  int i;
  for(i=layers.leaves-1;i>=0;i--) {
    l = (Layer*)layers.Leaf(i);
    if(l->lesioned()) continue;
    l->Connect();
  }
  UpdtAfterNetMod();
  StructUpdate(false);
  --taMisc::no_auto_expand;
  taMisc::DoneBusy();
}

void Network::CountRecvCons() {
  n_units = 0;
  n_cons = 0;
  max_prjns = 1;
  FOREACH_ELEM_IN_GROUP(Layer, l, layers) {
    n_cons += l->CountRecvCons();
    n_units += l->units.leaves;
    max_prjns = MAX(l->projections.size, max_prjns);
  }
}

bool Network::CheckBuild(bool quiet) {
  FOREACH_ELEM_IN_GROUP(Layer, l, layers) {
    if(l->lesioned()) continue;
    if(!l->CheckBuild(quiet)) {
      if(!quiet)
        taMisc::CheckError("Network:",GetName(),"Needs the 'Build' command to be run");
      return false;
    }
  }
  return true;
}

bool Network::CheckConnect(bool quiet) {
  FOREACH_ELEM_IN_GROUP(Layer, l, layers) {
    if(l->lesioned()) continue;
    if(!l->CheckConnect(quiet)) {
      if(!quiet)
        taMisc::CheckError("Network:",GetName(), "Needs the 'Connect' command to be run");
      return false;
    }
  }
  return true;
}

void Network::CheckThisConfig_impl(bool quiet, bool& rval) {
  //NOTE: slightly non-standard, because we bail on first detected issue
  if (!CheckBuild(quiet)) { rval = false; return; }
  if (!CheckConnect(quiet)) { rval = false; return; }
  UpdtAfterNetMod();            // just to be sure..
  inherited::CheckThisConfig_impl(quiet, rval);
}

void Network::CheckChildConfig_impl(bool quiet, bool& rval) {
  inherited::CheckChildConfig_impl(quiet, rval);
  specs.CheckConfig(quiet, rval); //note: this checks the specs themselves, not objs
  layers.CheckConfig(quiet, rval);
}

void Network::SetUnitType(TypeDef* td) {
  FOREACH_ELEM_IN_GROUP(Layer, l, layers) {
    l->SetUnitType(td);
  }
}

void Network::SyncSendPrjns() {
  FOREACH_ELEM_IN_GROUP(Layer, l, layers) {
    l->SyncSendPrjns();
  }
}

void Network::FixPrjnIndexes() {
  FOREACH_ELEM_IN_GROUP(Layer, l, layers) {
    l->FixPrjnIndexes();
  }
}

void Network::ConnectUnits(Unit* u_to, Unit* u_from, bool record, ConSpec* conspec) {
  if(u_to == NULL) return; // must have reciever
  if(u_from == NULL)    u_from = u_to; // assume self con if no from

  Layer* lay = GET_OWNER(u_to,Layer);
  Layer* l_from = GET_OWNER(u_from,Layer);
  // check to see if a pjrn already exists
  FOREACH_ELEM_IN_GROUP(Projection, pjn, lay->projections) {
    if((pjn->from.ptr() == l_from) &&
       (pjn->spec->InheritsFrom(&TA_CustomPrjnSpec)) &&
       ((conspec == NULL) || (pjn->con_spec == conspec)))
    {
      u_to->ConnectFromCk(u_from,pjn);
      if (record)
        taMisc::RecordScript(u_to->GetPath() + ".ConnectFromCk(" + u_from->GetPath() +
                                                ", " + pjn->GetPath() + ");\n");
      lay->UpdateAfterEdit();
      return;
    }
  }

  // no such projection found
  Projection* pjn = 0;
#ifdef DMEM_COMPILE
  if(record && (taMisc::dmem_nprocs == 1)) { // don't actually run under gui in dmem mode
#endif
    pjn = (Projection*) lay->projections.New(1);
    pjn->SetCustomFrom(l_from);
    pjn->spec.type = &TA_CustomPrjnSpec;
    if(conspec)
      pjn->con_spec.SetSpec(conspec);
    pjn->spec.UpdateAfterEdit();
    pjn->projected = true;
    pjn->UpdateAfterEdit();
#ifdef DMEM_COMPILE
  }
#endif
  if (record && pjn) {
    taMisc::RecordScript(lay->projections.GetPath() + ".NewEl(1);\n");
    taMisc::SREAssignment(pjn,pjn->FindMember("from_type"));
    taMisc::ScriptRecordAssignment(pjn,pjn->FindMember("from"));
    taMisc::RecordScript(pjn->GetPath() + ".spec.type = CustomPrjnSpec;");
  }
}

#ifdef TA_GUI
NetView* Network::NewView(T3Panel* fr) {
  return NetView::New(this, fr);
}
#endif

// Create an fMRI-style brain visualization to show activations
// in defined brain areas.  If no frame is specified, a new one
// will be created.
BrainView* Network::NewBrainView(T3Panel* fr) {
  // Assign voxel coordinates to the network's units.
  NetworkVoxelMapper nvm(this);
  nvm.AssignVoxels();

  // Create the new BrainView.
  return BrainView::New(this, fr);
}

void Network::AssignVoxels() {
  // Assign voxel coordinates to the network's units.
  NetworkVoxelMapper nvm(this);
  nvm.AssignVoxels();
}

// PlaceNetText is in netstru_qtso.cpp

void Network::NetTextUserData() {
  TypeDef* td = GetTypeDef();
  Variant bool_on_val;
  bool_on_val.setBool(true);
  Variant bool_off_val;
  bool_off_val.setBool(true);
  for(int i=td->members.size-1; i>=0; i--) {
    MemberDef* md = td->members[i];
    if(!md->HasOption("VIEW")) continue;
    UserDataItem* ud = (UserDataItem*)GetUserDataItem(md->name);
    if(!ud)
      ud = SetUserData(md->name, bool_on_val);
    ud->val_type_fixed = true;
  }
}

Layer* Network::NewLayer() {
  return layers.NewEl(1);
}

void Network::RemoveUnits() {
  taMisc::Busy();
  StructUpdate(true);
  //  RemoveMonitors(); // not needed with smartref!
  FOREACH_ELEM_IN_GROUP(Layer, l, layers)
    l->RemoveUnits();
  n_cons = 0;
  StructUpdate(false);
  taMisc::DoneBusy();
}

void Network::RemoveUnitGroups() {
  taMisc::Busy();
  StructUpdate(true);
  // RemoveMonitors(); // not needed with smartref!
  FOREACH_ELEM_IN_GROUP(Layer, l, layers)
    l->RemoveUnitGroups();
  StructUpdate(false);
  taMisc::DoneBusy();
}

void Network::RemoveCons() {
  taMisc::Busy();
  StructUpdate(true);
  //  RemoveMonitors(); // not needed with smartref!
  FOREACH_ELEM_IN_GROUP(Layer, l, layers)
    l->RemoveCons_Net();
  StructUpdate(false);
  taMisc::DoneBusy();
}

void Network::Init_InputData(){
  FOREACH_ELEM_IN_GROUP(Layer, l, layers) {
    if(!l->lesioned())
      l->Init_InputData(this);
  }
}

void Network::Init_Acts(){
  send_netin_tmp.InitVals(0.0f);
  FOREACH_ELEM_IN_GROUP(Layer, l, layers) {
    if(!l->lesioned())
      l->Init_Acts(this);
  }
}

void Network::Init_dWt(){
  FOREACH_ELEM_IN_GROUP(Layer, l, layers) {
    if(!l->lesioned())
      l->Init_dWt(this);
  }
}

void Network::Init_Weights() {
  // do lots of checking here to make sure, cuz often 1st thing that happens
  //NOTE: this will typically be nested inside a gui check
  if (!CheckConfig(false)) return;

  taMisc::Busy();
  FOREACH_ELEM_IN_GROUP(Layer, l, layers) {
    if(!l->lesioned())
      l->Init_Weights(this);
  }
  Init_Weights_post();          // done after all initialization (for scaling wts...)

#ifdef DMEM_COMPILE
  // do the dmem weight symmetrizing!
  DMem_SymmetrizeWts();
#endif

  Init_Acts();                  // also re-init state at this point..
  Init_Metrics();
  UpdateAllViews();
  taMisc::DoneBusy();
}

void Network::Init_Weights_post() {
  FOREACH_ELEM_IN_GROUP(Layer, l, layers) {
    if(!l->lesioned())
      l->Init_Weights_post(this);
  }
}

void Network::Init_Metrics() {
  Init_Counters();
  Init_Stats();
  Init_Timers();
}

void Network::Init_Counters() {
  // this is one you do not reinit: loops over inits!
//   batch = 0;
  epoch = 0;
  group = 0;
  group_name = "";
  trial = 0;
  trial_name = "";
  tick = 0;
  cycle = 0;
  time = 0.0f;
}

void Network::Init_Stats() {
  sse = 0.0f;
  sum_sse = 0.0f;
  avg_sse = 0.0f;
  cnt_err = 0.0f;
  pct_err = 0.0f;
  pct_cor = 0.0f;

  output_name = "";

  cur_sum_sse = 0.0f;
  avg_sse_n = 0;
  cur_cnt_err = 0.0f;
}

void Network::Init_Timers() {
  train_time.ResetUsed();
  epoch_time.ResetUsed();
  trial_time.ResetUsed();
  settle_time.ResetUsed();
  cycle_time.ResetUsed();
  wt_sync_time.ResetUsed();
  misc_time.ResetUsed();
}

// NOTE on compute load levels for thread.Run function (2nd arg) --
// any function that goes over all the cons gets a 1.0 (netin, dwt, wts)
// others (Compute_Act) just have small guess value, which should be replaced

void Network::Compute_Netin() {
  ThreadUnitCall un_call(&Unit::Compute_Netin);
  threads.Run(&un_call, 1.0f);

#ifdef DMEM_COMPILE
  DMem_SyncNet();
#endif
}

void Network::Send_Netin() {
  ThreadUnitCall un_call(&Unit::Send_Netin);
  threads.Run(&un_call, 1.0f);

  // now need to roll up the netinput into unit vals
  const int nu = units_flat.size;
  const int nt = threads.tasks.size;
  if(NetinPerPrjn()) {
    if(threads.using_threads) {
      for(int i=1;i<nu;i++) {     // 0 = dummy idx
        Unit* un = units_flat[i];
        float nw_nt = 0.0f;
        for(int p=0;p<un->recv.size;p++) {
          for(int j=0;j<nt;j++) {
            nw_nt += send_netin_tmp.FastEl3d(i, p, j);
          }
        }
        un->Compute_SentNetin(this, nw_nt);
      }
    }
    else {
      for(int i=1;i<nu;i++) {     // 0 = dummy idx
        Unit* un = units_flat[i];
        float nw_nt = 0.0f;
        for(int p=0;p<un->recv.size;p++) {
          nw_nt += send_netin_tmp.FastEl3d(i, p, 0); // use 0 thread
        }
        un->Compute_SentNetin(this, nw_nt);
      }
    }
  }
  else {
    if(threads.using_threads) {
      for(int i=1;i<nu;i++) {     // 0 = dummy idx
        Unit* un = units_flat[i];
        float nw_nt = 0.0f;
        for(int j=0;j<nt;j++) {
          nw_nt += send_netin_tmp.FastEl2d(i, j);
        }
        un->Compute_SentNetin(this, nw_nt);
      }
    }
    else {
      for(int i=1;i<nu;i++) {     // 0 = dummy idx
        Unit* un = units_flat[i];
        float nw_nt = send_netin_tmp.FastEl2d(i, 0); // use 0 thread
        un->Compute_SentNetin(this, nw_nt);
      }
    }
  }
  send_netin_tmp.InitVals(0.0f); // reset for next time around

#ifdef DMEM_COMPILE
  DMem_SyncNet();
#endif
}

void Network::Compute_Act() {
  ThreadUnitCall un_call(&Unit::Compute_Act);
  threads.Run(&un_call, .2f);
}

void Network::Compute_NetinAct() {
  // important note: any algorithms using this for feedforward computation are not
  // compatible with dmem computation on the network level (over connections)
  // because otherwise the netinput needs to be sync'd at the layer level prior to calling
  // the activation function at the layer level.  Threading should be much faster than
  // dmem in general so this takes precidence.  See BpNetwork::UpdateAfterEdit_impl for
  // a warning message that should be included.
  ThreadUnitCall un_call(&Unit::Compute_NetinAct);
  threads.Run(&un_call, 1.0f, false, true); // backwards = false, layer_sync=true
}

void Network::Compute_dWt() {
  ThreadUnitCall un_call(&Unit::Compute_dWt);
  threads.Run(&un_call, 1.0f);
}

bool Network::Compute_Weights_Test(int trial_no) {
  if(train_mode == TEST) return false;
  if(wt_update == ON_LINE) return true;
  if(wt_update == BATCH) return false;
  if(wt_update == SMALL_BATCH) {
    int trial_no_eff = trial_no;
#ifdef DMEM_COMPILE
    if(dmem_trl_comm.nprocs > 1) {
      trial_no_eff = ((trial_no_eff-1) / dmem_trl_comm.nprocs) + 1;
      // subtract the 1 that was presumably added to trial_no, then add it back
    }
#endif
    return (trial_no_eff % small_batch_n_eff == 0);
  }
  return false;
}

void Network::Compute_Weights() {
#ifdef DMEM_COMPILE
  DMem_SumDWts(dmem_trl_comm.comm);
#endif
  Compute_Weights_impl();
}

void Network::Compute_Weights_impl() {
  ThreadUnitCall un_call(&Unit::Compute_Weights);
  threads.Run(&un_call, 1.0f);
}

void Network::Compute_SSE(bool unit_avg, bool sqrt) {
  sse = 0.0f;
  int n_vals = 0;
  int lay_vals = 0;
  FOREACH_ELEM_IN_GROUP(Layer, l, layers) {
    if(l->lesioned()) continue;
    sse += l->Compute_SSE(this, lay_vals, unit_avg, sqrt);
    n_vals += lay_vals;
  }
  if(unit_avg && n_vals > 0)
    sse /= (float)n_vals;
  if(sqrt)
    sse = sqrtf(sse);
  cur_sum_sse += sse;
  avg_sse_n++;
  if(sse > cnt_err_tol)
    cur_cnt_err += 1.0;
}

void Network::Compute_PRerr() {
  prerr.InitVals();
  int n_vals = 0;
  FOREACH_ELEM_IN_GROUP(Layer, l, layers) {
    if(l->lesioned()) continue;
    int lay_vals = l->Compute_PRerr(this);
    if(lay_vals > 0) {
      prerr.IncrVals(l->prerr);
    }
    n_vals += lay_vals;
  }
  if(n_vals > 0) {
    sum_prerr.IncrVals(prerr);
    prerr.ComputePR();
    sum_prerr.ComputePR();
  }
}

void Network::Compute_TrialStats() {
  Compute_SSE(sse_unit_avg, sse_sqrt);
  if(compute_prerr)
    Compute_PRerr();
}

void Network::DMem_ShareTrialData(DataTable* dt, int n_rows) {
#ifdef DMEM_COMPILE
  dt->DMem_ShareRows(dmem_trl_comm.comm, n_rows);
#endif
}

void Network::Compute_EpochSSE() {
  sum_sse = cur_sum_sse;
  cnt_err = cur_cnt_err;
  if(avg_sse_n > 0) {
    avg_sse = cur_sum_sse / (float)avg_sse_n;
    pct_err = cnt_err / (float)avg_sse_n;
    pct_cor = 1.0f - pct_err;
  }

  cur_sum_sse = 0.0f;
  avg_sse_n = 0;
  cur_cnt_err = 0.0f;
}

void Network::Compute_EpochPRerr() {
  epc_prerr = sum_prerr;
  epc_prerr.ComputePR();        // make sure, in case of dmem summing
  sum_prerr.InitVals();         // reset!
}

void Network::Compute_EpochStats() {
#ifdef DMEM_COMPILE
  DMem_ComputeAggs(dmem_trl_comm.comm);
#endif
  Compute_EpochSSE();
  if(compute_prerr)
    Compute_EpochPRerr();
}


DataTable* Network::NetStructToTable(DataTable* dt, bool list_specs) {
  bool new_table = false;
  if(!dt) {
    taProject* proj = GET_MY_OWNER(taProject);
    dt = proj->GetNewAnalysisDataTable("NetStru_" + name, true);
    new_table = true;
  }
  dt->StructUpdate(true);
  int idx;
  dt->RemoveAllRows();
  DataCol* col;
  col = dt->FindMakeColName("Group", idx, VT_STRING);
  col->desc = "name of layer group to put layer in -- layer groups are useful for organizing layers logically, moving them together, etc";
  col = dt->FindMakeColName("Name", idx, VT_STRING);
  col->desc = "name of layer -- will be updated to be a valid C language label so it can be referred to in Programs etc";
  col = dt->FindMakeColName("Type", idx, VT_STRING);
  col->desc = "functional type of layer in terms of input/output data -- options are: INPUT, TARGET, OUTPUT, HIDDEN -- TARGET means it learns from target output data, while OUTPUT means it generates output error signals but does not use output values for error-driven learning, HIDDEN doesn't get any input/output, and INPUT is simply an input layer";
  col = dt->FindMakeColName("Size_X", idx, VT_INT);
  col->desc = "size of layer (number of units) in the horizontal (X) axis";
  col = dt->FindMakeColName("Size_Y", idx, VT_INT);
  col->desc = "size of layer (number of units) in the vertical (Y) axis";
  col = dt->FindMakeColName("UnitGps_X", idx, VT_INT);
  col->desc = "number of unit groups (subgroups of units within a layer) in the horizontal (X) axis -- set to 0 to not have any subgroups at all";
  col = dt->FindMakeColName("UnitGps_Y", idx, VT_INT);
  col->desc = "number of unit groups (subgroups of units within a layer) in the vertical (Y) axis -- set to 0 to not have any subgroups at all";
  col = dt->FindMakeColName("RecvPrjns", idx, VT_STRING);
  col->desc = "receiving projections -- connections from other layers that send into this one -- these must be valid names of other layers in the network, separated by a space if there are multiple";
  col = dt->FindMakeColName("SendPrjns", idx, VT_STRING);
  col->desc = "sending projections -- the other layers that the layer sends connections to";
  if(list_specs) {
    col = dt->FindMakeColName("UnitSpec", idx, VT_STRING);
    col->desc = "name of unit spec to use for this layer";
    col = dt->FindMakeColName("LayerSpec", idx, VT_STRING);
    col->desc = "name of layer spec to use for this layer";
  }

  FOREACH_ELEM_IN_GROUP(Layer, l, layers) {
//     if(l->lesioned()) continue;   // for this, get everything
    Layer_Group* lg = NULL;
    if(l->owner != &layers)
      lg = (Layer_Group*)l->owner;
    dt->AddBlankRow();
    dt->SetVal(l->name, "Name", -1);
    if(lg)
      dt->SetVal(lg->name, "Group", -1);
    String ltype = l->GetEnumString("LayerType", l->layer_type);
    dt->SetVal(ltype, "Type", -1);
    dt->SetVal(l->un_geom.x, "Size_X", -1);
    dt->SetVal(l->un_geom.y, "Size_Y", -1);
    if(l->unit_groups) {
      dt->SetVal(l->gp_geom.x, "UnitGps_X", -1);
      dt->SetVal(l->gp_geom.y, "UnitGps_Y", -1);
    }
    String fmp;
    for(int i=0; i<l->projections.size; i++) {
      Projection* pj = l->projections.FastEl(i);
      fmp += pj->from->name + " ";
    }
    dt->SetVal(fmp, "RecvPrjns", -1);

    String snp;
    for(int i=0; i<l->send_prjns.size; i++) {
      Projection* pj = l->send_prjns.FastEl(i);
      snp += pj->layer->name + " ";
    }
    dt->SetVal(snp, "SendPrjns", -1);

    if(list_specs) {
      UnitSpec* us = l->GetUnitSpec();
      if(us)
        dt->SetVal(us->name, "UnitSpec", -1);
      else
        dt->SetVal("NULL", "UnitSpec", -1);
      LayerSpec* ls = l->GetLayerSpec();
      if(ls)
        dt->SetVal(ls->name, "LayerSpec", -1);
      else
        dt->SetVal("NULL", "LayerSpec", -1);
    }
  }
  dt->StructUpdate(false);
  if(new_table)
    tabMisc::DelayedFunCall_gui(dt, "BrowserSelectMe");
  return dt;
}

void Network::NetStructFmTable(DataTable* dt) {
  if(TestError(!dt, "NetStructFmTable", "must pass the data table argument"))
    return;
  // set tag for all layers to do cleanup at end
  FOREACH_ELEM_IN_GROUP(Layer, lay, layers) {
    lay->SetBaseFlag(BF_MISC1);
  }

  Layer *l = 0;
  // first pass build all the layers
  for(int i=0;i<dt->rows; i++) {
    String gpnm = trim(dt->GetVal("Group", i).toString());
    String lnm = trim(dt->GetVal("Name", i).toString());
    String ltyp = trim(dt->GetVal("Type", i).toString());
    int szx = dt->GetVal("Size_X", i).toInt();
    int szy = dt->GetVal("Size_Y", i).toInt();
    int gszx = dt->GetVal("UnitGps_X", i).toInt();
    int gszy = dt->GetVal("UnitGps_Y", i).toInt();
    String recvs = trim(dt->GetVal("RecvPrjns", i).toString());

    if(gpnm.empty()) {
      l = FindMakeLayer(lnm);
      if(l->InLayerSubGroup()) {
        layers.Transfer(l);     // transfer into main list
      }
    }
    else {
      Layer_Group* lgp = FindMakeLayerGroup(gpnm);
      l = FindLayer(lnm);
      if(l && l->owner != lgp) {
        lgp->Transfer(l);       // make it ours
      }
      else {
        l = lgp->FindMakeLayer(lnm); // make new one
      }
    }
    l->ClearBaseFlag(BF_MISC1); // mark it
    String etyp;
    l->layer_type = (Layer::LayerType)l->GetEnumVal(ltyp, etyp);
    l->un_geom.x = szx;
    l->un_geom.y = szy;
    l->un_geom.UpdateAfterEdit();
    if(gszx > 0 && gszy > 0) {
      l->unit_groups = true;
      l->gp_geom.x = gszx;
      l->gp_geom.y = gszy;
      l->gp_geom.UpdateAfterEdit();
    }
  }
  // second pass make projections
  for(int i=0;i<dt->rows; i++) {
    String lnm = trim(dt->GetVal("Name", i).toString());
    String recvs = trim(dt->GetVal("RecvPrjns", i).toString());

    l = FindLayer(lnm);
    for(int p=0;p<l->projections.size;p++) {
      Projection* prjn = l->projections.FastEl(p);
      prjn->SetBaseFlag(BF_MISC1);
    }

    String_Array rps;
    rps.FmDelimString(recvs, " "); // fill with items
    for(int p=0;p<rps.size; p++) {
      String fmnm = rps.FastEl(p);
      Layer* fm = FindLayer(fmnm);
      if(fm) {
        Projection* prjn = FindMakePrjn(l, fm);
        prjn->ClearBaseFlag(BF_MISC1);
      }
    }

    // cull any non-wanted projections
    for(int p=l->projections.size-1; p>=0; p--) {
      Projection* prjn = l->projections.FastEl(p);
      if(prjn->HasBaseFlag(BF_MISC1)) {
        l->projections.RemoveIdx(p);
      }
    }
  }

  // cull any non-wanted layers
  for(int i=layers.leaves-1; i>=0; i--) {
    l = layers.Leaf(i);
    if(l->HasBaseFlag(BF_MISC1))
      layers.RemoveLeafIdx(i);
  }
}

DataTable* Network::NetPrjnsToTable(DataTable* dt) {
  bool new_table = false;
  if(!dt) {
    taProject* proj = GET_MY_OWNER(taProject);
    dt = proj->GetNewAnalysisDataTable("NetPrjns_" + name, true);
    new_table = true;
  }
  dt->StructUpdate(true);
  int idx;
  dt->RemoveAllRows();
  DataCol* col;
  col = dt->FindMakeColName("LayerName", idx, VT_STRING);
  col->desc = "name of layer -- will be updated to be a valid C language label so it can be referred to in Programs etc";
  col = dt->FindMakeColName("PrjnFrom", idx, VT_STRING);
  col->desc = "receiving projection -- name of sending layer that this layer receives from";
  col = dt->FindMakeColName("PrjnSpec", idx, VT_STRING);
  col->desc = "name of projection spec for this projection";
  col = dt->FindMakeColName("ConSpec", idx, VT_STRING);
  col->desc = "name of connection spec for this projection";

  FOREACH_ELEM_IN_GROUP(Layer, l, layers) {
//     if(l->lesioned()) continue;   // for this, get everything
    for(int i=0; i<l->projections.size; i++) {
      Projection* pj = l->projections.FastEl(i);
      dt->AddBlankRow();
      dt->SetVal(l->name, "LayerName", -1);
      dt->SetVal(pj->from->name, "PrjnFrom", -1);
      ProjectionSpec* ps = pj->GetPrjnSpec();
      if(ps)
        dt->SetVal(ps->name, "PrjnSpec", -1);
      else
        dt->SetVal("NULL", "PrjnSpec", -1);
      ConSpec* cs = pj->GetConSpec();
      if(cs)
        dt->SetVal(cs->name, "ConSpec", -1);
      else
        dt->SetVal("NULL", "ConSpec", -1);
    }
  }
  dt->StructUpdate(false);
  if(new_table)
    tabMisc::DelayedFunCall_gui(dt, "BrowserSelectMe");
  return dt;
}

void Network::DMemTrialBarrier() {
#ifndef DMEM_COMPILE
  return;
#else
  if(taMisc::dmem_nprocs <= 1 || dmem_trl_comm.nprocs <= 1)
    return;
  dmem_trl_comm.Barrier();
#endif
}

#ifdef DMEM_COMPILE

void Network::DMem_SyncNRecvCons() {
  if(dmem_nprocs_actual <= 1) return;
  if(n_cons <= 0) return;
  if(dmem_sync_level == DMEM_SYNC_LAYER) {
    FOREACH_ELEM_IN_GROUP(Layer, l, layers) {
      if(!l->lesioned())
        l->DMem_SyncNRecvCons();
    }
  }
  else {
    dmem_share_units.Sync(0);
  }
  // need to re-agg all the cons after syncing!
  n_cons = 0;
  FOREACH_ELEM_IN_GROUP(Layer, l, layers) {
    if(l->lesioned()) continue;
    FOREACH_ELEM_IN_GROUP(Unit, u, l->units) {
      if(u->lesioned()) continue;
      n_cons += u->n_recv_cons;
    }
  }
}

void Network::DMem_SyncNet() {
  if(dmem_nprocs_actual <= 1) return;
  if(TestError(dmem_sync_level != DMEM_SYNC_NETWORK, "DMem_SyncNet",
               "attempt to DMem sync at network level, should only be at layer level!")) {
    return;
  }
  dmem_share_units.Sync(1);
}

void Network::DMem_SyncAct() {
  if(dmem_nprocs_actual <= 1) return;
  if(TestError(dmem_sync_level != DMEM_SYNC_NETWORK, "DMem_SyncAct",
               "attempt to DMem sync at network level, should only be at layer level!")) {
    return;
  }
  dmem_share_units.Sync(2);
}

void Network::DMem_DistributeUnits() {
  dmem_nprocs_actual = MIN(dmem_nprocs, taMisc::dmem_nprocs);
  dmem_net_comm.CommSubGpInner(dmem_nprocs_actual);     // network is inner-group
  dmem_trl_comm.CommSubGpOuter(dmem_nprocs_actual);     // trial is outer-group
  dmem_share_units.comm = dmem_net_comm.comm;

  dmem_share_units.Reset();
  bool any_custom_distrib = false;
  FOREACH_ELEM_IN_GROUP(Layer, lay, layers) {
    if(lay->lesioned()) continue;
    lay->dmem_share_units.comm = dmem_share_units.comm;
    if(dmem_sync_level == DMEM_SYNC_LAYER) {
      lay->DMem_DistributeUnits();
    }
    else {
      if(lay->DMem_DistributeUnits_impl(dmem_share_units))
        any_custom_distrib = true;
    }
  }
  if(dmem_sync_level == DMEM_SYNC_NETWORK) {
    if(!any_custom_distrib) {
      dmem_share_units.DistributeItems(); // use more efficient full distribution
    }
    else
      dmem_share_units.Compile_ShareTypes(); // use custom distribution: just compile it
  }
}

void Network::DMem_UpdtWtUpdt() {
  if(dmem_trl_comm.nprocs > 1) {
    TestWarning(wt_update != SMALL_BATCH, "DMem_UpdtWtUpdt",
                "changing wt_update to SMALL_BATCH because dmem trial nprocs > 1");
    wt_update = SMALL_BATCH;                      // must be small batch
    small_batch_n_eff = small_batch_n / dmem_trl_comm.nprocs; // effective small_batch_n
    if(small_batch_n_eff < 1) small_batch_n_eff = 1;
  }
}

void Network::DMem_InitAggs() {
  dmem_agg_sum.ScanMembers(GetTypeDef(), (void*)this);
  dmem_agg_sum.CompileVars();
}

void Network::DMem_PruneNonLocalCons() {
  if(dmem_nprocs_actual <= 1) return;
  FOREACH_ELEM_IN_GROUP(Layer, l, layers) {
    if(l->lesioned()) continue;
    FOREACH_ELEM_IN_GROUP(Unit, u, l->units) {
      if(u->DMem_IsLocal()) {
        continue;
      }
      // only non-local
      RecvCons* recv_gp;
      int g;
      for (g = 0; g < u->recv.size; g++) {
        recv_gp = (RecvCons *)u->recv.FastEl(g);
        if(recv_gp->GetConSpec()->DMem_AlwaysLocal()) continue;
        for (int sui = recv_gp->size-1; sui >= 0; sui--) {
          u->DisConnectFrom(recv_gp->Un(sui,this), NULL);
        }
      }
    }
  }
}

void Network::DMem_SumDWts(MPI_Comm comm) {
  wt_sync_time.StartTimer(false); // don't reset
  static float_Array values;
  static float_Array results;

  int np = 0; MPI_Comm_size(comm, &np);
  if(np <= 1) return;

  values.SetSize(n_cons + n_units);

  int cidx = 0;
  FOREACH_ELEM_IN_GROUP(Layer, lay, layers) {
    if(lay->lesioned()) continue;
    FOREACH_ELEM_IN_GROUP(Unit, un, lay->units) {
      if(un->lesioned()) continue;
      if(un->bias.size)
        values.FastEl(cidx++) = un->bias.OwnCn(0,BaseCons::DWT);
      if(RecvOwnsCons()) {
        for(int g = 0; g < un->recv.size; g++) {
          RecvCons* cg = un->recv.FastEl(g);
          float* dwts = cg->OwnCnVar(BaseCons::DWT);
          for(int i = 0;i<cg->size;i++) { // todo: could use memcopy here!
            values.FastEl(cidx++) = dwts[i];
          }
        }
      }
      else {
        for(int g = 0; g < un->send.size; g++) {
          SendCons* cg = un->send.FastEl(g);
          float* dwts = cg->OwnCnVar(BaseCons::DWT);
          for(int i = 0;i<cg->size;i++) {
            values.FastEl(cidx++) = dwts[i];
          }
        }
      }
    }
  }

  results.SetSize(cidx);
  DMEM_MPICALL(MPI_Allreduce(values.el, results.el, cidx, MPI_FLOAT, MPI_SUM, comm),
               "Network::SumDWts", "Allreduce");

  cidx = 0;
  FOREACH_ELEM_IN_GROUP(Layer, lay, layers) {
    if(lay->lesioned()) continue;
    FOREACH_ELEM_IN_GROUP(Unit, un, lay->units) {
      if(un->lesioned()) continue;
      if(un->bias.size)
        un->bias.OwnCn(0,BaseCons::DWT) = results.FastEl(cidx++);
      if(RecvOwnsCons()) {
        for(int g = 0; g < un->recv.size; g++) {
          RecvCons* cg = un->recv.FastEl(g);
          float* dwts = cg->OwnCnVar(BaseCons::DWT);
          for(int i = 0;i<cg->size;i++) {
            dwts[i] = results.FastEl(cidx++);
          }
        }
      }
      else {
        for(int g = 0; g < un->send.size; g++) {
          SendCons* cg = un->send.FastEl(g);
          float* dwts = cg->OwnCnVar(BaseCons::DWT);
          for(int i = 0;i<cg->size;i++) {
            dwts[i] = results.FastEl(cidx++);
          }
        }
      }
    }
  }
  wt_sync_time.EndTimer();
}

void Network::DMem_AvgWts(MPI_Comm comm) {
  static float_Array values;
  static float_Array results;

  int np = 0; MPI_Comm_size(comm, &np);
  if(np <= 1) return;

  values.SetSize(n_cons + n_units);

  int cidx = 0;
  FOREACH_ELEM_IN_GROUP(Layer, lay, layers) {
    if(lay->lesioned()) continue;
    FOREACH_ELEM_IN_GROUP(Unit, un, lay->units) {
      if(un->lesioned()) continue;
      if(un->bias.size)
        values.FastEl(cidx++) = un->bias.OwnCn(0,BaseCons::WT);
      if(RecvOwnsCons()) {
        for(int g = 0; g < un->recv.size; g++) {
          RecvCons* cg = un->recv.FastEl(g);
          float* wts = cg->OwnCnVar(BaseCons::WT);
          for(int i = 0;i<cg->size;i++) {
            values.FastEl(cidx++) = wts[i];
          }
        }
      }
      else {
        for(int g = 0; g < un->send.size; g++) {
          SendCons* cg = un->send.FastEl(g);
          float* wts = cg->OwnCnVar(BaseCons::WT);
          for(int i = 0;i<cg->size;i++) {
            values.FastEl(cidx++) = wts[i];
          }
        }
      }
    }
  }

  results.SetSize(cidx);
  DMEM_MPICALL(MPI_Allreduce(values.el, results.el, cidx, MPI_FLOAT, MPI_SUM, comm),
                     "Network::AvgWts", "Allreduce");

  float avg_mult = 1.0f / (float)np;
  cidx = 0;
  FOREACH_ELEM_IN_GROUP(Layer, lay, layers) {
    if(lay->lesioned()) continue;
    FOREACH_ELEM_IN_GROUP(Unit, un, lay->units) {
      if(un->lesioned()) continue;
      if(un->bias.size)
        un->bias.OwnCn(0,BaseCons::WT) = avg_mult * results.FastEl(cidx++);
      if(RecvOwnsCons()) {
        for(int g = 0; g < un->recv.size; g++) {
          RecvCons* cg = un->recv.FastEl(g);
          float* wts = cg->OwnCnVar(BaseCons::WT);
          for(int i = 0;i<cg->size;i++) {
            wts[i] = avg_mult * results.FastEl(cidx++);
          }
        }
      }
      else {
        for(int g = 0; g < un->send.size; g++) {
          SendCons* cg = un->send.FastEl(g);
          float* wts = cg->OwnCnVar(BaseCons::WT);
          for(int i = 0;i<cg->size;i++) {
            wts[i] = avg_mult * results.FastEl(cidx++);
          }
        }
      }
    }
  }
}

void Network::DMem_ComputeAggs(MPI_Comm comm) {
  dmem_agg_sum.AggVar(comm, MPI_SUM);
}

void Network::DMem_SymmetrizeWts() {
  MPI_Comm comm = dmem_share_units.comm;
  int np = 0; MPI_Comm_size(comm, &np);

  if(np <= 1) return;

  static int_Array unit_idxs;
  static float_Array wt_vals;

  static int_Array all_unit_idxs;
  static float_Array all_wt_vals;

  FOREACH_ELEM_IN_GROUP(Layer, lay, layers) {
    if(lay->lesioned()) continue;
    if(lay->projections.size == 0) continue;
    FOREACH_ELEM_IN_GROUP(Unit, un, lay->units) {
      if(un->lesioned()) continue;
      int gi;
      for(gi=0;gi<un->recv.size;gi++) {
        RecvCons* cg = un->recv[gi];
        if(!cg->GetConSpec()->wt_limits.sym) continue;

        // check for presence of reciprocal connections in the first place..
        Layer* fmlay = cg->prjn->from;
        bool has_recip_prjn = false;
        FOREACH_ELEM_IN_GROUP(Projection, fmpj, fmlay->projections) {
          if(fmpj->from == lay) {
            has_recip_prjn = true;
            break;
          }
        }
        if(!has_recip_prjn) continue; // no sym cons there anyway

        // todo: below could probably use flat_idx instead of computing indexes on the fly

        if(un->DMem_IsLocal()) {
          // I'm local: I recv values from all other procs: each sends unit index and wt
          all_unit_idxs.Reset();
          all_wt_vals.Reset();
          int proc;
          for(proc = 0; proc < np; proc++) {
            if(proc == un->dmem_local_proc) continue;
            // recv the number of connection values obtained
            int msgsize = 0;
            MPI_Status status;
            DMEM_MPICALL(MPI_Recv((void*)&msgsize, 1, MPI_INT, proc, 101, comm, &status),
                         "DMem_SymmetrizeWts", "MPI_Recv msgsize");
            if(msgsize > 0) {
              unit_idxs.SetSize(msgsize);
              wt_vals.SetSize(msgsize);
              DMEM_MPICALL(MPI_Recv(unit_idxs.el, msgsize, MPI_INT, proc, 102, comm, &status),
                           "DMem_SymmetrizeWts", "MPI_Recv unit_idxs");
              DMEM_MPICALL(MPI_Recv(wt_vals.el, msgsize, MPI_FLOAT, proc, 103, comm, &status),
                           "DMem_SymmetrizeWts", "MPI_Recv wt_vals");

              all_unit_idxs.CopyVals(unit_idxs, 0, -1, all_unit_idxs.size);
              all_wt_vals.CopyVals(wt_vals, 0, -1, all_wt_vals.size);
            }
          }
          // now have all the data collected, to through and get the sym values!
          for(int i=0;i<cg->size;i++) {
            Unit* fm = cg->Un(i,this);
            int uidx = fm->GetMyLeafIndex();
            if(uidx < 0) continue;
            int sidx = all_unit_idxs.FindEl(uidx);
            if(sidx < 0) continue;
            cg->Cn(i,BaseCons::WT,this) = all_wt_vals[sidx];
          }
        }
        else {
          // collect my data and send it off!
          unit_idxs.Reset();
          wt_vals.Reset();
          int uni;
          for(uni=0;uni<fmlay->units.leaves;uni++) {
            Unit* fm = fmlay->units.Leaf(uni);
            if(!fm->DMem_IsLocal()) continue;
            for(int g = 0; g < fm->recv.size; g++) {
              RecvCons* fmg = fm->recv.FastEl(g);
              if(fmg->prjn->from != lay) continue;
              int con = fmg->FindConFromIdx(un);
              if(con >= 0) {
                unit_idxs.Add(uni);
                wt_vals.Add(fmg->Cn(con, BaseCons::WT, this));
              }
            }
          }
          // send the number, then the data
          int msgsize = unit_idxs.size;
          DMEM_MPICALL(MPI_Send((void*)&msgsize, 1, MPI_INT, un->dmem_local_proc, 101, comm),
                       "DMem_SymmetrizeWts", "MPI_Send msgsize");

          if(msgsize > 0) {
            DMEM_MPICALL(MPI_Send(unit_idxs.el, msgsize, MPI_INT, un->dmem_local_proc, 102, comm),
                         "DMem_SymmetrizeWts", "MPI_Send unit_idxs");
            DMEM_MPICALL(MPI_Send(wt_vals.el, msgsize, MPI_FLOAT, un->dmem_local_proc, 103, comm),
                         "DMem_SymmetrizeWts", "MPI_Send wt_vals");
          }
        }
      }
    }
  }
}

#endif  // DMEM

void Network::Copy_Weights(const Network* src) {
  taMisc::Busy();
  Layer* l, *sl;
  taLeafItr i,si;
  for(l = (Layer*)layers.FirstEl(i), sl = (Layer*)src->layers.FirstEl(si);
      (l) && (sl);
      l = (Layer*)layers.NextEl(i), sl = (Layer*)src->layers.NextEl(si))
  {
    if(!l->lesioned() && !sl->lesioned())
      l->Copy_Weights(sl);
  }
  UpdateAllViews();
  taMisc::DoneBusy();
}

void Network::SaveWeights_strm(ostream& strm, Network::WtSaveFormat fmt) {
  taMisc::Busy();
  if(fmt == NET_FMT) fmt = wt_save_fmt;

  strm << "<Fmt " << GetTypeDef()->GetEnumString("WtSaveFormat", fmt) << ">\n"
       << "<Name " << GetName() << ">\n"
       << "<Epoch " << epoch << ">\n";
  FOREACH_ELEM_IN_GROUP(Layer, l, layers) {
    if(l->lesioned()) continue;
    strm << "<Lay " << l->name << ">\n";
    l->SaveWeights_strm(strm, (RecvCons::WtSaveFormat)fmt);
    strm << "</Lay>\n";
  }
  taMisc::DoneBusy();
}

bool Network::LoadWeights_strm(istream& strm, bool quiet) {
  bool rval = false;
  String tag, val, enum_typ_nm;
  int stat = 0;
  RecvCons::WtSaveFormat fmt;
  taMisc::Busy();
  int c = strm.peek();
  if(TestError(c == '#', "LoadWeights_strm",
               "cannot read old formats from version 3.2 -- must use network save")) {
    goto exit;
  }
  stat = taMisc::read_tag(strm, tag, val);
  if(TestError((stat != taMisc::TAG_GOT) || (tag != "Fmt"), "LoadWeights_strm",
               "did not got find Fmt tag at start of weights file -- probably file not found")) {
    goto exit;
  }

  fmt = (RecvCons::WtSaveFormat)TA_RecvCons.GetEnumVal(val, enum_typ_nm);

  stat = taMisc::read_tag(strm, tag, val);
  if((stat != taMisc::TAG_GOT) || (tag != "Name")) goto exit;
  // don't set the name!!! this causes more trouble than it is worth!!
//   name = val;

  stat = taMisc::read_tag(strm, tag, val);
  if((stat != taMisc::TAG_GOT) || (tag != "Epoch")) goto exit;
  epoch = (int)val;

  while(true) {
    stat = taMisc::read_tag(strm, tag, val);
    if(stat != taMisc::TAG_GOT) break;          // *should* break at TAG_END
    if(tag != "Lay") { stat = taMisc::TAG_NONE;  break; } // bumping up against some other tag
    Layer* lay = layers.FindLeafName(val);
    if(lay) {
      stat = lay->LoadWeights_strm(strm, fmt, quiet);
    }
    else {
      TestWarning(!quiet, "LoadWeights", "Layer not found:", val);
      stat = Layer::SkipWeights_strm(strm, fmt, quiet);
    }
    if(stat != taMisc::TAG_END) break;
    stat = taMisc::TAG_NONE;           // reset so EndTag will definitely read new tag
    RecvCons::LoadWeights_EndTag(strm, "Lay", tag, stat, quiet);
    if(stat != taMisc::TAG_END) break;
  }
  // could try to read end tag but what is the point?
  rval = true;
  UpdateAllViews();
exit:
  taMisc::DoneBusy();
  return true;
}

void Network::SaveWeights(const String& fname, Network::WtSaveFormat fmt) {
  taFiler* flr = GetSaveFiler(fname, ".wts", true);
  if(flr->ostrm)
    SaveWeights_strm(*flr->ostrm, fmt);
  flr->Close();
  taRefN::unRefDone(flr);
}

bool Network::LoadWeights(const String& fname, bool quiet) {
  taFiler* flr = GetLoadFiler(fname, ".wts", true);
  bool rval = false;
  if(flr->istrm) {
    rval = LoadWeights_strm(*flr->istrm, quiet);
  }
  else {
    TestError(true, "LoadWeights", "aborted due to inability to load weights file");
    // the above should be unnecessary but we're not getting the error sometimes..
  }
  flr->Close();
  taRefN::unRefDone(flr);
  return rval;
}

void Network::SaveToWeights(Weights* wts) {
  if(wts == NULL) {
    wts = (Weights*)weights.New(1);
  }
  ostringstream oss;
  SaveWeights_strm(oss, TEXT);  // always use text for this
  wts->wt_file = oss.str().c_str();
  wts->epoch = epoch;
  wts->batch = batch;
  if(wts->name.contains("Weights") && file_name.nonempty()) {
    wts->SetName(file_name);
  }
  wts->SigEmitUpdated();
}

bool Network::LoadFmWeights(Weights* wts, bool quiet) {
  if(TestError(!wts, "LoadFmWeights", "Weights object is NULL")) {
    return false;
  }
  if(!wts->HasWeights(true)) return false;
  istringstream iss(wts->wt_file.chars());
  return LoadWeights_strm(iss, quiet);
}

void Network::SaveToFirstWeights() {
  if(weights.size == 0)
    weights.New(1);
  Weights* wts = weights[0];
  SaveToWeights(wts);
}

bool Network::LoadFmFirstWeights(bool quiet) {
  if(TestError(weights.size == 0, "LoadFmWeights", "There is not an existing first Weights object -- nothing to load from -- must call SaveToFirstWeights first")) {
    return false;
  }
  Weights* wts = weights[0];
  return LoadFmWeights(wts);
}

void Weights::WeightsFmNet() {
  Network* net = GET_MY_OWNER(Network);
  if(!net) return;
  net->SaveToWeights(this);
}

bool Weights::WeightsToNet() {
  Network* net = GET_MY_OWNER(Network);
  if(!net) return false;
  return net->LoadFmWeights(this, quiet_load);
}

void Network::LayerZPos_Unitize() {
  int_Array zvals;
  taVector3i lpos;
  FOREACH_ELEM_IN_GROUP(Layer, l, layers) {
    l->GetAbsPos(lpos);
    zvals.AddUnique(lpos.z);
  }
  zvals.Sort();
  FOREACH_ELEM_IN_GROUP(Layer, l, layers) {
    int nw_z = zvals.FindEl(l->pos.z); // replace with its index on sorted list..
    l->pos.z += nw_z - l->pos.z;
  }
  UpdateMaxDispSize();
}

void Network::LayerPos_Cleanup() {
  if (flags & MANUAL_POS) return;
  layers.LayerPos_Cleanup();
  UpdateMaxDispSize();          // must do that in case something moves
}

void Network::LayerPos_GridLayout_2d(int x_space, int y_space,
                                     int gp_grid_x, int lay_grid_x) {
  StructUpdate(true);
  layers.LayerPos_GridLayout_2d(x_space, y_space, gp_grid_x, lay_grid_x);
  UpdateMaxDispSize();          // must do that in case something moves
  StructUpdate(false);
}

void Network::LayerPos_GridLayout_3d(int x_space, int y_space,
                                     int z_size, int gp_grid_x, int lay_grid_x) {
  StructUpdate(true);
  layers.LayerPos_GridLayout_3d(x_space, y_space, z_size, gp_grid_x, lay_grid_x);
  StructUpdate(false);
}

void Network::Compute_LayerDistances() {
  // first reset all
  FOREACH_ELEM_IN_GROUP(Layer, l, layers) {
    if(l->lesioned()) continue;
    l->dist.fm_input = -1; l->dist.fm_output = -1;
  }

  // next go through and find inputs
  FOREACH_ELEM_IN_GROUP(Layer, l, layers) {
    if(l->lesioned()) continue;
    if(l->layer_type != Layer::INPUT) continue;
    l->dist.fm_input = 0;
    l->PropagateInputDistance();
  }
  // then outputs
  FOREACH_ELEM_IN_GROUP(Layer, l, layers) {
    if(l->lesioned()) continue;
    if(!((l->layer_type == Layer::OUTPUT) || (l->layer_type == Layer::TARGET))) continue;
    l->dist.fm_output = 0;
    l->PropagateOutputDistance();
  }
}

void Network::Compute_PrjnDirections() {
  Compute_LayerDistances();     // required data
  FOREACH_ELEM_IN_GROUP(Layer, l, layers) {
    if(l->lesioned()) continue;
    l->Compute_PrjnDirections();
  }
}

void Network::SetUnitNames(bool force_use_unit_names) {
  FOREACH_ELEM_IN_GROUP(Layer, l, layers) {
    if(!l->lesioned())
      l->SetUnitNames(force_use_unit_names);
  }
  UpdateAllViews();
}

void Network::SetUnitNamesFromDataTable(DataTable* undt, int max_unit_chars,
                                        bool propagate_names) {
  if(TestError(!undt || undt->rows < 1, "SetUnitNamesFromDataTable", "null unit names table or doesn't have 1 or more rows!")) {
    return;
  }

  for(int i=0;i<undt->cols();i++) {
    DataCol* ndc = undt->data.FastEl(i);
    Layer* lay = (Layer*)layers.FindLeafName(ndc->name);
    if(!lay) continue;
    lay->SetUnitNamesFromDataCol(ndc, max_unit_chars);
  }
  if(propagate_names)
    GetLocalistName();  // propagate
  return;
}

void Network::GetUnitNames(bool force_use_unit_names) {
  FOREACH_ELEM_IN_GROUP(Layer, l, layers) {
    if(!l->lesioned())
      l->GetUnitNames(force_use_unit_names);
  }
  UpdateAllViews();
}

void Network::GetLocalistName() {
  FOREACH_ELEM_IN_GROUP(Layer, l, layers) {
    if(!l->lesioned())
      l->GetLocalistName();
  }
  UpdateAllViews();
}

bool Network::SnapVar() {
  SimpleMathSpec sm;
  sm.opr = SimpleMathSpec::NONE;
  return Snapshot("", sm, false); // empty var is retrieved
}

bool Network::SnapAnd() {
  SimpleMathSpec sm;
  sm.opr = SimpleMathSpec::MIN;
  return Snapshot("", sm, true); // empty var is retrieved
}

bool Network::SnapOr() {
  SimpleMathSpec sm;
  sm.opr = SimpleMathSpec::MAX;
  return Snapshot("", sm, true); // empty var is retrieved
}

bool Network::SnapThresh(float thresh_val) {
  SimpleMathSpec sm;
  sm.opr = SimpleMathSpec::THRESH;
  sm.arg = thresh_val;
  sm.lw = 0.0;
  sm.hi = 1.0;
  return Snapshot("", sm, false);
}

// Network::GetViewVar is in netstru_qtso.cpp

bool Network::Snapshot(const String& variable, SimpleMathSpec& math_op, bool arg_is_snap) {
  String var = variable;
  if(var.empty()) {
    var = GetViewVar();
    if(TestError(var.empty(), "Snapshot", "No view variable found!"))
      return false;
  }
  if(var.startsWith("r.") || var.startsWith("s.")) {
    Unit* src_u = GetViewSrcU();
    if(TestError(!src_u, "Snapshot", "For r. or s. variables, must have a selected unit in the network view!"))
      return false;
  }

  FOREACH_ELEM_IN_GROUP(Layer, l, layers) {
    if(l->lesioned()) continue;
    if(!l->Snapshot(var, math_op, arg_is_snap)) return false;
  }
  UpdateAllViews();
  return true;
}

void Network::MonitorVar(NetMonitor* net_mon, const String& variable) {
  if(!net_mon) return;
  net_mon->AddObject(this, variable);
}

void Network::RemoveMonitors() {
  if (!proj) return;
  TokenSpace& ts = TA_NetMonitor.tokens;
  for (int i = 0; i < ts.size; ++i) {
    NetMonitor* nm = (NetMonitor*)ts.FastEl(i);
    if(nm->network.ptr() != this) continue;
    nm->RemoveMonitors();
  }
}
void Network::UpdateMonitors() {
  if (!proj) return;
  TokenSpace& ts = TA_NetMonitor.tokens;
  for (int i = 0; i < ts.size; ++i) {
    NetMonitor* nm = (NetMonitor*)ts.FastEl(i);
    if(nm->network.ptr() != this) continue;
    nm->UpdateDataTable();
  }
}

void Network::NetControlPanel(ControlPanel* ctrl_panel, const String& extra_label, const String& sub_gp_nm) {
  if(!ctrl_panel) {
    taProject* proj = GET_MY_OWNER(taProject);
    if(TestError(!proj, "NetControlPanel", "cannot find project")) return;
    ctrl_panel = (ControlPanel*)proj->ctrl_panels.New(1);
  }
  TypeDef* td = GetTypeDef();
  for(int i=td->members.size-1; i>=0; i--) {
    MemberDef* md = td->members[i];
    if(!md->HasOption("VIEW")) continue;
    // filter by current guys..
    if(HasUserData(md->name) && !GetUserDataAsBool(md->name)) continue;
    ctrl_panel->SelectMember(this, md, extra_label, "", sub_gp_nm);
  }
}

void Network::TransformWeights(const SimpleMathSpec& trans) {
  taMisc::Busy();
  FOREACH_ELEM_IN_GROUP(Layer, l, layers) {
    if(!l->lesioned())
      l->TransformWeights(trans);
  }
  UpdateAllViews();
  taMisc::DoneBusy();
}

void Network::AddNoiseToWeights(const Random& noise_spec) {
  taMisc::Busy();
  FOREACH_ELEM_IN_GROUP(Layer, l, layers) {
    if(!l->lesioned())
      l->AddNoiseToWeights(noise_spec);
  }
  UpdateAllViews();
  taMisc::DoneBusy();
}

int Network::PruneCons(const SimpleMathSpec& pre_proc,
                          Relation::Relations rel, float cmp_val)
{
  taMisc::Busy();
  StructUpdate(true);
  int rval = 0;
  FOREACH_ELEM_IN_GROUP(Layer, l, layers) {
    if(!l->lesioned())
      rval += l->PruneCons(pre_proc, rel, cmp_val);
  }
  StructUpdate(false);
  taMisc::DoneBusy();
  UpdtAfterNetMod();
  return rval;
}

int Network::ProbAddCons(float p_add_con, float init_wt) {
  taMisc::Busy();
  StructUpdate(true);
  int rval = 0;
  FOREACH_ELEM_IN_GROUP(Layer, l, layers) {
    if(!l->lesioned())
      rval += l->ProbAddCons(p_add_con, init_wt);
  }
  StructUpdate(false);
  taMisc::DoneBusy();
  UpdtAfterNetMod();
  return rval;
}

int Network::LesionCons(float p_lesion, bool permute) {
  taMisc::Busy();
  StructUpdate(true);
  int rval = 0;
  FOREACH_ELEM_IN_GROUP(Layer, l, layers) {
    if(!l->lesioned())
      rval += l->LesionCons(p_lesion, permute);
  }
  StructUpdate(false);
  taMisc::DoneBusy();
  UpdtAfterNetMod();
  return rval;
}

int Network::LesionUnits(float p_lesion, bool permute) {
  taMisc::Busy();
  StructUpdate(true);
  int rval = 0;
  FOREACH_ELEM_IN_GROUP(Layer, l, layers) {
    if(!l->lesioned())
      rval += l->LesionUnits(p_lesion, permute);
  }
  StructUpdate(false);
  taMisc::DoneBusy();
  UpdtAfterNetMod();
  return rval;
}

void Network::UnLesionUnits() {
  taMisc::Busy();
  StructUpdate(true);
  int rval = 0;
  FOREACH_ELEM_IN_GROUP(Layer, l, layers) {
    if(!l->lesioned())
      l->UnLesionUnits();
  }
  StructUpdate(false);
  taMisc::DoneBusy();
  UpdtAfterNetMod();
}

void Network::UpdateMaxDispSize() {
  layers.UpdateMaxDispSize();
  max_disp_size = layers.max_disp_size;
  max_disp_size2d = layers.max_disp_size2d;
}

bool Network::UpdateUnitSpecs(bool force) {
  bool rval = true;
  FOREACH_ELEM_IN_GROUP(Layer, lay, layers){
    if(!lay->UpdateUnitSpecs(force))
      rval = false;
  }
  return rval;
}

bool Network::UpdateConSpecs(bool force) {
  bool rval = true;
  FOREACH_ELEM_IN_GROUP(Layer, lay, layers){
    if(!lay->UpdateConSpecs(force))
      rval = false;
  }
  return rval;
}

bool Network::UpdateAllSpecs(bool force) {
  bool rval = UpdateUnitSpecs(force);
  if(!UpdateConSpecs(force))
    rval = false;
  return rval;
}

void Network::ReplaceSpecs(BaseSpec* old_sp, BaseSpec* new_sp) {
  if(old_sp->InheritsFrom(&TA_UnitSpec))
    ReplaceUnitSpec((UnitSpec*)old_sp, (UnitSpec*)new_sp);
  else if(old_sp->InheritsFrom(&TA_ConSpec))
    ReplaceConSpec((ConSpec*)old_sp, (ConSpec*)new_sp);
  else if(old_sp->InheritsFrom(&TA_ProjectionSpec))
    ReplacePrjnSpec((ProjectionSpec*)old_sp, (ProjectionSpec*)new_sp);
  else if(old_sp->InheritsFrom(&TA_LayerSpec))
    ReplaceLayerSpec((LayerSpec*)old_sp, (LayerSpec*)new_sp);

  ReplaceSpecs_Gp(old_sp->children, new_sp->children);
}

void Network::ReplaceSpecs_Gp(const BaseSpec_Group& old_spg, BaseSpec_Group& new_spg) {
  taLeafItr spo, spn;
  BaseSpec* old_sp, *new_sp;
  for(old_sp = (BaseSpec*)old_spg.FirstEl(spo), new_sp = (BaseSpec*)new_spg.FirstEl(spn);
      old_sp && new_sp;
      old_sp = (BaseSpec*)old_spg.NextEl(spo), new_sp = (BaseSpec*)new_spg.NextEl(spn)) {
    ReplaceSpecs(old_sp, new_sp);
  }
}

int Network::ReplaceUnitSpec(UnitSpec* old_sp, UnitSpec* new_sp) {
  int nchg = 0;
  FOREACH_ELEM_IN_GROUP(Layer, l, layers) {
    if(!l->lesioned())
      nchg += l->ReplaceUnitSpec(old_sp, new_sp);
  }
  return nchg;
}

int Network::ReplaceConSpec(ConSpec* old_sp, ConSpec* new_sp) {
  int nchg = 0;
  FOREACH_ELEM_IN_GROUP(Layer, l, layers) {
    if(!l->lesioned())
      nchg += l->ReplaceConSpec(old_sp, new_sp);
  }
  return nchg;
}

int Network::ReplacePrjnSpec(ProjectionSpec* old_sp, ProjectionSpec* new_sp) {
  int nchg = 0;
  FOREACH_ELEM_IN_GROUP(Layer, l, layers) {
    if(!l->lesioned())
      nchg += l->ReplacePrjnSpec(old_sp, new_sp);
  }
  return nchg;
}

int Network::ReplaceLayerSpec(LayerSpec* old_sp, LayerSpec* new_sp) {
  int nchg = 0;
  FOREACH_ELEM_IN_GROUP(Layer, l, layers) {
    if(!l->lesioned())
      nchg += l->ReplaceLayerSpec(old_sp, new_sp);
  }
  return nchg;
}

DataTable* Network::WeightsToTable(DataTable* dt, Layer* recv_lay, Layer* send_lay)
{
  if(recv_lay == NULL) return NULL;
  return recv_lay->WeightsToTable(dt, send_lay);
}

DataTable* Network::VarToTable(DataTable* dt, const String& variable) {
  bool new_table = false;
  if(!dt) {
    taProject* proj = GET_MY_OWNER(taProject);
    dt = proj->GetNewAnalysisDataTable(name + "_Var_" + variable, true);
    new_table = true;
  }

  NetMonitor nm;
  taBase::Own(nm, this);
  nm.AddNetwork(this, variable);
  nm.items[0]->max_name_len = 20; // allow long names
  nm.SetDataNetwork(dt, this);
  nm.UpdateDataTable();
  dt->AddBlankRow();
  nm.GetMonVals();
  dt->WriteClose();
  if(new_table)
    tabMisc::DelayedFunCall_gui(dt, "BrowserSelectMe");
  return dt;
}

DataTable* Network::ConVarsToTable(DataTable* dt, const String& var1, const String& var2,
                             const String& var3, const String& var4, const String& var5,
                             const String& var6, const String& var7, const String& var8,
                             const String& var9, const String& var10, const String& var11,
                             const String& var12, const String& var13, const String& var14) {
  bool new_table = false;
  if(!dt) {
    taProject* proj = GET_MY_OWNER(taProject);
    dt = proj->GetNewAnalysisDataTable("ConVars", true);
    new_table = true;
  }
  dt->StructUpdate(true);
  FOREACH_ELEM_IN_GROUP(Layer, l, layers) {
    if(!l->lesioned())
      l->ConVarsToTable(dt, var1, var2, var3, var4, var5, var6, var7, var8,
                        var9, var10, var11, var12, var13, var14);
  }
  dt->StructUpdate(false);
  if(new_table)
    tabMisc::DelayedFunCall_gui(dt, "BrowserSelectMe");
  return dt;
}

bool Network::VarToVarCopy(const String& dest_var, const String& src_var) {
  FOREACH_ELEM_IN_GROUP(Layer, l, layers) {
    if(!l->lesioned())
      l->VarToVarCopy(dest_var, src_var);
  }
  return true;
}

bool Network::VarToVal(const String& dest_var, float val) {
  FOREACH_ELEM_IN_GROUP(Layer, l, layers) {
    if(!l->lesioned())
      l->VarToVal(dest_var, val);
  }
  return true;
}

static bool net_project_wts_propagate(Network* net, Unit* u, bool swt) {
  bool got_some = false;
  // propagate!
  for(int g = 0; g < (swt ? u->send.size : u->recv.size); g++) {
    taOBase* cg = (swt ? (taOBase*)u->send.FastEl(g) : (taOBase*)u->recv.FastEl(g));
    Projection* prjn = (swt ? ((SendCons*)cg)->prjn : ((RecvCons*)cg)->prjn);
    if(!prjn) continue;
    Layer* slay = (swt ? prjn->layer : prjn->from);

    if(slay->lesioned() || (prjn->from.ptr() == prjn->layer) ||
       slay->HasLayerFlag(Layer::PROJECT_WTS_DONE)) continue;
    slay->SetLayerFlag(Layer::PROJECT_WTS_NEXT); // next..
    got_some = true;                           // keep going..

    if(swt) {
      SendCons* scg = (SendCons*)cg;
      for(int ci = 0; ci < scg->size; ci++) {
        float wtv = scg->Cn(ci, BaseCons::WT, net);
        Unit* su = scg->Un(ci,net);
        su->wt_prjn += u->wt_prjn * wtv;
        su->tmp_calc1 += u->wt_prjn;
      }
    }
    else {
      RecvCons* scg = (RecvCons*)cg;
      for(int ci = 0; ci < scg->size; ci++) {
        float wtv = scg->Cn(ci, BaseCons::WT, net);
        Unit* su = scg->Un(ci,net);
        su->wt_prjn += u->wt_prjn * wtv;
        su->tmp_calc1 += u->wt_prjn;
      }
    }
  }
  return got_some;
}

void Network::ProjectUnitWeights(Unit* src_u, int top_k_un, int top_k_gp, bool swt,
                                 bool zero_sub_hiddens) {
  if(!src_u) return;

  float_Matrix topk_un_vec;             // for computing kwta
  float_Matrix topk_gp_vec;             // for computing kwta

  // first initialize all vars
  FOREACH_ELEM_IN_GROUP(Layer, lay, layers) {
    if(lay->lesioned()) continue;
    lay->ClearLayerFlag(Layer::PROJECT_WTS_NEXT);
    lay->ClearLayerFlag(Layer::PROJECT_WTS_DONE);
    FOREACH_ELEM_IN_GROUP(Unit, u, lay->units) {
      if(u->lesioned()) continue;
      u->wt_prjn = u->tmp_calc1 = 0.0f;
    }
  }

  // do initial propagation
  for(int g = 0; g < (swt ? src_u->send.size : src_u->recv.size); g++) {
    taOBase* cg = (swt ? (taOBase*)src_u->send.FastEl(g) : (taOBase*)src_u->recv.FastEl(g));
    Projection* prjn = (swt ? ((SendCons*)cg)->prjn : ((RecvCons*)cg)->prjn);
    if(!prjn) continue;
    Layer* slay = (swt ? prjn->layer : prjn->from);

    if(slay->lesioned() || (prjn->from.ptr() == prjn->layer)) continue; // no self prjns!!
    slay->SetLayerFlag(Layer::PROJECT_WTS_NEXT);

    if(swt) {
      SendCons* scg = (SendCons*)cg;
      for(int ci = 0; ci < scg->size; ci++) {
        float wtv = scg->Cn(ci, BaseCons::WT, this);
        Unit* su = scg->Un(ci,this);
        su->wt_prjn += wtv;
        su->tmp_calc1 += 1.0f;  // sum to 1
      }
    }
    else {
      RecvCons* scg = (RecvCons*)cg;
      for(int ci = 0; ci < scg->size; ci++) {
        float wtv = scg->Cn(ci, BaseCons::WT, this);
        Unit* su = scg->Un(ci,this);
        su->wt_prjn += wtv;
        su->tmp_calc1 += 1.0f;  // sum to 1
      }
    }
  }

  // now it is just the same loop until there are no more guys!
  bool got_some = false;
  do {
    got_some = false;
    FOREACH_ELEM_IN_GROUP(Layer, lay, layers) {
      if(lay->lesioned() || !lay->HasLayerFlag(Layer::PROJECT_WTS_NEXT)) continue;

      lay->SetLayerFlag(Layer::PROJECT_WTS_DONE); // we're done!

      topk_un_vec.SetGeom(1, lay->units.leaves);
      // first normalize the weights on this guy
      float abs_max = 0.0f;
      int uidx = 0;
      FOREACH_ELEM_IN_GROUP(Unit, u, lay->units) {
        if(u->lesioned()) continue;
        if(u->tmp_calc1 > 0.0f)
          u->wt_prjn /= u->tmp_calc1;
        abs_max = MAX(abs_max, fabsf(u->wt_prjn));
        topk_un_vec.FastEl_Flat(uidx) = u->wt_prjn;
        uidx++;
      }
      if(abs_max == 0.0f) abs_max = 1.0f;

      if(lay->layer_type == Layer::HIDDEN && lay->units.gp.size > 0 && top_k_gp > 0) {
        // units group version -- only for hidden layers..

        // pick the top k groups in terms of weighting for top-k guys from that group -- not all..
        float k_val = lay->units.gp.size * top_k_un;
        float thr_eff = taMath_float::vec_kwta(&topk_un_vec, (int)k_val, true); // descending

        topk_gp_vec.SetGeom(1, lay->units.gp.size);

        for(int gi=0;gi<lay->units.gp.size;gi++) {
          Unit_Group* ug = (Unit_Group*)lay->units.gp[gi];
          float gp_val = 0.0f;
          FOREACH_ELEM_IN_GROUP(Unit, u, *ug) {
            if(u->lesioned()) continue;
            if(u->wt_prjn > thr_eff) // only for those above threshold
              gp_val += u->wt_prjn;
          }
          topk_gp_vec.FastEl_Flat(gi) = gp_val;
        }

        float gp_thr_eff = taMath_float::vec_kwta(&topk_gp_vec, top_k_gp, true); // descending
        for(int gi=0;gi<lay->units.gp.size;gi++) {
          Unit_Group* ug = (Unit_Group*)lay->units.gp[gi];
          topk_un_vec.SetGeom(1, ug->leaves);

          int uidx = 0;
          FOREACH_ELEM_IN_GROUP(Unit, u, *ug) {
            if(u->lesioned()) continue;
            topk_un_vec.FastEl_Flat(uidx) = u->wt_prjn;
            uidx++;
          }

          float thr_eff = taMath_float::vec_kwta(&topk_un_vec, top_k_un, true); // descending
          FOREACH_ELEM_IN_GROUP(Unit, u, *ug) {
            if(u->lesioned()) continue;
            float prjval = u->wt_prjn;
            u->wt_prjn /= abs_max;      // normalize --
            if((top_k_un > 0 && prjval < thr_eff) ||
               (topk_gp_vec.FastEl_Flat(gi) < gp_thr_eff)) {
              if(zero_sub_hiddens)
                u->wt_prjn = 0.0f; // these are always HIDDEN so no need to check.
              continue;
            }

            bool got = net_project_wts_propagate(this, u, swt);
            got_some |= got;
          }
        }
      }
      else {                                            // flat layer version
        float thr_eff = taMath_float::vec_kwta(&topk_un_vec, top_k_un, true); // descending

        FOREACH_ELEM_IN_GROUP(Unit, u, lay->units) {
          if(u->lesioned()) continue;
          float prjval = u->wt_prjn;
          u->wt_prjn /= abs_max;        // normalize
          if(top_k_un > 0 && prjval < thr_eff) {
            if(lay->layer_type == Layer::HIDDEN && zero_sub_hiddens)
              u->wt_prjn = 0.0f;
            continue; // bail
          }

          bool got = net_project_wts_propagate(this, u, swt);
          got_some |= got;
        }
      }
    }
  } while(got_some);
}

// new monitor is in emergent_project.cc

////////////////////////////////////////////
//      Wizard functions
////////////////////////////////////////////

bool Network::nw_itm_def_arg = false;

BaseSpec_Group* Network::FindMakeSpecGp(const String& nm, bool& nw_itm) {
  BaseSpec_Group* gp = (BaseSpec_Group*)specs.gp.FindName(nm);
  nw_itm = false;
  if(gp == NULL) {
    gp = (BaseSpec_Group*)specs.gp.New(1);
    gp->SetName(nm);
    gp->SigEmitUpdated();
    nw_itm = true;
  }
  return gp;
}

BaseSpec* Network::FindMakeSpec(const String& nm, TypeDef* td, bool& nw_itm) {
  return (BaseSpec*)specs.FindMakeSpec(nm, td, nw_itm);
}

BaseSpec* Network::FindSpecName(const String& nm) {
  BaseSpec* rval = (BaseSpec*)specs.FindSpecName(nm);
  TestError(!rval, "FindSpecName", "could not find spec named:", nm);
  return rval;
}

BaseSpec* Network::FindSpecType(TypeDef* td) {
  BaseSpec* rval = (BaseSpec*)specs.FindSpecType(td);
  TestError(!rval, "FindSpecType", "could not find spec of type:", td->name);
  return rval;
}

Layer* Network::FindMakeLayer(const String& nm, TypeDef* td, bool& nw_itm, const String& alt_nm) {
  return layers.FindMakeLayer(nm, td, nw_itm, alt_nm);
}

Layer_Group* Network::FindMakeLayerGroup(const String& nm, TypeDef* td, bool& nw_itm, const String& alt_nm) {
  return layers.FindMakeLayerGroup(nm, td, nw_itm, alt_nm);
}

Projection* Network::FindMakePrjn(Layer* recv, Layer* send, ProjectionSpec* ps, ConSpec* cs, bool& nw_itm)
{
  nw_itm = false; // default, esp for early return
  Projection* use_prj = NULL;
  int i;
  for(i=0;i<recv->projections.size;i++) {
    Projection* prj = (Projection*)recv->projections[i];
    if(prj->from.ptr() == send) {
      if((ps == NULL) && (cs == NULL)) {
        return prj;
      }
      if((ps) && (prj->spec.spec.ptr() != ps)) {
        use_prj = prj;
        break;
      }
      if((cs) && (prj->con_spec.spec.ptr() != cs)) {
        use_prj = prj;
        break;
      }
      return prj;
    }
  }
  if (use_prj == NULL) {
    nw_itm = true;
    use_prj = (Projection*)recv->projections.NewEl(1);
  }
  use_prj->SetCustomFrom(send);
  if(ps) {
    use_prj->spec.SetSpec(ps);
  }
  if(cs) {
    use_prj->SetConType(cs->min_obj_type);
    use_prj->con_spec.SetSpec(cs);
  }
  return use_prj;
}

Projection* Network::FindMakePrjnAdd(Layer* recv, Layer* send, ProjectionSpec* ps, ConSpec* cs, bool& nw_itm) {
  int i;
  for(i=0;i<recv->projections.size;i++) {
    Projection* prj = (Projection*)recv->projections[i];
    if((prj->from.ptr() == send)
       && ((ps == NULL) || (prj->spec.spec.ptr() == ps) ||
           (prj->spec.spec->InheritsFrom(TA_FullPrjnSpec) &&
            ps->InheritsFrom(TA_FullPrjnSpec)))
       && ((cs == NULL) || (prj->con_spec.spec.ptr() == cs))) {
      nw_itm = false;
      return prj;
    }
  }
  nw_itm = true;
  Projection* prj = (Projection*)recv->projections.NewEl(1);
  prj->SetCustomFrom(send);
  if(ps) {
    prj->spec.SetSpec(ps);
  }
  if(cs) {
    prj->SetConType(cs->min_obj_type);
    prj->con_spec.SetSpec(cs);
  }
  return prj;
}

Projection* Network::FindMakeSelfPrjn(Layer* recv, ProjectionSpec* ps, ConSpec* cs, bool& nw_itm) {
  Projection* use_prj = NULL;
  int i;
  for(i=0;i<recv->projections.size;i++) {
    Projection* prj = (Projection*)recv->projections[i];
    if(prj->from.ptr() == recv) {
      if((ps == NULL) && (cs == NULL)) {
        nw_itm = false;
        return prj;
      }
      if((ps) && (prj->spec.spec.ptr() != ps)) {
        use_prj = prj;
        break;
      }
      if((cs) && (prj->con_spec.spec.ptr() != cs)) {
        use_prj = prj;
        break;
      }
      nw_itm = false;
      return prj;
    }
  }
  nw_itm = true;
  if(use_prj == NULL)
    use_prj = (Projection*)recv->projections.NewEl(1);
  use_prj->from_type = Projection::SELF;
  use_prj->from = recv;
  if(ps)
    use_prj->spec.SetSpec(ps);
  if(cs)
    use_prj->con_spec.SetSpec(cs);
  use_prj->SigEmitUpdated();
  return use_prj;
}

Projection* Network::FindMakeSelfPrjnAdd(Layer* recv, ProjectionSpec* ps, ConSpec* cs, bool& nw_itm) {
  int i;
  for(i=0;i<recv->projections.size;i++) {
    Projection* prj = (Projection*)recv->projections[i];
    if((prj->from.ptr() == recv)
       && ((ps == NULL) || (prj->spec.spec.ptr() == ps))
       && ((cs == NULL) || (prj->con_spec.spec.ptr() == cs))) {
      nw_itm = false;
      return prj;
    }
  }
  nw_itm = true;
  Projection* prj = (Projection*)recv->projections.NewEl(1);
  prj->from_type = Projection::SELF;
  prj->from = recv;
  if(ps)
    prj->spec.SetSpec(ps);
  if(cs)
    prj->con_spec.SetSpec(cs);
  prj->SigEmitUpdated();
  return prj;
}

bool Network::RemovePrjn(Layer* recv, Layer* send, ProjectionSpec* ps, ConSpec* cs) {
  int i;
  for(i=recv->projections.size-1;i>=0;i--) {
    Projection* prj = (Projection*)recv->projections[i];
    if((prj->from.ptr() == send)
       && ((ps == NULL) || (prj->spec.spec.ptr() == ps) ||
           (prj->spec.spec->InheritsFrom(TA_FullPrjnSpec) &&
            ps->InheritsFrom(TA_FullPrjnSpec)))
       && ((cs == NULL) || (prj->con_spec.spec.ptr() == cs))) {
      recv->projections.RemoveEl(prj);
      return true;
    }
  }
  return false;
}

taBase* Network::ChooseNew(taBase* origin) {
  Network* ntwrk = NULL;
  ProjectBase* prj = GET_OWNER(origin, ProjectBase);  // who initiated the choice/new datatable call?
  if(prj) {
    ntwrk = (Network*)prj->networks.New(1);
  }
  return ntwrk;
}

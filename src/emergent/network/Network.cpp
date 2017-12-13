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

#include "Network.h"

#include <Network_Group>
#include <BaseSpec>
#include <Layer>
#include <Projection>
#include <NetView>
#include <NetworkVoxelMapper>
#include <BrainView>
#include <MemberDef>
#include <UserDataItem>
#include <ProjectBase>
#include <ControlPanel>
#include <ClusterRunJob>
#include <DataTable>
#include <taFiler>
#include <SimpleMathSpec>
#include <NetMonitor>
#include <taMath_float>
#include <SpecMemberBase>
#include <taiEdit>

#include "sha3.h"

#include <tabMisc>
#include <taMisc>

eTypeDef_Of(FullPrjnSpec);
eTypeDef_Of(CustomPrjnSpec);

#include <sstream>

#ifdef CUDA_COMPILE
#include "NetworkState_cuda.h"
#endif

TA_BASEFUNS_CTORS_DEFN(NetNetMonitor);

TA_BASEFUNS_CTORS_DEFN(NetStatsSpecs);
TA_BASEFUNS_CTORS_DEFN(NetworkCudaSpec);

TA_BASEFUNS_CTORS_DEFN(NetStateSync);
TA_BASEFUNS_CTORS_DEFN(NetStateSync_List);

TA_BASEFUNS_CTORS_DEFN(Network);

using namespace std;

void NetStateSync_List::ParseTypes_impl(TypeDef* main_td, TypeDef* state_td, int main_off, int state_off,
                                        bool main_to_st) {
  for(int i=0; i < state_td->members.size; i++) {
    MemberDef* smd = state_td->members[i];
    MemberDef* mmd = main_td->members.FindName(smd->name);
    if(!mmd) continue;
    if(smd->HasOption("READ_ONLY")) continue;

    bool mts_opt = smd->HasOption("MAIN");
    bool eff_st = main_to_st;
    if(mts_opt)
      eff_st = true;
    
    if(smd->type->IsAtomic()) {
      NetStateSync* nss = (NetStateSync*)New(1);
      nss->main_off = mmd->GetRelOff() + main_off;
      nss->state_off = smd->GetRelOff() + state_off;
      nss->size = smd->type->size;
      nss->main_md = mmd;
      nss->state_md = smd;
      nss->main_to_state = eff_st;
    }
    else {
      ParseTypes_impl(mmd->type, smd->type, main_off + mmd->GetRelOff(), state_off + smd->GetRelOff(),
                      eff_st);
    }
  }
}

void NetStateSync_List::ParseTypes(TypeDef* main_td, TypeDef* state_td) {
  Reset();
  ParseTypes_impl(main_td, state_td, 0, 0, false);
}

void NetStateSync_List::DoSync(void* main_addr, void* state_addr) {
  for(int i=0; i<size; i++) {
    NetStateSync* nss = FastEl(i);
    void* mna = ((char*)main_addr) + nss->main_off;
    void* sta = ((char*)state_addr) + nss->state_off;
    if(nss->main_to_state) {
      memcpy(sta, mna, nss->size);
    }
    else {
      memcpy(mna, sta, nss->size);
    }
  }
}


taBrainAtlas_List* Network::brain_atlases = NULL;


// every Network type must redefine these

NetworkState_cpp* Network::NewNetworkState() const {
  NetworkState_cpp* nns = new NetworkState_cpp;
  nns->Initialize_net_core();
  return nns;
}

TypeDef* Network::NetworkStateType() const {
  return &TA_NetworkState_cpp;
}
TypeDef* Network::LayerStateType() const {
  return &TA_LayerState_cpp;
}
TypeDef* Network::PrjnStateType() const {
  return &TA_PrjnState_cpp;
}
TypeDef* Network::UnGpStateType() const {
  return &TA_UnGpState_cpp;
}
TypeDef* Network::UnitStateType() const {
  return &TA_UnitState_cpp;
}
TypeDef* Network::ConStateType() const {
  return &TA_ConState_cpp;
}


void Network::Initialize() {
  specs.SetBaseType(&TA_BaseSpec);
  layers.SetBaseType(&TA_Layer);

  if(!brain_atlases)
    brain_atlases = &Network_Group::brain_atlases;

  Initialize_net_core();
  
  main_obj = true;
  net_state = NULL;

  n_threads = taMisc::thread_defaults.n_threads;
  
  flags = (NetFlags)(BUILD_INIT_WTS);
  auto_build = AUTO_BUILD;
  auto_load_wts = NO_AUTO_LOAD;

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

  wt_save_fmt = TEXT;

  max_disp_size.x = 1;
  max_disp_size.y = 1;
  max_disp_size.z = 1;

  max_disp_size2d.x = 1;
  max_disp_size2d.y = 1;

  proj = NULL;

  spec_tables.save_tables = false;     // don't save -- prevents project bloat
  
#ifdef DMEM_COMPILE
  dmem_agg_sum.agg_op = MPI_SUM;
#endif
}

void Network::Destroy() {
  CutLinks();
}

void Network::InitLinks() {
  GetSigLink(); // forces creation, so we track Updates

  BuildNetState();
  
  proj = GET_MY_OWNER(ProjectBase);
  taBase::Own(spec_tables, this);
  taBase::Own(specs, this);
  taBase::Own(param_seqs, this);
  taBase::Own(monitor, this);
  taBase::Own(mon_data, this);
  taBase::Own(layers, this);
  taBase::Own(weights, this);
  taBase::Own(max_disp_size, this);
  taBase::Own(max_disp_size2d, this);

  taBase::Own(stats, this);
  taBase::Own(cuda, this);
  taBase::Own(net_timer_names, this);

  taBase::Own(avg_sse, this);
  taBase::Own(prerr, this);
  taBase::Own(sum_prerr, this);
  taBase::Own(epc_prerr, this);

  taBase::Own(train_time, this);
  taBase::Own(epoch_time, this);
  taBase::Own(group_time, this);
  taBase::Own(trial_time, this);
  taBase::Own(settle_time, this);
  taBase::Own(cycle_time, this);
  taBase::Own(wt_sync_time, this);
  taBase::Own(misc_time, this);

  brain_atlas = brain_atlases->FindNameContains("Talairach"); // default

  monitor.network = this;
  monitor.data = &mon_data;
  mon_data.ClearDataFlag(DataTable::SAVE_ROWS);
  
  ClearNetFlag(BUILT);
  ClearIntact();
  
#ifdef DMEM_COMPILE
  taBase::Own(dmem_trl_comm, this);
  taBase::Own(dmem_agg_sum, this);
  dmem_trl_comm.CommAll();
  dmem_agg_sum.agg_op = MPI_SUM;
#endif

  n_threads = taMisc::thread_defaults.n_threads;
#ifdef CUDA_COMPILE
  n_threads = 1;        // always must be!
#endif
  
  inherited::InitLinks();
}

void Network::CutLinks() {
  if(!owner) return; // already replacing or already dead
#ifdef DMEM_COMPILE
  dmem_trl_comm.FreeComm();
#endif
  state_con_specs.Reset();
  state_unit_specs.Reset();
  state_prjn_specs.Reset();
  state_layer_specs.Reset();
  state_prjns.Reset();
  state_layers.Reset();
  
  RemoveUnits();
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
  monitor.CutLinks();
  mon_data.CutLinks();
  param_seqs.CutLinks();
  specs.CutLinks();
  proj = NULL;
  inherited::CutLinks();
}

void Network::Copy_(const Network& cp) {
  // we just copy structure, not thread-allocated data
  RemoveUnits();

  specs = cp.specs;
  layers = cp.layers;

  auto_build = cp.auto_build;
  auto_load_wts = cp.auto_load_wts;
  auto_load_file = cp.auto_load_file;

  train_mode = cp.train_mode;
  wt_update = cp.wt_update;
  small_batch_n = cp.small_batch_n;
  small_batch_n_eff = cp.small_batch_n_eff;
  stats = cp.stats;

  batch = cp.batch;
  epoch = cp.epoch;
  group = cp.group;
  trial = cp.trial;
  tick = cp.tick;
  cycle = cp.cycle;
  time = cp.time;
  total_trials = cp.total_trials;
  
  group_name = cp.group_name;
  trial_name = cp.trial_name;
  output_name = cp.output_name;

  sse = cp.sse;
  sum_sse = cp.sum_sse;
  avg_sse = cp.avg_sse;
  cnt_err = cp.cnt_err;
  pct_err = cp.pct_err;
  pct_cor = cp.pct_cor;

  cur_cnt_err = cp.cur_cnt_err;

  prerr = cp.prerr;
  sum_prerr = cp.sum_prerr;
  epc_prerr = cp.epc_prerr;

  wt_save_fmt = cp.wt_save_fmt;

  max_disp_size = cp.max_disp_size;
  max_disp_size2d = cp.max_disp_size2d;
}

void Network::UpdatePointersAfterCopy_impl(const taBase& cp) {
  inherited::UpdatePointersAfterCopy_impl(cp);
  ClearIntact();
  SyncSendPrjns();
  UpdatePrjnIdxs();             // fix the recv_idx and send_idx (not copied!)
}


void Network::UpdateAfterEdit_impl(){
  inherited::UpdateAfterEdit_impl();

#ifdef CUDA_COMPILE
  n_threads = 1;        // always must be!
#endif
  
  if(wt_save_fmt == NET_FMT)
    wt_save_fmt = TEXT;

  spec_tables.save_tables = false;     // don't save -- prevents project bloat

//  if(name.empty()) {
//    taMisc::Error("name should not be empty -- this indicates the dreaded null dialog bug!!  triggering div zero now -- please report the trace to Randy!");
//    int zero = 0;
//    String msg = String(1 / zero); // trigger error
//  }

  if(taMisc::is_loading) {
    brain_atlas = brain_atlases->FindName(brain_atlas_name);

    taVersion v704(7, 0, 4);
    if(taMisc::loading_version < v704) { // default prior to 704 is to build
      SetNetFlag(BUILD_INIT_WTS);
    }
    ClearNetFlag(BUILT);
    ClearIntact();
  }
  else {
    if(brain_atlas)
      brain_atlas_name = brain_atlas->name; // for later saving..
    if(n_threads != n_thrs_built) {
      ClearIntact();
    }
  }
}

void Network::ClearIntact() {
  if(taMisc::is_loading) return;
  if(!IsBuiltIntact()) return;
  ClearNetFlag(INTACT);
  SyncNetState();
}


void Network::setStale() {
  if(taMisc::is_loading) return;
  ClearIntact();
  RebuildAllViews();
  // taMisc::DebugInfo("net set stale");
  inherited::setStale();
}

void Network::UpdtAfterNetMod() {
  if(!HasNetFlag(BUILT)) return;
  if(!HasNetFlag(INTACT)) return; // already bad

#ifdef CUDA_COMPILE
  n_threads = 1;        // always must be!
#endif

  if(!net_state) {
    ClearIntact();
    return;
  }
  
  bool units_diff = 
    (net_state->n_thrs_built != n_threads) ||
    (net_state->n_layers_built != state_layers.size) ||
    (net_state->n_prjns_built != state_prjns.size) ||
    (net_state->n_ungps_built != n_ungps_built);

  if(units_diff) {
    ClearIntact();
  }

  // make sure active flags are updated on all connections, e.g., from lesions
  small_batch_n_eff = small_batch_n;
  if(small_batch_n_eff < 1) small_batch_n_eff = 1;
#ifdef DMEM_COMPILE
  DMem_UpdtWtUpdt();
#endif
  SyncAllState();
  SyncPrjnState();
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
  int rval = inherited::Dump_Load_Value(strm, par);
#ifdef DMEM_COMPILE
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
	if(taMisc::undo.debug) {
	  taMisc::Info("NOT saving for Undo network named:", name);
	}
	return 1;
      }
    }
    if(taMisc::undo.debug) {
      taMisc::Info("YES saving for Undo network named:", name);
    }
  }
  return inherited::Dump_Save_impl(strm, par, indent);
}

int Network::Save_strm(ostream& strm, taBase* par, int indent) {
  int rval = inherited::Save_strm(strm, par, indent);
  return rval;
}


////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
//              Below here are run-time computational routines

// notes on these compute loops:
// * ALL unit-level access should go through a thread-specific method that goes right
//   down to the unit level, and same for con-group levels -- do it in a separate pass
// * ConState loops should usually operate on the one that owns the cons -- 
//   check RecvOwnsCons() and proceed accordingly (algo-specific code should know and
//   just do the right thing directly)
// * if layer-level access is required, then do that separately -- and layer level 
//   should generally NOT then do the unit level (per above)
// * Exception to above: if the layer-level is specifically computing layer-level
//   aggregate stats over units, then that is most efficiently done by layer-level


void Network::SyncAllState() {
  SyncNetState();
  SyncLayerState();
}


void Network::SyncNetState() {
  net_state_sync.DoSync((void*)this, (void*)net_state);
  
  // special stuff 
  net_state->net_owner = this;
  net_state->dmem_nprocs = taMisc::dmem_nprocs;
  net_state->dmem_proc = taMisc::dmem_proc;
}

void Network::SyncLayerState() {
  if(!IsBuiltIntact()) return;
  for(int li=0; li< n_layers_built; li++) {
    Layer* lay = StateLayer(li);
    // note: don't check lesioned here b/c could be sending lesion flag!
    SyncLayerState_Layer(lay);
  }
}

void Network::SyncLayerState_Layer(Layer* lay) {
  if(lay->layer_idx < 0) return;
  layer_state_sync.DoSync((void*)lay, (void*)GetLayerState(lay->layer_idx));
}

void Network::SyncPrjnState() {
  if(!IsBuiltIntact()) return;
  for(int li=0; li< n_layers_built; li++) {
    Layer* lay = StateLayer(li);
    // note: don't check lesioned here b/c could be sending lesion flag!
    for(int pi=0; pi < lay->projections.size; pi++) {
      Projection* prjn = lay->projections[pi];
      SyncPrjnState_Prjn(prjn);
    }
  }
}

void Network::SyncPrjnState_Prjn(Projection* prjn) {
  if(prjn->prjn_idx < 0) return;
  PrjnState_cpp* pst = prjn->GetPrjnState(net_state);
  prjn_state_sync.DoSync((void*)prjn, (void*)pst);
}


////////////////////////////////////////
//              Init

void Network::Init_Epoch() {
  if(TestError(!IsBuiltIntact(), "Init_Epoch",
               "Network is not built or is not intact -- must Build first")) {
    return;
  }
  bool got_some = param_seqs.SetParamsAtEpoch(epoch);
  if(got_some) {
    UpdateAllStateSpecs();
  }
  SyncAllState();
}

void Network::Init_InputData() {
  NET_STATE_RUN(NetworkState, Init_InputData());
  // also need to initialize our ext flags -- ext_flags have #MAIN source as apply input is main
  for(int li=0; li< n_layers_built; li++) {
    Layer* lay = StateLayer(li);
    if(lay->lesioned()) continue;
    lay->ext_flag = Layer::NO_EXTERNAL;
  }
  SyncLayerState();
}

void Network::Init_Acts() {
  if(TestError(!IsBuiltIntact(), "Trial_Acts",
               "Network is not built or is not intact -- must Build first")) {
    return;
  }
  NET_STATE_RUN(NetworkState, Init_Acts());
}

void Network::Init_dWt(){
  NET_STATE_RUN(NetworkState, Init_dWt());
}

void Network::Init_Weights() {
  // do lots of checking here to make sure, cuz often 1st thing that happens
  // NOTE: this will typically be nested inside a gui check
  if (!CheckConfig(false)) return;

  taMisc::Busy();

  monitor.UpdateDataTable();

  SyncAllState();
  UpdateAllStateSpecs();
  
  NET_STATE_RUN(NetworkState, Init_Weights());
  Init_Counters_impl();         // int our counters -- they are copied to state so we need to init
  Init_Timers();                // and our timers..

  Init_Weights_AutoLoad();

  SyncAllState();
  
  UpdateAllViews();

  param_seqs.SetParamsAtEpoch(0);
  
  taMisc::DoneBusy();
}


void Network::Init_Weights_renorm() {
  NET_STATE_RUN(NetworkState, Init_Weights_renorm());
}

void Network::Init_Weights_post() {
  NET_STATE_RUN(NetworkState, Init_Weights_post());
}

void Network::Init_Weights_AutoLoad() {
  for(int i=0; i < weights.size; i++) {
    Weights* wts = weights[i];
    if(wts->auto_init && wts->HasWeights(false)) {
      wts->WeightsToNet();
      break;
    }
  }
}

void Network::Init_Metrics() {  // not called -- Init_Weights on state calls its version
  NET_STATE_RUN(NetworkState, Init_Metrics());
  Init_Counters_impl();         // get our own init'd anyway
  Init_Timers();
  SyncAllState();
}

void Network::Init_Counters() { // not called usually -- only if needed by program
  Init_Counters_impl();         // get our own init'd anyway
  NET_STATE_RUN(NetworkState, Init_Counters());
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


////////////////////////////////////////
//              Compute

void Network::Compute_Netin() {
  NET_STATE_RUN(NetworkState, Compute_Netin());
}

void Network::Send_Netin() {
  NET_STATE_RUN(NetworkState, Send_Netin());
}

void Network::Compute_Act() {
  NET_STATE_RUN(NetworkState, Compute_Act());
}

void Network::Compute_NetinAct() {
  NET_STATE_RUN(NetworkState, Compute_NetinAct());
}

void Network::Compute_dWt() {
  NET_STATE_RUN(NetworkState, Compute_dWt());
}

bool Network::Compute_Weights_Test(int trial_no) {
  if(TestError(!IsBuiltIntact(), "Compute_Weights_Test",
               "Network is not built or is not intact -- must Build first")) {
    return false;
  }
  return net_state->Compute_Weights_Test_impl(trial_no); // in NetworkState_core
}

void Network::Compute_Weights() {
#ifdef DMEM_COMPILE
  DMem_SumDWts(dmem_trl_comm.comm);
#endif
  NET_STATE_RUN(NetworkState, Compute_Weights());

  SaveWeights_ClusterRunTerm();
}

void Network::Compute_SSE(bool unit_avg, bool sqrt) {
  NET_STATE_RUN(NetworkState, Compute_SSE(unit_avg, sqrt));
}

void Network::Compute_PRerr() {
  NET_STATE_RUN(NetworkState, Compute_PRerr());
}

void Network::Compute_TrialStats() {
  NET_STATE_RUN(NetworkState, Compute_TrialStats());
  SyncAllState();
}

void Network::DMem_ShareTrialData(DataTable* dt, int n_rows) {
#ifdef DMEM_COMPILE
  dt->DMem_ShareRows(dmem_trl_comm.comm, n_rows);
#endif
}

void  Network::Compute_EpochSSE() {
  NET_STATE_RUN(NetworkState, Compute_EpochSSE());
}  

void  Network::Compute_EpochPRerr() {
  NET_STATE_RUN(NetworkState, Compute_EpochPRerr());
}

void Network::Compute_EpochStats() {
#ifdef DMEM_COMPILE
  DMem_ComputeAggs(dmem_trl_comm.comm);
#endif

  NET_STATE_RUN(NetworkState, Compute_EpochStats());
  SyncAllState();
  SyncPrjnState();

  SaveWeights_ClusterRunCmd();  // check for cluster commands!
}


//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
//      Build etc below here


String Network::GetUnitStatePath(UnitState_cpp* unit) {
  if(!IsBuiltIntact()) return _nilString;
  Layer* lay = StateLayer(unit->own_lay_idx);
  if(!lay) return _nilString;
  String path = lay->GetPath(this);
  path << "[" << unit->lay_un_idx << "]";
  return path;
}

UnitState_cpp* Network::GetUnitStateFromPath(const String& path) {
  if(!IsBuiltIntact()) return NULL;
  String lay_path = path.before('[',-1);
  String un_path = path.after('[',-1);
  un_path = un_path.before(']',-1);
  MemberDef* md;
  Layer* lay = (Layer*)FindFromPath(lay_path, md);
  if(!lay) {
    return NULL;
  }
  int un_idx = un_path.toInt();
  if(un_idx >= 0) {
    return lay->GetUnitStateSafe(net_state, un_idx);
  }
  return NULL;
}

void Network::BuildNetState() {
  if(prjn_state_sync.size == 0 && layers.leaves > 0) {
    Layer* lay = layers.Leaf(0);
    if(lay) {
      prjn_state_sync.ParseTypes(lay->projections.el_typ, PrjnStateType());
    }
  }
  if(net_state) {
    net_state->threads.InitState(n_threads, net_state);
    SyncNetState();
    return;
  }
  net_state = NewNetworkState(); // network state has virtual pointers, needs new

#ifdef CUDA_COMPILE
  cuda_state = NewCudaState();
  cuda_state->net_owner = this;
  cuda_state->CudaInit();
#endif

  net_state->net_owner = this;
  net_state->threads.InitState(n_threads, net_state);
  net_state_sync.ParseTypes(GetTypeDef(), NetworkStateType());
  layer_state_sync.ParseTypes(layers.el_typ, LayerStateType());

#ifdef DMEM_COMPILE
  DMem_InitAggs();              // this references the network state, so do it here..
#endif
  
  SyncNetState();
}


void Network::Build() {
  taMisc::Busy();
  ++taMisc::no_auto_expand; // c'mon...!!! ;)
  StructUpdate(true);


  net_state->FreeStateMem();     // free any and all existing memory!  must still have built params from before!

  specs.ResetAllSpecIdxs();
  
  BuildNetState();
  CheckSpecs();
  SyncSendPrjns();
  UpdatePrjnIdxs();

  BuildIndexesSizes();
  BuildSpecs();
  
  SyncNetState();

  BuildLayerUnitState();
  SyncNetState();
  
  BuildConState();
  Connect();

  n_units = net_state->n_units;
  n_cons = net_state->n_cons;
  max_prjns = net_state->max_prjns;
  
  SyncNetState();
  BuildSendNetinTmp();
  
  SetNetFlag(BUILT);
  SetNetFlag(INTACT);

  SyncLayerState();
  SyncPrjnState();
  
  if(taMisc::gui_active)	// only when gui is active..
    AssignVoxels();

  if (taMisc::is_post_loading) {
    specs.RestorePanels();
    layers.RestorePanels();
  }
  
  Compute_PrjnDirections();
  
  if(HasNetFlag(BUILD_INIT_WTS)) {
    Init_Weights();
  }

  SyncNetState();
  SyncLayerState();
  SyncPrjnState();

#ifdef CUDA_COMPILE
  // one-stop call does everything..
  cuda_state->BuildCudaFmCpp(net_state);
#endif  

  monitor.UpdateDataTable();
  
  StructUpdate(false);
  --taMisc::no_auto_expand;
  taMisc::DoneBusy();
}

void Network::CheckSpecs() {
  FOREACH_ELEM_IN_GROUP(Layer, lay, layers) {
    lay->CheckSpecs();
  }
}

void Network::BuildIndexesSizes() {
  ++taMisc::no_auto_expand;
  StructUpdate(true);
  LayerPos_RelPos();

  state_con_specs.Reset();
  state_unit_specs.Reset();
  state_prjn_specs.Reset();
  state_layer_specs.Reset();

  state_prjns.Reset();
  state_layers.Reset();

  Layer_Group* last_laygp = NULL;
  int last_laygp_lay0_idx = -1;
  int last_laygp_n = 0;

  n_ungps_built = 0;
  n_units_built = 1;            // count 'em up -- first one is null
  
  for(int i=0;i<layers.leaves; i++) {
    Layer* lay = (Layer*)layers.Leaf(i);
    lay->n_units_built = 0;
    lay->un_geom.UpdateAfterEdit_NoGui(); // make sure n is accurate!
    lay->gp_geom.UpdateAfterEdit_NoGui();
    if(lay->lesioned()) {
      lay->layer_idx = -1;
      continue;
    }
    lay->layer_idx = state_layers.size;
    lay->n_units = lay->flat_geom_n;
    state_layers.Add(lay);
    lay->ungp_idx = n_ungps_built; // always add the main layer group first!
    n_ungps_built++;
    n_units_built += lay->n_units; // don't factor in lesions yet!

    Layer_Group* lg = GET_OWNER(lay,Layer_Group);
    if(lg != &layers) {
      if(lg == last_laygp && last_laygp_lay0_idx >= 0) {
        lay->laygp_lay0_idx = last_laygp_lay0_idx;
        last_laygp_n++;
      }
      else {
        if(last_laygp && last_laygp_lay0_idx >= 0) {
          for(int li=0; li < last_laygp->leaves; li++) {
            Layer* sl = last_laygp->Leaf(li);
            if(sl->lesioned()) continue;
            sl->laygp_n = last_laygp_n;
          }
        }
        last_laygp = lg;
        last_laygp_lay0_idx = lay->layer_idx;
        last_laygp_n = 1;
        lay->laygp_lay0_idx = lay->layer_idx;
      }
    }
    else {
      if(last_laygp && last_laygp_lay0_idx >= 0) {
        for(int li=0; li < last_laygp->leaves; li++) {
          Layer* sl = last_laygp->Leaf(li);
          if(sl->lesioned()) continue;
          sl->laygp_n = last_laygp_n;
        }
      }
      last_laygp = NULL;
      last_laygp_lay0_idx = -1;
      lay->laygp_n = 0;
      lay->laygp_lay0_idx = -1;
    }
    
    // LayerSpec and UnitSpec
    LayerSpec* ls = lay->GetMainLayerSpec();
    if(ls) {
      TestError(!ls->InheritsFrom(&TA_LayerSpec), "Build",
                "layer spec on layer:", lay->name, "is not a layer spec:", ls->name,
                "type:", ls->GetTypeDef()->name);
      if(state_layer_specs.AddUnique(ls)) {
        ls->spec_idx = state_layer_specs.size-1;
      }
      lay->spec_idx = ls->spec_idx;
    }
    else {
      lay->spec_idx = -1;
    }
          
    UnitSpec* us = lay->GetMainUnitSpec();
    if(us) {
      TestError(!us->InheritsFrom(&TA_UnitSpec), "Build",
                "unit spec on layer:", lay->name, "is not a unit spec:", us->name,
                "type:", us->GetTypeDef()->name);
      if(state_unit_specs.AddUnique(us)) {
        us->spec_idx = state_unit_specs.size-1;
      }
      lay->unit_spec_idx = us->spec_idx;
      if(us->bias_spec.SPtr()) {
        ConSpec* bs = us->bias_spec.SPtr();
        TestError(!bs->InheritsFrom(&TA_ConSpec), "Build",
                  "bias spec on unit spec:", us->name, "is not a con spec:", bs->name,
                  "type:", bs->GetTypeDef()->name);
        if(state_con_specs.AddUnique(bs)) {
          bs->spec_idx = state_con_specs.size-1;
        }
        us->bias_spec_idx = bs->spec_idx;
      }
      else {
        us->bias_spec_idx = -1;
      }
    }
    else {
      lay->unit_spec_idx = -1;
    }
    
    // sub-unit-groups
    if(lay->unit_groups && lay->gp_geom.n > 0) {
      lay->n_ungps = lay->gp_geom.n;
      n_ungps_built += lay->gp_geom.n;
    }
    else {
      lay->n_ungps = 0;
    }
    
    // projections and conspecs
    if(lay->n_recv_prjns > 0) {
      lay->prjn_start_idx = state_prjns.size;
      for(int j=0; j < lay->projections.size; j++) {
        Projection* prjn = lay->projections[j];
        if(!prjn->MainIsActive()) {
          prjn->prjn_idx = -1;
          continue;
        }
        prjn->prjn_idx = state_prjns.size;
        state_prjns.Add(prjn);
        // taMisc::Info("added recv prjn:", lay->name, prjn->name);

        ProjectionSpec* ps = prjn->GetMainPrjnSpec();
        if(ps) {
          TestError(!ps->InheritsFrom(&TA_ProjectionSpec), "Build",
                    "prjn spec on prjn:", prjn->name, "is not a prjn spec:", ps->name,
                    "type:", ps->GetTypeDef()->name);
          if(state_prjn_specs.AddUnique(ps)) {
            ps->spec_idx = state_prjn_specs.size-1;
          }
          prjn->spec_idx = ps->spec_idx;
        }
        else {
          prjn->spec_idx = -1;
        }

        ConSpec* cs = prjn->GetMainConSpec();
        if(cs) {
          TestError(!cs->InheritsFrom(&TA_ConSpec), "Build",
                    "con spec on prjn:", prjn->name, "is not a con spec:", cs->name,
                    "type:", cs->GetTypeDef()->name);
          if(state_con_specs.AddUnique(cs)) {
            cs->spec_idx = state_con_specs.size-1;
          }
        }
      }
    }
    else {
      lay->prjn_start_idx = -1;
      for(int j=0; j < lay->projections.size; j++) {
        Projection* prjn = lay->projections[j];
        prjn->prjn_idx = -1;
      }
    }
  }
  // could have ended on a layer group..
  if(last_laygp && last_laygp_lay0_idx >= 0) {
    for(int li=0; li < last_laygp->leaves; li++) {
      Layer* sl = last_laygp->Leaf(li);
      if(sl->lesioned()) continue;
      sl->laygp_n = last_laygp_n;
    }
  }

  StructUpdate(false);
  --taMisc::no_auto_expand;
}


void Network::BuildSpecs() {
  SetSpecSizes(state_layer_specs.size, state_prjn_specs.size, state_unit_specs.size, state_con_specs.size);
  net_state->SetSpecSizes(state_layer_specs.size, state_prjn_specs.size, state_unit_specs.size, state_con_specs.size);
  net_state->AllocSpecMem();

  // todo: CUDA TOO

  for(int i=0; i < state_con_specs.size; i++) {
    ConSpec* ls = StateConSpec(i);
    net_state->con_specs[i] = net_state->NewConSpec(ls->GetStateSpecType());
    // note: cannot use UpdateStateSpecs because it has check for build intact..
    ls->CopyToState(net_state->con_specs[i], net_state->GetStateSuffix());
#ifdef CUDA_COMPILE
    ls->CopyToState(cuda_state->con_specs[i], cuda_state->GetStateSuffix());
#endif
  }
  for(int i=0; i < state_unit_specs.size; i++) {
    UnitSpec* ls = StateUnitSpec(i);
    net_state->unit_specs[i] = net_state->NewUnitSpec(ls->GetStateSpecType());
    ls->CopyToState(net_state->unit_specs[i], net_state->GetStateSuffix());
#ifdef CUDA_COMPILE
    ls->CopyToState(cuda_state->unit_specs[i], cuda_state->GetStateSuffix());
#endif
  }
  for(int i=0; i < state_prjn_specs.size; i++) {
    ProjectionSpec* ls = StatePrjnSpec(i);
    net_state->prjn_specs[i] = net_state->NewPrjnSpec(ls->GetStateSpecType());
    ls->CopyToState(net_state->prjn_specs[i], net_state->GetStateSuffix());
#ifdef CUDA_COMPILE
    ls->CopyToState(cuda_state->prjn_specs[i], cuda_state->GetStateSuffix());
#endif
  }
  for(int i=0; i < state_layer_specs.size; i++) {
    LayerSpec* ls = StateLayerSpec(i);
    net_state->layer_specs[i] = net_state->NewLayerSpec(ls->GetStateSpecType());
    ls->CopyToState(net_state->layer_specs[i], net_state->GetStateSuffix());
#ifdef CUDA_COMPILE
    ls->CopyToState(cuda_state->layer_specs[i], cuda_state->GetStateSuffix());
#endif
  }
}

void Network::UpdateAllStateSpecs() {
  UpdateAllStateLayerSpecs();
  UpdateAllStatePrjnSpecs();
  UpdateAllStateUnitSpecs();
  UpdateAllStateConSpecs();
}

void Network::UpdateAllStateLayerSpecs() {
  for(int i=0; i < n_layer_specs_built; i++) {
    LayerSpec* ls = StateLayerSpec(i);
    ls->UpdateStateSpecs();
  }
}

void Network::UpdateAllStatePrjnSpecs() {
  for(int i=0; i < n_prjn_specs_built; i++) {
    ProjectionSpec* ls = StatePrjnSpec(i);
    ls->UpdateStateSpecs();
  }
}

void Network::UpdateAllStateUnitSpecs() {
  for(int i=0; i < n_unit_specs_built; i++) {
    UnitSpec* ls = StateUnitSpec(i);
    ls->UpdateStateSpecs();
  }
}

void Network::UpdateAllStateConSpecs() {
  for(int i=0; i < n_con_specs_built; i++) {
    ConSpec* ls = StateConSpec(i);
    ls->UpdateStateSpecs();
  }
}

void Network::BuildLayerUnitState() {
  BuildStateSizes();
  net_state->AllocLayerUnitMem();
  BuildLayerState_FromNet();
  net_state->BuildUnitState();
}

void Network::BuildStateSizes() {
  TypeDef* lay_typ = LayerStateType();
  TypeDef* prjn_typ = PrjnStateType();
  TypeDef* ungp_typ = UnGpStateType();
  TypeDef* un_typ = UnitStateType();
  TypeDef* con_typ = ConStateType();

  SetStateSizes
    (n_threads, lay_typ->size, prjn_typ->size, ungp_typ->size, un_typ->size, con_typ->size,
     state_layers.size, state_prjns.size, n_ungps_built, n_units_built);

  net_state->SetStateSizes
    (n_threads, lay_typ->size, prjn_typ->size, ungp_typ->size, un_typ->size, con_typ->size,
     state_layers.size, state_prjns.size, n_ungps_built, n_units_built);
}

void Network::BuildLayerState_FromNet() {
  // todo: we could save a JSON format network description with all the key info and use
  // that to build this information in a standalone impl

  // at this point all the basic layer and unit level memory has been allocated on net_state

  int un_idx = 1;
  int ungp_un_idx = 1;
  int send_prjn_idx = 0;
  
  for(int li=0; li < n_layers_built; li++) {
    LayerState_cpp* lst = GetLayerState(li);
    Layer* lay = StateLayer(li);
    lay->units_flat_idx = un_idx;
    lay->n_units_built = lay->n_units;
    lst->Initialize_lay_core
      (li, lay->laygp_lay0_idx, lay->laygp_n, lay->units_flat_idx, lay->ungp_idx, lay->n_units,
       lay->n_ungps, lay->prjn_start_idx, lay->spec_idx, lay->unit_spec_idx,
       lay->n_recv_prjns, lay->n_send_prjns, lay->flags, (LayerState_cpp::LayerType)lay->layer_type);

    lst->Initialize_lay_geom
      (lay->un_geom_x, lay->un_geom_y, lay->un_geom_n, lay->gp_geom_x, lay->gp_geom_y, lay->gp_geom_n,
       lay->flat_geom_x, lay->flat_geom_y, lay->flat_geom_n, lay->gp_spc_x, lay->gp_spc_y);

    lst->net_state = net_state;
    lay->net_state = net_state;

    lst->SetLayerName(lay->name);
    
#ifdef DMEM_COMPILE
    lay->DMem_InitAggs(); // this references the network state, so do it here..
#endif
    
    LayerSpec* ls = lay->GetMainLayerSpec();
    if(ls) {
      ls->Init_LayerState(lst, net_state);
    }

    net_state->ungp_lay_idxs[lay->ungp_idx] = lay->layer_idx;
    UnGpState_cpp* lugst = GetUnGpState(lay->ungp_idx);
    lugst->Initialize_core(lay->ungp_idx, lay->layer_idx, -1, ungp_un_idx, lay->n_units);

    if(lay->n_ungps > 0) {      // has sub unit groups
      for(int j=0; j < lay->n_ungps; j++) {
        int ungp_idx = lay->ungp_idx + 1 + j;
        net_state->ungp_lay_idxs[ungp_idx] = lay->layer_idx;
        UnGpState_cpp* ugst = GetUnGpState(ungp_idx);
        ugst->Initialize_core(ungp_idx, lay->layer_idx, j, ungp_un_idx, lay->un_geom.n);
        for(int ui=0; ui < ugst->n_units; ui++) {
          net_state->units_ungps[ungp_un_idx++] = ungp_idx;
        }
      }
    }
    else {
      for(int ui=0; ui < lst->n_units; ui++) {
        net_state->units_ungps[ungp_un_idx++] = lay->ungp_idx;
      }
    }

    // lookup table from units to layers
    for(int ui=0; ui < lst->n_units; ui++) {
      net_state->units_lays[un_idx++] = lst->layer_idx;
    }

    // sending projection list
    if(lay->n_send_prjns > 0) {
      lay->send_prjn_start_idx = send_prjn_idx;
      lst->send_prjn_start_idx = send_prjn_idx;

      for(int j=0; j < lay->send_prjns.size; j++) {
        Projection* prjn = lay->send_prjns[j];
        if(!prjn->MainIsActive()) continue;
        // taMisc::Info("adding send prjn:", prjn->layer->name, prjn->name);
        if(send_prjn_idx >= n_prjns_built) { // this should not happen.. just checking..
          taMisc::Error("programmer error: sending prjn idx > number of projections built!");
        }
        net_state->lay_send_prjns[send_prjn_idx++] = prjn->prjn_idx;
      }
    }
    else {
      lay->send_prjn_start_idx = -1;
      lst->send_prjn_start_idx = -1;
    }
  }
  
  for(int i=0; i < n_prjns_built; i++) {
    PrjnState_cpp* pst = GetPrjnState(i);
    Projection* prjn = StatePrjn(i);
    Layer* recv_lay = prjn->layer;
    Layer* send_lay = prjn->from;
    ConSpec* cs = prjn->con_spec;
    ProjectionSpec* ps = prjn->spec;
    prjn->recv_lay_idx = recv_lay->layer_idx;
    prjn->send_lay_idx = send_lay->layer_idx;
    pst->Initialize_core
      (prjn->off, prjn->MainNotActive(), i, recv_lay->layer_idx, send_lay->layer_idx, prjn->send_prjn_idx,
       prjn->recv_idx, prjn->send_idx, prjn->spec_idx, cs->spec_idx, prjn->con_type->members.size);
    ps->Init_PrjnState(pst, net_state);


#ifdef DMEM_COMPILE
    prjn->DMem_InitAggs(); // this references the network state, so do it here..
#endif
  }
}

void Network::BuildConState() {
  net_state->BuildConState();
}

void Network::Connect() {
  ++taMisc::no_auto_expand;
  StructUpdate(true);

  CheckSpecs();
  RemoveCons();

  net_state->Connect();         // does all the heavy lifting
  // todo: as for all Build-level code, need to do cuda in parallel (at least for now)
  
  UpdtAfterNetMod();

  StructUpdate(false);
  --taMisc::no_auto_expand;
}

void Network::BuildSendNetinTmp() {
  net_state->BuildSendNetinTmp();
}

void Network::UnBuild() {
  RemoveUnits();
}

bool Network::AutoBuild() {
  if(auto_build == NO_BUILD) return false;
  
  if(taMisc::gui_active && (auto_build == PROMPT_BUILD)) {
    int chs = taMisc::Choice("Build network: " + name, "Yes", "No");
    if(chs == 1) return false;
  }
  taMisc::Info("Network:",name,"auto building");
  Build();
  
  switch(auto_load_wts) {
    case NO_AUTO_LOAD:
      return true;
    case AUTO_LOAD_WTS_0:
      LoadFmFirstWeights(false);  // not quiet
      return true;
      break;
    case AUTO_LOAD_FILE:
      if(TestWarning(auto_load_file.empty(), "AutoBuild",
                     "auto_load_file is empty -- cannot auto-load weights"))
        return true;
      LoadWeights(auto_load_file);
      return true;
      break;
    default:
      taMisc::Warning("Programmer error: Network::AutoBuild - Should not reach default case in this switch");
      return true;
  }
}
  

String Network::MemoryReport(bool print) {
  if(!HasNetFlag(BUILT)) {
    String rval = "Network not built yet!";
    if(print)
      taMisc::Info(rval);
    return rval;
  }
  
  String constr;
  constr.convert((float)net_state->n_cons);

  int64_t recv_cons_tot = 0;
  int64_t send_cons_tot = 0;
  int     own_cons_max_size = 0;
  int64_t own_cons_tot_size = 0;
  int64_t own_cons_tot_size_nonshared = 0;
  int     own_cons_avg_size = 0;
  for(int i=0; i< n_thrs_built; i++) {
    recv_cons_tot += net_state->thrs_recv_cons_cnt[i];
    send_cons_tot += net_state->thrs_send_cons_cnt[i];
    own_cons_max_size = MAX(net_state->thrs_own_cons_max_size[i], own_cons_max_size);
    own_cons_tot_size += net_state->thrs_own_cons_tot_size[i];
    own_cons_tot_size_nonshared += net_state->thrs_own_cons_tot_size_nonshared[i];
    own_cons_avg_size += net_state->thrs_own_cons_avg_size[i];
  }

  own_cons_avg_size = (int)((float)own_cons_avg_size / 4.0f);

  int64_t recv_cons_mem = sizeof(float) * recv_cons_tot;
  int64_t send_cons_mem = sizeof(float) * send_cons_tot;
  
  int64_t congp_mem = (net_state->n_recv_cgps + net_state->n_send_cgps) * con_state_size;
  int64_t unit_mem = n_units_built * unit_state_size;

  int64_t mem_tot = recv_cons_mem + send_cons_mem + congp_mem + unit_mem;

  String report = name + " memory report:\n";
  report << "number of units:            " << n_units_built << "\n"
         << "    bytes per unitvar:      " << unit_state_size << "\n"
         << "    total unit memory:      " << taMisc::GetSizeString
    (unit_mem) << "\n"
         << "number of recv con groups:  " << net_state->n_recv_cgps << "\n"
         << "number of send con groups:  " << net_state->n_send_cgps << "\n"
         << "    bytes per con group:    " << con_state_size << "\n"
         << "    total con group memory: " << taMisc::GetSizeString
    (congp_mem) << "\n"
         << "number of connections:      " << constr << "\n"
         << "    bytes per con+idx:      " << (net_state->thrs_own_cons_max_vars[0]+1) *
    sizeof(float) << "\n"
         << "    total con memory:       " << taMisc::GetSizeString
    (recv_cons_mem + send_cons_mem) << "\n" 
         << "       recv cons:           " << taMisc::GetSizeString
    (recv_cons_mem) << "\n" 
         << "       send cons:           " << taMisc::GetSizeString
    (send_cons_mem) << "\n"
         << "grand total memory:         " << taMisc::GetSizeString
    (mem_tot) << "\n\n"
         << "owned connection statistics:\n"
         << "    max_size:               " << (own_cons_max_size) << "\n"
         << "    avg_size:               " << (own_cons_avg_size) << "\n"
         << "    pct_vector_chunked:     " << (net_state->pct_cons_vec_chunked) << "\n"
         << "    total_size:             " << (own_cons_tot_size) << "\n"
         << "      total_nonshared:      " << (own_cons_tot_size_nonshared) << "\n"
         << "      total_shared:         " << (own_cons_tot_size -
                                               own_cons_tot_size_nonshared) << "\n";
#ifdef CUDA_COMPILE
  report << "\n\nCUDA specific memory:\n"
         << "    unit_spec_mem:          " << taMisc::GetSizeString
    (cuda_net->unit_spec_mem_tot) << "\n"
         << "    con_spec_mem:           " << taMisc::GetSizeString
    (cuda_net->con_spec_mem_tot) << "\n";
#endif    
  
  if(print)
    taMisc::Info(report);
  return report;
}

void Network::TimingReportInitNames() {
  net_timer_names.SetSize(net_state->n_net_timers_built);
  net_timer_names[NetworkState_cpp::NT_NETIN] = "Net_Input ";
  net_timer_names[NetworkState_cpp::NT_NETIN_INTEG] = "Net_Input_Integ";
  net_timer_names[NetworkState_cpp::NT_ACT] = "Activation";
  net_timer_names[NetworkState_cpp::NT_DWT] = "Weight_Change";
  net_timer_names[NetworkState_cpp::NT_WT] = "Weight_Updt";
  net_timer_names[NetworkState_cpp::NT_DMEM_WT_SYNC] = "DMem_Wt_Sync";
  net_timer_names[NetworkState_cpp::NT_BUILD] = "Build_Net ";
  net_timer_names[NetworkState_cpp::NT_CONNECT] = "Connect_Net";
}

void Network::StartTiming() {
  if(TestError(!IsBuiltIntact(), "StartTiming",
               "can only record timing data after network is built")) {
    return;
  }
  SetNetFlag(TIMING);
  net_state->threads.StartTimers();
  SigEmitUpdated();
}

void Network::StopTiming() {
  if(!HasNetFlag(TIMING)) return;
  if(TestError(!IsBuiltIntact(), "StopTiming",
               "can only turn on / off timing data after network is built")) {
    return;
  }
  ClearNetFlag(TIMING);
  net_state->threads.EndTimers(false);
  SigEmitUpdated();
}

static String pct_val_out(float val, float sum) {
  String rval;
  rval = taMisc::FormatValue(val, 7, 3) + " " 
    + taMisc::FormatValue(100.0f * (val / sum), 7, 3);
  return rval;
}

String Network::TimingReport(DataTable* dt, bool print) {
  if(!HasNetFlag(BUILT)) {
    String rval = "Network not built yet!";
    if(print)
      taMisc::Info(rval);
    return rval;
  }

  StopTiming();

  if(!dt) {
    dt = proj->GetNewAnalysisDataTable("TimingReport_" + name, true);
  }
  
  TimingReportInitNames();
  String report = name + " timing report:\n";
  report << "function  \ttime     percent \n";

  dt->StructUpdate(true);
  int idx;
  DataCol* thc = dt->FindMakeColName("thread", idx, VT_INT);
  DataCol* stat = dt->FindMakeColName("stat", idx, VT_STRING);
  DataCol* rca = dt->FindMakeColName("run_avg", idx, VT_FLOAT);
  DataCol* rcs = dt->FindMakeColName("run_sum", idx, VT_FLOAT);

  float rescale_val = 1.0e6;        // how many microseconds
  
  float tot_time = 0.0f;
  
  for(int tt = 0; tt < net_state->n_net_timers_built; tt++) {
    TimeUsedHR_cpp* sum_tu = net_state->GetNetTimer(tt, n_thrs_built); // net sum
    sum_tu->avg_used.ResetSum();
    for(int thr_no = 0; thr_no < n_thrs_built; thr_no++) {
      TimeUsedHR_cpp* tu = net_state->GetNetTimer(tt, thr_no);
      sum_tu->avg_used.IncrementAvg(tu->avg_used.sum);

      dt->AddBlankRow();
      thc->SetValAsInt(thr_no, -1);
      stat->SetValAsString(net_timer_names[tt], -1);
      rca->SetValAsFloat(tu->avg_used.avg * rescale_val, -1);
      rcs->SetValAsFloat(tu->avg_used.sum, -1);
    }
    tot_time += sum_tu->avg_used.avg;

    dt->AddBlankRow();
    thc->SetValAsInt(-1, -1);
    stat->SetValAsString(net_timer_names[tt], -1);
    rca->SetValAsFloat(sum_tu->avg_used.avg, -1);
    rcs->SetValAsFloat(sum_tu->avg_used.sum, -1);
  }
  
  for(int tt = 0; tt < net_state->n_net_timers_built; tt++) {
    TimeUsedHR_cpp* sum_tu = net_state->GetNetTimer(tt, n_thrs_built); // net sum
    report << net_timer_names[tt] << "\t"
           << pct_val_out(sum_tu->avg_used.avg, tot_time) << "\n";
  }

  report << "    total:\t" << taMisc::FormatValue(tot_time, 7, 3) << "\n\n"
         <<  net_state->threads.TimersReport();
    ;
    
  dt->StructUpdate(false);

  if(print)
    taMisc::Info(report);
  return report;
}

bool Network::CheckBuild(bool quiet) {
  if(!HasNetFlag(BUILT)) {
    if(!quiet) {
      taMisc::CheckError("Network:",GetName(),
                         "has not been built yet -- run the Build command");
    }
    return false;
  }
  if(!HasNetFlag(INTACT)) {
    if(!quiet) {
      taMisc::CheckError("Network:",GetName(),
                         "has been modified in incompatible ways since it was last built -- please run the Build command again");
    }
    return false;
  }
  return true;
}

void Network::CheckThisConfig_impl(bool quiet, bool& rval) {
  //NOTE: slightly non-standard, because we bail on first detected issue
  if (!CheckBuild(quiet)) { rval = false; return; }
  UpdtAfterNetMod();            // just to be sure..
  inherited::CheckThisConfig_impl(quiet, rval);
}

void Network::CheckChildConfig_impl(bool quiet, bool& rval) {
  inherited::CheckChildConfig_impl(quiet, rval);
  specs.CheckConfig(quiet, rval); //note: this checks the specs themselves, not objs
  layers.CheckConfig(quiet, rval);
}

void Network::SyncSendPrjns() {
  FOREACH_ELEM_IN_GROUP(Layer, lay, layers) {
    lay->SyncSendPrjns();
  }
  // two iterations are sometimes needed!
  FOREACH_ELEM_IN_GROUP(Layer, lay, layers) {
    lay->SyncSendPrjns();
  }
}

void Network::UpdatePrjnIdxs() {
  FOREACH_ELEM_IN_GROUP(Layer, lay, layers) {
    lay->UpdatePrjnIdxs();
  }
}


#if 0 // turn off when not in need for debugging

bool Network::CompareNetThrVal_int
(Network* oth_net, const String& nm, const int our, const int their) {
  if(our != their) {
    taMisc::Info("CompareNetThrVals int diff net:", name, "vs. oth_net:",
                 oth_net->name, "var:", nm, "our: " + String(our) + "their:" + String(their));
    return false;
  }
  return true;
}

bool Network::CompareNetThrVal_int64
(Network* oth_net, const String& nm, const int64_t our, const int64_t their) {
  if(our != their) {
    taMisc::Info("CompareNetThrVals int64 diff net:", name, "vs. oth_net:",
                 oth_net->name, "var:", nm, "our: " + String(our) + "their:" + String(their));
    return false;
  }
  return true;
}

bool Network::CompareNetThrVal_ints
(Network* oth_net, const String& nm, const int* our, const int* their, int n) {
  int n_diff = 0;
  for(int i=0; i < n; i++) {
    if(our[i] != their[i]) {
      taMisc::Info(String(i), "our:", String(our[i]), "their:", String(their[i]));
      n_diff++;
    }
  }
  if(n_diff > 0) {
    taMisc::Info("CompareNetThrVals ints diff net:", name, "vs. oth_net:",
                 oth_net->name, "var:", nm, "n_diffs:", String(n_diff));
    return false;
  }
  return true;
}

#define CNVcg(x) String("cg.") + #x, ocg->x, tcg->x

bool Network::CompareNetThrVal_mem
(Network* oth_net, const String& nm, const char* our, const char* their, int n) {

  int n_cg = n / con_state_size; // this is only called for con group!
  
  int n_diff = 0;
  for(int i=0; i < n_cg; i++) {
    int idx = i * con_state_size;
    ConState_cpp* ocg = (ConState_cpp*)(our + idx);
    ConState_cpp* tcg = (ConState_cpp*)(their + idx);
    if(!CompareNetThrVal_int(oth_net, CNVcg(size))) n_diff++;
    if(!CompareNetThrVal_int(oth_net, CNVcg(vec_chunked_size))) n_diff++;
    if(!CompareNetThrVal_int(oth_net, CNVcg(alloc_size))) n_diff++;
    if(!CompareNetThrVal_int(oth_net, CNVcg(other_idx))) n_diff++;
    if(!CompareNetThrVal_int(oth_net, CNVcg(share_idx))) n_diff++;
    if(!CompareNetThrVal_int(oth_net, CNVcg(n_con_vars))) n_diff++;
    if(!CompareNetThrVal_int(oth_net, CNVcg(own_flat_idx))) n_diff++;
    if(!CompareNetThrVal_int(oth_net, CNVcg(own_thr_idx))) n_diff++;
  }
  if(n_diff > 0) {
    taMisc::Info("CompareNetThrVals cg diff net:", name, "vs. oth_net:",
                 oth_net->name, "var:", nm, "n_diffs:", String(n_diff));
    return false;
  }
  return true;
}

bool Network::CompareNetThrVal_memf
(Network* oth_net, const String& nm, const float* our, const float* their, int64_t n) {
  int n_diff = 0;
  if(nm.contains("recv")) {
    for(int i=0; i < n; i++) {
      if(our[i] != their[i]) { // fabs(our[i] - their[i]) > 1.0e-6f) {
        // taMisc::Info(String(i), "our:", String(our[i]), "their:", String(their[i]));
        n_diff++;
      }
    }
  }
  else {
    int32_t* oint = (int32_t*)our;
    int32_t* tint = (int32_t*)their;
    for(int i=0; i < n; i++) {
      if(oint[i] != tint[i]) {
        // taMisc::Info(String(i), "our:", String(our[i]), "their:", String(their[i]));
        n_diff++;
      }
    }
  }
  if(n_diff > 0) {
    taMisc::Info("CompareNetThrVals mem diff net:", name, "vs. oth_net:",
                 oth_net->name, "var:", nm, "n_diffs:", String(n_diff));
    return false;
  }
  return true;
}

#define CNV(x) #x, x, oth_net->x
#define CNVi(x,i) (String)#x + "_" + String(i), x[i], oth_net->x[i]

bool Network::CompareNetThrVals(Network* oth_net) {
  bool thr_same = CompareNetThrVal_int(oth_net, CNV(n_thrs_built));
  if(!thr_same) return thr_same;
  CompareNetThrVal_int(oth_net, CNV(con_state_size));
  CompareNetThrVal_int(oth_net, CNV(unit_state_size));
  CompareNetThrVal_int(oth_net, CNV(n_units_built));
  CompareNetThrVal_int(oth_net, CNV(n_layers_built));
  CompareNetThrVal_int(oth_net, CNV(n_ungps_built));
  CompareNetThrVal_int(oth_net, CNV(max_thr_n_units));
  CompareNetThrVal_int(oth_net, CNV(n_recv_cgps));
  CompareNetThrVal_int(oth_net, CNV(n_send_cgps));
  CompareNetThrVal_int(oth_net, CNV(n_lay_stats));
  CompareNetThrVal_int(oth_net, CNV(n_lay_stats_vars));
  CompareNetThrVal_ints(oth_net, CNV(units_thrs), n_units_built);
  CompareNetThrVal_ints(oth_net, CNV(units_thr_un_idxs), n_units_built);
  CompareNetThrVal_ints(oth_net, CNV(thrs_n_units), n_thrs_built);
  CompareNetThrVal_ints(oth_net, CNV(units_n_recv_cgps), n_units_built);
  CompareNetThrVal_ints(oth_net, CNV(units_n_send_cgps), n_units_built);
  
  for(int thr_no=0; thr_no < n_thrs_built; thr_no++) {
    CompareNetThrVal_int(oth_net, CNVi(thrs_n_units, thr_no));
    CompareNetThrVal_ints(oth_net, CNVi(thrs_unit_idxs, thr_no), thrs_n_units[thr_no]);
    CompareNetThrVal_ints(oth_net, CNVi(thrs_lay_unit_idxs, thr_no), n_layers_built*2);
    CompareNetThrVal_ints(oth_net, CNVi(thrs_ungp_unit_idxs, thr_no), n_ungps_built*2);
    CompareNetThrVal_int(oth_net, CNVi(thrs_n_recv_cgps, thr_no));
    CompareNetThrVal_int(oth_net, CNVi(thrs_n_send_cgps, thr_no));
    CompareNetThrVal_ints(oth_net, CNVi(thrs_units_n_recv_cgps, thr_no),
                          thrs_n_units[thr_no]);
    CompareNetThrVal_ints(oth_net, CNVi(thrs_units_n_send_cgps, thr_no),
                          thrs_n_units[thr_no]);
    CompareNetThrVal_ints(oth_net, CNVi(thrs_recv_cgp_start, thr_no),
                          thrs_n_units[thr_no]);
    CompareNetThrVal_ints(oth_net, CNVi(thrs_send_cgp_start, thr_no),
                          thrs_n_units[thr_no]);
    CompareNetThrVal_int(oth_net, CNVi(thrs_own_cons_max_size, thr_no));
    CompareNetThrVal_int(oth_net, CNVi(thrs_own_cons_avg_size, thr_no));

    CompareNetThrVal_int64(oth_net, CNVi(thrs_recv_cons_cnt, thr_no));
    CompareNetThrVal_int64(oth_net, CNVi(thrs_send_cons_cnt, thr_no));

    CompareNetThrVal_mem(oth_net, CNVi(thrs_recv_cgp_mem, thr_no),
                         thrs_n_recv_cgps[thr_no] * con_state_size);
    CompareNetThrVal_mem(oth_net, CNVi(thrs_send_cgp_mem, thr_no),
                         thrs_n_send_cgps[thr_no] * con_state_size);

    CompareNetThrVal_memf(oth_net, CNVi(thrs_recv_cons_mem, thr_no),
                          thrs_recv_cons_cnt[thr_no]);
    CompareNetThrVal_memf(oth_net, CNVi(thrs_send_cons_mem, thr_no),
                          thrs_send_cons_cnt[thr_no]);

  }
  return true;
}

#endif // CompareNetThrVals debugging -- turn off when not needed

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

bool Network::ComputeHash(bool incl_weights) {
  if(TestError(!IsBuiltIntact(), "ComputeHash", "network is not built an intact!")) {
    return false;
  }

  sha3_context c;
  sha3_Init256(&c);

  TypeDef* lay_typ = LayerStateType();
  TypeDef* ungp_typ = UnGpStateType();
  TypeDef* prjn_typ = PrjnStateType();
  TypeDef* un_typ = UnitStateType();
  TypeDef* con_typ = ConStateType();

  // range of 
  int lay_st = lay_typ->members.FindName("layer_idx")->GetRelOff();
  int lay_ed = lay_typ->members.FindName("ext_flag")->GetRelOff();
  int lay_n = lay_ed - lay_st;

  int ugp_ed = ungp_typ->members.FindName("disp_pos_y")->GetRelOff() + sizeof(int); // inclusive
  int prjn_ed = prjn_typ->members.FindName("recv_con_stats")->GetRelOff();
  int un_ed = un_typ->members.FindName("tmp_calc1")->GetRelOff();
  int cn_ed = con_typ->members.FindName("n_con_vars")->GetRelOff() + sizeof(int); // inclusive

  for(int li=0; li < n_layers_built; li++) {
    LayerState_cpp* lay = GetLayerState(li);
    if(lay->lesioned()) continue;
    sha3_Update(&c, lay + lay_st, lay_n);

    for(int gi=0; gi < lay->n_ungps; gi++) {
      UnGpState_cpp* ugp = lay->GetUnGpState(net_state, gi);
      sha3_Update(&c, ugp, ugp_ed);
    }
    for(int pi=0; pi < lay->n_recv_prjns; pi++) {
      PrjnState_cpp* prjn = lay->GetRecvPrjnState(net_state, pi);
      if(prjn->NotActive(net_state)) continue;
      sha3_Update(&c, prjn, prjn_ed);
    }
    for(int pi=0; pi < lay->n_send_prjns; pi++) {
      PrjnState_cpp* prjn = lay->GetSendPrjnState(net_state, pi);
      if(prjn->NotActive(net_state)) continue;
      sha3_Update(&c, prjn, prjn_ed);
    }

    for(int ui=0; ui < lay->n_units; ui++) {
      UnitState_cpp* u = lay->GetUnitState(net_state, ui);
      sha3_Update(&c, u, un_ed);

      const int rsz = u->NRecvConGps(net_state);
      for(int ri = 0; ri < rsz; ri++) {
        ConState_cpp* cg = u->RecvConState(net_state, ri);
        sha3_Update(&c, cg, cn_ed);
        if(cg->size > 0) {
          sha3_Update(&c, cg->mem_start, cg->size * sizeof(int32_t));
          if(cg->OwnCons() && incl_weights) {
            sha3_Update(&c, cg->cnmem_start, cg->size * sizeof(float));
          }
        }
      }
      
      const int ssz = u->NSendConGps(net_state);
      for(int si = 0; si < ssz; si++) {
        ConState_cpp* cg = u->SendConState(net_state, si);
        sha3_Update(&c, cg, cn_ed);
        if(cg->size > 0) {
          sha3_Update(&c, cg->mem_start, cg->size * sizeof(int32_t));
          if(cg->OwnCons() && incl_weights) {
            sha3_Update(&c, cg->cnmem_start, cg->size * sizeof(float));
          }
        }
      }
    }
  }

  const uint8_t* hash = sha3_Finalize(&c);
  int bytes = 32;               // 256 bits
  hash_value.SetSize(bytes);
  for(int i=0; i < bytes; i++) {
    hash_value[i] = hash[i];
  }
  return true;
}

#ifdef DMEM_COMPILE

bool Network::DMem_ConfirmHash(bool incl_weights) {
  if(taMisc::dmem_nprocs <= 1 || dmem_trl_comm.nprocs <= 1)
    return true;
  if(!ComputeHash(incl_weights)) return false;

  int hash_size = hash_value.size;
  
  int n_recv = hash_size * dmem_trl_comm.nprocs;
  if(dmem_trl_comm.this_proc == 0) {
    byte_Array gather;
    gather.SetSize(n_recv);
    DMEM_MPICALL(MPI_Gather(hash_value.el, hash_size, MPI_BYTE, gather.el, hash_size,
                            MPI_BYTE, 0, dmem_trl_comm.comm), "DMem_ConfirmHash", "Gather");
    for(int j=0; j < hash_size; j++) {
      byte hvb = hash_value[j];
      for(int i=1; i<dmem_trl_comm.nprocs; i++) {
        byte cmb = gather[i * hash_size + j];
        if(hvb != cmb) {
          taMisc::Error("DMem_ConfirmHash: hash value differs for proc:", String(i));
          return false;
        }
      }
    }
    taMisc::Info("DMem_ConfirmHash: Network hash values are CONSISTENT across all nodes");
  }
  else {
    DMEM_MPICALL(MPI_Gather(hash_value.el, hash_size, MPI_BYTE, NULL, n_recv,
                            MPI_BYTE, 0, dmem_trl_comm.comm), "DMem_ConfirmHash", "Gather");
  }
  return true;
}

#endif

Layer* Network::NewLayer() {
  return layers.NewEl(1);
}

bool Network::EditState() {
  if(!taMisc::gui_active || !net_state) return false;
  if (taiEdit *ie = NetworkStateType()->ie) {
    return ie->Edit((void*)net_state, false);
  }
  return false;
}

void Network::RemoveUnits() {
  ClearNetFlag(BUILT);
  ClearIntact();
  RemoveCons();
  SyncNetState();
  
  taMisc::Busy();
  StructUpdate(true);

  for(int i=0;i<layers.leaves; i++) {
    Layer* lay = (Layer*)layers.Leaf(i);
    lay->n_units_built = 0;
  }

  net_state->UnBuildState();
  n_units = 0;
  specs.ResetAllSpecIdxs();
  StructUpdate(false);
  taMisc::DoneBusy();
}

void Network::RemoveCons() {
  taMisc::Busy();
  StructUpdate(true);
  RemoveCons_impl();
  StructUpdate(false);
  taMisc::DoneBusy();
}

void Network::RemoveCons_impl() {
  ClearNetFlag(BUILT);
  ClearIntact();
  if(!net_state->thrs_n_recv_cgps) return; // cgps already gone
  net_state->FreeConMem();
  n_cons = 0;
}


DataTable* Network::NetStructToTable(DataTable* dt, bool list_specs) {
  bool new_table = false;
  if(!dt) {
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

  FOREACH_ELEM_IN_GROUP(Layer, lay, layers) {
//     if(l->lesioned()) continue;   // for this, get everything
    Layer_Group* lg = NULL;
    if(lay->owner != &layers)
      lg = (Layer_Group*)lay->owner;
    dt->AddBlankRow();
    dt->SetVal(lay->name, "Name", -1);
    if(lg)
      dt->SetVal(lg->name, "Group", -1);
    String ltype = lay->GetEnumString("LayerType", lay->layer_type);
    dt->SetVal(ltype, "Type", -1);
    dt->SetVal(lay->un_geom.x, "Size_X", -1);
    dt->SetVal(lay->un_geom.y, "Size_Y", -1);
    if(lay->unit_groups) {
      dt->SetVal(lay->gp_geom.x, "UnitGps_X", -1);
      dt->SetVal(lay->gp_geom.y, "UnitGps_Y", -1);
    }
    String fmp;
    for(int i=0; i<lay->projections.size; i++) {
      Projection* pj = lay->projections.FastEl(i);
      if(pj->off) continue;
      fmp += pj->from->name + " ";
    }
    dt->SetVal(fmp, "RecvPrjns", -1);

    String snp;
    for(int i=0; i<lay->send_prjns.size; i++) {
      Projection* pj = lay->send_prjns.FastEl(i);
      if(pj->off) continue;
      snp += pj->layer->name + " ";
    }
    dt->SetVal(snp, "SendPrjns", -1);

    if(list_specs) {
      UnitSpec* us = lay->GetMainUnitSpec();
      if(us)
        dt->SetVal(us->name, "UnitSpec", -1);
      else
        dt->SetVal("NULL", "UnitSpec", -1);
      LayerSpec* ls = lay->GetMainLayerSpec();
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

  Layer* lay = 0;
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
      lay = FindMakeLayer(lnm);
      if(lay->InLayerSubGroup()) {
        layers.Transfer(lay);     // transfer into main list
      }
    }
    else {
      Layer_Group* lgp = FindMakeLayerGroup(gpnm);
      lay = FindLayer(lnm);
      if(lay && lay->owner != lgp) {
        lgp->Transfer(lay);       // make it ours
      }
      else {
        lay = lgp->FindMakeLayer(lnm); // make new one
      }
    }
    lay->ClearBaseFlag(BF_MISC1); // mark it
    String etyp;
    lay->layer_type = (Layer::LayerType)lay->GetEnumVal(ltyp, etyp);
    lay->un_geom.x = szx;
    lay->un_geom.y = szy;
    lay->un_geom.UpdateAfterEdit();
    if(gszx > 0 && gszy > 0) {
      lay->unit_groups = true;
      lay->gp_geom.x = gszx;
      lay->gp_geom.y = gszy;
      lay->gp_geom.UpdateAfterEdit();
    }
  }
  // second pass make projections
  for(int i=0;i<dt->rows; i++) {
    String lnm = trim(dt->GetVal("Name", i).toString());
    String recvs = trim(dt->GetVal("RecvPrjns", i).toString());

    lay = FindLayer(lnm);
    for(int pi=0; pi < lay->projections.size; pi++) {
      Projection* prjn = lay->projections[pi];
      prjn->SetBaseFlag(BF_MISC1);
    }

    String_Array rps;
    rps.FmDelimString(recvs, " "); // fill with items
    for(int pi=0; pi < rps.size; pi++) {
      String fmnm = rps[pi];
      Layer* fm = FindLayer(fmnm);
      if(fm) {
        Projection* prjn = FindMakePrjn(lay, fm);
        prjn->ClearBaseFlag(BF_MISC1);
      }
    }

    // cull any non-wanted projections
    for(int pi=lay->projections.size-1; pi >= 0; pi--) {
      Projection* prjn = lay->projections[pi];
      if(prjn->HasBaseFlag(BF_MISC1)) {
        lay->projections.RemoveIdx(pi);
      }
    }
  }

  // cull any non-wanted layers
  for(int li= layers.leaves-1; li>=0; li--) {
    lay = layers.Leaf(li);
    if(lay->HasBaseFlag(BF_MISC1))
      layers.RemoveLeafIdx(li);
  }
}

DataTable* Network::NetPrjnsToTable(DataTable* dt, bool include_off) {
  bool new_table = false;
  if(!dt) {
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
  col = dt->FindMakeColName("Notes", idx, VT_STRING);
  col->desc = "user-entered notes for each projection";

  FOREACH_ELEM_IN_GROUP(Layer, lay, layers) {
//     if(l->lesioned()) continue;   // for this, get everything
    for(int pi=0; pi < lay->projections.size; pi++) {
      Projection* prjn = lay->projections[pi];
      dt->AddBlankRow();
      dt->SetVal(lay->name, "LayerName", -1);
      dt->SetVal(prjn->name, "PrjnFrom", -1);
      ProjectionSpec* ps = prjn->GetMainPrjnSpec();
      if(ps)
        dt->SetVal(ps->name, "PrjnSpec", -1);
      else
        dt->SetVal("NULL", "PrjnSpec", -1);
      ConSpec* cs = prjn->GetMainConSpec();
      if(cs)
        dt->SetVal(cs->name, "ConSpec", -1);
      else
        dt->SetVal("NULL", "ConSpec", -1);
      dt->SetVal(prjn->notes, "Notes", -1);
    }
  }
  dt->StructUpdate(false);
  if(new_table)
    tabMisc::DelayedFunCall_gui(dt, "BrowserSelectMe");
  return dt;
}

String Network::NetPrjnsToList(taMarkUp::Format fmt, bool include_off) {
  int indent = 0;
  if(layers.size == 0 && layers.leaves > 0) // no top-level guys -- will have an extra indent
    indent = -1;
  String rval = "\n";
  NetPrjnsToList_gp(&layers, rval, fmt, include_off, indent);
  return rval;
}

void Network::NetPrjnsToList_gp(Layer_Group* gp, String& rval, taMarkUp::Format fmt, bool include_off,
                                int& indent) {
  if(gp->gp.size > 0) {
    rval << taMarkUp::ListStart(fmt, indent, false)
         << taMarkUp::ListItem(fmt, indent, false) << taMarkUp::Escape(fmt, gp->name) << "\n";
  }
  if(gp->size > 0) {
    rval << taMarkUp::ListStart(fmt, indent, false);
    for(int li=0; li<gp->size; li++) {
      Layer* lay = gp->FastEl(li);
      if(!include_off && lay->lesioned())
        continue;
      if(lay->projections.size == 0)
        continue;
      rval << taMarkUp::ListItem(fmt, indent, false) << taMarkUp::Escape(fmt, lay->name) << "\n";
      rval << taMarkUp::ListStart(fmt, indent, false);
      for(int pi=0; pi < lay->projections.size; pi++) {
        Projection* prjn = lay->projections[pi];
        if(!include_off && prjn->off)
          continue;
        rval << taMarkUp::ListItem(fmt, indent, false);
        if(prjn->off)
          rval << taMarkUp::Strike(fmt, prjn->name);
        else
          rval << taMarkUp::Bold(fmt, prjn->name);
        rval << ": ";
        rval << taMarkUp::Escape(fmt, prjn->notes) << " ";
        ProjectionSpec* ps = prjn->GetMainPrjnSpec();
        if(ps)
          rval << "(" << taMarkUp::Escape(fmt, ps->name) << ") ";
        ConSpec* cs = prjn->GetMainConSpec();
        if(cs)
          rval << "(" << taMarkUp::Escape(fmt, cs->name) << ") ";
        rval << "\n";
      }
      rval << taMarkUp::ListEnd(fmt, indent, false);
    }
    rval << taMarkUp::ListEnd(fmt, indent, false);
  }
  if(gp->gp.size > 0) {
    for(int gi=0; gi<gp->gp.size; gi++) {
      Layer_Group* lg = (Layer_Group*)gp->gp.FastEl(gi);
      rval << taMarkUp::ListItem(fmt, indent, false) << lg->name << "\n";
      NetPrjnsToList_gp(lg, rval, fmt, include_off, indent);
    }
    rval << taMarkUp::ListEnd(fmt, indent, false);
  }
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
  dmem_agg_sum.agg_op = MPI_SUM;
  dmem_agg_sum.ScanMembers(NetworkStateType(), (void*)net_state);
  dmem_agg_sum.CompileVars();
}


void Network::DMem_SumDWts(MPI_Comm comm) {
  wt_sync_time.StartTimer(false); // don't reset
  static double timerabs = 0;

  double timer1s = MPI_Wtime();
  int np = 0; MPI_Comm_size(comm, &np);
  int this_proc = 0; MPI_Comm_rank(comm, &this_proc);
  if(np <= 1) return;

  // note: memory is not contiguous for all DWT vars, so we still need to do this..

  net_state->DMem_SumDWts_ToTmp();

  double timer2s = MPI_Wtime();

  // the one-big call seems slower in terms of usr time, but not wallclock
  // keeping everything thread-specific is likely to incur less overhead
  // if(false) {			// one big call
  //   if(taMisc::thread_defaults.alt_mpi) {
  //     dmem_trl_comm.my_reduce->allreduce
  //       (all_dmem_sum_dwts_send, all_dmem_sum_dwts_recv, all_dmem_sum_dwts_size);
  //   }
  //   else {
  //     DMEM_MPICALL(MPI_Allreduce
  //       	   (all_dmem_sum_dwts_send, all_dmem_sum_dwts_recv,
  //       	    all_dmem_sum_dwts_size, MPI_FLOAT, MPI_SUM, comm),
  //       	   "Network::SumDWts", "Allreduce");
  //   }
  // }
  // else {
  for(int i=0; i<n_thrs_built; i++) {
    int64_t n_floats = net_state->thrs_own_cons_tot_size_nonshared[i] + net_state->thrs_n_units[i];
    if(taMisc::thread_defaults.alt_mpi) {
      dmem_trl_comm.my_reduce->allreduce
        (net_state->thrs_dmem_sum_dwts_send[i], net_state->thrs_dmem_sum_dwts_recv[i], n_floats);
    }
    else {
      DMEM_MPICALL(MPI_Allreduce
                   (net_state->thrs_dmem_sum_dwts_send[i], net_state->thrs_dmem_sum_dwts_recv[i],
                    n_floats, MPI_FLOAT, MPI_SUM, comm),
                   "Network::SumDWts", "Allreduce");
    }
  }
  // }

  double timer2e = MPI_Wtime();

  net_state->DMem_SumDWts_FmTmp();
  
  double timer1e = MPI_Wtime();

  // if (false && this_proc == 0) {
  //   // todo: best to just add stats params to Network object to record this, which can
  //   // then be logged -- print statements go into .out file of jobs and clutter that massively
  //   printf("P%i: Computing MPI dmem after %fs resulting in %.1f%% time spent in syncing: Transmitted %i Mb in %fs / %fs = %.1f %.2fGbit/s\n", this_proc,
  //          (timer1s - timerabs), ((timer1e-timer1s)/(timer1s - timerabs)) * 100.0,
  //          (all_dmem_sum_dwts_size*sizeof(float)/1024/1024), (timer2e - timer2s), (timer1e - timer1s),
  //          ((timer2e-timer2s)/(timer1e - timer1s)*100.0), (all_dmem_sum_dwts_size*sizeof(float)*8.0/(timer2e-timer2s)/1024.0/1024.0/1024.0));
  // }
  timerabs = timer1s;
  wt_sync_time.EndTimer();
}

void Network::DMem_ComputeAggs(MPI_Comm comm) {
  dmem_agg_sum.AggVar(comm, MPI_SUM);

  // also need to do layers and projections
  for(int li=0; li < n_layers_built; li++) {
    LayerState_cpp* lay = GetLayerState(li);
    if(lay->lesioned()) continue;
    Layer* mlay = LayerFromState(lay);
    mlay->DMem_ComputeAggs(comm);
    
    for(int pi=0; pi < mlay->projections.size; pi++) {
      Projection* prjn = mlay->projections[pi];
      if(prjn->NotActive(net_state)) continue;
      prjn->DMem_ComputeAggs(comm);
    }
  }
}

#endif  // DMEM

void Network::Copy_Weights(const Network* src) {
  taMisc::Busy();
  Cuda_ConStateToHost();
  Layer* lay, *slay;
  taLeafItr li,si;
  for(lay = layers.FirstEl(li), slay = src->layers.FirstEl(si);
      (lay) && (slay);
      lay = layers.NextEl(li), slay = src->layers.NextEl(si))
  {
    if(!lay->lesioned() && !slay->lesioned())
      lay->Copy_Weights(slay, true);
  }
  Cuda_ConStateToDevice();
  UpdateAllViews();
  taMisc::DoneBusy();
}

void Network::SaveWeights(const String& fname, WtSaveFormat fmt) {
  if(fmt == NET_FMT) fmt = wt_save_fmt;

  // todo: sync weights from cuda
  SyncAllState();
  
  if(TestError(!IsBuiltIntact(), "LoadWeights",
               "network is not built or intact -- cannot save weights")) {
    return;
  }
  taFiler* flr = GetSaveFiler(fname, ".wts", true);
  if(flr->ostrm) {
    net_state->NetworkSaveWeights_strm(*flr->ostrm, (NetworkState_cpp::WtSaveFormat)fmt);
  }
  flr->Close();
  taRefN::unRefDone(flr);
}

bool Network::LoadWeights(const String& fname, bool quiet) {
  if(TestError(!IsBuiltIntact(), "LoadWeights",
               "network is not built or intact -- cannot load weights")) {
    return false;
  }
  taFiler* flr = GetLoadFiler(fname, ".wts", true);
  bool rval = false;
  if(flr->istrm) {
    rval = net_state->NetworkLoadWeights_strm(*flr->istrm, quiet);
    epoch = net_state->epoch;   // this usually sync's the other way..
    SyncAllState();
    // sync wieghts..
  }
  else {
    TestError(true, "LoadWeights", "aborted due to inability to load weights file");
    // the above should be unnecessary but we're not getting the error sometimes..
  }
  flr->Close();
  taRefN::unRefDone(flr);
  UpdateAllViews();
  return rval;
}

void Network::SaveToWeights(Weights* wts) {
  if(TestError(!IsBuiltIntact(), "SaveToWeights",
               "network is not built or intact -- cannot save to weights")) {
    return;
  }
  if(wts == NULL) {
    wts = (Weights*)weights.New(1);
  }
  // todo: sync weights from cuda
  ostringstream oss;
  net_state->NetworkSaveWeights_strm(oss, NetworkState_cpp::TEXT);  // always use text for this
  wts->wt_file = oss.str().c_str();
  wts->epoch = epoch;
  wts->batch = batch;
  if(wts->name.contains("Weights") && file_name.nonempty()) {
    wts->SetName(taMisc::GetFileFmPath(file_name));
  }
  wts->SigEmitUpdated();
}

bool Network::LoadFmWeights(Weights* wts, bool quiet) {
  if(TestError(!IsBuiltIntact(), "LoadFmWeights",
               "network is not built or intact -- cannot load from weights")) {
    return false;
  }
  if(TestError(!wts, "LoadFmWeights", "Weights object is NULL")) {
    return false;
  }
  if(!wts->HasWeights(true)) return false;
  istringstream iss(wts->wt_file.chars());
  bool rval = net_state->NetworkLoadWeights_strm(iss, quiet);
  SyncAllState();
  return rval;
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

void Network::SaveWeights_Tagged() {
  if(!IsBuiltIntact() || epoch < 1) return;

  if(taMisc::dmem_proc > 0) {
    return;
  }
  String batch_str = taMisc::LeadingZeros(batch, 2);
  String epoch_str = taMisc::LeadingZeros(epoch, 4);
  String tag = taMisc::FindArgByName("tag");
  String final_tag = tag + "." + batch_str + "_" + epoch_str;
  String fname = GetFileNameFmProject(".wts.gz", final_tag, "", false);
  taMisc::Info("Saving tagged weights to:", fname);
  SaveWeights(fname);
}

bool Network::SaveWeights_ClusterRunTerm() {
  if(!taMisc::cluster_run) return false;
  if(total_trials % 10 != 0) return false; // check every 10 trials to minimize load
  
  proj->GetClusterRunJob();     // make sure we have cluster run job data
  if(ClusterRunJob::CurJobCheckSaveTermState()) {
    if(taMisc::dmem_proc == 0) {
      taMisc::Info("Cluster Run: saving weights 5 min prior to termination");
      SaveWeights_Tagged();
    }
    return true;
  }
  return false;
}

bool Network::SaveWeights_ClusterRunCmd() {
  if(!taMisc::cluster_run) return false;
  if(taMisc::dmem_proc != 0) return false;
  String cmd = proj->CheckClusterRunCmd();
  if(cmd.nonempty()) {
    if(cmd == "SAVESTATE") {
      taMisc::Info("Cluster Run: saving weights from SAVESTATE command");
      SaveWeights_Tagged();
      return true;
    }
    else {
      taMisc::Info("Cluster Run: don't know how to process this command:", cmd);
    }
  }
  return false;
}

void Network::LayerPos_RelPos() {
  taBase_List loop_check;
  FOREACH_ELEM_IN_GROUP(Layer, lay, layers) {
    if(!lay->pos_rel.IsRel()) {
      continue;
    }
    bool has_loop = false;
    loop_check.Reset();
    Layer* cur = lay;
    while(cur) {
      if(!cur->pos_rel.IsRel())
        break;
      int fidx = loop_check.FindEl(cur->pos_rel.other);
      if(TestWarning(fidx >= 0, "RelPosLoopCheck",
                     "a loop was found in the connection graph of relative positioning of layers -- severing last link!")) {
        cur->pos_rel.other = NULL;
        has_loop = true;
        break;
      }
      loop_check.Link(cur);
      cur = cur->pos_rel.other;
    }
    lay->UpdateGeometry();
  }
  bool lay_moved = false;
  int n_iters = 0;
  do {
    lay_moved = false;
    n_iters++;
    FOREACH_ELEM_IN_GROUP(Layer, lay, layers) {
      bool updt_pos = lay->UpdatePosition();
      if(updt_pos) {
        lay_moved = true;
        lay->SigEmitUpdated();
      }
    }
  }
  while(lay_moved && n_iters < 5);
  
  UpdateLayerGroupGeom();          // one last time.. has likely been updated already but..
}


void Network::LayerZPos_Unitize() {
  int_Array zvals;
  taVector3i lpos;
  FOREACH_ELEM_IN_GROUP(Layer, lay, layers) {
    lay->GetAbsPos(lpos);
    zvals.AddUnique(lpos.z);
  }
  zvals.Sort();
  FOREACH_ELEM_IN_GROUP(Layer, lay, layers) {
    int nw_z = zvals.FindEl(lay->pos.z); // replace with its index on sorted list..
    lay->pos.z += nw_z - lay->pos.z;
  }
  LayerPos_RelPos();
}

void Network::LayerPos_Cleanup() {
  layers.LayerPos_Cleanup();
  LayerPos_RelPos();
}

void Network::LayerPos_GridLayout_3d(int x_space, int y_space,
                                     int z_size, int gp_grid_x, int lay_grid_x) {
  StructUpdate(true);
  layers.LayerPos_GridLayout_3d(x_space, y_space, z_size, gp_grid_x, lay_grid_x);
  LayerPos_RelPos();
  StructUpdate(false);
}

void Network::Compute_LayerDistances() {
  // first reset all
  FOREACH_ELEM_IN_GROUP(Layer, lay, layers) {
    if(lay->lesioned()) continue;
    lay->dist.fm_input = -1; lay->dist.fm_output = -1;
  }

  // next go through and find inputs
  FOREACH_ELEM_IN_GROUP(Layer, lay, layers) {
    if(lay->lesioned()) continue;
    if(lay->layer_type != Layer::INPUT) continue;
    lay->dist.fm_input = 0;
    lay->PropagateInputDistance();
  }
  // then outputs
  FOREACH_ELEM_IN_GROUP(Layer, lay, layers) {
    if(lay->lesioned()) continue;
    if(!((lay->layer_type == Layer::OUTPUT) || (lay->layer_type == Layer::TARGET))) continue;
    lay->dist.fm_output = 0;
    lay->PropagateOutputDistance();
  }
}

void Network::Compute_PrjnDirections() {
  Compute_LayerDistances();     // required data
  FOREACH_ELEM_IN_GROUP(Layer, lay, layers) {
    if(lay->lesioned()) continue;
    lay->Compute_PrjnDirections();
  }
}

void Network::SetUnitNames(bool force_use_unit_names) {
  FOREACH_ELEM_IN_GROUP(Layer, lay, layers) {
    if(!lay->lesioned())
      lay->SetUnitNames(force_use_unit_names);
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

void Network::GetLocalistName() {
  FOREACH_ELEM_IN_GROUP(Layer, lay, layers) {
    if(!lay->lesioned())
      lay->GetLocalistName();
  }
  UpdateAllViews();
}

bool Network::SnapVar(const String& variable) {
  SimpleMathSpec sm;
  sm.opr = SimpleMathSpec::NONE;
  return Snapshot(variable, sm, false); // empty var is retrieved
}

bool Network::SnapAnd(const String& variable) {
  SimpleMathSpec sm;
  sm.opr = SimpleMathSpec::MIN;
  return Snapshot(variable, sm, true); // empty var is retrieved
}

bool Network::SnapOr(const String& variable) {
  SimpleMathSpec sm;
  sm.opr = SimpleMathSpec::MAX;
  return Snapshot(variable, sm, true); // empty var is retrieved
}

bool Network::SnapThresh(float thresh_val, const String& variable) {
  SimpleMathSpec sm;
  sm.opr = SimpleMathSpec::THRESH;
  sm.arg = thresh_val;
  sm.lw = 0.0;
  sm.hi = 1.0;
  return Snapshot(variable, sm, false);
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
    UnitState_cpp* src_u = GetViewSrcU();
    if(TestError(!src_u, "Snapshot", "For r. or s. variables, must have a selected unit in the network view!"))
      return false;
  }

  FOREACH_ELEM_IN_GROUP(Layer, lay, layers) {
    if(lay->lesioned()) continue;
    if(!lay->Snapshot(var, math_op, arg_is_snap)) return false;
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

void Network::MonitorData() {
  if(monitor.items.size == 0) return;
  if(!taMisc::gui_active) return;
  mon_data.EnforceRows(1);
  mon_data.WriteItem(0);
  monitor.GetMonVals();
}

void Network::NetControlPanel(ControlPanel* ctrl_panel, const String& extra_label, const String& sub_gp_nm) {
  if(!ctrl_panel) {
    if(TestError(!proj, "NetControlPanel", "cannot find project")) return;
    ctrl_panel = (ControlPanel*)proj->ctrl_panels.New(1);
  }
  TypeDef* td = GetTypeDef();
  for(int i=td->members.size-1; i>=0; i--) {
    MemberDef* md = td->members[i];
    if(!md->HasOption("VIEW")) continue;
    // filter by current guys..
    if(HasUserData(md->name) && !GetUserDataAsBool(md->name)) continue;
    ctrl_panel->AddMember(this, md, "", sub_gp_nm);
  }
}

void Network::TransformWeights(const SimpleMathSpec& trans) {
  taMisc::Busy();
  Cuda_ConStateToHost();
  FOREACH_ELEM_IN_GROUP(Layer, lay, layers) {
    if(!lay->lesioned())
      lay->TransformWeights(trans);
  }
  Init_Weights_post();
  Cuda_ConStateToDevice();
  UpdateAllViews();
  taMisc::DoneBusy();
}

void Network::AddNoiseToWeights(const Random& noise_spec) {
  taMisc::Busy();
  Cuda_ConStateToHost();
  FOREACH_ELEM_IN_GROUP(Layer, lay, layers) {
    if(!lay->lesioned())
      lay->AddNoiseToWeights(noise_spec);
  }
  Init_Weights_post();
  Cuda_ConStateToDevice();
  UpdateAllViews();
  taMisc::DoneBusy();
}

int Network::PruneCons(const SimpleMathSpec& pre_proc,
                          Relation::Relations rel, float cmp_val)
{
  taMisc::Busy();
  StructUpdate(true);
  int rval = 0;
  FOREACH_ELEM_IN_GROUP(Layer, lay, layers) {
    if(!lay->lesioned())
      rval += lay->PruneCons(pre_proc, rel, cmp_val);
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
  FOREACH_ELEM_IN_GROUP(Layer, lay, layers) {
    if(!lay->lesioned())
      rval += lay->ProbAddCons(p_add_con, init_wt);
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
  FOREACH_ELEM_IN_GROUP(Layer, lay, layers) {
    if(!lay->lesioned())
      rval += lay->LesionCons(p_lesion, permute);
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
  FOREACH_ELEM_IN_GROUP(Layer, lay, layers) {
    if(!lay->lesioned())
      rval += lay->LesionUnits(p_lesion, permute);
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
  FOREACH_ELEM_IN_GROUP(Layer, lay, layers) {
    if(!lay->lesioned())
      lay->UnLesionUnits();
  }
  StructUpdate(false);
  taMisc::DoneBusy();
  UpdtAfterNetMod();
}

void Network::LesionAllLayers() {
  taMisc::Busy();
  StructUpdate(true);
  FOREACH_ELEM_IN_GROUP(Layer, lay, layers) {
    lay->Lesion();
  }
  StructUpdate(false);
  taMisc::DoneBusy();
  UpdtAfterNetMod();
}

void Network::IconifyAllLayers() {
  taMisc::Busy();
  StructUpdate(true);
  FOREACH_ELEM_IN_GROUP(Layer, lay, layers) {
    lay->Iconify();
  }
  StructUpdate(false);
  taMisc::DoneBusy();
  UpdtAfterNetMod();
}

void Network::UnLesionAllLayers() {
  taMisc::Busy();
  StructUpdate(true);
  FOREACH_ELEM_IN_GROUP(Layer, lay, layers) {
    lay->UnLesion();
  }
  StructUpdate(false);
  taMisc::DoneBusy();
  UpdtAfterNetMod();
}

void Network::DeIconifyAllLayers() {
  taMisc::Busy();
  StructUpdate(true);
  FOREACH_ELEM_IN_GROUP(Layer, lay, layers) {
    lay->DeIconify();
  }
  StructUpdate(false);
  taMisc::DoneBusy();
  UpdtAfterNetMod();
}

void Network::UpdateLayerGroupGeom() {
  layers.UpdateLayerGroupGeom();

  // layer level only depends on visible, non-iconified layers!
  max_disp_size = 1;
  max_disp_size2d = 1;
  FOREACH_ELEM_IN_GROUP(Layer, lay, layers) {
    if(lay->lesioned()) continue;
    if(lay->Iconified()) continue;
    max_disp_size.x = MAX(max_disp_size.x, lay->pos_abs.x + lay->scaled_disp_geom.x);
    max_disp_size.y = MAX(max_disp_size.y, lay->pos_abs.y + lay->scaled_disp_geom.y);
    max_disp_size.z = MAX(max_disp_size.z, lay->pos_abs.z + 1); // extra bonus for unit vars

    max_disp_size2d.x = MAX(max_disp_size2d.x, lay->pos2d_abs.x + lay->scaled_disp_geom.x);
    max_disp_size2d.y = MAX(max_disp_size2d.y, lay->pos2d_abs.y + lay->scaled_disp_geom.y);
  }  
}

void Network::ReplaceSpecs(BaseSpec* old_sp, BaseSpec* new_sp, bool prompt) {
  if(old_sp->InheritsFrom(&TA_UnitSpec))
    ReplaceUnitSpec((UnitSpec*)old_sp, (UnitSpec*)new_sp, prompt);
  else if(old_sp->InheritsFrom(&TA_ConSpec))
    ReplaceConSpec((ConSpec*)old_sp, (ConSpec*)new_sp, prompt);
  else if(old_sp->InheritsFrom(&TA_ProjectionSpec))
    ReplacePrjnSpec((ProjectionSpec*)old_sp, (ProjectionSpec*)new_sp, prompt);
  else if(old_sp->InheritsFrom(&TA_LayerSpec))
    ReplaceLayerSpec((LayerSpec*)old_sp, (LayerSpec*)new_sp, prompt);

  ReplaceSpecs_Gp(old_sp->children, new_sp->children, prompt);
}

void Network::ReplaceSpecs_Gp(const BaseSpec_Group& old_spg, BaseSpec_Group& new_spg, bool prompt) {
  taLeafItr spo, spn;
  BaseSpec* old_sp, *new_sp;
  for(old_sp = (BaseSpec*)old_spg.FirstEl(spo), new_sp = (BaseSpec*)new_spg.FirstEl(spn);
      old_sp && new_sp;
      old_sp = (BaseSpec*)old_spg.NextEl(spo), new_sp = (BaseSpec*)new_spg.NextEl(spn)) {
    ReplaceSpecs(old_sp, new_sp, prompt);
  }
}

int Network::ReplaceUnitSpec(UnitSpec* old_sp, UnitSpec* new_sp, bool prompt) {
  int nchg = 0;
  FOREACH_ELEM_IN_GROUP(Layer, lay, layers) {
    if(!lay->lesioned())
      nchg += lay->ReplaceUnitSpec(old_sp, new_sp, prompt);
  }
  return nchg;
}

int Network::ReplaceConSpec(ConSpec* old_sp, ConSpec* new_sp, bool prompt) {
  int nchg = 0;
  FOREACH_ELEM_IN_GROUP(Layer, lay, layers) {
    if(!lay->lesioned())
      nchg += lay->ReplaceConSpec(old_sp, new_sp, prompt);
  }
  return nchg;
}

int Network::ReplacePrjnSpec(ProjectionSpec* old_sp, ProjectionSpec* new_sp, bool prompt) {
  int nchg = 0;
  FOREACH_ELEM_IN_GROUP(Layer, lay, layers) {
    if(!lay->lesioned())
      nchg += lay->ReplacePrjnSpec(old_sp, new_sp, prompt);
  }
  return nchg;
}

int Network::ReplaceLayerSpec(LayerSpec* old_sp, LayerSpec* new_sp, bool prompt) {
  int nchg = 0;
  FOREACH_ELEM_IN_GROUP(Layer, lay, layers) {
    if(!lay->lesioned())
      nchg += lay->ReplaceLayerSpec(old_sp, new_sp, prompt);
  }
  return nchg;
}

DataTable* Network::WeightsToTable(DataTable* dt, Layer* recv_lay, Layer* send_lay)
{
  if(recv_lay == NULL) return NULL;
  Cuda_ConStateToHost();
  return recv_lay->WeightsToTable(dt, send_lay);
}

DataTable* Network::VarToTable(DataTable* dt, const String& variable) {
  bool new_table = false;
  if(!dt) {
    dt = proj->GetNewAnalysisDataTable(name + "_Var_" + variable, true);
    new_table = true;
  }

  NetMonitor nm;
  taBase::Own(nm, this);
  nm.SetDataNetwork(dt, this);
  nm.AddNetwork(this, variable);
  nm.items[0]->max_name_len = 20; // allow long names
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
  Cuda_ConStateToHost();
  if(!dt) {
    dt = proj->GetNewAnalysisDataTable("ConVars", true);
    new_table = true;
  }
  dt->StructUpdate(true);
  FOREACH_ELEM_IN_GROUP(Layer, lay, layers) {
    if(!lay->lesioned())
      lay->ConVarsToTable(dt, var1, var2, var3, var4, var5, var6, var7, var8,
                        var9, var10, var11, var12, var13, var14);
  }
  dt->StructUpdate(false);
  if(new_table)
    tabMisc::DelayedFunCall_gui(dt, "BrowserSelectMe");
  return dt;
}

bool Network::VarToVarCopy(const String& dest_var, const String& src_var) {
  FOREACH_ELEM_IN_GROUP(Layer, lay, layers) {
    if(!lay->lesioned())
      lay->VarToVarCopy(dest_var, src_var);
  }
  return true;
}

bool Network::VarToVal(const String& dest_var, float val) {
  FOREACH_ELEM_IN_GROUP(Layer, lay, layers) {
    if(!lay->lesioned())
      lay->VarToVal(dest_var, val);
  }
  return true;
}

static bool net_project_wts_propagate(NetworkState_cpp* net, UnitState_cpp* u, bool swt) {
  bool got_some = false;
  // propagate!
  for(int g = 0; g < (swt ? u->NSendConGps(net) : u->NRecvConGps(net)); g++) {
    ConState_cpp* cg = (swt ? u->SendConState(net, g) : u->RecvConState(net, g));
    PrjnState_cpp* prjn = cg->GetPrjnState(net);
    if(!prjn || prjn->NotActive(net)) continue;
    LAYER_STATE* recv_lay = prjn->GetRecvLayer(net);
    LAYER_STATE* send_lay = prjn->GetSendLayer(net);
    LAYER_STATE* slay = (swt ? recv_lay : send_lay);

    if(slay->lesioned() || (send_lay == recv_lay) ||
       slay->HasLayerFlag(LAYER_STATE::PROJECT_WTS_DONE)) continue;
    slay->SetLayerFlag(LAYER_STATE::PROJECT_WTS_NEXT); // next..
    got_some = true;                           // keep going..

    for(int ci = 0; ci < cg->size; ci++) {
      float wtv = cg->Cn(ci, ConState_cpp::WT, net);
      UnitState_cpp* su = cg->UnState(ci,net);
      su->wt_prjn += u->wt_prjn * wtv;
      su->tmp_calc1 += u->wt_prjn;
    }
  }
  return got_some;
}

void Network::ProjectUnitWeights(UnitState_cpp* src_u, int top_k_un, int top_k_gp, bool swt,
                                 bool zero_sub_hiddens) {
  if(!src_u) return;

  // Cuda_ConStateToHost();

  NetworkState_cpp* net = net_state;
  
  float_Matrix topk_un_vec;             // for computing kwta
  float_Matrix topk_gp_vec;             // for computing kwta

  // first initialize all vars
  for(int li=0; li < n_layers_built; li++) {
    LayerState_cpp* lay = GetLayerState(li);
    if(lay->lesioned()) continue;
    lay->ClearLayerFlag(LAYER_STATE::PROJECT_WTS_NEXT);
    lay->ClearLayerFlag(LAYER_STATE::PROJECT_WTS_DONE);
    for(int ui = 0; ui < lay->n_units; ui++) {
      UNIT_STATE* u = lay->GetUnitState(net, ui);
      if(u->lesioned()) continue;
      u->wt_prjn = u->tmp_calc1 = 0.0f;
    }
  }

  // do initial propagation
  int maxn = (swt ? src_u->NSendConGps(net) : src_u->NRecvConGps(net));
  for(int g = 0; g < maxn; g++) {
    ConState_cpp* cg = (swt ? src_u->SendConState(net, g) : src_u->RecvConState(net, g));
    PrjnState_cpp* prjn = cg->GetPrjnState(net);
    if(!prjn || prjn->NotActive(net)) continue;
    LAYER_STATE* recv_lay = prjn->GetRecvLayer(net);
    LAYER_STATE* send_lay = prjn->GetSendLayer(net);
    LAYER_STATE* slay = (swt ? recv_lay : send_lay);

    if(slay->lesioned() || (send_lay == recv_lay)) continue; // no self prjns!!
    slay->SetLayerFlag(LAYER_STATE::PROJECT_WTS_NEXT);

    for(int ci = 0; ci < cg->size; ci++) {
      float wtv = cg->Cn(ci, ConState_cpp::WT, net);
      UnitState_cpp* su = cg->UnState(ci,net);
      su->wt_prjn += wtv;
      su->tmp_calc1 += 1.0f;  // sum to 1
    }
  }

  // now it is just the same loop until there are no more guys!
  bool got_some = false;
  do {
    got_some = false;
    for(int li=0; li < n_layers_built; li++) {
      LayerState_cpp* lay = GetLayerState(li);
      if(lay->lesioned() || !lay->HasLayerFlag(LAYER_STATE::PROJECT_WTS_NEXT)) continue;
      lay->SetLayerFlag(LAYER_STATE::PROJECT_WTS_DONE); // we're done!

      topk_un_vec.SetGeom(1, lay->n_units);
      // first normalize the weights on this guy
      float abs_max = 0.0f;
      int uidx = 0;
      for(int ui = 0; ui < lay->n_units; ui++) {
        UNIT_STATE* u = lay->GetUnitState(net, ui);
        if(u->lesioned()) continue;
        if(u->tmp_calc1 > 0.0f)
          u->wt_prjn /= u->tmp_calc1;
        abs_max = MAX(abs_max, fabsf(u->wt_prjn));
        topk_un_vec.FastEl_Flat(uidx) = u->wt_prjn;
        uidx++;
      }
      if(abs_max == 0.0f) abs_max = 1.0f;

      if(lay->layer_type == LAYER_STATE::HIDDEN && lay->n_ungps > 0 && top_k_gp > 0) {
        // units group version -- only for hidden layers..

        // pick the top k groups in terms of weighting for top-k guys from that group -- not all..
        float k_val = lay->n_ungps * top_k_un;
        float thr_eff = taMath_float::vec_kwta(&topk_un_vec, (int)k_val, true); // descending

        topk_gp_vec.SetGeom(1, lay->n_ungps);

        for(int gi=0;gi<lay->n_ungps;gi++) {
          float gp_val = 0.0f;
          for(int ui = 0; ui < lay->un_geom_n; ui++) {
            UNIT_STATE* u = lay->GetUnitStateGpUnIdx(net, gi, ui);
            if(u->lesioned()) continue;
            if(u->wt_prjn > thr_eff) // only for those above threshold
              gp_val += u->wt_prjn;
          }
          topk_gp_vec.FastEl_Flat(gi) = gp_val;
        }

        float gp_thr_eff = taMath_float::vec_kwta(&topk_gp_vec, top_k_gp, true); // descending
        for(int gi=0;gi<lay->n_ungps;gi++) {
          topk_un_vec.SetGeom(1, lay->un_geom_n);

          int tuidx = 0;
          for(int ui = 0; ui < lay->un_geom_n; ui++) {
            UNIT_STATE* u = lay->GetUnitStateGpUnIdx(net, gi, ui);
            if(u->lesioned()) continue;
            topk_un_vec.FastEl_Flat(tuidx) = u->wt_prjn;
            tuidx++;
          }

          thr_eff = taMath_float::vec_kwta(&topk_un_vec, top_k_un, true); // descending
          for(int ui = 0; ui < lay->un_geom_n; ui++) {
            UNIT_STATE* u = lay->GetUnitStateGpUnIdx(net, gi, ui);
            if(u->lesioned()) continue;
            float prjval = u->wt_prjn;
            u->wt_prjn /= abs_max;      // normalize --
            if((top_k_un > 0 && prjval < thr_eff) ||
               (topk_gp_vec.FastEl_Flat(gi) < gp_thr_eff)) {
              if(zero_sub_hiddens)
                u->wt_prjn = 0.0f; // these are always HIDDEN so no need to check.
              continue;
            }

            bool got = net_project_wts_propagate(net, u, swt);
            got_some |= got;
          }
        }
      }
      else {                                            // flat layer version
        float thr_eff = taMath_float::vec_kwta(&topk_un_vec, top_k_un, true); // descending

        for(int ui = 0; ui < lay->n_units; ui++) {
          UNIT_STATE* u = lay->GetUnitState(net, ui);
          if(u->lesioned()) continue;
          float prjval = u->wt_prjn;
          u->wt_prjn /= abs_max;        // normalize
          if(top_k_un > 0 && prjval < thr_eff) {
            if(lay->layer_type == LAYER_STATE::HIDDEN && zero_sub_hiddens)
              u->wt_prjn = 0.0f;
            continue; // bail
          }

          bool got = net_project_wts_propagate(net, u, swt);
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

void Network::SpecComparePeers(BaseSpec* key_spec, BaseSpec* peer_spec) {
  if (!key_spec || !peer_spec) return;
  
  String table_name = key_spec->name + "_spec_peer_table";
  DataTable* spec_table = (DataTable*)spec_tables.FindLeafName_(table_name);
  
  if (!spec_table) {  // create new table, add member column and key_spec column
    spec_table = spec_tables.NewEl(1, NULL);   // add a new data table to the group
    spec_table->SetName(table_name);
    spec_table->ClearDataFlag(DataTable::SAVE_ROWS); // don't save these
    spec_table->StructUpdate(true);
    
    DataCol* dc_member = (DataCol*)spec_table->FindMakeCol("Member", taBase::VT_STRING);
    dc_member->SetColFlag(DataCol::READ_ONLY);
    
    DataCol* dc_spec = (DataCol*)spec_table->FindMakeCol(key_spec->name, taBase::VT_STRING);
    dc_spec->SetColFlag(DataCol::READ_ONLY);
    spec_table->StructUpdate(false);
    spec_table->RefreshViews();
    
    WriteSpecMbrNamesToTable(spec_table, key_spec);
    WriteSpecMbrValsToTable(spec_table, key_spec, false, false);
  }
  else {  // do updates
    WriteSpecMbrNamesToTable(spec_table, key_spec);
    WriteSpecMbrValsToTable(spec_table, key_spec, false, false);
  }
  
  spec_table->StructUpdate(true);
  AddPeerToSpecCompareTable(spec_table, peer_spec);
  spec_table->StructUpdate(false);
  tabMisc::DelayedFunCall_gui(spec_table, "BrowserSelectMe");
}

void Network::AddPeerToSpecCompareTable(DataTable* spec_table, BaseSpec* peer_spec) {
  if (!spec_table->FindColName(peer_spec->name)) {
    spec_table->StructUpdate(true);
    DataCol* dc = (DataCol*)spec_table->FindMakeCol(peer_spec->name, taBase::VT_STRING);
    if (dc) {
      dc->SetColFlag(DataCol::READ_ONLY);
    }
    spec_table->StructUpdate(false);
  }
  // regardless update values
  WriteSpecMbrNamesToTable(spec_table, peer_spec);  // and any members not in first peer
  WriteSpecMbrValsToTable(spec_table, peer_spec, false, true); // not child, is peer
}

void Network::SpecCompareWithChildren(BaseSpec* parent_spec) {
  if (parent_spec->children.size == 0) {
    taMisc::Warning("Spec has no children - nothing to compare to");
    return;
  }
  
  String table_name = parent_spec->name + "_spec_table";
  DataTable* spec_table = (DataTable*)spec_tables.FindLeafName_(table_name);
  
  if (spec_table) {
    spec_table->StructUpdate(true);
    spec_table->RemoveAllCols();  // not worth dealing with specs that have been deleted, changed name, etc.
  }
  else {
    spec_table = spec_tables.NewEl(1, NULL);   // add a new data table to the group
    spec_table->SetName(table_name);
    spec_table->ClearDataFlag(DataTable::SAVE_ROWS); // don't save these
    spec_table->StructUpdate(true);
  }
  
  DataCol* dc_member = (DataCol*)spec_table->NewColString("Member");
  dc_member->SetColFlag(DataCol::READ_ONLY);
  DataCol* dc_spec = (DataCol*)spec_table->NewColString(parent_spec->name);
  dc_spec->SetColFlag(DataCol::READ_ONLY);
  spec_table->RefreshViews();
  
  WriteSpecMbrNamesToTable(spec_table, parent_spec);
  WriteSpecMbrValsToTable(spec_table, parent_spec, false, false);
  AddChildToSpecCompareTable(spec_table, parent_spec);
  spec_table->StructUpdate(false);
  tabMisc::DelayedFunCall_gui(spec_table, "BrowserSelectMe");
}

void Network::AddChildToSpecCompareTable(DataTable* spec_table, BaseSpec* spec) {
  FOREACH_ELEM_IN_GROUP(BaseSpec, child, spec->children) {
    DataCol* dc = (DataCol*)spec_table->NewColString(child->name);
    if (dc) {
      dc->SetColFlag(DataCol::READ_ONLY);
    }
    WriteSpecMbrNamesToTable(spec_table, child);  // and any members not in parent
    WriteSpecMbrValsToTable(spec_table, child, true, false); // rows already add by parent - pass false
    if (child->children.size > 0) {
      AddChildToSpecCompareTable(spec_table, child);  // recursion
    }
  }
}

void Network::WriteSpecMbrNamesToTable(DataTable* spec_table, BaseSpec* spec) {
  TypeDef* spec_td = spec->GetTypeDef();
  
  int index = spec_table->rows; // we are appending
  for(int m=0; m<spec_td->members.size; m++) {
    MemberDef* spec_td_md = spec_td->members.FastEl(m);
    TypeDef* spec_member_td = spec_td_md->type;
    if (spec_member_td->IsBool() || spec_member_td->IsString() || spec_member_td->IsInt() ||
        spec_member_td->IsFloat() || spec_member_td->IsVariant() || spec_member_td->IsEnum()) {
      DataCol* name_column = spec_table->data.FindName("Member");
      if (ShowSpecMember(spec_td_md, NULL)) {
        String name = spec_td_md->name;
        if (name_column->FindVal(name) == -1) {
          spec_table->AddBlankRow();
          spec_table->SetValAsVar(name, 0, index);
          index++;
        }
      }
    }
    else {
      for(int n=0; n<spec_member_td->members.size; n++) {
        MemberDef* spec_member_base_md = spec_member_td->members.FastEl(n);
        DataCol* name_column = spec_table->data.FindName("Member");
        if (ShowSpecMember(spec_td_md, spec_member_base_md)) {
          String name = spec_td_md->name + "_" + spec_member_base_md->name;
          if (name_column->FindVal(name) == -1) {
            spec_table->AddBlankRow();
            spec_table->SetValAsVar(name, 0, index);
            index++;
          }
        }
      }
    }
  }
}

bool Network::ShowSpecMember(MemberDef* spec_md, MemberDef* spec_member_md) {
  BaseSpec* base_spec = new BaseSpec;
  TypeDef* base_td = base_spec->GetTypeDef();
  if (base_td->members.FindName(spec_md->name)) {
    return false;
  }
  if (spec_md->IsInvisible() || spec_md->HasHiddenInline()) {
    return false;
  }
  if (spec_md->name.contains("lrate_sched")) {
    return false;
  }
  
  if (spec_member_md == NULL) {
    return true;
  }
  if (spec_member_md->IsInvisible() || spec_member_md->HasHiddenInline()) {
    return false;
  }
  if (spec_member_md->name == "user_data_") {
    return false;
  }
  return true;
}

void Network::WriteSpecMbrValsToTable(DataTable* spec_table, BaseSpec* spec, bool is_child, bool is_peer) {
  TypeDef* spec_td = spec->GetTypeDef();
  
  for(int m=0; m<spec_td->members.size; m++) {
    MemberDef* spec_td_md = spec_td->members.FastEl(m);
    TypeDef* spec_member_td = spec_td_md->type;
    if (spec_member_td->IsBool() || spec_member_td->IsString() || spec_member_td->IsInt() ||
        spec_member_td->IsFloat() || spec_member_td->IsVariant() || spec_member_td->IsEnum()) {
      if (ShowSpecMember(spec_td_md, NULL)) {
        if ((!is_child && !is_peer) || (is_child && spec->GetUnique(spec_td_md->name))) {
          DataCol* name_column = spec_table->data.FindName("Member");
          String name = spec_td_md->name;
          int row = name_column->FindVal(name);
          String value = spec_td_md->GetValStr(spec);
          spec_table->SetValAsVar(value, spec->name, row);
        }
        else if (!is_child && is_peer) {
          DataCol* name_column = spec_table->data.FindName("Member");
          String name = spec_td_md->name;
          int row = name_column->FindVal(name);
          String peer_value = spec_td_md->GetValStr(spec);
          String col_1_value = spec_table->GetValAsString(1, row);
          if (peer_value != col_1_value) {
            spec_table->SetValAsVar(peer_value, spec->name, row);
          }
        }
      }
    }
    else {
      for(int n=0; n<spec_member_td->members.size; n++) {
        MemberDef* spec_member_base_md = spec_member_td->members.FastEl(n);
        if (ShowSpecMember(spec_td_md, spec_member_base_md)) {
          SpecMemberBase* new_base = (SpecMemberBase*)spec_td->members.SafeEl(m)->GetOff(spec);
          // display value if member overrides
          if ((!is_child && !is_peer) || (is_child && spec->GetUnique(spec_td_md->name))) {
            // but check conditional show value
            if (ShowSpecMemberValue(spec_member_base_md, spec_member_td, new_base)) {
              DataCol* name_column = spec_table->data.FindName("Member");
              String name = spec_td_md->name + "_" + spec_member_base_md->name;
              int row = name_column->FindVal(name);
              String value = spec_member_base_md->GetValStr(new_base);
              spec_table->SetValAsVar(value, spec->name, row);
            }
          }
          else if (!is_child && is_peer) {
            // but check conditional show value
            if (ShowSpecMemberValue(spec_member_base_md, spec_member_td, new_base)) {
              DataCol* name_column = spec_table->data.FindName("Member");
              String name = spec_td_md->name + "_" + spec_member_base_md->name;
              int row = name_column->FindVal(name);
              String peer_value = spec_member_base_md->GetValStr(new_base);
              String col_1_value = spec_table->GetValAsString(1, row); // column one is the key spec
              if (peer_value != col_1_value) {
                spec_table->SetValAsVar(peer_value, spec->name, row);
              }
            }
          }
        }
      }
    }
  }
}

bool Network::ShowSpecMemberValue(MemberDef* spec_member_md, TypeDef* typ, taBase* base) {
  if (spec_member_md->GetCondOptTest("CONDSHOW", typ, base)) {
    return true;
  }
  return false;
}

Layer* Network::FindMakeLayer(const String& nm, TypeDef* td, bool& nw_itm, const String& alt_nm) {
  return layers.FindMakeLayer(nm, td, nw_itm, alt_nm);
}

Layer_Group* Network::FindMakeLayerGroup(const String& nm, TypeDef* td, bool& nw_itm, const String& alt_nm) {
  return layers.FindMakeLayerGroup(nm, td, nw_itm, alt_nm);
}

Layer_Group* Network::FindLayerGroup(const String& nm) {
  return (Layer_Group*)layers.gp.FindName(nm);
}

Projection* Network::FindMakePrjn(Layer* recv, Layer* send, ProjectionSpec* ps, ConSpec* cs, bool& nw_itm)
{
  nw_itm = false; // default, esp for early return
  Projection* use_prjn = NULL;
  for(int pi=0; pi < recv->projections.size; pi++) {
    Projection* prjn = recv->projections[pi];
    if(prjn->from.ptr() == send) {
      if((ps == NULL) && (cs == NULL)) {
        return prjn;
      }
      if((ps) && (prjn->spec.spec.ptr() != ps)) {
        use_prjn = prjn;
        break;
      }
      if((cs) && (prjn->con_spec.spec.ptr() != cs)) {
        use_prjn = prjn;
        break;
      }
      return prjn;
    }
  }
  if (use_prjn == NULL) {
    nw_itm = true;
    use_prjn = (Projection*)recv->projections.New(1);
  }
  use_prjn->SetCustomFrom(send);
  if(ps) {
    use_prjn->spec.SetSpec(ps);
  }
  if(cs) {
    use_prjn->SetConType(cs->min_obj_type);
    use_prjn->con_spec.SetSpec(cs);
  }
  return use_prjn;
}

Projection* Network::FindMakePrjnAdd(Layer* recv, Layer* send, ProjectionSpec* ps, ConSpec* cs, bool& nw_itm) {
 
  for(int pi=0; pi < recv->projections.size; pi++) {
    Projection* prjn = recv->projections[pi];
    if((prjn->from.ptr() == send)
       && ((ps == NULL) || (prjn->spec.spec.ptr() == ps) ||
           (prjn->spec.spec->InheritsFrom(TA_FullPrjnSpec) &&
            ps->InheritsFrom(TA_FullPrjnSpec)))
       && ((cs == NULL) || (prjn->con_spec.spec.ptr() == cs))) {
      nw_itm = false;
      return prjn;
    }
  }
  nw_itm = true;
  Projection* prjn = (Projection*)recv->projections.New(1);
  prjn->SetCustomFrom(send);
  if(ps) {
    prjn->spec.SetSpec(ps);
  }
  if(cs) {
    prjn->SetConType(cs->min_obj_type);
    prjn->con_spec.SetSpec(cs);
  }
  return prjn;
}

Projection* Network::FindMakeSelfPrjn(Layer* recv, ProjectionSpec* ps, ConSpec* cs, bool& nw_itm) {
  
  Projection* use_prjn = NULL;
  for(int pi=0; pi < recv->projections.size; pi++) {
    Projection* prjn = recv->projections[pi];
    if(prjn->from.ptr() == recv) {
      if((ps == NULL) && (cs == NULL)) {
        nw_itm = false;
        return prjn;
      }
      if((ps) && (prjn->spec.spec.ptr() != ps)) {
        use_prjn = prjn;
        break;
      }
      if((cs) && (prjn->con_spec.spec.ptr() != cs)) {
        use_prjn = prjn;
        break;
      }
      nw_itm = false;
      return prjn;
    }
  }
  nw_itm = true;
  if(use_prjn == NULL)
    use_prjn = (Projection*)recv->projections.New(1);
  use_prjn->from_type = Projection::SELF;
  use_prjn->from = recv;
  if(ps)
    use_prjn->spec.SetSpec(ps);
  if(cs)
    use_prjn->con_spec.SetSpec(cs);
  use_prjn->SigEmitUpdated();
  return use_prjn;
}

Projection* Network::FindMakeSelfPrjnAdd(Layer* recv, ProjectionSpec* ps, ConSpec* cs, bool& nw_itm) {

  for(int pi=0; pi < recv->projections.size; pi++) {
    Projection* prjn = recv->projections[pi];
    if((prjn->from.ptr() == recv)
       && ((ps == NULL) || (prjn->spec.spec.ptr() == ps))
       && ((cs == NULL) || (prjn->con_spec.spec.ptr() == cs))) {
      nw_itm = false;
      return prjn;
    }
  }
  nw_itm = true;
  Projection* prjn = (Projection*)recv->projections.New(1);
  prjn->from_type = Projection::SELF;
  prjn->from = recv;
  if(ps)
    prjn->spec.SetSpec(ps);
  if(cs)
    prjn->con_spec.SetSpec(cs);
  prjn->SigEmitUpdated();
  return prjn;
}

bool Network::RemovePrjn(Layer* recv, Layer* send, ProjectionSpec* ps, ConSpec* cs) {
  
  for(int pi=recv->projections.size-1; pi >= 0; pi--) {
    Projection* prjn = recv->projections[pi];
    if((prjn->from.ptr() == send)
       && ((ps == NULL) || (prjn->spec.spec.ptr() == ps) ||
           (prjn->spec.spec->InheritsFrom(TA_FullPrjnSpec) &&
            ps->InheritsFrom(TA_FullPrjnSpec)))
       && ((cs == NULL) || (prjn->con_spec.spec.ptr() == cs))) {
      recv->projections.RemoveEl(prjn);
      return true;
    }
  }
  return false;
}

taBase* Network::ChooseNew(taBase* origin, const String& choice_text) {
  Network* ntwrk = NULL;
  ProjectBase* prjn = GET_OWNER(origin, ProjectBase);  // who initiated the choice/new datatable call?
  if(prjn) {
    ntwrk = (Network*)prjn->networks.New(1);
  }
  return ntwrk;
}

void Network::BgRunKilled() {
  if(!HasNetFlag(SAVE_KILLED_WTS)) return;
  if(!IsBuiltIntact() || epoch < 1) return;
  String fname;
  if(file_name.nonempty()) {
    fname = file_name;
    if(fname.contains(".wts"))
      fname = fname.before(".wts",-1);
    fname += "_killed." + taMisc::LeadingZeros(epoch,4) + ".wts.gz";
  }
  else {
    fname = GetFileNameFmProject(".wts.gz", "_killed." + taMisc::LeadingZeros(epoch,4),
                                 "", false);
  }
  taMisc::Info("Saving final killed weights to:", fname);
  SaveWeights(fname);
}


//////////////////////////////////////////////////////
//                      CUDA


#if CUDA_COMPILE

bool Network::Cuda_MakeCudaNet() {
  if(cuda_net) return false;
  cuda_net = new Network_cuda;  // note: this will leak memory..
  return true;
}


void Network::Cuda_FreeNet() {
  if(!cuda_net) return;
  cuda_net->NetFree();
  cuda_unit_specs.Reset();
  cuda_con_specs.Reset();
}

void Network::Cuda_BuildNet() {
  if(taMisc::is_loading)        // ignore all the loading-time ones
    return;

  Cuda_MakeCudaNet();
  
  // note: we use thread 0 for all of these things b/c there is only 1 and that's it!
  // n_units_built on network includes first full unit, not in cuda side..
  
  cuda_net->NetAlloc
    (unit_state_size, n_units_built-1, n_layers_built, n_ungps_built,
     thrs_units_mem[0], thrs_lay_unit_idxs[0], thrs_ungp_unit_idxs[0],
     n_lay_stats, n_lay_stats_vars, thrs_lay_stats[0], RecvOwnsCons(),
     thrs_units_n_recv_cgps[0], thrs_units_n_send_cgps[0], n_recv_cgps, n_send_cgps, 
     thrs_recv_cgp_start[0], thrs_send_cgp_start[0],
     thrs_recv_cons_cnt[0], thrs_send_cons_cnt[0], 
     thrs_recv_cons_mem[0], thrs_send_cons_mem[0]);

  if(n_units_built == 0 || units_n_recv_cgps == 0) return;

  cuda.n_threads = cuda_net->SetCudaParams
    (cuda.min_threads, cuda.max_threads, cuda.cons_per_thread, thrs_own_cons_avg_size[0]);

  taMisc::Info("CUDA using:", String(cuda.n_threads),
               "threads for avg number of cons:", String(thrs_own_cons_avg_size[0]));

  Cuda_InitConStates();         // copy over con groups

  cuda_net->NetToDevice();  // copy everything over to the device
  
  Cuda_MakeUnitSpecs();         // make and copy to device -- also does bias specs as start to make con specs!
  Cuda_MakeConSpecs();

  // essential to copy AFTER making specs so spec indexes get copied over!
  cuda_net->OwnCons_HostToDevice(true); // sync
  cuda_net->UnitState_HostToDevice(true); // sync
}

void Network::Cuda_InitConStates() {
  const int cncgs = cuda_net->con_state_size;
  const int cgs = con_state_size;
  for(int i=0; i< n_recv_cgps; i++) {
    ConState_cpp* cg = (ConState_cpp*)ThrRecvConState(0, i);
    ConState_cuda* ccg = Network_cuda::GetConState_Flat
      (cuda_net->recv_cgp_mem_h, cncgs, i);
    memcpy((char*)ccg, (char*)cg, sizeof(ConState_core)); // copy core elements
    ccg->mem_idx = cg->mem_start - thrs_recv_cons_mem[0]; // index
    if(cg->OwnCons()) {
      ccg->cnmem_idx = cg->cnmem_start - thrs_recv_cons_mem[0]; // index
    }
  }
  for(int i=0; i< n_send_cgps; i++) {
    ConState_cpp* cg = (ConState_cpp*)ThrSendConState(0, i);
    ConState_cuda* ccg = Network_cuda::GetConState_Flat
      (cuda_net->send_cgp_mem_h, cncgs, i);
    memcpy((char*)ccg, (char*)cg, sizeof(ConState_core)); // copy core elements
    ccg->mem_idx = cg->mem_start - thrs_send_cons_mem[0]; // index
    if(cg->OwnCons()) {
      ccg->cnmem_idx = cg->cnmem_start - thrs_send_cons_mem[0]; // index
    }
  }
}

void Network::Cuda_MakeUnitSpecs() {
  cuda_unit_specs.Reset();
  for(int li=0; li< n_layers_built; li++) {
    int st_ui = ThrLayUnStart(0, li);
    int ed_ui = ThrLayUnEnd(0, li);
    Layer* lay = StateLayer(li);
    UnitSpec* us = lay->GetUnitSpec();
    int us_idx = cuda_unit_specs.FindEl(us);
    if(us_idx < 0) {
      cuda_unit_specs.Add(us); // does ref
      us_idx = cuda_unit_specs.size-1;
    }
    for(int ui=st_ui; ui < ed_ui; ui++) {
      UnitState_cuda* uv = cuda_net->GetUnitState
        (cuda_net->units_mem_h, cuda_net->unit_state_size, ui);
      uv->cuda_unit_spec_idx = us_idx;
    }
  }
  bool ok = cuda_net->AllocUnitSpecs(cuda_unit_specs.size);
  if(TestError(!ok, "Cuda_MakeUnitSpecs",
               "alloc of unit specs failed -- perhaps there are too many to fit in the 64k constant memory limit?  n_specs:", String(cuda_unit_specs.size), " size: ",
               String(cuda_net->unit_spec_size), " total: ",
               taMisc::GetSizeString(cuda_net->unit_spec_mem_tot))) {
    ClearIntact();
  }

  // now we start on the conspecs by getting all the bias specs
  cuda_con_specs.Reset();
  for(int usi=0; usi < cuda_unit_specs.size; usi++) {
    UnitSpec* us = (UnitSpec*)cuda_unit_specs[usi];
    UnitSpec_cuda* cuda_us = cuda_net->GetUnitSpec
      (cuda_net->unit_spec_mem_h, cuda_net->unit_spec_size, usi);
    ConSpec* bs = us->bias_spec;
    if(bs) {
      int cs_idx = cuda_con_specs.FindEl(bs);
      if(cs_idx < 0) {
        cuda_con_specs.Add(bs); // does ref
        cs_idx = cuda_con_specs.size-1;
      }
      cuda_us->bias_spec_idx = cs_idx;
    }
    else {
      cuda_us->bias_spec_idx = -1;
    }
  }

  Cuda_UpdateUnitSpecs();
}

void Network::Cuda_UpdateUnitSpecs() {
  if(cuda_unit_specs.size != cuda_net->n_unit_specs) {
    ClearIntact();
    taMisc::Error("number of unitspecs has changed from when network was built -- CUDA cannot continue running -- rebuild network!");
    return;
  }
     
  for(int usi=0; usi < cuda_unit_specs.size; usi++) {
    UnitSpec* us = (UnitSpec*)cuda_unit_specs[usi];
    UnitSpec_cuda* cuda_us = cuda_net->GetUnitSpec
      (cuda_net->unit_spec_mem_h, cuda_net->unit_spec_size, usi);
    Cuda_CopyUnitSpec(cuda_us, us);
  }

  cuda_net->UnitSpecs_HostToDevice();
}

void Network::Cuda_MakeConSpecs() {
  //   cuda_con_specs.Reset();  this was reset in unit specs for bias specs
  for(int li=0; li < n_layers_built; li++) {
    int st_ui = ThrLayUnStart(0, li);
    int ed_ui = ThrLayUnEnd(0, li);
    Layer* lay = StateLayer(li);

    // first doing recv
    for(int pi=0; pi < lay->projections.size; pi++) {
      Projection* prjn = lay->projections[pi];
      if(prjn->NotActive()) continue;
      ConSpec* cs = prjn->GetConSpec();
      int cs_idx = cuda_con_specs.FindEl(cs);
      if(cs_idx < 0) {
        cuda_con_specs.Add(cs); // does ref
        cs_idx = cuda_con_specs.size-1;
      }
      for(int ui=st_ui; ui < ed_ui; ui++) {
        // this is for recv as iterating over projections
        ConState_cuda* cg = cuda_net->GetUnConState
          (cuda_net->recv_cgp_mem_h, cuda_net->recv_cgp_start_h, cuda_net->con_state_size,
           ui, pi);
        cg->con_spec_idx = cs_idx;
      }
    }
    
    // then send
    for(int pi=0; pi < lay->send_prjns.size; pi++) {
      Projection* prjn = lay->send_prjns[pi];
      if(prjn->NotActive()) continue;
      ConSpec* cs = prjn->GetConSpec();
      int cs_idx = cuda_con_specs.FindEl(cs);
      if(cs_idx < 0) {
        cuda_con_specs.Add(cs); // does ref
        cs_idx = cuda_con_specs.size-1;
      }
      for(int ui=st_ui; ui < ed_ui; ui++) {
        // this is for recv as iterating over projections
        ConState_cuda* cg = cuda_net->GetUnConState
          (cuda_net->send_cgp_mem_h, cuda_net->send_cgp_start_h, cuda_net->con_state_size,
           ui, pi);
        cg->con_spec_idx = cs_idx;
      }
    }
  }
  bool ok = cuda_net->AllocConSpecs(cuda_con_specs.size);
  if(TestError(!ok, "Cuda_MakeConSpecs",
               "alloc of con specs failed -- perhaps there are too many to fit in the 64k constant memory limit?  n_specs:", String(cuda_con_specs.size), " size: ",
               String(cuda_net->con_spec_size), " total: ",
               taMisc::GetSizeString(cuda_net->con_spec_mem_tot))) {
    ClearIntact();
  }
  Cuda_UpdateConSpecs();
}

void Network::Cuda_UpdateConSpecs() {
  if(cuda_con_specs.size != cuda_net->n_con_specs) {
    ClearIntact();
    taMisc::Error("number of conspecs has changed from when network was built -- CUDA cannot continue running -- rebuild network!");
    return;
  }
     
  for(int csi=0; csi < cuda_con_specs.size; csi++) {
    ConSpec* cs = (ConSpec*)cuda_con_specs[csi];
    ConSpec_cuda* cuda_cs = cuda_net->GetConSpec
      (cuda_net->con_spec_mem_h, cuda_net->con_spec_size, csi);
    Cuda_CopyConSpec(cuda_cs, cs);
  }

  cuda_net->ConSpecs_HostToDevice();
}

void Network::Cuda_UnitStateToHost() {
  cuda_net->UnitState_DeviceToHost(true); // sync
}

void Network::Cuda_UnitStateToDevice() {
  cuda_net->UnitState_HostToDevice(true); // sync
}

void Network::Cuda_ConStateToHost() {
  cuda_net->OwnCons_DeviceToHost(true); // sync
}

void Network::Cuda_ConStateToDevice() {
  cuda_net->OwnCons_HostToDevice(true); // sync
}

void Network::Cuda_UpdateSpecs() {
  Cuda_UpdateUnitSpecs();
  Cuda_UpdateConSpecs();
}

String Network::Cuda_TimingReport(bool print) {
  String report = "CUDA timing report:\n\
numbers are average microseconds per call of a given type\n\n";
  // report << "send netin:  " << cuda_send_netin_time.ReportAvg(1.0e6) << "\n";
  // report << "compute dwt: " << cuda_compute_dwt_time.ReportAvg(1.0e6) << "\n";
  // report << "compute wt:  " << cuda_compute_wt_time.ReportAvg(1.0e6) << "\n";
  if(print)
    taMisc::Info(report);
  return report;
}

#else // NO CUDA_COMPILE

void Network::Cuda_UnitStateToHost() {
}

void Network::Cuda_UnitStateToDevice() {
}

void Network::Cuda_ConStateToHost() {
}

void Network::Cuda_ConStateToDevice() {
}

void Network::Cuda_UpdateSpecs() {
}

String Network::Cuda_TimingReport(bool print) {
  taMisc::Info("CUDA not compiled!");
  return "";
}

#endif // CUDA_COMPILE

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

TA_BASEFUNS_CTORS_DEFN(NetTiming);
TA_BASEFUNS_CTORS_DEFN(NetTiming_List);
TA_BASEFUNS_CTORS_DEFN(NetStatsSpecs);
TA_BASEFUNS_CTORS_DEFN(Network);

using namespace std;

void NetStatsSpecs::Initialize() {
  sse_unit_avg = false;
  sse_sqrt = false;
  cnt_err_tol = 0.0f;
  prerr = false;
}


taBrainAtlas_List* Network::brain_atlases = NULL;

void Network::Initialize() {
  specs.SetBaseType(&TA_BaseSpec);
  layers.SetBaseType(&TA_Layer);

  if(!brain_atlases)
    brain_atlases = &Network_Group::brain_atlases;

  unit_vars_type = &TA_UnitVars;
  con_group_type = &TA_ConGroup;

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

  sse = 0.0f;
  sum_sse = 0.0f;
  avg_sse = 0.0f;
  cnt_err = 0.0f;
  pct_err = 0.0f;
  pct_cor = 0.0f;

  cur_sum_sse = 0.0f;
  avg_sse_n = 0;
  cur_cnt_err = 0.0f;

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
  n_thrs_built = 0;
  unit_vars_built = NULL;
  con_group_built = NULL;
  con_group_size = 0;
  unit_vars_size = 0;
  n_units_built = 0;
  n_layers_built = 0;
  max_thr_n_units = 0;
  units_thrs = NULL;
  units_thr_un_idxs = NULL;
  thrs_n_units = NULL;
  thrs_unit_idxs = NULL;
  thrs_units_mem = NULL;
  thrs_lay_unit_idxs = NULL;

  units_n_recv_cgps = NULL;
  units_n_send_cgps = NULL;
  n_recv_cgps = 0;
  n_send_cgps = 0;

  thrs_units_n_recv_cgps = NULL;
  thrs_units_n_send_cgps = NULL;
  thrs_n_recv_cgps = NULL;
  thrs_n_send_cgps = NULL;

  thrs_recv_cgp_mem = NULL;
  thrs_send_cgp_mem = NULL;
  thrs_recv_cgp_start = NULL;
  thrs_send_cgp_start = NULL;
  
  thrs_recv_cons_cnt = NULL;
  thrs_send_cons_cnt = NULL;
  thrs_recv_cons_mem = NULL;
  thrs_send_cons_mem = NULL;

  thrs_own_cons_max_size = NULL;
  thrs_own_cons_avg_size = NULL;
  thrs_own_cons_max_vars = NULL;
  thrs_pct_cons_vec_chunked = NULL;

  thrs_tmp_chunks = NULL;
  thrs_tmp_not_chunks = NULL;
  thrs_tmp_con_mem =  NULL;

  thrs_send_netin_tmp = NULL;

#ifdef DMEM_COMPILE
  // dmem_trl_comm = ??
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

  taBase::Own(stats, this);
  taBase::Own(threads, this);

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
  taBase::Own(active_layers, this);

  brain_atlas = brain_atlases->FindNameContains("Talairach"); // default

  ClearNetFlag(BUILT);
  ClearNetFlag(INTACT);
  
#ifdef DMEM_COMPILE
  taBase::Own(dmem_trl_comm, this);
  taBase::Own(dmem_agg_sum, this);
  dmem_trl_comm.CommAll();
  DMem_InitAggs();
#endif

  NetTextUserData();

  inherited::InitLinks();
}

void Network::CutLinks() {
  if(!owner) return; // already replacing or already dead
#ifdef DMEM_COMPILE
  dmem_trl_comm.FreeComm();
#endif
  units_flat.Reset();
  threads.CutLinks();
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
  specs.CutLinks();
  proj = NULL;
  taBase::DelPointer((taBase**)&null_unit);
  inherited::CutLinks();
}

void Network::Copy_(const Network& cp) {
  // we just copy structure, not thread-allocated data
  RemoveUnits();

  specs = cp.specs;
  layers = cp.layers;

  auto_build = cp.auto_build;

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
  group_name = cp.group_name;
  trial_name = cp.trial_name;
  output_name = cp.output_name;

  sse = cp.sse;
  sum_sse = cp.sum_sse;
  avg_sse = cp.avg_sse;
  cnt_err = cp.cnt_err;
  pct_err = cp.pct_err;
  pct_cor = cp.pct_cor;

  cur_sum_sse = cp.cur_sum_sse;
  avg_sse_n = cp.avg_sse_n;
  cur_cnt_err = cp.cur_cnt_err;

  prerr = cp.prerr;
  sum_prerr = cp.sum_prerr;
  epc_prerr = cp.epc_prerr;

  usr1_save_fmt = cp.usr1_save_fmt;
  wt_save_fmt = cp.wt_save_fmt;

  max_disp_size = cp.max_disp_size;
  max_disp_size2d = cp.max_disp_size2d;

  UpdatePointers_NewPar((taBase*)&cp, this); // update all the pointers
  SyncSendPrjns();
  UpdatePrjnIdxs();             // fix the recv_idx and send_idx (not copied!)
  UpdateAllSpecs();
  Build();
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
    if(HasNetFlag(SAVE_UNITS) || HasNetFlag(SAVE_UNITS_FORCE)) {
      UpdateAllSpecs(true);       // need to fix up some things after a load with saved
      // todo: no longer supported!!!
    }
  }
  else {
    if(brain_atlas)
      brain_atlas_name = brain_atlas->name; // for later saving..
  }

  ClearNetFlag(SAVE_UNITS_FORCE); // might have been saved in on state from recover file or something!
}

void Network::UpdtAfterNetMod() {
  if(!HasNetFlag(BUILT)) return;
  if(!HasNetFlag(INTACT)) return; // already bad

  bool units_diff = 
    (n_units_built != units_flat.size) ||
    (n_thrs_built != threads.n_threads) ||
    (unit_vars_built != unit_vars_type) ||
    (con_group_built != con_group_type) ||
    (unit_vars_size != unit_vars_built->size);

  if(units_diff)
    ClearNetFlag(INTACT);

  // make sure active flags are updated on all connections, e.g., from lesions
  small_batch_n_eff = small_batch_n;
  if(small_batch_n_eff < 1) small_batch_n_eff = 1;
#ifdef DMEM_COMPILE
  DMem_UpdtWtUpdt();
#endif
}

void Network::CountCons() {
  n_units = 0;
  n_cons = 0;
  max_prjns = 1;
  FOREACH_ELEM_IN_GROUP(Layer, l, layers) {
    if(l->lesioned()) continue;
    n_cons += l->CountCons(this);
    n_units += l->units.leaves;
    max_prjns = MAX(l->projections.size, max_prjns);
  }
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

  // todo:  no can load units anyway!
  // if(old_load_cons) { // old dump format
  //   Connect();                    // needs an explicit connect to make everything
  //   FOREACH_ELEM_IN_GROUP(Layer, lay, layers) {
  //     FOREACH_ELEM_IN_GROUP(Unit, u, lay->units) {
  //       for(int g=0; g<u->recv.size; g++) {
  //         ConGroup* cg = u->recv.FastEl(g);
  //         cg->Dump_Load_Old_Cons(u, g);
  //       }
  //     }
  //   }
  //   old_load_cons = false;
  // }

  ClearNetFlag(SAVE_UNITS_FORCE);       // might have been saved in on state from recover file or something!

  //  Build();
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

bool Network::net_aligned_malloc(void** ptr, size_t sz) {
  // alignment -- 64 = 64 byte (not bit) -- this is needed for Phi MIC but not clear
  // that it is useful for AVX2??  anyway, better safe than sorry?
  if(sz > 1024) {
#ifdef TA_OS_WIN
    *ptr = _aligned_malloc(sz, 64);
#else
    posix_memalign(ptr, 64, sz);
#endif
  }
  else {                        // don't bother with align for small guys..
    *ptr = malloc(sz);
  }
  if(!*ptr) {
    taMisc::Error("Network::net_aligned_alloc memory allocation error! usually fatal -- please quit!  maybe your network is too big to fit into RAM?");
    return false;
  }
  return true;
}

bool Network::net_free(void** ptr) {
  if(ptr && *ptr) {
    free(*ptr);
    *ptr = NULL;
    return true;
  }
  return false;
}

void Network::Build() {
  taMisc::Busy();
  ++taMisc::no_auto_expand; // c'mon...!!! ;)
  StructUpdate(true);

  FreeUnitConGpThreadMem();     // free any and all existing memory!  must still have built params from before!

  //  CheckSpecs();
  BuildLayers(); // note: for Area constructs
  BuildPrjns(); // note: for Area constructs
  BuildUnits();

  n_units_built = units_flat.size;
  n_thrs_built = threads.n_threads;
  unit_vars_built = unit_vars_type;
  con_group_built = con_group_type;
  con_group_size = con_group_built->size;
  unit_vars_size = unit_vars_built->size;
  // all members are floats or ints, except the first pointer which is 2x..

  AllocUnitConGpThreadMem();

  Connect();

  AllocSendNetinTmp();

  if(taMisc::gui_active)	// only when gui is active..
    AssignVoxels();

  SetNetFlag(BUILT);
  SetNetFlag(INTACT);
  
  specs.RestorePanels();
  layers.RestorePanels();

  net_timing.SetSize(n_thrs_built + 1);

  StructUpdate(false);
  --taMisc::no_auto_expand;
  taMisc::DoneBusy();
}

void Network::CheckSpecs() {
  FOREACH_ELEM_IN_GROUP(Layer, l, layers) {
    l->CheckSpecs();
  }
}

void Network::BuildLayers() {
  taMisc::Busy();
  ++taMisc::no_auto_expand;
  StructUpdate(true);
  LayerPos_Cleanup();
  layers.BuildLayers(); // recurses

  active_layers.Reset();
  for(int i=0;i<layers.leaves; i++) {
    Layer* l = (Layer*)layers.Leaf(i);
    if(l->lesioned()) continue;
    active_layers.Add(i);
  }
  n_layers_built = active_layers.size;

  StructUpdate(false);
  --taMisc::no_auto_expand;
  taMisc::DoneBusy();
}

void Network::BuildUnits() {
  taMisc::Busy();
  StructUpdate(true);
  threads.InitAll();
  BuildNullUnit();
  FOREACH_ELEM_IN_GROUP(Layer, l, layers) {
    if(l->lesioned()) continue;
    l->BuildUnits();
  }

  BuildUnitsFlatList();

  StructUpdate(false);
  taMisc::DoneBusy();
}

void Network::BuildNullUnit() {
  // in derived classes, just replace the unit type with appropriate one -- do NOT call
  // inherited!
  if(!null_unit) {
    taBase::OwnPointer((taBase**)&null_unit, new Unit, this);
  }
}

void Network::BuildUnitsFlatList() {
  units_flat.Reset();
  // real indexes start at 1, to allow 0 to be a dummy case for inactive units that may
  // nevertheless get a send netin call to them -- all those just go to this 0 bin
  units_flat.Add(null_unit);         // add a dummy null
  FOREACH_ELEM_IN_GROUP(Layer, l, layers) {
    if(l->lesioned()) {
      l->units_flat_idx = 0;
      continue; // don't even add units from lesioned layers!!
    }
    l->BuildUnitsFlatList(this);
  }
}

#ifdef NUMA_COMPILE
void Network::FreeUnitConGpThreadMem_Thr(int thr_no) {
  // todo: super annoyingly this requires the size for the free method!
  numa_free(thrs_recv_cons_mem[i]); // go in reverse order: cons to units..
  numa_free(thrs_send_cons_mem[i]);
  numa_free(thrs_recv_cgp_mem[i]);
  numa_free(thrs_send_cgp_mem[i]);
  numa_free(thrs_units_n_recv_cgps[i]);
  numa_free(thrs_units_n_send_cgps[i]);
  numa_free(thrs_recv_cgp_start[i]);
  numa_free(thrs_send_cgp_start[i]);
  numa_free(thrs_unit_idxs[i]);
  numa_free(thrs_lay_unit_idxs[i]);
  numa_free(thrs_units_mem[i]);
}
#endif

void Network::FreeUnitConGpThreadMem() {
  if(!units_thrs) return; // nothing allocated yet -- otherwise assume EVERYTHING is

  FreeConThreadMem();           // this must go first!

#ifdef NUMA_COMPILE
  NET_THREAD_CALL(Network::FreeUnitConGpThreadMem_Thr);
#else
  for(int i=0; i<n_thrs_built; i++) {
     // go in reverse order: con gps to units..
    net_free((void**)&thrs_recv_cgp_start[i]);
    net_free((void**)&thrs_send_cgp_start[i]);

    net_free((void**)&thrs_recv_cgp_mem[i]);
    net_free((void**)&thrs_send_cgp_mem[i]);

    net_free((void**)&thrs_units_n_recv_cgps[i]);
    net_free((void**)&thrs_units_n_send_cgps[i]);

    net_free((void**)&thrs_unit_idxs[i]);
    net_free((void**)&thrs_lay_unit_idxs[i]);
    net_free((void**)&thrs_units_mem[i]);
  }
#endif

  // first all the doubly-allocated by-thread guys from above
  net_free((void**)&thrs_recv_cgp_start);
  net_free((void**)&thrs_send_cgp_start);

  net_free((void**)&thrs_recv_cgp_mem);
  net_free((void**)&thrs_send_cgp_mem);

  net_free((void**)&thrs_units_n_recv_cgps);
  net_free((void**)&thrs_units_n_send_cgps);

  net_free((void**)&thrs_unit_idxs);
  net_free((void**)&thrs_lay_unit_idxs);
  net_free((void**)&thrs_units_mem);

  // now go back and get the rest
  net_free((void**)&thrs_n_recv_cgps);
  net_free((void**)&thrs_n_send_cgps);

  net_free((void**)&units_n_recv_cgps);
  net_free((void**)&units_n_send_cgps);

  net_free((void**)&thrs_n_units);
  net_free((void**)&units_thr_un_idxs);
  net_free((void**)&units_thrs);
}

#ifdef NUMA_COMPILE
void Network::FreeConThreadMem_Thr(int thr_no) {
  // todo: super annoyingly this requires the size for the free method!
  numa_free(thrs_recv_cons_mem[i]); // go in reverse order: cons to units..
  numa_free(thrs_send_cons_mem[i]);
}
#endif

void Network::FreeConThreadMem() {
  if(!thrs_recv_cons_mem) return; // nothing allocated yet -- otherwise assume EVERYTHING is

#ifdef NUMA_COMPILE
  NET_THREAD_CALL(Network::FreeConThreadMem_Thr);
#else
  for(int i=0; i<n_thrs_built; i++) {
     // go in reverse order: cons to units..
    net_free((void**)&thrs_send_netin_tmp[i]);

    net_free((void**)&thrs_tmp_chunks[i]);
    net_free((void**)&thrs_tmp_not_chunks[i]);
    net_free((void**)&thrs_tmp_con_mem[i]);

    net_free((void**)&thrs_recv_cons_mem[i]);
    net_free((void**)&thrs_send_cons_mem[i]);
  }
#endif

  // first all the doubly-allocated by-thread guys from above
  net_free((void**)&thrs_send_netin_tmp);

  net_free((void**)&thrs_tmp_chunks);
  net_free((void**)&thrs_tmp_not_chunks);
  net_free((void**)&thrs_tmp_con_mem);

  net_free((void**)&thrs_recv_cons_mem);
  net_free((void**)&thrs_send_cons_mem);

  // now go back and get the rest
  net_free((void**)&thrs_own_cons_max_size);
  net_free((void**)&thrs_own_cons_avg_size);
  net_free((void**)&thrs_own_cons_max_vars);
  net_free((void**)&thrs_pct_cons_vec_chunked);

  net_free((void**)&thrs_recv_cons_cnt);
  net_free((void**)&thrs_send_cons_cnt);
}

void Network::AllocUnitConGpThreadMem() {
  // NUMA alloc notes:
  // absent any other special process, the first thread to touch (write) to a memory block
  // gets that memory allocated on its physical memory.  so we could simplify things
  // by doing all the malloc here in one method, and then ensure that the threads
  // initialize the memory, or we could do the malloc by thread, with proper numa calls
  // probably latter is preferable.  but we need a coherent overall threading framework
  // then -- can't be the UnitCall thing

  net_aligned_malloc((void**)&units_thrs, n_units_built * sizeof(int));
  net_aligned_malloc((void**)&units_thr_un_idxs, n_units_built * sizeof(int));
  net_aligned_malloc((void**)&thrs_n_units, n_thrs_built * sizeof(int));
  net_aligned_malloc((void**)&thrs_unit_idxs, n_thrs_built * sizeof(int*));
  net_aligned_malloc((void**)&thrs_lay_unit_idxs, n_thrs_built * sizeof(int*));
  net_aligned_malloc((void**)&thrs_units_mem, n_thrs_built * sizeof(float*));

  net_aligned_malloc((void**)&units_n_recv_cgps, n_units_built * sizeof(int));
  net_aligned_malloc((void**)&units_n_send_cgps, n_units_built * sizeof(int));

  net_aligned_malloc((void**)&thrs_units_n_recv_cgps, n_thrs_built * sizeof(int*));
  net_aligned_malloc((void**)&thrs_units_n_send_cgps, n_thrs_built * sizeof(int*));

  net_aligned_malloc((void**)&thrs_n_recv_cgps, n_thrs_built * sizeof(int));
  net_aligned_malloc((void**)&thrs_n_send_cgps, n_thrs_built * sizeof(int));

  net_aligned_malloc((void**)&thrs_recv_cgp_mem, n_thrs_built * sizeof(char*));
  net_aligned_malloc((void**)&thrs_send_cgp_mem, n_thrs_built * sizeof(char*));

  net_aligned_malloc((void**)&thrs_recv_cgp_start, n_thrs_built * sizeof(int*));
  net_aligned_malloc((void**)&thrs_send_cgp_start, n_thrs_built * sizeof(int*));

  max_thr_n_units = (n_units_built / n_thrs_built) + 2;

#ifdef NUMA_COMPILE
  NET_THREAD_CALL(Network::AllocUnitConGpThreadMem_Thr);
#else
  for(int i=0; i<n_thrs_built; i++) {
    net_aligned_malloc((void**)&thrs_unit_idxs[i], max_thr_n_units * sizeof(int));
    net_aligned_malloc((void**)&thrs_units_mem[i], max_thr_n_units * unit_vars_size);
    net_aligned_malloc((void**)&thrs_lay_unit_idxs[i], 2 * n_layers_built * sizeof(int));

    net_aligned_malloc((void**)&thrs_units_n_recv_cgps[i], max_thr_n_units * sizeof(int));
    net_aligned_malloc((void**)&thrs_units_n_send_cgps[i], max_thr_n_units * sizeof(int));

    net_aligned_malloc((void**)&thrs_recv_cgp_start[i], max_thr_n_units * sizeof(int));
    net_aligned_malloc((void**)&thrs_send_cgp_start[i], max_thr_n_units * sizeof(int));
  }
#endif

  n_recv_cgps = 0;
  n_send_cgps = 0;

  for(int i=0; i<n_thrs_built; i++) {
    thrs_n_units[i] = 0;
    thrs_n_recv_cgps[i] = 0;
    thrs_n_send_cgps[i] = 0;
  }

  for(int i=1; i< n_units_built; i++) {
    Unit* un = units_flat[i];
    int thr_no = (i-1) % n_thrs_built; // just increment sequentialy 0,1,2..n_threads-1
    units_thrs[i] = thr_no;
    int thr_un_idx = thrs_n_units[thr_no]++;
    if(TestError(thr_un_idx >= max_thr_n_units, "AllocUnitConGpThreadMem",
                 "Programmer error -- thr_un_idx >= max_thr_n_units -- please report!")) {
      return;
    }
    units_thr_un_idxs[i] = thr_un_idx;
    un->thr_un_idx = thr_un_idx;
    Layer* lay = un->own_lay();
    units_n_recv_cgps[i] = lay->n_recv_prjns;
    units_n_send_cgps[i] = lay->n_send_prjns;
    n_recv_cgps += lay->n_recv_prjns;
    n_send_cgps += lay->n_send_prjns;
  }

  NET_THREAD_CALL(Network::InitUnitThreadIdxs);

#ifdef NUMA_COMPILE
  NET_THREAD_CALL(Network::AllocUnitConGpThreadMem_Thr2);
#else
  for(int i=0; i<n_thrs_built; i++) {
    net_aligned_malloc((void**)&thrs_recv_cgp_mem[i], thrs_n_recv_cgps[i] *
                       con_group_size);
    net_aligned_malloc((void**)&thrs_send_cgp_mem[i], thrs_n_send_cgps[i] *
                       con_group_size);
  }
#endif

  NET_THREAD_CALL(Network::InitUnitConGpThreadMem);
}

int  Network::FindActiveLayerIdx(Layer* lay, const int st_idx) {
  int upi = st_idx + 1;
  int dni = st_idx;
  while(true) {
    bool upo = false;
    if(upi < n_layers_built) {
      if(ActiveLayer(upi) == lay) return upi;
      ++upi;
    }
    else {
      upo = true;
    }
    if(dni >= 0) {
      if(ActiveLayer(dni) == lay) return dni;
      --dni;
    }
    else if(upo) {
      break;
    }
  }
  return -1;
}

void Network::InitUnitThreadIdxs(int thr_no) {

  for(int i=0; i< 2*n_layers_built; i++) {
    thrs_lay_unit_idxs[thr_no][i] = -1;
  }

  int thr_un_idx = 0;
  int act_lay_idx = 0;
  int prv_act_lay_idx = -1;
  for(int i=1; i< n_units_built; i++) {
    int th = units_thrs[i];
    if(th != thr_no) continue;

    Unit* un = units_flat[i];
    if(TestError(un->thr_un_idx != thr_un_idx, "InitUnitThreadIdxs",
                 "Programmer error -- un->thr_un_idx != thr_un_idx -- please report!")) {
      return;
    }
    Layer* lay = un->own_lay();
    act_lay_idx = FindActiveLayerIdx(lay, act_lay_idx);
    if(act_lay_idx != prv_act_lay_idx) {
      thrs_lay_unit_idxs[thr_no][act_lay_idx * 2] = thr_un_idx; // start
      if(prv_act_lay_idx >= 0)
        thrs_lay_unit_idxs[thr_no][prv_act_lay_idx * 2 + 1] = thr_un_idx; // end of prev
      prv_act_lay_idx = act_lay_idx;
    }

    thrs_unit_idxs[thr_no][thr_un_idx] = i;
    thrs_units_n_recv_cgps[thr_no][thr_un_idx] = lay->n_recv_prjns;
    thrs_units_n_send_cgps[thr_no][thr_un_idx] = lay->n_send_prjns;

    thrs_recv_cgp_start[thr_no][thr_un_idx] = thrs_n_recv_cgps[thr_no];
    thrs_send_cgp_start[thr_no][thr_un_idx] = thrs_n_send_cgps[thr_no];

    thrs_n_recv_cgps[thr_no] += lay->n_recv_prjns;
    thrs_n_send_cgps[thr_no] += lay->n_send_prjns;

    thr_un_idx++;
  }
  if(prv_act_lay_idx >= 0)
    thrs_lay_unit_idxs[thr_no][prv_act_lay_idx * 2 + 1] = thr_un_idx; // end of prev
}

void Network::InitUnitConGpThreadMem(int thr_no) {
  // note: cannot just go over ThrsNRecvConGps(thr_no); b/c ConGroup doesn't have units etc yet

  int rcg_flags = ConGroup::RECV_CONS | ConGroup::IS_ACTIVE;
  int scg_flags = ConGroup::CG_0 | ConGroup::IS_ACTIVE;
  if(RecvOwnsCons()) {
    rcg_flags |= ConGroup::OWN_CONS;
  }
  else {
    scg_flags |= ConGroup::OWN_CONS;
  }

  const int nu = ThrNUnits(thr_no);
  for(int i=0; i < nu; i++) {
    int flt_idx = ThrUnitIdx(thr_no, i);
    Unit* un = UnFmIdx(flt_idx);
    Layer* lay = un->own_lay();

    UnitSpec* us = un->GetUnitSpec();
    UnitVars* uv = ThrUnitVars(thr_no, i);
    uv->unit_spec = us;
    uv->thr_un_idx = i;
    if(us) {
      us->Init_Vars(uv, this, thr_no);  // initialze -- causes this thread to own mem
    }

    int rcg_idx = 0;
    for(int j=0; j < lay->projections.size; j++) {
      Projection* prjn = lay->projections[j];
      if(!prjn->IsActive()) continue;
      ConGroup* rcg = ThrUnRecvConGroup(thr_no, i, rcg_idx);
      rcg->Initialize(rcg_flags, prjn, prjn->send_idx, flt_idx, i); 
      rcg_idx++;
    }

    int scg_idx = 0;
    for(int j=0; j < lay->send_prjns.size; j++) {
      Projection* prjn = lay->send_prjns[j];
      if(!prjn->IsActive()) continue;
      ConGroup* scg = ThrUnSendConGroup(thr_no, i, scg_idx);
      scg->Initialize(scg_flags, prjn, prjn->recv_idx, flt_idx, i); 
      scg_idx++;
    }
  }
}

#ifdef NUMA_COMPILE
void Network::AllocSendNetinTmp_Thr(int thr_no) {
}
#endif

void Network::AllocSendNetinTmp() {
  // temporary storage for sender-based netinput computation
  if(n_units_built == 0 || threads.n_threads == 0) return;

  net_aligned_malloc((void**)&thrs_send_netin_tmp, n_thrs_built * sizeof(float*));

#ifdef NUMA_COMPILE
  NET_THREAD_CALL(Network::AllocSendNetinTmp_Thr);
#else
  for(int i=0; i<n_thrs_built; i++) {
    if(NetinPerPrjn()) {
      net_aligned_malloc((void**)&thrs_send_netin_tmp[i],
                         n_units_built * max_prjns * sizeof(float));
    }
    else {
      net_aligned_malloc((void**)&thrs_send_netin_tmp[i],
                         n_units_built * sizeof(float));
    }
  }
#endif

  NET_THREAD_CALL(Network::InitSendNetinTmp_Thr);
}

void Network::InitSendNetinTmp_Thr(int thr_no) {
  if(NetinPerPrjn()) {
    memset(thrs_send_netin_tmp[thr_no], 0, n_units_built * max_prjns * sizeof(float));
  }
  else {
    memset(thrs_send_netin_tmp[thr_no], 0, n_units_built * sizeof(float));
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
  ++taMisc::no_auto_expand;
  StructUpdate(true);

  CheckSpecs();
  RemoveCons();
  SyncSendPrjns();

  Connect_Sizes();
  Connect_Alloc();
  Connect_Cons();

  NET_THREAD_CALL(Network::Connect_VecChunk_Thr);
  NET_THREAD_CALL(Network::Connect_UpdtActives_Thr);

  CountCons();
  UpdtAfterNetMod();

  // Init_Weights(); // not doing -- slow for large nets -- this is separate now

  StructUpdate(false);
  --taMisc::no_auto_expand;
  taMisc::DoneBusy();
}

void Network::Connect_Sizes() {
  // go in reverse order so that symmetric prjns can be made in
  // response to receiver-based projections
  Layer* l;
  int i;
  for(i=layers.leaves-1;i>=0;i--) {
    l = (Layer*)layers.Leaf(i);
    if(l->lesioned()) continue;
    l->Connect_Sizes(this);
  }
}

void Network::Connect_Alloc() {
  net_aligned_malloc((void**)&thrs_recv_cons_cnt, n_thrs_built * sizeof(int64_t));
  net_aligned_malloc((void**)&thrs_send_cons_cnt, n_thrs_built * sizeof(int64_t));

  net_aligned_malloc((void**)&thrs_recv_cons_mem, n_thrs_built * sizeof(float*));
  net_aligned_malloc((void**)&thrs_send_cons_mem, n_thrs_built * sizeof(float*));

  net_aligned_malloc((void**)&thrs_own_cons_max_size, n_thrs_built * sizeof(int));
  net_aligned_malloc((void**)&thrs_own_cons_avg_size, n_thrs_built * sizeof(int));
  net_aligned_malloc((void**)&thrs_own_cons_max_vars, n_thrs_built * sizeof(int));
  net_aligned_malloc((void**)&thrs_pct_cons_vec_chunked, n_thrs_built * sizeof(float));

  net_aligned_malloc((void**)&thrs_tmp_chunks, n_thrs_built * sizeof(int*));
  net_aligned_malloc((void**)&thrs_tmp_not_chunks, n_thrs_built * sizeof(int*));
  net_aligned_malloc((void**)&thrs_tmp_con_mem, n_thrs_built * sizeof(float*));

  NET_THREAD_CALL(Network::Connect_AllocSizes_Thr);

#ifndef NUMA_ALLOC              // NUMA_ALLOC did it already in the thread
  for(int thr_no=0; thr_no<n_thrs_built; thr_no++) {
    if(thrs_recv_cons_cnt[thr_no] > 0) {
      net_aligned_malloc((void**)&thrs_recv_cons_mem[thr_no],
                         thrs_recv_cons_cnt[thr_no] * sizeof(float));
    }
    else {
      thrs_recv_cons_mem[thr_no] = 0;
    }
    if(thrs_send_cons_cnt[thr_no] > 0) {
      net_aligned_malloc((void**)&thrs_send_cons_mem[thr_no],
                         thrs_send_cons_cnt[thr_no] * sizeof(float));
    }
    else {
      thrs_send_cons_mem[thr_no] = 0;
    }
    if(thrs_own_cons_max_size[thr_no] > 0) {
      net_aligned_malloc((void**)&thrs_tmp_chunks[thr_no],
                         thrs_own_cons_max_size[thr_no] * sizeof(int));
      net_aligned_malloc((void**)&thrs_tmp_not_chunks[thr_no],
                         thrs_own_cons_max_size[thr_no] * sizeof(int));
      net_aligned_malloc((void**)&thrs_tmp_con_mem[thr_no],
                      thrs_own_cons_max_size[thr_no] * (thrs_own_cons_max_vars[thr_no] + 1)
                         * sizeof(float));
    }
    else {
      thrs_tmp_chunks[thr_no] = 0;
      thrs_tmp_not_chunks[thr_no] = 0;
      thrs_tmp_con_mem[thr_no] = 0;
    }
  }
#endif

  NET_THREAD_CALL(Network::Connect_Alloc_Thr); // allocate to con groups
}

void Network::Connect_AllocSizes_Thr(int thr_no) {
  thrs_recv_cons_cnt[thr_no] = 0;
  thrs_send_cons_cnt[thr_no] = 0;
  thrs_own_cons_max_size[thr_no] = 0;
  thrs_own_cons_max_vars[thr_no] = 0;
  float ocsum = 0.0f;
  int ocn = 0;

  // recv cons
  const int nrcg = ThrNRecvConGps(thr_no);
  for(int i=0; i<nrcg; i++) {
    ConGroup* rcg = ThrRecvConGroup(thr_no, i); // guaranteed to be active..
    if(rcg->OwnCons()) {
      thrs_recv_cons_cnt[thr_no] += rcg->OwnMemReq();
      thrs_own_cons_max_size[thr_no] = MAX(thrs_own_cons_max_size[thr_no],
                                           rcg->alloc_size);
      thrs_own_cons_max_vars[thr_no] = MAX(thrs_own_cons_max_vars[thr_no],
                                           rcg->NConVars());
      ocsum += rcg->alloc_size;
      ocn++;
    }
    else {
      thrs_recv_cons_cnt[thr_no] += rcg->PtrMemReq();
    }
  }

  // send cons
  const int nscg = ThrNSendConGps(thr_no);
  for(int i=0; i<nscg; i++) {
    ConGroup* scg = ThrSendConGroup(thr_no, i); // guaranteed to be active..
    if(scg->OwnCons()) {
      thrs_send_cons_cnt[thr_no] += scg->OwnMemReq();
      thrs_own_cons_max_size[thr_no] = MAX(thrs_own_cons_max_size[thr_no],
                                           scg->alloc_size);
      thrs_own_cons_max_vars[thr_no] = MAX(thrs_own_cons_max_vars[thr_no],
                                           scg->NConVars());
      ocsum += scg->alloc_size;
      ocn++;
    }
    else {
      thrs_send_cons_cnt[thr_no] += scg->PtrMemReq();
    }
  }

  if(ocn > 0) {
    thrs_own_cons_avg_size[thr_no] = round(ocsum / (float)ocn);
  }

#ifdef NUMA_ALLOC
  // NOTE: this is doing the malloc within the thread..
  if(thrs_recv_cons_cnt[thr_no] > 0) {
    net_aligned_malloc((void**)&thrs_recv_cons_mem[thr_no],
                       thrs_recv_cons_cnt[thr_no] * sizeof(float));
  }
  else {
    thrs_recv_cons_mem[thr_no] = 0;
  }
  if(thrs_send_cons_cnt[thr_no] > 0) {
    net_aligned_malloc((void**)&thrs_send_cons_mem[thr_no],
                       thrs_send_cons_cnt[thr_no] * sizeof(float));
  }
  else {
    thrs_send_cons_mem[thr_no] = 0;
  }

  if(thrs_own_cons_max_size[thr_no] > 0) {
    net_aligned_malloc((void**)&thrs_tmp_chunks[thr_no],
                       thrs_own_cons_max_size[thr_no] * sizeof(int));
    net_aligned_malloc((void**)&thrs_tmp_not_chunks[thr_no],
                       thrs_own_cons_max_size[thr_no] * sizeof(int));
    net_aligned_malloc((void**)&thrs_tmp_con_mem[thr_no],
                       thrs_own_cons_max_size[thr_no] * (tmp_own_cons_max_vars[thr_no] + 1)
                       * sizeof(float));
  }
  else {
    thrs_tmp_chunks[thr_no] = 0;
    thrs_tmp_not_chunks[thr_no] = 0;
    thrs_tmp_con_mem[thr_no] = 0;
  }

#endif
}

void Network::Connect_Alloc_Thr(int thr_no) {
  // then dole it out to the units..
  int64_t thrs_recv_cons_idx = 0;
  int64_t thrs_send_cons_idx = 0;

  // recv cons
  const int nrcg = ThrNRecvConGps(thr_no);
  for(int i=0; i<nrcg; i++) {
    ConGroup* rcg = ThrRecvConGroup(thr_no, i); // guaranteed to be active..
    rcg->SetMemStart(thrs_recv_cons_mem[thr_no], thrs_recv_cons_idx);
    if(TestError(thrs_recv_cons_idx >= thrs_recv_cons_cnt[thr_no],
                 "Connect_Alloc_Thr",
                 "thrs_recv_cons_idx >= thrs_recv_cons_cnt[thr_no] -- programmer error -- please report!")) {
      return;
    }
    thrs_recv_cons_idx += rcg->MemReq();
  }

  // send cons
  const int nscg = ThrNSendConGps(thr_no);
  for(int i=0; i<nscg; i++) {
    ConGroup* scg = ThrSendConGroup(thr_no, i); // guaranteed to be active..
    scg->SetMemStart(thrs_send_cons_mem[thr_no], thrs_send_cons_idx);
    if(TestError(thrs_send_cons_idx >= thrs_send_cons_cnt[thr_no],
                 "Connect_Alloc_Thr",
                 "thrs_send_cons_idx >= thrs_send_cons_cnt[thr_no] -- programmer error -- please report!")) {
      return;
    }
    thrs_send_cons_idx += scg->MemReq();
  }
}

void Network::Connect_Cons() {
  // go in reverse order so that symmetric prjns can be made in
  // response to receiver-based projections
  Layer* l;
  int i;
  for(i=layers.leaves-1;i>=0;i--) {
    l = (Layer*)layers.Leaf(i);
    if(l->lesioned()) continue;
    l->Connect_Cons(this);
  }
}

void Network::Connect_VecChunk_Thr(int thr_no) {
  float pct_chunked = 0.0f;
  int   ncg = 0;
  if(RecvOwnsCons()) {
    const int nrcg = ThrNRecvConGps(thr_no);
    for(int i=0; i<nrcg; i++) {
      ConGroup* rcg = ThrRecvConGroup(thr_no, i); // guaranteed to be active..
      rcg->VecChunk_RecvOwns
        (this, thrs_tmp_chunks[thr_no], thrs_tmp_not_chunks[thr_no],
         thrs_tmp_con_mem[thr_no]);
      pct_chunked += rcg->VecChunkPct();
      ncg++;
    }
  }
  else {
    const int nscg = ThrNSendConGps(thr_no);
    for(int i=0; i<nscg; i++) {
      ConGroup* scg = ThrSendConGroup(thr_no, i); // guaranteed to be active..
      scg->VecChunk_SendOwns
        (this, thrs_tmp_chunks[thr_no], thrs_tmp_not_chunks[thr_no],
         thrs_tmp_con_mem[thr_no]);
      pct_chunked += scg->VecChunkPct();
      ncg++;
    }
  }

  if(ncg > 0) {
    thrs_pct_cons_vec_chunked[thr_no] = (pct_chunked / (float)ncg);
  }
  else {
    thrs_pct_cons_vec_chunked[thr_no] = 0.0f;
  }    
}

void Network::Connect_UpdtActives_Thr(int thr_no) {
  const int nrcg = ThrNRecvConGps(thr_no);
  for(int i=0; i<nrcg; i++) {
    ConGroup* rcg = ThrRecvConGroup(thr_no, i);
    rcg->UpdtIsActive();
  }
  const int nscg = ThrNSendConGps(thr_no);
  for(int i=0; i<nscg; i++) {
    ConGroup* scg = ThrSendConGroup(thr_no, i);
    scg->UpdtIsActive();
  }
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

void Network::UpdatePrjnIdxs() {
  FOREACH_ELEM_IN_GROUP(Layer, l, layers) {
    l->UpdatePrjnIdxs();
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
  ClearNetFlag(BUILT);
  ClearNetFlag(INTACT);
  RemoveCons();
  taMisc::Busy();
  StructUpdate(true);
  FOREACH_ELEM_IN_GROUP(Layer, l, layers) {
    l->RemoveUnits();
  }

  FreeUnitConGpThreadMem();
  n_units = 0;

  StructUpdate(false);
  taMisc::DoneBusy();
}

void Network::RemoveCons() {
  ClearNetFlag(BUILT);
  ClearNetFlag(INTACT);
  if(!thrs_n_recv_cgps) return; // cgps already gone
  taMisc::Busy();
  StructUpdate(true);
  for(int i=0; i<n_thrs_built; i++) { // don't use actual threading -- maybe destroying
    RemoveCons_Thr(i);
  }
  FreeConThreadMem();
  n_cons = 0;
  StructUpdate(false);
  taMisc::DoneBusy();
}

void Network::RemoveCons_Thr(int thr_no) {
  // recv cons
  const int nrcg = ThrNRecvConGps(thr_no);
  for(int i=0; i<nrcg; i++) {
    ConGroup* rcg = ThrRecvConGroup(thr_no, i); // guaranteed to be active..
    rcg->FreeCons();
  }

  // send cons
  const int nscg = ThrNSendConGps(thr_no);
  for(int i=0; i<nscg; i++) {
    ConGroup* scg = ThrSendConGroup(thr_no, i); // guaranteed to be active..
    scg->FreeCons();
  }
}

//              Above here are structural build/connect things
////////////////////////////////////////////////////////////////////////////
//              Below here are run-time computational routines

// notes on these compute loops:
// * ALL unit-level access should go through a thread-specific method that goes right
//   down to the unit level, and same for con-group levels -- do it in a separate pass
// * ConGroup loops should usually operate on the one that owns the cons -- 
//   check RecvOwnsCons() and proceed accordingly (algo-specific code should know and
//   just do the right thing directly)
// * if layer-level access is required, then do that separately -- and layer level 
//   should generally NOT then do the unit level (per above)
// * Exception to above: if the layer-level is specifically computing layer-level
//   aggregate stats over units, then that is most efficiently done by layer-level
// TODO: need to figure out a better soln for layer-level processing!!

void Network::Init_InputData() {
  FOREACH_ELEM_IN_GROUP(Layer, l, layers) {
    if(!l->lesioned())
      l->Init_InputData(this);
  }
  NET_THREAD_CALL(Network::Init_InputData_Thr);
}

void Network::Init_InputData_Thr(int thr_no) {
  const int nu = ThrNUnits(thr_no);
  for(int i=0; i<nu; i++) {
    UnitVars* uv = ThrUnitVars(thr_no, i);
    if(uv->lesioned()) continue;
    uv->unit_spec->Init_InputData(uv, this, thr_no);
  }
}

void Network::Init_Acts() {
  NET_THREAD_CALL(Network::Init_Acts_Thr);
}

void Network::Init_Acts_Thr(int thr_no) {
  const int nu = ThrNUnits(thr_no);
  for(int i=0; i<nu; i++) {
    UnitVars* uv = ThrUnitVars(thr_no, i);
    if(uv->lesioned()) continue;
    uv->unit_spec->Init_Acts(uv, this, thr_no);
  }
}

void Network::Init_dWt(){
  NET_THREAD_CALL(Network::Init_dWt_Thr);
}

void Network::Init_dWt_Thr(int thr_no) {
  if(RecvOwnsCons()) {
    const int nrcg = ThrNRecvConGps(thr_no);
    for(int i=0; i<nrcg; i++) {
      ConGroup* rcg = ThrRecvConGroup(thr_no, i);
      if(rcg->NotActive()) continue;
      rcg->con_spec->Init_dWt(rcg, this, thr_no);
    }
  }
  else {
    const int nscg = ThrNSendConGps(thr_no);
    for(int i=0; i<nscg; i++) {
      ConGroup* scg = ThrSendConGroup(thr_no, i);
      if(scg->NotActive()) continue;
      scg->con_spec->Init_dWt(scg, this, thr_no);
    }
  }
  // also unit-level, as separate pass
  const int nu = ThrNUnits(thr_no);
  for(int i=0; i<nu; i++) {
    UnitVars* uv = ThrUnitVars(thr_no, i);
    if(uv->lesioned()) continue;
    uv->unit_spec->Init_dWt(uv, this, thr_no);
  }
}

void Network::Init_Weights() {
  // do lots of checking here to make sure, cuz often 1st thing that happens
  //NOTE: this will typically be nested inside a gui check
  if (!CheckConfig(false)) return;

  taMisc::Busy();

  needs_wt_sym = false;          // will get set to true if needed

  // can't actually do this threaded because random number gen is not thread safe,
  // for the time being..
  NET_THREAD_LOOP(Network::Init_Weights_Thr);

  if(needs_wt_sym) {
    // taMisc::Info("Starting Init_Weights_sym...");
    NET_THREAD_CALL(Network::Init_Weights_sym);
  }
  // taMisc::Info("Starting Init_Weights_post...");
  NET_THREAD_CALL(Network::Init_Weights_post_Thr);

  // taMisc::Info("Starting Init_Weights_Layer..");
  Init_Weights_Layer();

  Init_Acts();                  // also re-init state at this point..
  Init_Metrics();
  UpdateAllViews();

  // taMisc::Info("Done Init_Weights...");

  SendWeightsToGPU();
  taMisc::DoneBusy();
}

void Network::Init_Weights_Thr(int thr_no) {
  if(RecvOwnsCons()) {
    const int nrcg = ThrNRecvConGps(thr_no);
    for(int i=0; i<nrcg; i++) {
      ConGroup* rcg = ThrRecvConGroup(thr_no, i);
      if(rcg->NotActive()) continue;
      if(rcg->prjn->spec->init_wts) {
        rcg->prjn->Init_Weights_Prjn(rcg, this, thr_no);
      }
      else {
        rcg->con_spec->Init_Weights(rcg, this, thr_no);
      }
    }
  }
  else {
    const int nscg = ThrNSendConGps(thr_no);
    for(int i=0; i<nscg; i++) {
      ConGroup* scg = ThrSendConGroup(thr_no, i);
      if(scg->NotActive()) continue;
      if(scg->prjn->spec->init_wts) continue; // do with recv's below
      scg->con_spec->Init_Weights(scg, this, thr_no);
    }
    const int nrcg = ThrNRecvConGps(thr_no);
    for(int i=0; i<nrcg; i++) {
      ConGroup* rcg = ThrRecvConGroup(thr_no, i);
      if(rcg->NotActive()) continue;
      if(rcg->prjn->spec->init_wts) {
        rcg->prjn->Init_Weights_Prjn(rcg, this, thr_no);
      }
    }
  }
  // also unit-level, as separate pass
  const int nu = ThrNUnits(thr_no);
  for(int i=0; i<nu; i++) {
    UnitVars* uv = ThrUnitVars(thr_no, i);
    if(uv->lesioned()) continue;
    uv->unit_spec->Init_Weights(uv, this, thr_no);
  }
}

void Network::Init_Weights_sym(int thr_no) {
  if(RecvOwnsCons()) {
    const int nrcg = ThrNRecvConGps(thr_no);
    for(int i=0; i<nrcg; i++) {
      ConGroup* rcg = ThrRecvConGroup(thr_no, i);
      if(rcg->NotActive()) continue;
      if(rcg->prjn->layer->units_flat_idx < rcg->prjn->from->units_flat_idx)
        continue;                 // higher copies from lower, so if we're lower, bail..
      rcg->con_spec->Init_Weights_sym_r(rcg, this, thr_no);
    }
  }
  else {
    const int nscg = ThrNSendConGps(thr_no);
    for(int i=0; i<nscg; i++) {
      ConGroup* scg = ThrSendConGroup(thr_no, i);
      if(scg->NotActive()) continue;
      if(scg->prjn->layer->units_flat_idx < scg->prjn->from->units_flat_idx)
        continue;                 // higher copies from lower, so if we're lower, bail..
      scg->con_spec->Init_Weights_sym_s(scg, this, thr_no);
    }
  }
}


void Network::Init_Weights_post() {
  NET_THREAD_CALL(Network::Init_Weights_post_Thr);
}

void Network::Init_Weights_post_Thr(int thr_no) {
  if(RecvOwnsCons()) {
    const int nrcg = ThrNRecvConGps(thr_no);
    for(int i=0; i<nrcg; i++) {
      ConGroup* rcg = ThrRecvConGroup(thr_no, i);
      if(rcg->NotActive()) continue;
      rcg->con_spec->Init_Weights_post(rcg, this, thr_no);
    }
  }
  else {
    const int nscg = ThrNSendConGps(thr_no);
    for(int i=0; i<nscg; i++) {
      ConGroup* scg = ThrSendConGroup(thr_no, i);
      if(scg->NotActive()) continue;
      scg->con_spec->Init_Weights_post(scg, this, thr_no);
    }
  }
  // also unit-level, as separate pass
  const int nu = ThrNUnits(thr_no);
  for(int i=0; i<nu; i++) {
    UnitVars* uv = ThrUnitVars(thr_no, i);
    if(uv->lesioned()) continue;
    uv->unit_spec->Init_Weights_post(uv, this, thr_no);
  }
}

void Network::Init_Weights_Layer() {
  FOREACH_ELEM_IN_GROUP(Layer, l, layers) {
    if(!l->lesioned())
      l->Init_Weights_Layer(this);
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

void Network::Compute_Netin() {
  NET_THREAD_CALL(Network::Compute_Netin_Thr);
}

void Network::Compute_Netin_Thr(int thr_no) {
  // unit-level, as separate pass -- this initializes net
  const int nu = ThrNUnits(thr_no);
  for(int i=0; i<nu; i++) {
    UnitVars* uv = ThrUnitVars(thr_no, i);
    if(uv->lesioned()) continue;
    uv->unit_spec->Compute_Netin(uv, this, thr_no);
  }
  // assuming recv owns -- otherwise use Send
  const int nrcg = ThrNRecvConGps(thr_no);
  for(int i=0; i<nrcg; i++) {
    ConGroup* rcg = ThrRecvConGroup(thr_no, i);
    if(rcg->NotActive()) continue;
    rcg->con_spec->Compute_Netin(rcg, this, thr_no);
  }
}

void Network::Send_Netin() {
  NET_THREAD_CALL(Network::Send_Netin);

  // now need to roll up the netinput into unit vals
  const int nu = n_units_built;
  const int nt = n_thrs_built;
  if(NetinPerPrjn()) {
    for(int i=1;i<nu;i++) {     // 0 = dummy idx
      float nw_nt = 0.0f;
      for(int p=0;p<UnNRecvConGps(i);p++) {
        for(int j=0;j<nt;j++) {
          float& ntmp = thrs_send_netin_tmp[nt][p * n_units_built + i];
          nw_nt += ntmp;
          ntmp = 0.0f; // reset immediately
        }
      }
      UnitVars* uv = UnUnitVars(i);
      uv->unit_spec->Compute_SentNetin(uv, this, nw_nt);
    }
  }
  else {
    for(int i=1;i<nu;i++) {     // 0 = dummy idx
      UnitVars* uv = UnUnitVars(i);
      float nw_nt = 0.0f;
      for(int j=0;j<nt;j++) {
        float& ntmp = thrs_send_netin_tmp[nt][i];
        nw_nt += ntmp;
        ntmp = 0.0f;
      }
      uv->unit_spec->Compute_SentNetin(uv, this, nw_nt);
    }
  }
}

void Network::Send_Netin_Thr(int thr_no) {
  // typically the whole point of using sender based net input is that you
  // want to check here if the sending unit's activation is above some threshold
  // so you don't send if it isn't above that threshold..
  // this isn't implemented here though.
  if(NetinPerPrjn()) {
    const int nscg = ThrNSendConGps(thr_no);
    for(int i=0; i<nscg; i++) {
      ConGroup* scg = ThrSendConGroup(thr_no, i);
      if(scg->NotActive()) continue;
      scg->con_spec->Send_Netin_PerPrjn(scg, this, thr_no);
    }
  }
  else {
    const int nscg = ThrNSendConGps(thr_no);
    for(int i=0; i<nscg; i++) {
      ConGroup* scg = ThrSendConGroup(thr_no, i);
      if(scg->NotActive()) continue;
      scg->con_spec->Send_Netin(scg, this, thr_no);
    }
  }
}

void Network::Compute_Act() {
  // todo: test performance of each..
  NET_THREAD_CALL(Network::Compute_Act_Thr);
}

void Network::Compute_Act_Thr(int thr_no) {
  const int nu = ThrNUnits(thr_no);
  for(int i=0; i<nu; i++) {
    UnitVars* uv = ThrUnitVars(thr_no, i);
    if(uv->lesioned()) continue;
    uv->unit_spec->Compute_Act(uv, this, thr_no);
  }
}

void Network::Compute_NetinAct() {
  NET_THREAD_CALL(Network::Compute_NetinAct_Thr);
}

void Network::Compute_NetinAct_Thr(int thr_no) {
  // todo: this needs to proceed in layer-level chunks!
  const int nu = ThrNUnits(thr_no);
  for(int i=0; i<nu; i++) {
    UnitVars* uv = ThrUnitVars(thr_no, i);
    if(uv->lesioned()) continue;
    uv->unit_spec->Compute_NetinAct(uv, this, thr_no);
  }
}

void Network::Compute_dWt() {
  NET_THREAD_CALL(Network::Compute_dWt_Thr);
}

void Network::Compute_dWt_Thr(int thr_no) {
  if(threads.get_timing)
    net_timing[thr_no]->dwt.StartTimer(true); // reset

  if(RecvOwnsCons()) {
    const int nrcg = ThrNRecvConGps(thr_no);
    for(int i=0; i<nrcg; i++) {
      ConGroup* rcg = ThrRecvConGroup(thr_no, i);
      if(rcg->NotActive()) continue;
      rcg->con_spec->Compute_dWt(rcg, this, thr_no);
    }
  }
  else {
    const int nscg = ThrNSendConGps(thr_no);
    for(int i=0; i<nscg; i++) {
      ConGroup* scg = ThrSendConGroup(thr_no, i);
      if(scg->NotActive()) continue;
      scg->con_spec->Compute_dWt(scg, this, thr_no);
    }
  }
  // also unit-level, as separate pass
  const int nu = ThrNUnits(thr_no);
  for(int i=0; i<nu; i++) {
    UnitVars* uv = ThrUnitVars(thr_no, i);
    if(uv->lesioned()) continue;
    uv->unit_spec->Compute_dWt(uv, this, thr_no);
  }

  if(threads.get_timing)
    net_timing[thr_no]->dwt.EndIncrAvg();
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
  NET_THREAD_CALL(Network::Compute_Weights_Thr);
}

void Network::Compute_Weights_Thr(int thr_no) {
  if(threads.get_timing)
    net_timing[thr_no]->wt.StartTimer(true); // reset

  if(RecvOwnsCons()) {
    const int nrcg = ThrNRecvConGps(thr_no);
    for(int i=0; i<nrcg; i++) {
      ConGroup* rcg = ThrRecvConGroup(thr_no, i);
      if(rcg->NotActive()) continue;
      rcg->con_spec->Compute_Weights(rcg, this, thr_no);
    }
  }
  else {
    const int nscg = ThrNSendConGps(thr_no);
    for(int i=0; i<nscg; i++) {
      ConGroup* scg = ThrSendConGroup(thr_no, i);
      if(scg->NotActive()) continue;
      scg->con_spec->Compute_Weights(scg, this, thr_no);
    }
  }
  // also unit-level, as separate pass
  const int nu = ThrNUnits(thr_no);
  for(int i=0; i<nu; i++) {
    UnitVars* uv = ThrUnitVars(thr_no, i);
    if(uv->lesioned()) continue;
    uv->unit_spec->Compute_Weights(uv, this, thr_no);
  }

  if(threads.get_timing)
    net_timing[thr_no]->wt.EndIncrAvg();
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
  if(sse > stats.cnt_err_tol)
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
  Compute_SSE(stats.sse_unit_avg, stats.sse_sqrt);
  if(stats.prerr)
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
  if(stats.prerr)
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

void Network::DMem_SumDWts(MPI_Comm comm) {
  wt_sync_time.StartTimer(false); // don't reset
  static float_Array values;
  static float_Array results;

  int np = 0; MPI_Comm_size(comm, &np);
  if(np <= 1) return;

  // note: memory is not contiguous for all DWT vars, so we still need to do this..

  values.SetSize(n_cons + n_units);

  int cidx = 0;
  FOREACH_ELEM_IN_GROUP(Layer, lay, layers) {
    if(lay->lesioned()) continue;
    FOREACH_ELEM_IN_GROUP(Unit, un, lay->units) {
      if(un->lesioned()) continue;
      values.FastEl(cidx++) = un->bias_wt();
      if(RecvOwnsCons()) {
        for(int g = 0; g < un->NRecvConGps(); g++) {
          ConGroup* cg = un->RecvConGroup(g);
          if(cg->NotActive()) continue;
          float* dwts = cg->OwnCnVar(ConGroup::DWT);
          memcpy(values.el + cidx, (char*)dwts, cg->size * sizeof(float));
          cidx += cg->size;
          // for(int i = 0;i<cg->size;i++) {
          //   values.FastEl(cidx++) = dwts[i];
          // }
        }
      }
      else {
        for(int g = 0; g < un->NSendConGps(); g++) {
          ConGroup* cg = un->SendConGroup(g);
          if(cg->NotActive()) continue;
          float* dwts = cg->OwnCnVar(ConGroup::DWT);
          memcpy(values.el + cidx, (char*)dwts, cg->size * sizeof(float));
          cidx += cg->size;
          // for(int i = 0;i<cg->size;i++) {
          //   values.FastEl(cidx++) = dwts[i];
          // }
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
        un->bias.OwnCn(0,ConGroup::DWT) = results.FastEl(cidx++);
      if(RecvOwnsCons()) {
        for(int g = 0; g < un->NRecvConGps(); g++) {
          ConGroup* cg = un->RecvConGroup(g);
          if(cg->NotActive()) continue;
          float* dwts = cg->OwnCnVar(ConGroup::DWT);
          memcpy(dwts, (char*)(results.el + cidx), cg->size * sizeof(float));
          cidx += cg->size;
          // for(int i = 0;i<cg->size;i++) {
          //   dwts[i] = results.FastEl(cidx++);
          // }
        }
      }
      else {
        for(int g = 0; g < un->NSendConGps(); g++) {
          ConGroup* cg = un->SendConGroup(g);
          if(cg->NotActive()) continue;
          float* dwts = cg->OwnCnVar(ConGroup::DWT);
          memcpy(dwts, (char*)(results.el + cidx), cg->size * sizeof(float));
          cidx += cg->size;
          // for(int i = 0;i<cg->size;i++) {
          //   dwts[i] = results.FastEl(cidx++);
          // }
        }
      }
    }
  }
  wt_sync_time.EndTimer();
}

void Network::DMem_ComputeAggs(MPI_Comm comm) {
  dmem_agg_sum.AggVar(comm, MPI_SUM);
}

#endif  // DMEM

void Network::Copy_Weights(const Network* src) {
  taMisc::Busy();
  GetWeightsFromGPU();
  Layer* l, *sl;
  taLeafItr i,si;
  for(l = (Layer*)layers.FirstEl(i), sl = (Layer*)src->layers.FirstEl(si);
      (l) && (sl);
      l = (Layer*)layers.NextEl(i), sl = (Layer*)src->layers.NextEl(si))
  {
    if(!l->lesioned() && !sl->lesioned())
      l->Copy_Weights(sl);
  }
  SendWeightsToGPU();
  UpdateAllViews();
  taMisc::DoneBusy();
}

void Network::SaveWeights_strm(ostream& strm, Network::WtSaveFormat fmt) {
  taMisc::Busy();
  GetWeightsFromGPU();
  if(fmt == NET_FMT) fmt = wt_save_fmt;

  strm << "<Fmt " << GetTypeDef()->GetEnumString("WtSaveFormat", fmt) << ">\n"
       << "<Name " << GetName() << ">\n"
       << "<Epoch " << epoch << ">\n";
  FOREACH_ELEM_IN_GROUP(Layer, l, layers) {
    if(l->lesioned()) continue;
    strm << "<Lay " << l->name << ">\n";
    l->SaveWeights_strm(strm, (ConGroup::WtSaveFormat)fmt);
    strm << "</Lay>\n";
  }
  taMisc::DoneBusy();
}

bool Network::LoadWeights_strm(istream& strm, bool quiet) {
  bool rval = false;
  String tag, val, enum_typ_nm;
  int stat = 0;
  ConGroup::WtSaveFormat fmt;
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

  fmt = (ConGroup::WtSaveFormat)TA_ConGroup.GetEnumVal(val, enum_typ_nm);

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
    ConGroup::LoadWeights_EndTag(strm, "Lay", tag, stat, quiet);
    if(stat != taMisc::TAG_END) break;
  }

  Init_Weights_post();
  NET_THREAD_CALL(Network::Connect_VecChunk_Thr); // re-chunk just to be sure, in case they moved around
  
  // could try to read end tag but what is the point?
  rval = true;
  UpdateAllViews();
  SendWeightsToGPU();
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
  GetWeightsFromGPU();
  FOREACH_ELEM_IN_GROUP(Layer, l, layers) {
    if(!l->lesioned())
      l->TransformWeights(trans);
  }
  SendWeightsToGPU();
  UpdateAllViews();
  taMisc::DoneBusy();
}

void Network::AddNoiseToWeights(const Random& noise_spec) {
  taMisc::Busy();
  GetWeightsFromGPU();
  FOREACH_ELEM_IN_GROUP(Layer, l, layers) {
    if(!l->lesioned())
      l->AddNoiseToWeights(noise_spec);
  }
  SendWeightsToGPU();
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

void Network::LesionAllLayers() {
  taMisc::Busy();
  StructUpdate(true);
  FOREACH_ELEM_IN_GROUP(Layer, l, layers) {
    l->Lesion();
  }
  StructUpdate(false);
  taMisc::DoneBusy();
  UpdtAfterNetMod();
}

void Network::IconifyAllLayers() {
  taMisc::Busy();
  StructUpdate(true);
  FOREACH_ELEM_IN_GROUP(Layer, l, layers) {
    l->Iconify();
  }
  StructUpdate(false);
  taMisc::DoneBusy();
  UpdtAfterNetMod();
}

void Network::UnLesionAllLayers() {
  taMisc::Busy();
  StructUpdate(true);
  FOREACH_ELEM_IN_GROUP(Layer, l, layers) {
    l->UnLesion();
  }
  StructUpdate(false);
  taMisc::DoneBusy();
  UpdtAfterNetMod();
}

void Network::DeIconifyAllLayers() {
  taMisc::Busy();
  StructUpdate(true);
  FOREACH_ELEM_IN_GROUP(Layer, l, layers) {
    l->DeIconify();
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
  GetWeightsFromGPU();
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
  GetWeightsFromGPU();
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
  for(int g = 0; g < (swt ? u->NSendConGps() : u->NRecvConGps()); g++) {
    ConGroup* cg = (swt ? u->SendConGroup(g) : u->RecvConGroup(g));
    Projection* prjn = cg->prjn;
    if(!prjn || prjn->NotActive()) continue;
    Layer* slay = (swt ? prjn->layer : prjn->from);

    if(slay->lesioned() || (prjn->from.ptr() == prjn->layer) ||
       slay->HasLayerFlag(Layer::PROJECT_WTS_DONE)) continue;
    slay->SetLayerFlag(Layer::PROJECT_WTS_NEXT); // next..
    got_some = true;                           // keep going..

    for(int ci = 0; ci < cg->size; ci++) {
      float wtv = cg->Cn(ci, ConGroup::WT, net);
      Unit* su = cg->Un(ci,net);
      su->wt_prjn += u->wt_prjn * wtv;
      su->tmp_calc1 += u->wt_prjn;
    }
  }
  return got_some;
}

void Network::ProjectUnitWeights(Unit* src_u, int top_k_un, int top_k_gp, bool swt,
                                 bool zero_sub_hiddens) {
  if(!src_u) return;

  GetWeightsFromGPU();

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
  for(int g = 0; g < (swt ? src_u->NSendConGps() : src_u->NRecvConGps()); g++) {
    ConGroup* cg = (swt ? src_u->SendConGroup(g) : src_u->RecvConGroup(g));
    Projection* prjn = cg->prjn;
    if(!prjn || prjn->NotActive()) continue;
    Layer* slay = (swt ? prjn->layer : prjn->from);

    if(slay->lesioned() || (prjn->from.ptr() == prjn->layer)) continue; // no self prjns!!
    slay->SetLayerFlag(Layer::PROJECT_WTS_NEXT);

    for(int ci = 0; ci < cg->size; ci++) {
      float wtv = cg->Cn(ci, ConGroup::WT, this);
      Unit* su = cg->Un(ci,this);
      su->wt_prjn += wtv;
      su->tmp_calc1 += 1.0f;  // sum to 1
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

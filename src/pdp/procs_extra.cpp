/* -*- C++ -*- */
/*=============================================================================
//									      //
// This file is part of the PDP++ software package.			      //
//									      //
// Copyright (C) 1995 Randall C. O'Reilly, Chadley K. Dawson, 		      //
//		      James L. McClelland, and Carnegie Mellon University     //
//     									      //
// Permission to use, copy, and modify this software and its documentation    //
// for any purpose other than distribution-for-profit is hereby granted	      //
// without fee, provided that the above copyright notice and this permission  //
// notice appear in all copies of the software and related documentation.     //
//									      //
// Permission to distribute the software or modified or extended versions     //
// thereof on a not-for-profit basis is explicitly granted, under the above   //
// conditions. 	HOWEVER, THE RIGHT TO DISTRIBUTE THE SOFTWARE OR MODIFIED OR  //
// EXTENDED VERSIONS THEREOF FOR PROFIT IS *NOT* GRANTED EXCEPT BY PRIOR      //
// ARRANGEMENT AND WRITTEN CONSENT OF THE COPYRIGHT HOLDERS.                  //
// 									      //
// Note that the taString class, which is derived from the GNU String class,  //
// is Copyright (C) 1988 Free Software Foundation, written by Doug Lea, and   //
// is covered by the GNU General Public License, see ta_string.h.             //
// The iv_graphic library and some iv_misc classes were derived from the      //
// InterViews morpher example and other InterViews code, which is             //
// Copyright (C) 1987, 1988, 1989, 1990, 1991 Stanford University             //
// Copyright (C) 1991 Silicon Graphics, Inc.				      //
//									      //
// THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,         //
// EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY 	      //
// WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  	      //
// 									      //
// IN NO EVENT SHALL CARNEGIE MELLON UNIVERSITY BE LIABLE FOR ANY SPECIAL,    //
// INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND, OR ANY DAMAGES  //
// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER OR NOT     //
// ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF LIABILITY,      //
// ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS        //
// SOFTWARE. 								      //
==============================================================================*/

// procs_extra.cc


#include "procs_extra.h"
#include "pdpshell.h"
//#include "net_qt.h"
#include "ta_filer.h"
#include "css_machine.h"

#include <limits.h>
#include <float.h>
//nn #include <unistd.h>


//////////////////////////////////
//	Basic Processes		//
//////////////////////////////////

void ScriptProcess::InitLinks() {
  Process::InitLinks();
  taBase::Own(s_args, this);
}

void ScriptProcess::Copy_(const ScriptProcess& cp) {
  s_args = cp.s_args;
}

void ScriptProcess::UpdateAfterEdit() {
  Process::UpdateAfterEdit();
  if((script_file) && !script_file->fname.empty()) {
    name = script_file->fname.before(".css");
    if(name.contains('/'))
      name = name.after('/', -1);
    int mx = MIN(s_args.size, 5);
    int i;
    for(i=0;i<mx;i++)
      name += String("_") + s_args[i];
  }
}

void ScriptProcess::Interact() {
  if(script == NULL)   return;
  cssMisc::next_shell = script;
}

void ScriptProcess::Compile() {
  LoadScript();
}

//////////////////////////////////
//	Save Nets Proc		//
//////////////////////////////////

void SaveNetsProc::C_Code() {
#ifdef DMEM_COMPILE
  if(taMisc::dmem_proc != 0) return;
#endif
  String batval = "0";
  Process_MGroup* procs = GET_MY_OWNER(Process_MGroup);
  if(procs != NULL) {
    BatchProcess* bproc = (BatchProcess*)procs->FindLeafType(&TA_BatchProcess);
    if(bproc != NULL)
      batval = taMisc::LeadingZeros(bproc->batch.val, 2);
  }
  String netnm = network->GetName() + "." + batval +
    "." + taMisc::LeadingZeros(network->epoch,3) + ".net" + taMisc::compress_sfx;
  taFiler* gf = taFiler_CreateInstance();		// this is all done for free in css..
  taRefN::Ref(gf);
  gf->fname = netnm;
  ostream* strm = gf->open_write();
  if((strm != NULL) && (strm->good()))
    network->Save(*strm);
  gf->Close();
  taRefN::unRefDone(gf);
}


//////////////////////////////////
//	Save Wts Proc		//
//////////////////////////////////

void SaveWtsProc::C_Code() {
  if(network == NULL) return;
  String batval = "0";
  BatchProcess* bproc = (BatchProcess*)GetMySProcOfType(&TA_BatchProcess);
  if(bproc == NULL) {
    Process_MGroup* procs = GET_MY_OWNER(Process_MGroup);
    if(procs != NULL)
      bproc = (BatchProcess*)procs->FindLeafType(&TA_BatchProcess);
  }
  if(bproc != NULL)
    batval = taMisc::LeadingZeros(bproc->batch.val, 2);

  String netnm = network->GetName() + "." + batval +
    "." + taMisc::LeadingZeros(network->epoch,3);
#ifdef DMEM_COMPILE
  if(taMisc::dmem_nprocs > 1) {
    int netsz = 0; MPI_Comm_size(network->dmem_share_units.comm, &netsz);
    if(taMisc::dmem_proc >= netsz) return; // only save from one networks worth of procs
    if(netsz > 1)
      netnm += String(".p") + String(taMisc::dmem_proc);
  }
#endif
  netnm += String(".wts") + taMisc::compress_sfx;
  taFiler* gf = taFiler_CreateInstance();		// this is all done for free in css..
  taRefN::Ref(gf);
  gf->fname = netnm;
  ostream* strm = gf->open_write();
  if((strm != NULL) && (strm->good()))
    network->WriteWeights(*strm, network->wt_save_fmt);
  gf->Close();
  taRefN::unRefDone(gf);
}

//////////////////////////////////
//	Load Wts Proc		//
//////////////////////////////////

void LoadWtsProc::C_Code() {
  taFiler* gf = taFiler_CreateInstance();		// this is all done for free in css..
  taRefN::Ref(gf);
  gf->fname = weights_file;
  istream* strm = gf->open_read();
  if((strm != NULL) && (strm->good()))
    network->ReadWeights(*strm);
  gf->Close();
  taRefN::unRefDone(gf);
}

//////////////////////////////////
//	Init Wts Proc		//
//////////////////////////////////

void InitWtsProc::C_Code() {
  if(network != NULL)
    network->InitWtState();
}

//////////////////////////////////
//	Schedule Processes	//
//////////////////////////////////

//////////////////////////
// 	SyncEpoch	//
//////////////////////////

void SyncEpochProc::Initialize() {
  second_proc_type = &TA_TrialProcess;
  second_proc = NULL;
  second_network = NULL;
}

void SyncEpochProc::Destroy() {
  CutLinks();
}

void SyncEpochProc::InitLinks() {
  EpochProcess::InitLinks();
}

void SyncEpochProc::CutLinks() {
  if(second_proc != NULL) {
    Process_MGroup* gp = GET_MY_OWNER(Process_MGroup);
    if(gp != NULL)
      gp->RemoveLeaf(second_proc);
  }
  taBase::DelPointer((TAPtr*)&second_proc);
  taBase::DelPointer((TAPtr*)&second_network);
  EpochProcess::CutLinks();
}

void SyncEpochProc::Copy_(const SyncEpochProc& cp) {
  second_proc_type = cp.second_proc_type;
  taBase::SetPointer((TAPtr*)&second_network, cp.second_network);
  if(cp.second_proc == NULL)
    taBase::SetPointer((TAPtr*)&second_proc, NULL);
  else {
    CreateSubProcs(false);	// make sure we have the right type..
    if(second_proc != NULL)
      second_proc->UnSafeCopy(cp.second_proc); // keep on copying..
  }
}

void SyncEpochProc::SetDefaultPNEPtrs() {
  EpochProcess::SetDefaultPNEPtrs();
  if(project == NULL) return;
  if(second_network == NULL)
    taBase::SetPointer((TAPtr*)&second_network, project->networks.DefaultEl());
}

SchedProcess* SyncEpochProc::FindSubProc(TypeDef* td) {
  SchedProcess* sp = sub_proc;
  while((sp != NULL) && !(sp->InheritsFrom(td)))
    sp = sp->sub_proc;
  if(sp == NULL) {
    sp = second_proc;
    while((sp != NULL) && !(sp->InheritsFrom(td)))
      sp = sp->sub_proc;
  }
  return sp;
}

void SyncEpochProc::CreateSubProcs(bool update) {
  EpochProcess::CreateSubProcs(false);
  if(second_proc_type == NULL)
    return;

  if((second_proc == NULL) || (second_proc->GetTypeDef() != second_proc_type)) {
    Process_MGroup* gp = GET_MY_OWNER(Process_MGroup);
    if(gp == NULL)
      return;

    if(second_proc != NULL) {
      gp->RemoveLeaf(second_proc);
      taBase::DelPointer((TAPtr*)&second_proc);
    }

    second_proc = (SchedProcess*)gp->NewEl(1, second_proc_type);
    taBase::Ref(second_proc);	// give it an extra reference for us
  }

  if(second_proc != NULL) {
    second_proc->super_proc = this;
    second_proc->CopyPNEPtrs(second_network, environment);
    second_proc->UpdateAfterEdit();
  }

  if(update)
    UpdateAfterEdit();
}

void SyncEpochProc::Loop() {
  if(sub_proc != NULL)
    sub_proc->Run();
  if(!bailing) {
    if(second_proc != NULL)
      second_proc->Run();
    if(!bailing) {
      if(wt_update == ON_LINE) {
	network->UpdateWeights();
	second_network->UpdateWeights();
      }
      else if((wt_update == SMALL_BATCH) && (((trial.val + 1) % batch_n) == 0)) {
	network->UpdateWeights();
	second_network->UpdateWeights();
      }
    }
  }
}

void SyncEpochProc::Init(){
  EpochProcess::Init();
  if(second_proc != NULL)		// set sub-procs to be re-inited when run
    second_proc->Init_flag();
}

void SyncEpochProc::Init_flag() {
  EpochProcess::Init_flag();
  if(second_proc != NULL)		// set sub-procs to be re-inited when run
    second_proc->Init_flag();
}

void SyncEpochProc::Init_force() {
  EpochProcess::Init_force();
  if(second_proc != NULL)		// force re-init of sub-procs
    second_proc->Init_impl();
}

void SyncEpochProc::Final() {
  if(wt_update == BATCH) {
    network->UpdateWeights();
    second_network->UpdateWeights();
  }
  else if((wt_update == SMALL_BATCH) && ((trial.val % batch_n) != 0)) {
    network->UpdateWeights();	// we didn't do this just before end of trial..
    second_network->UpdateWeights();	// we didn't do this just before end of trial..
  }
}


//////////////////////////////////
// 	GridSearchBatch		//
//////////////////////////////////

void GridSearchBatch::Initialize() {
  start_val = .1f;
  inc_val = .1f;
  cur_val = .1f;
}

void GridSearchBatch::Compute_CurVal() {
  cur_val = start_val + ((float)batch.val * inc_val);
}

void GridSearchBatch::SetParamVal() {
  if(param_path.empty()) return;
  if(param_path.lastchar() == ']') {
    String ppath = param_path.before('[', -1);
    MemberDef* md = NULL;
    TAPtr ary = ((TAPtr)(void*)project)->FindFromPath(ppath, md);
    if((md == NULL) || (ary == NULL)) {
      taMisc::Error("GridSearchBatch parameter path is invalid:", param_path);
      return;
    }
    String sidx = param_path.after('[', -1);
    sidx = sidx.before(']');
    int idx = (int)sidx;
    if(md->type->InheritsFrom(TA_String_Array)) {
      ((String_Array*)ary)->Set(idx, String(cur_val));
    }
    else if(md->type->InheritsFrom(TA_int_Array)) {
      ((int_Array*)ary)->Set(idx, (int)cur_val);
    }
    else if(md->type->InheritsFrom(TA_float_Array)) {
      ((float_Array*)ary)->Set(idx, cur_val);
    }
    else {
      taMisc::Error("GridSearchBatch parameter path does not point to float, int or String array:", param_path);
      return;
    }
    String par_path = ppath.before(".", -1);
    if(par_path.empty())	return;
    TAPtr par_par = (TAPtr)((TAPtr)(void*)project)->FindFromPath(par_path, md);
    if((par_par == NULL) || !(md->type->InheritsFrom(&TA_taBase)))
      return;
    par_par->UpdateAfterEdit();
  }
  else {
    String par_path = param_path.before('.', -1);
    MemberDef* md = NULL;
    TAPtr par_par = (TAPtr)((TAPtr)(void*)project)->FindFromPath(par_path, md);
    if((par_par == NULL) || !(md->type->InheritsFrom(&TA_taBase))) {
      taMisc::Error("GridSearchBatch can't find parent of parameter:", par_path);
      return;
    }
    String subpth = param_path.after('.', -1);
    void* param;
    md = par_par->FindMembeR(subpth, param);
    if((md == NULL) || (param == NULL)) {
      taMisc::Error("GridSearchBatch can't find:", subpth,"in object:", par_path);
      return;
    }
    if(md->type->InheritsFrom(TA_float))
      *((float*)param) = cur_val;
    else if(md->type->InheritsFrom(TA_int))
      *((int*)param) = (int)cur_val;
    else if(md->type->InheritsFrom(TA_taString))
      *((String*)param) = String(cur_val);
    else {
      taMisc::Error("GridSearchBatch parameter path does not point to float, int or String:", param_path);
      return;
    }
    par_par->UpdateAfterEdit();
  }
}

void GridSearchBatch::Init_impl() {
  BatchProcess::Init_impl();
  Compute_CurVal();
  SetParamVal();
}

void GridSearchBatch::Loop() {
  Compute_CurVal();
  SetParamVal();
  BatchProcess::Loop();
}

void GridSearchBatch::GetCntrDataItems() {
  cntr_items.EnforceSize(2);
  SchedProcess::GetCntrDataItems();
  DataItem* it = (DataItem*)cntr_items.FastEl(1);
  it->SetNarrowName("cur_val");
}

void GridSearchBatch::GenCntrLog(LogData* ld, bool gen) {
  SchedProcess::GenCntrLog(ld, gen);
  if(gen) {
    if(cntr_items.size < 2)
      GetCntrDataItems();
    ld->AddFloat(cntr_items.FastEl(1), cur_val);
  }
}


//////////////////////////////////
// 	SameSeedBatch		//
//////////////////////////////////

void SameSeedBatch::Initialize() {
  rnd_seeds.SetBaseType(&TA_RndSeed);
  in_goto_val = -1;
}

void SameSeedBatch::Destroy() {
  CutLinks();
}

void SameSeedBatch::InitLinks() {
  BatchProcess::InitLinks();
  taBase::Own(rnd_seeds, this);
}

void SameSeedBatch::CutLinks() {
  rnd_seeds.Reset();
  BatchProcess::CutLinks();
}

void SameSeedBatch::InitSeeds(int n_seeds) {
  RndSeed tmp_seed;
  tmp_seed.GetCurrent();
  rnd_seeds.EnforceSize(n_seeds);
  int i;
  for(i=0;i<n_seeds;i++) {
    RndSeed* seed = (RndSeed*)rnd_seeds[i];
    seed->NewSeed();
    // todo: this will break on DMEM unless seeds are loaded from a file
    // technically, these seeds need to be synced across procs, but this isn't
    // really worth it given how rare this would come up
  }
  tmp_seed.OldSeed();
}

void SameSeedBatch::UseCurrentSeed() {
  if(batch.val < rnd_seeds.size) {
    RndSeed* seed = (RndSeed*)rnd_seeds[batch.val];
    seed->OldSeed();
  }
  rndm_seed.GetCurrent();
  OldSeed();			// propagate and instantiate this seed
}

void SameSeedBatch::Init_impl() {
  BatchProcess::Init_impl();
  if(in_goto_val >= 0)
    batch.val = in_goto_val;
  UseCurrentSeed();
}

void SameSeedBatch::Loop() {
  // this is not good here: gets called every step!
  //  UseCurrentSeed();
  BatchProcess::Loop();
}

void SameSeedBatch::ReInit() {
  if(running) {
    taMisc::Choice("A process is running; Stop first, then you can ReInit.", "Ok");
    return;		// already running
  }
  if(!CheckAllTypes())		// do this before any actions in Init_force
    return;
  GoTo(0);			// make these equivalent..
}

void SameSeedBatch::GoTo(int goto_val) {
  batch.val = goto_val;
  UseCurrentSeed();
  in_goto_val = goto_val;
  Init_force();			// first initialize (gets all sub-guys inited too)
  in_goto_val = -1;
  if(cntr != NULL)
    cntr->val = goto_val;
  SetReInit(false);		// don't init again!
  UseCurrentSeed();
}

void SameSeedBatch::UpdateState() {
  BatchProcess::UpdateState();
  //  UseCurrentSeed();
  GoTo(batch.val);
}


//////////////////////////
// 	ForkProcess	//
//////////////////////////

void ForkProcess::Initialize() {
  second_proc_type = &TA_TrialProcess;
  second_proc = NULL;
  second_network = NULL;
  second_environment = NULL;
}

void ForkProcess::Destroy() {
  CutLinks();
}

void ForkProcess::InitLinks() {
  SchedProcess::InitLinks();
}

void ForkProcess::CutLinks() {
  if(second_proc != NULL) {
    Process_MGroup* gp = GET_MY_OWNER(Process_MGroup);
    if(gp != NULL)
      gp->RemoveLeaf(second_proc);
  }
  taBase::DelPointer((TAPtr*)&second_proc);
  taBase::DelPointer((TAPtr*)&second_network);
  taBase::DelPointer((TAPtr*)&second_environment);
  SchedProcess::CutLinks();
}

void ForkProcess::Copy_(const ForkProcess& cp) {
  second_proc_type = cp.second_proc_type;
  taBase::SetPointer((TAPtr*)&second_network, cp.second_network);
  taBase::SetPointer((TAPtr*)&second_environment, cp.second_environment);
  if(cp.second_proc == NULL)
    taBase::SetPointer((TAPtr*)&second_proc, NULL);
  else {
    CreateSubProcs(false);	// make sure we have the right type..
    if(second_proc != NULL)
      second_proc->UnSafeCopy(cp.second_proc); // keep on copying..
  }
}

SchedProcess* ForkProcess::FindSubProc(TypeDef* td) {
  SchedProcess* sp = sub_proc;
  while((sp != NULL) && !(sp->InheritsFrom(td)))
    sp = sp->sub_proc;
  if(sp == NULL) {
    sp = second_proc;
    while((sp != NULL) && !(sp->InheritsFrom(td)))
      sp = sp->sub_proc;
  }
  return sp;
}

void ForkProcess::SetDefaultPNEPtrs() {
  SchedProcess::SetDefaultPNEPtrs();
  if(project == NULL) return;
  if(second_network == NULL)
    taBase::SetPointer((TAPtr*)&second_network, project->networks.DefaultEl());
  if(second_environment == NULL)
    taBase::SetPointer((TAPtr*)&second_environment, project->environments.DefaultEl());
}

void ForkProcess::CreateSubProcs(bool update) {
  SchedProcess::CreateSubProcs(false);
  if(second_proc_type == NULL)
    return;

  if((second_proc == NULL) || (second_proc->GetTypeDef() != second_proc_type)) {
    Process_MGroup* gp = GET_MY_OWNER(Process_MGroup);
    if(gp == NULL)
      return;

    if(second_proc != NULL) {
      gp->RemoveLeaf(second_proc);
      taBase::DelPointer((TAPtr*)&second_proc);
    }

    second_proc = (SchedProcess*)gp->NewEl(1, second_proc_type);
    taBase::Ref(second_proc);	// give it an extra reference for us
  }

  if(second_proc != NULL) {
    second_proc->super_proc = this;
    second_proc->CopyPNEPtrs(second_network, second_environment);
    second_proc->UpdateAfterEdit();
  }

  if(update)
    UpdateAfterEdit();
}

void ForkProcess::Init(){
  SchedProcess::Init();
  if(second_proc != NULL)		// set sub-procs to be re-inited when run
    second_proc->Init_flag();
}

void ForkProcess::Init_flag() {
  SchedProcess::Init_flag();
  if(second_proc != NULL)		// set sub-procs to be re-inited when run
    second_proc->Init_flag();
}

void ForkProcess::Init_force() {
  SchedProcess::Init_force();
  if(second_proc != NULL)		// force re-init of sub-procs
    second_proc->Init_impl();
}

void ForkProcess::Loop() {
  if(sub_proc != NULL)
    sub_proc->Run();
  if(!bailing) {
    if(second_proc != NULL)
      second_proc->Run();
  }
}


//////////////////////////
// 	BridgeProcess	//
//////////////////////////

void BridgeProcess::Initialize() {
  second_network = NULL;
  src_layer = NULL;
  trg_layer = NULL;
  direction = ONE_TO_TWO;
  src_variable = "act";
  trg_variable = "ext";
  trg_ext_flag = Unit::EXT;
}

void BridgeProcess::Destroy() {
  CutLinks();
}

void BridgeProcess::InitLinks() {
  Process::InitLinks();
}

void BridgeProcess::CutLinks() {
  taBase::DelPointer((TAPtr*)&second_network);
  taBase::DelPointer((TAPtr*)&src_layer);
  taBase::DelPointer((TAPtr*)&trg_layer);
  Process::CutLinks();
}

void BridgeProcess::Copy_(const BridgeProcess& cp) {
  taBase::SetPointer((TAPtr*)&second_network, cp.second_network);
  taBase::SetPointer((TAPtr*)&src_layer, cp.src_layer);
  taBase::SetPointer((TAPtr*)&trg_layer, cp.trg_layer);
  direction = cp.direction;
  src_layer_nm = cp.src_layer_nm;
  trg_layer_nm = cp.trg_layer_nm;
  src_variable = cp.src_variable;
  trg_variable = cp.trg_variable;
  trg_ext_flag = cp.trg_ext_flag;
}

void BridgeProcess::UpdateAfterEdit() {
  Process::UpdateAfterEdit();
  SetLayerPtrs();
}

bool BridgeProcess::SetLayerPtrs() {
  taBase::DelPointer((TAPtr*)&src_layer);
  taBase::DelPointer((TAPtr*)&trg_layer);
  Network* net_1, *net_2;
  if(direction == ONE_TO_TWO) {
    net_1 = network;
    net_2 = second_network;
  }
  else {
    net_1 = second_network;
    net_2 = network;
  }
  if((net_1 == NULL) || (net_2 == NULL)) {
    taMisc::Error("Null network(s) in BridgeProcess:",name);
    return false;
  }
  Layer* lay_1 = (Layer*)net_1->layers.FindName(src_layer_nm);
  if(lay_1 == NULL) {
    taMisc::Error("BridgeProcess: layer:", src_layer_nm, "not found in network:",
		  net_1->GetName());
    return false;
  }
  Layer* lay_2 = (Layer*)net_2->layers.FindName(trg_layer_nm);
  if(lay_2 == NULL) {
    taMisc::Error("BridgeProcess: layer:", trg_layer_nm, "not found in network:",
		  net_2->GetName());
    return false;
  }
  taBase::SetPointer((TAPtr*)&src_layer, lay_1);
  taBase::SetPointer((TAPtr*)&trg_layer, lay_2);
  return true;
}

void BridgeProcess::C_Code() {
  if((src_layer == NULL) || (trg_layer == NULL) ||
     (src_layer->name != src_layer_nm) || (trg_layer->name != trg_layer_nm)) {
    if(!SetLayerPtrs())
      return;
  }
  MemberDef* smd = src_layer->units.el_typ->members.FindName(src_variable);
  if((smd == NULL) || !smd->type->InheritsFrom(TA_float)) {
    taMisc::Error("BridgeProcess: variable:", src_variable,
		  "not found or not a float on units of type:",
		  src_layer->units.el_typ->name);
    return;
  }
  MemberDef* tmd = trg_layer->units.el_typ->members.FindName(trg_variable);
  if((tmd == NULL) || !tmd->type->InheritsFrom(TA_float)) {
    taMisc::Error("BridgeProcess: variable:", trg_variable,
		  "not found or not a float on units of type:",
		  trg_layer->units.el_typ->name);
    return;
  }

  taLeafItr si, ti;
  Unit* su, *tu;
  for(su = src_layer->units.FirstEl(si), tu = trg_layer->units.FirstEl(ti);
      (su != NULL) && (tu != NULL);
      su = src_layer->units.NextEl(si), tu = trg_layer->units.NextEl(ti)) {
    *((float*)tmd->GetOff((void*)tu)) = *((float*)smd->GetOff((void*)su));
    if(trg_ext_flag != Unit::NO_EXTERNAL)
      tu->SetExtFlag(trg_ext_flag);
  }
}

//////////////////////////////////
// 	MultiEnvProcess		//
//////////////////////////////////

void MultiEnvProcess::UpdateAfterEdit() {
  SetCurEnvironment();		// calls regular update after edit.
}

void MultiEnvProcess::Initialize() {
  log_counter = true;
  env.SetMax(0);
  use_subctr_max = false;
}

void MultiEnvProcess::Destroy() {
  CutLinks();
}

void MultiEnvProcess::InitLinks() {
  SchedProcess::InitLinks();
  taBase::Own(environments, this);
  taBase::Own(env, this);
  taBase::Own(subctr_max, this);
  environments.SetBaseType(&TA_Environment);

  // add in any defaulted environment
  if((environments.size == 0) && (environment != NULL)) {
    environments.Link(environment);
  }
}

void MultiEnvProcess::CutLinks() {
  environments.RemoveAll();
  env.CutLinks();
  SchedProcess::CutLinks();
}

void MultiEnvProcess::Copy_(const MultiEnvProcess& cp) {
  environments.BorrowUnique(cp.environments);	// link group
  env = cp.env;
  use_subctr_max = cp.use_subctr_max;
  subctr_max = cp.subctr_max;
}

void MultiEnvProcess::SetCurEnvironment() {
  subctr_max.EnforceSize(environments.size);
  if(!cntr->Crit() && (cntr->val < environments.size)) {
    // an environment is available; set it
    taBase::SetPointer((TAPtr*)&environment, environments.FastEl(cntr->val));
    if(use_subctr_max && (sub_proc != NULL)) {
      sub_proc->cntr->SetMax(subctr_max.FastEl(cntr->val));
    }
  }
  else {
    // no environnment available; release it
    taBase::DelPointer((TAPtr*)&environment);
    environment = NULL;
  }

  if(sub_proc != NULL) {
    // ensure that lower procs have the new environment
    sub_proc->CopyPNEPtrs(network, environment);
    sub_proc->UpdateAfterEdit();
  }
  SchedProcess::UpdateAfterEdit();
}

void MultiEnvProcess::UpdateState() {
  SchedProcess::UpdateState();
  // reset all the environment pointers
  SetCurEnvironment();
}

void MultiEnvProcess::Init_impl(){
  SchedProcess::Init_impl();

  env.SetMax(environments.size);
  SetCurEnvironment();
}


//////////////////////////////////
//	PatternFlagProcess	//
//////////////////////////////////

void PatternFlagProcess::Initialize() {
  sub_proc_type = &TA_EpochProcess;
  log_counter = true;
  pattern_no = 0;
  flag = PatternSpec::NO_FLAGS;
  invert = false;
  val_idx.SetMax(0);
}

void PatternFlagProcess::InitLinks() {
  SchedProcess::InitLinks();
  taBase::Own(val_idx, this);
}

void PatternFlagProcess::Copy_(const PatternFlagProcess& cp) {
  pattern_no = cp.pattern_no;
  flag = cp.flag;
  invert = cp.invert;
  val_idx.Copy(cp.val_idx);
}

void PatternFlagProcess::UpdateAfterEdit() {
  SchedProcess::UpdateAfterEdit();

  // make sure we are using flags
  if(environment == NULL) return;

  EventSpec* es = (EventSpec*) environment->event_specs.DefaultEl();
  if(es == NULL) return;
  PatternSpec* ps = (PatternSpec*) es->patterns.SafeEl(pattern_no);
  if(ps == NULL) return;
  switch (ps->use_flags) {
  case PatternSpec::USE_NO_FLAGS:
    ps->use_flags = PatternSpec::USE_GLOBAL_FLAGS;
    ps->UpdateAllEvents();
    break;
  case PatternSpec::USE_PATTERN_FLAGS:
    ps->use_flags = PatternSpec::USE_PAT_THEN_GLOBAL_FLAGS;
    ps->UpdateAllEvents();
    break;
  default:
    break;
  }
}

void PatternFlagProcess::SetCurFlags(int* old_flag) {
  if (environment == NULL) return;
  EventSpec* es = (EventSpec*) environment->event_specs.DefaultEl();
  if(es == NULL) return;
  PatternSpec* ps = (PatternSpec*) es->patterns.SafeEl(pattern_no);
  if((ps == NULL) || (val_idx.val >= ps->n_vals)) return;

  // set the flags as appropriate
  if(old_flag != NULL) *old_flag = ps->global_flags[val_idx.val];
  if(invert) ps->global_flags[val_idx.val] &= ~flag;
  else ps->global_flags[val_idx.val] |= flag;
}

void PatternFlagProcess::ClearCurFlags(int* old_flag) {
  if (environment == NULL) return;
  EventSpec* es = (EventSpec*) environment->event_specs.DefaultEl();
  if(es == NULL) return;
  PatternSpec* ps = (PatternSpec*) es->patterns.SafeEl(pattern_no);
  if(ps == NULL) return;

  // unset the flags as appropriate
  if (old_flag) {
    ps->global_flags[val_idx.val] = *old_flag;
  }
  else {
    if (invert) ps->global_flags[val_idx.val] |= flag;
    else ps->global_flags[val_idx.val] &= ~flag;
  }
}

void PatternFlagProcess::Loop() {
  int old_flag;
  SetCurFlags(&old_flag);
  SchedProcess::Loop();
  ClearCurFlags(&old_flag);
}

void PatternFlagProcess::Init_impl() {
  SchedProcess::Init_impl();

  // default
  val_idx.SetMax(0);

  if(environment != NULL) {
    EventSpec* es = (EventSpec*) environment->event_specs.DefaultEl();
    if(es != NULL) {
      PatternSpec* ps = (PatternSpec*) es->patterns.SafeEl(pattern_no);
      if(ps != NULL)
	val_idx.SetMax(ps->n_vals);
    }
  }
}


//////////////////////////////////
//	Stats Processes		//
//////////////////////////////////

//////////////////////////////////
// 	ClosestEventStat	//
//////////////////////////////////

void ClosestEventStat::Initialize() {
  trial_proc = NULL;
  cmp_type = float_RArray::SUM_SQUARES;
  dist_tol = 0.0f;
  norm = false;
  subgp_no = -1;
  net_agg.op = Aggregate::MIN;
}

void ClosestEventStat::InitLinks() {
  Stat::InitLinks();
  if(network != NULL) {
    taBase::SetPointer((TAPtr*)&layer, network->layers.DefaultEl());
  }
  else layer = NULL;
  taBase::Own(lay_act, this);
}

void ClosestEventStat::CutLinks() {
  trial_proc = NULL;
  Stat::CutLinks();
}

void ClosestEventStat::UpdateAfterEdit() {
  Stat::UpdateAfterEdit();
  if(own_proc == NULL) return;
  trial_proc = (TrialProcess*)own_proc->FindProcOfType(&TA_TrialProcess);
}

void ClosestEventStat::Copy_(const ClosestEventStat& cp) {
  cmp_type = cp.cmp_type;
  dist_tol = cp.dist_tol;
  norm = cp.norm;
  subgp_no = cp.subgp_no;
  dist = cp.dist;
  ev_nm = cp.ev_nm;
  sm_nm = cp.sm_nm;
}

void ClosestEventStat::CopyPNEPtrs(Network* net, Environment*) {
  taBase::SetPointer((TAPtr*)&network, net);
}

void ClosestEventStat::NameStatVals() {
  if(time_agg.from != NULL) {
    Stat::NameStatVals();
    return;
  }
  // don't include net_agg!
  String misc_nm;
  if(layer != NULL) {
    misc_nm = layer->name;
    if(misc_nm.length() > 4)
      misc_nm = misc_nm.before(4);
  }
  String nm;
  if(!misc_nm.empty())    nm = misc_nm + "_dist";  else    nm = "dist";
  dist.SetName(nm);
  if(!misc_nm.empty())    nm = misc_nm + "_sm_nm";  else    nm = "sm_nm";
  sm_nm.SetNarrowName(nm);
  dist.AddDispOption("MIN=0");
  ev_nm.SetStringName("ev_nm");
}

void ClosestEventStat::InitStat() {
  dist.InitStat(InitStatVal());
  sm_nm.InitStat(InitStatVal());
  ev_nm.str_val = "";
  InitStat_impl();
}

void ClosestEventStat::Init() {
  if(loop_init == NO_INIT) return;
  dist.Init();
  Init_impl();
}

bool ClosestEventStat::Crit() {
  if(!has_stop_crit)    return false;
  if(n_copy_vals > 0)   return copy_vals.Crit();
  return dist.Crit();
}

void ClosestEventStat::Layer_Run() {
  if(layer == NULL) 	return;
  if(!CheckLayerInNet()) return;
  if(layer->lesion)	return;
//   Layer_Init(layer);
//   Unit_Run(layer);
  Layer_Stat(layer);
}

void ClosestEventStat::Layer_Stat(Layer* lay) {
  if(environment == NULL) return;

  lay_act.Reset();
  if(subgp_no >= 0) {
    Unit_Group* ug = (Unit_Group*)lay->units.SafeGp(subgp_no);
    if(ug != NULL) {
      Unit* un;
      taLeafItr i;
      FOR_ITR_EL(Unit, un, ug->, i)
	lay_act.Add(un->act);
    }
    else {
      taMisc::Error("*** ClosestEventStat: Sub group number:", String(subgp_no),
		    "not found in layer", lay->name);
      return;
    }
  }
  else {
    Unit* un;
    taLeafItr i;
    FOR_ITR_EL(Unit, un, lay->units., i)
      lay_act.Add(un->act);
  }

  bool larger_further = float_RArray::LargerFurther(cmp_type);
  if(larger_further)
    dist.val = FLT_MAX;
  else
    dist.val = -FLT_MAX;

  taLeafItr ei;
  Event* ev;
  FOR_ITR_EL(Event, ev, environment->events., ei) {
    Pattern* pat;
    PatternSpec* ps;
    taLeafItr pi, psi;
    FOR_ITR_PAT_SPEC(Pattern, pat, ev->patterns., pi, PatternSpec, ps, ev->spec->patterns., psi) {
      // only skip if multiple event patterns, else assume it must be correct..
      if((ps->layer_name != lay->name) && (ev->patterns.leaves > 1))
	continue;

      float rval = pat->value.Dist(lay_act, cmp_type, norm, dist_tol);

      bool closer;
      if(larger_further)
	closer = (rval < dist.val);
      else
	closer = (rval > dist.val);

      if(closer) {
	dist.val = rval;
	ev_nm.str_val = ev->name;
      }
    }
  }
  if(dist.val == 1.0e23f)
    dist.val = 0.0f;

  sm_nm.val = 1.0f;
  if((trial_proc != NULL) && (trial_proc->cur_event != NULL)) {
    if(trial_proc->cur_event->name == ev_nm.str_val)
      sm_nm.val = 0.0f;
  }
}


//////////////////////////////////
// 	CyclesToSettle		//
//////////////////////////////////

void CyclesToSettle::Initialize() {
  settle = NULL;
  net_agg.op = Aggregate::LAST;
}

void CyclesToSettle::InitLinks() {
  Stat::InitLinks();
  GetSettleProc();
}

void CyclesToSettle::NameStatVals() {
  Stat::NameStatVals();
  cycles.AddDispOption("MIN=0");
  cycles.AddDispOption("TEXT");
}

void CyclesToSettle::UpdateAfterEdit() {
  Stat::UpdateAfterEdit();
  GetSettleProc();
}

void CyclesToSettle::GetSettleProc() {
  settle = (SettleProcess*)GetMySProcOfType(&TA_SettleProcess);
}

void CyclesToSettle::CutLinks() {
  Stat::CutLinks();
  settle = NULL;
}

void CyclesToSettle::Copy_(const CyclesToSettle& cp) {
  cycles = cp.cycles;
}

void CyclesToSettle::InitStat() {
  cycles.InitStat(InitStatVal());
  InitStat_impl();
}

void CyclesToSettle::Init() {
  if(loop_init == NO_INIT) return;
  cycles.Init();
  Init_impl();
}

bool CyclesToSettle::Crit() {
  if(!has_stop_crit)    return false;
  if(n_copy_vals > 0)   return copy_vals.Crit();
  return cycles.Crit();
}

void CyclesToSettle::Network_Stat() {
  if(settle == NULL) return;
  cycles.val = settle->cycle.val;
}

//////////////////////////////////
// 	ActThreshRTStat		//
//////////////////////////////////

void ActThreshRTStat::Initialize() {
  loop_init = INIT_START_ONLY;
  settle = NULL;
  net_agg.op = Aggregate::LAST;
  act_thresh = .75f;
  // max_act.stopcrit.flag = true;  // don't actually stop..
  max_act.stopcrit.val = act_thresh;
  max_act.stopcrit.rel = CritParam::GREATERTHANOREQUAL;
  crossed_thresh = false;
}

void ActThreshRTStat::InitLinks() {
  Stat::InitLinks();
  GetSettleProc();
}

void ActThreshRTStat::NameStatVals() {
  Stat::NameStatVals();
  rt_cycles.AddDispOption("MIN=0");
  rt_cycles.AddDispOption("TEXT");
}

void ActThreshRTStat::UpdateAfterEdit() {
  Stat::UpdateAfterEdit();
  GetSettleProc();
  max_act.stopcrit.val = act_thresh;
}

void ActThreshRTStat::GetSettleProc() {
  settle = (SettleProcess*)GetMySProcOfType(&TA_SettleProcess);
}

void ActThreshRTStat::CutLinks() {
  Stat::CutLinks();
  settle = NULL;
}

void ActThreshRTStat::Copy_(const ActThreshRTStat& cp) {
  act_thresh = cp.act_thresh;
  max_act = cp.max_act;
  rt_cycles = cp.rt_cycles;
}

void ActThreshRTStat::InitStat() {
  max_act.InitStat(InitStatVal());
  rt_cycles.InitStat(InitStatVal());
  InitStat_impl();
}

void ActThreshRTStat::Init() {
  if(loop_init == NO_INIT) return;
  max_act.Init();
  rt_cycles.Init();
  crossed_thresh = false;
  Init_impl();
}

bool ActThreshRTStat::Crit() {
  if(!has_stop_crit)    return false;
  if(n_copy_vals > 0)   return copy_vals.Crit();
  return max_act.Crit() || rt_cycles.Crit();
}

void ActThreshRTStat::Network_Stat() {
  if(settle == NULL) return;
  if(layer == NULL) {
    taMisc::Error("*** ActThreshRTStat:", name, "layer must be set to point to layer to record activations from for stopping criterion.");
    return;
  }

  float mx_act = -FLT_MAX;
  taLeafItr ri;
  Unit* un;
  FOR_ITR_EL(Unit, un, layer->units., ri) {
    mx_act = MAX(un->act, mx_act);
  }
  net_agg.ComputeAgg(&max_act, mx_act);

  if(!crossed_thresh) {
    rt_cycles.val = settle->cycle.val;
    if(mx_act >= act_thresh)
      crossed_thresh = true;
  }
}

//////////////////////////
// 	ScriptStat	//
//////////////////////////

void ScriptStat::InitLinks() {
  Stat::InitLinks();
  taBase::Own(s_args, this);
}

void ScriptStat::Copy_(const ScriptStat& cp) {
  vals = cp.vals;
  s_args = cp.s_args;
}

void ScriptStat::UpdateAfterEdit() {
  Stat::UpdateAfterEdit();
  UpdateReCompile();
  String altnm = AltTypeName();
  if((own_proc != NULL) && !script_file->fname.empty()) {
    name = script_file->fname.before(".css");
    if(name.contains('/'))
      name = name.after('/', -1);
    name = own_proc->name + "_" + name;
    int mx = MIN(s_args.size, 5);
    int i;
    for(i=0;i<mx;i++)
      name += String("_") + s_args[i];
  }
  if(time_agg.from != NULL) {
    ScriptStat* rs = (ScriptStat*)time_agg.real_stat;
    if((own_proc != NULL) && (rs != NULL) && !rs->script_file->fname.empty()) {
      String nm = rs->script_file->fname.before(".css");
      if(nm.contains('/'))
	nm = nm.after('/', -1);
      nm = own_proc->name + "_" + nm;
      name = time_agg.PrependAggName(nm);
    }
  }
}


void ScriptStat::InitStat() {
  vals.InitStat(InitStatVal());
  InitStat_impl();
}

void ScriptStat::Init() {
  if(loop_init == NO_INIT) return;
  vals.Init();
  Init_impl();
}

bool ScriptStat::Crit() {
  if(!has_stop_crit)    return false;
  if(n_copy_vals > 0)   return copy_vals.Crit();
  return vals.Crit();
}

void ScriptStat::NameStatVals() {
  if(time_agg.from == NULL) return; // if non-agg, let script name vars
  Stat::NameStatVals();
}

void ScriptStat::Interact() {
  if(script == NULL)   return;
  cssMisc::next_shell = script;
}

void ScriptStat::Compile() {
  LoadScript();
}

//////////////////////////
// 	CompareStat	//
//////////////////////////

void CompareStat::Initialize() {
  stat_1 = NULL;
  stat_2 = NULL;
  cmp_type = float_RArray::CORREL;
  dist_tol = 0.0f;
  norm = false;
  net_agg.op = Aggregate::LAST;
}

void CompareStat::InitLinks() {
  Stat::InitLinks();
  taBase::Own(svals_1, this);
  taBase::Own(svals_2, this);
  taBase::Own(pre_proc_1, this);
  taBase::Own(pre_proc_2, this);
  taBase::Own(pre_proc_3, this);
}

void CompareStat::CutLinks() {
  Stat::CutLinks();
  taBase::DelPointer((TAPtr*)&stat_1);
  taBase::DelPointer((TAPtr*)&stat_2);
}

void CompareStat::Copy_(const CompareStat& cp) {
  taBase::SetPointer((TAPtr*)&stat_1, cp.stat_1);
  taBase::SetPointer((TAPtr*)&stat_2, cp.stat_2);
  cmp_type = cp.cmp_type;
  cmp = cp.cmp;
  dist_tol = cp.dist_tol;
  norm = cp.norm;
  pre_proc_1 = cp.pre_proc_1;
  pre_proc_2 = cp.pre_proc_2;
  pre_proc_3 = cp.pre_proc_3;
}

void CompareStat::InitStat() {
  cmp.InitStat(InitStatVal());
  InitStat_impl();
}

void CompareStat::Init() {
  if(loop_init == NO_INIT) return;
  cmp.Init();
  Init_impl();
}

bool CompareStat::Crit() {
  if(!has_stop_crit)    return false;
  if(n_copy_vals > 0)   return copy_vals.Crit();
  return cmp.Crit();
}

void CompareStat::GetStatVals(Stat* st, float_RArray& sv) {
  if(st == NULL)
    return;

  sv.Reset();

  StatVal_List* sg = NULL;
  if(st->copy_vals.size > 0)
    sg = &(st->copy_vals);	// use the copy vals if relevant
  else {
    TypeDef* td = st->GetTypeDef();
    int i;
    for(i=TA_Stat.members.size; i<td->members.size; i++) {
      MemberDef* md = td->members.FastEl(i);
      if(md->type->InheritsFrom(TA_StatVal))
	sv.Add(((StatVal*)md->GetOff(st))->val);
      else if(md->type->InheritsFrom(TA_StatVal_List)) {
	sg = (StatVal_List*)md->GetOff(st);
	break;
      }
    }
  }
  if(sg != NULL) {
    int i;
    for(i=0; i<sg->size; i++)
      sv.Add(((StatVal*)sg->FastEl(i))->val);
  }
  if(pre_proc_1.opr != SimpleMathSpec::NONE)    sv.SimpleMath(pre_proc_1);
  if(pre_proc_2.opr != SimpleMathSpec::NONE)    sv.SimpleMath(pre_proc_2);
  if(pre_proc_3.opr != SimpleMathSpec::NONE)    sv.SimpleMath(pre_proc_3);
}


void CompareStat::Network_Stat() {
  if((stat_1 == NULL) || (stat_2 == NULL)) {
    taMisc::Error("*** CompareStat", name,"must have a stat_1 and stat_2 to compare");
    return;
  }

  GetStatVals(stat_1, svals_1);
  GetStatVals(stat_2, svals_2);

  float rval = svals_1.Dist(svals_2, cmp_type, norm, dist_tol);

  net_agg.ComputeAgg(&cmp, rval);
}

//////////////////////////
// 	ProjectionStat	//
//////////////////////////

void ProjectionStat::Initialize() {
  stat = NULL;
  dist_metric = float_RArray::INNER_PROD;
  dist_tol = 0.0f;
  dist_norm = false;
  net_agg.op = Aggregate::LAST;
}

void ProjectionStat::InitLinks() {
  Stat::InitLinks();
  taBase::Own(prjn_vector, this);
  taBase::Own(svals, this);
}

void ProjectionStat::CutLinks() {
  Stat::CutLinks();
  taBase::DelPointer((TAPtr*)&stat);
}

void ProjectionStat::Copy_(const ProjectionStat& cp) {
  taBase::SetPointer((TAPtr*)&stat, cp.stat);
  dist_metric = cp.dist_metric;
  prjn = cp.prjn;
  dist_tol = cp.dist_tol;
  dist_norm = cp.dist_norm;
}

void ProjectionStat::InitStat() {
  prjn.InitStat(InitStatVal());
  InitStat_impl();
}

void ProjectionStat::Init() {
  if(loop_init == NO_INIT) return;
  prjn.Init();
  Init_impl();
}

bool ProjectionStat::Crit() {
  if(!has_stop_crit)    return false;
  if(n_copy_vals > 0)   return copy_vals.Crit();
  return prjn.Crit();
}

void ProjectionStat::GetStatVals(Stat* st, float_RArray& sv) {
  if(st == NULL)
    return;

  sv.Reset();

  StatVal_List* sg = NULL;
  if(st->copy_vals.size > 0)
    sg = &(st->copy_vals);	// use the copy vals if relevant
  else {
    TypeDef* td = st->GetTypeDef();
    int i;
    for(i=TA_Stat.members.size; i<td->members.size; i++) {
      MemberDef* md = td->members.FastEl(i);
      if(md->type->InheritsFrom(TA_StatVal))
	sv.Add(((StatVal*)md->GetOff(st))->val);
      else if(md->type->InheritsFrom(TA_StatVal_List)) {
	sg = (StatVal_List*)md->GetOff(st);
	break;
      }
    }
  }
  if(sg != NULL) {
    int i;
    for(i=0; i<sg->size; i++)
      sv.Add(((StatVal*)sg->FastEl(i))->val);
  }
}

void ProjectionStat::Network_Stat() {
  if((stat == NULL)) {
    taMisc::Error("*** ProjectionStat", name,"must have a stat to get values to project against");
    return;
  }
  GetStatVals(stat, svals);
  float rval = prjn_vector.Dist(svals, dist_metric, dist_norm, dist_tol);
  net_agg.ComputeAgg(&prjn, rval);
}

void ProjectionStat::VecFmPCA(Environment* env, int pat_no, int pca_c) {
  if(env == NULL) return;
  int dim;			// dimensionality
  float_RArray evecs;		// eigen vectors
  float_RArray evals;		// eigen values
  env->PCAEigens(evecs, evals, pat_no, dim);

  if((pca_c < 0) || (pca_c >= dim)) {
    taMisc::Error("*** PrjnVectorFmPCA: component must be between 0 and",String(dim-1));
    return;
  }

  int c_rev = dim - 1 - pca_c; // reverse-order indicies, for accessing data structs
  evecs.GetMatCol(dim, prjn_vector, c_rev); // get eigen vector = column of correl_matrix
}

void ProjectionStat::VecFmEvent(Event* event, int pat_no) {
  if(event == NULL) return;
  Pattern* pat = (Pattern*)event->patterns.Leaf(pat_no);
  if(pat == NULL) {
    taMisc::Error("*** Pattern number:", String(pat_no), "not found");
    return;
  }
  prjn_vector.CopyFrom(&(pat->value));
}


//////////////////////////
// 	ComputeStat	//
//////////////////////////

void ComputeStat::Initialize() {
  stat_1 = NULL;
  stat_2 = NULL;
  net_agg.op = Aggregate::AVG;
}

void ComputeStat::InitLinks() {
  Stat::InitLinks();
  taBase::Own(svals_1, this);
  taBase::Own(svals_2, this);
  taBase::Own(pre_proc_1, this);
  taBase::Own(pre_proc_2, this);
  taBase::Own(pre_proc_3, this);
  taBase::Own(compute_1, this);
  taBase::Own(compute_2, this);
  taBase::Own(compute_3, this);
}

void ComputeStat::CutLinks() {
  Stat::CutLinks();
  taBase::DelPointer((TAPtr*)&stat_1);
  taBase::DelPointer((TAPtr*)&stat_2);
}

void ComputeStat::Copy_(const ComputeStat& cp) {
  taBase::SetPointer((TAPtr*)&stat_1, cp.stat_1);
  taBase::SetPointer((TAPtr*)&stat_2, cp.stat_2);
  cpt = cp.cpt;
  pre_proc_1 = cp.pre_proc_1;
  pre_proc_2 = cp.pre_proc_2;
  pre_proc_3 = cp.pre_proc_3;
  compute_1 = cp.compute_1;
  compute_2 = cp.compute_2;
  compute_3 = cp.compute_3;
}

void ComputeStat::InitStat() {
  cpt.InitStat(InitStatVal());
  InitStat_impl();
}

void ComputeStat::Init() {
  if(loop_init == NO_INIT) return;
  cpt.Init();
  Init_impl();
}

bool ComputeStat::Crit() {
  if(!has_stop_crit)    return false;
  if(n_copy_vals > 0)   return copy_vals.Crit();
  return cpt.Crit();
}

void ComputeStat::GetStatVals(Stat* st, float_RArray& sv) {
  if(st == NULL)
    return;

  sv.Reset();

  StatVal_List* sg = NULL;
  if(st->copy_vals.size > 0)
    sg = &(st->copy_vals);	// use the copy vals if relevant
  else {
    TypeDef* td = st->GetTypeDef();
    int i;
    for(i=TA_Stat.members.size; i<td->members.size; i++) {
      MemberDef* md = td->members.FastEl(i);
      if(md->type->InheritsFrom(TA_StatVal))
	sv.Add(((StatVal*)md->GetOff(st))->val);
      else if(md->type->InheritsFrom(TA_StatVal_List)) {
	sg = (StatVal_List*)md->GetOff(st);
	break;
      }
    }
  }
  if(sg != NULL) {
    int i;
    for(i=0; i<sg->size; i++)
      sv.Add(((StatVal*)sg->FastEl(i))->val);
  }
  if(pre_proc_1.opr != SimpleMathSpec::NONE)    sv.SimpleMath(pre_proc_1);
  if(pre_proc_2.opr != SimpleMathSpec::NONE)    sv.SimpleMath(pre_proc_2);
  if(pre_proc_3.opr != SimpleMathSpec::NONE)    sv.SimpleMath(pre_proc_3);
}


void ComputeStat::Network_Stat() {
  if(stat_1 == NULL) {
    taMisc::Error("*** ComputeStat", name,"must have a stat_1 to compute on");
    return;
  }

  GetStatVals(stat_1, svals_1);
  if(stat_2 != NULL)
    GetStatVals(stat_2, svals_2);
  else
    svals_2.Reset();
  if(compute_1.opr != SimpleMathSpec::NONE) 	svals_1.SimpleMathArg(svals_2, compute_1);
  if(compute_2.opr != SimpleMathSpec::NONE) 	svals_1.SimpleMathArg(svals_2, compute_2);
  if(compute_3.opr != SimpleMathSpec::NONE) 	svals_1.SimpleMathArg(svals_2, compute_3);

  int i;
  for(i=0; i<svals_1.size; i++)
    net_agg.ComputeAgg(&cpt, svals_1.FastEl(i));
}

//////////////////////////
//   CopyToEnvStat	//
//////////////////////////

void CopyToEnvStat::Initialize() {
  stat = NULL;
  data_env = NULL;
  accum_scope = SUPER;
  net_agg.op = Aggregate::LAST;
}

void CopyToEnvStat::InitLinks() {
  Stat::InitLinks();
  taBase::Own(svals, this);
  taBase::Own(pre_proc_1, this);
  taBase::Own(pre_proc_2, this);
  taBase::Own(pre_proc_3, this);
  taBase::Own(last_ctr_vals, this);
}

void CopyToEnvStat::CutLinks() {
  Stat::CutLinks();
  taBase::DelPointer((TAPtr*)&stat);
  taBase::DelPointer((TAPtr*)&data_env);
}

void CopyToEnvStat::Copy_(const CopyToEnvStat& cp) {
  taBase::SetPointer((TAPtr*)&stat, cp.stat);
  taBase::SetPointer((TAPtr*)&data_env, cp.data_env);
  pre_proc_1 = cp.pre_proc_1;
  pre_proc_2 = cp.pre_proc_2;
  pre_proc_3 = cp.pre_proc_3;
}

void CopyToEnvStat::InitStat() {
  InitStat_impl();
}

void CopyToEnvStat::Init() {
  if(loop_init == NO_INIT) return;
  Init_impl();
}

bool CopyToEnvStat::Crit() {
  return false;
}

SchedProcess* CopyToEnvStat::GetAccumProc() {
  if(own_proc == NULL) {
    taMisc::Error("*** CopyToEnvStat", name,"own_proc is NULL -- can't find accum scope proc!");
    return NULL;
  }
  if(accum_scope == OWNER)
    return own_proc;
  if(own_proc->super_proc == NULL) {
    taMisc::Error("*** CopyToEnvStat", name,"super_proc is NULL -- can't find accum scope proc!");
    return NULL;
  }
  if(accum_scope == SUPER)
    return own_proc->super_proc;
  SchedProcess* accum_proc = NULL;
  TypeDef* accum_td = NULL;  TypeDef* accum_td2 = NULL;
  if(accum_scope == TRAIN) {
    accum_td = &TA_TrainProcess; accum_td2 = &TA_NEpochProcess;
  }
  else if(accum_scope == EPOCH)
    accum_td = &TA_EpochProcess;
  else if(accum_scope == SEQUENCE)
    accum_td = &TA_SequenceProcess;
  else if(accum_scope == SETTLE)
    accum_td = &TA_SettleProcess;

  if(accum_td == NULL) {
    taMisc::Error("*** CopyToEnvStat", name,"incorrect accum_scope:",String(accum_scope));
    return NULL;
  }
  if(own_proc->InheritsFrom(accum_td) || ((accum_td2 != NULL) && own_proc->InheritsFrom(accum_td2)))
    return own_proc;

  accum_proc = own_proc->FindSuperProc(accum_td);
  if((accum_proc == NULL) && (accum_td2 != NULL))
    accum_proc = own_proc->FindSuperProc(accum_td2);
  if(accum_proc == NULL) {
    taMisc::Error("*** CopyToEnvStat", name,"could not find accum scope process of type:",accum_td->name);
  }
  return accum_proc;
}

bool CopyToEnvStat::InitEvtIdx() {
  SchedProcess* accum_proc = GetAccumProc();
  if(accum_proc == NULL) return true; // always init if nothing else!

  int n_accum_procs = 1;	// number of accumulation procs above me, starting at own_proc
  if(accum_proc != own_proc) {
    SchedProcess* sp = own_proc;
    do {
      sp = sp->super_proc; if(sp == NULL) break;
      n_accum_procs++;
    } while (sp != accum_proc);
  }

  if(last_ctr_vals.size != n_accum_procs) {
    last_ctr_vals.EnforceSize(n_accum_procs);
    last_ctr_vals.InitVals(1000000); // big number, so it resets now!
  }

  if(accum_proc == own_proc) {
    int lc = last_ctr_vals[0];
    last_ctr_vals[0] = own_proc->GetCounter();
    if(own_proc->GetCounter() <= lc)
      return true;
    return false;
  }
  SchedProcess* sp = own_proc;
  int ctidx = 0;
  int lc = last_ctr_vals[ctidx];
  last_ctr_vals[ctidx] = sp->GetCounter();
  bool all_under = (sp->GetCounter() <= lc);
  do {
    sp = sp->super_proc;    if(sp == NULL) break;
    ctidx++;
    lc = last_ctr_vals[ctidx];
    last_ctr_vals[ctidx] = sp->GetCounter();
    all_under = all_under && (sp->GetCounter() <= lc);
  } while(sp != accum_proc);
  return all_under;		// if all under previous vals up to target proc, then init, otherwise not..
}

void CopyToEnvStat::InitEnv() {
  if(stat == NULL) {
    taMisc::Error("*** CopyToEnvStat", name,"must have a stat to collect data from");
    return;
  }
  if(data_env == NULL) {
    taMisc::Error("*** CopyToEnvStat", name,"must have an data_env to store data in");
    return;
  }

  GetStatVals(stat, svals);

  evt_idx = 0;
  data_env->events.Reset();	// get rid of all events

  EventSpec* es = data_env->GetAnEventSpec();
  es->UpdateAfterEdit();	// make sure its all done with internals..
  es->patterns.EnforceSize(1);

  PatternSpec* ps = (PatternSpec*)es->patterns[0];
  ps->n_vals = svals.size;
  ps->UpdateAfterEdit();	// this will sort out cases where nvals > geom
}

void CopyToEnvStat::GetStatVals(Stat* st, float_RArray& sv) {
  if(st == NULL)
    return;

  sv.Reset();

  StatVal_List* sg = NULL;
  if(st->copy_vals.size > 0)
    sg = &(st->copy_vals);	// use the copy vals if relevant
  else {
    TypeDef* td = st->GetTypeDef();
    int i;
    for(i=TA_Stat.members.size; i<td->members.size; i++) {
      MemberDef* md = td->members.FastEl(i);
      if(md->type->InheritsFrom(TA_StatVal))
	sv.Add(((StatVal*)md->GetOff(st))->val);
      else if(md->type->InheritsFrom(TA_StatVal_List)) {
	sg = (StatVal_List*)md->GetOff(st);
	break;
      }
    }
  }
  if(sg != NULL) {
    int i;
    for(i=0; i<sg->size; i++)
      sv.Add(((StatVal*)sg->FastEl(i))->val);
  }
  if(pre_proc_1.opr != SimpleMathSpec::NONE)    sv.SimpleMath(pre_proc_1);
  if(pre_proc_2.opr != SimpleMathSpec::NONE)    sv.SimpleMath(pre_proc_2);
  if(pre_proc_3.opr != SimpleMathSpec::NONE)    sv.SimpleMath(pre_proc_3);
}

void CopyToEnvStat::Network_Stat() {
  if(stat == NULL) {
    taMisc::Error("*** CopyToEnvStat", name,"must have a stat to collect data from");
    return;
  }
  if(data_env == NULL) {
    taMisc::Error("*** CopyToEnvStat", name,"must have an data_env to store data in");
    return;
  }

  bool init_evt_idx = InitEvtIdx();
  if(init_evt_idx || (data_env->event_specs.size == 0)) {
    InitEnv();
  }
  else {
    GetStatVals(stat, svals);
    evt_idx++;			// increment event index!
  }

  data_env->events.EnforceSize(evt_idx+1);

  Event* ev = (Event*)data_env->events.SafeEl(evt_idx);
  if(ev == NULL) return;
  ((Pattern*)ev->patterns[0])->value = svals; // just copy it over..

  SchedProcess* accum_proc = GetAccumProc();
  Event* cur_ev = GetMyCurEvent();
  if((accum_proc == NULL) && (cur_ev != NULL)) {
    ev->name = cur_ev->name;
  }
  else {
    String nm;
    if(accum_proc == own_proc) {
      if((cur_ev != NULL) && (own_proc->GetTypeDef() == &TA_EpochProcess) || own_proc->InheritsFrom(TA_SequenceProcess))
	nm = cur_ev->name;
      else if(own_proc->cntr != NULL)
	nm = own_proc->cntr->name + "_" + String(own_proc->cntr->val);
    }
    else {
      SchedProcess* sp = own_proc;
      do {
	sp = sp->super_proc;
	if((cur_ev != NULL) && (sp->GetTypeDef() == &TA_EpochProcess) || sp->InheritsFrom(TA_SequenceProcess)) {
	  if(!nm.empty()) nm += ":";
	  nm += cur_ev->name;
	}
	else if(sp->cntr != NULL) {
	  if(!nm.empty()) nm += ":";
	  nm += sp->cntr->name + String("_") + String(sp->cntr->val);
	}
      }
      while((sp != NULL) && (sp != accum_proc));
    }
    ev->name = nm;
  }
}


//////////////////////////////////
//     EpochCounterStat		//
//////////////////////////////////

void EpochCounterStat::Initialize() {
  net_agg.op = Aggregate::LAST;
}

void EpochCounterStat::Copy_(const EpochCounterStat& cp) {
  epoch = cp.epoch;
}

void EpochCounterStat::InitStat() {
  epoch.InitStat(InitStatVal());
  InitStat_impl();
}

void EpochCounterStat::Init() {
  if(loop_init == NO_INIT) return;
  epoch.Init();
  Init_impl();
}

bool EpochCounterStat::Crit() {
  if(!has_stop_crit)    return false;
  if(n_copy_vals > 0)   return copy_vals.Crit();
  return epoch.Crit();
}

void EpochCounterStat::NameStatVals() {
  if(time_agg.from != NULL) {
    Stat::NameStatVals();
    return;
  }
  epoch.SetNarrowName("epoch");
}

void EpochCounterStat::Network_Stat() {
  net_agg.ComputeAgg(&epoch, network->epoch);
}


//////////////////////////////////
// 	ProcCounterStat		//
//////////////////////////////////

void ProcCounterStat::Initialize() {
  proc = NULL;
  net_agg.op = Aggregate::LAST;
}

void ProcCounterStat::InitLinks() {
  Stat::InitLinks();
  taBase::Own(counters, this);
}

void ProcCounterStat::CutLinks() {
  Stat::CutLinks();
  taBase::DelPointer((TAPtr*)&proc);
}

void ProcCounterStat::Copy_(const ProcCounterStat& cp) {
  counters = cp.counters;
  taBase::SetPointer((TAPtr*)&proc, cp.proc);
}

void ProcCounterStat::InitStat() {
  counters.InitStat(InitStatVal());
  InitStat_impl();
}

void ProcCounterStat::Init() {
  if(loop_init == NO_INIT) return;
  counters.Init();
  Init_impl();
  if(NeedsUpdated())
    UpdateAfterEdit();
}

bool ProcCounterStat::Crit() {
  if(!has_stop_crit)    return false;
  if(n_copy_vals > 0)   return copy_vals.Crit();
  return counters.Crit();
}

bool ProcCounterStat::NeedsUpdated() {
  if(time_agg.real_stat == NULL)
    return false;
  ProcCounterStat* rstat = (ProcCounterStat*)time_agg.real_stat;
  if(rstat->proc == NULL) 	return false;
  log_data.Reset();
  rstat->proc->GenCntrLog(&log_data, true);
  if(counters.size != log_data.items.size)
    return true;
  return false;
}

void ProcCounterStat::NameStatVals() {
  if(time_agg.from != NULL) {
    Stat::NameStatVals();
    return;
  }
  log_data.Reset();
  if(proc == NULL) return;
  proc->GenCntrLog(&log_data, true);
  int sz = MIN(counters.size, log_data.items.size);
  int i;
  for(i=0; i<sz; i++) {
    StatVal* sv = (StatVal*)counters.FastEl(i);
    DataItem* di = (DataItem*)sv;
    (*di).Copy(*(log_data.items.FastEl(i)));
  }
}

void ProcCounterStat::UpdateAfterEdit() {
  Stat::UpdateAfterEdit();
  if(time_agg.from != NULL)
    return;
  log_data.Reset();
  if(proc == NULL) 	return;
  proc->GenCntrLog(&log_data, true);
  if(counters.size != log_data.items.size) {
    counters.Reset();
    counters.New(log_data.items.size);
    int i;
    for(i=0; i<counters.size; i++) {
      StatVal* sv = (StatVal*)counters.FastEl(i);
      DataItem* di = (DataItem*)sv;
      (*di).Copy(*(log_data.items.FastEl(i)));
    }
  }
}

void ProcCounterStat::Network_Stat() {
  if(proc == NULL)	return;

  log_data.Reset();
  proc->GenCntrLog(&log_data, true);

  if(counters.size != log_data.items.size)
    UpdateAfterEdit();
  int i;
  for(i=0; i<log_data.items.size; i++) {
    if(log_data.IsString(i))
      ((StatVal*)counters.FastEl(i))->str_val = log_data.GetString(i);
    else
      ((StatVal*)counters.FastEl(i))->val = log_data.GetFloat(i);
  }
}


//////////////////////////
// 	MaxActTrgStat	//
//////////////////////////

void MaxActTrgStat::Initialize() {
}

void MaxActTrgStat::NameStatVals() {
  Stat::NameStatVals();
  mxat.AddDispOption("MIN=0");
  mxat.AddDispOption("TEXT");
}

void MaxActTrgStat::InitStat() {
  float init_val = InitStatVal();
  mxat.InitStat(init_val);
  InitStat_impl();
}

void MaxActTrgStat::Init() {
  if(loop_init == NO_INIT) return;
  mxat.Init();
  Init_impl();
}

bool MaxActTrgStat::Crit() {
  if(!has_stop_crit)    return false;
  if(n_copy_vals > 0)   return copy_vals.Crit();
  return mxat.Crit();
}

void MaxActTrgStat::Network_Init() {
  InitStat();
}

void MaxActTrgStat::Layer_Run() {
  if(layer != NULL) {
    if(!CheckLayerInNet()) return;
    if(layer->lesion)      return;
    Layer_Init(layer);
    Unit_Run(layer);
    Layer_Stat(layer);
    return;
  }
  Layer *lay;
  taLeafItr i;
  FOR_ITR_EL(Layer, lay, network->layers., i) {
    // only target layers need to be checked for error
    if(lay->lesion || !((lay->ext_flag & Unit::TARG) || (lay->ext_flag & Unit::COMP)))
      continue;
    Layer_Init(lay);
    Unit_Run(lay);
    Layer_Stat(lay);
  }
}

void MaxActTrgStat::Layer_Stat(Layer* lay) {
  float mx_act = -FLT_MAX;
  Unit* mx_un = NULL;
  Unit* unit;
  taLeafItr i;
  FOR_ITR_EL(Unit, unit, lay->units., i) {
    if(unit->act > mx_act) {
      mx_act = unit->act;
      mx_un = unit;
    }
  }
  float tmp = 0.0f;
  if((mx_un != NULL) && (mx_un->targ == 1.0f))
    tmp = 1.0f;
  net_agg.ComputeAgg(&mxat, tmp);
}


//////////////////////////
// 	UnitActRFStat	//
//////////////////////////

void UnitActRFStat::Initialize() {
  data_env = NULL;
  norm_mode = NORM_UNIT;
  net_agg.op = Aggregate::AVG;
}

void UnitActRFStat::InitLinks() {
  Stat::InitLinks();
  taBase::Own(rf_layers, this);
  taBase::Own(avg_norms, this);
}

void UnitActRFStat::CutLinks() {
  Stat::CutLinks();
  taBase::DelPointer((TAPtr*)&data_env);
  rf_layers.Reset();
}

void UnitActRFStat::Copy_(const UnitActRFStat& cp) {
  taBase::SetPointer((TAPtr*)&data_env, cp.data_env);
  rf_layers.BorrowUnique(cp.rf_layers); // links, just borrow them..
}

void UnitActRFStat::UpdateAfterEdit() {
  Stat::UpdateAfterEdit();
}

bool UnitActRFStat::CheckRFLayersInNet() {
  int i;
  for(i=0; i<rf_layers.size; i++) {
    Layer* lay = (Layer*)rf_layers[i];
    if(lay->own_net == network)
      continue;

    taMisc::Error("*** Stat:", name, "rf_layer:", lay->name,
		  "is not in current network, finding one of same name...");
    Layer* nw_lay = (Layer*)network->layers.FindName(lay->name);
    if(nw_lay == NULL) {
      taMisc::Error("==> could not find layer with same name in current network, aborting");
      return false;
    }
    rf_layers.ReplaceLink(i, nw_lay);
  }
  return true;
}

void UnitActRFStat::InitRFVals() {
  if(layer == NULL) {
    taMisc::Error("*** UnitActRFStat", name,"must have a layer to collect rf over");
    return;
  }
  if(data_env == NULL) {
    taMisc::Error("*** UnitActRFStat", name,"must have an data_env to store data in");
    return;
  }
  if(rf_layers.size == 0) {
    taMisc::Error("*** UnitActRFStat", name,"must have rf_layers to get acts from");
    return;
  }

  if(!CheckLayerInNet()) return;
  if(!CheckRFLayersInNet()) return;

  EventSpec* es = data_env->GetAnEventSpec();
  es->UpdateAfterEdit();	// make sure its all done with internals..
  es->patterns.EnforceSize(rf_layers.size);

  int i;
  for(i=0; i<rf_layers.size; i++) {
    PatternSpec* ps = (PatternSpec*)es->patterns[i];
    Layer* lay = (Layer*)rf_layers[i];
    ps->geom.x = lay->geom.x;
    ps->geom.y = lay->geom.y;
    ps->n_vals = lay->units.leaves;
    ps->name = lay->name;
    ps->layer_name = lay->name;
    ps->to_layer = PatternSpec::LAY_NAME;
    ps->UpdateAfterEdit();	// this will sort out cases where nvals > geom
  }

  es->UpdateAllEvents();	// get them all straightened out
  data_env->events.EnforceSize(layer->units.leaves); // make sure we have enough events

  SimpleMathSpec zeroit;
  zeroit.opr = SimpleMathSpec::THRESH; // set everything to 0
  zeroit.arg = 0;
  zeroit.lw = 0;
  zeroit.hi = 0;

  for(i=0; i<rf_layers.size; i++)
    data_env->TransformPats(i, zeroit);

  for(i=0; i<data_env->events.size; i++) {
    Event* ev = (Event*)data_env->events.FastEl(i);
    ev->name = layer->name + ".un[" + String(i) + "]";
  }

  avg_norms.EnforceSize(layer->units.leaves);
  avg_norms.SimpleMath(zeroit);
}

void UnitActRFStat::InitStat() {
  InitStat_impl();
}

void UnitActRFStat::Init() {
  if(loop_init == NO_INIT) return;
  Init_impl();
}

bool UnitActRFStat::Crit() {
  if(!has_stop_crit)    return false;
  if(n_copy_vals > 0)   return copy_vals.Crit();
  return false;
}

void UnitActRFStat::Network_Stat() {
  if((layer == NULL) || (data_env == NULL) || (rf_layers.size == 0)) {
    taMisc::Error("*** UnitActRFStat", name,"layer, rf_layers, and/or data_env not set");
    return;
  }
  if(!CheckLayerInNet()) return;
  if(!CheckRFLayersInNet()) return;

  if((data_env->events.size != layer->units.leaves) ||
     (avg_norms.size != layer->units.leaves) ||
     (((EventSpec*)data_env->event_specs[0])->patterns.size != rf_layers.size)) {
    InitRFVals();
  }


  if(norm_mode != NORM_GROUP) {
    float max_lay_norm = 0.0f;
    int mxidx;
    float prv_max_lay_norm = avg_norms.MaxVal(mxidx);

    int rcnt = 0;
    taLeafItr ri;
    Unit* runp;
    FOR_ITR_EL(Unit, runp, layer->units., ri) {
      float cur_act = fabs(runp->act); // weight by magnitude of the unit response..
      float& avg_norm = avg_norms[rcnt];
      float nw_norm = avg_norm + cur_act;
      if(nw_norm > max_lay_norm)
	max_lay_norm = nw_norm;
      if((norm_mode == NORM_UNIT) && (nw_norm > 0.0f)) {
	Event* ev = (Event*)data_env->events[rcnt];
	int i;
	for(i=0; i<rf_layers.size; i++) {
	  float_RArray& ary = ((Pattern*)ev->patterns[i])->value;
	  Layer* lay = (Layer*)rf_layers.FastEl(i);
	  int scnt = 0;
	  taLeafItr si;
	  Unit* sunp;
	  FOR_ITR_EL(Unit, sunp, lay->units., si) {
	    float& sval = ary[scnt];
	    sval = ((sval * avg_norm) + (cur_act * sunp->act)) / nw_norm;
	    scnt++;
	  }
	}
      }
      avg_norm = nw_norm;
      rcnt++;
    }

    if((norm_mode == NORM_LAYER) && (max_lay_norm > 0.0f)) {
      rcnt = 0;
      FOR_ITR_EL(Unit, runp, layer->units., ri) {
	float cur_act = fabs(runp->act); // weight by magnitude of the unit response..
	Event* ev = (Event*)data_env->events[rcnt];
	int i;
	for(i=0; i<rf_layers.size; i++) {
	  float_RArray& ary = ((Pattern*)ev->patterns[i])->value;
	  Layer* lay = (Layer*)rf_layers.FastEl(i);
	  int scnt = 0;
	  taLeafItr si;
	  Unit* sunp;
	  FOR_ITR_EL(Unit, sunp, lay->units., si) {
	    float& sval = ary[scnt];
	    sval = ((sval * prv_max_lay_norm) + (cur_act * sunp->act)) / max_lay_norm;
	    scnt++;
	  }
	}
	rcnt++;
      }
    }
  }
  else {			// NORM_GROUPS
    int gi;
    int rcnt = 0;
    Unit_Group* rgp;
    FOR_ITR_GP(Unit_Group, rgp, layer->units., gi) {
      int gst = rcnt;
      int j;
      float prv_max_gp_norm = 0.0f;
      float max_gp_norm = 0.0f;
      for(j=0;j<rgp->size;j++) {
	Unit* runp = (Unit*)rgp->FastEl(j);
	float cur_act = fabs(runp->act); // weight by magnitude of the unit response..
	float& avg_norm = avg_norms[rcnt];
	float nw_norm = avg_norm + cur_act;
	if(nw_norm > max_gp_norm)
	  max_gp_norm = nw_norm;
	if(avg_norm > prv_max_gp_norm)
	  prv_max_gp_norm = avg_norm;
	avg_norm = nw_norm;
	rcnt++;
      }

      for(j=0;j<rgp->size;j++) {
	Unit* runp = (Unit*)rgp->FastEl(j);
	float cur_act = fabs(runp->act); // weight by magnitude of the unit response..
	Event* ev = (Event*)data_env->events[gst];
	int i;
	for(i=0; i<rf_layers.size; i++) {
	  float_RArray& ary = ((Pattern*)ev->patterns[i])->value;
	  Layer* lay = (Layer*)rf_layers.FastEl(i);
	  int scnt = 0;
	  taLeafItr si;
	  Unit* sunp;
	  FOR_ITR_EL(Unit, sunp, lay->units., si) {
	    float& sval = ary[scnt];
	    sval = ((sval * prv_max_gp_norm) + (cur_act * sunp->act)) / max_gp_norm;
	    scnt++;
	  }
	}
	gst++;
      }
    }
  }
}

////////////////

void UnitActRFStatResetProc::Initialize() {
  unit_act_rf_stat = NULL;
}

void UnitActRFStatResetProc::CutLinks() {
  taBase::DelPointer((TAPtr*)&unit_act_rf_stat);
  Process::CutLinks();
}

void UnitActRFStatResetProc::C_Code() {
  if(unit_act_rf_stat == NULL) {
    taMisc::Error("UnitActRFStatResetProc: RF's not reset because unit_act_rf_stat = NULL in: ", GetPath());
    return;
  }
  unit_act_rf_stat->InitRFVals();
}

//////////////////////////
// 	UnitEventRFStat	//
//////////////////////////

void UnitEventRFStat::Initialize() {
  epoch_proc = NULL;
  data_env = NULL;
  net_agg.op = Aggregate::AVG;
}

void UnitEventRFStat::InitLinks() {
  Stat::InitLinks();
  GetEpochProc();
}

void UnitEventRFStat::CutLinks() {
  Stat::CutLinks();
  taBase::DelPointer((TAPtr*)&data_env);
  epoch_proc = NULL;
}

void UnitEventRFStat::Copy_(const UnitEventRFStat& cp) {
  taBase::SetPointer((TAPtr*)&data_env, cp.data_env);
}

void UnitEventRFStat::UpdateAfterEdit() {
  Stat::UpdateAfterEdit();
  GetEpochProc();
}

void UnitEventRFStat::GetEpochProc() {
  SchedProcess* proc = GET_MY_OWNER(SchedProcess);
  if(proc == NULL) return;
  epoch_proc = (EpochProcess*)proc->FindProcOfType(&TA_EpochProcess);
}

void UnitEventRFStat::InitRFVals() {
  if(epoch_proc == NULL) {
    taMisc::Error("*** UnitEventRFStat", name,"could not find EpochProcess for event count");
    return;
  }
  if(layer == NULL) {
    taMisc::Error("*** UnitEventRFStat", name,"must have a layer to collect rf over");
    return;
  }
  if(data_env == NULL) {
    taMisc::Error("*** UnitEventRFStat", name,"must have an data_env to store data in");
    return;
  }

  if(!CheckLayerInNet()) return;

  EventSpec* es = data_env->GetAnEventSpec();
  es->UpdateAfterEdit();	// make sure its all done with internals..
  es->patterns.EnforceSize(1);

  PatternSpec* ps = (PatternSpec*)es->patterns[0];
  ps->n_vals = epoch_proc->trial.max;
  ps->UpdateAfterEdit();	// this will sort out cases where nvals > geom

  es->UpdateAllEvents();	// get them all straightened out
  data_env->events.EnforceSize(layer->units.leaves); // make sure we have enough events

  SimpleMathSpec zeroit;
  zeroit.opr = SimpleMathSpec::THRESH; // set everything to 0
  zeroit.arg = 0;
  zeroit.lw = 0;
  zeroit.hi = 0;

  data_env->TransformPats(0, zeroit);

  int i;
  for(i=0; i<data_env->events.size; i++) {
    Event* ev = (Event*)data_env->events.FastEl(i);
    ev->name = layer->name + ".un[" + String(i) + "]";
  }
}

void UnitEventRFStat::InitStat() {
  InitStat_impl();
}

void UnitEventRFStat::Init() {
  if(loop_init == NO_INIT) return;
  Init_impl();
}

bool UnitEventRFStat::Crit() {
  if(!has_stop_crit)    return false;
  if(n_copy_vals > 0)   return copy_vals.Crit();
  return false;
}

void UnitEventRFStat::Network_Stat() {
  if((epoch_proc == NULL) || (layer == NULL) || (data_env == NULL)) {
    taMisc::Error("*** UnitEventRFStat", name,"layer, and/or data_env not set");
    return;
  }
  if(!CheckLayerInNet()) return;
  if(data_env->events.size < layer->units.leaves)
    InitRFVals();

  int rcnt = 0;
  taLeafItr ri;
  Unit* runp;
  FOR_ITR_EL(Unit, runp, layer->units., ri) {
    Event* ev = (Event*)data_env->events[rcnt];
    ((Pattern*)ev->patterns[0])->value[epoch_proc->trial.val] = runp->act;
    rcnt++;
  }
}

//////////////////////////
//     UniquePatStat	//
//////////////////////////

void UniquePatStat::Initialize() {
  loop_init = INIT_START_ONLY;
  pat_stat = NULL;
  data_env = NULL;
  cmp_type = float_RArray::CORREL;
  dist_tol = 0.0f;
  norm = false;
  uniq_tol = .8f;
  net_agg.op = Aggregate::AVG;
}

void UniquePatStat::InitLinks() {
  Stat::InitLinks();
  taBase::Own(svals, this);
  taBase::Own(pre_proc_1, this);
  taBase::Own(pre_proc_2, this);
  taBase::Own(pre_proc_3, this);
}

void UniquePatStat::CutLinks() {
  Stat::CutLinks();
  taBase::DelPointer((TAPtr*)&pat_stat);
  taBase::DelPointer((TAPtr*)&data_env);
}

void UniquePatStat::Copy_(const UniquePatStat& cp) {
  taBase::SetPointer((TAPtr*)&pat_stat, cp.pat_stat);
  taBase::SetPointer((TAPtr*)&data_env, cp.data_env);
  cmp_type = cp.cmp_type;
  unq = cp.unq;
  dist_tol = cp.dist_tol;
  norm = cp.norm;
  uniq_tol = cp.uniq_tol;
  pre_proc_1 = cp.pre_proc_1;
  pre_proc_2 = cp.pre_proc_2;
  pre_proc_3 = cp.pre_proc_3;
}

void UniquePatStat::InitStat() {
  unq.InitStat(InitStatVal());
  InitStat_impl();
}

void UniquePatStat::Init() {
  if(loop_init == NO_INIT) return;
  unq.Init();
  Init_impl();
  if(data_env != NULL) {
    data_env->events.Reset();
    EventSpec* es = data_env->GetAnEventSpec();
    es->UpdateAfterEdit();	// make sure its all done with internals..
    es->patterns.EnforceSize(1);
    es->UpdateAfterEdit();
  }
}

bool UniquePatStat::Crit() {
  if(!has_stop_crit)    return false;
  if(n_copy_vals > 0)   return copy_vals.Crit();
  return unq.Crit();
}

void UniquePatStat::GetStatVals(Stat* st, float_RArray& sv) {
  if(st == NULL)
    return;

  sv.Reset();

  StatVal_List* sg = NULL;
  if(st->copy_vals.size > 0)
    sg = &(st->copy_vals);	// use the copy vals if relevant
  else {
    TypeDef* td = st->GetTypeDef();
    int i;
    for(i=TA_Stat.members.size; i<td->members.size; i++) {
      MemberDef* md = td->members.FastEl(i);
      if(md->type->InheritsFrom(TA_StatVal))
	sv.Add(((StatVal*)md->GetOff(st))->val);
      else if(md->type->InheritsFrom(TA_StatVal_List)) {
	sg = (StatVal_List*)md->GetOff(st);
	break;
      }
    }
  }
  if(sg != NULL) {
    int i;
    for(i=0; i<sg->size; i++)
      sv.Add(((StatVal*)sg->FastEl(i))->val);
  }
  if(pre_proc_1.opr != SimpleMathSpec::NONE)    sv.SimpleMath(pre_proc_1);
  if(pre_proc_2.opr != SimpleMathSpec::NONE)    sv.SimpleMath(pre_proc_2);
  if(pre_proc_3.opr != SimpleMathSpec::NONE)    sv.SimpleMath(pre_proc_3);
}


void UniquePatStat::Network_Stat() {
  if(pat_stat == NULL) {
    taMisc::Error("*** UniquePatStat", name,"must have a pat_stat to get patterns from");
    return;
  }
  if(data_env == NULL) {
    taMisc::Error("*** UniquePatStat", name,"must have an data_env to store data in");
    return;
  }

  GetStatVals(pat_stat, svals);
  EventSpec* es = (EventSpec*)data_env->event_specs[0];
  if(es == NULL) {
    InitStat();
    es = (EventSpec*)data_env->event_specs[0];
  }
  PatternSpec* ps = (PatternSpec*)es->patterns[0];
  ps->n_vals = svals.size;
  ps->UpdateAfterEdit();	// this will sort out cases where nvals > geom

  bool larger_further = float_RArray::LargerFurther(cmp_type);
  Event* newev = (Event*)data_env->events.NewEl(1);
  ((Pattern*)newev->patterns[0])->value.CopyFrom(&svals);

  float min_d = data_env->LastMinDist(data_env->events.size-1, 0, cmp_type, norm, dist_tol);
  bool keep_pat = false;
  if(larger_further)
    keep_pat = (min_d > uniq_tol);
  else
    keep_pat = (min_d < uniq_tol);

  if((data_env->events.size > 1) && !keep_pat)
    data_env->events.RemoveLast();

  float rval = (float)data_env->events.size;
  net_agg.ComputeAgg(&unq, rval);
}

//////////////////////////////////
//     TimeCounterStat		//
//////////////////////////////////

void TimeCounterStat::Initialize() {
  net_agg.op = Aggregate::LAST;
  loop_init = INIT_START_ONLY;
}

void TimeCounterStat::Copy_(const TimeCounterStat& cp) {
  time = cp.time;
}

void TimeCounterStat::InitStat() {
  //  time.InitStat(InitStatVal());
  InitStat_impl();
}

void TimeCounterStat::Init() {
  if(loop_init == NO_INIT) return;
  //  time.Init();
  Init_impl();
}

bool TimeCounterStat::Crit() {
  if(!has_stop_crit)    return false;
  if(n_copy_vals > 0)   return copy_vals.Crit();
  return time.Crit();
}

void TimeCounterStat::NameStatVals() {
  if(time_agg.from != NULL) {
    Stat::NameStatVals();
    return;
  }
  time.SetNarrowName("time");
}

void TimeCounterStat::Network_Stat() {
  time.val += 1.0f;
}

void TimeCounterStat::InitTime() {
  time.val = 0.0f;
}

////////////////

void TimeCounterStatResetProc::Initialize() {
  time_ctr_stat = NULL;
}

void TimeCounterStatResetProc::CutLinks() {
  taBase::DelPointer((TAPtr*)&time_ctr_stat);
  Process::CutLinks();
}

void TimeCounterStatResetProc::C_Code() {
  if(time_ctr_stat == NULL) {
    taMisc::Error("TimeCounterStatResetProc: Time not reset because time_ctr_stat = NULL in: ", GetPath());
    return;
  }
  time_ctr_stat->InitTime();
}

//////////////////////////////////
//     DisplayDataEnvProc	//
//////////////////////////////////

void DispDataEnvProc::Initialize() {
  data_env = NULL;
  pat_no = 0;
  disp_type = RAW_DATA_GRID;
  disp_log = NULL;
  dist_metric = float_RArray::EUCLIDIAN;
  dist_norm = false;
  dist_tol = 0.0f;
  x_axis_component = 0;
  y_axis_component = 1;
}

void DispDataEnvProc::CutLinks() {
  taBase::DelPointer((TAPtr*)&data_env);
  taBase::DelPointer((TAPtr*)&disp_log);
  Process::CutLinks();
}

bool DispDataEnvProc::DispIsGridLog() {
#ifdef TA_GUI
  if((disp_log != NULL) && disp_log->InheritsFrom(TA_GridLog))
    return true;
  if(disp_log != NULL) {
    taMisc::Error("*** DispDataEnvProc: disp_log is not a GridLog in:", GetPath(),
		  "making a new log");
  }
  GridLog* gl = (GridLog*)pdpMisc::GetNewLog(GET_MY_OWNER(Project), &TA_GridLog);
  if(gl == NULL) return false;
  taBase::SetPointer((TAPtr*)&disp_log, gl);
  return true;
#else
  return false;
#endif
}

bool DispDataEnvProc::DispIsGraphLog() {
#ifdef TA_GUI
  if((disp_log != NULL) && disp_log->InheritsFrom(TA_GraphLog))
    return true;
  if(disp_log != NULL) {
    taMisc::Error("*** DispDataEnvProc: disp_log is not a GraphLog in:", GetPath(),
		  "making a new log");
  }
  GraphLog* gl = (GraphLog*)pdpMisc::GetNewLog(GET_MY_OWNER(Project), &TA_GraphLog);
  if(gl == NULL) return false;
  taBase::SetPointer((TAPtr*)&disp_log, gl);
  return true;
#else
  return false;
#endif
}

void DispDataEnvProc::C_Code() {
#ifdef TA_GUI
  if(data_env == NULL) {
    taMisc::Error("DispDataEnvProc: data_env is NULL so nothing displayed in:", GetPath());
    return;
  }
  switch(disp_type) {
  case RAW_DATA_GRID:
    if(DispIsGridLog())
      data_env->EnvToGrid((GridLog*)disp_log, pat_no);
    break;
  case DIST_MATRIX:
    if(DispIsGridLog())
      data_env->DistMatrixGrid((GridLog*)disp_log, pat_no, dist_metric, dist_norm, dist_tol);
    break;
  case CLUSTER_PLOT:
    if(DispIsGraphLog())
      data_env->ClusterPlot((GraphLog*)disp_log, pat_no, dist_metric, dist_norm, dist_tol);
    break;
  case CORREL_MATRIX:
    if(DispIsGridLog())
      data_env->CorrelMatrixGrid((GridLog*)disp_log, pat_no);
    break;
  case PCA_EIGEN_GRID:
    if(DispIsGridLog())
      data_env->PCAEigenGrid((GridLog*)disp_log, pat_no);
    break;
  case PCA_PRJN_PLOT:
    if(DispIsGraphLog())
      data_env->PCAPrjnPlot((GraphLog*)disp_log, pat_no, x_axis_component, y_axis_component);
    break;
  case MDS_PRJN_PLOT:
    if(DispIsGraphLog())
      data_env->MDSPrjnPlot((GraphLog*)disp_log, pat_no, x_axis_component, y_axis_component);
    break;
  }
#endif
}

//////////////////////////////////
//     DispNetWeightsProc	//
//////////////////////////////////

void DispNetWeightsProc::Initialize() {
  recv_layer = NULL;
  send_layer = NULL;
  grid_log = NULL;
}

void DispNetWeightsProc::CutLinks() {
  taBase::DelPointer((TAPtr*)&recv_layer);
  taBase::DelPointer((TAPtr*)&send_layer);
  taBase::DelPointer((TAPtr*)&grid_log);
  Process::CutLinks();
}

bool DispNetWeightsProc::SetPtrs() {
  if(network == NULL) return false;
  taBase::DelPointer((TAPtr*)&recv_layer);
  taBase::DelPointer((TAPtr*)&send_layer);
  Layer* rl = (Layer*)network->layers.FindName(recv_layer_nm);
  if(rl == NULL) {
    taMisc::Error("DispNetWeightsProc: layer:", recv_layer_nm, "not found in network:",
		  network->GetName());
    return false;
  }
  Layer* sl = (Layer*)network->layers.FindName(send_layer_nm);
  if(sl == NULL) {
    taMisc::Error("DispNetWeightsProc: layer:", send_layer_nm, "not found in network:",
		  network->GetName());
    return false;
  }
  taBase::SetPointer((TAPtr*)&recv_layer, rl);
  taBase::SetPointer((TAPtr*)&send_layer, sl);

  if(grid_log != NULL) return true;

  GridLog* gl = (GridLog*)pdpMisc::GetNewLog(GET_MY_OWNER(Project), &TA_GridLog);
  if(gl == NULL) return false;
  taBase::SetPointer((TAPtr*)&grid_log, gl);
  return true;
}

void DispNetWeightsProc::C_Code() {
  if(!SetPtrs())
    return;

  network->GridViewWeights(grid_log, recv_layer, send_layer);
}

//////////////////////////////////
//     	   ClearLogProc		//
//////////////////////////////////

void ClearLogProc::Initialize() {
  log_to_clear = NULL;
}

void ClearLogProc::CutLinks() {
  taBase::DelPointer((TAPtr*)&log_to_clear);
  Process::CutLinks();
}

void ClearLogProc::C_Code() {
  if(log_to_clear == NULL) {
    taMisc::Error("ClearLogProc: log_to_clear is NULL so nothing cleared in:", GetPath());
    return;
  }
  log_to_clear->Clear();
}


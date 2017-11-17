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


// todo: ifdef this depedency for non-emergent build
#include <taMisc>

#include "NetworkState_cpp.h"

#include <LayerState_cpp>
#include <UnGpState_cpp>
#include <UnitState_cpp>

#include <LayerSpec_cpp>
#include <UnitSpec_cpp>
#include <ProjectionSpec_cpp>

// yikes -- need them all here, for the NewPrjnSpec method
#include <AllProjectionSpecs_cpp>

// common impl code from NetworkState_core

#include <State_cpp>


#include "NetworkState_core.cpp"

TA_BASEFUNS_CTORS_DEFN(NetStateThreadMgr);
TA_BASEFUNS_CTORS_DEFN(NetStateThreadTask);

#include "Network_mbrs.cpp"

using namespace std;

////////////////////////////////////////////////////////////
//              Threading


void NetStateThreadMgr::Initialize() {
  task_type = &TA_NetStateThreadTask;
  net_state = NULL;
}

void NetStateThreadMgr::InitState(int n_thr, NetworkState_cpp* ns) {
  n_threads = n_thr;
  net_state = ns;
  if(GetOwner() == NULL) {
    OwnTempObj();
  }
  InitAll();
}

void NetStateThreadMgr::InitAll() {
  if((threads.size == n_threads-1) && (tasks.size == n_threads)) return; // fast bail if same
  n_threads_prev = n_threads;
  inherited::InitAll();
  for(int i=0;i<tasks.size;i++) {
    NetStateThreadTask* uct = (NetStateThreadTask*)tasks[i];
    uct->net_state = net_state;
  }
}

void NetStateThreadMgr::Run(NetStateThreadCall& meth_call) {
  const int nt = tasks.size;
  for(int i=0;i<nt;i++) {
    NetStateThreadTask* ntt = (NetStateThreadTask*)tasks[i];
    ntt->meth_call = meth_call;
  }

  inherited::Run();
}


////////////////////////////////////////////////////////////
//              Regular

void NetworkState_cpp::StateError
(const char* a, const char* b, const char* c, const char* d, const char* e, const char* f,
 const char* g, const char* h, const char* i) const {
  String aa; String bb; String cc; String dd; String ee; String ff; String gg;
  String hh; String ii;
  if(a) aa =a;
  if(b) bb =b;
  if(c) cc =c;
  if(d) dd =d;
  if(e) ee =e;
  if(f) ff =f;
  if(g) gg =g;
  if(h) hh =h;
  if(i) ii =i;
  taMisc::Error(aa, bb, cc, dd, ee, ff, gg, hh, ii);
}

void NetworkState_cpp::StateErrorVals
  (const char* msg, const char* var1, float val1, const char* var2, float val2,
   const char* var3, float val3, const char* var4, float val4) const {
  String aa; String bb; String cc; String dd; String ee; String ff; String gg;
  String hh; String ii;
  if(var1) { aa = var1; bb = String(val1); }
  if(var2) { cc = var2; dd = String(val2); }
  if(var3) { ee = var3; ff = String(val3); }
  if(var4) { gg = var4; hh = String(val4); }
  taMisc::Error(msg, aa, bb, cc, dd, ee, ff, gg, hh);
}

void NetworkState_cpp::ThreadSyncSpin(int thr_no, int sync_no) {
  threads.SyncSpin(thr_no, sync_no);
}

void NetworkState_cpp::Init_InputData() {
  Init_InputData_Layers();
  NET_THREAD_CALL(NetworkState_cpp::Init_InputData_Thr);
}

void NetworkState_cpp::Init_Acts() {
  NET_THREAD_CALL(NetworkState_cpp::Init_Acts_Thr);
}

void NetworkState_cpp::Init_dWt(){
  NET_THREAD_CALL(NetworkState_cpp::Init_dWt_Thr);
}

void NetworkState_cpp::Init_Weights() {
  needs_wt_sym = false;          // will get set to true if needed

  if(HasNetFlag(INIT_WTS_1_THREAD)) {
    Init_Weights_1Thr();
    Init_Weights_renorm();
    if(needs_wt_sym) {
      Init_Weights_sym();
    }
  }
  else {
    NET_THREAD_CALL(NetworkState_cpp::Init_Weights_Thr);
    Init_Weights_renorm();
    if(needs_wt_sym) {
      Init_Weights_sym();
    }
  }
  
  Init_Weights_post();
  Init_Weights_Layers();

  Init_Acts();                  // also re-init state at this point..
  Init_Metrics();
}

void NetworkState_cpp::Init_Weights_renorm() {
  NET_THREAD_CALL(NetworkState_cpp::Init_Weights_renorm_Thr);
}

void NetworkState_cpp::Init_Weights_sym() {
  NET_THREAD_CALL(NetworkState_cpp::Init_Weights_sym_Thr);
}

void NetworkState_cpp::Init_Weights_post() {
  NET_THREAD_CALL(NetworkState_cpp::Init_Weights_post_Thr);
}

void NetworkState_cpp::Init_Metrics() {
  Init_Counters();
  Init_Stats();                 // in NetworkState_core.cpp -- calls layer stats too
  // Init_Timers();
}

// void NetworkState_cpp::Init_Counters() {
//   Init_Counters_impl();         // does all the work
//   Init_Counters_State();
// }
  
void NetworkState_cpp::Init_Timers() {
  // todo: figure out timers at some point
  // train_time.ResetUsed();
  // epoch_time.ResetUsed();
  // trial_time.ResetUsed();
  // settle_time.ResetUsed();
  // cycle_time.ResetUsed();
  // wt_sync_time.ResetUsed();
  // misc_time.ResetUsed();
}

void NetworkState_cpp::Compute_Netin() {
  NET_THREAD_CALL(NetworkState_cpp::Compute_Netin_Thr);
}

void NetworkState_cpp::Send_Netin() {
  NET_THREAD_CALL(NetworkState_cpp::Send_Netin_Thr);

  Send_Netin_Integ();           // integrate from temp buff
}

void NetworkState_cpp::Compute_Act() {
  NET_THREAD_CALL(NetworkState_cpp::Compute_Act_Thr);
}

void NetworkState_cpp::Compute_NetinAct() {
  NET_THREAD_CALL(NetworkState_cpp::Compute_NetinAct_Thr);
}

void NetworkState_cpp::Compute_dWt() {
  NET_THREAD_CALL(NetworkState_cpp::Compute_dWt_Thr);
}

void NetworkState_cpp::Compute_Weights() {
  NET_THREAD_CALL(NetworkState_cpp::Compute_Weights_Thr);
}


#ifdef DMEM_COMPILE

void NetworkState_cpp::DMem_SumDWts_ToTmp() {
  NET_THREAD_CALL(NetworkState_cpp::DMem_SumDWts_ToTmp_Thr);
}  
 
void NetworkState_cpp::DMem_SumDWts_FmTmp() {
  NET_THREAD_CALL(NetworkState_cpp::DMem_SumDWts_FmTmp_Thr);
}  

#endif

void NetworkState_cpp::Compute_SSE(bool unit_avg, bool sqrt) {
  NET_THREAD_CALL(NetworkState_cpp::Compute_SSE_Thr);
  Compute_SSE_Agg(unit_avg, sqrt);
}

void NetworkState_cpp::Compute_PRerr() {
  NET_THREAD_CALL(NetworkState_cpp::Compute_PRerr_Thr);
  Compute_PRerr_Agg();
}

void NetworkState_cpp::Compute_TrialStats() {
  Compute_SSE(stats.sse_unit_avg, stats.sse_sqrt);
  if(stats.prerr)
    Compute_PRerr();
}

void NetworkState_cpp::Compute_EpochStats() {
  Compute_EpochSSE();
  if(stats.prerr)
    Compute_EpochPRerr();
  Compute_EpochStats_Layers();
}

//////////////////////////////////////////////////////////////////////////
//              Building

bool NetworkState_cpp::NetStateMalloc(void** ptr, size_t sz) const {
  // alignment -- 64 = 64 byte (not bit) -- this is needed for Phi MIC but not clear
  // that it is useful for AVX2??  anyway, better safe than sorry?
  // 8/23/16 -- unnec to do the 64 byte align -- even any align may be not needed
  // and windows requires a different free based on the align alloc type, so
  // we are just standardizing on 16 byte align for all..
  // if(sz > 1024) {
#ifdef TA_OS_WIN
  *ptr = _aligned_malloc(sz, 16);
#else
  posix_memalign(ptr, 16, sz);
#endif
  // }
  // else {                        // don't bother with align for small guys..
  //   *ptr = malloc(sz);
  // }
  if(!*ptr) {
    StateError("Network::net_aligned_alloc memory allocation error! usually fatal -- please quit!  maybe your network is too big to fit into RAM?");
    return false;
  }
  return true;
}
  
bool NetworkState_cpp::NetStateFree(void** ptr) const {
  if(ptr && *ptr) {
#ifdef TA_OS_WIN
    _aligned_free(*ptr);
#else
    free(*ptr);
#endif
    *ptr = NULL;
    return true;
  }
  return false;
}

void NetworkState_cpp::BuildUnitState() {
  // AllocLayerUnitMem has been called, and all the layer, prjn, ungp state initialized by this point

  // this assigns units to threads, also counts up number of con states
  InitUnitIdxs();               // in _core
  
  // absent any other special process, the first thread to touch (write) to a memory block
  // gets that memory allocated on its physical memory.  so we simplify things
  // by doing all the malloc in one method, and then ensure that the threads
  // initialize the memory during the subsequent Init call

  NET_THREAD_CALL(NetworkState_cpp::InitUnitState_Thr); // _core

  LayoutUnits(); // _core
}

void NetworkState_cpp::BuildConState() {
  AllocConStateMem();           // _core
  
  NET_THREAD_CALL(NetworkState_cpp::InitConState_Thr); // _core
}

void NetworkState_cpp::BuildSendNetinTmp() {
  AllocSendNetinTmpState();     // _core
  
  NET_THREAD_CALL(NetworkState_cpp::InitSendNetinTmp_Thr); // _core
}


void NetworkState_cpp::Connect() {
  // RemoveCons(); // -- has been called at Network level.

  Connect_Sizes();
  Connect_Alloc();
  NET_THREAD_CALL(NetworkState_cpp::Connect_CacheMemStart_Thr);
  Connect_Cons(1);              // first pass

  if(needs_prjn_pass2) {
    NET_THREAD_CALL(NetworkState_cpp::Connect_CacheUnitLoHiIdxs_Thr);
    Connect_Cons(2);              // second pass (symmetry)
  }

  NET_THREAD_CALL(NetworkState_cpp::Connect_CacheUnitLoHiIdxs_Thr);
  NET_THREAD_CALL(NetworkState_cpp::Connect_VecChunk_Thr);
  NET_THREAD_CALL(NetworkState_cpp::Connect_UpdtActives_Thr);

  CountCons();
}

void NetworkState_cpp::Connect_Alloc() {
  AllocConsCountStateMem();
  
  NET_THREAD_CALL(NetworkState_cpp::Connect_AllocSizes_Thr);

  AllocConnectionMem();
  
  NET_THREAD_CALL(NetworkState_cpp::Connect_Alloc_Thr); // allocate to con groups
}

void NetworkState_cpp::UnBuildState() {
  // threads.RemoveAll();
  FreeStateMem();
  n_units = 0;
}


//////////////////////////////////////////////////////////////////////////
//            Weight Save / Load -- Network has all the code..


// todo: need some kind of string class.. could figure out std::string..

String NetworkState_cpp::StateLexBuf;

int NetworkState_cpp::skip_white_noeol(istream& strm, bool peek ) {
  int c;
  while (((c=strm.peek()) == ' ') || (c == '\t') || (c == '\r'))
    strm.get();
  if(!peek)
    strm.get();
  return c;
}

int NetworkState_cpp::read_till_eol(istream& strm, bool peek ) {
  int c = skip_white_noeol(strm, true);
  StateLexBuf = "";
  while (((c = strm.peek()) != EOF) && !((c == '\n'))) {
    if(c != '\r') StateLexBuf += (char)c;
    strm.get();
  }
  if(!peek)
    strm.get();
  return c;
}


int NetworkState_cpp::skip_white(istream& strm, bool peek ) {
  int c;
  while (((c=strm.peek()) == ' ') || (c == '\t') || (c == '\n') || (c == '\r'))
    strm.get();
  if(!peek)
    strm.get();
  return c;
}

int NetworkState_cpp::read_till_rangle(istream& strm, bool peek ) {
  int c;
  StateLexBuf = "";
  int depth = 0;
  while (((c = strm.peek()) != EOF) && !((c == '>') && (depth <= 0))) {
    StateLexBuf += (char)c;
    if(c == '<')      depth++;
    if(c == '>')      depth--;
    strm.get();
  }
  if(!peek)
    strm.get();
  return c;
}

NetworkState_cpp::ReadTagStatus NetworkState_cpp::read_tag(istream& strm, String& tag, String& val) {
  int c = skip_white(strm, true);
  if(c == EOF) return TAG_EOF;
  if(c != '<') return TAG_NONE;
  strm.get();
  c = strm.peek();
  if(c == EOF) return TAG_EOF;
  ReadTagStatus rval = TAG_GOT;
  if(c == '/') {
    strm.get();
    rval = TAG_END;
  }
  read_till_rangle(strm, false);
  if(StateLexBuf.contains(' ')) {
    tag = StateLexBuf.before(' ');
    val = StateLexBuf.after(' ');
  }
  else {
    tag = StateLexBuf;
    val = "";
  }
  c = strm.peek();
  if(c == '\r' || c == '\n') strm.get(); // absorb an immediate cr after tag, which is common
  if(c == '\r') {
    c = strm.peek();
    if(c == '\n') strm.get();   // absorb an immediate cr after tag, which is common
  }
  return rval;
}

// return values:
// TAG_END = successfully got to end of thing;
// TAG_NONE = some kind of error
// TAG_EOF = EOF

int NetworkState_cpp::LoadWeights_StartTag(istream& strm, const String& tag, String& val, bool quiet) {
  String in_tag;
  int stat = read_tag(strm, in_tag, val);
  if(stat == TAG_END) return TAG_NONE; // some other end -- not good
  if(stat != TAG_GOT) {
    if(!quiet) cerr << "LoadWeights_StartTag: bad read of start tag: " << tag << endl;
    return stat;
  }
  if(in_tag != tag) {
    if(!quiet) cerr << "LoadWeights_StartTag: read different start tag: " << in_tag
                    << "expecting: " << tag << endl;
    return TAG_NONE; // bumping up against some other tag
  }
  return stat;
}

// static
int NetworkState_cpp::LoadWeights_EndTag(istream& strm, const String& trg_tag, String& cur_tag,
                              int& stat, bool quiet) {
  String val;
  if(stat != TAG_END)   // haven't already hit the end
    stat = read_tag(strm, cur_tag, val);
  if((stat != TAG_END) || (cur_tag != trg_tag)) {
    if(!quiet) cerr << "ConState::LoadWeights: bad read of end tag:" << trg_tag << "got:"
                    << cur_tag << "stat:" << stat << endl;
    if(stat == TAG_END) stat = TAG_NONE;
  }
  return stat;
}

int NetworkState_cpp::ConsSkipWeights_strm(istream& strm, NetworkState_cpp::WtSaveFormat fmt, bool quiet) {
  String tag, val;
  int stat = LoadWeights_StartTag(strm, "Cn", val, quiet);
  if(stat != TAG_GOT) return stat;

  int sz = (int)val;
  if(sz < 0) {
    return TAG_NONE;
  }

  for(int i=0; i < sz; i++) {
    int lidx;
    float wt;
    if(fmt == NetworkState_cpp::TEXT) {
      read_till_eol(strm);
    }
    else {                      // binary
      strm.read((char*)&(lidx), sizeof(lidx));
      strm.read((char*)&(wt), sizeof(wt));
    }
  }
  LoadWeights_EndTag(strm, "Cn", tag, stat, quiet);
  return stat;
}

int NetworkState_cpp::SkipWeights_strm(istream& strm, NetworkState_cpp::WtSaveFormat fmt, bool quiet) {
  String tag, val;
  int stat = LoadWeights_StartTag(strm, "Un", val, quiet);
  if(stat != TAG_GOT) return stat;

  float bwt = 0.0;
  switch(fmt) {
  case NetworkState_cpp::TEXT:
    read_till_eol(strm);
    bwt = (float)StateLexBuf;
    break;
  case NetworkState_cpp::BINARY:
    strm.read((char*)&bwt, sizeof(bwt));
    strm.get();         // get the /n
    break;
  }
  while(true) {
    stat = read_tag(strm, tag, val);
    if(stat != TAG_GOT) break;          // *should* break at TAG_END
    if(tag != "Cg") { stat = TAG_NONE;  break; } // bumping up against some other tag
    stat = ConsSkipWeights_strm(strm, fmt, quiet); // skip over
    if(stat != TAG_END) break; // something is wrong
    stat = TAG_NONE;           // reset so EndTag will definitely read new tag
    LoadWeights_EndTag(strm, "Cg", tag, stat, quiet);
    if(stat != TAG_END) break;
  }
  LoadWeights_EndTag(strm, "Un", tag, stat, quiet);
  return stat;
}

/////////////////////////////////////////////////////////////
//      Network-level  Save/Load Weights

void NetworkState_cpp::NetworkSaveWeights_strm(ostream& strm, WtSaveFormat fmt) {
  strm << "<Fmt " << ((fmt == TEXT) ? "TEXT" : "BINARY") << ">\n"
       << "<Name " << "network_name" << ">\n" // todo: network name!
       << "<Epoch " << epoch << ">\n";
  for(int li=0; li < n_layers_built; li++) {
    LayerState_cpp* lay = GetLayerState(li);
    if(lay->lesioned()) continue;
    strm << "<Lay " << lay->layer_name << ">\n";
    LayerSaveWeights_strm(strm, lay, fmt);
    strm << "</Lay>\n";
  }
}

bool NetworkState_cpp::NetworkLoadWeights_strm(istream& strm, bool quiet) {
  String tag, val;
  int stat = 0;
  WtSaveFormat fmt;

  int c = strm.peek();
  if(c == '#') {
    cerr << "LoadWeights_strm: cannot read old formats from version 3.2 -- must use network save" << endl;
    return false;
  }
  stat = read_tag(strm, tag, val);
  if(stat != TAG_GOT || (tag != "Fmt")) {
    cerr << "LoadWeights_strm: did not got find Fmt tag at start of weights file -- probably file not found"
         << endl;
    return false;
  }

  if(val == "BINARY")
    fmt = BINARY;
  else
    fmt = TEXT;

  stat = read_tag(strm, tag, val);
  if((stat != TAG_GOT) || (tag != "Name")) return false;
  // don't set the name!!! this causes more trouble than it is worth!!
//   name = val;

  stat = read_tag(strm, tag, val);
  if((stat != TAG_GOT) || (tag != "Epoch")) return false;
  epoch = (int)val;

  while(true) {
    stat = read_tag(strm, tag, val);
    if(stat != TAG_GOT) break;          // *should* break at TAG_END
    if(tag != "Lay") { stat = TAG_NONE;  break; } // bumping up against some other tag
    LayerState_cpp* lay = FindLayerName(val);
    if(lay) {
      stat = LayerLoadWeights_strm(strm, lay, fmt, quiet);
    }
    else {
      if(!quiet) {
        cerr << "LoadWeights: Layer not found: " << val << endl;
      }
      stat = SkipWeights_strm(strm, fmt, quiet);
    }
    if(stat != TAG_END) break;
    stat = TAG_NONE;           // reset so EndTag will definitely read new tag
    LoadWeights_EndTag(strm, "Lay", tag, stat, quiet);
    if(stat != TAG_END) break;
  }

  NET_THREAD_CALL(NetworkState_cpp::Connect_VecChunk_Thr);

  // could try to read end tag but what is the point?
  return true;
}

/////////////////////////////////////////////////////////////
//  Layer-level  Save/Load Weights


void NetworkState_cpp::LayerSaveWeights_LayerVars(ostream& strm, LayerState_cpp* lay, WtSaveFormat fmt) {
  // overload this to save specific layer state -- see Leabra for example
}

void NetworkState_cpp::LayerSaveWeights_strm(ostream& strm, LayerState_cpp* lay, WtSaveFormat fmt,
                                             PrjnState_cpp* prjn) {
  // name etc is saved & processed by network level guy -- this is equiv to unit group

  LayerSaveWeights_LayerVars(strm, lay, fmt);
  
  strm << "<Ug>\n";
  for(int ui = 0; ui < lay->n_units; ui++) {
    UNIT_STATE* u = lay->GetUnitState(this, ui);
    if(u->lesioned()) continue;
    int lfi = u->lay_un_idx;
    strm << "<UgUn " << lfi << " >\n"; // note: space is b/c old fmts could have name there..
    UnitSaveWeights_strm(strm, u, fmt, prjn);
    strm << "</UgUn>\n";
  }
  strm << "</Ug>\n";
}

int NetworkState_cpp::LayerLoadWeights_LayerVars(istream& strm, LayerState_cpp* lay, WtSaveFormat fmt,
                                                  bool quiet) {
  // overload this to load specific layer state -- see Leabra for example
  return TAG_END;
}

int NetworkState_cpp::LayerLoadWeights_strm(istream& strm, LayerState_cpp* lay, WtSaveFormat fmt,
                                            bool quiet, PrjnState_cpp* prjn) {
  // name etc is saved & processed by network level guy -- this is equiv to unit group

  LayerLoadWeights_LayerVars(strm, lay, fmt, quiet);
  
  String tag, val;
  int stat = LoadWeights_StartTag(strm, "Ug", val, quiet);
  if(stat != TAG_GOT) return stat;

  while(true) {
    stat = read_tag(strm, tag, val);
    if(stat != TAG_GOT) break;          // *should* break at TAG_END
    if(tag != "UgUn") { stat = TAG_NONE;  break; } // bumping up against some other tag
    int lfi = (int)val.before(' ');
    if(lfi < lay->n_units) {
      UnitState_cpp* u = lay->GetUnitState(this, lfi);
      stat = UnitLoadWeights_strm(strm, u, fmt, quiet, prjn);
    }
    else {
      stat = SkipWeights_strm(strm, fmt, quiet);
    }
    if(stat != TAG_END) break;
    stat = TAG_NONE;           // reset so EndTag will definitely read new tag
    LoadWeights_EndTag(strm, "UgUn", tag, stat, quiet);
    if(stat != TAG_END) break;
  }
  LoadWeights_EndTag(strm, "Ug", tag, stat, quiet);
  return stat;
}


/////////////////////////////////////////////////////////////
//      Unit-level  Save/Load Weights


void NetworkState_cpp::UnitSaveWeights_strm(ostream& strm, UnitState_cpp* u, WtSaveFormat fmt,
                                            PrjnState_cpp* prjn) {
  strm << "<Un>\n";
  switch(fmt) {
  case TEXT:
    strm << u->bias_wt << "\n";
    break;
  case BINARY:
    strm.write((char*)&(u->bias_wt), sizeof(u->bias_wt));
    strm << "\n";
    break;
  }
  const int rsz = u->NRecvConGps(this);
  for(int g = 0; g < rsz; g++) {
    ConState_cpp* cg = u->RecvConState(this, g);
    PrjnState_cpp* cg_prjn = cg->GetPrjnState(this);
    if(cg->NotActive() || (prjn && (cg_prjn != prjn)) || cg->Sharing()) continue;
    LayerState_cpp* fm = cg_prjn->GetSendLayerState(this);
    if(fm->lesioned()) continue; // shouldn't happen!
    strm << "<Cg " << g << " Fm:" << fm->layer_name << ">\n";
    ConsSaveWeights_strm(strm, cg, u, fmt);
    strm << "</Cg>\n";
  }
  strm << "</Un>\n";
}

int NetworkState_cpp::UnitLoadWeights_strm(istream& strm, UnitState_cpp* u, WtSaveFormat fmt,
                                           bool quiet, PrjnState_cpp* prjn) {
  String tag, val;
  int stat = LoadWeights_StartTag(strm, "Un", val, quiet);
  if(stat != TAG_GOT) return stat;

  //   String lidx = val.before(' ');
  // todo: could compare lidx with GetMyLeafIdx()...
  float bwt = 0.0;
  switch(fmt) {
  case TEXT:
    read_till_eol(strm);
    bwt = (float)StateLexBuf;
    break;
  case BINARY:
    strm.read((char*)&bwt, sizeof(bwt));
    strm.get();         // get the /n
    break;
  }
  
  UnitSpec_cpp* us = u->GetUnitSpec(this);
  if(us) {
    us->LoadBiasWtVal(bwt, u, this);
  }
  else {
    u->bias_wt = bwt;
  }

  while(true) {
    stat = read_tag(strm, tag, val);
    if(stat != TAG_GOT) break;                  // *should* break at TAG_END
    if(tag != "Cg") { stat = TAG_NONE;  break; } // bumping up against some other tag
    int gi = (int)val.before(' ');
    String fm_nm = val.after("Fm:");
    ConState_cpp* cg = NULL;
    PrjnState_cpp* cg_prjn = NULL;
    const int rsz = u->NRecvConGps(this);
    if(gi < rsz) {
      cg = u->RecvConState(this, gi);
      cg_prjn = cg->GetPrjnState(this);
      LayerState_cpp* fm = cg_prjn->GetSendLayerState(this);
      if(!fm->LayerNameIs(fm_nm)) {
        cg = u->FindRecvConStateFromName(this, fm_nm);
      }
    }
    else {
      cg = u->FindRecvConStateFromName(this, fm_nm);
    }
    if(cg && !(cg->NotActive() || (prjn && (cg_prjn != prjn)) || cg->Sharing())) {
      stat = ConsLoadWeights_strm(strm, cg, u, fmt, quiet);
    }
    else {
      stat = ConsSkipWeights_strm(strm, fmt, quiet); // skip over
    }
    if(stat != TAG_END) break; // something is wrong
    stat = TAG_NONE;           // reset so EndTag will definitely read new tag
    LoadWeights_EndTag(strm, "Cg", tag, stat, quiet);
    if(stat != TAG_END) break;
  }

  LoadWeights_EndTag(strm, "Un", tag, stat, quiet);
  return stat;
}


/////////////////////////////////////////////////////////////
//      Unit-level  Save/Load Weights

void NetworkState_cpp::ConsSaveWeights_strm(ostream& strm, ConState_cpp* cg, UnitState_cpp* un,
                                            WtSaveFormat fmt) {
  if(cg->NotActive()) {
    strm << "<Cn 0>\n" << "</Cn>\n";
    return;
  }

  // PrjnState_cpp* prjn = cg->GetPrjnState(this);
  ConSpec_cpp* cs = cg->GetConSpec(this);
  int n_vars = 0;
  int smds[10];          // no more than 10!
  for(int i=0; i<cg->n_con_vars; i++) {
    if(cs->SaveVar(cg, this, i)) {
      smds[n_vars++] = i;
    }
  }
  
  strm << "<Cn " << cg->size << ">\n";
  switch(fmt) {
  case TEXT:
    for(int i=0; i < cg->size; i++) {
      int lidx = cg->UnState(i,this)->lay_un_idx;
      strm << lidx;
      for(int mi=0; mi < n_vars; mi++) {
        strm << " " << cg->Cn(i,smds[mi],this);
      }
      strm << "\n";
    }
    break;
  case BINARY:
    for(int i=0; i < cg->size; i++) {
      int lidx = cg->UnState(i,this)->lay_un_idx;
      strm.write((char*)&(lidx), sizeof(lidx));
      for(int mi=0; mi < n_vars; mi++) {
        strm.write((char*)&(cg->Cn(i,smds[mi],this)), sizeof(float));
      }
    }
    strm << "\n";
    break;
  }
  strm << "</Cn>\n";
}

int NetworkState_cpp::ConsLoadWeights_strm(istream& strm, ConState_cpp* cg, UnitState_cpp* ru,
                                           WtSaveFormat fmt, bool quiet) {
  static bool warned_already = false;
  static bool sz_warned_already = false;

  ConSpec_cpp* cs = cg->GetConSpec(this);
  PrjnState_cpp* prjn = cg->GetPrjnState(this);
  LayerState_cpp* fm_lay = prjn->GetSendLayerState(this);
  
  String tag, val;
  int stat = LoadWeights_StartTag(strm, "Cn", val, quiet);
  if(stat != TAG_GOT) return stat;

  int sz = (int)val;
  if(sz < 0) {
    if(!quiet) {
      cerr << "ConsLoadWeights_strm: read size < 0" << endl;
    }
    return TAG_NONE;
  }
  if(sz < cg->size) {
    if(!quiet && !sz_warned_already) {
      cerr << "ConsLoadWeights_strm: weights file has fewer connections: " << sz
           << " than existing group size of: " << cg->size << endl;
      sz_warned_already = true;
    // doesn't really make sense to nuke these -- maybe add a flag overall to enable this
//     for(int i=size-1; i >= sz; i--) {
//       UnitState_cpp* su = Un(i);
//       ru->DisConnectFrom(su, prjn);
//     }
    }
  }
  else if(sz > cg->size) {
    if(sz > cg->alloc_size) {
      if(!quiet && !sz_warned_already) {
        cerr << "ConsLoadWeights_strm: weights file has more connections: " << sz
             << " than allocated size: " << cg->alloc_size <<  " -- only alloc_size will be loaded"
             << endl;
        sz_warned_already = true;
        sz = cg->alloc_size;
      }
    }
    else {
      if(!quiet && !sz_warned_already) {
        cerr << "ConsLoadWeights_strm: weights file has more connections: " << sz
             << " than existing group size of: " << cg->size
             << " -- but these will fit within alloc_size and will be loaded"  << endl;
        sz_warned_already = true;
      }
    }
  }
  else {
    sz_warned_already = false;
  }

  int n_vars = 0;
  int smds[10];          // no more than 10!
  for(int i=0; i<cg->n_con_vars; i++) {
    if(cs->SaveVar(cg, this, i)) {
      smds[n_vars++] = i;
    }
  }

  float wtvals[10];             // 10 here is number of diff vars!
  int n_wts_loaded = 0;

  for(int i=0; i < sz; i++) {   // using load size as key factor
    int lidx;
    if(fmt == TEXT) {
      read_till_eol(strm);
      int vidx = 0;
      int last_ci = 0;
      const int lbln = StateLexBuf.length();
      int ci;
      for(ci = 1; ci < lbln; ci++) {
        if(StateLexBuf[ci] != ' ') continue;
        if(last_ci == 0) {
          lidx = (int)StateLexBuf.before(ci);
        }
        else {
          wtvals[vidx++] = (float)StateLexBuf.at(last_ci, ci-last_ci);
        }
        last_ci = ci+1;
      }
      if(ci > last_ci) {
        wtvals[vidx++] = (float)StateLexBuf.at(last_ci, ci-last_ci);
      }
      n_wts_loaded = MIN(vidx, n_vars); // can't effectively load more than we can use!
    }
    else {                      // binary
      strm.read((char*)&(lidx), sizeof(lidx));
      for(int mi=0; mi<n_vars; mi++) { // no way to check! MUST be right format..
        strm.read((char*)&(wtvals[mi]), sizeof(float));
      }
      n_wts_loaded = n_vars;
    }
    UnitState_cpp* su = fm_lay->GetUnitStateSafe(this, lidx);
    if(!su) {
      if(!quiet && !warned_already) {
        cerr << "ConsLoadWeights_strm: unit at leaf index: " << lidx
             << " not found in layer: " << fm_lay->layer_name << " removing this connection" << endl;
        warned_already = true;
      }
      if(cg->size > i) {
        ru->DisConnectFrom(this, su, prjn);
      }
      sz--;                            // now doing less..
      i--;
      continue;
    }
    ConState_cpp* send_gp = su->SendConStatePrjn(this, prjn);
    if(!send_gp) {
      if(!quiet && !warned_already) {
        cerr << "ConsLoadWeights_strm: unit at leaf index: " << lidx
             << " does not have proper send group: " << prjn->send_idx << endl;
        warned_already = true;
      }
      if(cg->size > i) {
        ru->DisConnectFrom(this, su, prjn); // remove this guy to keep total size straight
      }
      sz--;                            // now doing less..
      i--;
      continue;
    }
    if(i >= cg->size) {             // new connection
      // too many msgs with this:
      if(!quiet && !warned_already) {
        cerr << "ConsLoadWeights_strm: attempting to load beyond size of allocated connections -- cannot do this" << endl;
        warned_already = true;
      }
      //      ru->ConnectFromCk(su, prjn, false, true, wtval); // set init wt
    }
    else if(su != cg->UnState(i,this)) {
      // not same unit -- note that at this point, the only viable strategy is to discon
      // all existing cons and start over, as otherwise everything will be hopelessly out
      // of whack -- this never happens
      if(!quiet && !warned_already) {
        cerr << "ConsLoadWeights_strm: unit at index: " << i
             << " in cons group does not match the loaded unit -- weights will be off" << endl;
        warned_already = true;
      }

      for(int mi=1; mi<n_wts_loaded; mi++) { // set non-weight params first!
        cg->Cn(i,smds[mi],this) = wtvals[mi];
      }
      cs->LoadWeightVal(wtvals[0], cg, i, this);

      // this is not viable:
      // for(int j=cg->size-1; j >= i; j--) {
      //   UnitState_cpp* su = Un(j,this);
      //   ru->DisConnectFrom(su, prjn);
      // }
      // ru->ConnectFromCk(su, prjn, false, true, wtval); // set init wt
    }
    else {                      // all good normal case, just set the weights!
      warned_already = false;
      for(int mi=1; mi<n_wts_loaded; mi++) { // set non-weight params first!
        cg->Cn(i,smds[mi],this) = wtvals[mi];
      }
      cs->LoadWeightVal(wtvals[0], cg, i, this);
    }
  }
  LoadWeights_EndTag(strm, "Cn", tag, stat, quiet);
  return stat;                  // should be tag end!
}


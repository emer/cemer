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

#ifndef NetworkState_cpp_h
#define NetworkState_cpp_h 1

// State classes define full implementation of code -- this is included in all other
// State classes so it cannot depend on any of them -- every other class uses
// NetworkState to access their State variables which are allocated here

#ifndef __MAKETA__
# include <cmath>
# include <math.h>
#endif

#include "network_def.h"

#include <State_main>

#include <taTask>
#include <taThreadMgr>

#include <State_cpp>

// this defines classes that are members of NetworkState -- as _cpp
#include <Network_mbrs>

////////////////////////////////////////////////////////////
//              Define our own Thread manager
// todo: later need to make this independent of taThread code

// declare all other types mentioned but not required to include:
class NetworkState_cpp; //
class NetStateThreadMgr; //

// this is the standard network function call taking the thread number int value
// all threaded unit-level functions MUST use this call signature!
#ifdef __MAKETA__
typedef void* NetStateThreadCall;
#else
typedef taTaskMethCall1<NetworkState_cpp, void, int> NetStateThreadCall;
typedef void (NetworkState_cpp::*NetStateThreadMethod)(int);
#endif

#define NET_THREAD_CALL(meth) { NetStateThreadCall meth_call((NetStateThreadMethod)(&meth));\
  threads.Run(meth_call); }


eTypeDef_Of(NetStateThreadTask);

class E_API NetStateThreadTask : public taTask {
  // Network state thread task
typedef taTask inherited;
public:
  NetStateThreadCall     meth_call; // #IGNORE method to call on the network
  NetworkState_cpp*      net_state; // #IGNORE network state
  
  void run() override {
    meth_call.call(net_state, task_id); // task id indicates threading, and which thread
  }

  NetStateThreadMgr* mgr() { return (NetStateThreadMgr*)owner->GetOwner(); }
  
  TA_BASEFUNS_NOCOPY(NetStateThreadTask);
private:
  void  Initialize() { net_state =  NULL; }
  void  Destroy()    { };
};

eTypeDef_Of(NetStateThreadMgr);

class E_API NetStateThreadMgr : public taThreadMgr {
  // #INLINE thread manager for network methods -- manages threads and tasks, and coordinates threads running the tasks
typedef taThreadMgr inherited;
public:
  NetworkState_cpp*     net_state;

  void  InitAll() override;      // initialize threads and tasks

  void  InitState(int n_thr, NetworkState_cpp* ns);
  // init the state threads -- called by network if it ever changes
  
  void Run(NetStateThreadCall& meth_call);
  // #IGNORE run given function on the NetState, passing thread number as arg

  TA_BASEFUNS_NOCOPY(NetStateThreadMgr);
protected:

  int           n_threads_prev;         // #IGNORE number of threads set previously in net build -- for update diffs

private:
  void  Initialize();
  void  Destroy() { }
};

class E_API NetworkState_cpp {
  // ##NO_TOKENS ##NO_UPDATE_AFTER #STEM_BASE ##CAT_Network plain C++ implementation of NetworkState 
public:

  enum WtSaveFormat {
    TEXT,                       // weights are saved as ascii text representation of digits (completely portable)
    BINARY,                     // weights are written directly to the file in binary format (no loss in accuracy and more space efficient, but possibly non-portable)
  };
  
  enum ReadTagStatus {
    TAG_GOT,                    // got a starting tag <xxx...>
    TAG_END,                    // got an ending tag </xx>
    TAG_NONE,                   // no start of < tag there
    TAG_EOF,                    // got an EOF
  };


// this directly defines a bunch of shared vars with Network
#include <Network_core>
// and common state between C++ and _cuda
#include <NetworkState_core>

  NetStateThreadMgr threads;  // #CAT_Threads parallel threading of network computation
  
  //////////////////////////////////////////////////////////////////////
  //    C++ versions of all major code -- Network.cpp just does switch
  //    on cuda vs cpp -- cuda must define its own versions

  virtual void  Init_InputData();
  // #CAT_Activation Initializes external and target inputs
  virtual void  Init_Acts();
  // #CAT_Activation initialize the unit activation state variables
  virtual void  Init_dWt();
  // #CAT_Learning Initialize the weight change variables
  virtual void  Init_Weights();
  // #CAT_Learning Initialize the weights -- also inits acts, counters and stats -- does unit level threaded and then does Layers after
    virtual void Init_Weights_renorm();
    // #IGNORE renormalize weights after init, before sym
    virtual void Init_Weights_sym();
    // #IGNORE symmetrize weights after first init pass, called when needed
    virtual void Init_Weights_post();
    // #CAT_Learning post-initialize state variables (ie. for scaling symmetrical weights, other wt state keyed off of weights, etc) -- this MUST be called after any external modifications to the weights, e.g., the TransformWeights or AddNoiseToWeights calls on any lower-level objects (layers, units, con groups)

  virtual void  Init_Metrics();
  // #CAT_Statistic this is an omnibus guy that initializes every metric: Counters, Stats, and Timers

  virtual void  Init_Counters();
  // #EXPERT #CAT_Counter initialize all counter variables on network (called in Init_Weights; except batch because that loops over inits!)

  virtual void  Init_Sequence() { };
  // #CAT_Activation called by NetGroupedDataLoop at the start of a sequence (group) of input data events -- some algorithms may want to have a flag to optionally initialize activations at this point

  virtual void  Compute_Netin();
  // #CAT_Activation Compute NetInput: weighted activation from other units
  virtual void  Send_Netin();
  // #CAT_Activation sender-based computation of net input: weighted activation from other units
  virtual void  Compute_Act();
  // #CAT_Activation Compute Activation based on net input
  virtual void  Compute_NetinAct();
  // #CAT_Activation compute net input from other units and then our own activation value based on that -- use this for feedforward networks to propagate activation through network in one compute cycle

  virtual void  Compute_dWt();
  // #CAT_Learning compute weight changes -- the essence of learning

  virtual void  Compute_Weights();
  // #CAT_Learning update weights for whole net -- DMem_SumDWts must have already been called if in dmem mode
#ifdef DMEM_COMPILE
  virtual void  DMem_SumDWts_ToTmp();
  // #IGNORE copy to temp send buffer for sending, per thread
  virtual void  DMem_SumDWts_FmTmp();
  // #IGNORE copy from temp recv buffer, per thread
#endif

  virtual void  Compute_SSE(bool unit_avg = false, bool sqrt = false);
  // #CAT_Statistic compute sum squared error of activations vs targets over the entire network -- optionally taking the average over units, and square root of the final results
  virtual void  Compute_PRerr();
  // #CAT_Statistic compute precision and recall error statistics over entire network -- true positive, false positive, and false negative -- precision = tp / (tp + fp) recall = tp / (tp + fn) fmeasure = 2 * p * r / (p + r), specificity, fall-out, mcc.

  virtual void  Compute_TrialStats();
  // #CAT_Statistic compute trial-level statistics (SSE and others defined by specific algorithms)
  virtual void  Compute_EpochStats();
  // #CAT_Statistic compute epoch-level statistics; calls DMem_ComputeAggs (if dmem) and EpochSSE -- specific algos may add more


  /////////////////////////////////////////////////////////////////////
  //            Build Code
  
  virtual bool  NetStateMalloc(void** ptr, size_t sz) const;
  // #IGNORE properly (maximally) aligned memory allocation routine to given pointer of given number of bytes -- alignment is (currently) 64 bytes
  
  virtual bool  NetStateFree(void** ptr) const; 
  // #IGNORE free previously malloc'd memory, and set *ptr = NULL

  virtual void  BuildUnitState();
  // #IGNORE build unit state objects and associated indexes, after memory has been allocated and higher-level (layer, prjn, ungp) state has been initialized

  virtual void  BuildConState();
  // #IGNORE build connection-state objects and associated indexes -- also calls BuildSendNetinTmp
  
  virtual void  BuildSendNetinTmp();
  // #IGNORE build sender-based netinput temporary memory -- called after connect -- can be selectively overridden in subclasses
  
  virtual void  Connect();
  // #IGNORE Connect this network according to projections on Layers -- must be done as part of Build to ensure proper sync
  virtual void  Connect_Alloc();
  // #IGNORE second pass of connecting -- allocate all the memory for all the connections -- managed by the Network and done by thread

  virtual void  UnBuildState();
  // #IGNORE main unbuild call for state-side -- frees threads, mem
  
  //////////////////////////////////////////////////////////////////////////
  //            Weight Save / Load -- Network Manages Everything 
  
  virtual void  NetworkSaveWeights_strm(std::ostream& strm, WtSaveFormat fmt = TEXT);
  // #EXT_wts #COMPRESS #CAT_File write weight values out in a simple ordered list of weights (optionally in binary fmt)
  virtual bool  NetworkLoadWeights_strm(std::istream& strm, bool quiet = false);
  // #EXT_wts #COMPRESS #CAT_File read weight values in from a simple ordered list of weights (fmt is read from file)

  virtual void  LayerSaveWeights_strm(std::ostream& strm, LayerState_cpp* lay, WtSaveFormat fmt = TEXT,
                                      PrjnState_cpp* prjn = NULL);
  // #EXT_wts #COMPRESS #CAT_File write weight values out in a simple ordered list of weights (optionally in binary fmt)
  virtual int   LayerLoadWeights_strm(std::istream& strm, LayerState_cpp* lay, WtSaveFormat fmt = TEXT,
                                      bool quiet = false, PrjnState_cpp* prjn = NULL);
  // #EXT_wts #COMPRESS #CAT_File read weight values in from a simple ordered list of weights (optionally in binary fmt) -- rval is taMisc::ReadTagStatus = END_TAG if successful

  virtual void  LayerSaveWeights_LayerVars(std::ostream& strm, LayerState_cpp* lay, WtSaveFormat fmt = TEXT);
  // #EXT_wts #COMPRESS #CAT_File write layer state values that are key adaptive state that determines network function
  virtual int   LayerLoadWeights_LayerVars(std::istream& strm, LayerState_cpp* lay,
                                           WtSaveFormat fmt = TEXT, bool quiet = false);
  // #EXT_wts #COMPRESS #CAT_File write layer state values that are key adaptive state that determines network function
  
  virtual void  UnitSaveWeights_strm(std::ostream& strm, UnitState_cpp* ru, WtSaveFormat fmt = TEXT,
                                     PrjnState_cpp* prjn = NULL);
  // #EXT_wts #COMPRESS #CAT_File write weight values out in a simple ordered list of weights (optionally in binary fmt)
  virtual int   UnitLoadWeights_strm(std::istream& strm, UnitState_cpp* ru, WtSaveFormat fmt = TEXT,
                                     bool quiet = false, PrjnState_cpp* prjn = NULL);
  // #EXT_wts #COMPRESS #CAT_File read weight values in from a simple ordered list of weights (optionally in binary fmt) -- rval is taMisc::ReadTagStatus, TAG_END if successful

  virtual void  ConsSaveWeights_strm(std::ostream& strm, ConState_cpp* cg, UnitState_cpp* ru,
                                     WtSaveFormat fmt = TEXT);
  // #IGNORE write weight values out in a simple ordered list of weights (optionally in binary fmt)
  virtual int   ConsLoadWeights_strm(std::istream& strm, ConState_cpp* cg, UnitState_cpp* ru,
                                     WtSaveFormat fmt = TEXT, bool quiet = false);
  // #EXT_wts #COMPRESS #CAT_File read weight values in from a simple ordered list of weights (optionally in binary format) -- rval is taMisc::ReadTagStatus, TAG_END if successful -- the connections for both sides must already be allocated, but it can rearrange connections based on save unit indexes for random connectivity etc

  ////////////////////////////////////////////////////////////
  //            Basic Parsing Code -- reads into this LexBuf
  
  static String StateLexBuf;    // #NO_SAVE #HIDDEN buffer for reading state info from parsing routines
  
  static int    skip_white_noeol(std::istream& strm, bool peek = false);
  // #CAT_Parse don't skip end-of-line
  static int    read_till_eol(std::istream& strm, bool peek = false);
  // #CAT_Parse eol = end of line
  static int    skip_white(std::istream& strm, bool peek = false);
  // #CAT_Parse skip over all whitespace
  static int    read_till_rangle(std::istream& strm, bool peek = false);
  // #CAT_Parse rangle = >
  static ReadTagStatus read_tag(std::istream& strm, String& tag, String& val);
  // #CAT_Parse read an html-style tag from the file: <XXX ...> tag = XXX, val = ... (optional)

  static int    LoadWeights_StartTag(std::istream& strm, const String& tag,
                                     String& val, bool quiet);
  // #IGNORE read in a start tag -- makes sure it matches tag, returns TAG_GOT if got it
  static int    LoadWeights_EndTag(std::istream& strm, const String& trg_tag,
                                   String& cur_tag, int& stat, bool quiet);
  // #IGNORE read in an end tag -- makes sure it matches trg_tag, cur_tag, stat are current read_tag & status (if !END_TAG, will try to read end)
  static int    SkipWeights_strm(std::istream& strm, WtSaveFormat fmt = TEXT, bool quiet = false);
  // #EXT_wts #COMPRESS #CAT_File skip over weight values in from a simple ordered list of weights (optionally in binary fmt) -- rval is ReadTagStatus = END_TAG if successful
  static int    ConsSkipWeights_strm(std::istream& strm, WtSaveFormat fmt = TEXT,  bool quiet = false);
  // #IGNORE skip over saved weight values -- rval is ReadTagStatus, TAG_END if successful

  NetworkState_cpp() { Initialize_net_core(); Initialize_core(); }
};

#endif // NetworkState_cpp_h

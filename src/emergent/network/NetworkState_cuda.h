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

#ifndef NetworkState_cuda_h
#define NetworkState_cuda_h 1

// State classes define full implementation of code -- this is included in all other
// State classes so it cannot depend on any of them -- every other class uses
// NetworkState to access their State variables which are allocated here

// parent includes:
#include <State_cuda>

// member includes:

// this defines classes that are members of NetworkState -- as _cpp
#include <Network_mbrs>

// declare all other types mentioned but not required to include:

eTypeDef_Of(NetworkState_cuda);

class NetworkState_cuda  {
  // ##NO_TOKENS ##NO_UPDATE_AFTER #STEM_BASE ##CAT_Network plain C++ implementation of NetworkState 
public:

// this directly defines a bunch of shared vars with Network
#include <Network_core>
// and common state between C++ and _cuda
#include <NetworkState_core>

  //////////////////////////////////////////////////////////////////////
  //    CUDA versions of all major code -- Network.cpp just does switch
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
  virtual void  Init_Timers();
  // #EXPERT #CAT_Statistic initialize statistic variables on network

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
  // #CAT_Learning update weights for whole net: calls DMem_SumDWts before doing update if in dmem mode

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
  // #IGNORE build sender-based netinput temporary memory  -- called by BuildConState but can be selectively overridden in subclasses
  
  virtual void  Connect();
  // #IGNORE Connect this network according to projections on Layers -- must be done as part of Build to ensure proper sync
  virtual void  Connect_Alloc();
  // #IGNORE second pass of connecting -- allocate all the memory for all the connections -- managed by the Network and done by thread
  
  NetworkState_cuda() { Initialize_net_core(); Initialize_core(); }
};

#endif // NetworkState_cuda_h

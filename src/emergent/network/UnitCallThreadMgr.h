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

#ifndef UnitCallThreadMgr_h
#define UnitCallThreadMgr_h 1

// parent includes:
#include "network_def.h"
#include <taTask>
#include <taThreadMgr>

// member includes:
#include <NetworkRef>

// declare all other types mentioned but not required to include:
class UnitCallThreadMgr; //
class Unit; //

// this is the standard unit function call taking a network pointer arg
// and the thread number int value
// all threaded unit-level functions MUST use this call signature!
#ifdef __MAKETA__
typedef void* ThreadUnitCall;
#else
typedef taTaskMethCall2<Unit, void, Network*, int> ThreadUnitCall;
typedef void (Unit::*ThreadUnitMethod)(Network*, int);
#endif


TypeDef_Of(UnitCallTask);

class EMERGENT_API UnitCallTask : public taTask {
INHERITED(taTask)
public:
  NetworkRef    network;        // the network we're operating on
  int           uidx_st;        // unit list number to start on
  int           uidx_ed;        // unit number to end before
  int           uidx_inc;       // how much to increment counter by
  ThreadUnitCall* unit_call;    // method to call on the unit

  override void run();
  // runs specified chunks and then nibbles on remainder

  UnitCallThreadMgr* mgr() { return (UnitCallThreadMgr*)owner->GetOwner(); }

  TA_BASEFUNS_NOCOPY(UnitCallTask);
private:
  void  Initialize();
  void  Destroy();
};

TypeDef_Of(UnitCallThreadMgr);

class EMERGENT_API UnitCallThreadMgr : public taThreadMgr {
  // #INLINE thread manager for UnitCall tasks -- manages threads and tasks, and coordinates threads running the tasks
INHERITED(taThreadMgr)
public:
  float         alloc_pct;      // #MIN_0 #MAX_1 #DEF_0 #NO_SAVE NOTE: not saved -- initialized from user prefs.  proportion (0-1) of total to process by pre-allocating a set of computations to a given thread -- the remainder of the load is allocated dynamically through a nibbling mechanism, where each thread takes a nibble_chunk at a time until the job is done.  current experience is that this should be no greater than .2, unless the load is quite large, as there is a high degree of variability in thread start times, so the automatic load balancing of nibbling is important, and it has very little additional overhead.
  int           nibble_chunk;   // #MIN_1 #DEF_8 #NO_SAVE NOTE: not saved -- initialized from user prefs.  how many units does each thread grab to process while nibbling?  Too small a value results in increased contention and inefficiency, while too large a value results in poor load balancing across processors.
  float         compute_thr;    // #MIN_0 #MAX_1 #DEF_0.5 #NO_SAVE NOTE: not saved -- initialized from user prefs.  threshold value for amount of computation in a given function to actually deploy on threads, as opposed to just running it on main thread -- value is normalized (0-1) with 1 being the most computationally intensive task, and 0 being the least -- as with min_units, it may not be worth it to parallelize very lightweight computations.  See Thread_Params page on emergent wiki for relevant comparison values.
  int           min_units;      // #MIN_1 #DEF_3000 #NO_SAVE NOTE: not saved -- initialized from user prefs.  minimum number of units required to use threads at all -- for feedforward algorithms requiring layer-level syncing, this applies to each layer -- if less than this number, all will be computed on the main thread to avoid threading overhead which may be more than what is saved through parallelism, if there are only a small number of things to compute.
  bool          interleave;     // #DEF_true #EXPERT deploy threads in an interleaved fashion over units, which improves load balancing as neighboring units are likely to have similar compute demands, but it costs in cache coherency as the memory access per processor is more distributed -- only affects network level (non lay_sync) processes
  bool          ignore_lay_sync;// #DEF_false ignore need to sync at the layer level for feedforward algorithms that require this (e.g., backprop) -- results in faster but less accurate processing
  QAtomicInt    nibble_i;       // #IGNORE current nibble index -- atomic incremented by working threads to nibble away the rest..
  int           nibble_stop;    // #IGNORE nibble stopping value
  bool          using_threads;  // #READ_ONLY #NO_SAVE are we currently using threads for a computation or not -- also useful for just after a thread call to see if threads were used

  Network*      network()       { return (Network*)owner; }

  override void InitAll();      // initialize threads and tasks

  override void Run(ThreadUnitCall* unit_call, float comp_load,
                    bool backwards=false, bool layer_sync=false);
  // #IGNORE run given function on all units, with specified level of computational load (0-1), and flags controlling order of processing and syncing: backwards = go through units in reverse order, and layer_sync = sync processing at each layer (else at network level) -- needed for feedforward network topologies (unfortunately)

  void          RunThread0(ThreadUnitCall* unit_call, bool backwards=false);
  // #IGNORE run only on thread 0 (the main thread) -- calls method with arg thread_no = -1 -- order matters but layer_sync is irrelevant here
  void          RunThreads_FwdNetSync(ThreadUnitCall* unit_call);
  // #IGNORE run on all threads -- calls method with arg thread_no -- forward order and network-level sync
  void          RunThreads_BkwdNetSync(ThreadUnitCall* unit_call);
  // #IGNORE run on all threads -- calls method with arg thread_no -- backward order and network-level sync
  void          RunThreads_FwdLaySync(ThreadUnitCall* unit_call);
  // #IGNORE run on all threads -- calls method with arg thread_no -- forward order and layer-level sync
  void          RunThreads_BkwdLaySync(ThreadUnitCall* unit_call);
  // #IGNORE run on all threads -- calls method with arg thread_no -- backward order and layer-level sync

  TA_BASEFUNS_NOCOPY(UnitCallThreadMgr);
protected:
  void  UpdateAfterEdit_impl();

  int           n_threads_prev;         // #IGNORE number of threads set previously in net build -- for update diffs

private:
  void  Initialize();
  void  Destroy();
};

#ifdef __TA_COMPILE__
#include <Network>
#endif

#endif // UnitCallThreadMgr_h

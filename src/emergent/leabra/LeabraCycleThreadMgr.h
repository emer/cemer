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

#ifndef LeabraCycleThreadMgr_h
#define LeabraCycleThreadMgr_h 1

// declare all other types mentioned but not required to include:

// parent includes:
#include "network_def.h"
#include <taTask>
#include <taThreadMgr>

// member includes:
#include <NetworkRef>

// declare all other types mentioned but not required to include:
class LeabraCycleThreadMgr; //
class LeabraUnit; //

eTypeDef_Of(LeabraCycleTask);

class E_API LeabraCycleTask : public taTask {
INHERITED(taTask)
public:
  NetworkRef    network;        // the network we're operating on
  int           uidx_st;        // unit list number to start on
  int           uidx_ed;        // unit number to end before
  int           lay_st;         // layer leaf number to start on
  int           lay_ed;         // layer leaf number to end on

  TimeUsedHR	run_time; 	// total time for this thread during run() call
  float         avg_run_time;   // progressive average run time since rebalancing
  int           avg_run_time_n; // number of measures in the average

  TimeUsedHR	wait_time; 	// total time for this thread during run() call
  float         avg_wait_time;   // progressive average run time since rebalancing
  int           avg_wait_time_n; // number of measures in the average

  void  run() override;
  // runs full cycle

  void  SyncAtom(QAtomicInt& stage);
  // #IGNORE sync on given atomic step

  LeabraCycleThreadMgr* mgr() { return (LeabraCycleThreadMgr*)owner->GetOwner(); }

  TA_BASEFUNS_NOCOPY(LeabraCycleTask);
private:
  void  Initialize();
  void  Destroy();
};

eTypeDef_Of(LeabraCycleThreadMgr);

class E_API LeabraCycleThreadMgr : public taThreadMgr {
  // #INLINE thread manager for LeabraCycle tasks -- manages threads and tasks, and coordinates threads running the tasks
INHERITED(taThreadMgr)
public:
  int           min_units;      // #MIN_1 #DEF_3000 #NO_SAVE NOTE: not saved -- initialized from user prefs.  minimum number of units required to use threads at all -- for feedforward algorithms requiring layer-level syncing, this applies to each layer -- if less than this number, all will be computed on the main thread to avoid threading overhead which may be more than what is saved through parallelism, if there are only a small number of things to compute.
  bool          sync_steps;     // keep each step of computation within the cycle syncronized, using atomic ints 
  bool          using_threads;  // #READ_ONLY #NO_SAVE are we currently using threads for a computation or not -- also useful for just after a thread call to see if threads were used

  // the following track how many threads have reached each stage -- atomic incremented by working threads
  QAtomicInt    stage_net;       // #IGNORE 
  QAtomicInt    stage_net_post;  // #IGNORE 
  QAtomicInt    stage_exnet;     // #IGNORE 
  QAtomicInt    stage_net_int;   // #IGNORE 
  QAtomicInt    stage_net_stats; // #IGNORE 

  QAtomicInt    stage_inhib;     // #IGNORE 
  QAtomicInt    stage_applyinhib; // #IGNORE 
  QAtomicInt    stage_act;       // #IGNORE 
  QAtomicInt    stage_cyc_stats; // #IGNORE 

  QAtomicInt    stage_cyc_syndep; // #IGNORE 
  QAtomicInt    stage_sravg;       // #IGNORE 

  Network*      network()       { return (Network*)owner; }

  void InitAll() override;      // initialize threads and tasks


  bool  CanRun();
  // #IGNORE can we run in threaded mode?

  void  Run();
  // #IGNORE run one cycle of processing -- only call if CanRun returns true

  void  InitStages(); 
  // #IGNORE set all stage counters to 0

  TA_BASEFUNS_NOCOPY(LeabraCycleThreadMgr);
protected:
  void  UpdateAfterEdit_impl();

  int   n_threads_prev;         // #IGNORE number of threads set previously in net build -- for update diffs

private:
  void  Initialize();
  void  Destroy();
};

#ifdef __TA_COMPILE__
#include <Network>
#endif

#endif // LeabraCycleThreadMgr_h

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

taTypeDef_Of(RunWaitTime);

class TA_API RunWaitTime : public taNBase {
  // ##CAT_Program contains timers for run time and wait time
INHERITED(taNBase)
public:
  TimeUsedHR    run;            // amount of run time used
  TimeUsedHR    wait;           // amount of wait time used

  inline void  StartRun(bool reset_used = true)
  { run.StartTimer(reset_used); }
  // #CAT_TimeUsed start the run timer
  inline void  StartWait(bool reset_used = true)
  { wait.StartTimer(reset_used); }
  // #CAT_TimeUsed start the wait timer

  inline void  EndRun()
  { run.EndTimer(); }
  // #CAT_TimeUsed end the run timer
  inline void  EndWait()
  { wait.EndTimer(); }
  // #CAT_TimeUsed end the wait timer

  inline void  RunToWait()
  { EndRun(); StartWait(false); }
  // #CAT_TimeUsed transition from running to waiting -- no reset
  inline void  WaitToRun()
  { EndWait(); StartRun(false); }
  // #CAT_TimeUsed transition from waiting to running -- no reset

  inline void  ResetUsed()
  { run.ResetUsed(); wait.ResetUsed(); }
  // #CAT_TimeUsed reset time used information for both run and wait

  inline void IncrAvg()
  { run.IncrAvg(); wait.IncrAvg(); }
  // #CAT_TimeUsed increment the avg_used running average with the current s_used data -- for both run and wait timers

  inline void ResetAvg() 
  { run.ResetAvg(); wait.ResetAvg(); }
  // #CAT_TimeUsed reset the running averages

  String        ReportAvg(float rescale = 1.0f);
  // return string with run: <avg>, wait: <avg> values -- optional rescaling factor just multiplies averages, to get a value in a different set of units

  TA_SIMPLE_BASEFUNS(RunWaitTime);
private:
  void  Initialize()    { }
  void  Destroy()       { };
};

eTypeDef_Of(LeabraCycleTask);

class E_API LeabraCycleTask : public taTask {
INHERITED(taTask)
public:
  NetworkRef    network;        // #NO_SAVE the network we're operating on
  int           uidx_st;        // #NO_SAVE unit list number to start on
  int           uidx_ed;        // #NO_SAVE unit number to end before
  int           lay_st;         // #NO_SAVE layer leaf number to start on
  int           lay_ed;         // #NO_SAVE layer leaf number to end on

  RunWaitTime   send_netin_time;  // #NO_SAVE connection-level send netin computation
  RunWaitTime   netin_integ_time; // #NO_SAVE unit-level netin integration computation
  RunWaitTime   inhib_time;       // #NO_SAVE layer-level inhibition computation
  RunWaitTime   act_time;         // #NO_SAVE unit-level act computation
  RunWaitTime   cycstats_time;    // #NO_SAVE layer-level cyclestats computation

  // optional ones
  RunWaitTime   sravg_cons_time; // #NO_SAVE connection-level sravg
  RunWaitTime   cycsyndep_time;  // #NO_SAVE connection-level syn dep

  void  run() override;
  // runs full cycle

  void          StartCycle();
  // reset all the timers

  inline void   StartStep(RunWaitTime& time)
  { time.StartRun(false); }

  void          EndStep(QAtomicInt& stage, RunWaitTime& time, int cyc);
  // #IGNORE end a given step, including sync on given atomic step

  inline void   EndTime(RunWaitTime& time) 
  { time.EndRun(); }
  // use this one if there isn't a sync possibility -- just stop the timer

  void          EndCycle();
  // end all the timers

  String   ThreadReport();
  // return a report on thread stats, etc

  LeabraCycleThreadMgr* mgr() { return (LeabraCycleThreadMgr*)owner->GetOwner(); }

  TA_SIMPLE_BASEFUNS(LeabraCycleTask);
private:
  void  Initialize();
  void  Destroy();
};

eTypeDef_Of(LeabraCycleThreadMgr);

class E_API LeabraCycleThreadMgr : public taThreadMgr {
  // #INLINE thread manager for LeabraCycle tasks -- manages threads and tasks, and coordinates threads running the tasks
INHERITED(taThreadMgr)
public:
  int           n_threads_act;  // #READ_ONLY #SHOW #NO_SAVE actual number of threads deployed, based on parameters
  int           n_cycles;       // #MIN_1 how many cycles to run at a time -- more efficient to run multiple cycles per Run
  float         input_cost;     // relative computational cost of a hard-clamped INPUT layer compared to a full hidden layer -- much reduced due to many steps skipped for these layers -- optimize this to get initial allocation closer to balanced
  float         target_cost;     // relative computational cost of a hard-clamped TARGET layer compared to a full hidden layer -- reduced due to many steps skipped for these layers -- optimize this to get initial allocation closer to balanced
  int           min_units;      // #MIN_1 #DEF_3000 #NO_SAVE NOTE: not saved -- initialized from user prefs.  minimum number of units required to use threads at all -- for feedforward algorithms requiring layer-level syncing, this applies to each layer -- if less than this number, all will be computed on the main thread to avoid threading overhead which may be more than what is saved through parallelism, if there are only a small number of things to compute.
  bool          sync_steps;     // keep each step of computation within the cycle syncronized, using atomic ints -- ensures 100% accurate computation -- turning this off is untested and likely to cause problems at this point
  bool          using_threads;  // #READ_ONLY #NO_SAVE are we currently using threads for a computation or not -- also useful for just after a thread call to see if threads were used

  // the following track how many threads have reached each stage -- atomic incremented by working threads
  QAtomicInt    stage_net;       // #IGNORE 
  QAtomicInt    stage_net_int;   // #IGNORE 

  QAtomicInt    stage_inhib;     // #IGNORE 

  QAtomicInt    stage_act;       // #IGNORE 
  QAtomicInt    stage_cyc_stats; // #IGNORE 

  Network*      network()       { return (Network*)owner; }

  void InitAll() override;      // initialize threads and tasks


  bool  CanRun();
  // #IGNORE can we run in threaded mode?

  void  Run();
  // #IGNORE run one cycle of processing -- only call if CanRun returns true

  void  InitStages(); 
  // #IGNORE set all stage counters to 0

  String   ThreadReport();
  // return a report on thread stats, etc

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

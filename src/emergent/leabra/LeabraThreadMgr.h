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

#ifndef LeabraThreadMgr_h
#define LeabraThreadMgr_h 1

// declare all other types mentioned but not required to include:

// parent includes:
#include "network_def.h"
#include <taTask>
#include <taThreadMgr>

// member includes:
#include <NetworkRef>

// declare all other types mentioned but not required to include:
class LeabraThreadMgr; //
class DataTable; //
class LeabraUnit; //
class LeabraNetwork; //
class LeabraLayer; //

// this is the standard unit function call taking a network pointer arg
// and the thread number int value
// all threaded unit-level functions MUST use this call signature!
#ifdef __MAKETA__
typedef void* LeabraThreadUnitCall;
#else
typedef taTaskMethCall2<LeabraUnit, void, LeabraNetwork*, int> LeabraThreadUnitCall;
typedef void (LeabraUnit::*LeabraThreadUnitMethod)(LeabraNetwork*, int);
#endif

#ifdef __MAKETA__
typedef void* LeabraThreadLayerCall;
#else
typedef taTaskMethCall2<LeabraLayer, void, LeabraNetwork*, int> LeabraThreadLayerCall;
typedef void (LeabraLayer::*LeabraThreadLayerMethod)(LeabraNetwork*, int);
#endif


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


taTypeDef_Of(RunWaitTime_List);

class TA_API RunWaitTime_List: public taList<RunWaitTime> {
  // ##CAT_Program a list of run-wait-timers
INHERITED(taList<RunWaitTime>)
public:

  void  ResetUsed(int st_idx, int n);
  // #CAT_TimeUsed reset time used information for both run and wait for given range of timers

  TA_BASEFUNS_NOCOPY(RunWaitTime_List);
private:
  void          Initialize() { SetBaseType(&TA_RunWaitTime); }
  void          Destroy() { }
};



eTypeDef_Of(LeabraTask);

class E_API LeabraTask : public taTask {
INHERITED(taTask)
public:
  NetworkRef    network;        // #NO_SAVE the network we're operating on

  RunWaitTime   done_time;        // #NO_SAVE just waiting for everyone to be fully done

  RunWaitTime   send_netin_time;  // #NO_SAVE connection-level send netin computation
  RunWaitTime   netin_integ_time; // #NO_SAVE unit-level netin integration computation
  RunWaitTime   inhib_time;       // #NO_SAVE layer-level inhibition computation
  RunWaitTime   act_time;         // #NO_SAVE unit-level act computation
  RunWaitTime   cycstats_time;    // #NO_SAVE layer-level cyclestats computation

  RunWaitTime   ti_netin_time;     // #NO_SAVE TI netins

  RunWaitTime   dwt_time;          // #NO_SAVE weight change
  RunWaitTime   dwt_norm_time;     // #NO_SAVE dwt norm
  RunWaitTime   wt_time;           // #NO_SAVE update weights

  inline void   StartTime(RunWaitTime& time)
  { time.StartRun(false); }

  void          EndStep(QAtomicInt& stage, RunWaitTime& time, int cyc);
  // #IGNORE end a given step, including sync on given atomic step

  inline void   EndTime(RunWaitTime& time)
  { time.EndRun(); }
  // use this one if there isn't a sync possibility -- just stop the timer

  void          RunUnits(LeabraThreadUnitCall& unit_call, QAtomicInt& lpidx);
  // #IGNORE run units on given method, using given looping index
  void          RunUnitsStep(LeabraThreadUnitCall& unit_call, QAtomicInt& lpidx,
                             QAtomicInt& stage, RunWaitTime& time, int cyc,
                             bool reset_used = true);
  // #IGNORE run units on given method with StartTime, EndStep
  void          RunUnitsTime(LeabraThreadUnitCall& unit_call, QAtomicInt& lpidx,
                             RunWaitTime& time, bool reset_used = true);
  // #IGNORE run units on given method with StartTime, EndTime

  void          RunLayers(LeabraThreadLayerCall& lay_call, QAtomicInt& lpidx);
  // #IGNORE run layers on given method, using given looping index 
  void          RunLayersStep(LeabraThreadLayerCall& unit_call, QAtomicInt& lpidx,
                              QAtomicInt& stage, RunWaitTime& time, int cyc,
                              bool reset_used = true);
  // #IGNORE run layers on given method with StartTime, EndStep

  void          Cycle_Run();    // run n cycles of basic Leabra cycle update loop
  void          TI_Send_Netins();
  void          Compute_dWt();  // run compute_dwt
  void          Compute_Weights(); // run compute_weights

  void  run() override;
  // run loop

  void          ThreadReport(DataTable& dt);
  // report data to data table

  LeabraThreadMgr* mgr() { return (LeabraThreadMgr*)owner->GetOwner(); }

  TA_SIMPLE_BASEFUNS(LeabraTask);
private:
  void  Initialize();
  void  Destroy();
};

eTypeDef_Of(LeabraThreadMgr);

class E_API LeabraThreadMgr : public taThreadMgr {
  // #INLINE thread manager for Leabra tasks -- manages threads and tasks, and coordinates threads running the tasks
INHERITED(taThreadMgr)
public:
  enum RunStates {              // defines run_state states
    NOT_RUNNING,                // no current run call is active for the threads -- need to get them started up again -- this can also be used as a stop signal
    ACTIVE_WAIT,                // threads are active (busy tight loop) waiting for a change in run state
    RUN_CYCLE,                  // Cycle_Run()
    RUN_TI_NETS,                // TI_Send_Netins: Deep5bNetin(), CtxtNetin()
    RUN_DWT,                    // Compute_dWt()
    RUN_WT,                     // Compute_Weights()
  };

  int           n_threads_act;  // #READ_ONLY #SHOW #NO_SAVE actual number of threads deployed, based on parameters
  int           n_cycles;       // #MIN_1 #DEF_10 how many cycles to run at a time -- more efficient to run multiple cycles per Run
  int           unit_chunks;    // #MIN_1 #DEF_32 how many units to bite off for each thread off of the list at a time
  bool          timers_on;      // Accumulate timing information for each step of processing -- including at the cycle level for each different type of operation -- for debugging / optimizing threading
  bool          using_threads;  // #READ_ONLY #NO_SAVE are we currently using threads for a computation or not -- also useful for just after a thread call to see if threads were used

  // the following track how many threads have reached each stage -- atomic incremented by working threads
  QAtomicInt    run_state;       // #IGNORE a fast lock mechanism that the threads wait on and 
  QAtomicInt    done_run;        // #IGNORE done with current run -- make sure!

  QAtomicInt    stage_net;       // #IGNORE 
  QAtomicInt    stage_net_int;   // #IGNORE 

  QAtomicInt    stage_inhib;     // #IGNORE 

  QAtomicInt    stage_act;       // #IGNORE 
  QAtomicInt    stage_sr_cons;   // #IGNORE 
  QAtomicInt    stage_cyc_stats; // #IGNORE 
  QAtomicInt    stage_syndep; // #IGNORE 

  QAtomicInt    stage_deep5b;    // #IGNORE 
  QAtomicInt    stage_deep5b_p;  // #IGNORE post
  QAtomicInt    stage_ctxt;      // #IGNORE 
  QAtomicInt    stage_ctxt_p;    // #IGNORE post

  QAtomicInt    stage_dwt;      // #IGNORE 
  QAtomicInt    stage_dwt_norm; // #IGNORE 
  QAtomicInt    stage_wt;       // #IGNORE 

  QAtomicInt    loop_idx0;      // #IGNORE index for looping to deploy threads, 1st one
  QAtomicInt    loop_idx1;      // #IGNORE index for looping to deploy threads, 2nd one
  QAtomicInt    loop_idx2;      // #IGNORE index for looping to deploy threads, 3rd one..

  Network*      network()       { return (Network*)owner; }

  void InitAll() override;      // initialize threads and tasks


  bool  CanRun();
  // #IGNORE can we run in threaded mode?

  void  Run(RunStates run_typ);
  // #IGNORE run given type of computation

  void  InitStages(); 
  // #IGNORE set all stage counters to 0

  void   ThreadReport(DataTable* table);
  // run a report on thread stats, to data table (which is fully reset in the process)

  TA_BASEFUNS_NOCOPY(LeabraThreadMgr);
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

#endif // LeabraThreadMgr_h

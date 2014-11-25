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

#ifndef NetworkThreadMgr_h
#define NetworkThreadMgr_h 1

// parent includes:
#include "network_def.h"
#include <taTask>
#include <taThreadMgr>

// member includes:
#include <NetworkRef>

// declare all other types mentioned but not required to include:
class NetworkThreadMgr; //

// this is the standard network function call taking the thread number int value
// all threaded unit-level functions MUST use this call signature!
#ifdef __MAKETA__
typedef void* NetworkThreadCall;
#else
typedef taTaskMethCall1<Network, void, int> NetworkThreadCall;
typedef void (Network::*NetworkThreadMethod)(int);
#endif

#define NET_THREAD_CALL(meth) { NetworkThreadCall meth_call((NetworkThreadMethod)(&meth));\
  threads.Run(meth_call); }


eTypeDef_Of(NetworkThreadTask);

class E_API NetworkThreadTask : public taTask {
INHERITED(taTask)
public:
  NetworkRef            network;   // the network we're operating on
  NetworkThreadCall     meth_call; // #IGNORE method to call on the network
  TimeUsedHR            wait_time; // amount of time spent in spin-lock wait

  void run() override;

  void SyncSpin(int usec_wait = 0);
  // synchronize all the threads using atomic int counters and spin waiting (active looping) -- if usec_wait > 0, then sleep for given number of micro seconds during each wait loop

  NetworkThreadMgr* mgr() { return (NetworkThreadMgr*)owner->GetOwner(); }

  TA_BASEFUNS_NOCOPY(NetworkThreadTask);
private:
  void  Initialize();
  void  Destroy();
};

eTypeDef_Of(NetworkThreadMgr);

class E_API NetworkThreadMgr : public taThreadMgr {
  // #INLINE thread manager for network methods -- manages threads and tasks, and coordinates threads running the tasks
INHERITED(taThreadMgr)
public:
  QAtomicInt    sync_ctr;       // #IGNORE for thread sync -- this is the counter that keeps incrementing 
  QAtomicInt    sync_step;      // #IGNORE for thread sync -- this determines the target count -- every call through Sync* increments this by 1

  Network*      network()       { return (Network*)owner; }

  void InitAll() override;      // initialize threads and tasks

  void Run(NetworkThreadCall& meth_call);
  // #IGNORE run given function on the Network, passing thread number as arg

  void SyncSpin(int thread_no, int usec_wait = 0);
  // #IGNORE synchronize all the threads using atomic int counters with an active spin loop -- optional sleep for given number of micro seconds if usec_wait > 0 during each wait loop -- each thread must call this with its own thread_no at given point in code, to make sure all threads are synchronized -- MUST ONLY BE CALLED from methods that were invoked through Run() function in first place!!

  TA_BASEFUNS_NOCOPY(NetworkThreadMgr);
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

#endif // NetworkThreadMgr_h

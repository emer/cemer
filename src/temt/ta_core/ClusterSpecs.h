// Copyright, 1995-2013, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of The Emergent Toolkit
//
//   This library is free software; you can redistribute it and/or
//   modify it under the terms of the GNU Lesser General Public
//   License as published by the Free Software Foundation; either
//   version 2.1 of the License, or (at your option) any later version.
//
//   This library is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//   Lesser General Public License for more details.

#ifndef ClusterSpecs_h
#define ClusterSpecs_h 1

// parent includes:
#include "ta_def.h"
#include <taString>
#include <taPlainArray>

// member includes:

// declare all other types mentioned but not required to include:

taTypeDef_Of(ClusterSpecs);

class TA_API ClusterSpecs  {
  // ##INLINE specifications for compute cluster resources -- for submitting compute jobs through the ClusterRun system -- generally a good idea to have one per queue on a given cluster, and just store all the specs here.
public:
  enum ClusterType {
    CLUSTER,                    // standard compute cluster with a centralized job scheduler (e.g., Maui/Torque/PBS or SGE)
    CLOUD,                      // a cloud computing system (e.g., Amazon, etc)
  };

  String        name;           // name of the cluster
  ClusterType   type;           // type of cluster -- may affect how jobs are submitted
  int           procs_per_node; // how many processors (CPUs) per node are there
  int           nodes;          // how many nodes total are there
  int           procs;          // #READ_ONLY #SHOW total number of processors available on system -- computed as nodes * procs_per_node
  int           gpus;         // number of GPU (graphical processing units) per node (0 if none)
  bool          mpi;            // supports use of MPI (message passing interface) distributed memory jobs
  int           min_procs;       // minimum number of processors that can be used for this cluster / queue -- some queues require a minimum number of processors, so this enables checking on that constraint
  int           max_procs;       // guideline for number of processors maximum to use on this cluster -- you will be prompted if you request more than this number -- use 0 for no constraint
  int           max_jobs;       // guideline for number of different jobs that can be run at the same time -- you will be prompted if you request more than this number -- use 0 for no constraint
  int           max_time;       // maximum time in hours that can be requested on this cluster for any given job
  int           max_ram;        // maximum RAM in Gb avail per node on this cluster -- you will not be able to request more than this amount of ram on this cluster -- use 0 for no constraint (strongly recommend setting this!)
  bool          by_node;        // if true, jobs are allocated by whole nodes at a time, instead of just by individual processor -- cluster run script must support managed parallel batch jobs to fill up a node for most typical use cases (otherwise high proc count mpi jobs etc)

  void   UpdateProcs()
  { procs_per_node = MAX(1, procs_per_node); nodes = MAX(1, nodes); 
    procs = nodes * procs_per_node; }

  void   SetFmStr(const String& val);
  String GetStr() const;

  // need these operators for array comparsion
  bool operator>(const ClusterSpecs& cp) { return procs > cp.procs; }
  bool operator<(const ClusterSpecs& cp) { return procs < cp.procs; }
  bool operator==(const ClusterSpecs& cp) { return procs == cp.procs; }

  ClusterSpecs()
  { type = CLUSTER; procs_per_node = 1; nodes = 1; procs = 1; gpus = 0; mpi = true;
    min_procs = 0; max_procs = 0; max_jobs = 0; max_time = 0; max_ram = 0;
    by_node = false; }
  ClusterSpecs(const ClusterSpecs& cp)
  { name = cp.name; type = cp.type; procs_per_node = cp.procs_per_node;
    nodes = cp.nodes; procs = cp.procs; gpus = cp.gpus; mpi = cp.mpi; 
    min_procs = cp.min_procs; max_procs = cp.max_procs; max_jobs = cp.max_jobs;
    max_time = cp.max_time; max_ram = cp.max_ram; by_node = cp.by_node; }
  ~ClusterSpecs()   { };
};

taTypeDef_Of(NameVar_PArray);

class TA_API ClusterSpecs_PArray : public taPlainArray<ClusterSpecs> {
  // #NO_TOKENS a plain-array of cluster specs items
INHERITED(taPlainArray<ClusterSpecs>)
public:
  int   FindName(const String& nm, int start=0) const;
  // find by name  (start < 0 = from end)
  int   FindNameContains(const String& nm, int start=0) const;
  // find by name containing nm (start < 0 = from end)

  void  operator=(const ClusterSpecs_PArray& cp)     { Copy_Duplicate(cp); }
  ClusterSpecs_PArray()                              { };
  ClusterSpecs_PArray(const ClusterSpecs_PArray& cp)      { Copy_Duplicate(cp); }
protected:
  int           El_Compare_(const void* a, const void* b) const
  { int rval=-1; if(((ClusterSpecs*)a)->procs > ((ClusterSpecs*)b)->procs) rval=1; else if(((ClusterSpecs*)a)->procs == ((ClusterSpecs*)b)->procs) rval=0; return rval; }
  bool          El_Equal_(const void* a, const void* b) const
  { return (((ClusterSpecs*)a)->name == ((ClusterSpecs*)b)->name); }
  String        El_GetStr_(const void* it) const { return ((ClusterSpecs*)it)->GetStr(); }
  void          El_SetFmStr_(void* it, const String& val) { ((ClusterSpecs*)it)->SetFmStr(val); }
};

#endif // ClusterSpecs_h

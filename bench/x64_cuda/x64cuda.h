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

#ifndef x64vec_h
#define x64vec_h 1

// define this to expand the connection object to full size..
// #define STABLE_WEIGHTS 1

class LeabraSendCons;
class LeabraUnit;
class LeabraSendCon;
class LeabraCon;

/////////////////////////////////////////////////////
//      Basic Utilities -- Timing etc

# include <sys/time.h>
# include <sys/times.h>
# include <unistd.h>
# include <stdio.h>
# include <stdlib.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cuda_runtime.h>
#include <curand.h>
#define cudaSafeCall(err)  __cudaSafeCall(err,__FILE__,__LINE__)
inline void __cudaSafeCall(cudaError err,
                           const char *file, const int line){
  if(cudaSuccess != err) {
    printf("%s(%i) : cudaSafeCall() Runtime API error : %s.\n",
           file, line, cudaGetErrorString(err) );
    exit(-1);
  }
}
// computes a-b
#define ONE_MILLION 1000000

timeval timeval_diff(timeval a, timeval b) {
  a.tv_sec -= b.tv_sec;
  a.tv_usec -= b.tv_usec;
  if (a.tv_usec < 0) {
    a.tv_sec-- ;
    a.tv_usec += ONE_MILLION;
  }
  return a;
}

class TimeUsedHR {
  // high-res timing -- linux/mac platform only
public:
  double        s_used;         // #SHOW #GUI_READ_ONLY total number of seconds used
  int           n_used;         // #SHOW #GUI_READ_ONLY number of individual times the timer has been used without resetting accumulation

  timeval	start;
  timeval	end;
  timeval	used;

  void		GetStartTime()
  { gettimeofday(&start, NULL); }
  void		GetEndTime()
  {  gettimeofday(&end, NULL);
    used = timeval_diff(end, start); }
  double	GetTotSecs() { 
    double rval = used.tv_usec / 1000000.0;
    if (used.tv_sec) rval += used.tv_sec;
    return rval;
  }

  void StartTimer(bool reset_used) {
    if (reset_used) ResetUsed();
    GetStartTime();
  }

  void EndTimer() {
     GetEndTime();
     s_used += GetTotSecs();
     n_used++;
  }

  void ResetUsed() {
    s_used = 0.0;
    n_used = 0;
  }
};


float   rand_float();
// get a random floating point number


class LeabraNetwork {
  // network -- only has flat units -- no layers or any other structure
public:
  int           n_units;        // number of units built in network

  LeabraUnit**  units_flat;     // flat list of units
  float*        send_netin_tmp; // temp storage of netinput values

  inline LeabraUnit*  UnFmIdx(int idx) const { return units_flat[idx]; }
  // #CAT_Structure get the unit from its flat_idx value

  virtual void  BuildUnits(int n_un);
  // build a network -- allocate units
  virtual void  ConnectUnits(int n_per_un, int n_layers=5, int n_prjns=2);
  // connect, with given number of (simulated) layers and sending connections per unit, and given number of projections per unit

  int maxConnections;
  int totalSize;
  cudaError_t err;
  int * column_offsets_gpu;
  int * row_indices_gpu;
  float * wts_gpu;

  curandGenerator_t gen;

  float * send_netin_gpu; // temp storage of netinput values
  float * send_netin_cpu; // temp storage of netinput values

  float * su_act_delta_gpu;
  float * su_act_delta_cpu;

  int * si_gpu;
  int * si_cpu; // temp storage of netinput values
  
  cudaStream_t stream1;
  cudaStream_t stream2;

  cudaEvent_t event1;
  cudaEvent_t event2;

  float totalTime;
  
  virtual void BuildCudaData(float pct_delta);
  virtual void FreeCudaData();

  

  virtual void  FreeUnits();
  // free any allocated units

  virtual void  Send_Netin(float pct_delta);

  virtual void  Send_NetinCuda(float pct_delta);
  // run a simulated netinput sending routine, with given percentage actually sending

  LeabraNetwork();
  virtual ~LeabraNetwork();
};

class LeabraUnit {
  // leabra unit -- has all the vars and takes the same space -- 768 bytes -- as actual one
public:
  int           overhead[4];
  // pad this out to match overall size
  int           flags;
  // #CAT_Structure flags controlling various aspects of unit state and function
  int          ext_flag;
  // #GUI_READ_ONLY #SHOW #CAT_Activation tells what kind of external input unit received -- this is normally set by the ApplyInputData function -- it is not to be manipulated directly
  float         targ;
  // #VIEW_HOT #CAT_Activation target value: drives learning to produce this activation value
  float         ext;
  // #VIEW_HOT #CAT_Activation external input: drives activation of unit from outside influences (e.g., sensory input)
  float         act;
  // #DMEM_SHARE_SET_2 #VIEW_HOT #CAT_Activation activation value -- what the unit communicates to others
  float         net;
  // #DMEM_SHARE_SET_1 #VIEW_HOT #CAT_Activation net input value -- what the unit receives from others (typically sum of sending activations times the weights)
  float         wt_prjn;
  // #NO_SAVE #CAT_Statistic weight projection value -- computed by Network::ProjectUnitWeights (triggered in GUI by setting wt prjn variable in netview control panel to point to a layer instead of NULL) -- represents weight values projected through any intervening layers from source unit (selected unit in netview or passed to ProjectUnitWeights function directly)
  float         snap;
  // #NO_SAVE #CAT_Statistic current snapshot value, as computed by the Snapshot function -- this can be displayed as a border around the units in the netview
  float         tmp_calc1;
  // #NO_SAVE #READ_ONLY #HIDDEN #CAT_Statistic temporary calculation variable (used for computing wt_prjn and prossibly other things)

  //  RecvCons_List recv;
  int              recv[16];
  // #CAT_Structure Receiving connections (128 bytes), one set of connections for each projection (collection of connections) received from other units
  int              sendpad[15];
  LeabraSendCons*  send;
  // #CAT_Structure Sending connections, one set of connections for each projection (collection of connections) sent from other units -- only has one!
  int              bias[13];
  // #CAT_Structure bias weight connection (104 bytes) type determined in unit spec) -- provides intrinsic activation in absence of other inputs

  int           n_recv_cons;
  // #CAT_Structure #DMEM_SHARE_SET_0 #READ_ONLY #EXPERT total number of receiving connections
  int          pos[3];
  // #CAT_Structure display position in space relative to owning group or layer -- in structural 3D coordinates
  void*         voxels;
  // #CAT_Structure #READ_ONLY #HIDDEN #NO_COPY #NO_SAVE #NO_VIEW Voxels assigned to this unit in a brain view.
  int           idx;
  // #CAT_Structure #READ_ONLY #HIDDEN #NO_COPY #NO_SAVE index of this unit within containing unit group
  int           flat_idx;
  // #CAT_Structure #READ_ONLY #HIDDEN #NO_COPY #NO_SAVE index of this unit in a flat array of units (used by parallel threading) -- 0 is special null case -- real idx's start at 1

  // here starts the unique variables for LeabraUnit
  
  float		act_eq;		// #VIEW_HOT #CAT_Activation rate-code equivalent activity value (time-averaged spikes or just act)
  float		act_nd;		// #CAT_Activation non-depressed rate-code equivalent activity value (time-averaged spikes or just act) -- used for final phase-based variables used in learning and stats
  float         act_lrn;        // #VIEW_HOT #CAT_Activation activation for learning -- potentially higher contrast than that for processing -- only contributes to XCAL learning, not CHL
  float		act_avg;	// #CAT_Activation average activation (of final plus phase activation state) over long time intervals (dt = act.avg_dt)
  float		act_m;		// #VIEW_HOT #CAT_Activation minus_phase activation (act_nd), set after settling, used for learning and performance stats 
  float		act_p;		// #VIEW_HOT #CAT_Activation plus_phase activation (act_nd), set after settling, used for learning and performance stats
  float		act_dif;	// #VIEW_HOT #CAT_Activation difference between plus and minus phase acts, gives unit err contribution
  float		act_m2;		// #CAT_Activation second minus_phase (e.g., nothing phase) activation (act_nd), set after settling, used for learning and performance stats
  float		act_mid;	// #CAT_Activation mid minus_phase -- roughly half-way through minus phase -- used in hippocampal ThetaPhase (for auto-encoder CA1 training) 
  float		act_dif2;	// #CAT_Activation difference between second set of phases, where relevant (e.g., act_p - act_m2 for MINUS_PLUS_NOTHING)
  float		da;		// #NO_SAVE #CAT_Activation delta activation: change in act from one cycle to next, used to stop settling
  float		avg_ss;		// #CAT_Activation super-short time-scale activation average -- provides the lowest-level time integration, important specifically for spiking networks using the XCAL_C algorithm -- otherwise ss_dt = 1 and this is just the current activation
  float		avg_s;		// #CAT_Activation short time-scale activation average -- tracks the most recent activation states, and represents the plus phase for learning in XCAL algorithms
  float		avg_m;		// #CAT_Activation medium time-scale activation average -- integrates over entire trial of activation, and represents the minus phase for learning in XCAL algorithms
  float		avg_l;		// #CAT_Activation long time-scale average of medium-time scale (trial level) activation, used for the BCM-style floating threshold in XCAL
  float         act_ctxt;       // #VIEW_HOT #CAT_Activation leabra TI context activation value -- computed from LeabraTICtxtConspec connection when network ti_mode is on
  float         net_ctxt;       // #CAT_Activation leabra TI context netinput value for computing act_ctxt -- computed from LeabraTICtxtConspec connection when network ti_mode is on
  float         p_act_p;        // #CAT_Activation prior trial act_p value -- needed for leabra TI context weight learning in the LeabraTICtxtConspec connection
  float		davg;		// #CAT_Activation delta average activation -- computed from changes in the short time-scale activation average (avg_s) -- used for detecting jolts or transitions in the network, to drive learning
  int   	vcb[5];		// #CAT_Activation (40 bytes) voltage-gated channel basis variables
  int           gc[4];		// #DMEM_SHARE_SET_1 #NO_SAVE #CAT_Activation current unit channel conductances
  float		I_net;		// #NO_SAVE #CAT_Activation net current produced by all channels
  float		v_m;		// #NO_SAVE #CAT_Activation membrane potential
  float		vm_dend;	// #NO_SAVE #CAT_Activation dendritic membrane potential -- reflects back-propagated spike values in spiking mode -- these are not subject to immediate AHP and thus decay exponentially, and are used for learning
  float		adapt;		// #NO_SAVE #CAT_Activation adaptation factor -- driven by both sub-threshold membrane potential and spiking activity -- subtracts directly from the membrane potential on every time step
  float		noise;		// #NO_SAVE #CAT_Activation noise value added to unit (noise_type on unit spec determines where it is added) -- this can be used in learning in some cases
  float 	dav;		// #VIEW_HOT #CAT_Activation dopamine value (da is delta activation) which modulates activations (e.g., via accom and hyst currents) to then drive learning

  bool		in_subgp;	// #READ_ONLY #NO_SAVE #CAT_Structure determine if unit is in a subgroup
  float		net_scale;	// #NO_VIEW #NO_SAVE #EXPERT #CAT_Activation total netinput scaling basis
  float		bias_scale;	// #NO_VIEW #NO_SAVE #EXPERT #CAT_Activation bias weight scaling factor
  float		prv_net;	// #NO_VIEW #NO_SAVE #EXPERT #CAT_Activation previous net input (for time averaging)
  float		prv_g_i;	// #NO_VIEW #NO_SAVE #EXPERT #CAT_Activation previous inhibitory conductance value (for time averaging)

  float		act_sent;	// #NO_VIEW #NO_SAVE #EXPERT #CAT_Activation last activation value sent (only send when diff is over threshold)
  float		net_raw;	// #NO_VIEW #NO_SAVE #EXPERT #CAT_Activation raw net input received from sending units (send delta delta's are added to this value)
  float		net_delta;	// #NO_VIEW #NO_SAVE #EXPERT #CAT_Activation delta net input received from sending units -- only used for non-threaded case
  float		g_i_raw;	// #NO_VIEW #NO_SAVE #EXPERT #CAT_Activation raw inhib net input received from sending units (increments the deltas in send_delta)
  float		g_i_delta;	// #NO_VIEW #NO_SAVE #EXPERT #CAT_Activation delta inhibitory net input received from sending units -- only used for non-threaded case

  float		i_thr;		// #NO_SAVE #CAT_Activation inhibitory threshold value for computing kWTA
  float		spk_amp;	// #CAT_Activation amplitude/probability of spiking output (for synaptic depression function if unit spec depress.on is on)
  float		misc_1;		// #NO_SAVE #CAT_Activation miscellaneous variable for other algorithms that need it
  int		spk_t;		// #NO_SAVE #CAT_Activation time in ct_cycle units when spiking last occurred (-1 for not yet)
  void* act_buf;	// #NO_VIEW #NO_SAVE #CAT_Activation buffer of activation states for synaptic delay computation
  void* spike_e_buf;	// #NO_VIEW #NO_SAVE #CAT_Activation buffer of excitatory net input from spikes for synaptic integration over discrete spikes
  void* spike_i_buf; // #NO_VIEW #NO_SAVE #CAT_Activation buffer of inhibitory net input from spikes for synaptic integration over discrete spikes

  int           extra_padding[67]; // not sure what this is!!

  LeabraUnit();
  virtual ~LeabraUnit();
};


class LeabraCon {
  // Leabra connection object
public:
  float         wt;             // #VIEW_HOT weight of connection
  float         dwt;            // #VIEW #NO_SAVE resulting net weight change
  float		pdw;		// #VIEW_HOT #NO_SAVE previous delta-weight change -- useful for viewing because current weight change (dwt) is typically reset to 0 when views are updated
#ifdef STABLE_WEIGHTS
  float         lwt;            // #NO_SAVE learning weight value -- adapts according to learning rules every trial in a dynamic online manner
  float         swt;            // #NO_SAVE stable (protein-synthesis and potentially sleep dependent) weight value -- updated from lwt value periodically (e.g., at the end of an epoch) by Compute_StableWeight function
#endif
  
  LeabraCon() {
    wt = dwt = pdw = 0.0f;
#ifdef STABLE_WEIGHTS
    pdw = 0.0f; lwt = swt = 0.0f;
#endif
  }
};

class LeabraSendCons  {
  // Leabra sending connections -- owns connection objects, has int pointers to master unit list
public:
  int           size;           // #CAT_Structure #READ_ONLY #NO_SAVE #SHOW number of connections currently active
  int           alloc_size;     // #CAT_Structure #READ_ONLY #NO_SAVE #SHOW allocated size -- no more than this number of connections may be created -- it is a hard limit set by the alloc function
  int           other_idx;      // #CAT_Structure #READ_ONLY #SHOW index into other direction's list of cons objects (i.e., send_idx for RecvCons and recv_idx for SendCons)

protected:
  int           con_size;       // size of the connection object -- set if we own cons and built them
  char*         cons_own;       // if we own the cons, this is their physical memory: alloc_size * con_size
  int*          unit_idxs;      // list of unit flat_idx indexes on the other side of the connection, in index association with the connections

public:

  /////////////////////////////
  //    Accessors

  inline LeabraCon*     OwnCn(int idx) const
  { return (LeabraCon*)&(cons_own[con_size * idx]); }
  // #CAT_Access fast access (no range or own_cons checking) to owned connection at given index, consumer must cast to appropriate type (for type safety, check con_type) -- compute algorithms should use this, as they know whether the connections are owned

  inline int            UnIdx(int idx) const
  { return unit_idxs[idx]; }
  // #CAT_Access fast access (no range checking) to unit flat index at given connection index
  inline LeabraUnit*          Un(int idx, LeabraNetwork** net) const;
  // #IGNORE #CAT_Access fast access (no range checking) to unit pointer at given connection index (goes through flat index at network level) -- todo: must be marked as IGNORE for maketa, but actually needs to be usable from css -- need to fix
  inline LeabraUnit*          UnFmLst(int idx, LeabraUnit** flat_units) const
  { return flat_units[unit_idxs[idx]]; }
  // #CAT_Access fast access (no range checking) to unit pointer at given index (goes through flat index at network level)

  /////////////////////////////
  //    Infrastructure

  virtual LeabraCon*    ConnectUnOwnCn(int to_idx);
  // add a connection to given unit
  virtual void          AllocCons(int n);
  // #CAT_Structure allocate storage for given number of connections (and Unit pointers) -- this MUST be called prior to making any new connections
  virtual void          FreeCons();
  // #CAT_Structure deallocate all connection-level storage (cons and units)

  /////////////////////////////
  //    Computation -- usually goes in LeabraConSpec / LeabraConSpec_inlines
  
  // this is all we really care about !!!

  inline void C_Send_NetinDelta_Thread(LeabraCon* cn, float* send_netin_vec,
                                       int ru_idx,
                                       const float su_act_delta_eff) {

    // note: may need to do various things here to convince the system to do sse
    // see: http://locklessinc.com/articles/vectorize/
    // float *x = __builtin_assume_aligned(send_netin_vec, 16);
    send_netin_vec[ru_idx] += cn->wt * su_act_delta_eff;
  }

  inline void Send_NetinDelta(LeabraNetwork* net, float su_act_delta) {
    const float su_act_delta_eff = su_act_delta;
    float* send_netin_vec = net->send_netin_tmp;
    for(int i=0; i<size; i++) {
      C_Send_NetinDelta_Thread(OwnCn(i), send_netin_vec,
                               UnIdx(i), su_act_delta_eff);
    }
  }

  LeabraSendCons();
  virtual ~LeabraSendCons();
};

#endif // x64vec_h

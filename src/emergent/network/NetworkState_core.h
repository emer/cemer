// this is included directly in NetworkState or NetworkState_cuda -- note must be made with new due to
// presence of virtual functions and need for virtual table pointer on appropriate device!!
// mark all methods as IGNORE -- no direct access to functions on this or any State objects
//{

  enum NetThrLayStats {         // stats that require holding threaded layer-level variables for subsequent aggregation
    SSE,
    PRERR,
    N_NetThrLayStats,
  };

  int           max_thr_n_units; // #NO_SAVE #READ_ONLY #CAT_State maximum number of units assigned to any one thread

  void*         net_owner;      // #IGNORE for emergent code, this is Network* that owns net_state -- for some rare cases that need it..
  char*         layers_mem;    // #IGNORE LayerState memory for all the state layers in order -- must all be the same type, size layer_state_size
  char*         prjns_mem;    // #IGNORE PrjnState memory for all the state projections in order -- must all be the same type, size prjn_state_size
  char*         ungps_mem;     // #IGNORE UnGpState memory for all the state unit groups in order -- must all be the same type, size ungp_state_size -- every layer has at least one of these for the global unit group
  int*          ungp_lay_idxs; // #IGNORE allocation of unit groups to layers -- array of int[n_ungps_built], containing layer indexes for each unit group
  int*          lay_send_prjns; // #IGNORE ordered list of sending projections for each layer -- layer has starting index into this list for each of its set of sending projections -- index is into prjns_mem 

  int*          units_lays;   // #IGNORE allocation of units to layers -- array of int[n_units_built], indexed starting at 1 (i.e., flat_idx with 0 = null unit) with each int indicating which layer is responsible for that unit
  int*          units_ungps;   // #IGNORE allocation of units to unit groups -- array of int[n_units_built], indexed starting at 1 (i.e., flat_idx with 0 = null unit) with each int indicating which unit group is responsible for that unit -- layer unit group if layer doesn't have sub unit groups, otherwise always the sub unit group
  int*          units_thrs;   // #IGNORE allocation of units to threads -- array of int[n_units+1], indexed starting at 1 (i.e., flat_idx with 0 = null unit) with each int indicating which thread is responsible for that unit
  int*          units_thr_un_idxs; // #IGNORE allocation of units to threads, thread-specific index for this unit -- array of int[n_units+1], indexed starting at 1 (i.e., flat_idx with 0 = null unit) with each int indicating index in thread-specific memory where that unit lives
  int*          thrs_n_units; // #IGNORE number of units assigned to each thread -- array of int[n_threads_built]
  int**         thrs_unit_idxs; // #IGNORE allocation of units to threads -- array of int*[n_thrs_built], pointing to arrays of int[thrs_n_units[thr_no]], containing unit->flat_idx indexes of units processed by a given thread (thread-centric complement to units_thrs)
  char**       thrs_units_mem;  // #IGNORE actual memory allocation of UnitState variables, organized by thread -- array of char*[n_thrs_built], pointing to arrays of char[thrs_n_units[thr_no] * unit_state_size], containing the units processed by a given thread -- this is the primary memory allocation of units
  int**        thrs_lay_unit_idxs; // #IGNORE allocation of units to layers by threads -- array of int**[n_thrs_built], pointing to arrays of int[n_layers_built * 2], containing  start and end thr_un_idx indexes of units processed by a given thread and a given layer
  int**        thrs_ungp_unit_idxs; // #IGNORE allocation of units to unit groups by threads -- array of int**[n_thrs_built], pointing to arrays of int[n_ungps_built * 2], containing  start and end thr_un_idx indexes of units processed by a given thread and a given unit group

  int          n_lay_stats;     // #IGNORE #DEF_6 number of thread-specific layer-level statistics that require variable memory storage
  int          n_lay_stats_vars; // #IGNORE #DEF_6 number of thread-specific layer-level statistic variables, per stat, available for stats algorithms
  float**      thrs_lay_stats;  // #IGNORE thread-specific layer-level stats variables available for stats routines to do efficient initial pre-computation across units at the thread level, followed by a main-thread integration of the thread-specific values -- array of float*[n_thrs_built] of float[n_lay_stats * n_lay_stats_vars * n_layers_built] -- n_lay_stats_vars is accessed as the inner dimension, then n_layers_built, then n_lay_stats as outer

  int*          units_n_recv_cgps;  // #IGNORE number of receiving connection groups per unit (flat_idx unit indexing, starts at 1)
  int*          units_n_send_cgps;  // #IGNORE number of sending connection groups per unit (flat_idx unit indexing, starts at 1)
  int           n_recv_cgps; // #IGNORE total number of units * recv con groups
  int           n_send_cgps; // #IGNORE total number of units * send con groups

  int**         thrs_units_n_recv_cgps;   // #IGNORE number of receiving connection groups per unit, per thread units (organized 1-to-1 with thrs_units*) -- array of int*[n_thrs_built], pointing to arrays of int[thrs_n_units[thr_no]], containing number of recv groups per unit
  int**         thrs_units_n_send_cgps;   // #IGNORE number of sending connection groups per unit, per thread units (organized 1-to-1 with thrs_units*) -- array of int*[n_thrs_built], pointing to arrays of int[thrs_n_units[thr_no]], containing number of send groups per unit
  int*          thrs_n_recv_cgps;       // #IGNORE total number of units * recv con groups per unit, per thread: array of int[n_thrs_built] of units * thrs_units_n_recv_cgps 
  int*          thrs_n_send_cgps;       // #IGNORE total number of units * send con groups per unit, per thread: array of int[n_thrs_built] of units * thrs_units_n_send_cgps 

  char**        thrs_recv_cgp_mem; // #IGNORE memory allocation for ConState for all recv connection group objects, by thread -- array of char*[n_thrs_built], pointing to arrays of char[thrs_n_recv_cgps[thr_no] * con_state_size], containing the recv ConState processed by a given thread -- this is the primary memory allocation of recv ConStates
  char**        thrs_send_cgp_mem; // #IGNORE memory allocation for ConState for all send connection group objects, by thread -- array of char*[n_thrs_built], pointing to arrays of char[thrs_n_send_cgps[thr_no] * con_state_size], containing the send ConState processed by a given thread -- this is the primary memory allocation of send ConStates
  int**         thrs_recv_cgp_start; // #IGNORE starting indexes into thrs_recv_cgp_mem, per thread units (organized 1-to-1 with thrs_units*) -- array of int*[n_thrs_built], pointing to arrays of int[thrs_n_units[thr_no]], containing indexes into thrs_recv_cgp_mem for first recv gp for given unit -- contains 0 for units that have none 
  int**         thrs_send_cgp_start; // #IGNORE starting indexes into thrs_send_cgp_mem, per thread units (organized 1-to-1 with thrs_units*) -- array of int*[n_thrs_built], pointing to arrays of int[thrs_n_units[thr_no]], containing indexes into thrs_send_cgp_mem for send recv gp for given unit -- contains 0 for units that have none 

  int64_t*      thrs_recv_cons_cnt; // #IGNORE number of floats to allocate to thrs_recv_cons_mem
  int64_t*      thrs_send_cons_cnt; // #IGNORE number of floats to allocate to thrs_send_cons_mem
  float**       thrs_recv_cons_mem; // #IGNORE bulk memory allocated for all of the recv connections, by thread -- array of float*[thrs_recv_cons_cnt[thr_no]]
  float**       thrs_send_cons_mem; // #IGNORE bulk memory allocated for all of the send connections, by thread -- array of float*[thrs_send_cons_cnt[thr_no]]

  int*          thrs_own_cons_max_size; // #IGNORE maximum alloc_size of any owning connection group, by thread -- for allocating temp structures..
  int64_t*      thrs_own_cons_tot_size; // #IGNORE total number of owned connections, by thread
  int64_t*      thrs_own_cons_tot_size_nonshared; // #IGNORE total number of owned connections, by thread, non-shared
  int*          thrs_own_cons_avg_size; // #IGNORE average size of any owning connection group, by thread -- for optimizing computation
  int*          thrs_own_cons_max_vars; // #IGNORE maximum NConVars of any owning connection group, by thread -- for allocating temp structures..
  float*        thrs_pct_cons_vec_chunked; // #IGNORE average percent of connections that are vector chunked (across owned projections and units), by thread
  float         pct_cons_vec_chunked; // #NO_SAVE #READ_ONLY #EXPERT average percent of connections that are vector chunked (across owned projections and units)

  int**         thrs_tmp_chunks;      // #IGNORE tmp con vec chunking memory
  int**         thrs_tmp_not_chunks;  // #IGNORE tmp con vec chunking memory
  float**       thrs_tmp_con_mem;     // #IGNORE tmp con vec chunking memory

  float**       thrs_send_netin_tmp; // #IGNORE temporary storage for threaded sender-based netinput computation -- float*[threads] array of float[n_units]

  int           dmem_nprocs;            // #IGNORE number of dmem processes being used now
  int           dmem_proc;              // #IGNORE process number for this process
#ifdef DMEM_COMPILE
  int64_t       all_dmem_sum_dwts_size; // #IGNORE size of temporary storage for threaded dmem sum dwts sync operation -- master block of all the mem -- this is what is actually allocated
  float**       thrs_dmem_sum_dwts_send; // #IGNORE temporary storage for threaded dmem sum dwts sync operation -- float*[threads] array of float[thrs_own_cons_tot_size + thrs_n_units] per thread (n_units for bias weights)
  float**       thrs_dmem_sum_dwts_recv; // #IGNORE temporary storage for threaded dmem sum dwts sync operation -- float*[threads] array of float[thrs_own_cons_tot_size + thrs_n_units] per thread (n_units for bias weights)
#endif

  /////////////////////////////////////////////////////////
  //            Specs and spec memory

  LAYER_SPEC**  layer_specs; // #NO_SAVE #READ_ONLY #CAT_Specs array of pointers to specs
  PRJN_SPEC**   prjn_specs; // #NO_SAVE #READ_ONLY #CAT_Specs array of pointers to specs
  UNIT_SPEC**   unit_specs; // #NO_SAVE #READ_ONLY #CAT_Specs array of pointers to specs
  CON_SPEC**    con_specs; // #NO_SAVE #READ_ONLY #CAT_Specs array of pointers to specs


  /////////////////////////////////////////////////////////
  //            Access

  INIMPL virtual void  StateError
  (const char* a, const char* b=NULL, const char* c=NULL, const char* d=NULL, const char* e=NULL,
   const char* f=NULL, const char* g=NULL, const char* h=NULL, const char* i=NULL) const;
  // #IGNORE every state impl must define this method in their .cpp file -- can be null -- all errors go here

  INIMPL virtual void  StateErrorVals
  (const char* msg, const char* var1=NULL, float val1=0, const char* var2=NULL, float val2=0,
   const char* var3=NULL, float val3=0, const char* var4=NULL, float val4=0) const;
  // #IGNORE for reporting values in error messages -- every state impl must define this method in their .cpp file -- can be null -- all errors go here

  INIMPL void   ThreadSyncSpin(int thr_no, int sync_no = 0);
  // #IGNORE net->threads.SyncSpin() for C++ threading -- this is a place where threaded code needs to sync with other threads -- null for cuda.. every machine target should have something or nothing here

  INLINE LAYER_SPEC*  GetLayerSpec(int spec_no) const {
    if(spec_no < 0 || spec_no >= n_layer_specs_built) {
#ifdef DEBUG    
      StateErrorVals("GetLayerSpec: spec index out of range", "spec_no", spec_no);
#endif
      return NULL;
    }
    return layer_specs[spec_no];
  }
  // #CAT_State Get layer spec at given index

  INLINE PRJN_SPEC*  GetPrjnSpec(int spec_no) const {
    if(spec_no < 0 || spec_no >= n_prjn_specs_built) {
#ifdef DEBUG    
      StateErrorVals("GetPrjnSpec: spec index out of range", "spec_no", spec_no);
#endif
      return NULL;
    }
    return prjn_specs[spec_no];
  }
  // #CAT_State Get projection spec at given index

  INLINE UNIT_SPEC*   GetUnitSpec(int spec_no) const {
    if(spec_no < 0 || spec_no >= n_unit_specs_built) {
#ifdef DEBUG    
      StateErrorVals("GetUnitSpec: spec index out of range", "spec_no", spec_no);
#endif
      return NULL;
    }
    return unit_specs[spec_no];
  }
  // #CAT_State Get unit spec at given index
  
  INLINE CON_SPEC*    GetConSpec(int spec_no) const {
    if(spec_no < 0 || spec_no >= n_con_specs_built) {
#ifdef DEBUG    
      StateErrorVals("GetConSpec: spec index out of range", "spec_no", spec_no);
#endif
      return NULL;
    }
    return con_specs[spec_no];
  }
  // #CAT_State Get con spec at given index
  
  INLINE bool   ThrInRange(int thr_no, bool err_msg = true) const
  { if(thr_no >= 0 && thr_no < n_thrs_built) return true;
    if(err_msg) StateErrorVals("ThrInRange: thread number out of range", "thread number:", thr_no);
    return false; }
  // #IGNORE test if thread number is in range

  INLINE bool   LayerInRange(int lay_idx, bool err_msg = true) const
  { if(lay_idx >= 0 && lay_idx < n_layers_built) return true;
    if(err_msg) StateErrorVals("LayerInRange: layer number out of range", "layer number:", lay_idx);
    return false; }
  // #IGNORE test if layer number is in range
  
  INLINE bool   PrjnInRange(int prjn_idx, bool err_msg = true) const
  { if(prjn_idx >= 0 && prjn_idx < n_prjns_built) return true;
    if(err_msg) StateErrorVals("PrjnInRange: projection number out of range", "prjn number:",prjn_idx);
    return false; }
  // #IGNORE test if prjn number is in range
  
  INLINE bool   UnGpInRange(int ungp_idx, bool err_msg = true) const
  { if(ungp_idx >= 0 && ungp_idx < n_ungps_built) return true;
    if(err_msg) StateErrorVals("UnGpInRange: unit group number out of range", "ungp number:", ungp_idx);
    return false; }
  // #IGNORE test if unit group number is in range
  
  INLINE LAYER_STATE* GetLayerState(int lay_idx) const {
#ifdef DEBUG
    if(!LayerInRange(lay_idx)) return NULL;
#endif
    return (LAYER_STATE*)(layers_mem + (lay_idx * layer_state_size));
  }
  // #CAT_State get layer state for given layer index
  INLINE LAYER_STATE* LayerStateForUn(int flat_idx) const {
    if(!UnFlatIdxInRange(flat_idx)) return NULL;
    return GetLayerState(units_lays[flat_idx]);
  }
  // #CAT_State get layer state for unit at given flat unit index

  INIMPL LAYER_STATE* FindLayerName(const char* lay_name) const;
  // #CAT_State find layer of given name

  INLINE PRJN_STATE* GetPrjnState(int prjn_idx) const {
#ifdef DEBUG
    if(!PrjnInRange(prjn_idx)) return NULL;
#endif
    return (PRJN_STATE*)(prjns_mem + (prjn_idx * prjn_state_size));
  }
  // #CAT_State get projection state for given prjn index (organized by receiving projections by layer)

  INLINE PRJN_STATE* GetSendPrjnState(int prjn_idx) const {
#ifdef DEBUG
    if(!PrjnInRange(prjn_idx)) return NULL;
#endif
    return GetPrjnState(lay_send_prjns[prjn_idx]);
  }
  // #CAT_State get projection state for given sending projection index (see layer prjn_start_idx)

  INLINE UNGP_STATE* GetUnGpState(int ungp_idx) const {
#ifdef DEBUG
    if(!UnGpInRange(ungp_idx)) return NULL;
#endif
    return (UNGP_STATE*)(ungps_mem + (ungp_idx * ungp_state_size));
  }
  // #CAT_State get unit group state for given ungp index
   INLINE int    UnGpLayIdx(int ungp_idx) const {
#ifdef DEBUG
    if(!UnGpInRange(ungp_idx)) return -1;
#endif
    return ungp_lay_idxs[ungp_idx]; }
  // #IGNORE layer index for given unit group 

  INLINE bool   UnFlatIdxInRange(int flat_idx, bool err_msg = true) const
  { if(flat_idx >= 1 && flat_idx < n_units_built) return true;
    if(err_msg) StateErrorVals("UnFlatIdxInRange: unit flat index number out of range",
                               "unit flat index number:", flat_idx);
    return false; }
  // #IGNORE test if unit flat index is in range
  INLINE bool   ThrUnIdxInRange(int thr_no, int thr_un_idx, bool err_msg = true) const
  { if(ThrInRange(thr_no) && thr_un_idx >= 0 && thr_un_idx < ThrNUnits(thr_no))
      return true;
    if(err_msg) StateErrorVals("ThrUnIdxInRange: unit thread index number out of range",
                               "unit thread index number:", thr_un_idx);
    return false; }
  // #IGNORE test if thread-based unit index is in range

  INLINE bool   UnRecvConGpInRange(int flat_idx, int recv_idx, bool err_msg = true) const
  { if(UnFlatIdxInRange(flat_idx) && recv_idx >= 0 && recv_idx < UnNRecvConGps(flat_idx))
      return true;
    if(err_msg) StateErrorVals("UnRecvConGpInRange: unit recv con group index number out of range",
                               "unit recv con group index number:", recv_idx);
    return false; }
  // #IGNORE test if unit recv con group index is in range
  INLINE bool   UnSendConGpInRange(int flat_idx, int send_idx, bool err_msg = true) const
  { if(UnFlatIdxInRange(flat_idx) && send_idx >= 0 && send_idx < UnNSendConGps(flat_idx))
      return true;
    if(err_msg) StateErrorVals("UnSendConGpInRange: unit send con group index number out of range",
                               "unit send con group index number:", send_idx);
    return false; }
  // #IGNORE test if unit send con group index is in range
  INLINE bool   ThrUnRecvConGpInRange(int thr_no, int thr_un_idx, int recv_idx,
                                      bool err_msg = true) const
  { if(ThrUnIdxInRange(thr_no, thr_un_idx)
       && recv_idx >= 0 && recv_idx < ThrUnNRecvConGps(thr_no, thr_un_idx))
      return true;
    if(err_msg) StateErrorVals("ThrUnRecvConGpInRange: unit recv con group index number out of range",
                               "unit recv con group index number:", recv_idx,
                               "thread unit idx:", thr_un_idx, "in thread:", thr_no);
    return false; }
  // #IGNORE test if thread-specified unit recv con group index is in range
  INLINE bool   ThrUnSendConGpInRange(int thr_no, int thr_un_idx, int send_idx,
                                      bool err_msg = true) const
  { if(ThrUnIdxInRange(thr_no, thr_un_idx)
       && send_idx >= 0 && send_idx < ThrUnNSendConGps(thr_no, thr_un_idx))
      return true;
    if(err_msg) StateErrorVals("ThrUnSendConGpInRange: unit send con group index number out of range",
                               "unit send con group index number:", send_idx,
                               "thread unit idx:", thr_un_idx, "in thread:", thr_no);
    return false; }
  // #IGNORE test if thread-specified unit send con group index is in range


  INLINE int    UnThr(int flat_idx) const {
#ifdef DEBUG
    if(!UnFlatIdxInRange(flat_idx)) return 0;
#endif
    return units_thrs[flat_idx]; }
  // #IGNORE thread that owns and processes the given unit (flat_idx)
  INLINE int    UnThrUnIdx(int flat_idx) const {
#ifdef DEBUG
    if(!UnFlatIdxInRange(flat_idx)) return 0;
#endif
    return units_thr_un_idxs[flat_idx]; }
  // #IGNORE index in thread-specific memory where that unit lives for given unit (flat_idx)
  INLINE int    ThrNUnits(int thr_no) const {
#ifdef DEBUG
    if(!ThrInRange(thr_no)) return 0;
#endif
    return thrs_n_units[thr_no]; }
  // #IGNORE number of units processed by given thread
  INLINE int    ThrUnitIdx(int thr_no, int thr_un_idx) const {
#ifdef DEBUG
    if(!ThrUnIdxInRange(thr_no, thr_un_idx)) return 0;
#endif
    return thrs_unit_idxs[thr_no][thr_un_idx]; }
  // #IGNORE flat_idx of unit at given thread, thread-specific unit index (max ThrNUnits()-1)
  INLINE UNIT_STATE*  ThrUnitState(int thr_no, int thr_un_idx) const {
#ifdef DEBUG
    if(!ThrUnIdxInRange(thr_no, thr_un_idx)) return NULL;
#endif
    return (UNIT_STATE*)(thrs_units_mem[thr_no] + (thr_un_idx * unit_state_size)); }
  // #IGNORE unit variables for unit at given thread, thread-specific unit index (max ThrNUnits()-1)
  INLINE UNIT_STATE*  GetUnitState(int flat_idx) const
  { return ThrUnitState(UnThr(flat_idx), UnThrUnIdx(flat_idx)); }
  // #IGNORE unit state at given flat_idx 
  INLINE int    ThrLayUnStart(int thr_no, int lay_no)
  { return thrs_lay_unit_idxs[thr_no][2*lay_no]; }
  // #IGNORE starting thread-specific unit index for given layer (from active_layers list)
  INLINE int    ThrLayUnEnd(int thr_no, int lay_no)
  { return thrs_lay_unit_idxs[thr_no][2*lay_no + 1]; }
  // #IGNORE ending thread-specific unit index for given layer (from state_layers list) -- this is like the max in a for loop -- valid indexes are < end
  INLINE int    ThrUnGpUnStart(int thr_no, int ugp_no)
  { return thrs_ungp_unit_idxs[thr_no][2*ugp_no]; }
  // #IGNORE starting thread-specific unit index for given unit group (from state_ungps list)
  INLINE int    ThrUnGpUnEnd(int thr_no, int ugp_no)
  { return thrs_ungp_unit_idxs[thr_no][2*ugp_no + 1]; }
  // #IGNORE ending thread-specific unit index for given unit group (from state_ungps list) -- this is like the max in a for loop -- valid indexes are < end

  INLINE float& ThrLayStats(int thr_no, int lay_idx, int stat_var, int stat_type) 
  { return thrs_lay_stats[thr_no]
      [stat_type * n_layers_built * n_lay_stats_vars + lay_idx * n_lay_stats_vars +
       stat_var]; }
  // #IGNORE get layer statistic value for given thread, layer (state layer index), stat variable number (0..n_lay_stats_vars-1 max), and stat type (SSE, PRERR, etc)

  INLINE int    UnNRecvConGps(int flat_idx) const {
#ifdef DEBUG
    if(!UnFlatIdxInRange(flat_idx)) return 0;
#endif
    return units_n_recv_cgps[flat_idx]; }
  // #IGNORE number of recv connection groups for given unit at flat_idx
  INLINE int    UnNSendConGps(int flat_idx) const {
#ifdef DEBUG
    if(!UnFlatIdxInRange(flat_idx)) return 0;
#endif
    return units_n_send_cgps[flat_idx]; }
  // #IGNORE number of send connection groups for given unit at flat_idx
  INLINE int    UnNRecvConGpsSafe(int flat_idx) const {
    if(!UnFlatIdxInRange(flat_idx)) return 0;
    return units_n_recv_cgps[flat_idx]; }
  // #IGNORE number of recv connection groups for given unit at flat_idx
  INLINE int    UnNSendConGpsSafe(int flat_idx) const {
    if(!UnFlatIdxInRange(flat_idx)) return 0;
    return units_n_send_cgps[flat_idx]; }
  // #IGNORE number of send connection groups for given unit at flat_idx

  INLINE int    ThrUnNRecvConGps(int thr_no, int thr_un_idx) const {
#ifdef DEBUG
    if(!ThrUnIdxInRange(thr_no, thr_un_idx)) return 0;
#endif
    return thrs_units_n_recv_cgps[thr_no][thr_un_idx]; }
  // #IGNORE number of recv connection groups for given unit within thread-specific memory at given thread number and thread-specific unit index
  INLINE int    ThrUnNSendConGps(int thr_no, int thr_un_idx) const {
#ifdef DEBUG
    if(!ThrUnIdxInRange(thr_no, thr_un_idx)) return 0;
#endif
    return thrs_units_n_send_cgps[thr_no][thr_un_idx]; }
  // #IGNORE number of send connection groups for given unit within thread-specific memory at given thread number and thread-specific unit index
  INLINE int    ThrUnNRecvConGpsSafe(int thr_no, int thr_un_idx) const {
    if(!ThrUnIdxInRange(thr_no, thr_un_idx)) return 0;
    return thrs_units_n_recv_cgps[thr_no][thr_un_idx]; }
  // #IGNORE number of recv connection groups for given unit within thread-specific memory at given thread number and thread-specific unit index
  INLINE int    ThrUnNSendConGpsSafe(int thr_no, int thr_un_idx) const {
    if(!ThrUnIdxInRange(thr_no, thr_un_idx)) return 0;
    return thrs_units_n_send_cgps[thr_no][thr_un_idx]; }
  // #IGNORE number of send connection groups for given unit within thread-specific memory at given thread number and thread-specific unit index
  
  INLINE int    ThrNRecvConGps(int thr_no) const {
#ifdef DEBUG
    if(!ThrInRange(thr_no)) return 0;
#endif
    return thrs_n_recv_cgps[thr_no]; }
  // #IGNORE number of recv connection groups as a flat list across all units processed by given thread
  INLINE int    ThrNSendConGps(int thr_no) const {
#ifdef DEBUG
    if(!ThrInRange(thr_no)) return 0;
#endif
    return thrs_n_send_cgps[thr_no]; }
  // #IGNORE number of send connection groups as a flat list across all units processed by given thread

  INLINE CON_STATE* ThrRecvConState(int thr_no, int thr_cgp_idx) const
  { return (CON_STATE*)(thrs_recv_cgp_mem[thr_no] + (thr_cgp_idx * con_state_size)); }
  // #IGNORE recv ConState for given thread, thread-specific con-group index 
  INLINE CON_STATE* ThrSendConState(int thr_no, int thr_cgp_idx) const
  { return (CON_STATE*)(thrs_send_cgp_mem[thr_no] + (thr_cgp_idx * con_state_size)); }
  // #IGNORE send ConState for given thread, thread-specific con-group index 

  INLINE CON_STATE* ThrUnRecvConState(int thr_no, int thr_un_idx, int recv_idx) const {
#ifdef DEBUG
    if(!ThrUnRecvConGpInRange(thr_no, thr_un_idx, recv_idx)) return NULL;
#endif
    return ThrRecvConState(thr_no, thrs_recv_cgp_start[thr_no][thr_un_idx] + recv_idx); }
  // #IGNORE recv ConState for given thread, thread-specific unit index, and recv group index
  INLINE CON_STATE* ThrUnSendConState(int thr_no, int thr_un_idx, int send_idx) const {
#ifdef DEBUG
    if(!ThrUnSendConGpInRange(thr_no, thr_un_idx, send_idx)) return 0;
#endif
    return ThrSendConState(thr_no, thrs_send_cgp_start[thr_no][thr_un_idx] + send_idx); }
  // #IGNORE send ConState for given thread, thread-specific unit index, and send group index

  INLINE CON_STATE* RecvConState(int flat_idx, int recv_idx) const {
#ifdef DEBUG
    if(!UnRecvConGpInRange(flat_idx, recv_idx)) return NULL;
#endif
    int thr_no = UnThr(flat_idx); 
    return ThrUnRecvConState(thr_no, UnThrUnIdx(flat_idx), recv_idx); }
  // #IGNORE recv ConState for given flat unit index and recv group index number
  INLINE CON_STATE* SendConState(int flat_idx, int send_idx) const {
#ifdef DEBUG
    if(!UnSendConGpInRange(flat_idx, send_idx)) return 0;
#endif
    int thr_no = UnThr(flat_idx); 
    return ThrUnSendConState(thr_no, UnThrUnIdx(flat_idx), send_idx); }
  // #IGNORE send ConState for given flat unit index and send index number

  INLINE CON_STATE* ThrUnRecvConStateSafe(int thr_no, int thr_un_idx, int recv_idx) const {
    if(!ThrUnRecvConGpInRange(thr_no, thr_un_idx, recv_idx)) return NULL;
    return ThrRecvConState(thr_no, thrs_recv_cgp_start[thr_no][thr_un_idx] + recv_idx); }
  // #IGNORE recv ConState for given thread, thread-specific unit index, and recv group index
  INLINE CON_STATE* ThrUnSendConStateSafe(int thr_no, int thr_un_idx, int send_idx) const {
    if(!ThrUnSendConGpInRange(thr_no, thr_un_idx, send_idx)) return 0;
    return ThrSendConState(thr_no, thrs_send_cgp_start[thr_no][thr_un_idx] + send_idx); }
  // #IGNORE send ConState for given thread, thread-specific unit index, and send group index

  INLINE CON_STATE* RecvConStateSafe(int flat_idx, int recv_idx) const {
    if(!UnRecvConGpInRange(flat_idx, recv_idx)) return NULL;
    int thr_no = UnThr(flat_idx); 
    return ThrUnRecvConState(thr_no, UnThrUnIdx(flat_idx), recv_idx); }
  // #IGNORE recv ConState for given flat unit index and recv group index number
  INLINE CON_STATE* SendConStateSafe(int flat_idx, int send_idx) const {
    if(!UnSendConGpInRange(flat_idx, send_idx)) return 0;
    int thr_no = UnThr(flat_idx); 
    return ThrUnSendConState(thr_no, UnThrUnIdx(flat_idx), send_idx); }
  // #IGNORE send ConState for given flat unit index and send index number

  INLINE float* ThrRecvConMem(int thr_no) const {
    return thrs_recv_cons_mem[thr_no];
  }
  // #IGNORE recv connection state memory for given thread
  INLINE float* ThrSendConMem(int thr_no) const {
    return thrs_send_cons_mem[thr_no];
  }
  // #IGNORE send connection state memory for given thread

  INLINE float* ThrSendNetinTmp(int thr_no) const 
  { return thrs_send_netin_tmp[thr_no]; }
  // #IGNORE temporary sending netinput memory for given thread -- no NETIN_PER_PRJN version
  INLINE float* ThrSendNetinTmpPerPrjn(int thr_no, int recv_idx) const 
  { return thrs_send_netin_tmp[thr_no] + recv_idx * n_units_built; }
  // #IGNORE temporary sending netinput memory for given thread -- NETIN_PER_PRJN version


  ////////////////////////////////////////////////////////////////
  //    Network algorithm code -- only the "within a thread" code here
  //    thread dispatch is on NetworkState_cpp, and Network calls into that
  //    also, because LayerState can't have any virtual methods, put those here!
  
  INIMPL virtual void Init_InputData_Thr(int thr_no);
  // #IGNORE initialize input data fields (ext, targ, ext_flags)
  INIMPL virtual void Init_InputData_Layers();
  // #IGNORE initialize input data fields for layers
  INIMPL virtual void Init_InputData_Layer(LAYER_STATE* lay);
  // #IGNORE Initializes external and target inputs (layer level only)

  INIMPL virtual void Init_Acts_Thr(int thr_no);
  // #IGNORE initialize activations
  INIMPL virtual void Init_Acts_Layer(LAYER_STATE* lay);
  // #IGNORE Initialize the unit state variables (layer level only)

  INIMPL virtual void Init_dWt_Thr(int thr_no);
  // #IGNORE inlitialize delta weight change aggregates

  INIMPL virtual void Init_Weights_Thr(int thr_no);
  // #IGNORE -- note: uses Projection wt init code so is not on net_state yet
  INIMPL virtual void Init_Weights_1Thr();
  // #IGNORE for INIT_WTS_1_THREAD -- requires consistent order!
  INIMPL virtual void Init_Weights_renorm_Thr(int thr_no);
  // #IGNORE renormalize weights after init, before sym
  INIMPL virtual void Init_Weights_sym_Thr(int thr_no);
  // #IGNORE symmetrize weights after first init pass, called when needed
  INIMPL virtual void Init_Weights_post_Thr(int thr_no);
  // #IGNORE
  INIMPL virtual void Init_Weights_Layers();
  // #IGNORE call layer-level init weights function -- after all unit-level inits

  INIMPL virtual void Init_Counters_State();
  // #IGNORE initialize counters controlled by the state-side
  INIMPL virtual void Init_Stats();
  // #IGNORE initialize statistics -- also calls Init_Stats_Layer
  INIMPL virtual void Init_Stats_Layers();
  // #IGNORE call layer-level Init stats function
  INIMPL virtual void Compute_Netin_Thr(int thr_no);
  // #IGNORE compute net input, receiver based
  INIMPL virtual void Send_Netin_Thr(int thr_no);
  // #IGNORE compute net input, sender based -- requires a wrap-up integration from temp buffers
  INIMPL virtual void Send_Netin_Integ();
  // #IGNORE integrate sent netinput across threads -- single thread only
  INIMPL virtual void Compute_Act_Thr(int thr_no);
  // #IGNORE compute activation from netinput
  INIMPL virtual void Compute_NetinAct_Thr(int thr_no);
  // #IGNORE compute both recv-based netinput and activation in one step -- for purely feedforward nets
  INIMPL virtual void Compute_dWt_Thr(int thr_no);
  // #IGNORE compute weight changes (learning)
  INIMPL virtual bool Compute_Weights_Test_impl(int trial_no);
  // #IGNORE test if it is time to update weights
  INIMPL virtual void Compute_Weights_Thr(int thr_no);
  // #IGNORE update weights from weight changes
#ifdef DMEM_COMPILE
  INIMPL virtual void  DMem_SumDWts_ToTmp_Thr(int thr_no);
  // #IGNORE copy to temp send buffer for sending, per thread
  INIMPL virtual void  DMem_SumDWts_FmTmp_Thr(int thr_no);
  // #IGNORE copy from temp recv buffer, per thread
#endif

  INIMPL virtual void Compute_SSE_Thr(int thr_no);
  // #IGNORE compute sum-of-squared error on output layers and units with targ state and ext_flags -- into stats temp buffs by thread
  INIMPL virtual void Compute_SSE_Agg(bool unit_avg = false, bool sqrt = false);
  // #IGNORE aggregate SSE from thread-specific stats buffers
  INIMPL virtual float Compute_SSE_Layer(LAYER_STATE* lay, int& n_vals, bool unit_avg = false, bool sqrt = false);
  // #IGNORE compute sum squared error of activation vs target over the entire layer -- always returns the actual sse, but unit_avg and sqrt flags determine averaging and sqrt of layer's own sse value -- uses sse_tol so error is 0 if within tolerance on a per unit basis


  INIMPL virtual void Compute_PRerr_Thr(int thr_no);
  // #IGNORE compute compute precision and recall error statistics over entire network -- true positive, false positive, and false negative -- precision = tp / (tp + fp) recall = tp / (tp + fn) fmeasure = 2 * p * r / (p + r), specificity, fall-out, mcc. -- into thread-specific stats buffers
  INIMPL virtual void Compute_PRerr_Agg();
  // #IGNORE aggregate thread-specific data
  INIMPL virtual int   Compute_PRerr_Layer(LAYER_STATE* lay);
  // #IGNORE compute precision and recall error statistics over entire layer -- true positive, false positive, and false negative -- returns number of values entering into computation (depends on number of targets) -- precision = tp / (tp + fp) recall = tp / (tp + fn) fmeasure = 2 * p * r / (p + r) -- uses sse_tol so error is 0 if within tolerance on a per unit basis -- results are stored in prerr values on layer


  INIMPL virtual void Compute_EpochSSE();
  // #IGNORE compute epoch-level sum squared error and related statistics
  INIMPL virtual void Compute_EpochSSE_Layer(LAYER_STATE* lay);
  // #IGNORE compute epoch-level sum squared error and related statistics
  
  INIMPL virtual void Compute_EpochPRerr();
  // #IGNORE compute epoch-level precision and recall statistics
  INIMPL virtual void Compute_EpochPRerr_Layer(LAYER_STATE* lay);
  // #IGNORE compute epoch-level precision and recall statistics


  INIMPL virtual void Compute_EpochStats_Layers();
  // #IGNORE compute epoch-level statistics at the layer level: SSE, PRerr -- overload in derived classes
  INIMPL virtual void Compute_EpochStats_Layer(LAYER_STATE* lay);
  // #IGNORE compute epoch-level statistics; DMem_ComputeAggs must have been called already (if dmem)

  INIMPL CON_STATE* FindRecipRecvCon(int& con_idx, UNIT_STATE* su, UNIT_STATE* ru);
  // #IGNORE find the reciprocal recv con group and con index for sending unit su to this receiving unit ru
  INIMPL CON_STATE* FindRecipSendCon(int& con_idx, UNIT_STATE* ru, UNIT_STATE* su);
  // #IGNORE find the reciprocal send con group and con index for receiving unit ru from this sending unit su

  INIMPL PRJN_STATE* FindRecvPrjnFrom(LAYER_STATE* recv_lay, LAYER_STATE* send_lay);
  // #IGNORE find projection for given sending layer into given recv layer -- null if not existant


  /////////////////////////////////////////////////////
  //    Build network

  INIMPL virtual void AllocSpecMem();
  // #IGNORE allocate spec mem

  INIMPL virtual LAYER_SPEC_CPP*    NewLayerSpec(int spec_type) const;
  // #IGNORE each type of Network MUST override this to create a new spec of correct type 
  INIMPL virtual PRJN_SPEC_CPP*     NewPrjnSpec(int spec_type) const;
  // #IGNORE add all new projections into main code base if possible -- so this usu not overriden
  INIMPL virtual UNIT_SPEC_CPP*     NewUnitSpec(int spec_type) const;
  // #IGNORE each type of Network MUST override this to create a new spec of correct type 
  INIMPL virtual CON_SPEC_CPP*      NewConSpec(int spec_type) const;
  // #IGNORE each type of Network MUST override this to create a new spec of correct type 

  INIMPL virtual void AllocLayerUnitMem();
  // #IGNORE allocate layer-level (prjn, ungp) and unit state memory
  INIMPL virtual void InitUnitIdxs();
  // #IGNORE initialize unit indexes to prepare for thread-specific later iteration, including assigning units to threads -- also counts up total number of recv and send ConState objects that we need to make
  INIMPL virtual void InitUnitState_Thr(int thr_no);
  // #IGNORE initialize thread-specific unit state memory -- this bind the thread-specific memory to threads, and further initializes all the indexes, and then initializes the UnitState objects, including setting all the relevant indexes etc
  INIMPL virtual void LayoutUnits();
  // #CAT_State update the UnitState positions based on layer geometry etc
  INIMPL virtual void AllocConStateMem();
  // #IGNORE allocate connection state object memory -- this is not the full connection-variable memory but just the ConState objects that manage that connection memory
  INIMPL virtual void InitConState_Thr(int thr_no);
  // #IGNORE initialize thread-specific ConState objects
  INIMPL virtual void AllocSendNetinTmpState();
  // #IGNORE allocate temp buffers for sender-based netinput
  INIMPL virtual void InitSendNetinTmp_Thr(int thr_no);
  // #IGNORE init send_netin_tmp for netin computation
  INIMPL virtual void Connect_Sizes();
  // #IGNORE first pass of connecting -- sets up all the Cons objects within units, and computes all the target allocation size information (done by projection specs)
  INIMPL virtual void AllocConsCountStateMem();
  // #IGNORE allocate connection counting state memory -- prior to connecting
  INIMPL virtual void Connect_AllocSizes_Thr(int thr_no);
  // #IGNORE second pass of connecting -- allocate all the memory for all the connections -- get the total sizes needed
  INIMPL virtual void Connect_Alloc_Thr(int thr_no);
  // #IGNORE second pass of connecting -- dole out the allocated memory to con groups
  INIMPL virtual void Connect_CacheMemStart_Thr(int thr_no);
  // #IGNORE cache connection memory start pointers -- after allocating but before connecting
  INIMPL virtual void Connect_Cons();
  // #IGNORE third pass of connecting -- actually make the connections -- done by projection specs
  INIMPL virtual void Connect_CacheUnitLoHiIdxs_Thr(int thr_no);
  // #IGNORE optimize searching for unit connections by caching the low and high unit indexes for connections
  INIMPL virtual void Connect_VecChunk_Thr(int thr_no);
  // #IGNORE fourth pass of connecting -- organize connections into optimal vectorizable chunks
  INIMPL virtual void AllocConnectionMem();
  // #IGNORE allocate full connection-level memory, after counts have been aggregated -- then second pass 
  INIMPL virtual void Connect_UpdtActives_Thr(int thr_no);
  // #IGNORE update the active flag status of all connections
  INIMPL virtual void CountCons();
  // #CAT_Structure count connections for all units in network
  INIMPL virtual void CountNonSharedRecvCons_Thr(int thr_no);
  // #IGNORE count non-shared recv cons, after cons all made..

  INIMPL virtual void FreeConMem();
  // #IGNORE free connection memory -- called by FreeStateMem
  INIMPL virtual void FreeStateMem();
  // #IGNORE free all state memory


  INIMPL void Initialize_core();
  // #IGNORE 


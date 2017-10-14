// this is included directly in NetworkState or NetworkState_cuda -- note must be made with new due to
// presence of virtual functions and need for virtual table pointer on appropriate device!!
//{

  enum NetThrLayStats {         // stats that require holding threaded layer-level variables for subsequent aggregation
    SSE,
    PRERR,
    N_NetThrLayStats,
  };

  int           max_thr_n_units; // #NO_SAVE #READ_ONLY #CAT_State maximum number of units assigned to any one thread

  char*         layers_mem;    // #NO_SAVE #READ_ONLY #CAT_State LayerState memory for all the state layers in order -- must all be the same type, size layer_state_size
  char*         prjns_mem;    // #NO_SAVE #READ_ONLY #CAT_State PrjnState memory for all the state projections in order -- must all be the same type, size prjn_state_size
  char*         ungps_mem;     // #NO_SAVE #READ_ONLY #CAT_State UnGpState memory for all the state unit groups in order -- must all be the same type, size ungp_state_size -- every layer has at least one of these for the global unit group
  int*          ungp_lay_idxs; // #IGNORE allocation of unit groups to layers -- array of int[n_ungps_built], containing layer indexes for each unit group

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
  UNIT_SPEC**   unit_specs; // #NO_SAVE #READ_ONLY #CAT_Specs array of pointers to specs
  CON_SPEC**    con_specs; // #NO_SAVE #READ_ONLY #CAT_Specs array of pointers to specs

  INIMPL virtual void  StateError(const char* a, const char* b=NULL, const char* c=NULL, const char* d=NULL,
                           const char* e=NULL, const char* f=NULL, const char* g=NULL,
                           const char* h=NULL, const char* i=NULL) const;
  // #IGNORE every state impl must define this method in their .cpp file -- can be null -- all errors go here

  INIMPL void   ThreadSyncSpin(int thr_no, int sync_no = 0);
  // #IGNORE net->threads.SyncSpin() for C++ threading -- this is a place where threaded code needs to sync with other threads -- null for cuda.. every machine target should have something or nothing here

  INLINE LAYER_SPEC*  GetLayerSpec(int spec_no)
  { if(spec_no >= 0 && spec_no < n_layer_specs_built)  return layer_specs[spec_no];
    return NULL; }
  // #CAT_Spec Get layer spec at given index

  INLINE UNIT_SPEC*   GetUnitSpec(int spec_no)
  { if(spec_no >= 0 && spec_no < n_unit_specs_built) return unit_specs[spec_no];
    return NULL; }
  // #CAT_Spec Get unit spec at given index
  
  INLINE CON_SPEC*    GetConSpec(int spec_no)
  { if(spec_no >= 0 && spec_no < n_con_specs_built) return con_specs[spec_no];
    return NULL; }
  // #CAT_Spec Get con spec at given index
  
  INLINE bool   ThrInRange(int thr_no, bool err_msg = true) const
  { if(thr_no >= 0 && thr_no < n_thrs_built) return true;
    if(err_msg) StateError("ThrInRange", "thread number:", /* String(thr_no),*/ "out of range");
    return false; }
  // #CAT_State test if thread number is in range

  INLINE bool   LayerInRange(int lay_idx, bool err_msg = true) const
  { if(lay_idx >= 0 && lay_idx < n_layers_built) return true;
    if(err_msg) StateError("LayerInRange", "layer number:", /* String(lay_idx),*/ "out of range");
    return false; }
  // #CAT_State test if layer number is in range
  
  INLINE bool   PrjnInRange(int prjn_idx, bool err_msg = true) const
  { if(prjn_idx >= 0 && prjn_idx < n_prjns_built) return true;
    if(err_msg) StateError("PrjnInRange", "prjn number:", /*String(prjn_idx),*/ "out of range");
    return false; }
  // #CAT_State test if prjn number is in range
  
  INLINE bool   UnGpInRange(int ungp_idx, bool err_msg = true) const
  { if(ungp_idx >= 0 && ungp_idx < n_ungps_built) return true;
    if(err_msg) StateError("UnGpInRange", "ungp number:", /* String(ungp_idx),*/ "out of range");
    return false; }
  // #CAT_State test if unit group number is in range
  
  INLINE LAYER_STATE* GetLayerState(int lay_idx) {
#ifdef DEBUG
    if(!LayerInRange(lay_idx)) return NULL;
#endif
    return (LAYER_STATE*)(layers_mem + (lay_idx * layer_state_size));
  }
  // #CAT_State get layer state for given layer index

  INLINE PRJN_STATE* GetPrjnState(int prjn_idx) {
#ifdef DEBUG
    if(!PrjnInRange(prjn_idx)) return NULL;
#endif
    return (PRJN_STATE*)(prjns_mem + (prjn_idx * prjn_state_size));
  }
  // #CAT_State get unit group state for given prjn index

  INLINE UNGP_STATE* GetUnGpState(int ungp_idx) {
#ifdef DEBUG
    if(!UnGpInRange(ungp_idx)) return NULL;
#endif
    return (UNGP_STATE*)(ungps_mem + (ungp_idx * ungp_state_size));
  }
  // #CAT_State get unit group state for given ungp index
   INLINE int    UnGpLayIdx(int ungp_idx) {
#ifdef DEBUG
    if(!UnGpInRange(ungp_idx)) return -1;
#endif
    return ungp_lay_idxs[ungp_idx]; }
  // #CAT_State layer index for given unit group 

  INLINE bool   UnFlatIdxInRange(int flat_idx, bool err_msg = true) const
  { if(flat_idx >= 1 && flat_idx < n_units_built) return true;
    if(err_msg) StateError("UnFlatIdxInRange", "unit flat index number:"/*, String(flat_idx),*/
                           "out of range");
    return false; }
  // #CAT_State test if unit flat index is in range
  INLINE bool   ThrUnIdxInRange(int thr_no, int thr_un_idx, bool err_msg = true) const
  { if(ThrInRange(thr_no) && thr_un_idx >= 0 && thr_un_idx < ThrNUnits(thr_no))
      return true;
    if(err_msg) StateError("ThrUnIdxInRange", "unit thread index number:",
                           /* String(thr_un_idx),*/ "out of range in thread:"/*, String(thr_no)*/);
    return false; }
  // #CAT_State test if thread-based unit index is in range

  INLINE bool   UnRecvConGpInRange(int flat_idx, int recv_idx, bool err_msg = true) const
  { if(UnFlatIdxInRange(flat_idx) && recv_idx >= 0 && recv_idx < UnNRecvConGps(flat_idx))
      return true;
    if(err_msg) StateError("UnRecvConGpInRange", "unit recv con group index number:",
                           /*String(recv_idx),*/ "out of range in unit flat idx:"/*, String(flat_idx)*/);
    return false; }
  // #CAT_State test if unit recv con group index is in range
  INLINE bool   UnSendConGpInRange(int flat_idx, int send_idx, bool err_msg = true) const
  { if(UnFlatIdxInRange(flat_idx) && send_idx >= 0 && send_idx < UnNSendConGps(flat_idx))
      return true;
    if(err_msg) StateError("UnSendConGpInRange", "unit send con group index number:",
                           /* String(send_idx),*/ "out of range in unit flat idx:"/*, String(flat_idx)*/);
    return false; }
  // #CAT_State test if unit send con group index is in range
  INLINE bool   ThrUnRecvConGpInRange(int thr_no, int thr_un_idx, int recv_idx,
                                      bool err_msg = true) const
  { if(ThrUnIdxInRange(thr_no, thr_un_idx)
       && recv_idx >= 0 && recv_idx < ThrUnNRecvConGps(thr_no, thr_un_idx))
      return true;
    if(err_msg) StateError("ThrUnRecvConGpInRange", "unit recv con group index number:",
                           /*String(recv_idx),*/
                           "out of range in thread unit idx:", /*String(thr_un_idx),*/
                           "in thread:"/*, String(thr_no)*/);
    return false; }
  // #CAT_State test if thread-specified unit recv con group index is in range
  INLINE bool   ThrUnSendConGpInRange(int thr_no, int thr_un_idx, int send_idx,
                                      bool err_msg = true) const
  { if(ThrUnIdxInRange(thr_no, thr_un_idx)
       && send_idx >= 0 && send_idx < ThrUnNSendConGps(thr_no, thr_un_idx))
      return true;
    if(err_msg) StateError("ThrUnSendConGpInRange", "unit send con group index number:",
                           /*String(send_idx),*/
                           "out of range in thread unit idx:", /*String(thr_un_idx),*/
                           "in thread:" /*, String(thr_no)*/ );
    return false; }
  // #CAT_State test if thread-specified unit send con group index is in range


  INLINE int    UnThr(int flat_idx) const {
#ifdef DEBUG
    if(!UnFlatIdxInRange(flat_idx)) return 0;
#endif
    return units_thrs[flat_idx]; }
  // #CAT_State thread that owns and processes the given unit (flat_idx)
  INLINE int    UnThrUnIdx(int flat_idx) const {
#ifdef DEBUG
    if(!UnFlatIdxInRange(flat_idx)) return 0;
#endif
    return units_thr_un_idxs[flat_idx]; }
  // #CAT_State index in thread-specific memory where that unit lives for given unit (flat_idx)
  INLINE int    ThrNUnits(int thr_no) const {
#ifdef DEBUG
    if(!ThrInRange(thr_no)) return 0;
#endif
    return thrs_n_units[thr_no]; }
  // #CAT_State number of units processed by given thread
  INLINE int    ThrUnitIdx(int thr_no, int thr_un_idx) const {
#ifdef DEBUG
    if(!ThrUnIdxInRange(thr_no, thr_un_idx)) return 0;
#endif
    return thrs_unit_idxs[thr_no][thr_un_idx]; }
  // #CAT_State flat_idx of unit at given thread, thread-specific unit index (max ThrNUnits()-1)
  INLINE UNIT_STATE*  ThrUnitState(int thr_no, int thr_un_idx) const {
#ifdef DEBUG
    if(!ThrUnIdxInRange(thr_no, thr_un_idx)) return NULL;
#endif
    return (UNIT_STATE*)(thrs_units_mem[thr_no] + (thr_un_idx * unit_state_size)); }
  // #CAT_State unit variables for unit at given thread, thread-specific unit index (max ThrNUnits()-1)
  INLINE UNIT_STATE*  UnUnitState(int flat_idx) const
  { return ThrUnitState(UnThr(flat_idx), UnThrUnIdx(flat_idx)); }
  // #CAT_State unit variables for unit at given unit at flat_idx 
  INLINE int    ThrLayUnStart(int thr_no, int lay_no)
  { return thrs_lay_unit_idxs[thr_no][2*lay_no]; }
  // #CAT_State starting thread-specific unit index for given layer (from active_layers list)
  INLINE int    ThrLayUnEnd(int thr_no, int lay_no)
  { return thrs_lay_unit_idxs[thr_no][2*lay_no + 1]; }
  // #CAT_State ending thread-specific unit index for given layer (from state_layers list) -- this is like the max in a for loop -- valid indexes are < end
  INLINE int    ThrUnGpUnStart(int thr_no, int ugp_no)
  { return thrs_ungp_unit_idxs[thr_no][2*ugp_no]; }
  // #CAT_State starting thread-specific unit index for given unit group (from state_ungps list)
  INLINE int    ThrUnGpUnEnd(int thr_no, int ugp_no)
  { return thrs_ungp_unit_idxs[thr_no][2*ugp_no + 1]; }
  // #CAT_State ending thread-specific unit index for given unit group (from state_ungps list) -- this is like the max in a for loop -- valid indexes are < end

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
  // #CAT_State number of recv connection groups for given unit at flat_idx
  INLINE int    UnNSendConGps(int flat_idx) const {
#ifdef DEBUG
    if(!UnFlatIdxInRange(flat_idx)) return 0;
#endif
    return units_n_send_cgps[flat_idx]; }
  // #CAT_State number of send connection groups for given unit at flat_idx
  INLINE int    UnNRecvConGpsSafe(int flat_idx) const {
    if(!UnFlatIdxInRange(flat_idx)) return 0;
    return units_n_recv_cgps[flat_idx]; }
  // #CAT_State number of recv connection groups for given unit at flat_idx
  INLINE int    UnNSendConGpsSafe(int flat_idx) const {
    if(!UnFlatIdxInRange(flat_idx)) return 0;
    return units_n_send_cgps[flat_idx]; }
  // #CAT_State number of send connection groups for given unit at flat_idx

  INLINE int    ThrUnNRecvConGps(int thr_no, int thr_un_idx) const {
#ifdef DEBUG
    if(!ThrUnIdxInRange(thr_no, thr_un_idx)) return 0;
#endif
    return thrs_units_n_recv_cgps[thr_no][thr_un_idx]; }
  // #CAT_State number of recv connection groups for given unit within thread-specific memory at given thread number and thread-specific unit index
  INLINE int    ThrUnNSendConGps(int thr_no, int thr_un_idx) const {
#ifdef DEBUG
    if(!ThrUnIdxInRange(thr_no, thr_un_idx)) return 0;
#endif
    return thrs_units_n_send_cgps[thr_no][thr_un_idx]; }
  // #CAT_State number of send connection groups for given unit within thread-specific memory at given thread number and thread-specific unit index
  INLINE int    ThrUnNRecvConGpsSafe(int thr_no, int thr_un_idx) const {
    if(!ThrUnIdxInRange(thr_no, thr_un_idx)) return 0;
    return thrs_units_n_recv_cgps[thr_no][thr_un_idx]; }
  // #CAT_State number of recv connection groups for given unit within thread-specific memory at given thread number and thread-specific unit index
  INLINE int    ThrUnNSendConGpsSafe(int thr_no, int thr_un_idx) const {
    if(!ThrUnIdxInRange(thr_no, thr_un_idx)) return 0;
    return thrs_units_n_send_cgps[thr_no][thr_un_idx]; }
  // #CAT_State number of send connection groups for given unit within thread-specific memory at given thread number and thread-specific unit index
  
  INLINE int    ThrNRecvConGps(int thr_no) const {
#ifdef DEBUG
    if(!ThrInRange(thr_no)) return 0;
#endif
    return thrs_n_recv_cgps[thr_no]; }
  // #CAT_State number of recv connection groups as a flat list across all units processed by given thread
  INLINE int    ThrNSendConGps(int thr_no) const {
#ifdef DEBUG
    if(!ThrInRange(thr_no)) return 0;
#endif
    return thrs_n_send_cgps[thr_no]; }
  // #CAT_State number of send connection groups as a flat list across all units processed by given thread

  INLINE CON_STATE* ThrRecvConState(int thr_no, int thr_cgp_idx) const
  { return (CON_STATE*)(thrs_recv_cgp_mem[thr_no] + (thr_cgp_idx * con_state_size)); }
  // #CAT_State recv ConState for given thread, thread-specific con-group index 
  INLINE CON_STATE* ThrSendConState(int thr_no, int thr_cgp_idx) const
  { return (CON_STATE*)(thrs_send_cgp_mem[thr_no] + (thr_cgp_idx * con_state_size)); }
  // #CAT_State send ConState for given thread, thread-specific con-group index 

  INLINE CON_STATE* ThrUnRecvConState(int thr_no, int thr_un_idx, int recv_idx) const {
#ifdef DEBUG
    if(!ThrUnRecvConGpInRange(thr_no, thr_un_idx, recv_idx)) return NULL;
#endif
    return ThrRecvConState(thr_no, thrs_recv_cgp_start[thr_no][thr_un_idx] + recv_idx); }
  // #CAT_State recv ConState for given thread, thread-specific unit index, and recv group index
  INLINE CON_STATE* ThrUnSendConState(int thr_no, int thr_un_idx, int send_idx) const {
#ifdef DEBUG
    if(!ThrUnSendConGpInRange(thr_no, thr_un_idx, send_idx)) return 0;
#endif
    return ThrSendConState(thr_no, thrs_send_cgp_start[thr_no][thr_un_idx] + send_idx); }
  // #CAT_State send ConState for given thread, thread-specific unit index, and send group index

  INLINE CON_STATE* RecvConState(int flat_idx, int recv_idx) const {
#ifdef DEBUG
    if(!UnRecvConGpInRange(flat_idx, recv_idx)) return NULL;
#endif
    int thr_no = UnThr(flat_idx); 
    return ThrUnRecvConState(thr_no, UnThrUnIdx(flat_idx), recv_idx); }
  // #CAT_State recv ConState for given flat unit index and recv group index number
  INLINE CON_STATE* SendConState(int flat_idx, int send_idx) const {
#ifdef DEBUG
    if(!UnSendConGpInRange(flat_idx, send_idx)) return 0;
#endif
    int thr_no = UnThr(flat_idx); 
    return ThrUnSendConState(thr_no, UnThrUnIdx(flat_idx), send_idx); }
  // #CAT_State send ConState for given flat unit index and send index number

  INLINE CON_STATE* ThrUnRecvConStateSafe(int thr_no, int thr_un_idx, int recv_idx) const {
    if(!ThrUnRecvConGpInRange(thr_no, thr_un_idx, recv_idx)) return NULL;
    return ThrRecvConState(thr_no, thrs_recv_cgp_start[thr_no][thr_un_idx] + recv_idx); }
  // #CAT_State recv ConState for given thread, thread-specific unit index, and recv group index
  INLINE CON_STATE* ThrUnSendConStateSafe(int thr_no, int thr_un_idx, int send_idx) const {
    if(!ThrUnSendConGpInRange(thr_no, thr_un_idx, send_idx)) return 0;
    return ThrSendConState(thr_no, thrs_send_cgp_start[thr_no][thr_un_idx] + send_idx); }
  // #CAT_State send ConState for given thread, thread-specific unit index, and send group index

  INLINE CON_STATE* RecvConStateSafe(int flat_idx, int recv_idx) const {
    if(!UnRecvConGpInRange(flat_idx, recv_idx)) return NULL;
    int thr_no = UnThr(flat_idx); 
    return ThrUnRecvConState(thr_no, UnThrUnIdx(flat_idx), recv_idx); }
  // #CAT_State recv ConState for given flat unit index and recv group index number
  INLINE CON_STATE* SendConStateSafe(int flat_idx, int send_idx) const {
    if(!UnSendConGpInRange(flat_idx, send_idx)) return 0;
    int thr_no = UnThr(flat_idx); 
    return ThrUnSendConState(thr_no, UnThrUnIdx(flat_idx), send_idx); }
  // #CAT_State send ConState for given flat unit index and send index number

  INLINE float* ThrRecvConMem(int thr_no) const {
    return thrs_recv_cons_mem[thr_no];
  }
  // #CAT_State recv connection state memory for given thread
  INLINE float* ThrSendConMem(int thr_no) const {
    return thrs_send_cons_mem[thr_no];
  }
  // #CAT_State send connection state memory for given thread

  INLINE float* ThrSendNetinTmp(int thr_no) const 
  { return thrs_send_netin_tmp[thr_no]; }
  // #CAT_State temporary sending netinput memory for given thread -- no NETIN_PER_PRJN version
  INLINE float* ThrSendNetinTmpPerPrjn(int thr_no, int recv_idx) const 
  { return thrs_send_netin_tmp[thr_no] + recv_idx * n_units_built; }
  // #CAT_State temporary sending netinput memory for given thread -- NETIN_PER_PRJN version


  /////////////////////////////////////////////////////
  //    Build network


  INIMPL virtual void AllocSpecMem();
  // #IGNORE allocate spec mem

  INIMPL virtual LAYER_SPEC*    NewLayerSpec(int spec_type) const;
  // #IGNORE each type of Network MUST override this to create a new spec of correct type 
  INIMPL virtual UNIT_SPEC*     NewUnitSpec(int spec_type) const;
  // #IGNORE each type of Network MUST override this to create a new spec of correct type 
  INIMPL virtual CON_SPEC*      NewConSpec(int spec_type) const;
  // #IGNORE each type of Network MUST override this to create a new spec of correct type 

  INIMPL virtual void AllocLayUnitMem();
  // #IGNORE allocate layer and unit state memory
  INIMPL virtual void AllocConGpMem();
  // #IGNORE allocate connection group memory
  INIMPL virtual void AllocSendNetinTmpState();
  // #IGNORE allocate temp buffers for sender-based netinput
  INIMPL virtual void AllocConsCountStateMem();
  // #IGNORE allocate connection counting state memory -- prior to connecting
  INIMPL virtual void AllocConsStateMem();
  // #IGNORE allocate connection state memory, after counts have been aggregated -- then second pass 
  INIMPL virtual void InitSendNetinTmp_Thr(int thr_no);
  // #IGNORE init send_netin_tmp for netin computation

  INIMPL virtual void FreeConMem();
  // #IGNORE free connection memory -- called by FreeStateMem
  INIMPL virtual void FreeStateMem();
  // #IGNORE free all state memory

  INIMPL virtual void CacheMemStart_Thr(int thr_no);
  // #IGNORE cache connection memory start pointers -- after connecting


  ////////////////////////////////////////////////////////////////
  //    Network algorithm code -- only the "within a thread" code here -- thread dispatch
  //    is only supported by main c++ outer loops
  
  INIMPL virtual void Init_InputData_Thr(int thr_no);
  // #IGNORE initialize input data fields (ext, targ, ext_flags)
  INIMPL virtual void Init_InputData_Layer();
  // #IGNORE initialize input data fields for layers
  INIMPL virtual void Init_Acts_Thr(int thr_no);
  // #IGNORE initialize activations
  INIMPL virtual void Init_dWt_Thr(int thr_no);
  // #IGNORE inlitialize delta weight change aggregates
  INIMPL virtual void Init_Weights_sym(int thr_no);
  // #IGNORE symmetrize weights after first init pass, called when needed
  INIMPL virtual void Init_Weights_post_Thr(int thr_no);
  // #IGNORE
  INIMPL virtual void Init_Weights_Layer();
  // #CAT_Learning call layer-level init weights function -- after all unit-level inits
  INIMPL virtual void Init_Stats_Layer();
  // #CAT_Learning call layer-level Init stats function
  INIMPL virtual void Compute_Netin_Thr(int thr_no);
  // #IGNORE compute net input, receiver based
  INIMPL virtual void Send_Netin_Thr(int thr_no);
  // #IGNORE compute net input, sender based -- requires a wrap-up integration from temp buffers
  INIMPL virtual void Compute_Act_Thr(int thr_no);
  // #IGNORE compute activation from netinput
  INIMPL virtual void Compute_NetinAct_Thr(int thr_no);
  // #IGNORE compute both recv-based netinput and activation in one step -- for purely feedforward nets
  INIMPL virtual void Compute_dWt_Thr(int thr_no);
  // #IGNORE compute weight changes (learning)
  INIMPL virtual void Compute_Weights_Thr(int thr_no);
  // #IGNORE update weights from weight changes
  INIMPL virtual void Compute_SSE_Thr(int thr_no);
  // #IGNORE compute sum-of-squared error on output layers and units with targ state and ext_flags -- into stats temp buffs by thread
  INIMPL virtual void Compute_SSE_Agg(bool unit_avg = false, bool sqrt = false);
  // #IGNORE aggregate SSE from thread-specific stats buffers
  INIMPL virtual void Compute_PRerr_Thr(int thr_no);
  // #IGNORE compute compute precision and recall error statistics over entire network -- true positive, false positive, and false negative -- precision = tp / (tp + fp) recall = tp / (tp + fn) fmeasure = 2 * p * r / (p + r), specificity, fall-out, mcc. -- into thread-specific stats buffers
  INIMPL virtual void Compute_PRerr_Agg();
  // #IGNORE aggregate thread-specific data
  INIMPL virtual void Compute_EpochStats_Layer();
  // #IGNORE compute epoch-level statistics at the layer level: SSE, PRerr -- overload in derived classes

  INIMPL ConState_cpp* FindRecipRecvCon(int& con_idx, NetworkState_cpp* net, UnitState_cpp* su,
                                             UnitState_cpp* ru);
  // #CAT_State find the reciprocal recv con group and con index for sending unit su to this receiving unit ru
  INIMPL ConState_cpp* FindRecipSendCon(int& con_idx, NetworkState_cpp* net, UnitState_cpp* ru,
                                        UnitState_cpp* su);
  // #CAT_State find the reciprocal send con group and con index for receiving unit ru from this sending unit su

  INIMPL void Initialize_core();
  // #IGNORE 


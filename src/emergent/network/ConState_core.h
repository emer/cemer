// this is included directly in ConState_cpp and _cuda
// {

  // note: define new enums for other variables, typically in ConSpec, adding from DWT
  enum ConVars {                // Connection variables -- must align with Connection obj
    WT,                         // the synaptic weight of connection
    DWT,                        // change in synaptic weight as computed by learning mechanism
  };

  enum ConStateFlags {  // #BITS flags for this connection group
    CG_0 = 0x0,         // #IGNORE 
    OWN_CONS = 0x0001,  // this guy owns the connections -- else gets links to others
    RECV_CONS = 0x0002, // we are a recv con group -- else a send con group
    IS_ACTIVE = 0x0004, // we are an active con group -- projection is active and size > 0
    SHARING = 0x0008,   // this OWN_CONS group is sharing connection objects from another con group
    CHUNKS_SAME_SUGP = 0x0010,  // for Leabra: chunks all have the same sender unit group -- important for optimization of netin
  };

  static const int vec_chunk_targ = 8; // #READ_ONLY #NO_SAVE #DEF_8 target chunk size for vectorized operations over connections -- is currently set to 8 for all types of processors so that the weight files have a consistent layout -- however it can be set dynamically prior to building the network to experiment with different values -- affects allocation modulus

  int           own_flat_idx;   // #CAT_State #READ_ONLY unit flat index of unit that owns us
  int           own_thr_idx;    // #CAT_State #READ_ONLY unit thread-specific index of unit that owns us
  int           spec_idx;       // #CAT_State #READ_ONLY index into network state conspecs list for our conspec
  int           prjn_idx;       // #CAT_State #READ_ONLY index into network state projection state list for the projection that we correspond to
  ConStateFlags flags;          // #CAT_State #READ_ONLY #SHOW flags for this connection group
  int           size;           // #CAT_State #READ_ONLY #SHOW number of connections currently active
  int           alloc_size;     // #CAT_State #READ_ONLY #SHOW allocated size -- no more than this number of connections may be created -- it is a hard limit set by the alloc function
  int           other_idx;      // #CAT_State #READ_ONLY #SHOW index into other direction's list of cons objects (i.e., send_idx for RecvCons and recv_idx for SendCons)
  int           share_idx;      // #CAT_State #READ_ONLY #SHOW index of other unit that this congroup shares connection objects with -- unit must be in same layer with same number of connections, and in the same thread (i.e., the number of units in a unit group must be an even multiple of the number of threads), and must be *earlier* in the layer (typically the first unit group) -- projection must set this during the initial pre-allocation phase of connecting -- if -1 then this congroup has its own unique connection objects
  int           vec_chunked_size; // #CAT_State #READ_ONLY #SHOW number of connections at start of list that are chunked according to vec_chunk_targ -- for sender-based, this means that the recv unit_idx's are sequential for each of the chunks (individually) -- between chunks can be non-sequential
  int           un_lo_idx;        // #CAT_State #READ_ONLY lowest flat_idx of units that we are connected to -- cached after connections are made -- optimizes finding connections
  int           un_hi_idx;        // #CAT_State #READ_ONLY highest flat_idx of units that we are connected to -- cached after connections are made -- optimizes finding connections

  int           n_con_vars;     // #CAT_State #READ_ONLY number of connection variables
  con_mem_idx   mem_idx;        // #IGNORE #CAT_State #READ_ONLY index into NetworkState allocated thread-specific connection memory -- for owned, non-shared cons, it is (con_type->members.size + 1) * sizeof(float) * alloc_size, for shared cons it is just sizeof(float) * alloc_size (first slice is index of other unit), for ptr cons, it is 2 * sizeof(float) * alloc_size (first slice is index of other unit, second is connection number there to find con data)
  con_mem_idx   cnmem_idx;      // #IGNORE #CAT_State #READ_ONLY index into NetworkState allocated thread-specific connection memory -- for shared connections this points to the memory in the source connection. it is of size (con_type->members.size + 1) * sizeof(float) * alloc_size
  float*        mem_start;      // #IGNORE pointer into Network allocated connection memory -- cached after network is built on device, based on mem_idx -- for owned, non-shared cons, it is (con_type->members.size + 1) * sizeof(float) * alloc_size, for shared cons it is just sizeof(float) * alloc_size (first slice is index of other unit), for ptr cons, it is 2 * sizeof(float) * alloc_size (first slice is index of other unit, second is connection number there to find con data)
  float*        cnmem_start;    // #IGNORE pointer to start of connection variable memory -- cached after network is built on device, based on mem_idx -- for shared connections this points to the memory in the source connection. it is of size (con_type->members.size + 1) * sizeof(float) * alloc_size
  float         temp1;          // temporary compute value -- e.g., net input

  
  INLINE CON_SPEC_CPP* GetConSpec(NETWORK_STATE* net) const {
    return net->GetConSpec(spec_idx);
  }
  // #CAT_State get connection spec that manages these connections
  INLINE PRJN_STATE* GetPrjnState(NETWORK_STATE* net) const {
    return net->GetPrjnState(prjn_idx);
  }
  // #CAT_State get projection state that manages these connections
  INLINE LAYER_STATE* GetRecvLayer(NETWORK_STATE* net) const {
    PRJN_STATE* prjn = GetPrjnState(net);
    if(prjn) {
      return prjn->GetRecvLayerState(net);
    }
    return NULL;
  }
  // #CAT_State get recv layer for this projection
  INLINE LAYER_STATE* GetSendLayer(NETWORK_STATE* net) const {
    PRJN_STATE* prjn = GetPrjnState(net);
    if(prjn) {
      return prjn->GetSendLayerState(net);
    }
    return NULL;
  }
  // #CAT_State get send layer for this projection
  
  INLINE bool           HasConStateFlag(int flag) const 
  { return (flags & flag); }
  // #CAT_ObjectMgmt true if flag set, or if multiple, any set
  INLINE void           SetConStateFlag(int flag)
  { flags = (ConStateFlags)(flags | flag); }
  // #CAT_ObjectMgmt sets the flag(s)
  INLINE void           ClearConStateFlag(int flag)
  { flags = (ConStateFlags)(flags & ~flag); }
  // #CAT_ObjectMgmt clears the flag(s)
  void                  ChangeConStateFlag(int flag, bool set)
  {if (set) SetConStateFlag(flag); else ClearConStateFlag(flag);}
  // #CAT_ObjectMgmt sets or clears the flag(s)

  INLINE bool  OwnCons() const { return HasConStateFlag(OWN_CONS); }
  // #CAT_State do we own the connections?  else just point to them
  INLINE bool  PtrCons() const { return !HasConStateFlag(OWN_CONS); }
  // #CAT_State do we have pointers to connections?  else we own them
  INLINE bool  IsRecv() const  { return HasConStateFlag(RECV_CONS); }
  // #CAT_State is this a receiving con group?  else sending
  INLINE bool  IsSend() const  { return !HasConStateFlag(RECV_CONS); }
  // #CAT_State is this a sending con group?  else receiving
  INLINE bool  IsActive() const { return HasConStateFlag(IS_ACTIVE); }
  // #CAT_State is this an active connection group, with connections and an active projection?
  INLINE bool  NotActive() const { return !HasConStateFlag(IS_ACTIVE); }
  // #CAT_State is this NOT an active connection group, with connections and an active projection?
  INLINE bool  Sharing() const { return HasConStateFlag(SHARING); }
  // #CAT_State is this group sharing connections from another connection group?

  INLINE bool  InRange(int idx) const
  { return ((idx < size) && (idx >= 0)); }
  // #CAT_Access is index in range?

  ////////////////////////////////////////////////////////////////////////////////
  //    Primary infrastructure management routines

  INLINE bool           PrjnIsActive(NETWORK_STATE* net)
  { PRJN_STATE* prjn = GetPrjnState(net);
    if(!prjn) return false;
    return prjn->IsActive(net); }
  // #CAT_State is the projection active for this connection group?

  INLINE void           UpdtIsActive(NETWORK_STATE* net)
  { if(alloc_size > 0 && size > 0 && PrjnIsActive(net))
      SetConStateFlag(IS_ACTIVE);
    else ClearConStateFlag(IS_ACTIVE);
  }
  // #CAT_State update active status: is this an active connection group, with connections and an active projection?

  INLINE void           SetInactive()
  { ClearConStateFlag(IS_ACTIVE); }
  // #CAT_State set to inactive status

  INLINE void           CacheMemStart(NETWORK_STATE* net, int thr_no)
  { if(IsRecv()) {
      mem_start = net->ThrRecvConMem(thr_no) + mem_idx;
      cnmem_start = net->ThrRecvConMem(thr_no) + cnmem_idx;
    }
    else {
      mem_start = net->ThrSendConMem(thr_no) + mem_idx;
      cnmem_start = net->ThrSendConMem(thr_no) + cnmem_idx;
    }
  }
  // #IGNORE cache mem_start and cnmem_start pointers -- must be called on-device after all the ConState has been allocated
  
  INLINE void           CacheUnitLoHiIdxs(NETWORK_STATE* net, int thr_no)
  { if(size == 0) return;
    un_lo_idx = UnIdx(0);  un_hi_idx = UnIdx(size-1);
    for(int i=1; i < size-1; i++) {
      int dx = UnIdx(i);
      if(dx < un_lo_idx) un_lo_idx = dx;
      if(dx > un_hi_idx) un_hi_idx = dx;
    }
  }
  // #IGNORE optimize searching for unit connections by caching the low and high unit indexes for connections
  
  INLINE float*         MemBlock(int mem_block) const
  { return mem_start + alloc_size * mem_block; }
  // #IGNORE access given memory block -- just increments of alloc_size from mem_idx -- for low-level routines
  INLINE float*         CnMemBlock(int mem_block) const
  { return cnmem_start + alloc_size * mem_block; }
  // #IGNORE access given connection memory block -- just increments of alloc_size from cnmem_idx -- for low-level routines

  INLINE int            NConVars() const  { return n_con_vars; }
  // #CAT_Access number of connection-level variables

  INLINE bool           VarInRange(int var_no) const {
    if(var_no < 0 || var_no >= NConVars()) return false;
    return true;
  }
  // #CAT_Access is var number valid?

  INLINE int            OwnMemReq()
  { if(share_idx > 0) return alloc_size; return alloc_size * (NConVars() + 1); }
  // #IGNORE memory allocation requirements for con owner, in terms of numbers of float's/int32's
  INLINE int            PtrMemReq()
  { return alloc_size * 2; }
  // #IGNORE memory allocation requirements for con ptr, in terms of numbers of float's/int32's
  INLINE int            MemReq()
  { if(OwnCons()) return OwnMemReq(); return PtrMemReq(); }
  // #IGNORE memory allocation requirements for this connection group, in terms of numbers of float's/int32's

  INLINE void           SetMemStart(NETWORK_STATE* net, con_mem_idx midx) {
    mem_idx = midx;
    if(OwnCons()) {
      cnmem_idx = mem_idx + alloc_size;
    }
    ClearConStateFlag(SHARING);
    if(share_idx > 0) {
      if(share_idx >= own_flat_idx) {
        // taMisc::Error("SetMemStart: share_idx cannot be >= our own_flat_idx - can only share from units *earlier* in the layer (i.e., the first units in the layer)");
        return;
      }
      CON_STATE* shcg = SharedUnCons(net);
      if(!shcg) {
        // taMisc::Error("SetMemStart: did not find share con group");
        share_idx = -1;
        return;
      }
      cnmem_idx = shcg->cnmem_idx; // now we're sharing!
      SetConStateFlag(SHARING);
    }
  }
  // #IGNORE set our starting memory location and index -- called by Network Connect_Alloc routine

  INLINE int            VecChunkMod(int sz) 
  { return ((int)(sz / vec_chunk_targ) + 1) * vec_chunk_targ; }
  // #IGNORE return value that is modulus of vec_chunk_targ -- for computing allocation sizes, etc

  INLINE UNIT_STATE*    OwnUnState(NETWORK_STATE* net)
  { return net->GetUnitState(own_flat_idx); }
  // #IGNORE #CAT_Access our own unit variables -- for the unit that owns these connections (could be sending or recv unit depending on type of connection group)
  INLINE UNIT_STATE*    ThrOwnUnState(NETWORK_STATE* net, int thr_no)
  { return net->ThrUnitState(thr_no, own_thr_idx); }
  // #IGNORE #CAT_Access thread-optimized version (faster!): our own unit variables -- for the unit that owns these connections (could be sending or recv unit depending on type of connection group)

  INLINE float*         OwnCnVar(int var_no) const {
#ifdef DEBUG
    if(!OwnCons()) return NULL;
#endif
    return cnmem_start + (alloc_size * var_no); }
  // #CAT_Access fastest access (no range checking) to owned connection variable value -- get this float* and then index it directly with loop index -- var_no is defined in ConSpec (e.g., ConSpec::WT, DWT or algorithm-specific types (e.g., LeabraConSpec::SCALE)

  INLINE int32_t*       OwnCnVarInt(int var_no) const {
#ifdef DEBUG
    if(!OwnCons()) return NULL;
#endif
    return ((int32_t*)cnmem_start) + (alloc_size * var_no); }
  // #CAT_Access fastest access (no range checking) to owned connection variable value of INTEGER type -- get this float* and then index it directly with loop index -- var_no is defined in ConSpec

  INLINE float&          OwnCn(int idx, int var_no) const
  { return cnmem_start[(alloc_size * var_no) + idx]; }
  // #CAT_Access fast access (no range checking) to owned connection variable value at given index -- OwnCnVar with index in loop is preferred for fastest access -- var_no is defined in ConSpec (e.g., ConSpec::WT, DWT or algorithm-specific types (e.g., LeabraConSpec::PDW)

  INLINE int32_t&        OwnCnInt(int idx, int var_no) const
  { return ((int32_t*)cnmem_start)[(alloc_size * var_no) + idx]; }
  // #CAT_Access fast access (no range checking) to owned connection INTEGER variable value at given index -- OwnCnVar with index in loop is preferred for fastest access -- var_no is defined in ConSpec (e.g., ConSpec::WT, DWT or algorithm-specific types (e.g., LeabraConSpec::PDW)

  INLINE const int32_t& UnIdx(int idx) const 
  { return ((int32_t*)mem_start)[idx]; }
  // #CAT_Access fast access (no range checking) to unit flat index at given connection index
  INLINE int32_t&       UnIdx(int idx)
  { return ((int32_t*)mem_start)[idx]; }
  // #CAT_Access fast access (no range checking) to unit flat index at given connection index
  INLINE UNIT_STATE*    UnState(int idx, NETWORK_STATE* net) const {
    return net->GetUnitState(UnIdx(idx));
  }
  // #IGNORE #CAT_Access fast access (no range checking) to unit pointer at given connection index (goes through flat index at network level) -- this is the unit on the other end of this connection 
  INLINE UNIT_STATE*    SafeUnState(int idx, NETWORK_STATE* net) const {
    if(!InRange(idx)) return NULL;
    return net->GetUnitState(UnIdx(idx));
  }
  // #IGNORE #CAT_Access fast access (no range checking) to unit pointer at given connection index (goes through flat index at network level) -- this is the unit on the other end of this connection 

  INLINE CON_STATE*      UnCons(int idx, NETWORK_STATE* net) const {
    if(IsRecv()) return net->SendConState(UnIdx(idx), other_idx);
    else         return net->RecvConState(UnIdx(idx), other_idx);
  }
  // #IGNORE get ConState for this projection in unit at given index at other end of this connection
  INLINE CON_STATE*      SafeUnCons(int idx, NETWORK_STATE* net) const {
    if(!InRange(idx)) return NULL;
    if(IsRecv()) return net->SendConState(UnIdx(idx), other_idx);
    else         return net->RecvConState(UnIdx(idx), other_idx);
  }
  // #IGNORE get ConState for this projection in unit at given index at other end of this connection -- uses safe access
  INLINE CON_STATE*      SharedUnCons(NETWORK_STATE* net) const {
    PRJN_STATE* prjn = GetPrjnState(net);
    if(IsRecv()) return net->RecvConState(share_idx, prjn->recv_idx);
    else         return net->SendConState(share_idx, prjn->send_idx);
  }
  // #CAT_Access get the con group of the unit that we share connections from

  INLINE const int32_t& PtrCnIdx(int idx) const
  { return ((int32_t*)mem_start)[alloc_size + idx]; }
  // #CAT_Access fast access (no range checking) to index of connection within unit cons on other side of connection 
  INLINE int32_t&    PtrCnIdx(int idx)
  { return ((int32_t*)mem_start)[alloc_size + idx]; }
  // #CAT_Access fast access (no range checking) to index of connection within unit cons on other side of connection 

  INLINE float&  PtrCn(int idx, int var_no, NETWORK_STATE* net) const
  { return UnCons(idx, net)->OwnCn(PtrCnIdx(idx), var_no); }
  // #IGNORE #CAT_Access fast access (no range or own_cons checking) to connection value at given index -- this is MUCH slower than OwnCn due to several index accesses, so where ever possible computations should be performed on the side that owns the connections -- var_no is defined in ConSpec (e.g., ConSpec::WT, DWT or algorithm-specific types (e.g., LeabraConSpec::PDW)
  INLINE int32_t&  PtrCnInt(int idx, int var_no, NETWORK_STATE* net) const
  { return UnCons(idx, net)->OwnCnInt(PtrCnIdx(idx), var_no); }
  // #IGNORE #CAT_Access fast access (no range or own_cons checking) to connection INTEGER value at given index -- this is MUCH slower than OwnCn due to several index accesses, so where ever possible computations should be performed on the side that owns the connections -- var_no is defined in ConSpec (e.g., ConSpec::WT, DWT or algorithm-specific types (e.g., LeabraConSpec::PDW)

  INLINE float&  Cn(int idx, int var_no, NETWORK_STATE* net) const
  { if(OwnCons()) return OwnCn(idx, var_no); return PtrCn(idx, var_no, net); }
  // #IGNORE #CAT_Access generic access of connection variable value at given index, regardless of whether it is owned or a pointer -- no range checking -- var_no is defined in ConSpec (e.g., ConSpec::WT, DWT or algorithm-specific types (e.g., LeabraConSpec::PDW) -- do not use in compute algorithm code that knows the ownership status of the connections (use OwnCn* or PtrCn*)
  INLINE float&  SafeFastCn(int idx, int var_no, NETWORK_STATE* net) const
  { if(OwnCons()) { if(InRange(idx)) return OwnCn(idx, var_no); return const_cast<float&>(temp1); }
    CON_STATE* bc = SafeUnCons(idx, net);
    if(bc) return bc->SafeFastCn(PtrCnIdx(idx), var_no, net); return const_cast<float&>(temp1); }
  // #IGNORE #CAT_Access generic access of connection variable value at given index, regardless of whether it is owned or a pointer -- does range checking but doesn't issue messages, and is otherwise as fast as possible -- var_no is defined in ConSpec (e.g., ConSpec::WT, DWT or algorithm-specific types (e.g., LeabraConSpec::PDW) -- do not use in compute algorithm code that knows the ownership status of the connections (use OwnCn* or PtrCn*)

  INLINE void    FreeCons() {
    mem_start = 0;    cnmem_start = 0;    size = 0;    vec_chunked_size = 0;    alloc_size = 0;
    SetInactive();
  }
  // #CAT_State deallocate all connection-level storage (cons and units)
  INLINE void    RemoveAll()     { FreeCons(); }
  // #CAT_Modify remove all conections -- frees all associated memory
  INLINE void    Reset()         { FreeCons(); }
  // #CAT_Modify remove all conections -- frees all associated memory

  INIMPL int FindConFromIdx(int trg_idx) const;
  // #CAT_Acccess find connection from given unit flat index -- optimized search uses index relative to un_lo_idx, un_hi_idx as starting point for bidirectional search from there -- should be realtively quick to find connection in general

  INIMPL bool RemoveConIdx(int i, NETWORK_STATE* net);
  // #CAT_Modify remove connection at given index, also updating other unit's information about this connection

  INLINE bool RemoveConUn(int trg_idx, NETWORK_STATE* net) {
    int idx = FindConFromIdx(trg_idx);
    if(idx < 0) return false;
    return RemoveConIdx(idx, net);
  }
  // #CAT_Modify remove connection from given unit with trg_idx flat index number (if found)

  INLINE void Copy_Weights(const CON_STATE* src, NETWORK_STATE* net) {
    int mx = MIN(size, src->size);
    for(int i=0; i < mx; i++) {
      Cn(i, WT, net) = src->Cn(i, WT, net);
    }
  }
  // #CAT_Modify copies weights from other con_state

  INLINE void AllocCons(NETWORK_STATE* net, int sz) {
    if(mem_start != 0) {
      net->StateError("AllocCons: mem_start is not null -- re-allocating already allocated connection -- this is a programmer error in the ProjectionSpec, usually from not following make_cons flag");
      return;
    }
    if(sz < 0) {
      net->StateError("AllocCons: size is negative -- some kind of programming bug in projection spec");
      return;
    }
    mem_start = 0;
    cnmem_start = 0;
    size = 0;
    vec_chunked_size = 0;
    alloc_size = sz;
    SetInactive();
  }

  INLINE void AllocConsFmSize(NETWORK_STATE* net) {
    AllocCons(net, size);              // this sets size back to zero and does full alloc
  }
  // #CAT_Connect allocate connections from accumulated size during first pass of connection algo

  INLINE void    ConnectAllocInc(int inc_n = 1) { size += inc_n; }
  // #CAT_Connect use this for dynamically figuring out how many connections to allocate, if it is not possible to compute directly -- increments size by given number -- later call AllocConsFmSize to allocate connections based on the size value

  INIMPL int     ConnectUnOwnCn(NETWORK_STATE* net, UNIT_STATE* un, bool ignore_alloc_errs = false,
                                bool allow_null_unit = false);
  // #CAT_Connect add a new connection from given unit for OwnCons case -- returns -1 if no more room relative to alloc_size (flag will turn off err msg) -- default is to not allow connections from a unit with flat_idx = 0 (null_unit) but this can be overridden -- returns index of new connection (-1 if failed)

  INIMPL bool    ConnectUnPtrCn(NETWORK_STATE* net, UNIT_STATE* un, int con_idx, bool ignore_alloc_errs = false);
  // #CAT_Connect add a new connection from given unit and connection index for PtrCons case -- returns false if no more room, else true

  INIMPL int     ConnectUnits
  (NETWORK_STATE* net, UNIT_STATE* our_un, UNIT_STATE* oth_un, CON_STATE* oth_cons,
   bool ignore_alloc_errs = false,  bool set_init_wt = false, float init_wt = 0.0f);
  // #CAT_Connect add a new connection betwee our unit and an other unit and its appropriate cons -- does appropriate things depending on who owns the connects, etc.  enough room must already be allocated on both sides  (flag will turn off err msg) -- returns index of new connection (-1 if failed) -- can also optionally set initial weight value

  INIMPL bool   SetShareFrom(NETWORK_STATE* net, UNIT_STATE* shu);
  // #CAT_Access set this connection group to share from given other unit -- checks to make sure this works -- returns false if not (will have already emitted warning message)

  INIMPL void   FixConPtrs_SendOwns(NETWORK_STATE* net, int st_idx);
  // #IGNORE only for sending cons that own the connections: fix all the pointer connections to our connections to be correct -- called after reorganizing the order of connections within this group
  INIMPL void   FixConPtrs_RecvOwns(NETWORK_STATE* net, int st_idx);
  // #IGNORE only for sending cons that own the connections: fix all the pointer connections to our connections to be correct -- called after reorganizing the order of connections within this group

  INLINE float  VecChunkPct()
  {  if(size > 0) return (float)vec_chunked_size / (float)size; return 0.0f; }
  // #CAT_State return percent of our cons that are vec chunked

  INIMPL void   VecChunk_SendOwns(NETWORK_STATE* net, int* tmp_chunks, int* tmp_not_chunks,
                                 float* tmp_con_mem);
  // #IGNORE chunks the connections in vectorizable units, for sender own case -- pass in our sending own unit, and temp scratch memory guaranteed to be >= alloc_size for doing the reorganization
  INIMPL void   VecChunk_RecvOwns(NETWORK_STATE* net, int* tmp_chunks, int* tmp_not_chunks,
                                 float* tmp_con_mem);
  // #IGNORE chunks the connections in vectorizable units, for recv own case -- pass in our recv own unit, and temp scratch memory guaranteed to be >= alloc_size for doing the reorganization

  INIMPL int    VecChunk_impl(int* tmp_chunks, int* tmp_not_chunks, float* tmp_con_mem);
  // #IGNORE impl -- returns first_change index for fixing con ptrs, and sets vec_chunked_size -- chunks the connections in vectorizable units, for sender own case -- gets our sending own unit, and temp scratch memory guaranteed to be >= alloc_size for doing the reorganization

  INIMPL float& SafeCn(NETWORK_STATE* net, int idx, int var_no) const;
  // #CAT_Access fully safe generic access of connection variable value at given index, regardless of whether it is owned or a pointer -- var_no is defined in ConSpec (e.g., ConSpec::WT, DWT or algorithm-specific types (e.g., LeabraConSpec::PDW) -- this is mainly for program access -- do not use in compute algorithm code that knows the ownership status of the connections (use OwnCn* or PtrCn*)
  INIMPL float& SafeCnName(NETWORK_STATE* net, int idx, const char* var_nm) const;
  // #CAT_Access generic safe access of connection variable value by name (e.g., wt, dwt, pdw, etc) at given index, regardless of whether it is owned or a pointer -- mainly for program access -- do not use in compute algorithm code that knows the ownership status of the connections (use OwnCn* or PtrCn*)

  INIMPL bool   SetCnVal(NETWORK_STATE* net, float val, int idx, int var_no);
  // #CAT_Access set connection variable to given value -- for use by programs, which cannot assign the value through the SafeCn function -- var_no is defined in ConSpec (e.g., ConSpec::WT, DWT or algorithm-specific types (e.g., LeabraConSpec::PDW) -- 
  INIMPL bool   SetCnValName(NETWORK_STATE* net, float val, int idx, const char* var_nm);
  // #CAT_Access set connection variable (specified by name, e.g., wt, dwt, pdw) to given value -- for use by programs, which cannot assign the value through the SafeCn function 


  INLINE void Initialize_core
  (int own_flt_idx=0, int own_th_idx=0, int spec_dx=0, int flgs=0, int prj_dx=0,
   int n_con_vr = 0, int oth_idx=0) {
    own_flat_idx = own_flt_idx;    own_thr_idx = own_th_idx;    spec_idx = spec_dx;
    flags = (ConStateFlags)flgs;    prjn_idx = prj_dx;    alloc_size = 0;
    size = 0;    vec_chunked_size = 0;  other_idx = oth_idx;   n_con_vars = n_con_vr;
    un_lo_idx = -1; un_hi_idx = -1; share_idx = -1;
    mem_idx = 0;    cnmem_idx = 0;    mem_start = 0;  cnmem_start = 0; temp1 = 0.0f;
  }

// this is included directly in UnitState_cpp and UnitState_cuda -- must include PrjnState too!
// {
  enum ExtFlags {        // #BITS indicates type of external input; some flags used in Layer to control usage
    NO_EXTERNAL         = 0x0000, // #NO_BIT no input
    TARG                = 0x0001, // a target value used to train the network (value goes in targ field of unit)
    EXT                 = 0x0002, // an external input value that drives activations (value goes in ext field of unit)
    COMP                = 0x0004, // a comparison value used for computing satistics but not training the network (value goes in targ field of unit)
    TARG_EXT            = 0x0003, // #NO_BIT as both external input and target value
    COMP_TARG           = 0x0005, // #NO_BIT as a comparision and target layer
    COMP_EXT            = 0x0006, // #NO_BIT as a comparison and external input layer
    COMP_TARG_EXT       = 0x0007, // #NO_BIT as a comparison, target, and external input layer
    LESIONED            = 0x0010, // #READ_ONLY unit is temporarily lesioned (inactivated for all network-level processing functions) -- copied from Unit -- should not be set directly here
    UN_FLAG_1           = 0x0100, // #READ_ONLY misc unit-level flag for use by algorithms to set binary state (e.g., used in backprop to mark units that have been dropped)
    UN_FLAG_2           = 0x0200, // #READ_ONLY misc unit-level flag for use by algorithms to set binary state
    UN_FLAG_3           = 0x0400, // #READ_ONLY misc unit-level flag for use by algorithms to set binary state
    UN_FLAG_4           = 0x0800, // #READ_ONLY misc unit-level flag for use by algorithms to set binary state
  };

  int           flat_idx;
  // #READ_ONLY #HIDDEN #NO_COPY #NO_SAVE #CAT_State index of this unit in a flat array of unit state -- 0 is special null case -- real idx's start at 1
  int           lay_un_idx;
  // #READ_ONLY #HIDDEN #NO_COPY #NO_SAVE #CAT_State index of this unit in owning layer ("leaf index") -- index in flat list of units irrespective of group structure
  int		gp_idx;
  // #READ_ONLY #HIDDEN #NO_COPY #NO_SAVE #CAT_State index of the sub-unit group this unit belongs in, if this unit belongs in a unit group (always -1 if no unit groups) -- same as layer_gp_idx of owning unit gp
  int           ungp_un_idx;
  // #READ_ONLY #HIDDEN #NO_COPY #NO_SAVE #CAT_State index of unit within owning unit group -- either for a sub-unit-group (at gp_idx) if layer has them, or within master layer units group
  int           thread_no;
  // #READ_ONLY #HIDDEN #NO_COPY #NO_SAVE #CAT_State which thread we live on
  int           thr_un_idx;
  // #READ_ONLY #HIDDEN #NO_COPY #NO_SAVE #CAT_State thread-based unit index where this unit state lives
  int           own_lay_idx;
  // #READ_ONLY #HIDDEN #NO_COPY #NO_SAVE #CAT_State index of layer that we live in
  int           own_ungp_idx;
  // #READ_ONLY #HIDDEN #NO_COPY #NO_SAVE #CAT_State index of unitgroup that we live in within networkstate master list of unit groups (either a sub unit group or the master layer units group)
  int           spec_idx;
  // #READ_ONLY #HIDDEN #NO_COPY #NO_SAVE #CAT_State index of unit spec in NetworkState
  int           pos_x;
  // #NO_COPY #NO_SAVE #CAT_State structural position within group (if unit groups used) or layer -- can also use GetGp/UnXY methods to get structural coordinates based on layer geometry
  int           pos_y;
  // #NO_COPY #NO_SAVE #CAT_State structural position within group (if unit groups used) or layer -- can also use GetGp/UnXY methods to get structural coordinates based on layer geometry
  int           disp_pos_x;
  // #NO_COPY #NO_SAVE #CAT_State display position within layer (not unit group) -- requires special non-optimized display style if not default position
  int           disp_pos_y;
  // #NO_COPY #NO_SAVE #CAT_State display position within layer (not unit group) -- requires special non-optimized display style if not default position
  float         tmp_calc1;
  // #NO_SAVE #READ_ONLY #HIDDEN #CAT_Statistic temporary calculation variable (used for computing wt_prjn and prossibly other things)
  ExtFlags      ext_flag;
  // #GUI_READ_ONLY #SHOW #CAT_Activation tells what kind of external input unit received -- this is normally set by the ApplyInputData function -- it is not to be manipulated directly
  float         targ;
  // #VIEW_HOT #CAT_Activation target value: drives learning to produce this activation value
  float         ext;
  // #VIEW_HOT #CAT_Activation external input: drives activation of unit from outside influences (e.g., sensory input)
  float         snap;
  // #NO_SAVE #CAT_Statistic current snapshot value, as computed by the Snapshot function -- this can be displayed as a border around the units in the netview
  float         wt_prjn;
  // #NO_SAVE #CAT_Statistic weight projection value -- computed by Network::ProjectUnitWeights (triggered in GUI by setting wt prjn variable in netview control panel to point to a layer instead of NULL) -- represents weight values projected through any intervening layers from source unit (selected unit in netview or passed to ProjectUnitWeights function directly)
  float         act;
  // #VIEW_HOT #CAT_Activation activation value -- what the unit communicates to others
  float         net;
  // #VIEW_HOT #CAT_Activation net input value -- what the unit receives from others (typically sum of sending activations times the weights)
  float         bias_wt;
  // #VIEW_HOT #CAT_Bias bias weight value -- the bias weight acts like a connection from a unit that is always active with a constant value of 1 -- reflects intrinsic excitability from a biological perspective
  float         bias_dwt;
  // #VIEW_HOT #CAT_Bias change in bias weight value as computed by a learning mechanism

  INLINE void   SetExtFlag(int flg)   { ext_flag = (ExtFlags)(ext_flag | flg); }
  // set flag state on
  INLINE void   ClearExtFlag(int flg) { ext_flag = (ExtFlags)(ext_flag & ~flg); }
  // clear flag state (set off)
  INLINE bool   HasExtFlag(int flg) const { return (ext_flag & flg); }
  // check if flag is set
  INLINE void   SetExtFlagState(int flg, bool on)
  { if(on) SetExtFlag(flg); else ClearExtFlag(flg); }
  // set flag state according to on bool (if true, set flag, if false, clear it)

  INLINE bool   lesioned() const { return HasExtFlag(LESIONED); }
  // #CAT_State check if this unit is lesioned -- must check for all processing functions (threaded calls automatically exclude lesioned units)
  INLINE void   Lesion() { SetExtFlag(LESIONED); }
  // #CAT_State lesion this unit
  INLINE void   UnLesion() { ClearExtFlag(LESIONED); }
  // #CAT_State un-lesion this unit

  INLINE UNIT_SPEC* GetUnitSpec(NETWORK_STATE* nnet) const {
    return nnet->GetUnitSpec(spec_idx);
  }
  // #CAT_State get our unit spec
  INLINE LAYER_STATE* GetOwnLayer(NETWORK_STATE* nnet) const {
    return nnet->GetLayerState(own_lay_idx);
  }
  // #CAT_State get the layer state that owns us
  INLINE UNGP_STATE* GetOwnUnGp(NETWORK_STATE* nnet) const {
    return nnet->GetUnGpState(own_ungp_idx);
  }
  // #CAT_State get the unit group state that owns us

  INLINE bool  InSubGp() const { return (gp_idx >= 0); }
  // #CAT_State returns true if this unit lives within a sub-unit-group of layer -- otherwise no groups and is just within overall layer group

  INIMPL void  GetUnXY(NETWORK_STATE* nnet, int& un_x, int& un_y) const;
  // #CAT_State get unit X,Y from our unit index within a unit group -- use this for structural coordinates, not pos_x,y
  INIMPL void  GetUnFlatXY(NETWORK_STATE* nnet, int& un_x, int& un_y) const;
  // #CAT_State get unit X,Y from our unit index within overall layer (flat index) -- use this for structural coordinates, not pos_x,y
  INIMPL void  GetGpXY(NETWORK_STATE* nnet, int& gp_x, int& gp_y) const;
  // #CAT_State get group X,Y from our group index within the layer -- use this for structural coordinates, not pos_x,y
  INIMPL void  GetGpUnXY(NETWORK_STATE* nnet, int& gp_x, int& gp_y, int& un_x, int& un_y) const;
  // #CAT_State get group X,Y and unit X,Y coordinates from our overall unit index within the layer -- use this for structural coordinates, not pos_x,y

  INLINE int            NRecvConGps(NETWORK_STATE* nnet) const
  { return nnet->ThrUnNRecvConGps(thread_no, thr_un_idx); }
  // #CAT_Structure get number of receiving connection states (determined by number of active layer projections at time of build)
  INLINE int            NSendConGps(NETWORK_STATE* nnet) const
  { return nnet->ThrUnNSendConGps(thread_no, thr_un_idx); }
  // #CAT_Structure get number of sending connection states (determined by number of active layer send_prjns at time of build)
  INLINE CON_STATE*     RecvConState(NETWORK_STATE* nnet, int rcg_idx) const
  { return nnet->ThrUnRecvConState(thread_no, thr_un_idx, rcg_idx); }
  // #CAT_Structure get receiving connection state at given index -- no safe range checking is applied to rcg_idx!
  INLINE CON_STATE*     SendConState(NETWORK_STATE* nnet, int scg_idx) const
  { return nnet->ThrUnSendConState(thread_no, thr_un_idx, scg_idx); }
  // #CAT_Structure get sendingconnection state at given index -- no safe range checking is applied to scg_idx!
  INLINE CON_STATE*     RecvConStateSafe(NETWORK_STATE* nnet, int rcg_idx) const {
    int nrcg = NRecvConGps(nnet);
    if(rcg_idx < 0 || rcg_idx >= nrcg) {
      nnet->StateErrorVals("RecvConStateSafe: rcg_idx out of range", "rcg_idx", rcg_idx); return NULL; }
    return nnet->ThrUnRecvConState(thread_no, thr_un_idx, rcg_idx); }
  // #CAT_Structure get receiving connection state at given index
  INLINE CON_STATE*     SendConStateSafe(NETWORK_STATE* nnet, int scg_idx) const {
    int nscg = NSendConGps(nnet);
    if(scg_idx < 0 || scg_idx >= nscg) {
      nnet->StateErrorVals("SendConStateSafe: scg_idx out of range", "scg_idx", scg_idx); return NULL; }
    return nnet->ThrUnSendConState(thread_no, thr_un_idx, scg_idx); }
  // #CAT_Structure get sending connection state at given index
  INLINE CON_STATE*     RecvConStatePrjn(NETWORK_STATE* nnet, PRJN_STATE* prjn) const
  { return RecvConStateSafe(nnet, prjn->recv_idx); }
  // #CAT_Structure get con state at given prjn->recv_idx -- if it is not in range, emits error message and returns NULL
  INLINE CON_STATE*     SendConStatePrjn(NETWORK_STATE* nnet, PRJN_STATE* prjn) const
  { return SendConStateSafe(nnet, prjn->send_idx); }
  // #CAT_Structure get con state at given prjn->send_idx -- if it is not in range, emits error message and returns NULL

  INIMPL CON_STATE*     FindRecvConStateFrom(NETWORK_STATE* nnet, LAYER_STATE* fm_lay) const;
  // #CAT_State get receiving connection state from given sending layer
  INIMPL CON_STATE*     FindSendConStateTo(NETWORK_STATE* nnet, LAYER_STATE* to_lay) const;
  // #CAT_State get sending connection state from given sending layer
  INIMPL CON_STATE*     FindRecvConStateFromName(NETWORK_STATE* nnet, const char* fm_layer_nm) const;
  // #CAT_State get receiving connection state from given sending layer name
  INIMPL CON_STATE*     FindSendConStateToName(NETWORK_STATE* nnet, const char* to_layer_nm) const;
  // #CAT_State get sending connection state to given receiving layer name


  //////////////////////////////////////////////////////////////////////////////////
  //            Connection API

  INLINE void  RecvConsPreAlloc(NETWORK_STATE* nnet, PRJN_STATE* prjn, int no)
  { CON_STATE* cgp = RecvConStatePrjn(nnet, prjn); if(cgp) cgp->AllocCons(nnet, no); }
  // #CAT_State pre-allocate given no of receiving connections -- sufficient connections must be allocated in advance of making specific connections
  INLINE void  SendConsPreAlloc(NETWORK_STATE* nnet, PRJN_STATE* prjn, int no)
  { CON_STATE* cgp = SendConStatePrjn(nnet, prjn); if(cgp) cgp->AllocCons(nnet, no); }
  // #CAT_State pre-allocate given no of sending connections -- sufficient connections must be allocated in advance of making specific connections
  INLINE void  RecvConsAllocInc(NETWORK_STATE* nnet, PRJN_STATE* prjn, int no)
  { CON_STATE* cgp = RecvConStatePrjn(nnet, prjn); if(cgp) cgp->ConnectAllocInc(no); }
  // #CAT_State increment size by given no of recv connections -- later call RecvConsPostAlloc to actually allocate connections
  INLINE void  SendConsAllocInc(NETWORK_STATE* nnet, PRJN_STATE* prjn, int no)
  { CON_STATE* cgp = SendConStatePrjn(nnet, prjn); if(cgp) cgp->ConnectAllocInc(no); }
  // #CAT_State increment size by given no of sending connections -- later call SendConsPostAlloc to actually allocate connections
  INLINE void  RecvConsPostAlloc(NETWORK_STATE* nnet, PRJN_STATE* prjn)
  { CON_STATE* cgp = RecvConStatePrjn(nnet, prjn); if(cgp) cgp->AllocConsFmSize(nnet); }
  // #CAT_State post-allocate given no of recv connections (calls AllocConsFmSize on recv con state) -- if connections were initially made using the alloc_send = true, then this must be called to actually allocate connections -- then routine needs to call ConnectFrom again to make the connections
  INLINE void  SendConsPostAlloc(NETWORK_STATE* nnet, PRJN_STATE* prjn)
  { CON_STATE* cgp = SendConStatePrjn(nnet, prjn); if(cgp) cgp->AllocConsFmSize(nnet); }
  // #CAT_State post-allocate given no of sending connections (calls AllocConsFmSize on send con state) -- if connections were initially made using the alloc_send = true, then this must be called to actually allocate connections -- then routine needs to call ConnectFrom again to make the connections

  INLINE int   ConnectFrom
    (NETWORK_STATE* nnet, UNIT_STATE* su, PRJN_STATE* prjn, bool alloc_send = false,
     bool ignore_alloc_errs = false, bool set_init_wt = false,  float init_wt = 0.0f) {
    CON_STATE* rcgp = RecvConStatePrjn(nnet, prjn);     if(!rcgp) return -1;
    CON_STATE* scgp = su->SendConStatePrjn(nnet, prjn); if(!scgp) return -1;
    if(alloc_send) {  scgp->ConnectAllocInc(); return -1; }
    int con = rcgp->ConnectUnits(nnet, this, su, scgp, ignore_alloc_errs,
                                 set_init_wt, init_wt);
    return con;
  }    
  // #CAT_State make a recv connection from given unit to this unit using given projection -- requires both recv and sender to have sufficient connections allocated already, unless alloc_send is true, then it only allocates connections on the sender -- does NOT make any connection on the receiver -- use this in a loop that runs connections twice, with first pass as allocation (then call SendConstPostAlloc) and second pass as actual connection making -- return val is index of recv connection -- can also optionally set initial weight value

  INLINE int   ConnectFromCk
    (NETWORK_STATE* nnet, UNIT_STATE* su, PRJN_STATE* prjn, bool ignore_alloc_errs = false,
     bool set_init_wt = false, float init_wt = 0.0f) {
    CON_STATE* rcgp = RecvConStatePrjn(nnet, prjn);     if(!rcgp) return -1;
    CON_STATE* scgp = su->SendConStatePrjn(nnet, prjn); if(!scgp) return -1;
    if(rcgp->FindConFromIdx(su->flat_idx) >= 0) // already connected!
      return -1;
    int con = rcgp->ConnectUnits(nnet, this, su, scgp, ignore_alloc_errs,
                                 set_init_wt, init_wt);
    return con;
  }
  // #CAT_State does ConnectFrom but checks for an existing connection to prevent double-connections -- note that this is expensive -- only use if there is a risk of multiple connections.  This does not support alloc_send option -- can call in 2nd pass if needed -- return val is index of recv connection -- can also optionally set initial weight value
  
  INLINE bool  DisConnectFrom(NETWORK_STATE* nnet, UNIT_STATE* su, PRJN_STATE* prjn=NULL) {
    if(!prjn) { prjn = nnet->FindRecvPrjnFrom(GetOwnLayer(nnet), su->GetOwnLayer(nnet)); 
      if(!prjn) return false; }
    CON_STATE* rcgp = RecvConStatePrjn(nnet, prjn);       if(!rcgp) return false;
    CON_STATE* scgp = su->SendConStatePrjn(nnet, prjn);   if(!scgp) return false;
    rcgp->RemoveConUn(su->flat_idx, nnet);
    return scgp->RemoveConUn(su->flat_idx, nnet);
  }
  // #CAT_State remove connection from given unit (projection is optional)
  
  INLINE void  DisConnectAll(NETWORK_STATE* nnet) {
    CON_STATE* recv_gp;  CON_STATE* send_gp;
    const int rsz = NRecvConGps(nnet);
    for(int g=0; g<rsz; g++) {
      recv_gp = RecvConState(nnet, g);
      for(int i=recv_gp->size-1; i>=0; i--) {
        UnitState_cpp* su = recv_gp->UnState(i,nnet);
        if(recv_gp->other_idx >= 0)
          send_gp = su->SendConState(nnet, recv_gp->other_idx);
        else
          send_gp = NULL;
        if(send_gp)
          send_gp->RemoveConUn(su->flat_idx, nnet);
        recv_gp->RemoveConIdx(i, nnet);
      }
    }
    const int ssz = NSendConGps(nnet);
    for(int g=0; g<ssz; g++) { // the removes cause the leaf_gp to crash..
      send_gp = SendConState(nnet, g);
      for(int i=send_gp->size-1; i>=0; i--) {
        UnitState_cpp* ru = send_gp->UnState(i, nnet);
        if(send_gp->other_idx >= 0)
          recv_gp = ru->RecvConState(nnet, send_gp->other_idx);
        else
          recv_gp = NULL;
        if(recv_gp)
          recv_gp->RemoveConUn(ru->flat_idx, nnet);
        send_gp->RemoveConIdx(i, nnet);
      }
    }
  }
  // #CAT_State disconnect unit from all other units
  
  INLINE void   CountCons(NETWORK_STATE* nnet, int& n_recv, int& n_send) {
    const int rsz = NRecvConGps(nnet);
    for(int g = 0; g < rsz; g++) {
      CON_STATE* cg = RecvConState(nnet, g);
      if(cg->NotActive()) continue;
      n_recv += cg->size;
    }
    const int ssz = NSendConGps(nnet);
    for(int g = 0; g < ssz; g++) {
      CON_STATE* cg = SendConState(nnet, g);
      if(cg->NotActive()) continue;
      n_send += cg->size;
    }
  }    
  // #CAT_State count total number recv, sending connections
  
  INLINE void  UpdtActiveCons(NETWORK_STATE* nnet) {
    if(lesioned()) {
      const int rsz = NRecvConGps(nnet);
      for(int g = 0; g < rsz; g++) {
        CON_STATE* cg = RecvConState(nnet, g);
        cg->SetInactive();
      }
      const int ssz = NSendConGps(nnet);
      for(int g = 0; g < ssz; g++) {
        CON_STATE* cg = SendConState(nnet, g);
        cg->SetInactive();
      }
    }
    else {
      const int rsz = NRecvConGps(nnet);
      for(int g = 0; g < rsz; g++) {
        CON_STATE* cg = RecvConState(nnet, g);
        cg->UpdtIsActive(nnet);
      }
      const int ssz = NSendConGps(nnet);
      for(int g = 0; g < ssz; g++) {
        CON_STATE* cg = SendConState(nnet, g);
        cg->UpdtIsActive(nnet);
      }
    }
  }
  // #CAT_State update the active state of all connection states

  INLINE bool  ShareRecvConsFrom(NETWORK_STATE* nnet, UNIT_STATE* shu, PRJN_STATE* prjn) {
    CON_STATE* rcgp = RecvConStatePrjn(nnet, prjn);
    if(!rcgp) return false;
    return rcgp->SetShareFrom(nnet, shu);
  }
  // #CAT_State share our receiving connection state connection memory for given projection from given other source unit -- shu must appear prior to this layer in the same layer

  INIMPL void  Copy_Weights(NETWORK_STATE* nnet, PRJN_STATE* prjn, const UNIT_STATE* src,
                            PRJN_STATE* src_prjn);
  // #CAT_ObjectMgmt copies weights from other unit (incl wts assoc with unit bias member) for given projections on each unit
  
  INLINE void Initialize_core(int fl_dx=0, int layu_dx=0, int gp_dx=0, int ugpu_dx=0, int thr_no=0,
                              int thr_dx=0, int lay_dx=0, int ugp_dx=0, int spec_dx=0) {
    flat_idx = fl_dx; lay_un_idx = layu_dx;  gp_idx = gp_dx; ungp_un_idx = ugpu_dx;
    thread_no = thr_no; thr_un_idx = thr_dx; own_lay_idx = lay_dx; own_ungp_idx = ugp_dx;
    spec_idx = spec_dx;  ext_flag = NO_EXTERNAL;
    targ = ext = act = net = bias_wt = bias_dwt = wt_prjn = snap = 0.0f;
    pos_x = pos_y = 0; disp_pos_x = disp_pos_y = 0;
  }
  // #IGNORE

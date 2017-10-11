// this is included directly in UnitState_cpp and UnitState_cuda
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
  // #READ_ONLY #HIDDEN #NO_COPY #NO_SAVE #CAT_State index of this unit in owning layer ("leaf index")
  int		gp_idx;
  // #READ_ONLY #HIDDEN #NO_COPY #NO_SAVE #CAT_State index of the unit group this unit belongs in, if this unit belongs in a unit group (either virtual or real) -- assigned at build by layer
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
  ExtFlags      ext_flag;
  // #GUI_READ_ONLY #SHOW #CAT_Activation tells what kind of external input unit received -- this is normally set by the ApplyInputData function -- it is not to be manipulated directly
  float         targ;
  // #VIEW_HOT #CAT_Activation target value: drives learning to produce this activation value
  float         ext;
  // #VIEW_HOT #CAT_Activation external input: drives activation of unit from outside influences (e.g., sensory input)
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
  // check if this unit is lesioned -- must check for all processing functions (threaded calls automatically exclude lesioned units)


  INLINE UNIT_SPEC* GetUnitSpec(NETWORK_STATE* nnet) {
    return nnet->GetUnitSpec(spec_idx);
  }
  // get our unit spec
  INLINE LAYER_STATE* GetOwnLayer(NETWORK_STATE* nnet) {
    return nnet->GetLayerState(own_lay_idx);
  }
  // get the layer state that owns us
  INLINE UNGP_STATE* GetOwnUnGp(NETWORK_STATE* nnet) {
    return nnet->GetUnGpState(own_ungp_idx);
  }
  // get the unit group state that owns us

  INLINE int            NRecvConGps(NETWORK_STATE* nnet) const
  { return nnet->ThrUnNRecvConGps(thread_no, thr_un_idx); }
  // #IGNORE #CAT_Structure get number of receiving connection groups (determined by number of active layer projections at time of build)
  INLINE int            NSendConGps(NETWORK_STATE* nnet) const
  { return nnet->ThrUnNSendConGps(thread_no, thr_un_idx); }
  // #IGNORE #CAT_Structure get number of sending connection groups (determined by number of active layer send_prjns at time of build)
  // int                   NRecvConGpsSafe(NETWORK_STATE* nnet) const;
  // // #CAT_Structure get number of receiving connection groups (determined by number of active layer projections at time of build)
  // int                   NSendConGpsSafe(NETWORK_STATE* nnet) const;
  // // #CAT_Structure get number of sending connection groups (determined by number of active layer send_prjns at time of build)
  INLINE CON_STATE*     RecvConState(NETWORK_STATE* nnet, int rcg_idx) const
  { return nnet->ThrUnRecvConState(thread_no, thr_un_idx, rcg_idx); }
  // #IGNORE #CAT_Structure get receiving connection group at given index -- no safe range checking is applied to rcg_idx!
  INLINE CON_STATE*     SendConState(NETWORK_STATE* nnet, int scg_idx) const
  {   return nnet->ThrUnSendConState(thread_no, thr_un_idx, scg_idx); }
  // #IGNORE #CAT_Structure get sendingconnection group at given index -- no safe range checking is applied to scg_idx!
  // INLINE CON_STATE*     RecvConStateSafe(NETWORK_STATE* nnet, int rcg_idx) const;
  // // #CAT_Structure get receiving connection group at given index
  // INLINE CON_STATE*     SendConStateSafe(NETWORK_STATE* nnet, int scg_idx) const;
  // // #CAT_Structure get sendingconnection group at given index
  // INLINE CON_STATE*      RecvConStatePrjn(NETWORK_STATE* nnet, Projection* prjn) const;
  // // #IGNORE #CAT_Structure get con group at given prjn->recv_idx -- if it is not in range, emits error message and returns NULL
  // INLINE CON_STATE*      SendConStatePrjn(NETWORK_STATE* nnet, Projection* prjn) const;
  // // #IGNORE #CAT_Structure get con group at given prjn->send_idx -- if it is not in range, emits error message and returns NULL

  INLINE void Initialize_core(int fl_dx=0, int layu_dx=0, int gp_dx=0, int ugpu_dx=0, int thr_no=0,
                              int thr_dx=0, int lay_dx=0, int ugp_dx=0, int spec_dx=0) {
    flat_idx = fl_dx; lay_un_idx = layu_dx;  gp_idx = gp_dx; ungp_un_idx = ugpu_dx;
    thread_no = thr_no; thr_un_idx = thr_dx; own_lay_idx = lay_dx; own_ungp_idx = ugp_dx;
    spec_idx = spec_dx;  ext_flag = NO_EXTERNAL;
    targ = ext = act = net = bias_wt = bias_dwt = 0.0f;
  }
  // #IGNORE

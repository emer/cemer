// this is included in Layer and LayerState -- common state between two
// {
  enum LayerType {      // type of layer, used to determine various default settings
    HIDDEN,             // layer does not receive external input of any form
    INPUT,              // layer receives external input (EXT) that drives activation states directly
    TARGET,             // layer receives a target input (TARG) that determines correct activation states, used for training
    OUTPUT,             // layer produces a visible output response but is not a target.  any external input serves as a comparison (COMP) against current activations.
  };

  enum LayerFlags {                     // #BITS flags for layer
    LF_NONE             = 0,            // #NO_BIT
    LESIONED            = 0x0001,       // #READ_ONLY this layer is temporarily lesioned (inactivated for all network-level processing functions) -- IMPORTANT: use the Lesion and UnLesion functions to set this flag -- they provide proper updating after changes -- otherwise network dynamics will be wrong and the display will not be properly updated
    ICONIFIED           = 0x0002,       // only display a single unit showing icon_value (set in algorithm-specific manner)
    NO_ADD_SSE          = 0x0004,       // do NOT add this layer's sse value (sum squared error) to the overall network sse value: this is for all types of SSE computed for ext_flag = TARG (layer_type = TARGET) or ext_flag = COMP (layer_type = OUTPUT) layers
    NO_ADD_COMP_SSE     = 0x0008,       // do NOT add this layer's sse value (sum squared error) to the overall network sse value: ONLY for ext_flag = COMP (OUTPUT) flag settings (NO_ADD_SSE blocks all contributions) -- this is relevant if the layer type or ext_flags are switched dynamically and only TARGET errors are relevant
    SAVE_UNIT_NAMES     = 0x0010,       // save the names for individual units in the unit_names matrix on this layer (the Units themselves are never saved) -- when the network is built, these names are then assigned to the units -- use SetUnitNames method to update unit names from unit_names matrix if you've changed them, and GetUnitNames to save current unit names into unit_names matrix
    PROJECT_WTS_NEXT    = 0x0020,       // #HIDDEN this layer is next in line for weight projection operation
    PROJECT_WTS_DONE    = 0x0040,       // #HIDDEN this layer is done with weight projection operation (prevents loops)
  };

  // note: must keep syncrhonized with UnitState
  enum ExtFlags {        // #BITS indicates type of external input; some flags used in Layer to control usage
    NO_EXTERNAL         = 0x0000, // #NO_BIT no input
    TARG                = 0x0001, // a target value used to train the network (value goes in targ field of unit)
    EXT                 = 0x0002, // an external input value that drives activations (value goes in ext field of unit)
    COMP                = 0x0004, // a comparison value used for computing satistics but not training the network (value goes in targ field of unit)
    TARG_EXT            = 0x0003, // #NO_BIT as both external input and target value
    COMP_TARG           = 0x0005, // #NO_BIT as a comparision and target layer
    COMP_EXT            = 0x0006, // #NO_BIT as a comparison and external input layer
    COMP_TARG_EXT       = 0x0007, // #NO_BIT as a comparison, target, and external input layer
  };

  enum AccessMode {     // how to access the units in the layer -- only relevant for layers with unit groups (otherwise modes are the same)
    ACC_LAY,            // access as a single layer-wide set of units
    ACC_GP,             // access via their subgroup structure, with group and unit index values
  };

  bool                  main_obj;       // #NO_SAVE #READ_ONLY #CAT_State true if this is a main-side object (emergent, TA-enabled) as opposed to a State-side object 
  int                   layer_idx;      // #NO_SAVE #READ_ONLY index of this layer in the network state_layers list and NetworkState layers array -- -1 if not active..
  int                   laygp_lay0_idx; // #NO_SAVE #READ_ONLY index of first layer in the layer group that this layer belongs in -- the first layer takes the lead for whole group -- -1 if not part of a layer group
  int                   laygp_n;        // #NO_SAVE #READ_ONLY number of layers in the layer group -- set for all members of the group
  int                   units_flat_idx; // #NO_SAVE #READ_ONLY starting index for this layer into the network units_flat list, used in threading
  int                   ungp_idx;       // #NO_SAVE #READ_ONLY unit group index for this layerstate in the networkstate list of unit group state -- this is the main units unit group and subsequent sub-unit-groups are ordered after this one in master list
  int                   n_units;        // #NO_SAVE #READ_ONLY number of units in the layer
  int                   n_ungps;        // #NO_SAVE #READ_ONLY number of unit groups beyond main layer one -- 0 = no unit groups -- set during state build
  int                   prjn_start_idx; // #NO_SAVE #READ_ONLY starting index of recv prjns in list of projection states, -1 if none
  int                   spec_idx;       // #NO_SAVE #READ_ONLY layer spec index in list in NetworkState
  int                   unit_spec_idx;  // #NO_SAVE #READ_ONLY unit spec index in list in NetworkState
  int                   n_recv_prjns;   // #CAT_Structure #READ_ONLY number of active receiving projections
  int                   n_send_prjns;   // #CAT_Structure #READ_ONLY number of active sending projections
  LayerFlags            flags;          // #MAIN #CONDEDIT_ON_main_obj flags controlling various aspects of layer funcdtion
  LayerType             layer_type;     // #MAIN #CONDEDIT_ON_main_obj #CAT_Activation type of layer: determines default way that external inputs are presented, and helps with other automatic functions (e.g., wizards)
  ExtFlags              ext_flag;       // #MAIN #CONDEDIT_ON_main_obj #NO_SAVE #CAT_Activation #GUI_READ_ONLY #SHOW indicates which kind of external input layer received -- this is normally set by the ApplyInputData function -- it is not to be manipulated directly
  float                 sse;            // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic #VIEW sum squared error over the network, for the current external input pattern
  STATE_CLASS(Average)  avg_sse;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic #DMEM_AGG_SUM average sum squared error over an epoch or similar larger set of external input patterns
  float                 cnt_err;        // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic count of number of times the sum squared error was above cnt_err_tol over an epoch or similar larger set of external input patterns
  float                 cur_cnt_err;    // #NO_SAVE #READ_ONLY #DMEM_AGG_SUM #CAT_Statistic current cnt_err -- used for computing cnt_err
  float                 pct_err;        // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic epoch-wise average of count of number of times the sum squared error was above cnt_err_tol over an epoch or similar larger set of external input patterns (= cnt_err / n)
  float                 pct_cor;        // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic epoch-wise average of count of number of times the sum squared error was below cnt_err_tol over an epoch or similar larger set of external input patterns (= 1 - pct_err -- just for convenience for whichever you want to plot)
  STATE_CLASS(PRerrVals) prerr;          // #NO_SAVE #GUI_READ_ONLY #CAT_Statistic precision and recall error values for this layer, for the current pattern
  STATE_CLASS(PRerrVals) sum_prerr;      // #NO_SAVE #READ_ONLY #DMEM_AGG_SUM #CAT_Statistic precision and recall error values over an epoch or similar larger set of external input patterns -- these are always up-to-date as the system is aggregating, given the additive nature of the statistics
  STATE_CLASS(PRerrVals) epc_prerr;      // #NO_SAVE #GUI_READ_ONLY #SHOW #CONDSHOW_ON_stats.prerr #CAT_Statistic precision and recall error values over an epoch or similar larger set of external input patterns
  float                 icon_value;     // #NO_SAVE #GUI_READ_ONLY #HIDDEN #CAT_Statistic value to display if layer is iconified (algorithmically determined)
  
  INLINE void   SetLayerFlag(LayerFlags flg)   { flags = (LayerFlags)(flags | flg); }
  // set flag state on
  INLINE void   ClearLayerFlag(LayerFlags flg) { flags = (LayerFlags)(flags & ~flg); }
  // clear flag state (set off)
  INLINE bool   HasLayerFlag(LayerFlags flg) const { return (flags & flg); }
  // check if flag is set
  INLINE void   SetLayerFlagState(LayerFlags flg, bool on)
  { if(on) SetLayerFlag(flg); else ClearLayerFlag(flg); }
  // set flag state according to on bool (if true, set flag, if false, clear it)

  INLINE void   SetExtFlag(int flg)   { ext_flag = (ExtFlags)(ext_flag | flg); }
  // set flag state on
  INLINE void   ClearExtFlag(int flg) { ext_flag = (ExtFlags)(ext_flag & ~flg); }
  // clear flag state (set off)
  INLINE bool   HasExtFlag(int flg) const { return (ext_flag & flg); }
  // check if flag is set
  INLINE void   SetExtFlagState(int flg, bool on)
  { if(on) SetExtFlag(flg); else ClearExtFlag(flg); }
  // set flag state according to on bool (if true, set flag, if false, clear it)

  INLINE bool   lesioned() const { return HasLayerFlag(LESIONED); }
  // check if this layer is lesioned -- use in function calls
  INLINE bool   Iconified() const { return HasLayerFlag(ICONIFIED); }
  // convenience function for checking iconified flag

  INLINE LAYER_SPEC_CPP* GetLayerSpec(NETWORK_STATE* net) const
  { return net->GetLayerSpec(spec_idx); }
  // #CAT_Structure get the layer spec for this layer

  INLINE UNIT_SPEC_CPP* GetUnitSpec(NETWORK_STATE* net) const
  { return net->GetUnitSpec(unit_spec_idx); }
  // #CAT_Structure get the unit spec for this layer

  INLINE LAYER_STATE* GetLayerState(NETWORK_STATE* net) const
  { return net->GetLayerState(layer_idx); }
  // #CAT_Structure get the layer state for this layer

  INLINE UNGP_STATE* GetLayUnGpState(NETWORK_STATE* net) const
  { return net->GetUnGpState(ungp_idx); }
  // #CAT_Structure get the main unit group state for this layer

  INLINE UNGP_STATE* GetUnGpState(NETWORK_STATE* net, int ungp_no) const
  { if(ungp_no < 0 || ungp_no >= n_ungps) return NULL;
    return net->GetUnGpState(ungp_idx + 1 + ungp_no); }
  // #CAT_Structure get the sub-unit group state for given unit group number for this layer

  INLINE PRJN_STATE* GetPrjnState(NETWORK_STATE* net, int prjn_no) const
  { if(prjn_start_idx < 0) return NULL;
    return net->GetPrjnState(prjn_start_idx + prjn_no); }
  // #CAT_Structure get the unit group state for this layer

  INLINE PRJN_STATE* FindPrjnFrom(NETWORK_STATE* net, int send_lay_idx) const {
    if(prjn_start_idx < 0 || n_recv_prjns == 0) return NULL;
    for(int i = 0; i < n_recv_prjns; i++) {
      PRJN_STATE* prjn = net->GetPrjnState(prjn_start_idx + i);
      if(prjn->send_lay_idx == send_lay_idx) return prjn;
    }
    return NULL;
  }
  // #CAT_Structure find projection from given layer index -- NULL if not found

  INLINE LAYER_STATE* GetLayGpLayer(NETWORK_STATE* net, int lgp_idx) const
  { if(laygp_lay0_idx < 0) return NULL;
    return net->GetLayerState(laygp_lay0_idx + lgp_idx); }
  // #CAT_Structure get the given layer in the layer group this layer belongs in

  INLINE void  Init_Stats() {
    sse = 0.0f;    avg_sse.ResetAvg();    cnt_err = 0.0f;
    cur_cnt_err = 0.0f;    pct_err = 0.0f;    pct_cor = 0.0f;
    sum_prerr.InitVals();    epc_prerr.InitVals(); icon_value = 0.0f;
  }
  // #EXPERT #CAT_Statistic initialize statistic variables on layer -- called by Network Init_Stats

  INLINE void Initialize_lay_core
  (int lay_dx=0, int laygp_lay0_dx=0, int laygpn=0, int units_dx=0, int ungp_dx=0, int n_un=0,
   int n_gps=0, int prjn_st_dx=0, int spec_dx=0, int uspec_dx=0, int n_recv=0, int n_send=0,
   int lf=LF_NONE, LayerType lt=HIDDEN) {
    main_obj = false; layer_idx = lay_dx; laygp_lay0_idx=0; laygp_n=0;
    units_flat_idx = units_dx;  ungp_idx = ungp_dx; n_units = n_un; n_ungps = n_gps;
    prjn_start_idx = prjn_st_dx;  spec_idx = spec_dx; unit_spec_idx = uspec_dx; 
    n_recv_prjns = n_recv;    n_send_prjns = n_send;
    flags = (LayerFlags)lf;    layer_type = lt;    ext_flag = NO_EXTERNAL;
    Init_Stats();
  }
  // #IGNORE
  

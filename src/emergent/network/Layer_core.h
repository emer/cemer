// this is included in Layer and LayerState -- common state between two
//{
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
    UN_GEOM_NOT_XY      = 0x0080,       // #HIDDEN unit geometry n != x*y -- requires extra math
    LAY_FLAG_1          = 0x0100,       // misc layer flag 1
    LAY_FLAG_2          = 0x0200,       // misc layer flag 2
    LAY_FLAG_3          = 0x0400,       // misc layer flag 3
    LAY_FLAG_4          = 0x0800,       // misc layer flag 2
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

  static const int LAY_NAME_MAX_LEN=256; // #IGNORE max length of layer name

  bool                  main_obj;       // #CAT_State #NO_SAVE #READ_ONLY #CAT_State true if this is a main-side object (emergent, TA-enabled) as opposed to a State-side object
  char                  layer_name[LAY_NAME_MAX_LEN]; // #CAT_State #NO_SAVE #READ_ONLY name of this layer -- needed for loading and saving weights
  int                   layer_idx;      // #CAT_State #NO_SAVE #READ_ONLY index of this layer in the network state_layers list and NetworkState layers array -- -1 if not active..
  int                   laygp_lay0_idx; // #CAT_State #NO_SAVE #READ_ONLY index of first layer in the layer group that this layer belongs in -- the first layer takes the lead for whole group -- -1 if not part of a layer group
  int                   laygp_n;        // #CAT_State #NO_SAVE #READ_ONLY number of layers in the layer group -- set for all members of the group
  int                   units_flat_idx; // #CAT_State #NO_SAVE #READ_ONLY starting index for this layer into the network units_flat list, used in threading
  int                   ungp_idx;       // #CAT_State #NO_SAVE #READ_ONLY unit group index for this layerstate in the networkstate list of unit group state -- this is the main units unit group and subsequent sub-unit-groups are ordered after this one in master list
  int                   n_units;        // #CAT_State #NO_SAVE #READ_ONLY number of units in the layer
  int                   n_ungps;        // #CAT_State #NO_SAVE #READ_ONLY number of unit groups beyond main layer one -- 0 = no unit groups -- set during state build
  int                   prjn_start_idx; // #CAT_State #NO_SAVE #READ_ONLY starting index of recv prjns in list of projection states, -1 if none
  int                   send_prjn_start_idx; // #CAT_State #NO_SAVE #READ_ONLY starting index of sending prjns in list of sending projection indexes, -1 if none
  int                   spec_idx;       // #CAT_State #NO_SAVE #READ_ONLY layer spec index in list in NetworkState
  int                   unit_spec_idx;  // #CAT_State #NO_SAVE #READ_ONLY unit spec index in list in NetworkState
  int                   n_recv_prjns;   // #CAT_State #READ_ONLY number of active receiving projections
  int                   n_send_prjns;   // #CAT_State #READ_ONLY number of active sending projections
  int                   un_geom_x;      // #CAT_State #READ_ONLY two-dimensional layout and number of units within the layer or each unit group within the layer
  int                   un_geom_y;      // #CAT_State #READ_ONLY two-dimensional layout and number of units within the layer or each unit group within the layer
  int                   un_geom_n;      // #CAT_State #READ_ONLY two-dimensional layout and number of units within the layer or each unit group within the layer
  int                   gp_geom_x;      // #CAT_State #READ_ONLY two-dimensional layout and number of unit groups (akin to hypercolumns in cortex) within the layer -- if n_ungps > 0 -- un_geom is geometry of each such unit group 
  int                   gp_geom_y;      // #CAT_State #READ_ONLY two-dimensional layout and number of unit groups (akin to hypercolumns in cortex) within the layer -- if n_ungps > 0 -- un_geom is geometry of each such unit group 
  int                   gp_geom_n;      // #CAT_State #READ_ONLY two-dimensional layout and number of unit groups (akin to hypercolumns in cortex) within the layer -- if n_ungps > 0 -- un_geom is geometry of each such unit group 
  int                   flat_geom_x;    // #CAT_State #READ_ONLY overall flat net two-dimensional layout and number of units within the layer, multiplying un_geom * gp_geom
  int                   flat_geom_y;    // #CAT_State #READ_ONLY overall flat net two-dimensional layout and number of units within the layer, multiplying un_geom * gp_geom
  int                   flat_geom_n;    // #CAT_State #READ_ONLY overall flat net two-dimensional layout and number of units within the layer, multiplying un_geom * gp_geom
  int                   gp_spc_x;       // #CAT_State #READ_ONLY spacing between groups -- for display only
  int                   gp_spc_y;       // #CAT_State #READ_ONLY spacing between groups -- for display only
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
  
  INLINE void   SetLayerFlag(int flg)   { flags = (LayerFlags)(flags | flg); }
  // set flag state on
  INLINE void   ClearLayerFlag(int flg) { flags = (LayerFlags)(flags & ~flg); }
  // clear flag state (set off)
  INLINE bool   HasLayerFlag(int flg) const { return (flags & flg); }
  // check if flag is set
  INLINE void   SetLayerFlagState(int flg, bool on)
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

  INLINE void   SetLayerName(const char* lay_name) {
    strncpy(layer_name, lay_name, LAY_NAME_MAX_LEN-1);
    layer_name[LAY_NAME_MAX_LEN-1] = '\0'; // C is so lame!
  }
  // #CAT_State set layer name to given name
  INLINE bool   LayerNameIs(const char* lay_name) const {
    return (strncmp(layer_name, lay_name, LAYER_STATE::LAY_NAME_MAX_LEN) == 0);
  }
  // #CAT_State return true if that is the name of the layer
  INLINE bool   LayerNameContains(const char* lay_name) const {
    return (strstr(layer_name, lay_name) != NULL);
  }
  // #CAT_State return true if the layer name contains the given string

  INLINE LAYER_SPEC_CPP* GetLayerSpec(NETWORK_STATE* net) const
  { return net->GetLayerSpec(spec_idx); }
  // #CAT_State get the layer spec for this layer

  INLINE UNIT_SPEC_CPP* GetUnitSpec(NETWORK_STATE* net) const
  { return net->GetUnitSpec(unit_spec_idx); }
  // #CAT_State get the unit spec for this layer

  INLINE LAYER_STATE* GetLayerState(NETWORK_STATE* net) const
  { return net->GetLayerState(layer_idx); }
  // #CAT_State get the layer state for this layer

  INLINE UNGP_STATE* GetLayUnGpState(NETWORK_STATE* net) const
  { return net->GetUnGpState(ungp_idx); }
  // #CAT_State get the main unit group state for this layer

  INLINE bool HasUnitGroups() const { return (n_ungps > 0); }
  // #CAT_State does this layer have sub-unit groups

  INLINE UNGP_STATE* GetUnGpState(NETWORK_STATE* net, int ungp_no) const
  { if(ungp_no < 0 || ungp_no >= n_ungps) return NULL;
    return net->GetUnGpState(ungp_idx + 1 + ungp_no); }
  // #CAT_State get the sub-unit group state for given unit group number for this layer

  INLINE UNGP_STATE* GetUnGpStateXY(NETWORK_STATE* net, int gp_x, int gp_y) const
  { return GetUnGpState(net, gp_y * gp_geom_x + gp_x); }
  // #CAT_State get the sub-unit group state for given unit group X,Y coordinates


  INLINE PRJN_STATE* GetRecvPrjnState(NETWORK_STATE* net, int prjn_no) const
  { if(prjn_start_idx < 0 || prjn_no >= n_recv_prjns) return NULL;
    return net->GetPrjnState(prjn_start_idx + prjn_no); }
  // #CAT_State get the receiving projection state for given projection index within this layer (0 = first one for this layer, etc)
  INLINE PRJN_STATE* FindRecvPrjnFrom(NETWORK_STATE* net, int send_lay_idx) const {
    if(prjn_start_idx < 0 || n_recv_prjns == 0) return NULL;
    for(int i = 0; i < n_recv_prjns; i++) {
      PRJN_STATE* prjn = net->GetPrjnState(prjn_start_idx + i);
      if(prjn->send_lay_idx == send_lay_idx) return prjn;
    }
    return NULL;
  }
  // #CAT_State find receiving projection from given layer index -- NULL if not found
  INLINE PRJN_STATE* FindRecvPrjnFromLay(NETWORK_STATE* net, LAYER_STATE* send_lay) const
  { return FindRecvPrjnFrom(net, send_lay->layer_idx); }
  // #CAT_State find receiving projection from given sending layer -- NULL if not found
  INLINE PRJN_STATE* FindRecvPrjnFromName(NETWORK_STATE* net, const char* fm_layer_nm) const {
    if(prjn_start_idx < 0 || n_recv_prjns == 0) return NULL;
    for(int i = 0; i < n_recv_prjns; i++) {
      PRJN_STATE* prjn = net->GetPrjnState(prjn_start_idx + i);
      LAYER_STATE* fm = prjn->GetSendLayerState(net);
      if(fm->LayerNameIs(fm_layer_nm)) return prjn;
    }
    return NULL;
  }
  // #CAT_State find receiving projection from given layer name -- NULL if not found

  INLINE PRJN_STATE* GetSendPrjnState(NETWORK_STATE* net, int prjn_no) const
  { if(send_prjn_start_idx < 0 || prjn_no >= n_send_prjns) return NULL;
    return net->GetSendPrjnState(send_prjn_start_idx + prjn_no); }
  // #CAT_State get the sending projection state for given projection index within this layer (0 = first one for this layer, etc)
  INLINE PRJN_STATE* FindSendPrjnTo(NETWORK_STATE* net, int to_lay_idx) const {
    if(send_prjn_start_idx < 0 || n_send_prjns == 0) return NULL;
    for(int i = 0; i < n_send_prjns; i++) {
      PRJN_STATE* prjn = net->GetSendPrjnState(send_prjn_start_idx + i);
      if(prjn->recv_lay_idx == to_lay_idx) return prjn;
    }
    return NULL;
  }
  // #CAT_State find sending projection to given layer index -- NULL if not found
  INLINE PRJN_STATE* FindSendPrjnToLay(NETWORK_STATE* net, LAYER_STATE* to_lay) const
  { return FindSendPrjnTo(net, to_lay->layer_idx); }
  // #CAT_State find sending projection to given layer -- NULL if not found

  INLINE PRJN_STATE* FindSendPrjnToName(NETWORK_STATE* net, const char* to_layer_nm) const {
    if(send_prjn_start_idx < 0 || n_send_prjns == 0) return NULL;
    for(int i = 0; i < n_send_prjns; i++) {
      PRJN_STATE* prjn = net->GetPrjnState(send_prjn_start_idx + i);
      LAYER_STATE* tol = prjn->GetRecvLayerState(net);
      if(tol->LayerNameIs(to_layer_nm)) return prjn;
    }
    return NULL;
  }
  // #CAT_State find sending projection to given layer name -- NULL if not found


  INLINE LAYER_STATE* GetLayGpLayer(NETWORK_STATE* net, int lgp_idx) const
  { if(laygp_lay0_idx < 0) return NULL;
    return net->GetLayerState(laygp_lay0_idx + lgp_idx); }
  // #CAT_State get the given layer in the layer group this layer belongs in


  /////////////////////////////////////////////////////////////////
  //            UnitState access
  
  INLINE bool FlatUnIdxInRange(int un_no) const
  { return (un_no >= 0 && un_no < n_units); }
  // #CAT_State is flat unit index (for full set of units in layer, regardless of sub unit groups) in range (0 <= idx < n_units)?
    
  INLINE bool UnIdxInRange(int un_no) const
  { return (un_no >= 0 && un_no < un_geom_n); }
  // #CAT_State is unit index in range according to un_geom_n (0 <= idx < un_geom_n) -- valid for units without sub unit groups or for accessing units within unit group
    
  INLINE bool GpIdxInRange(int gp_no) const
  { return (gp_no >= 0 && gp_no < n_ungps); }
  // #CAT_State is sub unit group index in range?  always false if layer does not have unit groups
    
  INLINE UNIT_STATE* GetUnitState(NETWORK_STATE* net, int un_no) const
  { return net->GetUnitState(units_flat_idx + un_no); }
  // #CAT_State get the unit state at given index within full list of units in the layer

  INLINE UNIT_STATE* GetUnitStateSafe(NETWORK_STATE* net, int un_no) const
  { if(!FlatUnIdxInRange(un_no)) return NULL;
    return net->GetUnitState(units_flat_idx + un_no); }
  // #CAT_State get the unit state at given index within full list of units in the layer -- safe range checking

  INLINE UNIT_STATE* GetUnitStateFlatXY(NETWORK_STATE* net, int fl_x, int fl_y) const
  { if(fl_x >= flat_geom_x || fl_y >= flat_geom_y) return NULL;
    if(n_ungps > 0) { int gp_x = fl_x / un_geom_x; int gp_y = fl_y / un_geom_y;
      int un_x = fl_x % un_geom_x; int un_y = fl_y % un_geom_y;
      return GetUnitStateGpUnXY(net, gp_x, gp_y, un_x, un_y); }
    return GetUnitStateSafe(net, fl_y * un_geom_x + fl_x); }
  // #CAT_State get the unit state at given flat X,Y coordinates -- NULL if out of range

  INLINE UNIT_STATE* GetUnitStateGpUnIdx(NETWORK_STATE* net, int gp_dx, int un_dx) const
  { if(!UnIdxInRange(un_dx)) return NULL;
    if(HasUnitGroups())  return GetUnitStateSafe(net, gp_dx * un_geom_n + un_dx);
    else                 return GetUnitStateSafe(net, un_dx);
  }
  // #CAT_State get the unit state at given group and unit indexes -- also works for gp_dx = 0 if there are no sub unit groups
    
  INLINE UNIT_STATE* GetUnitStateGpXYUnIdx(NETWORK_STATE* net, int gp_x, int gp_y, int un_dx) const
  { if(gp_x >= gp_geom_x) return NULL; return GetUnitStateGpUnIdx(net, (gp_y * gp_geom_x + gp_x), un_dx); }
  // #CAT_State get the unit state at given group X,Y coordinate and unit indexes
    
  INLINE UNIT_STATE* GetUnitStateGpIdxUnXY(NETWORK_STATE* net, int gp_dx, int un_x, int un_y) const
  { if(un_x >= un_geom_x) return NULL; return GetUnitStateGpUnIdx(net, gp_dx, (un_y * un_geom_x + un_x)); }
  // #CAT_State get the unit state at given group index and unit X,Y coordinate 
    
  INLINE UNIT_STATE* GetUnitStateGpUnXY(NETWORK_STATE* net, int gp_x, int gp_y, int un_x, int un_y) const
  { if(un_x >= un_geom_x || gp_x >= gp_geom_x) return NULL;
    return GetUnitStateGpUnIdx(net, (gp_y * gp_geom_x + gp_x), (un_y * un_geom_x + un_x)); }
  // #CAT_State get the unit state at given group X,Y and unit X,Y coordinates

  INLINE void  GetUnXYFmIdx(int un_idx, int& un_x, int& un_y) const
  {  un_y = un_idx / un_geom_x; un_x = un_idx % un_geom_x; }
  // #CAT_State get unit X,Y from unit index within a unit group
  INLINE void  GetUnFlatXYFmIdx(int un_idx, int& un_x, int& un_y) const
  {  un_y = un_idx / flat_geom_x; un_x = un_idx % flat_geom_x; }
  // #CAT_State get unit X,Y from flat unit index within layer
  INLINE void  GetGpXYFmIdx(int gp_idx, int& gp_x, int& gp_y) const
  {  if(gp_idx < 0) gp_idx = 0;  gp_y = gp_idx / gp_geom_x; gp_x = gp_idx % gp_geom_x; }
  // #CAT_State get group X,Y from group index within the layer
  INLINE void  GetGpUnXYFmIdx(int un_idx, int& gp_x, int& gp_y, int& un_x, int& un_y) const
  { int gp_dx = un_idx / un_geom_n; int un_dx = un_idx % un_geom_n;
    GetUnXYFmIdx(un_dx, un_x, un_y);  GetGpXYFmIdx(gp_dx, gp_x, gp_y); }
  // #CAT_State get group X,Y and unit X,Y coordinates from overall unit index within the layer
  INLINE int GetUnIdxFmXY(int un_x, int un_y) const
  { int idx = un_y * un_geom_x + un_x; return idx; }
  // #CAT_State get unit-level index for unit X,Y coordinates -- valid for index within unit group or within entire layer if no unit groups
  INLINE int GetGpIdxFmXY(int gp_x, int gp_y) const
  { int idx = gp_y * gp_geom_x + gp_x; return idx; }
  // #CAT_State get group-level index for group X,Y coordinates -- valid for unit groups

  /////////////////////////////////////////////////////////////////
  //            Build / Connect / Infrastructure

  INIMPL void  Connect_Sizes(NETWORK_STATE* net);
  // #IGNORE first pass connect -- get sizes
  INIMPL void  Connect_Cons(NETWORK_STATE* net);
  // #IGNORE second pass connect -- actually make connections
  INIMPL void  RecvConsPreAlloc(NETWORK_STATE* net, PRJN_STATE* prjn, int alloc_no);
  // #IGNORE allocate given number of recv connections for all units in layer, for given projection
  INIMPL void  SendConsPreAlloc(NETWORK_STATE* net, PRJN_STATE* prjn, int alloc_no);
  // #IGNORE allocate given number of send connections for all units in layer, for given projection
  INIMPL void  RecvConsPostAlloc(NETWORK_STATE* net, PRJN_STATE* prjn);
  // #IGNORE allocate recv connections based on those allocated previously
  INIMPL void  SendConsPostAlloc(NETWORK_STATE* net, PRJN_STATE* prjn);
  // #IGNORE allocate sending connections based on those allocated previously

  INIMPL void  LayoutUnits(NETWORK_STATE* net);
  // #IGNORE layout UnitState positions within the layer, for both structural and display positions

  INLINE void  Init_Weights(NETWORK_STATE* net, bool recv_cons) {
    if(recv_cons) {
      if(prjn_start_idx < 0 || n_recv_prjns == 0) return;
      for(int i = 0; i < n_recv_prjns; i++) {
        PRJN_STATE* prjn = GetRecvPrjnState(net, i);
        if(prjn->NotActive(net)) continue;
        prjn->Init_Weights(net);
      }
    }
    else {
      if(send_prjn_start_idx < 0 || n_send_prjns == 0) return;
      for(int i = 0; i < n_send_prjns; i++) {
        PRJN_STATE* prjn = GetSendPrjnState(net, i);
        if(prjn->NotActive(net)) continue;
        prjn->Init_Weights(net);
      }
    }
  }
  // #CAT_State initialize weights for all the recv or send projections in this layer

  INLINE void  Copy_Weights(NETWORK_STATE* net, LAYER_STATE* src, bool recv_cons) {
    if(recv_cons) {
      if(prjn_start_idx < 0 || n_recv_prjns == 0) return;
      if(src->prjn_start_idx < 0 || src->n_recv_prjns == 0) return;
      int maxn = MIN(n_recv_prjns, src->n_recv_prjns);
      for(int i = 0; i < maxn; i++) {
        PRJN_STATE* prjn = GetRecvPrjnState(net, i);
        PRJN_STATE* src_prjn = src->GetRecvPrjnState(net, i);
        if(prjn->NotActive(net) || src_prjn->NotActive(net)) continue;
        prjn->Copy_Weights(net, src_prjn);
      }
    }
    else {
      if(send_prjn_start_idx < 0 || n_send_prjns == 0) return;
      if(src->send_prjn_start_idx < 0 || src->n_send_prjns == 0) return;
      int maxn = MIN(n_send_prjns, src->n_send_prjns);
      for(int i = 0; i < maxn; i++) {
        PRJN_STATE* prjn = GetSendPrjnState(net, i);
        PRJN_STATE* src_prjn = src->GetSendPrjnState(net, i);
        if(prjn->NotActive(net) || src_prjn->NotActive(net)) continue;
        prjn->Copy_Weights(net, src_prjn);
      }
    }
  }
  // #CAT_State copy weights from other layer, going projection-by-projection in order by index (only sensible if the layers have matching projection structure) -- either recv or send

  INLINE void  Init_Stats() {
    sse = 0.0f;    avg_sse.ResetAvg();    cnt_err = 0.0f;
    cur_cnt_err = 0.0f;    pct_err = 0.0f;    pct_cor = 0.0f;
    sum_prerr.InitVals();    epc_prerr.InitVals(); icon_value = 0.0f;
  }
  // #EXPERT #CAT_Statistic initialize statistic variables on layer -- called by Network Init_Stats

  INLINE void Initialize_lay_geom
  (int unx=1, int uny=1, int unn=1, int gpx=1, int gpy=1, int gpn=1, int flx=1, int fly=1, int fln=1,
   int gpspcx=1, int gpspcy=1) {
    un_geom_x = unx; un_geom_y = uny; un_geom_n = unn;
    gp_geom_x = gpx; gp_geom_y = gpy; gp_geom_n = gpn;
    flat_geom_x = flx; flat_geom_y = fly; flat_geom_n = fln; gp_spc_x = gpspcx; gp_spc_y = gpspcy;
  }
  // #IGNORE init layer geometry
  
  INLINE void Initialize_lay_core
  (int lay_dx=0, int laygp_lay0_dx=0, int laygpn=0, int units_dx=0, int ungp_dx=0, int n_un=0,
   int n_gp=0, int prjn_st_dx=0, int spec_dx=0, int uspec_dx=0, int n_recv=0, int n_send=0,
   int lf=LF_NONE, LayerType lt=HIDDEN) {
    main_obj = false; layer_idx = lay_dx; laygp_lay0_idx=0; laygp_n=0;
    units_flat_idx = units_dx;  ungp_idx = ungp_dx; n_units = n_un; n_ungps = n_gp;
    prjn_start_idx = prjn_st_dx;  spec_idx = spec_dx; unit_spec_idx = uspec_dx; 
    n_recv_prjns = n_recv;    n_send_prjns = n_send;  send_prjn_start_idx = -1;
    flags = (LayerFlags)lf;    layer_type = lt;    ext_flag = NO_EXTERNAL;
    Initialize_lay_geom();
    Init_Stats();
  }
  // #IGNORE
  

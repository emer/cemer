// directly included in UnGpState_cpp, _cuda
//{
  int   ungp_idx;       // #CAT_State #READ_ONLY index into network state unit group
  int   layer_idx;      // #CAT_State #READ_ONLY index into network state layer that owns this unit group
  int   layer_gp_idx;   // #CAT_State #READ_ONLY index of this unit group within layer -1 = layer-level group, and 0..n = sub-unit groups -- use lay->ungp_idx+1 as starting index to deref
  int   units_flat_idx; // #CAT_State #READ_ONLY starting index into flat list of unit state
  int   n_units;        // #CAT_State #READ_ONLY number of units in this unit group
  int   pos_x;          // #CAT_State #READ_ONLY structural position of lower-left start of unit group within layer -- no spaces etc
  int   pos_y;          // #CAT_State #READ_ONLY structural position of lower-left start of unit group within layer -- no spaces etc
  int   disp_pos_x;     // #CAT_State #READ_ONLY display position of lower-left start of unit group within layer -- includes spaces 
  int   disp_pos_y;     // #CAT_State #READ_ONLY display position of lower-left start of unit group within layer -- includes spaces 

  INLINE LAYER_STATE* GetLayerState(NETWORK_STATE* net) {
    return net->GetLayerState(layer_idx);
  }
  // get the owning layer

  INLINE bool UnIdxInRange(int un_no) const
  { return (un_no >= 0 && un_no < n_units); }
  // #CAT_State is unit index in range according to n_units (0 <= idx < n_units)

  INLINE UNIT_STATE*  GetUnitState(NETWORK_STATE* net, int un_idx) {
    LAYER_STATE* lay = GetLayerState(net);
    return lay->GetUnitStateGpUnIdx(net, layer_gp_idx, un_idx);
  }
  // get unit within this unit group at given unit index

  INLINE UNIT_STATE*  GetUnitStateSafe(NETWORK_STATE* net, int un_idx) {
    if(!UnIdxInRange(un_idx)) return NULL;
    LAYER_STATE* lay = GetLayerState(net);
    return lay->GetUnitStateGpUnIdx(net, layer_gp_idx, un_idx);
  }
  // get unit within this unit group at given unit index, safe range checking

  INLINE bool lesioned(NETWORK_STATE* net) {
    LAYER_STATE* lay = GetLayerState(net); return lay->lesioned();
  }
  // checks if the owning layer is lesioned

  INLINE bool  IsSubGp() { return (layer_gp_idx >= 0); }
  // #CAT_State returns true if this is a sub-group of layer -- otherwise it is the unitgroup for the entire layer -- only layers with unit_groups set have sub groups
  INLINE bool  IsLayerGp() { return (layer_gp_idx == -1); }
  // #CAT_State returns true if this is the unitgroup for the entire layer -- otherwise it is a sub-group

  INLINE void Initialize_core(int ugp_dx=-1, int lay_dx=-1, int lay_gp_dx=0, int un_fl_dx = 0, int n_un=0) {
    ungp_idx = ugp_dx;
    layer_idx = lay_dx;
    layer_gp_idx = lay_gp_dx;
    n_units = n_un;
    pos_x = 0; pos_y = 0;
    disp_pos_x = 0; disp_pos_y = 0;
  }

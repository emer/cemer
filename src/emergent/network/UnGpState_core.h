// directly included in UnGpState_cpp, _cuda
//{
  int   ungp_idx;       // #CAT_State #READ_ONLY index into network state unit group
  int   layer_idx;      // #CAT_State #READ_ONLY index into network state layer that owns this unit group

  INLINE LAYER_STATE* GetLayerState(NETWORK_STATE* net) {
    return net->GetLayerState(layer_idx);
  }
  // get the owning layer

  INLINE bool lesioned(NETWORK_STATE* net) {
    LAYER_STATE* lay = GetLayerState(net); return lay->lesioned();
  }
  // checks if the owning layer is lesioned

  INLINE void Initialize_core(int ugp_dx=-1, int lay_dx=-1) {
    ungp_idx = ugp_dx;
    layer_idx = lay_dx;
  }

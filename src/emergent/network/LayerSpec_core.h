// this is included directly into LayerSpec_cpp and LayerSpec_cuda
//{
  int           spec_idx;
  // #READ_ONLY #NO_COPY #NO_SAVE #NO_INHERIT index of this spec in list of specs

  INLINE virtual int  GetStateSpecType() const { return NETWORK_STATE::T_LAYER_SPEC; }
  // #CAT_State derived classes MUST override this and pass correct global type id

  INLINE virtual void Init_LayerState(LAYER_STATE* lst, NETWORK_STATE* net) { };
  // #CAT_State initialize layer state

  INLINE void Initialize_core() {
    spec_idx = -1;
  }
  

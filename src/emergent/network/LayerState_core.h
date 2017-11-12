// this is included directly in LayerState_cpp and LayerState_cuda -- only things in addition to Layer_core
// {

  // IMPORTANT: because we cannot have virtual functions in LayerState, NO computational methods
  // are here -- all are defined as _Layer methods in NetworkState_core, which CAN be overridden
  // mark all methods as IGNORE -- no direct access to functions on this or any State objects

  INLINE void Initialize_core() {
  }
  // #IGNORE

// this contains core shared code, and is included directly in BpNetworkState _cpp.h, _cuda.h
//{

  enum BpUnitSpecTypes { // manual type registry system for all spec types used in state code -- any new spec type MUST be added to this list, extending from N case in the last list, for any derived classes, and each spec must return its appropriate enum in GetStateSpecType() method
    T_BP_UNIT_SPEC = N_NetworkUnitSpecs,
    N_BpUnitSpecs,    // derived classes start from this one -- use class name for subclasses
  };

  enum BpConSpecTypes { // manual type registry system for all spec types used in state code -- any new spec type MUST be added to this list, extending from N case in the last list, for any derived classes, and  each spec must return its appropriate enum in GetStateSpecType() method
    T_BP_CON_SPEC = N_NetworkConSpecs,
    N_BpConSpecs,    // derived classes start from this one -- use class name for subclasses
  };

  bool	bp_to_inputs;	// #DEF_false backpropagate errors to input layers (faster if not done, which is the default)
  int   prev_epoch;     // #NO_SAVE #HIDDEN #READ_ONLY previous epoch counter -- for detecting changes
  bool  lrate_updtd;    // #NO_SAVE #HIDDEN #READ_ONLY if learning rate was updated..

  INLINE void Initialize_net_core() {
    bp_to_inputs = false;
    prev_epoch = -1;
    lrate_updtd = false;
  }
  // #IGNORE

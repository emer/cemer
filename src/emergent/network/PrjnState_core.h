// directly included in PrjnState_cpp, _cuda
//{

  bool  off;            // #CAT_State #DEF_false #MAIN turn this projection off -- useful for experimenting with projections while being able to keep the specifications in place -- this can only be changed when the network is not built and is NOT for dynamic enabling and disabling of the projection -- see lesioned for that and use the Lesion / UnLesion methods
  bool  lesioned;       // #CAT_State #MAIN #GUI_READ_ONLY #SHOW #NO_SAVE this can be set directly for temporary inactivation (lesioning) of this projection -- call Lesion or UnLesion to set properly -- also set if a layer on either side of this projection lesioned
  int   prjn_idx;       // #CAT_State #READ_ONLY #NO_SAVE index into network state projection list
  int   recv_lay_idx;   // #CAT_State #READ_ONLY #NO_SAVE index into network state layer list for (owning) receiving layer of this projection
  int   send_lay_idx;   // #CAT_State #READ_ONLY #NO_SAVE index into network state layer list for sending (from) layer of this projection
  int   send_prjn_idx;  // #CAT_State #READ_ONLY #NO_SAVE index into network state list of projections organized by layer according to sending projection order
  int   recv_idx;       // #CAT_State #READ_ONLY #NO_SAVE receiving con_state index within unit 
  int   send_idx;       // #CAT_State #READ_ONLY #NO_SAVE sending con_state index within unit
  int   spec_idx;       // #CAT_State #READ_ONLY #NO_SAVE layer spec index in list in NetworkState
  int   con_spec_idx;   // #CAT_State #READ_ONLY #NO_SAVE con spec index
  int   n_con_vars;     // #CAT_State #READ_ONLY #NO_SAVE number of connection variables in connection state type
  STATE_CLASS(PrjnConStats)   recv_con_stats;  // #CAT_State #GUI_READ_ONLY #SHOW #NO_SAVE stats for recv connections created for the receiving units in this projection
  STATE_CLASS(PrjnConStats)   send_con_stats;  // #CAT_State #GUI_READ_ONLY #SHOW #NO_SAVE stats for recv connections created for the receiving units in this projection

  INLINE PRJN_STATE*  GetPrjnState(NETWORK_STATE* net) const {
    return net->GetPrjnState(prjn_idx);
  }
  INLINE LAYER_STATE* GetRecvLayer(NETWORK_STATE* net) const {
    return net->GetLayerState(recv_lay_idx);
  }
  INLINE LAYER_STATE* GetSendLayer(NETWORK_STATE* net) const {
    return net->GetLayerState(send_lay_idx);
  }
  INLINE PRJN_SPEC_CPP* GetPrjnSpec(NETWORK_STATE* net) const {
    return net->GetPrjnSpec(spec_idx);
  }
  INLINE CON_SPEC_CPP*  GetConSpec(NETWORK_STATE* net) const {
    return net->GetConSpec(con_spec_idx);
  }
  
  INLINE bool IsActive(NETWORK_STATE* net) const { return (!off && !lesioned); }
  // #CAT_Access is this projection active and valid?

  INLINE bool NotActive(NETWORK_STATE* net) const { return !IsActive(net); }
  // #CAT_Access is this projection NOT active and valid?

  INIMPL void  Init_Weights(NETWORK_STATE* net);
  // #CAT_State initialize weights for all the connections associated with this projection

  INIMPL void Copy_Weights(NETWORK_STATE* net, PRJN_STATE* src);
  // #CAT_State copy weights from other projection

  INLINE void ResetConStats() {
    recv_con_stats.ResetStats();  send_con_stats.ResetStats();
  }

  INIMPL void LesionState(NETWORK_STATE* net);
  // #IGNORE lesion all the state associated with this projection, including sending and recv connection groups on units
  INIMPL void UnLesionState(NETWORK_STATE* net);
  // #IGNORE un-lesion all the state associated with this projection, including sending and recv connection groups on units

  INLINE void Initialize_core
  (bool of=false, bool les=false, int prjn_dx=0, int recv_lay_dx=0, int send_lay_dx=0,
   int send_prjn_dx=0, int recv_dx=0, int send_dx=0, int spec_dx=0, int cs_dx=0, int ncv=0) {
    off = of;  lesioned = les;
    prjn_idx = prjn_dx;    recv_lay_idx = recv_lay_dx;    send_lay_idx = send_lay_dx;
    send_prjn_idx = send_prjn_dx; recv_idx = recv_dx;    send_idx = send_dx;
    spec_idx = spec_dx; con_spec_idx = cs_dx;  n_con_vars = ncv;
    ResetConStats();
  }

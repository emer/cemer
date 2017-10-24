// directly included in PrjnState_cpp, _cuda
//{
  bool  off; // #DEF_false #MAIN turn this projection off -- useful for experimenting with projections while being able to keep the specifications in place
  bool  lesioned;       // #CAT_State #MAIN #GUI_READ_ONLY #NO_SAVE is a layer on either side of this projection lesioned, or is from not set properly?
  int   prjn_idx;       // #CAT_State #READ_ONLY #NO_SAVE index into network state projection list
  int   recv_lay_idx;   // #CAT_State #READ_ONLY #NO_SAVE index into network state layer list for (owning) receiving layer of this projection
  int   send_lay_idx;   // #CAT_State #READ_ONLY #NO_SAVE index into network state layer list for sending (from) layer of this projection
  int   send_prjn_idx;  // #CAT_State #READ_ONLY #NO_SAVE index into network state list of projections organized by layer according to sending projection order
  int   recv_idx;       // #CAT_State #READ_ONLY #NO_SAVE receiving con_state index within unit 
  int   send_idx;       // #CAT_State #READ_ONLY #NO_SAVE sending con_state index within unit
  int   spec_idx;       // #CAT_State #READ_ONLY #NO_SAVE layer spec index in list in NetworkState
  int   con_spec_idx;   // #CAT_State #READ_ONLY #NO_SAVE con spec index
  int   n_con_vars;     // #CAT_State #READ_ONLY #NO_SAVE number of connection variables in connection state type

  INLINE PRJN_STATE*  GetPrjnState(NETWORK_STATE* net) const {
    return net->GetPrjnState(prjn_idx);
  }
  INLINE LAYER_STATE* GetRecvLayerState(NETWORK_STATE* net) const {
    return net->GetLayerState(recv_lay_idx);
  }
  INLINE LAYER_STATE* GetSendLayerState(NETWORK_STATE* net) const {
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

  INLINE void Initialize_core
  (bool of=true, bool les=false, int prjn_dx=0, int recv_lay_dx=0, int send_lay_dx=0,
   int send_prjn_dx=0, int recv_dx=0, int send_dx=0, int spec_dx=0, int cs_dx=0, int ncv=0) {
    off = of;  lesioned = les;
    prjn_idx = prjn_dx;    recv_lay_idx = recv_lay_dx;    send_lay_idx = send_lay_dx;
    send_prjn_idx = send_prjn_dx; recv_idx = recv_dx;    send_idx = send_dx;
    spec_idx = spec_dx; con_spec_idx = cs_dx;  n_con_vars = ncv;
  }

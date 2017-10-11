// directly included in PrjnState_cpp, _cuda
//{
  bool  off;            // #CAT_State is this turned off?
  bool  lesioned;       // #CAT_State a unit on either side of this projection lesioned?
  int   prjn_idx;       // #CAT_State #READ_ONLY index into network state projection list
  int   recv_lay_idx;   // #CAT_State #READ_ONLY index into network state layer list for (owning) receiving layer of this projection
  int   send_lay_idx;   // #CAT_State #READ_ONLY index into network state layer list for sending (from) layer of this projection
  int   recv_idx;       // #CAT_State #READ_ONLY receiving con_state index within 
  int   send_idx;       // #CAT_State #READ_ONLY sending con_state index
  int   con_spec_idx;   // #CAT_State #READ_ONLY con spec index

  INLINE LAYER_STATE* GetRecvLayerState(NETWORK_STATE* net) {
    return net->GetLayerState(recv_lay_idx);
  }
  INLINE LAYER_STATE* GetSendLayerState(NETWORK_STATE* net) {
    return net->GetLayerState(send_lay_idx);
  }
  INLINE CON_SPEC*    GetConSpec(NETWORK_STATE* net) {
    return net->GetConSpec(con_spec_idx);
  }
  
  INLINE bool IsActive(NETWORK_STATE* net) {
    return (!off && !lesioned);
  }
  // for updating lesioned flag -- must be done in network to avoid need to ref layerstate here
  // LAYER_STATE* recv = GetRecvLayerState(net);
  // if(!recv || recv->Lesioned()) return false;
  // LAYER_STATE* send = GetSendLayerState(net);
  // if(!send || send->Lesioned()) return false;
  // return true;

  INLINE void Initialize_core(bool of=true, bool les=false, int prjn_dx=0, int recv_lay_dx=0,
                              int send_lay_dx=0, int recv_dx=0, int send_dx=0, int cs_dx=0) {
    off = of;  lesioned = les;
    prjn_idx = prjn_dx;    recv_lay_idx = recv_lay_dx;    send_lay_idx = send_lay_dx;
    recv_idx = recv_dx;    send_idx = send_dx;  con_spec_idx = cs_dx;
  }

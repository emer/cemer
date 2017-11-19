// this is included directly in AllProjectionSpecs_cpp / _cuda
// {

  float		recv_p_con;     // #MIN_0 #MAX_1 for receiving units, what is probability for receiving sending connections from sending layer -- determines how many connections to allocate here -- look at the recv_cons stats in the projection object to tune this value (also error messages during build if too low)
  float		send_p_con;     // #MIN_0 #MAX_1 for sending units, what is probability for sending connections to receiving layer -- determines how many connections to allocate here -- look at the send_cons stats in the projection object to tune this value (also error messages during build if too low)

  INIMPL bool ConnectPassCheck(PRJN_STATE* prjn, NETWORK_STATE* net, int pass) const override
  { return (pass == 2); }
  
  INIMPL void Connect_impl(PRJN_STATE* prjn, NETWORK_STATE* net, int make_cons) override;

  INLINE void Initialize_core() { recv_p_con = 1.0f; send_p_con = 1.0f; }

  INLINE int  GetStateSpecType() const override
  { return NETWORK_STATE::T_SymmetricPrjnSpec; }

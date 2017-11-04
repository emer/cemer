// this is included directly in AllProjectionSpecs_cpp / _cuda
// {
  int	n_conns;		// number of connections to make (-1 for size of layer)
  int	recv_start;		// starting unit index for recv connections
  int 	send_start;		// starting unit index for sending connections
  bool  use_gp;                 // if unit groups are present in the recv or sender layer, and the other layer fits within one unit group, then connectivity will be replicated for across groups in that layer

  INIMPL void  Connect_impl(PRJN_STATE* prjn, NETWORK_STATE* net, bool make_cons) override;
  INIMPL virtual void ConnectRecvGp_impl(PRJN_STATE* prjn, NETWORK_STATE* net, bool make_cons); // recv is using groups
  INIMPL virtual void ConnectSendGp_impl(PRJN_STATE* prjn, NETWORK_STATE* net, bool make_cons); // send is using groups

  INLINE void Initialize_core() {
    n_conns = -1;
    recv_start = 0;
    send_start = 0;
    use_gp = false;
  }

  INLINE int  GetStateSpecType() const override
  { return NETWORK_STATE::T_OneToOnePrjnSpec; }

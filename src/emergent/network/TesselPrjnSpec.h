// this is included directly in AllProjectionSpecs_cpp / _cuda
// {
  TAVECTOR2I	recv_off;	// offset in layer for start of recv units to begin connecting -- can leave some unconnected units around the edges if desired
  TAVECTOR2I	recv_n;		// number of receiving units to connect in each dimension (-1 for all)
  TAVECTOR2I	recv_skip;	// increment for recv units in each dimension -- 1 = connect all units; 2 = skip every other unit, etc
  TAVECTOR2I	recv_group;	// group together this many units under the same starting coord, resulting in a tile pattern -- each member of the group (which needn't correspond to an actual unit group in the recv layer) gets the same effective location as the first member of the group
  bool		wrap;		// whether to wrap coordinates around in the sending layer (e.g., if it goes past the right side, then it continues back on the left).  otherwise it will clip off connections at the edges.  Any clipping will affect the ability to initialize weight patterns properly, so it is best to avoid that.
  TAVECTOR2F    send_scale;	// scale to apply to transform receiving unit coords into sending unit coords -- can use this to compensate for differences in the sizes between the two layers
  TAVECTOR2F    send_off;	// #AKA_send_border constant offset to add to sending offsets relative to the position of the receiving unit -- this just adds a constant to the specific send_offs that specify the detailed pattern of connectivity
  int           n_send_offs;      // #READ_ONLY number of sending offsets to use
  int           alloc_send_offs;  // #READ_ONLY number of sending offsets allocated
  STATE_CLASS(TessEl)*   send_offs_m;      // #READ_ONLY n_send_offs sending offsets

  // TODO: need to add to StateBaseSpec copy to copy offsets

  INIMPL void  AllocSendOffs(int ns);
  // #IGNORE allocate the send_offs_m array -- done by parent emergent obj
  INIMPL void  FreeSendOffs();
  // #IGNORE free any allocated send_offs_m
  
  INIMPL void Connect_impl(PRJN_STATE* prjn, NETWORK_STATE* net, bool make_cons) override;

  INIMPL void Init_Weights_Prjn(PRJN_STATE* prjn, NETWORK_STATE* net, int thr_no,
                                CON_STATE* cg) override;

  bool  HasRandomScale() override { return false; }

  INIMPL virtual void	GetCtrFmRecv(TAVECTOR2I& sctr, TAVECTOR2I ruc);
  // #IGNORE get center of sender coords from receiving coords
  INIMPL virtual void	Connect_RecvUnit
  (PRJN_STATE* prjn, NETWORK_STATE* net, UNIT_STATE* ru_u, const TAVECTOR2I& ruc,
   bool send_alloc);
  // #IGNORE connects one recv unit to all senders

  INIMPL void	Initialize_core();

  INLINE int  GetStateSpecType() const override { return NETWORK_STATE::T_TesselPrjnSpec; }

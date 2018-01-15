// this is included directly in AllProjectionSpecs_cpp / _cuda
// {
  TAVECTOR2I    recv_gp_off;    // offset for start of recv group to begin connecting
  TAVECTOR2I    recv_gp_n;      // number of receiving groups to connect in each dimension (-1 for all)
  TAVECTOR2I    recv_gp_skip;   // increment for recv groups in each dimension -- 1 = connect all groups; 2 = skip every other group, etc
  TAVECTOR2I    recv_gp_group;  // group together this many unit groups under the same starting coord, resulting in a tile pattern
  TAVECTOR2F    send_gp_scale;  // scale to apply to transform receiving unit group coords into sending unit group coords
  TAVECTOR2I    send_gp_border; // border size around sending layer (constant offset to add to sending offsets)
  bool          wrap;           // whether to wrap coordinates around (else clip)
  float         def_p_con;      // default probability of connectivity when new send_gp_offs are created
  bool          sym_self;       // if a self projection with p_con < 1 (full), make it symmetric (senders = receivers) otherwise it is not
  bool          same_seed;      // use the same random seed each time (same connect pattern)
  STATE_CLASS(RndSeed)  rndm_seed;  // #HIDDEN random seed
  int           n_send_gp_offs;     // #READ_ONLY #NO_COPY #NO_SAVE number of sending group offsets to use
  int           alloc_send_gp_offs; // #READ_ONLY #NO_COPY #NO_SAVE number of sending group offsets allocated
  STATE_CLASS(GpTessEl)* send_gp_offs_m; // #IGNORE n_send_gp_offs sending group offsets

  INIMPL void  AllocSendOffs(int ns);
  // #IGNORE allocate the send_offs_m array -- done by parent emergent obj
  INIMPL void  FreeSendOffs();
  // #IGNORE free any allocated send_offs_m
  
  INIMPL void Connect_impl(PRJN_STATE* prjn, NETWORK_STATE* net, int make_cons) override;

  INIMPL virtual void   GetCtrFmRecv(TAVECTOR2I& sctr, TAVECTOR2I ruc);
  // get center of sender coords from receiving coords
  INIMPL virtual void   Connect_RecvGp
  (PRJN_STATE* prjn, NETWORK_STATE* net, int rgpidx, const TAVECTOR2I& ruc,
   int make_cons);
  // #IGNORE connects one recv unit to all senders

  INIMPL void   Initialize_core();

  INLINE int  GetStateSpecType() const override
  { return NETWORK_STATE::T_GpTesselPrjnSpec; }

// this is included directly in AllProjectionSpecs_cpp / _cuda
// {

  TAVECTOR2I    send_tile_size;         // number of units in one tile of the sending unit group units
  TAVECTOR2I    send_tile_skip;         // number of units to skip when moving the tiling over to the next position (typically 1/2 of the size for nice overlap)
  bool          wrap;                   // if true, then connectivity has a wrap-around structure so it starts at -tile_skip (wrapped to right/top) and goes +tile_skip past the right/top edge (wrapped to left/bottom) -- this produces more uniform overlapping coverage of the space
  bool          reciprocal;             // if true, make the appropriate reciprocal connections for a backwards projection from recv to send

  INIMPL void Connect_impl(PRJN_STATE* prjn, NETWORK_STATE* net, int make_cons) override;

  INIMPL virtual void   Connect_Reciprocal(PRJN_STATE* prjn, NETWORK_STATE* net, int make_cons);

  // INIMPL int         ProbAddCons_impl(Projection* prjn, float p_add_con, float init_wt = 0.0f) override;

  INIMPL virtual void   Connect_UnitGroup
  (PRJN_STATE* prjn, NETWORK_STATE* net, LAYER_STATE* recv_lay, LAYER_STATE* send_lay,
   int rgpidx, int suidx, int make_cons);
  // #IGNORE connect one recv unit group to all sending unit groups -- rgpidx = recv unit group idx, suidx = send unit idx within subgroups

  INIMPL void   Initialize_core();
  // #IGNORE

  INLINE int  GetStateSpecType() const override
  { return NETWORK_STATE::T_TiledGpMapConvergePrjnSpec; }


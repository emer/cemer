// this is included directly in AllProjectionSpecs_cpp / _cuda
// {

  float		gauss_sigma;		// #CONDEDIT_ON_init_wts gaussian width parameter for initial weight values (only with init_wts on) to give a tuning curve in terms of distance from center of overall rf (normalized units)
  int		su_idx_st;		// starting sending unit index within each unit group to start connecting from -- allows for layers to have diff unit group structure
  int		ru_idx_st;		// starting receiving unit index within each unit group to start connecting from -- allows for layers to have diff unit group structure
  int		gp_n_cons;		// [-1 = all] number of units to connect within each group -- allows for layers to have diff unit group structure

  INIMPL void	Connect_UnitGroupRF
    (PRJN_STATE* prjn, NETWORK_STATE* net, LAYER_STATE* recv_lay, LAYER_STATE* send_lay,
     int rgpidx, int sgpidx, bool make_cons, bool share_cons, bool reciprocal)  override;

  INIMPL void Init_Weights_Prjn
    (PRJN_STATE* prjn, NETWORK_STATE* net, int thr_no, CON_STATE* cg) override;

  INLINE bool  HasRandomScale() override { return false; }

  INIMPL void	Initialize_core();
  // #IGNORE

  INLINE int  GetStateSpecType() const override { return NETWORK_STATE::T_TiledGpRFOneToOnePrjnSpec; }

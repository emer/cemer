// this is included directly in AllProjectionSpecs_cpp / _cuda
// {

  float		one_to_one_wt;	// #CONDEDIT_ON_init_wts weight between units with the same index in the unit group
  float		other_wt;	// #CONDEDIT_ON_init_wts weight between other non-one-to-one units

  INIMPL void Init_Weights_Prjn
    (PRJN_STATE* prjn, NETWORK_STATE* net, int thr_no, CON_STATE* cg) override;

  INLINE bool  HasRandomScale() override { return false; }

  INIMPL void	Initialize_core();

  INLINE int  GetStateSpecType() const override { return NETWORK_STATE::T_TiledGpRFOneToOneWtsPrjnSpec; }

// this is included directly in AllProjectionSpecs_cpp / _cuda
// {

  TAVECTOR2I 	 rf_width;	// size of the receptive field -- should be an even number
  TAVECTOR2F     rf_move;	// how much to move in input coordinates per each receiving increment (unit group or unit within group, depending on whether inner or outer) -- typically 1/2 rf_width
  float		gauss_sigma;	// #CONDEDIT_ON_init_wts gaussian width parameter for initial weight values to give a tuning curve
  bool		wrap;		// if true, then connectivity has a wrap-around structure so it starts at -rf_move (wrapped to right/top) and goes +rf_move past the right/top edge (wrapped to left/bottom)


  INIMPL void Connect_impl(PRJN_STATE* prjn, NETWORK_STATE* net, bool make_cons) override;

  INIMPL void Init_Weights_Prjn
    (PRJN_STATE* prjn, NETWORK_STATE* net, int thr_no, CON_STATE* cg) override;

  INLINE bool  HasRandomScale() override { return false; }

  INIMPL void	Initialize_core();
  // #IGNORE

  INLINE int  GetStateSpecType() const override
  { return NETWORK_STATE::T_GaussRFPrjnSpec; }

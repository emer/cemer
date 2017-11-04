// this is included directly in AllProjectionSpecs_cpp / _cuda
// {

  enum InitWtsType {
    GAUSSIAN,                    // standard gaussian weight distribution
  };
  
  TAVECTOR2I	send_gp_size;		// number of groups in the sending receptive field
  TAVECTOR2I	send_gp_skip;		// number of groups to skip per each recv group (typically 1/2 of the size for nice overlap)
  TAVECTOR2I	send_gp_start;		// starting offset for sending groups -- for wrapping this was previously automatically set to -send_gp_skip (else 0), but you can now set this to anything you want
  TAVECTOR2I	send_subgp_size;        // number of sub-groups in the sending receptive field -- these sub-groups are nested within the larger topographic receiptive field tiling
  TAVECTOR2I	recv_subgp_size;        // number of sub-groups in the receiving receptive field -- these sub-groups are nested within the larger topographic receiptive field tiling
  bool		wrap;			// if true, then connectivity has a wrap-around structure so it starts at -gp_skip (wrapped to right/top) and goes +gp_skip past the right/top edge (wrapped to left/bottom)
  bool		reciprocal;		// if true, make the appropriate reciprocal connections for a backwards projection from recv to send
  bool          share_cons;             // units in unit groups after the first unit group share connection values (weights etc) with those in the first unit group -- generally only works if wrap is true so that all units have same scope and order of connectivity -- also the number of units in the unit groups MUST be an even multiple of the number of threads, so that the source and user units are both in the same thread
  InitWtsType   wts_type;               // #CONDSHOW_ON_init_wts how to initialize the random initial weights
  float		gauss_sig;		// #CONDSHOW_ON_init_wts&&wts_type:GAUSSIAN gaussian sigma (width), in normalized units where entire distance across sending layer is 1.0
  float         gauss_ctr_mv;           // #CONDSHOW_ON_init_wts&&wts_type:GAUSSIAN how much the center of the gaussian moves with respect to the position of the receiving unit within its unit group -- 1.0 = centers span the entire range of the receptive field
  STATE_CLASS(MinMaxRange)	wt_range;
  // #CONDSHOW_ON_init_wts range of weakest (min) to strongest (max) weight values generated -- for bimodal, min and max are the two means of the bimodal distribution
  float         p_high;
  // #CONDSHOW_ON_init_wts&&wts_type:BIMODAL_PERMUTED probability of generating high values for bimodal permuted case

  INIMPL void Connect_impl(PRJN_STATE* prjn, NETWORK_STATE* net, bool make_cons) override;

  INIMPL void Init_Weights_Prjn
    (PRJN_STATE* prjn, NETWORK_STATE* net, int thr_no, CON_STATE* cg) override;

  INLINE bool  HasRandomScale() override { return false; }

  INIMPL virtual void	Init_Weights_Gaussian
    (PRJN_STATE* prjn, NETWORK_STATE* net, int thr_no, CON_STATE* cg);
  // gaussian initial weights

  INIMPL void Initialize_core();

  INLINE int  GetStateSpecType() const override
  { return NETWORK_STATE::T_TiledSubGpRFPrjnSpec; }

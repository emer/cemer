// this is included directly in AllProjectionSpecs_cpp / _cuda
// {

  enum InitWtsType {
    GAUSSIAN,                   // gassian initial weights
    SIGMOID,                    // sigmoidal initial weights
  };

 enum GroupUnitOpts {
   BY_UNIT,                     // use the individual unit positions to shape the weight distribution (sometimes also in conjunction with the group positions, depending on which level -- see member comment for more specifics per each case)
   BY_GROUP,                    // use the unit group positions, and NOT the individual unit positions, to shape the weight distribution
   ALL_SAME,                    // all units have the same weight distribution -- this is only applicable to the receiving layer side, not the sending layer
  };

  TAVECTOR2I	send_gp_size;		// number of groups in the sending receptive field
  TAVECTOR2I	send_gp_skip;		// number of groups to skip per each recv group (typically 1/2 of the size for nice overlap)
  TAVECTOR2I	send_gp_start;		// starting offset for sending groups -- for wrapping this was previously automatically set to -send_gp_skip (else 0), but you can now set this to anything you want
  bool		wrap;			// if true, then connectivity has a wrap-around structure so it starts at -gp_skip (wrapped to right/top) and goes +gp_skip past the right/top edge (wrapped to left/bottom)
  bool		reciprocal;		// if true, make the appropriate reciprocal connections for a backwards projection from recv to send
  bool          share_cons;             // units in unit groups after the first unit group share connection values (weights etc) with those in the first unit group -- generally only works if wrap is true so that all units have same scope and order of connectivity -- also the number of units in the unit groups MUST be an even multiple of the number of threads, so that the source and user units are both in the same thread
  InitWtsType   wts_type;               // #CONDSHOW_ON_init_wts type of initial weights to create for init_wts
  STATE_CLASS(GaussInitWtsSpec) full_gauss;          // #CONDSHOW_ON_init_wts&&wts_type:GAUSSIAN parameters for gaussian initial weight distribution for the full width of the projection across all sending unit groups and units within those sending units -- see also gauss_opts
  STATE_CLASS(GaussInitWtsSpec) gp_gauss;            // #CONDSHOW_ON_init_wts&&wts_type:GAUSSIAN parameters for gaussian initial weight distribution for each individual sending unit group
  GroupUnitOpts    full_send;        //  #CONDSHOW_ON_init_wts&&wts_type:GAUSSIAN&&full_gauss.on use units or unit groups to organize the sending weights for the full_gauss component -- BY_UNIT means that each unit within the sending layer gets a different gaussian weight value -- BY_GROUP means that all units within the same sending group get the same weight values -- ALL_SAME is not a valid option for this case
  GroupUnitOpts    full_recv;        //  #CONDSHOW_ON_init_wts&&wts_type:GAUSSIAN&&full_gauss.on use units, unit groups, or nothing (ALL_SAME) to organize the receiving weights for the full_gauss component -- BY_UNIT means that the relative position of the recv unit within its unit group (or layer if no groups) determines the center of the gaussian coming from the sending layer -- BY_GROUP means that the relative position of the recv unit group within the recv layer determines the center of the gaussian coming from the sending layer (only applicable for layers with unit groups!) -- ALL_SAME means that the recv position does not matter at all -- all recv units get the same gaussian profile across the sending unit projection (this still moves according to the connectivity parameters)
    
  STATE_CLASS(SigmoidInitWtsSpec) full_sig;          // #CONDSHOW_ON_init_wts&&wts_type:SIGMOID parameters for sigmoid initial weight distribution for the full width of the projection across all sending unit groups
  STATE_CLASS(SigmoidInitWtsSpec) gp_sig;            // #CONDSHOW_ON_init_wts&&wts_type:SIGMOID parameters for sigmoid initial weight distribution for each individual sending unit group
  STATE_CLASS(MinMaxRange)	wt_range;               // #CONDSHOW_ON_init_wts range of weakest (min) to strongest (max) weight values generated -- typically want to center this around .5, and often fairly subtle differences (.4 - .6) produce reasonably strong effects on Leabra networks

  
  INIMPL void Connect_impl(PRJN_STATE* prjn, NETWORK_STATE* net, bool make_cons) override;

  INIMPL void Init_Weights_Prjn
    (PRJN_STATE* prjn, NETWORK_STATE* net, int thr_no, CON_STATE* cg) override;

  INLINE bool  HasRandomScale() override { return false; }


  INIMPL virtual void	Init_Weights_Gaussian
    (PRJN_STATE* prjn, NETWORK_STATE* net, int thr_no, CON_STATE* cg);
  // gaussian initial weights
  INIMPL virtual void	Init_Weights_Sigmoid
    (PRJN_STATE* prjn, NETWORK_STATE* net, int thr_no, CON_STATE* cg);
  // sigmoid initial weights

  INIMPL void	Initialize_core();

  INLINE int  GetStateSpecType() const override
  { return NETWORK_STATE::T_TiledGpRFPrjnSpec; }

// this is included directly in AllProjectionSpecs_cpp / _cuda
// {

  enum GradType {		// type of gradient to establish
    LINEAR,			// linear fall-off as a function of distance
    GAUSSIAN,			// gaussian fall-off as a function of distance
  };

 STATE_CLASS(MinMaxRange)	wt_range;	// #CONDEDIT_ON_init_wts range of weakest (min) to strongest (max) weight values generated
  bool		invert;		// #CONDEDIT_ON_init_wts invert the gradient, such that the min is located "below" the recv units, and the max is furthest away
  bool		grad_x;		// #CONDEDIT_ON_init_wts compute a gradient over the x dimension of the sending layer, based on x axis location of the matrix stripe unit group
  bool		grad_y;		// #CONDEDIT_ON_init_wts compute a gradient over the y dimension of the sending layer, based on y axis location of the matrix stripe unit group
  bool		wrap;		// #CONDEDIT_ON_init_wts wrap weight values around relevant dimension(s) -- the closest location wins -- this ensures that all units have the same overall weight strengths
  bool		use_gps;	// #CONDEDIT_ON_init_wts if recv layer has unit groups, use them for determining relative position to compare with sending unit locations (unit group information is never used for the sending layer)
  GradType	grad_type;	// #CONDEDIT_ON_init_wts type of gradient to make -- applies to both axes
  float		gauss_sig;	// #CONDSHOW_ON_grad_type:GAUSSIAN gaussian sigma (width), in normalized units where entire distance across sending layer is 1.0 

  INIMPL void Init_Weights_Prjn
    (PRJN_STATE* prjn, NETWORK_STATE* net, int thr_no, CON_STATE* cg) override;

  INLINE bool  HasRandomScale() override { return false; }

  INIMPL virtual void	InitWeights_RecvGps
  (PRJN_STATE* prjn, NETWORK_STATE* net, int thr_no, CON_STATE* cg, UNIT_STATE* ru);
  // #IGNORE for recv unit group case
  INIMPL virtual void 	InitWeights_RecvFlat
  (PRJN_STATE* prjn, NETWORK_STATE* net, int thr_no, CON_STATE* cg, UNIT_STATE* ru);
  // #IGNORE for flat recv layer case (just unit positions)
  INIMPL virtual void 	SetWtFmDist
  (PRJN_STATE* prjn, NETWORK_STATE* net, int thr_no, CON_STATE* cg, UNIT_STATE* ru,
   float dist, int cg_idx);
  // #IGNORE actually set the weight value from distance value -- util used by both of above main routines -- can overload to implement different gradient functions -- cg_idx is index within con group, and dist is computed normalized distance value (0-1)

  INIMPL void	Initialize_core();
  // #IGNORE

  INLINE int  GetStateSpecType() const override
  { return NETWORK_STATE::T_GradientWtsPrjnSpec; }


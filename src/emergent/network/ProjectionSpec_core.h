// this is included directly in ProjectionSpec_cpp / _cuda etc
//{
  int           spec_idx;          // #CAT_State #READ_ONLY #NO_SAVE index of this spec in list of prjn specs in network
  bool          self_con;          // #CAT_State whether to create self-connections or not (if applicable)
  bool          init_wts;     	   // #CAT_State whether this projection spec should initialize the connection weights according to specific options supported by some specialized projection specs (e.g., topographically-organized connections) -- for any specs that do not have specific support for this, it will just fall back on the connection spec random weight settings, EXCEPT if set_scale is also selected, in which case it will set the scale values instead of the weights
  bool          set_scale;         // #CAT_State #CONDSHOW_ON_init_wts only for Leabra algorithm: if initializing the weights, set the connection scaling parameter in addition to intializing the weights -- for specifically-supported specs, this will for example set a gaussian scaling parameter on top of random initial weights, instead of just setting the initial weights to a gaussian weighted value -- for other specs that do not support a custom init_wts function, this will set the scale values to what the random weights would otherwise be set to, and set the initial weight value to a constant (init_wt_val)
  float         init_wt_val;       // #CAT_State #CONDSHOW_ON_init_wts&&set_scale constant initial weight value for specs that do not support a custom init_wts function and have set_scale set: the scale values are set to what the random weights would otherwise be set to, and the initial weight value is set to this constant: the net actual weight value is scale * init_wt_val..
  bool          add_rnd_var;  	   // #AKA_add_rnd_wts #CONDSHOW_ON_init_wts if init_wts is set, use the random weight settings on the conspec to add random values to the weights set by the projection spec -- the mean of the random distribution is subtracted, so we're just adding variance, not any mean value
  STATE_CLASS(RenormInitWtsSpec) renorm_wts;    // #CAT_State renormalize initial weight values -- this can be done even if this projection does not have init_wts set -- if set_scale is set, then the scales are renormalized instead of the weights

  INLINE virtual int  GetStateSpecType() const { return NETWORK_STATE::T_ProjectionSpec; }
  // #CAT_State derived classes MUST override this and pass correct global type id

  INIMPL virtual void Connect_Sizes(PRJN_STATE* prjn, NETWORK_STATE* net);
  // #CAT_State first-pass connects the network, doing Connect_impl(false), ending up with target allocation sizes
  INIMPL virtual void Connect_impl(PRJN_STATE* prjn, NETWORK_STATE* net, bool make_cons) { };
  // #CAT_State actually implements specific connection code -- called in two passes -- first with make_cons = false does allocation, and second with make_cons = true
  INIMPL virtual void Connect_Cons(PRJN_STATE* prjn, NETWORK_STATE* net);
  // #CAT_State second pass connection -- actually makes the connections via Connect_impl(true), and then calls Init_Weights

  INIMPL virtual int  ProbAddCons(PRJN_STATE* prjn, NETWORK_STATE* net, float p_add_con, float init_wt = 0.0);
  // #CAT_State probabilistically add a proportion of new connections to replace those pruned previously, init_wt = initial weight value of new connection
  INIMPL virtual int  ProbAddCons_impl(PRJN_STATE* prjn, NETWORK_STATE* net, float p_add_con, float init_wt = 0.0);
  // #CAT_State actual implementation: probabilistically add a proportion of new connections to replace those pruned previously, init_wt = initial weight value of new connection

  INIMPL virtual void SetCnWtScale(PRJN_STATE* prjn, NETWORK_STATE* net, int thr_no, CON_STATE* cg, int cn_idx, float wt_val);
  // #CAT_Weights standard function for setting weight or scale value depending on relevant prjnspec params (set_scale) -- calls following functions:
    INIMPL virtual void SetCnWt(PRJN_STATE* prjn, NETWORK_STATE* net, int thr_no, CON_STATE* cg, int cn_idx, float wt_val);
    // #CAT_Weights set given connection number in con group to given weight value -- this implements the add_rnd_var flag to add random variance to weights if set
    INIMPL virtual void SetCnWtRnd(PRJN_STATE* prjn, NETWORK_STATE* net, int thr_no, CON_STATE* cg, int cn_idx);
    // #CAT_Weights set given connection number in con group to standard random weight value as specified in the connection spec
    INIMPL virtual void SetCnScale(PRJN_STATE* prjn, NETWORK_STATE* net, int thr_no, CON_STATE* cg, int cn_idx, float scale_val);
    // #CAT_Weights set given connection number in con group to given scale value

  INIMPL virtual void Init_Weights_Prjn(PRJN_STATE* prjn, NETWORK_STATE* net, int thr_no, CON_STATE* cg);
  // #CAT_Weights #IGNORE when init_wts flag is set, the projection spec sets weights for the entire set of connections, from a recv perspective (always use safe access for Cn that does not depend on who owns it) -- overload in subclasses that set weights

  INIMPL virtual void Init_Weights_renorm(PRJN_STATE* prjn, NETWORK_STATE* net, int thr_no, CON_STATE* cg);
  // #CAT_Weights #IGNORE renormalize weights -- done as a second pass after Init_Weights and before Init_Weights_post

  INLINE virtual bool HasRandomScale() { return init_wts && set_scale; }
  // #CAT_Weights does this projection spec set a randomized scale value -- by default this returns true when init_wts && set_scale is true -- other specs that do set_scale in a different way (e.g., a non-random topographic pattern) may yet return false in this case -- this has implications for saving the scale value with the weights file for example

  INLINE virtual void Init_PrjnState(PRJN_STATE* prjn, NETWORK_STATE* net) { };
  // #CAT_State initialize projection state -- for projections that have extra state variables
  
  INIMPL virtual void  Connect_Gps
  (PRJN_STATE* prjn, NETWORK_STATE* net, int rgpidx, int sgpidx, float p_con,
   bool sym_same_lay, bool make_cons);
  // #IGNORE connects units in given recv and sending group index, with given probability: p_con = 1 = full connectivity, -1 = symmetric with existing connections from other projection, and (0..1) probabilistic, optionally with symmetry enforced for within same layer/group -- this call handles alloc vs. make cons step and uses *incremental* connectivity alloc so you need to call Recv/Send/ConsPostAlloc on layers after !make_cons pass -- must go through this call and not call sub-guys directly as they do not handle the alloc (or you can handle the alloc yourself separately)
    INIMPL virtual void  Connect_Gps_Full
    (PRJN_STATE* prjn, NETWORK_STATE* net, int rgpidx, int sgpidx);
    // #IGNORE connect groups, full connectivity
    INIMPL virtual void  Connect_Gps_Prob
    (PRJN_STATE* prjn, NETWORK_STATE* net, int rgpidx, int sgpidx, float p_con);
    // #IGNORE connect groups standard, not symmetric/same layer but with given probability
    INIMPL virtual void  Connect_Gps_ProbSymSameGp
    (PRJN_STATE* prjn, NETWORK_STATE* net, int rgpidx, int sgpidx, float p_con);
    // #IGNORE connect groups symmetric, same unit group (same layer)
    INIMPL virtual void  Connect_Gps_ProbSymSameLay
    (PRJN_STATE* prjn, NETWORK_STATE* net, int rgpidx, int sgpidx, float p_con);
    // #IGNORE connect groups symmetric, same layer (diff unit group)
    INIMPL virtual void  Connect_Gps_Sym
    (PRJN_STATE* prjn, NETWORK_STATE* net, int rgpidx, int sgpidx, float p_con);
    // #IGNORE connect groups with connections that are symmetric with existing (p_con < 0)

  INIMPL virtual void	Connect_UnitGroupRF
    (PRJN_STATE* prjn, NETWORK_STATE* net, LAYER_STATE* recv_lay, LAYER_STATE* send_lay,
     int rgpidx, int sgpidx, bool make_cons, bool share_cons = false, bool reciprocal = false);
  // #IGNORE receiptive-field version of connect one unit group to another -- rgpidx = recv unit group idx, sgpidx = send unit group idx -- either can be -1 for using entire layer instead -- has share cons and reciprocal connectivity options

  INLINE void Initialize_core_base() {
    spec_idx = 0; self_con = false;  init_wts = false;  set_scale = false;  init_wt_val = 1.0f;
    add_rnd_var = false;
  }
  // #IGNORE
    

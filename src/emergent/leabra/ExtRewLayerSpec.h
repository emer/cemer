// this is included directly in LeabraExtraLayerSpecs_cpp / _cuda
// {

  enum RewardType {		// how do we get the reward values?
    OUT_ERR_REW,		// get rewards as a function of errors on the output layer ONLY WHEN RewTarg layer act > .5 -- get from markerconspec from output layer(s)
    EXT_REW,			// get rewards as external inputs marked as ext_flag = TARG to the first unit in the layer (if ext val == norew_val, then no ext rew signaled)
    DA_REW			// get rewards from da values on first unit in layer (if ext val == norew_val, then no ext rew signaled)
  };

  RewardType	rew_type;	// how do we get the reward values?
  STATE_CLASS(OutErrSpec)	out_err;	// #CONDEDIT_ON_rew_type:OUT_ERR_REW how to compute external rewards based on output performance
  STATE_CLASS(ExtRewSpec)	rew;		// misc reward computation specifications


  INLINE virtual void Compute_Rew(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net) {
    if(rew_type == OUT_ERR_REW)
      Compute_OutErrRew(lay, net);
    else if(rew_type == EXT_REW)
      Compute_ExtRew(lay, net);
    else if(rew_type == DA_REW)
      Compute_DaRew(lay, net);
  }
  // #CAT_ExtRew overall compute reward function: calls appropriate sub-function based on rew_type
    INIMPL virtual void Compute_OutErrRew(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net);
    // #CAT_ExtRew get reward value based on external error (put in da val, clamp)
      INIMPL virtual bool OutErrRewAvail(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net);
      // #CAT_ExtRew figure out if reward is available on this trial (look if target signals are present)
      INIMPL virtual float GetOutErrRew(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net);
      // #CAT_ExtRew get reward value based on error at layer with MarkerConSpec connection: 1 = rew (correct), 0 = err, -1 = no info
    INIMPL virtual void Compute_ExtRew(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net);
    // #CAT_ExtRew get external rewards from inputs (put in da val, clamp)
    INIMPL virtual void Compute_DaRew(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net);
    // #CAT_ExtRew clamp external rewards as da values (put in da val, clamp)

    INIMPL virtual void Compute_UnitDa
      (LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net, int gpidx, float er, LEABRA_UNIT_STATE* u);
    // #CAT_ExtRew used in above routines: actually compute the unit da value based on external reward value er
    INIMPL virtual void Compute_NoRewAct(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net);
    // #CAT_ExtRew used in above routines: clamp norew_val values for when no reward information is present

  INIMPL void	Compute_HardClamp_Layer(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net) override;

  INLINE void Initialize_core() {
    rew_type = OUT_ERR_REW;
  }
  // #IGNORE

  INLINE int  GetStateSpecType() const override
  { return LEABRA_NETWORK_STATE::T_ExtRewLayerSpec; }
  

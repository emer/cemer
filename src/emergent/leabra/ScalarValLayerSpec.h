  STATE_CLASS(ScalarValSpec)	 scalar;	// specifies how values are represented in terms of distributed patterns of activation across the layer
  STATE_CLASS(MinMaxRange)	 unit_range;	// range of values represented across the units; for GAUSSIAN, add extra values above and below true useful range to prevent edge effects.
  STATE_CLASS(ScalarValBias)	 bias_val;	// specifies bias for given value (as gaussian bump) 
  STATE_CLASS(MinMaxRange)       avg_act_range; // #CONDSHOW_ON_scalar.rep:AVG_ACT range of variability of the average layer activity, used for AVG_ACT type to renormalize acts.avg before projecting it into the unit_range of values
  STATE_CLASS(MinMaxRange)	 val_range;	// #READ_ONLY #NO_INHERIT actual range of values (scalar.min/max taking into account un_range)

  INIMPL virtual void	Compute_BiasVal(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net);
  // #CAT_ScalarVal initialize the bias value 
    INIMPL virtual void Compute_WtBias_Val(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net,
                                           int gpidx, float val);
    // #IGNORE
    INIMPL virtual void Compute_UnBias_Val(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net,
                                           int gpidx, float val);
    // #IGNORE
    INIMPL virtual void Compute_UnBias_NegSlp(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net,
                                              int gpidx);
    // #IGNORE
    INIMPL virtual void Compute_UnBias_PosSlp(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net,
                                              int gpidx);
    // #IGNORE

  INIMPL virtual void	ClampValue_ugp(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net,
                                       int gpidx, float rescale=1.0f);
  // #CAT_ScalarVal clamp value in the first unit's ext field to the units in the group
  INIMPL virtual float	ClampAvgAct(int ugp_size);
  // #CAT_ScalarVal computes the average activation for a clamped unit pattern (for computing rescaling)
  INIMPL virtual void	ReadValue(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net);
  // #CAT_ScalarVal read out current value represented by activations in layer
    INIMPL virtual float ReadValue_ugp(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net,
                                int gpidx);
    // #CAT_ScalarVal unit group version: read out current value represented by activations in layer
  INIMPL virtual void 	Compute_ExtToPlus_ugp(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net,
                                      int gpidx);
  // #CAT_ScalarVal copy ext values to act_p -- used for internally-generated training signals for learning in several subclasses
  INIMPL virtual void 	Compute_ExtToAct_ugp(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net,
                                     int gpidx);
  // #CAT_ScalarVal copy ext values to act -- used for dynamically computed clamped layers
  INIMPL virtual void HardClampExt(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net);
  // #CAT_ScalarVal hard clamp current ext values (on all units, after ClampValue called) to all the units
    INIMPL virtual void HardClampExt_ugp(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net,
                                  int gpidx);
    // #IGNORE

  INLINE void  Init_Weights_Layer(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net) override {
    inherited::Init_Weights_Layer(lay, net);
    Compute_BiasVal(lay, net);
    // if(scalar.init_nms)
    //   LabelUnits(lay, net);
  }
    
  INIMPL void  Quarter_Init_Layer(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net) override;
  INLINE void  Quarter_Init_Layer_Post(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net) override;
  
  INLINE void  Quarter_Final_GetMinus(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net) override {
    inherited::Quarter_Final_GetMinus(lay, net);
    UNIT_GP_ITR(lay, Quarter_Final_GetMinus_ugp(lay, net, gpidx); );
  }

  INLINE virtual void Quarter_Final_GetMinus_ugp(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net,
                                                 int gpidx) {
    LEABRA_UNGP_STATE* ug = (LEABRA_UNGP_STATE*)lay->GetUnGpState(net, gpidx);
    LEABRA_UNIT_STATE* u = (LEABRA_UNIT_STATE*)ug->GetUnitState(net, 0);
    u->misc_2 = u->misc_1;      // save minus phase in misc_2
  }
  // #IGNORE

  INLINE void  Compute_CycleStats_Pre(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net) override {
    inherited::Compute_CycleStats_Pre(lay, net);
    ReadValue(lay, net);
  }

  INIMPL float Compute_SSE(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net, int& n_vals,
                           bool unit_avg = false, bool sqrt = false) override;
  INIMPL virtual float Compute_SSE_ugp(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net,
                                int gpidx, int& n_vals);
    // #IGNORE
  INIMPL float Compute_NormErr(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net) override;
  INIMPL virtual float Compute_NormErr_ugp(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net,
                                           int gpidx);
    // #IGNORE

  INLINE void Initialize_core() {
  }
  // #IGNORE

  INLINE int  GetStateSpecType() const override
  { return LEABRA_NETWORK_STATE::T_ScalarValLayerSpec; }


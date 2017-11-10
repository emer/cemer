// this is included directly in LeabraExtraLayerSpecs_cpp / _cuda
// {

  STATE_CLASS(TwoDValSpec)       twod;		// specifies how values are represented in terms of distributed patterns of activation across the layer
  STATE_CLASS(MinMaxRange)       x_range;	// range of values represented across the X (horizontal) axis; for GAUSSIAN, add extra values above and below true useful range to prevent edge effects.
  STATE_CLASS(MinMaxRange)       y_range;	// range of values represented across the Y (vertical) axis; for GAUSSIAN, add extra values above and below true useful range to prevent edge effects.
  STATE_CLASS(TwoDValBias)       bias_val;	// specifies bias values
  STATE_CLASS(MinMaxRange)       x_val_range;	// #READ_ONLY #NO_INHERIT actual range of values (scalar.min/max taking into account un_range)
  STATE_CLASS(MinMaxRange)       y_val_range;	// #READ_ONLY #NO_INHERIT actual range of values (scalar.min/max taking into account un_range)

  INIMPL virtual void  ClampValue_ugp(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net, int gpidx,
			       float rescale=1.0f);
  // #CAT_TwoDVal clamp value in the first unit's ext field to the units in the group
  INIMPL virtual void  ReadValue(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net);
  // #CAT_TwoDVal read out current value represented by activations in layer
  INIMPL virtual void  ReadValue_ugp(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net, int gpidx);
    // #CAT_TwoDVal unit group version: read out current value represented by activations in layer
  INIMPL virtual void  HardClampExt(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net);
  // #CAT_ScalarVal hard clamp current ext values (on all units, after ClampValue called) to all the units
  INIMPL virtual void  HardClampExt_ugp(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net,
                                        int gpidx);
  // #IGNORE

  INIMPL virtual void	LabelUnits(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net);
  // #CAT_TwoDVal label units in given layer with their underlying values
  INIMPL virtual void   LabelUnits_ugp(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net, int gpidx);
  // #CAT_TwoDVal label units with their underlying values
  INIMPL virtual void	LabelUnitsNet(LEABRA_NETWORK_STATE* net);
  // #BUTTON #CAT_TwoDVal label all layers in given network using this spec

  INIMPL virtual void	Compute_BiasVal(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net);
  // #CAT_TwoDVal initialize the bias value 
    INIMPL virtual void Compute_WtBias_Val(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net, int gpidx,
                                           float x_val, float y_val);
    // #IGNORE
    INIMPL virtual void Compute_UnBias_Val(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net, int gpidx,
                                           float x_val, float y_val);
    // #IGNORE

  INLINE void  Init_Weights_Layer(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net) override {
    inherited::Init_Weights_Layer(lay, net);
    Compute_BiasVal(lay, net);
  }
      
  INIMPL void	Quarter_Init_Layer(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net) override;
    INIMPL void Quarter_Init_TargFlags_Layer(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net) override;
    INIMPL virtual void Quarter_Init_TargFlags_Layer_ugp
      (LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net, int gpidx);
    // #IGNORE
  INLINE void  Quarter_Init_Layer_Post(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net) override;
  
  INLINE void  Compute_CycleStats_Pre(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net) override {
    inherited::Compute_CycleStats_Pre(lay, net);
    ReadValue(lay, net);
  }

  INLINE void  Quarter_Final_Layer(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net) override {
    inherited::Quarter_Final_Layer(lay, net);
    UNIT_GP_ITR(lay, Quarter_Final_ugp(lay, net, gpidx); );
  }
  
  INIMPL virtual void Quarter_Final_ugp(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net, int gpidx);
  // #CAT_TwoDVal unit group version: update variables based on phase

  INIMPL float Compute_SSE(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net, int& n_vals,
                           bool unit_avg = false, bool sqrt = false) override;
  INIMPL virtual float Compute_SSE_ugp
    (LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net, int gpidx, int& n_vals);
    // #IGNORE
  INIMPL float Compute_NormErr(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net) override;
  INIMPL virtual float Compute_NormErr_ugp
    (LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net,  int gpidx);
    // #IGNORE
    
  INLINE void Initialize_core() {
    // gaussian:
    x_range.min = -0.5f;   x_range.max = 1.5f; x_range.UpdateRange();
    y_range.min = -0.5f;   y_range.max = 1.5f; y_range.UpdateRange();
    twod.InitRange(x_range.min, x_range.range, y_range.min, y_range.range);
    x_val_range.min = x_range.min + (.5f * twod.un_width_x);
    x_val_range.max = x_range.max - (.5f * twod.un_width_x);
    y_val_range.min = y_range.min + (.5f * twod.un_width_y);
    y_val_range.max = y_range.max - (.5f * twod.un_width_y);
    x_val_range.UpdateRange(); y_val_range.UpdateRange();
  }
  // #IGNORE

  INLINE int  GetStateSpecType() const override
  { return LEABRA_NETWORK_STATE::T_TwoDValLayerSpec; }
  

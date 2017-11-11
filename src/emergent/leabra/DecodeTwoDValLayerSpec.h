// this is included directly in LeabraExtraLayerSpecs_cpp / _cuda
// {

  INIMPL void  ReadValue_ugp(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net, int gpidx) override;

  INLINE void  Compute_Inhib(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net) override { };

  INLINE void Initialize_core() {
  }
  // #IGNORE

  INLINE int  GetStateSpecType() const override
  { return LEABRA_NETWORK_STATE::T_DecodeTwoDValLayerSpec; }

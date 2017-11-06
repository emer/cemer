// this is included directly in LeabraExtraUnitSpecs_cpp / _cuda
// {

  enum PoolingFun {             // how to pool the inputs
    MAX_POOL,                   // our activation is max over our inputs
    AVG_POOL,                   // our activation is average over our inputs
  };

  PoolingFun    pool_fun;        // how to pool the inputs
 
  INIMPL virtual void Compute_PooledAct(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no);
  // set current act from current inputs

  INLINE void	Compute_NetinInteg(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) override { };
  INLINE void	Compute_Act_Rate(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) override {
    Compute_PooledAct(u, net, thr_no);
  }
  
  INLINE void	Compute_Act_Spike(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) override {
    Compute_PooledAct(u, net, thr_no);
  }

  INLINE void 	Compute_dWt(UNIT_STATE* u, NETWORK_STATE* net, int thr_no) override { };
  INLINE void	Compute_Weights(UNIT_STATE* u, NETWORK_STATE* net, int thr_no) override { };

  INLINE void Initialize_core() {
    pool_fun = MAX_POOL;
  }
  // #IGNORE

  INLINE int  GetStateSpecType() const override
  { return LEABRA_NETWORK_STATE::T_PoolInputsUnitSpec; }


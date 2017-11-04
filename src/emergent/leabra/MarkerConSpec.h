// this is included directly in LeabraExtraConSpecs_cpp / _cuda
// {

  INLINE bool  IsMarkerCon() override { return true; }
  INLINE bool  DoesStdNetin() override { return false; }
  INLINE bool  DoesStdDwt() override { return true; } //

  // don't send regular net inputs or learn
  INLINE void  Send_NetinDelta(LEABRA_CON_STATE* cg, LEABRA_NETWORK_STATE* net, int thr_no, 
                               const float su_act_delta) override { };
  INLINE float Compute_Netin(CON_STATE* cg, NETWORK_STATE* net, int thr_no) override
  { return 0.0f; }
  INLINE void  Compute_dWt(CON_STATE* cg, NETWORK_STATE* net, int thr_no) override { };
  INLINE void  Compute_Weights(CON_STATE* cg, NETWORK_STATE* net, int thr_no) override { };

  INLINE void Initialize_core() {
    rnd.mean = 0.0f; rnd.var = 0.0f;
    wt_limits.sym = false;
    wt_scale.rel = 0.0f;
    lrate = 0.0f;
    cur_lrate = 0.0f;
    learn = false;
  }
  // #IGNORE

  INLINE int  GetStateSpecType() const override
  { return LEABRA_NETWORK_STATE::T_MarkerConSpec; }


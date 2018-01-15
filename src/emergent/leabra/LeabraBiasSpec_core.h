// this contains core shared code, and is included directly in LeabraBiasSpec.h, _cpp.h, _cuda.h
//{
#ifdef __MAKETA__
  bool          learn;          // #CAT_Learning #DEF_false individual control over whether learning takes place in bias weights -- if false, no learning will take place regardless of any other settings -- if true, learning will take place if it is enabled at the network and other relevant levels -- default is OFF for bias weights, because they are generally not needed for larger models, and can only really get in the way -- turning them off also results in a small speed improvement
#endif

  float         dwt_thresh;  // #CONDSHOW_ON_learn #DEF_0.1 #MIN_0 #CAT_Learning don't change if dwt < thresh, prevents buildup of small changes

  INLINE int  GetStateSpecType() const override { return LEABRA_NETWORK_STATE::T_LeabraBiasSpec; }

  INLINE void  Trial_Init_Specs(LEABRA_NETWORK_STATE* net) override {
    // do NOT apply wt_bal for bias specs!
    if(learn) {
      net->net_misc.bias_learn = true;
    }
  }

  INLINE void B_Compute_dWt(UNIT_STATE* u, NETWORK_STATE* net, int thr_no) override {
    if(!learn) return;
    LEABRA_UNIT_STATE* uv = (LEABRA_UNIT_STATE*)u;
    // only err is useful contributor to this learning
    float dw = uv->avg_s - uv->avg_m;
    if(fabsf(dw) >= dwt_thresh) {
      uv->bias_dwt += cur_lrate * dw;
    }
  }

  INLINE void Initialize_core() {
    learn = false;
    dwt_thresh = 0.1f;
  }

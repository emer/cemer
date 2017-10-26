// this contains core shared code, and is included directly in BpUnitSpec.h, _cpp.h, _cuda.h
//{

#define SIGMOID_MAX_VAL 0.999999f
#define SIGMOID_MIN_VAL 0.000001f
#define SIGMOID_MAX_NET 13.81551f
  
  enum BpActFun {               // backprop activation function to use
    SIGMOID,                    // standard sigmoidal activation in 0-1 range: 1 / (1 + e^-net) -- note that we dropped gain and offset params in 8.0 -- just uses standard gain = 1, 0 offset
    TANH,                       // hyperbolic tangent function -- basically a sigmoid centered around 0 with range -1..1 (which is how we implement it)
    RELU,                       // rectified linear unit, i.e., a simple threshold nonlinearity --- if netinput < 0, activation = 0, else act = net
    LINEAR,                     // purely linear output -- not suitable for hidden units, as some form of nonlinearity is essential, but can be useful for output units
    NLXX1,                      // noisy-linear version of XX1 function = x/(x+1) (= 1/(1+x^1) -- sigmoidal asymptote but hard 0 threshold) where x = netin -- NL version adds a small linear "ramp" for negative inputs starting at ramp_start negative input and reaching a magnitude of ramp_max at 0, after which point it transitions to regular XX1 (starting at ramp_max) -- ramp approximates effects of convolving XX1 with gaussian noise, but with a simpler function -- provides a hybrid of sigmoid and ReLu with good overall properties for sparse representations with bias set to -2
    BINARY,                     // stochastic binary activation -- produces a 1 or a 0 based on sigmoidal underlying probability
    GAUSS,                      // gaussian applied to the standard dot-product netinput -- also known as a 'bump' function
    RBF,                        // radial basis function activation -- uses distance-based net input (net input is distance between activations and weights, instead of usual dot product) and runs that through a gaussian function to produce a radial basis function kernel activation
    MAX_POOL,                   // compute the activation = max over input activations, and send backprop error only back to max input, rest = 0
    SOFTMAX,                    // soft-max over the units within the layer -- does a few passes to compute activation -- useful for single-winner output layers (e.g., localist classification outputs) -- unlike in 8.0, this does NOT require an exponential layer input -- everything is done internally -- implies CROSS_ENTROPY error so that derivative is simple, and is ONLY applicable to output (TARGET) layers
  };

  enum BpErrFun {                // type of error function to use -- replaces err_fun setting from earlier versions 
    SQUARED_ERR,                 // use the squared error function in computing derivatives of Target layer units -- appropriate for unbounded linear targets
    CROSS_ENTROPY,               // use the cross-entropy (sigmoid-based) error function in computing derivatives of Target layer units -- appropriate for binary targets
  };

  enum BpNoiseType {
    NO_NOISE,                   // no noise
    ACT_NOISE,                  // add noise to activations
    NETIN_NOISE,                // add noise to netinputs
    DROPOUT,                    // drop out (zero activations) of units according to probability of noise.mean (like multiplying by a bernoulli distribution)
  };

  BpActFun      act_fun;        // activation function to use -- note that act_range is ignored for most functions except linear-based ones, and for output units using the cross-entropy error function
  BpErrFun      error_fun;      // error function to use: only applicable to layers of type TARGET -- squared error is appropriate for unbounded linear targets, while cross-entropy is more appropriate for binary targets
  float		err_tol;	// #DEF_0.05;0 error tolerance: no error signal for a unit if |targ-act| < err_tol) (i.e., as if act == targ exactly) -- often useful to set to .05 or so to prevent over-learning with binary training signals -- big weights often needed to get very high or low activations
  bool          save_err;       // whether to save the actual error value -- this requires extra computation and is not necessary for learning -- just for instructional / informational purposes
  STATE_CLASS(NLXX1ActSpec)  nlxx1; // #CONDSHOW_ON_act_fun:NLXX1 specs for nlxx1 function when that is being used
  STATE_CLASS(GaussActSpec)  gauss; // #CONDSHOW_ON_act_fun:GAUSS||act_fun:RBF specs for Gaussian bump or RBF activation function when that is being used
  BpNoiseType   noise_type;     // type of noise to use
  STATE_CLASS(Random)        noise; // #CONDSHOW_OFF_noise_type:NO_NOISE distribution parameters for random added noise
  
  INLINE int  GetStateSpecType() const override { return BP_NETWORK_STATE::T_BpUnitSpec; }

  // these methods keep sigmoidal-type values within the correct range to prevent numerical errors
  INLINE float  ClipSigAct(float y)
  { y = fmaxf(y,SIGMOID_MIN_VAL); y = fminf(y,SIGMOID_MAX_VAL); return y; }
  INLINE float  ClipTanhAct(float y)
  { y = fmaxf(y,-SIGMOID_MAX_VAL); y = fminf(y,SIGMOID_MAX_VAL); return y; }
  INLINE float  ClipSigNet(float x)
  { x = fmaxf(x,-SIGMOID_MAX_NET); x = fminf(x,SIGMOID_MAX_NET); return x; }

  // different activation and error derivative functions
  INLINE float  SigmoidFun(float netin)
  { return ClipSigAct(1.0f / (1.0f + expf(-ClipSigNet(netin)))); }
  INLINE float  SigmoidDeriv(float act)
  { act = ClipSigAct(act); return act * (1.0f - act); }
  INLINE float  TanhFun(float netin)
  { return ClipTanhAct( (2.0f / (1.0f + expf(-ClipSigNet(netin)))) - 1.0f); }
  INLINE float  TanhDeriv(float act)
  { act = ClipSigAct(0.5f * (act + 1.0f)); return act * (1.0f - act); }
  INLINE float  ReLuFun(float netin)
  { return fmaxf(netin, 0.0f); }
  INLINE float  ReLuDeriv(float act) { return (act > 0.0f) ? 1.0f : 0.0f; }
  INLINE float  NLXX1Fun(float netin) { return nlxx1.NLXX1Fun(netin); }
  INLINE float  NLXX1Deriv(float netin) { return nlxx1.NLXX1Deriv(netin); }

  INLINE float  ActFromNetin(float netin, int thr_no) {
    switch(act_fun) {
    case SIGMOID:
      return SigmoidFun(netin);
    case TANH:
      return TanhFun(netin);
    case RELU:
      return act_range.Clip(ReLuFun(netin));
    case LINEAR:
      return act_range.Clip(netin);
    case NLXX1:
      return NLXX1Fun(netin);
    case BINARY:
      return Random::BoolProb(SigmoidFun(netin), thr_no) ? 1.0f : 0.0f;
    case GAUSS:
      return gauss.GaussActFun(netin);
    case RBF:
      return gauss.GaussActFun(netin);
    case MAX_POOL:
      return netin;
    default:
      return 0.0f;
    }
  }
  // compute activation from netinput

  INLINE float ActDeriv(float netin, float act, int thr_no) {
    switch(act_fun) {
    case SIGMOID:
      return SigmoidDeriv(act);
    case TANH:
      return TanhDeriv(act);
    case RELU:
      return ReLuDeriv(act);
    case LINEAR:
      return 1.0f;
    case NLXX1:
      return NLXX1Deriv(netin);
    case BINARY:
      return act == 0.0f ? 0.0f : 1.0f;
    case GAUSS:
      return gauss.GaussActDeriv(act, netin);
    case RBF:
      return -act;
    case SOFTMAX:
      return 1.0f;
    case MAX_POOL:
      return 1.0f;
    default:
      return 0.0f;
    }
  }
  // compute derivative of activation from either netin or act
  
  // generic unit functions
  INLINE void Init_Acts(UNIT_STATE* u, NETWORK_STATE* net, int thr_no) override {
    inherited::Init_Acts(u, net, thr_no);
    BP_UNIT_STATE* bu = (BP_UNIT_STATE*)u;
    bu->err = bu->dEdA = bu->dEdNet = 0.0f;
    bu->misc1 = 0.0f;
    u->ClearExtFlag(UNIT_STATE::UN_FLAG_1);
  }
  
  INLINE void Compute_Netin(UNIT_STATE* u, NETWORK_STATE* net, int thr_no) override {
    if(u->HasExtFlag(UNIT_STATE::EXT)) return; // don't compute on clamped inputs
    BP_UNIT_STATE* bu = (BP_UNIT_STATE*)u;
    if(act_fun == MAX_POOL) {
      Compute_MaxPoolNetin(bu, (BP_NETWORK_STATE*)net, thr_no);
    }
    else if(act_fun == RBF) {
      float new_net = 0.0f;
      const int nrcg = net->ThrUnNRecvConGps(thr_no, u->thr_un_idx);
      for(int g=0; g<nrcg; g++) {
        CON_STATE* rgp = net->ThrUnRecvConState(thr_no, u->thr_un_idx, g);
        if(rgp->NotActive()) continue;
        new_net += rgp->GetConSpec(net)->Compute_Dist(rgp, net, thr_no);
      }
      u->net = new_net;
    }
    else {
      inherited::Compute_Netin(u, net, thr_no);

      if(act_fun == SOFTMAX) {
        float expnet = u->net;
        expnet = fmaxf(expnet, -50.0f);
        expnet = fminf(expnet, 50.0f);
        bu->misc1 = expf(expnet);
      }
    }
  }
  
  INLINE void Compute_Act(UNIT_STATE* u, NETWORK_STATE* net, int thr_no) override {
    if(noise_type == NETIN_NOISE) {
      u->net += noise.Gen(thr_no);
    }

    if(u->HasExtFlag(UNIT_STATE::EXT)) {
      u->act = u->ext;
    }
    else {
      if(act_fun == SOFTMAX) {
        BP_UNIT_STATE* bu = (BP_UNIT_STATE*)u;
        u->act = bu->misc1 / bu->err; // err temporarily contains sum of misc1 = exp(net) terms
      }
      else {
        u->act = ActFromNetin(u->net, thr_no);
      }
    }

    if(noise_type == ACT_NOISE) {
      u->act += noise.Gen(thr_no);
      act_range.Clip(u->act);
    }
    else if(noise_type == DROPOUT) {
      if(Random::BoolProb(noise.mean, thr_no)) {
        u->act = 0.0f;            // blank it
        u->SetExtFlag(UNIT_STATE::UN_FLAG_1); // dropout flag
      }
      else {
        u->ClearExtFlag(UNIT_STATE::UN_FLAG_1);
      }      
    }
  }
  
  INLINE void Compute_dWt(UNIT_STATE* u, NETWORK_STATE* net, int thr_no) override {
    if(u->HasExtFlag(UNIT_STATE::EXT))  return; // don't compute dwts for clamped units
    if(u->HasExtFlag(UNIT_STATE::UN_FLAG_1)) return; // dropout flag
    inherited::Compute_dWt(u, net, thr_no);
  }
    
  INLINE void Compute_Weights(UNIT_STATE* u, NETWORK_STATE* net, int thr_no) override {
    if(u->HasExtFlag(UNIT_STATE::EXT))  return; // don't update for clamped units
    inherited::Compute_Weights(u, net, thr_no);
  }


  INLINE virtual void 	Compute_Error(BP_UNIT_STATE* u, BP_NETWORK_STATE* net, int thr_no) {
    if(!u->HasExtFlag(UNIT_STATE::TARG)) return;
    if(u->HasExtFlag(UNIT_STATE::UN_FLAG_1)) return; // dropout flag

    float err = u->targ - u->act;
    if(fabsf(err) < err_tol) {
      if(save_err)
        u->err = 0.0f;
    }
    else {
      if(error_fun == SQUARED_ERR || act_fun == SOFTMAX) { // softmax always uses this!
        if(net->train_mode == NETWORK_STATE::TRAIN) {
          u->dEdA += err;
        }
        if(save_err) {
          u->err = err * err;
        }
      }
      else {                        // CROSS_ENTROPY
        err /= (u->act - act_range.min) * (act_range.max - u->act) * act_range.scale;
        if(net->train_mode == NETWORK_STATE::TRAIN) {
          u->dEdA += err;
        }
        if(save_err) {
          float a = ClipSigAct(act_range.Normalize(u->act));
          float t = act_range.Normalize(u->targ);
          u->err = (t * logf(a) + (1.0f - t) * logf(1.0f - a));
        }
      }
    }
  }
  // call the unit error function (only on target units)

  INLINE virtual void 	Compute_dEdA(BP_UNIT_STATE* u, BP_NETWORK_STATE* net, int thr_no) {
    // note: this has to be done at unit level b/c of sequencing with dEdNet etc
    // don't compute to inputs by default
    u->dEdA = 0.0f;
    u->err = 0.0f;
    if(u->HasExtFlag(UNIT_STATE::EXT) && !net->bp_to_inputs) return;
    if(u->HasExtFlag(UNIT_STATE::UN_FLAG_1)) return; // dropout flag
    const int nscg = net->ThrUnNSendConGps(thr_no, u->thr_un_idx);
    for(int g=0; g<nscg; g++) {
      CON_STATE* sgp = net->ThrUnSendConState(thr_no, u->thr_un_idx, g);
      if(sgp->NotActive()) continue;
      LAYER_STATE* tolay = sgp->GetPrjnSendLayer(net);
      BP_UNIT_SPEC_CPP* tous = (BP_UNIT_SPEC_CPP*)tolay->GetUnitSpec(net);
      if(tous->act_fun == BP_UNIT_SPEC_CPP::MAX_POOL) {
        for(int i=0; i<sgp->size; i++) {
          int su_idx = (int)((BP_UNIT_STATE*)sgp->UnState(i,net))->misc1;
          if(su_idx == u->flat_idx) { // it likes us!
            u->dEdA += ((BP_UNIT_STATE*)sgp->UnState(i,net))->dEdNet;
          }
        }
      }
      else {
        u->dEdA += ((BP_CON_SPEC_CPP*)sgp->GetConSpec(net))->Compute_dEdA(sgp, net, thr_no);
      }
    }
  }
  // compute derivative of error with respect to unit activation
  
  INLINE virtual void 	Compute_dEdNet(BP_UNIT_STATE* u, BP_NETWORK_STATE* net, int thr_no) {
    if(u->HasExtFlag(UNIT_STATE::EXT) && !net->bp_to_inputs) {
      u->dEdNet = 0.0f;
      return;
    }
    if(u->HasExtFlag(UNIT_STATE::UN_FLAG_1)) { // dropout flag
      u->dEdNet = 0.0f;
      return;
    }
    u->dEdNet = u->dEdA * ActDeriv(u->net, u->act, thr_no);
  }
  // compute derivative of error with respect to unit net input

  INLINE void   Compute_dEdA_dEdNet(BP_UNIT_STATE* u, BP_NETWORK_STATE* net, int thr_no)
  { Compute_dEdA(u, net, thr_no); Compute_Error(u, net, thr_no);
    Compute_dEdNet(u, net, thr_no); }
  // compute both derivatives in sequence

  INLINE virtual void Compute_MaxPoolNetin(BP_UNIT_STATE* u, BP_NETWORK_STATE* net, int thr_no) {
    const int nrcg = net->ThrUnNRecvConGps(thr_no, u->thr_un_idx);
    if(nrcg != 1) {
      net->StateError("Compute_MaxPoolNetin: must have exactly 1 recv prjn for MAX_POOL units");
      return;
    }
    CON_STATE* cg = net->ThrUnRecvConState(thr_no, u->thr_un_idx, 0);
    float max_act = cg->UnState(0,net)->act;
    int max_i = cg->UnState(0,net)->flat_idx;
    for(int i=1; i<cg->size; i++) {
      float su_act = cg->UnState(i,net)->act;
      if(su_act > max_act) {
        max_act = su_act;
        max_i = cg->UnState(i,net)->flat_idx;
      }
    }
    u->net = max_act;
    u->misc1 = (float)max_i;
  }    
  // compute netin = max over sending acts for max_pool case

  INLINE void Initialize_core() {
    act_fun = SIGMOID;    error_fun = SQUARED_ERR;    save_err = false;
    noise_type = NO_NOISE;
    act_range.min = -100.0f;       // general ranges for linear -- keep it bounded!
    act_range.max = 100.0f;

    Defaults_init();
  }
  // #IGNORE
  
  INLINE void Defaults_init() {
    err_tol = 0.05f;
    sse_tol = 0.5f;
  }
  // #IGNORE

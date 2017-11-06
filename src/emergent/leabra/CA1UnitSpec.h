// this is included directly in LeabraExtraUnitSpecs_cpp / _cuda
// {

// member includes:

// declare all other types mentioned but not required to include:

// timing of ThetaPhase dynamics -- based on quarter structure:
// [  q1      ][  q2  q3  ][     q4     ]
// [ ------ minus ------- ][ -- plus -- ]
// [   auto-  ][ recall-  ][ -- plus -- ]

//  DG -> CA3 -> CA1
//  /    /      /    \
// [----ECin---] -> [ ECout ]

// minus phase: ECout unclamped, driven by CA1
// auto-   CA3 -> CA1 = 0, ECin -> CA1 = 1
// recall- CA3 -> CA1 = 1, ECin -> CA1 = 0

// plus phase: ECin -> ECout auto clamped
// CA3 -> CA1 = 0, ECin -> CA1 = 1
// (same as auto- -- training signal for CA3 -> CA1 is what EC would produce!

// act_q1 = auto encoder minus phase state (in both CA1 and ECout
//   used by HippoEncoderConSpec relative to act_p plus phase)
// act_q3 / act_m = recall minus phase (normal minus phase dynamics for CA3 recall learning)
// act_a4 / act_p = plus (serves as plus phase for both auto and recall)

// learning just happens at end of trial as usual, but encoder projections use
// the act_q2, act_p variables to learn on the right signals

// todo: implement a two-trial version of the code to produce a true theta rhythm
// integrating over two adjacent alpha trials..

  STATE_CLASS(ThetaPhaseSpecs)       theta;  // specifications for how the theta phase cycle modulates the inputs from EC and CA3
  
  INLINE void Trial_Init_Specs(LEABRA_NETWORK_STATE* net) override {
    net->net_misc.diff_scale_p = true;
    net->net_misc.diff_scale_q1 = true;
    inherited::Trial_Init_Specs(net);
  }
  
  INIMPL void Compute_NetinScale(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) override;

  INLINE void Initialize_core() {
  }
  // #IGNORE

  INLINE int  GetStateSpecType() const override
  { return LEABRA_NETWORK_STATE::T_CA1UnitSpec; }


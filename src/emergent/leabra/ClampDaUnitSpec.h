// this is included directly in LeabraExtraUnitSpecs_cpp / _cuda
// {

  enum  SendDaMode {            // when to send da values to other layers
    CYCLE,                      // send every cycle
    PLUS_START,                 // start sending at start of plus phase
    PLUS_END,                   // send only at the end of plus phase
  };
  enum  SendDaVal {             // what da to send
    DA_P,                       // positive-valence oriented dopamine 
    DA_N,                       // negative-valence oriented dopamine 
  };

  SendDaMode    send_da;        // when to send da values
  SendDaVal     da_val;         // what da value to send to

  INIMPL virtual void  Send_Da(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no);
  // send the da value to sending projections

  INLINE void  Compute_Act_Post(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) override {
    inherited::Compute_Act_Post(u, net, thr_no);
    if(send_da == CYCLE) {
      Send_Da(u, net, thr_no);
    }
    else if(send_da == PLUS_START && net->phase == LEABRA_NETWORK_STATE::PLUS_PHASE) {
      Send_Da(u, net, thr_no);
    }
  }

  INLINE void  Quarter_Final(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) override {
    inherited::Quarter_Final(u, net, thr_no);
    if(send_da == PLUS_END && net->phase == LEABRA_NETWORK_STATE::PLUS_PHASE) {
      Send_Da(u, net, thr_no);
    }
  }

  INLINE void Initialize_core() {
    send_da = CYCLE;
    da_val = DA_P;
  }
  // #IGNORE

  INLINE int  GetStateSpecType() const override
  { return LEABRA_NETWORK_STATE::T_ClampDaUnitSpec; }

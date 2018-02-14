// this contains core shared code, and is included directly in LeabraConState _cpp.h, _cuda.h
//{

  float         net;            // #NO_SAVE #CAT_Activation netinput to this con_group: only computed for special statistics such as RelNetin
  float         net_raw;        // #NO_SAVE #CAT_Activation raw summed netinput to this con_group -- only used for NETIN_PER_PRJN
  float         wb_avg;         // #NO_SAVE #CAT_Learning average of effective weight values that exceed wt_bal.avg_thr across this con state -- used for weight balance
  float         wb_fact;        // #NO_SAVE #CAT_Learning overall weight balance factor that drives changes in wb_inc vs. wb_dec via as sigmoidal function -- this is the net strength of weigth balance changes
  float         wb_inc;         // #NO_SAVE #CAT_Learning weight balance increment factor -- extra multiplier to add to weight increases to maintain overall weight balance
  float         wb_dec;         // #NO_SAVE #CAT_Learning weight balance decrement factor -- extra multiplier to add to weight decreases to maintain overall weight balance
  float         err_dwt_max;    // #NO_SAVE #CAT_Learning maximum error-driven weight change across this set of sending connections
  float         bcm_dwt_max;    // #NO_SAVE #CAT_Learning maximum hebbian BCM weight change across this set of sending projections
  float         dwt_max;        // #NO_SAVE #CAT_Learning maximum weight change across this set of sending projections


  INLINE LEABRA_CON_SPEC_CPP* GetConSpec(NETWORK_STATE* nnet) const {
    return (LEABRA_CON_SPEC_CPP*)inherited::GetConSpec(nnet);
  }
  INLINE LEABRA_PRJN_STATE*  GetPrjnState(NETWORK_STATE* nnet) const {
    return (LEABRA_PRJN_STATE*)inherited::GetPrjnState(nnet);
  }
  INLINE LEABRA_LAYER_STATE* GetRecvLayer(NETWORK_STATE* nnet) const {
    return (LEABRA_LAYER_STATE*)inherited::GetRecvLayer(nnet);
  }
  INLINE LEABRA_LAYER_STATE* GetSendLayer(NETWORK_STATE* nnet) const {
    return (LEABRA_LAYER_STATE*)inherited::GetSendLayer(nnet);
  }
  INLINE LEABRA_UNIT_STATE*  OwnUnState(NETWORK_STATE* nnet) const {
    return (LEABRA_UNIT_STATE*)inherited::OwnUnState(nnet);
  }
  INLINE LEABRA_UNIT_STATE*  ThrOwnUnState(NETWORK_STATE* nnet, int thr_no) const {
    return (LEABRA_UNIT_STATE*)inherited::ThrOwnUnState(nnet, thr_no);
  }
  INLINE LEABRA_UNIT_STATE*  UnState(int idx, NETWORK_STATE* nnet) const {
    return (LEABRA_UNIT_STATE*)inherited::UnState(idx, nnet);
  }
  INLINE LEABRA_UNIT_STATE*  UnStateSafe(int idx, NETWORK_STATE* nnet) const {
    return (LEABRA_UNIT_STATE*)inherited::UnStateSafe(idx, nnet);
  }
  INLINE LEABRA_CON_STATE*   UnCons(int idx, NETWORK_STATE* nnet) const {
    return (LEABRA_CON_STATE*)inherited::UnCons(idx, nnet);
  }
  INLINE LEABRA_CON_STATE*   UnConsSafe(int idx, NETWORK_STATE* nnet) const {
    return (LEABRA_CON_STATE*)inherited::UnConsSafe(idx, nnet);
  }


  INLINE void  Init_ConState() {
    net = 0.0f; net_raw = 0.0f;
    wb_avg = 0.0f; wb_fact = 0.0f; wb_inc = 1.0f; wb_dec = 1.0f;
    err_dwt_max = 0.0f; bcm_dwt_max = 0.0f; dwt_max = 0.0f;
  }
  // #IGNORE leabra initialize

  INLINE void  Initialize_core() {
    Init_ConState();
  }
  // #IGNORE leabra initialize


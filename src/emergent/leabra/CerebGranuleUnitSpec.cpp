// this is included directly in LeabraExtraUnitSpecs_cpp / _cuda
// {

void STATE_CLASS(CerebGranuleUnitSpec)::Compute_GranLearnAct
  (LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) {
  
  int time_since_thr = (int)TimeSinceThr(u);
  float& act_lag = ActLag(u);
  float& act_max = ActMax(u);
  if(time_since_thr == 0) {
    act_lag = 0.0f;
    if(u->act > cereb.act_thr) {
      time_since_thr = 1;
      act_max = u->act;     // current act max
    }
    else {
      act_max = 0.0f;       // reset max always
    }
  }
  else {      // if we get here, we've crossed threshold
    time_since_thr++;
    if(time_since_thr < cereb.inhib_start_time) {
      act_max = fmaxf(act_max, u->act); // get max within time window
      act_lag = 0.0f;        // no learning yet
    }
    else if(time_since_thr < cereb.lrn_start_time) {
      act_lag = 0.0f;        // no learning yet
    }
    else if(time_since_thr <= cereb.lrn_end_time) {
      act_lag = act_max; // set learning to previous max
    }
    else {                      // at end of time window
      time_since_thr = 0;       // reset everything
      act_max = 0.0f;
      act_lag = 0.0f;
    }
  }
  TimeSinceThr(u) = (float)time_since_thr; // update counter
}


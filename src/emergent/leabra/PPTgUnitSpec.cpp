// this is included directly in LeabraExtraUnitSpecs_cpp / _cuda
// {

void STATE_CLASS(PPTgUnitSpec)::Compute_Act_Rate
  (LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) {
  
  float net_save = u->net;
  
  u->net = d_net_gain * (u->net - u->misc_1); // convert to delta OLD GUY
  //u->net = d_net_gain * (u->net - u->misc_1 + u->misc_2); // convert to delta, plus a neg-valued misc_2 rebound factor
  
  if(u->net < act_thr) { u->net = 0.0f; }
  // note: positive rectification means that trial after PV, which is often neg, will be nullified
  inherited::Compute_Act_Rate(u, net, thr_no);
  if(clamp_act) {
    u->act_eq = u->act_nd = u->act = u->net;
    u->da = 0.0f;
  }
  u->net = net_save;           // restore
}

void STATE_CLASS(PPTgUnitSpec)::Compute_Act_Spike
  (LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) {
  
  float net_save = u->net;
  u->net = d_net_gain * (u->net - u->misc_1); // convert to delta
  if(u->net < 0.0f) u->net = 0.0f;
  // note: positive rectification means that trial after PV, which is often neg, will be nullified
  inherited::Compute_Act_Spike(u, net, thr_no);
  if(clamp_act) {
    u->act_eq = u->act_nd = u->act = u->net;
    u->da = 0.0f;
  }
  u->net = net_save;           // restore
}


// this is included directly in LeabraExtraUnitSpecs_cpp / _cuda
// {

void STATE_CLASS(LeabraMultCopyUnitSpec)::Compute_MultCopy(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) {
  LEABRA_CON_STATE* copy_gp = u->RecvConState(net, 0);
  LEABRA_CON_STATE* mult_gp = u->RecvConState(net, 1);
  
  LEABRA_UNIT_STATE* copy_un = copy_gp->UnState(0,net);
  LEABRA_UNIT_STATE* mult_un = mult_gp->UnState(0,net);

  float mult_eff = mult_gain * mult_un->act_eq;;
  if(mult_eff > 1.0f) mult_eff = 1.0f;
  float new_act;
  if(one_minus)
    new_act = copy_un->act_eq * (1.0f - mult_eff);
  else
    new_act = copy_un->act_eq * mult_eff;
  u->ext = new_act;
  u->act_eq = u->act_nd = u->act = u->net = u->ext;
  u->da = 0.0f;               // I'm fully settled!
}
                                              

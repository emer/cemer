// this is included directly in LeabraExtraUnitSpecs_cpp / _cuda
// {

void STATE_CLASS(PoolInputsUnitSpec)::Compute_PooledAct
  (LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) {
  float new_act = 0.0f;
  int tot_n = 0;
  const int rsz = u->NRecvConGps(net);
  for(int g=0; g < rsz; g++) {
    LEABRA_CON_STATE* cg = u->RecvConState(net, g);
    const int sz = cg->size;
    if(pool_fun == MAX_POOL) {
      for(int i=0; i< sz; i++) {
        LEABRA_UNIT_STATE* su = cg->UnState(i, net);
        new_act = fmaxf(su->act_eq, new_act);
      }
    }
    else {                        // AVG_POOL
      for(int i=0; i< sz; i++) {
        LEABRA_UNIT_STATE* su = cg->UnState(i, net);
        new_act += su->act_eq;
      }
      tot_n += sz;
    }
  }

  if(pool_fun == AVG_POOL) {
    if(tot_n > 0) {
      new_act /= (float)tot_n;
    }
  }
    
  u->act = new_act;
  u->act_eq = u->act_nd = u->act;
  u->da = 0.0f;
  // u->AddToActBuf(syn_delay); // todo
}


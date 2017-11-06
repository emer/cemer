// this is included directly in LeabraExtraUnitSpecs_cpp / _cuda
// {

void STATE_CLASS(CA1UnitSpec)::Compute_NetinScale(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) {
  bool test_mode = (theta.use_test_mode && net->train_mode == NETWORK_STATE::TEST);
  const int nrg = u->NRecvConGps(net); 
  for(int g=0; g< nrg; g++) {
    LEABRA_CON_STATE* recv_gp = (LEABRA_CON_STATE*)u->RecvConState(net, g);
    // todo: why!!!???
    if(!recv_gp->PrjnIsActive(net)) continue; // key!! just check for prjn, not con group!
    LEABRA_LAYER_STATE* from = (LEABRA_LAYER_STATE*) recv_gp->GetSendLayer(net);
    LEABRA_CON_SPEC_CPP* cs = (LEABRA_CON_SPEC_CPP*)recv_gp->GetConSpec(net);
    if(!cs->DoesStdNetin()) continue; // skip any special guys

    if(from->LayerNameContains("EC")) {
      if(!theta.mod_ec_out && from->LayerNameContains("out"))
        continue;
      switch(net->quarter) {
      case 0:
        cs->wt_scale.abs = 1.0f;
        break;
      case 1:
        if(!test_mode)
          cs->wt_scale.abs = 0.0f;
        break;
      case 3:
        cs->wt_scale.abs = 1.0f;
        break;
      }
    }
    else if(from->LayerNameContains("CA3")) {
      switch(net->quarter) {
      case 0:
        cs->wt_scale.abs = 0.0f;
        break;
      case 1:
        cs->wt_scale.abs = 1.0f;
        break;
      case 3:
        if(!test_mode && !theta.ca3_on_p)
          cs->wt_scale.abs = 0.0f;
        break;
      }
    }
  }

  if(net->quarter == 1 && !test_mode) {
    DecayState(u, net, thr_no, theta.recall_decay);
  }
  if(net->quarter == 3 && !test_mode) {
    DecayState(u, net, thr_no, theta.plus_decay);
  }
  
  inherited::Compute_NetinScale(u, net, thr_no);
}  


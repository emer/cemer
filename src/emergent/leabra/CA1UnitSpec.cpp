// this is included directly in LeabraExtraUnitSpecs_cpp / _cuda
// {

void STATE_CLASS(CA1UnitSpec)::Compute_NetinScale
  (LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net) {
  
  bool test_mode = (theta.use_test_mode && net->train_mode == NETWORK_STATE::TEST);

  const int nrg = lay->n_recv_prjns;
  for(int g=0; g< nrg; g++) {
    LEABRA_PRJN_STATE* prjn = lay->GetRecvPrjnState(net, g);
    if(!prjn->IsActive(net)) continue;
    LEABRA_LAYER_STATE* from =  prjn->GetSendLayer(net);
    LEABRA_CON_SPEC_CPP* cs = prjn->GetConSpec(net);
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

  LEABRA_LAYER_SPEC_CPP* ls = lay->GetLayerSpec(net);
  
  if(net->quarter == 1 && !test_mode) {
    ls->DecayState(lay, net, theta.recall_decay);
  }
  if(net->quarter == 3 && !test_mode) {
    ls->DecayState(lay, net, theta.plus_decay);
  }
  
  inherited::Compute_NetinScale(lay, net);
}  


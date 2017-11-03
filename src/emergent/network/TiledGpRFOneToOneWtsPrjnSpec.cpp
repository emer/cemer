// this is included directly in AllProjectionSpecs_cpp / _cuda
// {

void STATE_CLASS(TiledGpRFOneToOneWtsPrjnSpec)::Initialize_core() {
  one_to_one_wt = 0.8f;
  other_wt = 0.2f;
  init_wts = true;
}

void STATE_CLASS(TiledGpRFOneToOneWtsPrjnSpec)::Init_Weights_Prjn
  (PRJN_STATE* prjn, NETWORK_STATE* net, int thr_no, CON_STATE* cg) {

  LAYER_STATE* recv_lay = prjn->GetRecvLayerState(net);
  LAYER_STATE* send_lay = prjn->GetSendLayerState(net);
  UNIT_STATE* ru = cg->OwnUnState(net);

  int rgpidx = ru->gp_idx;
  int rui = ru->ungp_un_idx;
  for(int i=0; i < cg->size; i++) {
    UNIT_STATE* su = cg->UnState(i,net);
    int sgpidx = su->gp_idx;
    int sui = su->ungp_un_idx;
    float wt = other_wt;
    if(sui == rui)
      wt = one_to_one_wt;
    SetCnWtScale(prjn, net, thr_no, cg, i, wt);
  }
}



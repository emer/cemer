// contains core non-inline (INIMPL) functions from _core.h
// if used, include directly in _cpp.cpp, _cuda.cpp

void PRJN_STATE::Init_Weights(NETWORK_STATE* net) {
  LAYER_STATE* recv_lay = GetRecvLayer(net);
  PRJN_SPEC_CPP* pspec = GetPrjnSpec(net);
  CON_SPEC_CPP* cs = GetConSpec(net);
  if(net->RecvOwnsCons() || pspec->init_wts) {
    for(int rui = 0; rui < recv_lay->n_units; rui++) {
      UNIT_STATE* ru = recv_lay->GetUnitState(net, rui);
      if(ru->lesioned()) continue;
      int thr_no = ru->thread_no;
      CON_STATE* cg = ru->RecvConState(net, recv_idx);
      if(!cg) continue;
      if(cg->NotActive() || cg->Sharing()) continue;
      if(pspec->init_wts) {
        pspec->Init_Weights_Prjn(this, net, thr_no, cg);
      }
      else {
        cs->Init_Weights(cg, net, thr_no);
      }
    }
  }
  else { // send owns cons, not prjn init
    LAYER_STATE* send_lay = GetSendLayer(net);
    for(int sui = 0; sui < send_lay->n_units; sui++) {
      UNIT_STATE* su = send_lay->GetUnitState(net, sui);
      if(su->lesioned()) continue;
      int thr_no = su->thread_no;
      CON_STATE* cg = su->SendConState(net, send_idx);
      if(!cg) continue;
      if(cg->NotActive()) continue;
      cs->Init_Weights(cg, net, thr_no);
    }
  }
}

void PRJN_STATE::Copy_Weights(NETWORK_STATE* net, PRJN_STATE* src) {
  LAYER_STATE* recv_lay = GetRecvLayer(net);
  LAYER_STATE* src_recv_lay = src->GetRecvLayer(net);
  PRJN_STATE* src_prjn = src->GetPrjnState(net);

  int maxn = MIN(recv_lay->n_units, src_recv_lay->n_units);
  
  for(int rui = 0; rui < maxn; rui++) {
    UNIT_STATE* ru = recv_lay->GetUnitState(net, rui);
    if(ru->lesioned()) continue;
    UNIT_STATE* sru = src_recv_lay->GetUnitState(net, rui);
    if(sru->lesioned()) continue;
    ru->Copy_Weights(net, this, sru, src_prjn);
  }
}


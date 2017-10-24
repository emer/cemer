// contains core non-inline (INIMPL) functions from _core.h
// if used, include directly in _cpp.cpp, _cuda.cpp

void UNIT_STATE::GetUnXY(NETWORK_STATE* nnet, int& un_x, int& un_y) const {
  LAYER_STATE* lay = GetOwnLayer(nnet);
  lay->GetUnXYFmIdx(ungp_un_idx, un_x, un_y);
}

void UNIT_STATE::GetUnFlatXY(NETWORK_STATE* nnet, int& un_x, int& un_y) const {
  LAYER_STATE* lay = GetOwnLayer(nnet);
  lay->GetUnFlatXYFmIdx(lay_un_idx, un_x, un_y);
}

void UNIT_STATE::GetGpXY(NETWORK_STATE* nnet, int& gp_x, int& gp_y) const {
  LAYER_STATE* lay = GetOwnLayer(nnet);
  lay->GetGpXYFmIdx(gp_idx, gp_x, gp_y);
}

void UNIT_STATE::GetGpUnXY(NETWORK_STATE* nnet, int& gp_x, int& gp_y, int& un_x, int& un_y) const {
  LAYER_STATE* lay = GetOwnLayer(nnet);
  lay->GetGpUnXYFmIdx(lay_un_idx, gp_x, gp_y, un_x, un_y);
}

CON_STATE* UNIT_STATE::FindRecvConStateFrom(NETWORK_STATE* nnet, LAYER_STATE* fm_lay) const {
  const int rsz = NRecvConGps(nnet);
  for(int g = 0; g < rsz; g++) {
    CON_STATE* cg = RecvConState(nnet, g);
    PRJN_STATE* prjn = cg->GetPrjnState(nnet);
    if(prjn && prjn->send_lay_idx == fm_lay->layer_idx)
      return cg;
  }
  return NULL;
}

CON_STATE* UNIT_STATE::FindSendConStateTo(NETWORK_STATE* nnet, LAYER_STATE* to_lay) const {
  const int ssz = NSendConGps(nnet);
  for(int g = 0; g < ssz; g++) {
    CON_STATE* cg = SendConState(nnet, g);
    PRJN_STATE* prjn = cg->GetPrjnState(nnet);
    if(prjn && prjn->recv_lay_idx == to_lay->layer_idx)
      return cg;
  }
  return NULL;
}

CON_STATE* UNIT_STATE::FindRecvConStateFromName(NETWORK_STATE* nnet, const char* fm_nm) const {
  const int rsz = NRecvConGps(nnet);
  for(int g = 0; g < rsz; g++) {
    CON_STATE* cg = RecvConState(nnet, g);
    PRJN_STATE* prjn = cg->GetPrjnState(nnet);
    LAYER_STATE* fm = prjn->GetSendLayerState(nnet);
    if(fm->LayerNameIs(fm_nm))
      return cg;
  }
  return NULL;
}

CON_STATE* UNIT_STATE::FindSendConStateToName(NETWORK_STATE* nnet, const char* to_nm) const {
  const int ssz = NSendConGps(nnet);
  for(int g = 0; g < ssz; g++) {
    CON_STATE* cg = SendConState(nnet, g);
    PRJN_STATE* prjn = cg->GetPrjnState(nnet);
    LAYER_STATE* tol = prjn->GetRecvLayerState(nnet);
    if(tol->LayerNameIs(to_nm))
      return cg;
  }
  return NULL;
}

void  UNIT_STATE::Copy_Weights(NETWORK_STATE* nnet, PRJN_STATE* prjn, const UNIT_STATE* src,
                               PRJN_STATE* src_prjn) {
  CON_STATE* cg = NULL;
  if(own_lay_idx == prjn->recv_lay_idx) { // this is a recv prjn for us
    cg = RecvConStatePrjn(nnet, prjn);
  }
  else {
    cg = SendConStatePrjn(nnet, prjn);
  }
  if(!cg) return;

  CON_STATE* scg = NULL;
  if(src->own_lay_idx == src_prjn->recv_lay_idx) { // this is a recv prjn for src
    scg = src->RecvConStatePrjn(nnet, src_prjn);
  }
  else {
    scg = src->SendConStatePrjn(nnet, src_prjn);
  }
  if(!scg) return;

  cg->Copy_Weights(scg, nnet);
}

// contains core non-inline (INIMPL) functions from _core.h
// if used, include directly in _cpp.cpp, _cuda.cpp

void LAYER_STATE::Connect_Sizes(NETWORK_STATE* net) {
  if(prjn_start_idx < 0 || n_recv_prjns == 0) return;
  for(int i = 0; i < n_recv_prjns; i++) {
    PRJN_STATE* prjn = GetRecvPrjnState(net, i);
    if(prjn->NotActive(net)) continue;
    PRJN_SPEC_CPP* pspec = prjn->GetPrjnSpec(net);
    pspec->Connect_Sizes(prjn, net);
  }
}

void LAYER_STATE::Connect_Cons(NETWORK_STATE* net) {
  if(prjn_start_idx < 0 || n_recv_prjns == 0) return;
  for(int i = 0; i < n_recv_prjns; i++) {
    PRJN_STATE* prjn = GetRecvPrjnState(net, i);
    if(prjn->NotActive(net)) continue;
    PRJN_SPEC_CPP* pspec = prjn->GetPrjnSpec(net);
    pspec->Connect_Cons(prjn, net);
  }
}

void LAYER_STATE::RecvConsPreAlloc(NETWORK_STATE* net, PRJN_STATE* prjn, int alloc_no) {
  for(int ui = 0; ui < n_units; ui++) {
    UNIT_STATE* uv = GetUnitState(net, ui);
    if(uv->lesioned()) continue;
    uv->RecvConsPreAlloc(net, prjn, alloc_no);
  }
}

void LAYER_STATE::SendConsPreAlloc(NETWORK_STATE* net, PRJN_STATE* prjn, int alloc_no) {
  for(int ui = 0; ui < n_units; ui++) {
    UNIT_STATE* uv = GetUnitState(net, ui);
    if(uv->lesioned()) continue;
    uv->SendConsPreAlloc(net, prjn, alloc_no);
  }
}

void LAYER_STATE::RecvConsPostAlloc(NETWORK_STATE* net, PRJN_STATE* prjn) {
  for(int ui = 0; ui < n_units; ui++) {
    UNIT_STATE* uv = GetUnitState(net, ui);
    if(uv->lesioned()) continue;
    uv->RecvConsPostAlloc(net, prjn);
  }
}

void LAYER_STATE::SendConsPostAlloc(NETWORK_STATE* net, PRJN_STATE* prjn) {
  for(int ui = 0; ui < n_units; ui++) {
    UNIT_STATE* uv = GetUnitState(net, ui);
    if(uv->lesioned()) continue;
    uv->SendConsPostAlloc(net, prjn);
  }
}

void LAYER_STATE::LayoutUnits(NETWORK_STATE* net) {
  if(!net->IsBuiltIntact()) return;
  int li = 0;
  if(n_ungps > 0) {
    int eff_un_sz_x = un_geom_x + gp_spc_x;
    int eff_un_sz_y = un_geom_y + gp_spc_y;
    int gi = 0;
    for(int gp_y=0; gp_y < gp_geom_y; gp_y++) {
      for(int gp_x=0; gp_x < gp_geom_x; gp_x++, gi++) {
        UNGP_STATE* ug = GetUnGpStateXY(net, gp_x, gp_y);
        ug->pos_x = gp_x * un_geom_x;
        ug->pos_y = gp_y * un_geom_y;
        ug->disp_pos_x = gp_x * eff_un_sz_x;
        ug->disp_pos_y = gp_y * eff_un_sz_y;
        
        int ui = 0;
        for(int un_y=0; un_y < un_geom_y; un_y++) {
          for(int un_x=0; un_x < un_geom_x; un_x++, li++, ui++) {
            UNIT_STATE* un = GetUnitState(net, li);
            if(!un) continue;
            // the following should already have been set, but we confirm here..
            un->lay_un_idx = li;
            un->gp_idx = gi;
            un->ungp_un_idx = ui;

            un->pos_x = un_x;   // pos is just within unit group
            un->pos_y = un_y;

            un->disp_pos_x = ug->disp_pos_x + un_x; // disp is for full layer-relative pos -- harder
            un->disp_pos_y = ug->disp_pos_y + un_y;
          }
        }
      }
    }
  }
  else {
    for(int un_y=0; un_y < un_geom_y; un_y++) {
      for(int un_x=0; un_x < un_geom_x; un_x++, li++) {
        UNIT_STATE* un = GetUnitState(net, li);
        if(!un) continue;
        un->lay_un_idx = li;
        un->gp_idx = -1;        // owned by layer group
        un->ungp_un_idx = li;

        un->pos_x = un_x;
        un->pos_y = un_y;
        un->disp_pos_x = un_x;  // same
        un->disp_pos_y = un_y;
      }
    }
  }
}

void LAYER_STATE::Init_Weights(NETWORK_STATE* net, bool recv_cons) {
  if(recv_cons) {
    if(prjn_start_idx < 0 || n_recv_prjns == 0) return;
    for(int i = 0; i < n_recv_prjns; i++) {
      PRJN_STATE* prjn = GetRecvPrjnState(net, i);
      if(prjn->NotActive(net)) continue;
      prjn->Init_Weights(net);
    }
  }
  else {
    if(send_prjn_start_idx < 0 || n_send_prjns == 0) return;
    for(int i = 0; i < n_send_prjns; i++) {
      PRJN_STATE* prjn = GetSendPrjnState(net, i);
      if(prjn->NotActive(net)) continue;
      prjn->Init_Weights(net);
    }
  }
}

void LAYER_STATE::Copy_Weights(NETWORK_STATE* net, LAYER_STATE* src, bool recv_cons) {
  if(recv_cons) {
    if(prjn_start_idx < 0 || n_recv_prjns == 0) return;
    if(src->prjn_start_idx < 0 || src->n_recv_prjns == 0) return;
    int maxn = MIN(n_recv_prjns, src->n_recv_prjns);
    for(int i = 0; i < maxn; i++) {
      PRJN_STATE* prjn = GetRecvPrjnState(net, i);
      PRJN_STATE* src_prjn = src->GetRecvPrjnState(net, i);
      if(prjn->NotActive(net) || src_prjn->NotActive(net)) continue;
      prjn->Copy_Weights(net, src_prjn);
    }
  }
  else {
    if(send_prjn_start_idx < 0 || n_send_prjns == 0) return;
    if(src->send_prjn_start_idx < 0 || src->n_send_prjns == 0) return;
    int maxn = MIN(n_send_prjns, src->n_send_prjns);
    for(int i = 0; i < maxn; i++) {
      PRJN_STATE* prjn = GetSendPrjnState(net, i);
      PRJN_STATE* src_prjn = src->GetSendPrjnState(net, i);
      if(prjn->NotActive(net) || src_prjn->NotActive(net)) continue;
      prjn->Copy_Weights(net, src_prjn);
    }
  }
}


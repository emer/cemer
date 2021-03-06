// contains core non-inline (INIMPL) functions from _core.h
// if used, include directly in _cpp.cpp, _cuda.cpp

void LAYER_STATE::Connect_Sizes(NETWORK_STATE* net) {
  if(prjn_start_idx < 0 || n_recv_prjns == 0) return;
  for(int pi = 0; pi < n_recv_prjns; pi++) {
    PRJN_STATE* prjn = GetRecvPrjnState(net, pi);
    if(prjn->NotActive(net)) continue;
    PRJN_SPEC_CPP* pspec = prjn->GetPrjnSpec(net);
    pspec->Connect_Sizes(prjn, net);
  }
}

void LAYER_STATE::Connect_Cons(NETWORK_STATE* net, int pass) {
  if(prjn_start_idx < 0 || n_recv_prjns == 0) return;
  for(int pi = 0; pi < n_recv_prjns; pi++) {
    PRJN_STATE* prjn = GetRecvPrjnState(net, pi);
    if(prjn->NotActive(net)) continue;
    PRJN_SPEC_CPP* pspec = prjn->GetPrjnSpec(net);
    pspec->Connect_Cons(prjn, net, pass);
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
  // this happens only during build so you cannot have this dependency!
  // if(!net->IsBuiltIntact()) return;
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


void LAYER_STATE::LesionState(NETWORK_STATE* net) {
  SetLayerFlag(LESIONED);
  for(int j=0; j < n_units; j++) {
    UNIT_STATE* u = GetUnitState(net, j);
    u->Lesion(net);
  }
  for(int pi = 0; pi < n_send_prjns; pi++) {
    PRJN_STATE* prjn = GetSendPrjnState(net, pi);
    prjn->LesionState(net);
  }
  for(int pi = 0; pi < n_recv_prjns; pi++) {
    PRJN_STATE* prjn = GetRecvPrjnState(net, pi);
    prjn->LesionState(net);
  }
}

void LAYER_STATE::UnLesionState(NETWORK_STATE* net) {
  ClearLayerFlag(LESIONED);
  for(int j=0; j < n_units; j++) {
    UNIT_STATE* u = GetUnitState(net, j);
    u->UnLesion(net);
  }
  for(int pi = 0; pi < n_send_prjns; pi++) {
    PRJN_STATE* prjn = GetSendPrjnState(net, pi);
    prjn->UnLesionState(net);
  }
  for(int pi = 0; pi < n_recv_prjns; pi++) {
    PRJN_STATE* prjn = GetRecvPrjnState(net, pi);
    prjn->UnLesionState(net);
  }
}


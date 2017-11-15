// this is included directly in AllProjectionSpecs_cpp / _cuda
// {


void STATE_CLASS(PFCPrjnSpec)::Connect_impl(PRJN_STATE* prjn, NETWORK_STATE* net, int make_cons) {
  
  LAYER_STATE* recv_lay = prjn->GetRecvLayerState(net);
  LAYER_STATE* send_lay = prjn->GetSendLayerState(net);
  
  int recv_y;
  int recv_x;
  bool recv_gps = true;

  if(recv_lay->HasUnitGroups() && recv_lay->gp_geom_y > 1) {
    recv_y = recv_lay->gp_geom_y;
    recv_gps = true;
  }
  else {
    recv_y = recv_lay->flat_geom_y;
    recv_gps = false;
  }
  if(recv_lay->HasUnitGroups() && recv_lay->gp_geom_x > 1) {
    recv_x = recv_lay->gp_geom_x;
  }
  else {
    recv_x = recv_lay->flat_geom_x;
  }

  for(int ry=0; ry<recv_y; ry++) {
    switch(recv_layer) {
    case PFC:
      if(!(recv_pfc_rows & (1 << ry)))
        continue;
      break;
    case GATING:
      if(!(recv_gate_rows & (1 << ry)))
        continue;
      break;
    case OTHER:
      break;
    }

    for(int rx=0; rx<recv_x; rx++) {
      if(row_1to1) {
        Connect_row1to1(prjn, net, make_cons, rx, ry, recv_x, recv_gps);
      }
      else {
        Connect_rowall(prjn, net, make_cons, rx, ry, recv_x, recv_gps);
      }
    }
  }
  
  if(!make_cons) { // on first pass through alloc loop, do allocations
    recv_lay->RecvConsPostAlloc(net, prjn);
    send_lay->SendConsPostAlloc(net, prjn);
  }
}

void STATE_CLASS(PFCPrjnSpec)::Connect_row1to1
  (PRJN_STATE* prjn, NETWORK_STATE* net, int make_cons, int rx, int ry, int recv_x, bool recv_gps) {
  
  LAYER_STATE* recv_lay = prjn->GetRecvLayerState(net);
  LAYER_STATE* send_lay = prjn->GetSendLayerState(net);

  int send_y;
  int send_x;
  bool send_gps = true;

  if(send_lay->HasUnitGroups() && send_lay->gp_geom_y > 1) {
    send_y = send_lay->gp_geom_y;
    send_gps = true;
  }
  else {
    send_y = send_lay->flat_geom_y;
    send_gps = false;
  }
  if(send_lay->HasUnitGroups() && send_lay->gp_geom_x > 1) {
    send_x = send_lay->gp_geom_x;
  }
  else {
    send_x = send_lay->flat_geom_x;
  }

  int sy = ry;

  if(recv_layer == PFC && send_layer == GATING) {
    sy = sy / 2;                // gating is 1/2 PFC
  }
  else if(recv_layer == GATING && send_layer == PFC) {
    // need to do both 2 * sy and 2 * sy + 1
    Connect_cols(prjn, net, make_cons, rx, ry, recv_x, recv_gps, 2 * sy, send_x, send_gps);
    Connect_cols(prjn, net, make_cons, rx, ry, recv_x, recv_gps, 2 * sy + 1, send_x, send_gps);
    return;                     // don't do default..
  }
  // otherwise, sy = ry either way

  Connect_cols(prjn, net, make_cons, rx, ry, recv_x, recv_gps, sy, send_x, send_gps);
}

void STATE_CLASS(PFCPrjnSpec)::Connect_rowall
  (PRJN_STATE* prjn, NETWORK_STATE* net, int make_cons, int rx, int ry, int recv_x, bool recv_gps) {

  LAYER_STATE* recv_lay = prjn->GetRecvLayerState(net);
  LAYER_STATE* send_lay = prjn->GetSendLayerState(net);

  int send_y;
  int send_x;
  bool send_gps = true;

  if(send_lay->HasUnitGroups() && send_lay->gp_geom_y > 1) {
    send_y = send_lay->gp_geom_y;
    send_gps = true;
  }
  else {
    send_y = send_lay->flat_geom_y;
    send_gps = false;
  }
  if(send_lay->HasUnitGroups() && send_lay->gp_geom_x > 1) {
    send_x = send_lay->gp_geom_x;
  }
  else {
    send_x = send_lay->flat_geom_x;
  }

  for(int sy=0; sy<send_y; sy++) {
    Connect_cols(prjn, net, make_cons, rx, ry, recv_x, recv_gps, sy, send_x, send_gps);
  }
}

void STATE_CLASS(PFCPrjnSpec)::Connect_cols
  (PRJN_STATE* prjn, NETWORK_STATE* net, int make_cons, int rx, int ry, int recv_x, bool recv_gps,
   int sy, int send_x, bool send_gps) {
  
  LAYER_STATE* recv_lay = prjn->GetRecvLayerState(net);
  LAYER_STATE* send_lay = prjn->GetSendLayerState(net);

  // send does the filtering!
  switch(send_layer) {
  case PFC:
    if(!(send_pfc_rows & (1 << sy)))
      return;
    break;
  case GATING:
    if(!(send_gate_rows & (1 << sy)))
      return;
    break;
  case OTHER:
    break;
  }

  if(unit_1to1) {
    Connect_unit1to1(prjn, net, make_cons, rx, ry, recv_x, recv_gps, sy, send_x, send_gps);
    return;
  }
  
  int rgpidx = -1;
  int rnu = 1;
  UNIT_STATE* ru = NULL;

  if(col_1to1 && recv_x == send_x) {
    int sx = rx;
    if(recv_gps) {
      rgpidx = ry * recv_x + rx;
      rnu = recv_lay->un_geom_n;
    }
    else {
      ru = recv_lay->GetUnitStateFlatXY(net, rx, ry);
    }
    for(int ri=0;ri<rnu;ri++) {
      if(rgpidx >= 0) {
        ru = recv_lay->GetUnitStateGpUnIdx(net, rgpidx, ri);
      }
      if(!ru) continue;

      int sgpidx = -1;
      int snu = 1;
      UNIT_STATE* su = NULL;
      if(send_gps) {
        sgpidx = sy * send_x + sx;
        snu = send_lay->un_geom_n;
      }
      else {
        su = send_lay->GetUnitStateFlatXY(net, sx, sy);
      }
      for(int si=0;si<snu;si++) {
        if(sgpidx >= 0) {
          su = send_lay->GetUnitStateGpUnIdx(net, sgpidx, si);
        }
        if(!su) continue;

        if(!make_cons) {
          su->SendConsAllocInc(net, prjn, 1);
          ru->RecvConsAllocInc(net, prjn, 1);
        }
        else {
          ru->ConnectFrom(net, su, prjn);
        }
      }
    }
  }
  else {                        // all to all cols
    for(int sx = 0; sx < send_x; sx++) {
      if(recv_gps) {
        rgpidx = ry * recv_x + rx;
        rnu = recv_lay->un_geom_n;
      }
      else {
        ru = recv_lay->GetUnitStateFlatXY(net, rx, ry);
      }
      for(int ri=0;ri<rnu;ri++) {
        if(rgpidx >= 0) {
          ru = recv_lay->GetUnitStateGpUnIdx(net, rgpidx, ri);
        }
        if(!ru) continue;

        int sgpidx = -1;
        int snu = 1;
        UNIT_STATE* su = NULL;
        if(send_gps) {
          sgpidx = sy * send_x + sx;
          su = send_lay->GetUnitStateGpUnIdx(net, sgpidx, ri);
        }
        else {
          su = send_lay->GetUnitStateFlatXY(net, sx, sy);
        }
        for(int si=0;si<snu;si++) {
          if(sgpidx >= 0) {
            su = send_lay->GetUnitStateGpUnIdx(net, sgpidx, si);
          }
          if(!su) continue;

          if(!make_cons) {
            su->SendConsAllocInc(net, prjn, 1);
            ru->RecvConsAllocInc(net, prjn, 1);
          }
          else {
            ru->ConnectFrom(net, su, prjn);
          }
        }
      }
    }
  }
}

void STATE_CLASS(PFCPrjnSpec)::Connect_unit1to1
  (PRJN_STATE* prjn, NETWORK_STATE* net, int make_cons, int rx, int ry, int recv_x, bool recv_gps,
   int sy, int send_x, bool send_gps) {
  
  LAYER_STATE* recv_lay = prjn->GetRecvLayerState(net);
  LAYER_STATE* send_lay = prjn->GetSendLayerState(net);

  int rgpidx = -1;
  int rnu = 1;
  int snu = 1;
  UNIT_STATE* ru = NULL;
  UNIT_STATE* su = NULL;

  if(recv_gps && !send_gps) {
    rgpidx = ry * recv_x + rx;
    rnu = recv_lay->un_geom_n;
    snu = send_lay->n_units;
    int mx = MIN(rnu, snu);
    for(int ri=0;ri<mx;ri++) {
      ru = recv_lay->GetUnitStateGpUnIdx(net, rgpidx, ri);
      if(!ru) continue;
      su = send_lay->GetUnitState(net, ri);
      if(!su) continue;

      if(!make_cons) {
        su->SendConsAllocInc(net, prjn, 1);
        ru->RecvConsAllocInc(net, prjn, 1);
      }
      else {
        ru->ConnectFrom(net, su, prjn);
      }
    }
  }
  else if(!recv_gps && send_gps) {
    rnu = recv_lay->n_units;
    snu = send_lay->un_geom_n;
    int mx = MIN(rnu, snu);
    for(int sx = 0; sx < send_x; sx++) {
      int sgpidx = sy * send_x + sx;
      for(int ri=0;ri<mx;ri++) {
        ru = recv_lay->GetUnitState(net, ri);
        if(!ru) continue;
        su = send_lay->GetUnitStateGpUnIdx(net, sgpidx, ri);
        if(!su) continue;

        if(!make_cons) {
          su->SendConsAllocInc(net, prjn, 1);
          ru->RecvConsAllocInc(net, prjn, 1);
        }
        else {
          ru->ConnectFrom(net, su, prjn);
        }
      }
    }
  }
  else if(recv_gps && send_gps) {
    rgpidx = ry * recv_x + rx;
    rnu = recv_lay->un_geom_n;
    snu = send_lay->un_geom_n;
    int sx = rx;                // assumes col_1to1 -- only sensible option
    int sgpidx = sy * send_x + sx;
    int mx = MIN(rnu, snu);
    for(int ri=0;ri<mx;ri++) {
      ru = recv_lay->GetUnitStateGpUnIdx(net, rgpidx, ri);
      if(!ru) continue;
      su = send_lay->GetUnitStateGpUnIdx(net, sgpidx, ri);
      if(!su) continue;

      if(!make_cons) {
        su->SendConsAllocInc(net, prjn, 1);
        ru->RecvConsAllocInc(net, prjn, 1);
      }
      else {
        ru->ConnectFrom(net, su, prjn);
      }
    }
  }
  else if(!recv_gps && !send_gps) {
    rnu = recv_lay->n_units;
    snu = send_lay->n_units;
    int mx = MIN(rnu, snu);
    for(int ri=0;ri<mx;ri++) {
      ru = recv_lay->GetUnitState(net, ri);
      if(!ru) continue;
      su = send_lay->GetUnitState(net, ri);
      if(!su) continue;

      if(!make_cons) {
        su->SendConsAllocInc(net, prjn, 1);
        ru->RecvConsAllocInc(net, prjn, 1);
      }
      else {
        ru->ConnectFrom(net, su, prjn);
      }
    }
  }
}

// this is included directly in AllProjectionSpecs_cpp / _cuda
// {

#define TESS_EL STATE_CLASS(TessEl)

void STATE_CLASS(TesselPrjnSpec)::Initialize_core() {
  recv_n = -1;
  recv_skip = 1;
  recv_group = 1;
  wrap = true;
  send_scale = 1.0f;
  n_send_offs = 0;
  alloc_send_offs = 0;
  send_offs_m = NULL;
}

void STATE_CLASS(TesselPrjnSpec)::AllocSendOffs(int ns) {
  if(alloc_send_offs >= ns) return;
  if(send_offs_m) {
    FreeSendOffs();
  }
  send_offs_m = new TESS_EL[ns];
  alloc_send_offs = ns;
  n_send_offs = 0;              // reset
}

void STATE_CLASS(TesselPrjnSpec)::FreeSendOffs() {
  if(send_offs_m) {
    delete [] send_offs_m;
    send_offs_m = NULL;
  }
  alloc_send_offs = 0;
  n_send_offs = 0;
}

void STATE_CLASS(TesselPrjnSpec)::Connect_impl(PRJN_STATE* prjn, NETWORK_STATE* net, bool make_cons) {
  LAYER_STATE* recv_lay = prjn->GetRecvLayerState(net);
  LAYER_STATE* send_lay = prjn->GetSendLayerState(net);

  TAVECTOR2I ru_geo;
  ru_geo.SetXY(recv_lay->flat_geom_x, recv_lay->flat_geom_y);
  TAVECTOR2I use_recv_n = recv_n;

  if(recv_n.x == -1)
    use_recv_n.x = ru_geo.x;
  if(recv_n.y == -1)
    use_recv_n.y = ru_geo.y;

  TAVECTOR2I ruc, nuc;
  for(ruc.y = recv_off.y, nuc.y = 0; (ruc.y < ru_geo.y) && (nuc.y < use_recv_n.y);
      ruc.y += recv_skip.y, nuc.y++)
    {
      for(ruc.x = recv_off.x, nuc.x = 0; (ruc.x < ru_geo.x) && (nuc.x < use_recv_n.x);
          ruc.x += recv_skip.x, nuc.x++)
        {
          UNIT_STATE* ru_u = recv_lay->GetUnitStateFlatXY(net, ruc.x, ruc.y);
          if(ru_u == NULL)
            continue;
          Connect_RecvUnit(prjn, net, ru_u, ruc, make_cons);
        }
    }

  if(!make_cons) { // on first pass through alloc loop, do sending allocations
    send_lay->SendConsPostAlloc(net, prjn);
  }
}

void STATE_CLASS(TesselPrjnSpec)::GetCtrFmRecv(TAVECTOR2I& sctr, TAVECTOR2I ruc) {
  ruc -= recv_off;
  ruc /= recv_group;    ruc *= recv_group;      // this takes int part of
  ruc += recv_off;      // then re-add offset
  TAVECTOR2F scruc = ruc;
  scruc *= send_scale;
  scruc += send_off;
  sctr.SetXY((int)scruc.x, (int)scruc.y);         // take int part at the end
}

void STATE_CLASS(TesselPrjnSpec)::Connect_RecvUnit(PRJN_STATE* prjn, NETWORK_STATE* net, UNIT_STATE* ru_u,
                                                   const TAVECTOR2I& ruc, bool make_cons) {
  LAYER_STATE* recv_lay = prjn->GetRecvLayerState(net);
  LAYER_STATE* send_lay = prjn->GetSendLayerState(net);
  if(recv_lay->n_units == 0) // an empty layer!
    return;

  // allocate cons
  if(!make_cons) {
    ru_u->RecvConsPreAlloc(net, prjn, n_send_offs);
  }

  TAVECTOR2I su_geo;
  su_geo.SetXY(send_lay->flat_geom_x, send_lay->flat_geom_y);

  // positions of center of recv in sending layer
  TAVECTOR2I sctr;
  GetCtrFmRecv(sctr, ruc);
  for(int i = 0; i< n_send_offs; i++) {
    TESS_EL* te = send_offs_m + i;
    TAVECTOR2I suc = te->send_off + sctr;
    if(suc.WrapClip(wrap, su_geo) && !wrap)
      continue;
    UNIT_STATE* su_u = send_lay->GetUnitStateFlatXY(net, suc.x, suc.y);
    if((su_u == NULL) || (!self_con && (su_u == ru_u)))
      continue;
    if(!make_cons)
      ru_u->ConnectFrom(net, su_u, prjn, true); // true = alloc_send
    else
      ru_u->ConnectFromCk(net, su_u, prjn); // check on 2nd pass
  }
}

// todo: this assumes that things are in order.. (can't really check otherwise)
// which breaks for clipped patterns
void STATE_CLASS(TesselPrjnSpec)::Init_Weights_Prjn(PRJN_STATE* prjn, NETWORK_STATE* net, int thr_no, CON_STATE* cg) {
  int mxi = MIN(cg->size, n_send_offs);
  for(int i=0; i<mxi; i++) {
    TESS_EL& te = send_offs_m[i];
    if(set_scale) {
      SetCnWtRnd(prjn, net, thr_no, cg, i);
      SetCnScale(prjn, net, thr_no, cg, i, te.wt_val);
    }
    else {
      SetCnWt(prjn, net, thr_no, cg, i, te.wt_val);
    }
  }
}


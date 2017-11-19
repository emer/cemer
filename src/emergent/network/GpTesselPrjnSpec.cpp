// this is included directly in AllProjectionSpecs_cpp / _cuda
// {

#define GPTESS_EL STATE_CLASS(GpTessEl)

void STATE_CLASS(GpTesselPrjnSpec)::Initialize_core() {
  recv_gp_n = -1;
  recv_gp_skip = 1;
  recv_gp_group = 1;
  send_gp_scale = 1.0f;

  wrap = true;
  def_p_con = 1.0f;
  sym_self = true;
  same_seed = false;
  n_send_gp_offs = 0;
  alloc_send_gp_offs = 0;
  send_gp_offs_m = NULL;
}

void STATE_CLASS(GpTesselPrjnSpec)::AllocSendOffs(int ns) {
  if(alloc_send_gp_offs >= ns) return;
  if(send_gp_offs_m) {
    FreeSendOffs();
  }
  send_gp_offs_m = new GPTESS_EL[ns];
  alloc_send_gp_offs = ns;
  n_send_gp_offs = 0;              // reset
}

void STATE_CLASS(GpTesselPrjnSpec)::FreeSendOffs() {
  if(send_gp_offs_m) {
    delete [] send_gp_offs_m;
    send_gp_offs_m = NULL;
  }
  alloc_send_gp_offs = 0;
  n_send_gp_offs = 0;
}

void STATE_CLASS(GpTesselPrjnSpec)::Connect_impl
  (PRJN_STATE* prjn, NETWORK_STATE* net, int make_cons) {

  if(same_seed)
    rndm_seed.OldSeed();

  LAYER_STATE* recv_lay = prjn->GetRecvLayer(net);
  LAYER_STATE* send_lay = prjn->GetSendLayer(net);

  if(!recv_lay->HasUnitGroups()) {
    net->StateError("GpTesselPrjnSpec::Connect_impl requires recv layer to have unit groups!");
    return;
  }
  if(!send_lay->HasUnitGroups()) {
    net->StateError("GpTesselPrjnSpec::Connect_impl requires send layer to have unit groups!");
    return;
  }

  int ru_nunits = recv_lay->un_geom_n;
  int su_nunits = send_lay->un_geom_n;

  TAVECTOR2I ru_geo;
  ru_geo.SetXY(recv_lay->gp_geom_x, recv_lay->gp_geom_y);
  TAVECTOR2I use_recv_gp_n = recv_gp_n;

  if(recv_gp_n.x == -1)
    use_recv_gp_n.x = ru_geo.x;
  if(recv_gp_n.y == -1)
    use_recv_gp_n.y = ru_geo.y;

  TAVECTOR2I ruc, nuc;
  for(ruc.y = recv_gp_off.y, nuc.y = 0; (ruc.y < ru_geo.y) && (nuc.y < use_recv_gp_n.y);
      ruc.y += recv_gp_skip.y, nuc.y++)
    {
      for(ruc.x = recv_gp_off.x, nuc.x = 0; (ruc.x < ru_geo.x) && (nuc.x < use_recv_gp_n.x);
          ruc.x += recv_gp_skip.x, nuc.x++)
        {
          int rgpidx = recv_lay->GetGpIdxFmXY(ruc.x, ruc.y);
          if(!recv_lay->GpIdxInRange(rgpidx)) continue;
          Connect_RecvGp(prjn, net, rgpidx, ruc, make_cons);
        }
    }
  if(!make_cons) { // on first pass through alloc loop, do sending allocations
    recv_lay->RecvConsPostAlloc(net, prjn);
    send_lay->SendConsPostAlloc(net, prjn);
  }
}

void STATE_CLASS(GpTesselPrjnSpec)::GetCtrFmRecv(TAVECTOR2I& sctr, TAVECTOR2I ruc) {
  ruc -= recv_gp_off;
  ruc /= recv_gp_group; ruc *= recv_gp_group;   // this takes int part of
  ruc += recv_gp_off;   // then re-add offset
  TAVECTOR2F scruc = ruc;
  scruc *= send_gp_scale;
  sctr = scruc;         // center of sending units
  sctr += send_gp_border;
}

void STATE_CLASS(GpTesselPrjnSpec)::Connect_RecvGp
  (PRJN_STATE* prjn, NETWORK_STATE* net, int rgpidx, const TAVECTOR2I& ruc, int make_cons) {

  LAYER_STATE* recv_lay = prjn->GetRecvLayer(net);
  LAYER_STATE* send_lay = prjn->GetSendLayer(net);
  int ru_nunits = recv_lay->un_geom_n;
  int su_nunits = send_lay->un_geom_n;

  TAVECTOR2I su_geo;
  su_geo.SetXY(send_lay->gp_geom_x, send_lay->gp_geom_y);
  TAVECTOR2I sctr;
  GetCtrFmRecv(sctr, ruc);  // positions of center of recv in sending layer
  for(int i = 0; i< n_send_gp_offs; i++) {
    GPTESS_EL* te = send_gp_offs_m + i;
    TAVECTOR2I suc = te->send_gp_off + sctr;
    if(suc.WrapClip(wrap, su_geo) && !wrap)
      continue;
    int sgpidx = send_lay->GetGpIdxFmXY(suc.x, suc.y);
    if(!send_lay->GpIdxInRange(sgpidx)) continue;
    // use prjnspec function:
    Connect_Gps(prjn, net, rgpidx, sgpidx, te->p_con, sym_self, make_cons);
  }
}


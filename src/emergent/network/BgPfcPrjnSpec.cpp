// this is included directly in AllProjectionSpecs_cpp / _cuda
// {

#define BGPFCPRJN_EL STATE_CLASS(BgPfcPrjnEl)

void STATE_CLASS(BgPfcPrjnSpec)::AllocPfcLayers(int ns) {
  if(alloc_pfc_layers >= ns) return;
  if(pfc_layers_m) {
    FreePfcLayers();
  }
  pfc_layers_m = new BGPFCPRJN_EL[ns];
  alloc_pfc_layers = ns;
  n_pfc_layers = 0;              // reset
}

void STATE_CLASS(BgPfcPrjnSpec)::FreePfcLayers() {
  if(pfc_layers_m) {
    delete [] pfc_layers_m;
    pfc_layers_m = NULL;
  }
  alloc_pfc_layers = 0;
  n_pfc_layers = 0;
}

void STATE_CLASS(BgPfcPrjnSpec)::Connect_impl(PRJN_STATE* prjn, NETWORK_STATE* net, int make_cons) {

  LAYER_STATE* recv_lay = prjn->GetRecvLayer(net);
  LAYER_STATE* send_lay = prjn->GetSendLayer(net);

  LAYER_STATE* pfc_lay = NULL;
  LAYER_STATE* bg_lay = NULL;

  bool found_a_pfc = false;
  bool found_a_bg = false;

  // first find out if one end of the thing is a pfc at all
  for(int i=0; i<n_pfc_layers; i++) {
    BGPFCPRJN_EL* pel = pfc_layers_m + i;
    if(recv_lay->LayerNameContains(pel->pfc_name)) {
      found_a_pfc = true;
      pfc_lay = recv_lay;
      bg_lay = send_lay;
    }
    else if(send_lay->LayerNameContains(pel->pfc_name)) {
      found_a_pfc = true;
      pfc_lay = send_lay;
      bg_lay = recv_lay;
    }
  }

  if(!found_a_pfc) {            // look for a bg
    const char* bg_names[] = {"GP", "SNr", "Matrix" };
    for(int i=0; i<3; i++) {
      if(recv_lay->LayerNameContains(bg_names[i])) {
        found_a_bg = true;
        bg_lay = recv_lay;
        pfc_lay = send_lay;
      }
      else if(send_lay->LayerNameContains(bg_names[i])) {
        found_a_bg = true;
        bg_lay = send_lay;
        pfc_lay = recv_lay;
      }
    }
  }

  if(!found_a_pfc && !found_a_bg) {
    if(recv_lay->gp_geom_n < send_lay->gp_geom_n) { // bg is usu bigger!
      pfc_lay = recv_lay;
      bg_lay = send_lay;
    }
    else {
      pfc_lay = send_lay;
      bg_lay = recv_lay;
    }
  }

  if(!found_a_pfc && !cross_connect) {
    net->StateError("BgPfcPrjnSpec::Connect_impl: could not find PFC layer from either recv layer:",
                    recv_lay->layer_name, "or send layer:", send_lay->layer_name,
                    "must use cross_connect in this case");
    return;
  }
  
  int trg_sz_x = 0;
  int trg_sz_y = 0;
  int sz_x = 0;
  int st_x = -1;
  int st_y = -1;
  for(int i=0; i<n_pfc_layers; i++) {
    BGPFCPRJN_EL* pel = pfc_layers_m + i;
    const char* nm = pel->pfc_name;
    int szx = pel->size.x;
    int szy = pel->size.y;
    int stx = pel->start.x;
    int sty = pel->start.y;

    bool got = false;
    if(cross_connect) {
      if(ConnectAsContains(nm)) {
        got = true;
      }
    }
    else {
      if(recv_lay->LayerNameContains(nm) || send_lay->LayerNameContains(nm)) {
        got = true;
      }
    }

    if(szx > 0 && szy > 0) {
      trg_sz_x = szx;
      trg_sz_y = szy;
    }
    else if(found_a_pfc) {
      trg_sz_x = pfc_lay->gp_geom_x;
      trg_sz_y = pfc_lay->gp_geom_y;
    }
    else {
      trg_sz_x = bg_lay->gp_geom_x / n_pfc_layers; // assume even
      trg_sz_y = bg_lay->gp_geom_y;
    }

    if(got) {
      if(stx < 0) {
        st_x = sz_x;
      }
      else {
        st_x = stx;
      }
      if(sty <= 0) {
        st_y = 0;
      }
      else {
        st_y = sty;
      }
      break;
    }

    if(szx > 0) {
      sz_x += szx;
    }
    else {
      sz_x += trg_sz_x;
    }
  }
    
  if(pfc_lay == NULL) {
    net->StateError("BgPfcPrjnSpec::Connect_impl: could not find PFC layer from either recv layer:",
                    recv_lay->layer_name, "or send layer:", send_lay->layer_name);
    return;
  }

  if(!bg_lay->HasUnitGroups()) {
    net->StateError("BgPfcPrjnSpec::Connect_impl: Bg layer must have unit groups", bg_lay->layer_name);
    return;
  }

  if(!found_a_pfc) {
    // do full connectivity into each of the bg groups -- typically from an input layer..
    for(int bgy = 0; bgy < trg_sz_y; bgy++) {
      for(int bgx = 0; bgx < trg_sz_x; bgx++) {
        int bggp = bgy * bg_lay->gp_geom_x + bgx;
        if(bg_lay == recv_lay) {
          Connect_Gps(prjn, net, bggp, -1, 1.0f, true, make_cons);
        }
        else {
          Connect_Gps(prjn, net, -1, bggp, 1.0f, true, make_cons);
        }
      }
    }
  
    if(!make_cons) { // on first pass through alloc loop, do allocations
      recv_lay->RecvConsPostAlloc(net, prjn);
      send_lay->SendConsPostAlloc(net, prjn);
    }
  }
  else {
    if(bg_lay->gp_geom_x < (st_x + pfc_lay->gp_geom_x)) {
      net->StateError("Connect_impl: BG layer is not big enough to hold x dimension of pfc layer:",
                      pfc_lay->layer_name);
      return;
    }
    if(bg_lay->gp_geom_y < (st_y + pfc_lay->gp_geom_y)) {
      net->StateError("Connect_impl: BG layer is not big enough to hold y dimension of pfc layer:",
                      pfc_lay->layer_name);
      return;
    }

    for(int pfcy = 0; pfcy < pfc_lay->gp_geom_y; pfcy++) {
      for(int pfcx = 0; pfcx < pfc_lay->gp_geom_x; pfcx++) {
        int pfcgp = pfcy * pfc_lay->gp_geom_x + pfcx;
        int bgx = st_x + pfcx;
        int bgy = st_y + pfcy;
        int bggp = bgy * bg_lay->gp_geom_x + bgx;
        if(pfc_lay == recv_lay) {
          Connect_Gps(prjn, net, pfcgp, bggp, 1.0f, true, make_cons);
        }
        else {
          Connect_Gps(prjn, net, bggp, pfcgp, 1.0f, true, make_cons);
        }
      }
    }
  
    if(!make_cons) { // on first pass through alloc loop, do allocations
      recv_lay->RecvConsPostAlloc(net, prjn);
      send_lay->SendConsPostAlloc(net, prjn);
    }
  }
}


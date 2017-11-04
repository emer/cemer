// this is included directly in AllPRJN_STATESpecs_cpp / _cuda
// {


void STATE_CLASS(PolarRndPrjnSpec)::Initialize_core() {
  p_con = .25;

  rnd_dist.type = STATE_CLASS(Random)::GAUSSIAN;
  rnd_dist.mean = 0.0f;
  rnd_dist.var = .25f;

  rnd_angle.type = STATE_CLASS(Random)::UNIFORM;
  rnd_angle.mean = 0.5f;
  rnd_angle.var = 0.5f;

  dist_type = XY_DIST_CENTER_NORM;
  wrap = false;
  max_retries = 1000;

  same_seed = false;
}

void STATE_CLASS(PolarRndPrjnSpec)::Connect_impl(PRJN_STATE* prjn, NETWORK_STATE* net, bool make_cons) { 
  if(same_seed)
    rndm_seed.OldSeed();

  LAYER_STATE* recv_lay = prjn->GetRecvLayerState(net);
  LAYER_STATE* send_lay = prjn->GetSendLayerState(net);

  const int rlay_no = recv_lay->n_units;
  const int slay_no = send_lay->n_units;
  int recv_no;
  if(!self_con && (send_lay == recv_lay))
    recv_no = (int) ((p_con * (float)(slay_no-1)) + .5f) + 1;
  else
    recv_no = (int) ((p_con * (float)slay_no) + .5f) + 1;
  if(recv_no <= 0) recv_no = 1;

  // sending number is even distribution across senders plus some imbalance factor
  float send_no_flt = (float)(recv_lay->n_units * recv_no) / (float)slay_no;
  if(send_no_flt < 2.0f)
    send_no_flt = 2.0f;
  // add SEM as corrective factor
  float send_sem = send_no_flt / sqrtf(send_no_flt);
  if(send_sem < 1.0f)
    send_sem = 1.0f;
  int send_no = (int)(send_no_flt + 3.0f * send_sem + 5.0f); // polar needs some extra insurance
  if(send_no > recv_lay->n_units) send_no = recv_lay->n_units;

  // pre-allocate connections!
  if(!make_cons) {
    recv_lay->RecvConsPreAlloc(net, prjn, recv_no);
    send_lay->SendConsPreAlloc(net, prjn, send_no);
    return;
  }

  TAVECTOR2I ru_geom;
  TAVECTOR2I ru_pos;             // do this according to act_geom..
  ru_geom.SetXY(recv_lay->flat_geom_x, recv_lay->flat_geom_x);
  for(int rui=0; rui < rlay_no; rui++) {
    UNIT_STATE* ru = recv_lay->GetUnitState(net, rui);
    ru_pos.SetXY(ru->pos_x, ru->pos_y);
    ConState_cpp* recv_gp = NULL;
    TAVECTOR2F suc;
    int n_con = 0;
    int n_retry = 0;
    while((n_con < recv_no) && (n_retry < max_retries)) { // limit number of retries
      float dist = rnd_dist.Gen();              // just get random deviate from distribution
      float angle = 2.0f * 3.14159265f * rnd_angle.Gen(); // same for angle
      suc.x = dist * cosf(angle);
      suc.y = dist * sinf(angle);
      UNIT_STATE* su = GetUnitFmOff(prjn, net, ru_pos, suc);
      if((su == NULL) || (!self_con && (ru == su))) {
        n_retry++;
        continue;
      }
      if(ru->ConnectFromCk(net, su, prjn)) {
        n_con++;
      }
      else {
        n_retry++;              // already connected, retry
        continue;
      }
    }
#ifdef STATE_CPP
    if(n_con < recv_no) {
      std::cout << "PolarRndPrjnSpec::Connect_impl target number of connections: " << recv_no
                << " not made, only made: " << n_con << std::endl;
    }
#endif    
  }
}

float STATE_CLASS(PolarRndPrjnSpec)::UnitDist
  (PRJN_STATE* prjn, NETWORK_STATE* net, UnitDistType typ, 
   const TAVECTOR2I& ru, const TAVECTOR2I& su)
{
  LAYER_STATE* recv_lay = prjn->GetRecvLayerState(net);
  LAYER_STATE* send_lay = prjn->GetSendLayerState(net);

  TAVECTOR2F half(.5f);
  TAVECTOR2I ru_geom;
  ru_geom.SetXY(recv_lay->flat_geom_x, recv_lay->flat_geom_x);
  TAVECTOR2I su_geom;
  su_geom.SetXY(send_lay->flat_geom_x, send_lay->flat_geom_x);
  switch(typ) {
  case XY_DIST:
    return ru.Dist(su);
  case XY_DIST_CENTER: {
    TAVECTOR2F rctr = ru_geom;   rctr *= half;
    TAVECTOR2F sctr = su_geom;    sctr *= half;
    TAVECTOR2I ruc = ru - (TAVECTOR2I)rctr;
    TAVECTOR2I suc = su - (TAVECTOR2I)sctr;
    return ruc.Dist(suc);
  }
  case XY_DIST_NORM: {
    TAVECTOR2F ruc = ru;    ruc /= (TAVECTOR2F)ru_geom;
    TAVECTOR2F suc = su;    suc /= (TAVECTOR2F)su_geom;
    return ruc.Dist(suc);
  }
  case XY_DIST_CENTER_NORM: {
    TAVECTOR2F rctr = ru_geom;   rctr *= half;
    TAVECTOR2F sctr = su_geom;    sctr *= half;
    TAVECTOR2F ruc = ((TAVECTOR2F)ru - rctr) / rctr;
    TAVECTOR2F suc = ((TAVECTOR2F)su - sctr) / sctr;
    return ruc.Dist(suc);
  }
  }
  return 0.0f;
}

UNIT_STATE* STATE_CLASS(PolarRndPrjnSpec)::GetUnitFmOff
(PRJN_STATE* prjn, NETWORK_STATE* net, const TAVECTOR2I& ru, const TAVECTOR2F& su_off)
{
  LAYER_STATE* recv_lay = prjn->GetRecvLayerState(net);
  LAYER_STATE* send_lay = prjn->GetSendLayerState(net);

  TAVECTOR2F half(.5f);
  TAVECTOR2I ru_geom;
  ru_geom.SetXY(recv_lay->flat_geom_x, recv_lay->flat_geom_x);
  TAVECTOR2I su_geom;
  su_geom.SetXY(send_lay->flat_geom_x, send_lay->flat_geom_x);
  TAVECTOR2I suc;                // actual su coordinates
  switch(dist_type) {
  case XY_DIST: {
    suc = su_off;
    suc += ru;
    break;
  }
  case XY_DIST_CENTER: {        // do everything relative to center
    TAVECTOR2F rctr = ru_geom;   rctr *= half;
    TAVECTOR2F sctr = su_geom;    sctr *= half;
    TAVECTOR2I ruc = ru - (TAVECTOR2I)rctr;
    suc = su_off;
    suc += ruc;                 // add the centerized coordinates
    suc += (TAVECTOR2I)sctr;     // then add the sending center back into it..
    break;
  }
  case XY_DIST_NORM: {
    TAVECTOR2F ruc = ru;    ruc /= (TAVECTOR2F)ru_geom;
    TAVECTOR2F suf = su_off + ruc; // su_off is in normalized coords, so normalize ru
    suf *= (TAVECTOR2F)su_geom;
    suc = suf;
    break;
  }
  case XY_DIST_CENTER_NORM: {
    TAVECTOR2F rctr = ru_geom;   rctr *= half;
    TAVECTOR2F sctr = su_geom;    sctr *= half;
    TAVECTOR2F ruc = ((TAVECTOR2F)ru - rctr) / rctr;
    TAVECTOR2F suf = su_off + ruc;
    suf *= sctr;    suf += sctr;
    suc = suf;
    break;
  }
  }
  if(suc.WrapClip(wrap, su_geom) && !wrap)
    return NULL;

  UNIT_STATE* su_u = (UNIT_STATE*)send_lay->GetUnitStateFlatXY(net, suc.x, suc.y);
  return su_u;
}


float STATE_CLASS(PolarRndPrjnSpec)::GetDistProb
(PRJN_STATE* prjn, NETWORK_STATE* net, UNIT_STATE* ru, UNIT_STATE* su) {
  if(rnd_dist.type == STATE_CLASS(Random)::UNIFORM)
    return p_con;
  
  LAYER_STATE* recv_lay = prjn->GetRecvLayerState(net);
  LAYER_STATE* send_lay = prjn->GetSendLayerState(net);

  TAVECTOR2I ruc;
  ruc.SetXY(ru->pos_x, ru->pos_y);
  TAVECTOR2I suc;
  suc.SetXY(su->pos_x, su->pos_y);

  float dist = UnitDist(prjn, net, dist_type, ruc, suc);
  float prob = p_con * rnd_dist.Density(dist);
  if(wrap) {
    TAVECTOR2I su_geom;
    su_geom.SetXY(send_lay->flat_geom_x, send_lay->flat_geom_x);
    suc.x += su_geom.x; // wrap around in x
    prob += p_con * rnd_dist.Density(UnitDist(prjn, net, dist_type, ruc, suc));
    suc.y += su_geom.y; // wrap around in x & y
    prob += p_con * rnd_dist.Density(UnitDist(prjn, net, dist_type, ruc, suc));
    suc.x = suc.x;          // just y
    prob += p_con * rnd_dist.Density(UnitDist(prjn, net, dist_type, ruc, suc));
    suc = suc;
    suc.x -= su_geom.x; // wrap around in x
    prob -= p_con * rnd_dist.Density(UnitDist(prjn, net, dist_type, ruc, suc));
    suc.y -= su_geom.y; // wrap around in y
    prob += p_con * rnd_dist.Density(UnitDist(prjn, net, dist_type, ruc, suc));
    suc.x = suc.x;          // just y
    prob += p_con * rnd_dist.Density(UnitDist(prjn, net, dist_type, ruc, suc));
  }
  return prob;
}

// todo: could put in some sending limits, and do recvs in random order

void STATE_CLASS(PolarRndPrjnSpec)::Init_Weights_Prjn
(PRJN_STATE* prjn, NETWORK_STATE* net, int thr_no, CON_STATE* cg) {
  UNIT_STATE* ru = cg->OwnUnState(net);
  for(int i=0; i<cg->size; i++) {
    float wt = GetDistProb(prjn, net, ru, cg->UnState(i,net));
    SetCnWtScale(prjn, net, thr_no, cg, i, wt);
  }
}


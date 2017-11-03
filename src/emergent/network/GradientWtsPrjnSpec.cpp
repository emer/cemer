// this is included directly in AllProjectionSpecs_cpp / _cuda
// {

void STATE_CLASS(GradientWtsPrjnSpec)::Initialize_core() {
  wt_range.min = 0.0f;
  wt_range.max = 0.5f;
  wt_range.UpdateRange();
  invert = false;
  grad_x = true;
  grad_y = true;
  wrap = true;
  grad_type = LINEAR;
  use_gps = true;
  gauss_sig = 0.3f;
  init_wts = true;
  add_rnd_var = true;
}

void STATE_CLASS(GradientWtsPrjnSpec)::Init_Weights_Prjn
(PRJN_STATE* prjn, NETWORK_STATE* net, int thr_no, CON_STATE* cg) {

  UNIT_STATE* ru = cg->OwnUnState(net);
  LAYER_STATE* recv_lay = prjn->GetRecvLayerState(net);
  LAYER_STATE* send_lay = prjn->GetSendLayerState(net);

  if(use_gps && recv_lay->HasUnitGroups()) {
    InitWeights_RecvGps(prjn, net, thr_no, cg, ru);
  }
  else {
    InitWeights_RecvFlat(prjn, net, thr_no, cg, ru);
  }
}


void STATE_CLASS(GradientWtsPrjnSpec)::SetWtFmDist
(PRJN_STATE* prjn, NETWORK_STATE* net, int thr_no, CON_STATE* cg, UNIT_STATE* ru,
 float dist, int cg_idx) {

  float wt_val = wt_range.min;
  if(grad_type == LINEAR) {
    if(invert)
      wt_val = wt_range.min + dist * wt_range.Range();
    else
      wt_val = wt_range.max - dist * wt_range.Range();
  }
  else if(grad_type == GAUSSIAN) {
    float gaus = STATE_CLASS(taMath_float)::gauss_den_nonorm(dist, gauss_sig);
    if(invert)
      wt_val = wt_range.max - gaus * wt_range.Range();
    else
      wt_val = wt_range.min + gaus * wt_range.Range();
  }
  SetCnWtScale(prjn, net, thr_no, cg, cg_idx, wt_val);
}

///////////////////////////////////////////////
//      0       1       2       3    recv
//      0       .33     .66     1    rgp_x
//
//      0       1       2       3    send
//      0       .33     .66     1    sgp_x
//      4       5       6       7    wrp_x > .5  int
//      1.33    1.66    2       2.33 wrp_x > .5  flt
//      -4      -3      -2      -1   wrp_x < .5  int
//      -1.33   -1      -.66    -.33 wrp_x < .5  flt

void STATE_CLASS(GradientWtsPrjnSpec)::InitWeights_RecvGps
(PRJN_STATE* prjn, NETWORK_STATE* net, int thr_no, CON_STATE* cg, UNIT_STATE* ru) {

  LAYER_STATE* recv_lay = prjn->GetRecvLayerState(net);
  LAYER_STATE* send_lay = prjn->GetSendLayerState(net);

  int rgpidx = ru->gp_idx;
  TAVECTOR2I rgp_pos;
  recv_lay->GetGpXYFmIdx(rgpidx, rgp_pos.x, rgp_pos.y); // position relative to overall gp geom
  float rgp_x = (float)rgp_pos.x / (float)MAX(recv_lay->gp_geom_x-1, 1);
  float rgp_y = (float)rgp_pos.y / (float)MAX(recv_lay->gp_geom_y-1, 1);

  float max_dist = 1.0f;
  if(grad_x && grad_y)
    max_dist = sqrtf(2.0f);

  float mxs_x = (float)MAX(send_lay->flat_geom_x-1, 1);
  float mxs_y = (float)MAX(send_lay->flat_geom_y-1, 1);

  for(int i=0; i<cg->size; i++) {
    UNIT_STATE* su = cg->UnState(i,net);
    TAVECTOR2I su_pos;
    su_pos.SetXY(su->pos_x, su->pos_y);
    float su_x = (float)su_pos.x / mxs_x;
    float su_y = (float)su_pos.y / mxs_y;

    float wrp_x, wrp_y;
    if(wrap) {
      if(rgp_x > .5f)   wrp_x = (float)(su_pos.x + send_lay->flat_geom_x) / mxs_x;
      else              wrp_x = (float)(su_pos.x - send_lay->flat_geom_x) / mxs_x;
      if(rgp_y > .5f)   wrp_y = (float)(su_pos.y + send_lay->flat_geom_y) / mxs_y;
      else              wrp_y = (float)(su_pos.y - send_lay->flat_geom_y) / mxs_y;
    }

    float dist = 0.0f;
    if(grad_x && grad_y) {
      dist = STATE_CLASS(taMath_float)::euc_dist(su_x, su_y, rgp_x, rgp_y);
      if(wrap) {
        float wrp_dist = STATE_CLASS(taMath_float)::euc_dist(wrp_x, su_y, rgp_x, rgp_y);
        if(wrp_dist < dist) {
          dist = wrp_dist;
          wrp_dist = STATE_CLASS(taMath_float)::euc_dist(wrp_x, wrp_y, rgp_x, rgp_y);
          if(wrp_dist < dist)
            dist = wrp_dist;
        }
        else {
          wrp_dist = STATE_CLASS(taMath_float)::euc_dist(su_x, wrp_y, rgp_x, rgp_y);
          if(wrp_dist < dist)
            dist = wrp_dist;
        }
      }
    }
    else if(grad_x) {
      dist = fabsf(su_x - rgp_x);
      if(wrap) {
        float wrp_dist = fabsf(wrp_x - rgp_x);
        if(wrp_dist < dist) dist = wrp_dist;
      }
    }
    else if(grad_y) {
      dist = fabsf(su_y - rgp_y);
      if(wrap) {
        float wrp_dist = fabsf(wrp_y - rgp_y);
        if(wrp_dist < dist) dist = wrp_dist;
      }
    }

    dist /= max_dist;           // keep it normalized

    SetWtFmDist(prjn, net, thr_no, cg, ru, dist, i);
  }
}

void STATE_CLASS(GradientWtsPrjnSpec)::InitWeights_RecvFlat
  (PRJN_STATE* prjn, NETWORK_STATE* net, int thr_no, CON_STATE* cg, UNIT_STATE* ru) {

  LAYER_STATE* recv_lay = prjn->GetRecvLayerState(net);
  LAYER_STATE* send_lay = prjn->GetSendLayerState(net);
  TAVECTOR2I ru_pos;
  ru_pos.SetXY(ru->pos_x, ru->pos_y);
  float ru_x = (float)ru_pos.x / (float)MAX(recv_lay->flat_geom_x-1, 1);
  float ru_y = (float)ru_pos.y / (float)MAX(recv_lay->flat_geom_y-1, 1);

  float max_dist = 1.0f;
  if(grad_x && grad_y)
    max_dist = sqrtf(2.0f);

  float mxs_x = (float)MAX(send_lay->flat_geom_x-1, 1);
  float mxs_y = (float)MAX(send_lay->flat_geom_y-1, 1);

  for(int i=0; i<cg->size; i++) {
    UNIT_STATE* su = cg->UnState(i,net);
    TAVECTOR2I su_pos;
    su_pos.SetXY(su->pos_x, su->pos_y);
    float su_x = (float)su_pos.x / mxs_x;
    float su_y = (float)su_pos.y / mxs_y;

    float wrp_x, wrp_y;
    if(wrap) {
      if(ru_x > .5f)    wrp_x = (float)(su_pos.x + send_lay->flat_geom_x) / mxs_x;
      else              wrp_x = (float)(su_pos.x - send_lay->flat_geom_x) / mxs_x;
      if(ru_y > .5f)    wrp_y = (float)(su_pos.y + send_lay->flat_geom_y) / mxs_y;
      else              wrp_y = (float)(su_pos.y - send_lay->flat_geom_y) / mxs_y;
    }

    float dist = 0.0f;
    if(grad_x && grad_y) {
      dist = STATE_CLASS(taMath_float)::euc_dist(su_x, su_y, ru_x, ru_y);
      if(wrap) {
        float wrp_dist = STATE_CLASS(taMath_float)::euc_dist(wrp_x, su_y, ru_x, ru_y);
        if(wrp_dist < dist) {
          dist = wrp_dist;
          wrp_dist = STATE_CLASS(taMath_float)::euc_dist(wrp_x, wrp_y, ru_x, ru_y);
          if(wrp_dist < dist)
            dist = wrp_dist;
        }
        else {
          wrp_dist = STATE_CLASS(taMath_float)::euc_dist(su_x, wrp_y, ru_x, ru_y);
          if(wrp_dist < dist)
            dist = wrp_dist;
        }
      }
    }
    else if(grad_x) {
      dist = fabsf(su_x - ru_x);
      if(wrap) {
        float wrp_dist = fabsf(wrp_x - ru_x);
        if(wrp_dist < dist) dist = wrp_dist;
      }
    }
    else if(grad_y) {
      dist = fabsf(su_y - ru_y);
      if(wrap) {
        float wrp_dist = fabsf(wrp_y - ru_y);
        if(wrp_dist < dist) dist = wrp_dist;
      }
    }

    dist /= max_dist;           // keep it normalized

    SetWtFmDist(prjn, net, thr_no, cg, ru, dist, i);
  }
}




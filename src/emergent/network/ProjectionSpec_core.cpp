// contains non-inline (INIMPL) functions 
// if used, must be included directly in ProjectionSpec*.cpp

void STATE_CLASS(ProjectionSpec)::Connect_Sizes(PRJN_STATE* prjn, NETWORK_STATE* net) {
  if(!prjn->IsActive(net)) return;
  Connect_impl(prjn, net, false);
}

void STATE_CLASS(ProjectionSpec)::Connect_Cons(PRJN_STATE* prjn, NETWORK_STATE* net) {
  if(!prjn->IsActive(net)) return;
  Connect_impl(prjn, net, true);
  // Init_Weights(prjn); // connection is NOT init weights -- now definitivitely 2 separate steps -- this is super slow for large nets
}

int STATE_CLASS(ProjectionSpec)::ProbAddCons_impl(PRJN_STATE* prjn, NETWORK_STATE* net, float p_add_con, float init_wt) {
  return 0;
}

int STATE_CLASS(ProjectionSpec)::ProbAddCons(PRJN_STATE* prjn, NETWORK_STATE* net, float p_add_con, float init_wt) {
  int rval = ProbAddCons_impl(prjn, net, p_add_con);
  return rval;
}

void STATE_CLASS(ProjectionSpec)::SetCnWt(PRJN_STATE* prjn, NETWORK_STATE* net, int thr_no, CON_STATE* cg, int cn_idx, float wt_val) {
  CON_SPEC_CPP* cs = cg->GetConSpec(net);
  if(add_rnd_var) {
    int eff_thr_no = net->HasNetFlag(NETWORK_STATE::INIT_WTS_1_THREAD) ? 0 : thr_no;
    cs->C_Init_Weight_AddRndVar(wt_val, eff_thr_no);
  }
  cs->C_ApplyLimits(wt_val);
  cg->Cn(cn_idx,CON_STATE::WT,net) = wt_val;
  cs->SetConScale(1.0f, cg, cn_idx, net, thr_no); // reset scale..
  cs->C_Init_dWt(cg->Cn(cn_idx,CON_STATE::DWT,net));
}

void STATE_CLASS(ProjectionSpec)::SetCnWtRnd(PRJN_STATE* prjn, NETWORK_STATE* net, int thr_no, CON_STATE* cg, int cn_idx) {
  CON_SPEC_CPP* cs = cg->GetConSpec(net);
  float& wt_val = cg->Cn(cn_idx,CON_STATE::WT,net);
  int eff_thr_no = net->HasNetFlag(NETWORK_STATE::INIT_WTS_1_THREAD) ? 0 : thr_no;
  cs->C_Init_Weight_Rnd(wt_val, eff_thr_no); // std rnd wts
  cs->C_Init_dWt(cg->Cn(cn_idx,CON_STATE::DWT,net));
}

void STATE_CLASS(ProjectionSpec)::SetCnScale(PRJN_STATE* prjn, NETWORK_STATE* net, int thr_no, CON_STATE* cg, int cn_idx, float sc_val) {
  CON_SPEC_CPP* cs = cg->GetConSpec(net);
  cs->SetConScale(sc_val, cg, cn_idx, net, thr_no);
}

void STATE_CLASS(ProjectionSpec)::Init_Weights_Prjn(PRJN_STATE* prjn, NETWORK_STATE* net, int thr_no, CON_STATE* cg) {
  if(!init_wts) return;         // shouldn't happen
  CON_SPEC_CPP* cs = prjn->GetConSpec(net);
  if(set_scale) {
    cs->Init_Weights_scale(cg, net, thr_no, init_wt_val);
  }
  else {
    cs->Init_Weights(cg, net, thr_no);
  }
}

void STATE_CLASS(ProjectionSpec)::Init_Weights_renorm(PRJN_STATE* prjn, NETWORK_STATE* net, int thr_no, CON_STATE* cg) {
  if(!renorm_wts.on) return;
  CON_SPEC_CPP* cs = prjn->GetConSpec(net);
  if(set_scale) {
    cs->RenormScales(cg, net, thr_no, renorm_wts.mult_norm, renorm_wts.avg_wt);
  }
  else {
    cs->RenormWeights(cg, net, thr_no, renorm_wts.mult_norm, renorm_wts.avg_wt);
  }
}


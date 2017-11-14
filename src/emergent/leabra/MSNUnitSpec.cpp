// this is included directly in LeabraExtraUnitSpecs_cpp / _cuda
// {

void STATE_CLASS(MSNUnitSpec)::Init_UnitState
  (UNIT_STATE* uv, NETWORK_STATE* net, int thr_no)  {

  inherited::Init_UnitState(uv, net, thr_no);
  LEABRA_UNIT_STATE* u = (LEABRA_UNIT_STATE*)uv;
  if(dar == D1R) {
    u->SetUnitFlag(LEABRA_UNIT_STATE::D1R);
    u->ClearUnitFlag(LEABRA_UNIT_STATE::D2R);
  }
  else {
    u->SetUnitFlag(LEABRA_UNIT_STATE::D2R);
    u->ClearUnitFlag(LEABRA_UNIT_STATE::D1R);
  }
  if(matrix_patch == MATRIX) {
    u->SetUnitFlag(LEABRA_UNIT_STATE::MATRIX);
    u->ClearUnitFlag(LEABRA_UNIT_STATE::PATCH);
  }
  else {
    u->SetUnitFlag(LEABRA_UNIT_STATE::PATCH);
    u->ClearUnitFlag(LEABRA_UNIT_STATE::MATRIX);
  }
  if(dorsal_ventral == DORSAL) {
    u->SetUnitFlag(LEABRA_UNIT_STATE::DORSAL);
    u->ClearUnitFlag(LEABRA_UNIT_STATE::VENTRAL);
  }
  else {
    u->SetUnitFlag(LEABRA_UNIT_STATE::VENTRAL);
    u->ClearUnitFlag(LEABRA_UNIT_STATE::DORSAL);
  }
  if(valence == APPETITIVE) {
    u->SetUnitFlag(LEABRA_UNIT_STATE::APPETITIVE);
    u->ClearUnitFlag(LEABRA_UNIT_STATE::AVERSIVE);
  }
  else {
    u->SetUnitFlag(LEABRA_UNIT_STATE::AVERSIVE);
    u->ClearUnitFlag(LEABRA_UNIT_STATE::APPETITIVE);
  }
}

STATE_CLASS(MSNUnitSpec)::GateType STATE_CLASS(MSNUnitSpec)::MatrixGateType
  (LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) {
  
  LEABRA_LAYER_STATE* lay = u->GetOwnLayer(net);
  int ugidx = u->gp_idx;
  int x_pos = ugidx % lay->gp_geom_x;
  if(matrix.n_mnt_x < 0) {
    int half_x = lay->gp_geom_x / 2;
    if(x_pos < half_x)            // maint on left, out on right..
      return MAINT;
    else
      return OUT;
  }
  else {
    if(x_pos < matrix.n_mnt_x)
      return MAINT;
    else
      return OUT;
  }
}

void STATE_CLASS(MSNUnitSpec)::Compute_DeepMod(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) {
  if(deep.SendDeepMod() || deep.IsTRC()) {
    inherited::Compute_DeepMod(u, net, thr_no);
    return;
  }
  LEABRA_LAYER_STATE* lay = u->GetOwnLayer(net);
  LEABRA_UNGP_STATE* lgpd = lay->GetLayUnGpState(net);
  // must be SUPER units at this point
  if(dorsal_ventral == DORSAL && matrix_patch == MATRIX) {
    u->deep_lrn = u->deep_mod = 1.0f;         // everybody gets 100% -- not using deep
    return;
  }
  else if(dorsal_ventral == VENTRAL && matrix_patch == MATRIX) {
    // if(lay->am_deep_mod_net.max <= deep.mod_thr) { // not enough yet
    if(u->deep_mod_net <= deep.mod_thr) { // per-unit, NOT layer
      u->deep_lrn = 0.0f;    // default is 0!
      u->deep_mod = 1.0f;
    }
    else {
      u->deep_lrn = u->deep_mod_net / lgpd->am_deep_mod_net.max;
      u->deep_mod = 1.0f;
    }
  }
  else if(dorsal_ventral == VENTRAL && matrix_patch == PATCH) {
    // if(lay->am_deep_mod_net.max <= deep.mod_thr) { // not enough yet
    if(u->deep_mod_net <= deep.mod_thr) { // per-unit, NOT layer
      u->deep_lrn = 0.0f;    // default is 0!
      if(deep_mod_zero) {
        u->deep_mod = 0.0f;
      }
      else {
        u->deep_mod = 1.0f;
      }
    }
    else {
      u->deep_lrn = u->deep_mod_net / lgpd->am_deep_mod_net.max;
      u->deep_mod = 1.0f;       // don't modulate activation here..
    }
  }
  else {
    inherited::Compute_DeepMod(u, net, thr_no); // use D1D2 one
  }
}



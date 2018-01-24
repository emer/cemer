// this is included directly in LeabraExtraUnitSpecs_cpp / _cuda
// {

  INLINE void Quarter_Init_TargFlags(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) override {
    if(u->gp_idx < 0) {   // only for unit groups
      inherited::Quarter_Init_TargFlags(u, net, thr_no);
      return;
    }

    LAYER_STATE* lay = u->GetOwnLayer(net);
    UNIT_STATE* src_u = lay->GetUnitStateGpUnIdx(net, 0, u->ungp_un_idx);
    // our corresponding unit in first unit group -- true target data only avail there
    
    if(net->phase == LEABRA_NETWORK_STATE::MINUS_PHASE) {
      if(u->gp_idx > 0)
        u->targ = src_u->targ;    // copy fm src
      u->ext = 0.0f;
      u->ext_orig = u->ext;
      u->ClearExtFlag(UNIT_STATE::EXT);
      u->SetExtFlag(UNIT_STATE::TARG); // may not have been set originally
    }
    else {                         // PLUS PHASE
      u->SetExtFlag(UNIT_STATE::EXT);
      u->SetExtFlag(UNIT_STATE::TARG); // may not have been set originally
      if(u->gp_idx == net->tick) { // we're it
        u->ext = u->targ;
        u->ext_orig = u->ext;
      }
      else {
        u->targ = u->act_m;     // negate stats vs. targ
        u->ext = u->act_m;      // negate any delta -- just clamp to act_m
        u->ext_orig = u->act_m;
      }
    }
  }

  INLINE void Initialize_core() {
  }
  // #IGNORE

  INLINE int  GetStateSpecType() const override
  { return LEABRA_NETWORK_STATE::T_LeabraTickDecodeUnitSpec; }


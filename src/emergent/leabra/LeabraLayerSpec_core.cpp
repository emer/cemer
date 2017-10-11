// contains core non-inline (INIMPL) functions from _core.h
// if used, include directly in LeabraLayerSpec.cpp, _cpp.cpp, _cuda.cpp

void STATE_CLASS(LeabraLayerSpec)::Trial_Init_Layer(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net) {
  LEABRA_UNIT_SPEC_CPP* us = (LEABRA_UNIT_SPEC_CPP*)lay->GetUnitSpec(net);
  LEABRA_UNGP_STATE* lgpd = lay->GetLayUnGpState(net);
  lay->deep_lrate_mod = us->deep.ApplyDeepMod();
  lgpd->acts_q0 = lgpd->acts_p;
  for(int g=0; g < lay->n_ungps; g++) {
    LEABRA_UNGP_STATE* gpd = lay->GetUnGpState(net, g);
    gpd->acts_q0 = gpd->acts_p;
  }
  
  if(decay.trial > 0.0f) {
    lay->laygp_i_val.ffi -= decay.trial * lay->laygp_i_val.ffi;
    lay->laygp_i_val.fbi -= decay.trial * lay->laygp_i_val.fbi;
    lgpd->i_val.ffi -= decay.trial * lgpd->i_val.ffi;
    lgpd->i_val.fbi -= decay.trial * lgpd->i_val.fbi;
    for(int g=0; g < lay->n_ungps; g++) {
      LEABRA_UNGP_STATE* gpd = lay->GetUnGpState(net, g);
      gpd->i_val.ffi -= decay.trial * gpd->i_val.ffi;
      gpd->i_val.fbi -= decay.trial * gpd->i_val.fbi;
    }
  }
  int eff_trial_interval = inhib_adapt.trial_interval;
  if(net->dmem_nprocs > 1) {
    eff_trial_interval /= net->dmem_nprocs;
  }
  if(inhib_adapt.on && ((net->total_trials+1) % eff_trial_interval == 0)) {
    if((lay->layer_type == LAYER_STATE::TARGET) || us->deep.IsTRC()) {
      // use plus-phase running average as the target!
      inhib_adapt.AdaptInhib(lay->adapt_gi, lgpd->acts_p_avg, lgpd->acts_m_avg);
    }
    else {
      inhib_adapt.AdaptInhib(lay->adapt_gi, avg_act.targ_init, lgpd->acts_m_avg);
    }
  }
}

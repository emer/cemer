// contains core non-inline (INIMPL) functions from _core.h
// if used, include directly in LeabraLayer*.cpp, _cpp.cpp, _cuda.cpp

void LEABRA_LAYER::DecayState(float decay) {
  if(!net_state || !net_state->IsBuiltIntact()) return;
  LEABRA_LAYER_SPEC_CPP* ls = GetLayerSpec(net_state);
  LEABRA_LAYER_STATE* st = GetLayerState(net_state);
  if(ls) ls->DecayState(st, (LEABRA_NETWORK_STATE*)net_state, decay);
}

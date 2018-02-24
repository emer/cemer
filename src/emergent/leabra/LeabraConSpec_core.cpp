// contains core non-inline (INIMPL) functions from _core.h
// if used, include directly in LeabraConSpec.cpp, _cpp.cpp, _cuda.cpp

bool LEABRA_CON_SPEC::SaveVar(CON_STATE* cg, NETWORK_STATE* net, int var_no) const {
  if(var_no == WT) return true;
  if(var_no != SCALE) return false;
  if(adapt_scale.on) return true;
  PRJN_STATE* prjn = cg->GetPrjnState(net);
  PRJN_SPEC_CPP* ps = prjn->GetPrjnSpec(net);
  return ps->HasRandomScale();  // if random scale, needs to save it!
}

const char* LEABRA_CON_SPEC::ConVarName(int var_no) const {
  switch(var_no) {
  case WT: return "wt";
  case DWT: return "dwt";
  case SCALE: return "scale";
  case DWNORM: return "dwnorm";
  case MOMENT: return "moment";
  case FWT: return "fwt";
  case SWT: return "swt";
  case WB_INC: return "wb_inc";
  case WB_DEC: return "wb_dec";
  }
  return "";
}

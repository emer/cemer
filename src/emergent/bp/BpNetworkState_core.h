// this contains core shared code, and is included directly in BpNetworkState _cpp.h, _cuda.h
//{
  
  INIMPL virtual void   Compute_dEdA_dEdNet_Thr(int thr_no);
  // #IGNORE compute derivatives of error with respect to activations & net inputs (backpropagate)
  INIMPL virtual void   Compute_Error_Thr(int thr_no);
  // #IGNORE compute local error values, for display purposes only (only call when testing, not training)

  INIMPL void   Compute_NetinAct_Thr(int thr_no) override;
  INIMPL void   Compute_dWt_Thr(int thr_no) override;
  INIMPL void   Compute_Weights_Thr(int thr_no) override;

  INIMPL virtual void  Trial_Run();
  // #CAT_Bp run one trial of Bp: calls SetCurLrate, Compute_NetinAct, Compute_dEdA_dEdNet, and, if train_mode == TRAIN, Compute_dWt.  If you want to save some speed just for testing, you can just call Compute_NetinAct and skip the other two (esp Compute_dEdA_dEdNet, which does a full backprop and is expensive, but often useful for visualization & testing)

  INIMPL virtual void  Trial_Run_Thr(int thr_no); // #IGNORE

  INIMPL UNIT_SPEC_CPP*     NewUnitSpec(int spec_type) const override;
  INIMPL CON_SPEC_CPP*      NewConSpec(int spec_type) const override;



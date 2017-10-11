// this contains core shared code, and is included directly in BpNetworkState _cpp.h, _cuda.h
//{
  INIMPL virtual void	Compute_dEdA_dEdNet_Thr(int thr_no);
  // #IGNORE compute derivatives of error with respect to activations & net inputs (backpropagate)
  INIMPL virtual void	Compute_Error_Thr(int thr_no);
  // #IGNORE compute local error values, for display purposes only (only call when testing, not training)

  INIMPL void   Compute_NetinAct_Thr(int thr_no) override;
  INIMPL void   Compute_dWt_Thr(int thr_no) override;
  INIMPL void   Compute_Weights_Thr(int thr_no) override;

  INIMPL virtual void  Trial_Run_Thr(int thr_no); // #IGNORE



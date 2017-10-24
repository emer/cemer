// this is included directly in AllProjectionSpecs_cpp / _cuda
// {
  INIMPL void  Connect_impl(PRJN_STATE* prjn, NETWORK_STATE* net, bool make_cons) override;
  INIMPL int   ProbAddCons_impl(PRJN_STATE* prjn, NETWORK_STATE* net, float p_add_con, float init_wt = 0.0f) override;

  INLINE int  GetStateSpecType() const override { return NETWORK_STATE::T_FullPrjnSpec; }
  // #CAT_State derived classes MUST override this and pass correct global type id

  INLINE void  Initialize_core() { };

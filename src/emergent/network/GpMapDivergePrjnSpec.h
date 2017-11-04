// this is included directly in AllProjectionSpecs_cpp / _cuda
// {

  INIMPL void  Connect_impl(PRJN_STATE* prjn, NETWORK_STATE* net, bool make_cons) override;

  INLINE void Initialize_core() { };

  INLINE int  GetStateSpecType() const override
  { return NETWORK_STATE::T_GpMapDivergePrjnSpec; }

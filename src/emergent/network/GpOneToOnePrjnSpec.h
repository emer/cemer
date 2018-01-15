// this is included directly in AllProjectionSpecs_cpp / _cuda
// {

  float         p_con;          // proportion connectivity within each group -- uses permuted uniform random -- negative value means make symmetric cons -- also optimized for full connectivity when = 1
  bool          sym_self;       // #CONDSHOW_OFF_p_con:1 if a self projection and p_con < 1 (full), make it symmetric (senders = receivers) otherwise it is not
  bool          same_seed;      // #CONDSHOW_OFF_p_con:1 use the same random seed each time (same connect pattern) -- only if p_con < 1
  STATE_CLASS(RndSeed)  rndm_seed;  // #HIDDEN random seed -- new one auto-generated upon first use

  INIMPL void  Connect_impl(PRJN_STATE* prjn, NETWORK_STATE* net, int make_cons) override;

  INIMPL virtual void Connect_RecvUnitsSendGps(PRJN_STATE* prjn, NETWORK_STATE* net, int make_cons);
  // #IGNORE
  INIMPL virtual void Connect_SendUnitsRecvGps(PRJN_STATE* prjn, NETWORK_STATE* net, int make_cons);
  // #IGNORE 

  INIMPL void   Initialize_core();

  INLINE int  GetStateSpecType() const override
  { return NETWORK_STATE::T_GpOneToOnePrjnSpec; }

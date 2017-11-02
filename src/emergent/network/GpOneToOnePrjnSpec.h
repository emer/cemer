// this is included directly in AllProjectionSpecs_cpp / _cuda
// {
  float		p_con;		// proportion connectivity within each group -- negative value means make symmetric cons within the same layer -- also optimized for full connectivity when = 1
  bool		sym_self;	// #CONDSHOW_OFF_p_con:1 if a self projection and p_con < 1 (full), make it symmetric (senders = receivers) otherwise it is not


  INIMPL void  Connect_impl(PRJN_STATE* prjn, NETWORK_STATE* net, bool make_cons) override;

  INIMPL virtual void Connect_RecvUnitsSendGps(PRJN_STATE* prjn, NETWORK_STATE* net, bool make_cons);
  // #IGNORE
  INIMPL virtual void Connect_SendUnitsRecvGps(PRJN_STATE* prjn, NETWORK_STATE* net, bool make_cons);
  // #IGNORE 

  INLINE void	Initialize_core()	{ p_con = 1.0f; sym_self = true; }

  INLINE int  GetStateSpecType() const override { return NETWORK_STATE::T_GpOneToOnePrjnSpec; }

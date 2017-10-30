// this is included directly in AllProjectionSpecs_cpp / _cuda
// {
  float		p_con;		// overall probability of connection
  bool		sym_self;	// if a self projection, make it symmetric (senders = receivers) otherwise it is not
  STATE_CLASS(RndSeed)	rndm_seed;	// random seed -- call NewSeed() to get a new random connectivity pattern

  INIMPL void Connect_impl(PRJN_STATE* prjn, NETWORK_STATE* net, bool make_cons) override;

  INLINE void Initialize_core() {   p_con = .25;  sym_self = false; }

  INLINE int  GetStateSpecType() const override { return NETWORK_STATE::T_RandomPrjnSpec; }

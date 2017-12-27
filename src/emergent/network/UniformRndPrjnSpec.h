// this is included directly in AllProjectionSpecs_cpp / _cuda
// {

  float		p_con;		// #MIN_0 #MAX_1 overall probability of connection -- if p_con = 1 then direct full projection is made, equivalent to FullPrjnSpec
  bool		sym_self;	// if a self projection, make it symmetric (senders = receivers) otherwise it is not
  bool		same_seed;	// use the same random seed each time (same connect pattern)
  STATE_CLASS(RndSeed)	rndm_seed;	// #HIDDEN random seed -- new one auto-generated at first use
  bool          crazy_dont_use;         // temporary flag to enable crazy mode under dmem / mpi: each processor uses their own random sequence, creating different patterns of connectivity, which are then aggregated together.  somehow this works spectactularly well for the WWI_emer project -- making this available until it is replicated through saner means

  INIMPL void Connect_impl(PRJN_STATE* prjn, NETWORK_STATE* net, int make_cons) override;

  INIMPL virtual void FullConnect_impl(PRJN_STATE* prjn, NETWORK_STATE* net, int make_cons);
  // #IGNORE full connection code when p_con = 1

  INLINE void Initialize_core() {   p_con = .25;  sym_self = false; same_seed = false; crazy_dont_use = false; }

  INLINE int  GetStateSpecType() const override
  { return NETWORK_STATE::T_UniformRndPrjnSpec; }


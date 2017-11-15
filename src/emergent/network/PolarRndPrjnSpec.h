// this is included directly in AllProjectionSpecs_cpp / _cuda
// {

  enum UnitDistType {	// how to compute the distance between two units
    XY_DIST,		// X-Y axis distance between units
    XY_DIST_CENTER, 	// centered distance (layers centered over each other)
    XY_DIST_NORM,	// normalized range (0-1 for each layer) distance
    XY_DIST_CENTER_NORM  // normalized and centered (-1:1 for each layer) distance
  };

  float		p_con;		// overall probability of connection (number of samples)
  STATE_CLASS(Random) 	rnd_dist;	// prob density of connectivity as a fctn of distance
  STATE_CLASS(Random)	rnd_angle;	// prob density of connectivity as a fctn of angle (1 = 2pi)
  UnitDistType	dist_type; 	// type of distance function to use
  bool		wrap;		// wrap around layer coordinates (else clip at ends)
  int	       	max_retries;	// maximum number of times attempt to con same sender allowed
  bool		same_seed;	// use the same random seed each time (same connect pattern)
  STATE_CLASS(RndSeed)	rndm_seed;	// #HIDDEN random seed

  INIMPL void Connect_impl(PRJN_STATE* prjn, NETWORK_STATE* net, int make_cons) override;

  INIMPL void Init_Weights_Prjn(PRJN_STATE* prjn, NETWORK_STATE* net, int thr_no,
                                CON_STATE* cg) override;

  INLINE bool  HasRandomScale() override { return false; }

  INIMPL virtual float	UnitDist(PRJN_STATE* prjn, NETWORK_STATE* net, UnitDistType typ,
                                 const TAVECTOR2I& ru, const TAVECTOR2I& su);
  // computes the distance between two units according to distance type
  INIMPL virtual UNIT_STATE*	GetUnitFmOff
  (PRJN_STATE* prjn, NETWORK_STATE* net, const TAVECTOR2I& ru, const TAVECTOR2F& su_off);
  // gets unit from real-valued offset scaled according to distance type

  INIMPL virtual float	GetDistProb(PRJN_STATE* prjn, NETWORK_STATE* net,
                                    UNIT_STATE* ru, UNIT_STATE* su);
  // compute the probability for connecting two units as a fctn of distance

  INIMPL void	Initialize_core();

  INLINE int  GetStateSpecType() const override
  { return NETWORK_STATE::T_PolarRndPrjnSpec; }


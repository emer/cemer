// this is included directly in AllProjectionSpecs_cpp / _cuda
// {

  bool		reciprocal;	// if true, make the appropriate reciprocal connections for a backwards projection from recv to send

  TAVECTOR2I ru_geo;		// #READ_ONLY receiving unit geometry
  TAVECTOR2I su_act_geom;	// #READ_ONLY sending actual geometry
  TAVECTOR2F rf_width;	        // #READ_ONLY how much to move sending rf per recv group

  INIMPL virtual bool	InitRFSizes(PRJN_STATE* prjn, NETWORK_STATE* net);
  // #IGNORE initialize sending receptive field sizes

  INIMPL void Connect_impl(PRJN_STATE* prjn, NETWORK_STATE* net, bool make_cons) override;

  INIMPL virtual void  Connect_Reciprocal(PRJN_STATE* prjn, NETWORK_STATE* net, bool make_cons);
  // #IGNORE 

  //   int 	ProbAddCons_impl(PRJN_STATE* prjn, NETWORK_STATE* net, float p_add_con, float init_wt = 0.0f);

  INIMPL void	Initialize_core();
  // #IGNORE

  INLINE int  GetStateSpecType() const override
  { return NETWORK_STATE::T_TiledNovlpPrjnSpec; }


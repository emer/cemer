// this is included directly in AllProjectionSpecs_cpp / _cuda
// {

  TAVECTOR2I	recv_gp_border;		// number of groups around edge of layer to not connect 
  TAVECTOR2I	recv_gp_ex_st; 		// start of groups to exclude (e.g., from the middle; -1 = no exclude)
  TAVECTOR2I	recv_gp_ex_n; 		// number of groups to exclude
  TAVECTOR2I	send_border;		// number of units around edge of sending layer to not connect
  TAVECTOR2I	send_adj_rfsz;		// adjust the total number of sending units by this amount in computing rfield size
  TAVECTOR2I	send_adj_sndloc;	// adjust the total number of sending units by this amount in computing sending locations
  float		rf_width_mult;		// multiplier factor on the receptive field width: 1.0 = exactly half overlap of RF's across groups.  Larger number = more overlap

  // computed values below
  TAVECTOR2I ru_geo;		// #READ_ONLY receiving unit geometry
  TAVECTOR2I recv_gp_ed;		// #READ_ONLY recv gp end
  TAVECTOR2I recv_gp_ex_ed;	// #READ_ONLY recv gp ex end
  TAVECTOR2I su_act_geom;	// #READ_ONLY sending actual geometry
  TAVECTOR2I n_recv_gps;		// #READ_ONLY number of recv gps
  TAVECTOR2I n_send_units;	// #READ_ONLY number of sending units total 
  TAVECTOR2I rf_ovlp; 		// #READ_ONLY ovlp = send / (ng + 1)
  TAVECTOR2F rf_move;	// #READ_ONLY how much to move sending rf per recv group
  TAVECTOR2I rf_width;		// #READ_ONLY width of the sending rf 

  INIMPL void Connect_impl(PRJN_STATE* prjn, NETWORK_STATE* net, bool make_cons) override;

  INIMPL virtual bool InitRFSizes(PRJN_STATE* prjn, NETWORK_STATE* net);
  // #IGNORE initialize sending receptive field sizes

  INIMPL void	Initialize_core();
  // #IGNORE

  INLINE int  GetStateSpecType() const override { return NETWORK_STATE::T_TiledRFPrjnSpec; }

  // int 	ProbAddCons_impl(PRJN_STATE* prjn, NETWORK_STATE* net, float p_add_con, float init_wt = 0.0f) override;

// this is directly included in NetworkState and Network files
// {
  enum NetFlags {               // #BITS flags for network
    NF_NONE             = 0,    // #NO_BIT
    NETIN_PER_PRJN      = 0x0001, // compute netinput per projection instead of a single aggregate value across all inputs (which is the default)
    BUILD_INIT_WTS      = 0x0002, // initialize the weights after building the network -- for very large networks, may want to turn this off to save some redundant time
    INIT_WTS_1_THREAD   = 0x0004, // use only one (main) thread to initialize weights -- this ensures that runs with different numbers of threads have the same initial weights, but is slower
    SAVE_KILLED_WTS     = 0x0008, // if the project is killed while running in a non-interactive mode (e.g., on cluster), save this network's weights (only if network is built and epoch > 0)
    BUILT               = 0x1000, // #READ_ONLY #NO_SAVE is the network built -- all memory allocated, etc
    INTACT              = 0x2000, // #READ_ONLY #NO_SAVE if the network is built, is it also still intact, with all the current params set as they were when it was built?
    BUILT_INTACT        = BUILT | INTACT // #NO_BIT built and intact
  };

  enum WtUpdate {
    ON_LINE,                    // update weights on-line (after every event) -- this is not viable for dmem processing across trials and is automatically switched to small_batch in that case
    SMALL_BATCH,                // update weights every small_batch_n trials
    BATCH                       // update weights in batch mode (after every epoch)
  };

  enum TrainMode {
    TEST,                       // network is only being tested; no learning should occur
    TRAIN                       // network is being trained: learning should occur
  };

  enum StateLayerSpecTypes { // manual type registry system for all spec types used in state code -- any new spec type MUST be added to this list, extending from N case in the last list, for any derived classes, and each spec must return its appropriate enum in GetStateSpecType() method
    T_LayerSpec,            // base LayerSpec type
    N_NetworkLayerSpecs,     // derived classes start from this one -- use class name for subclasses
  };

  enum StatePrjnSpecTypes { // manual type registry system for all spec types used in state code -- any new spec type MUST be added to this list, extending from N case in the last list, for any derived classes, and each spec must return its appropriate enum in GetStateSpecType() method
    T_ProjectionSpec,            // base PrjnSpec type
    T_FullPrjnSpec,

    T_OneToOnePrjnSpec,
    T_GpOneToOnePrjnSpec,
    T_MarkerGpOneToOnePrjnSpec,
    T_GpMapConvergePrjnSpec,
    T_GpMapDivergePrjnSpec,

    T_RandomPrjnSpec,
    T_UniformRndPrjnSpec,
    T_PolarRndPrjnSpec,

    T_TesselPrjnSpec,
    T_GpTesselPrjnSpec,
    T_SymmetricPrjnSpec,

    T_TiledGpRFPrjnSpec,
    T_TiledGpRFOneToOnePrjnSpec,
    T_TiledGpRFOneToOneWtsPrjnSpec,
    T_TiledSubGpRFPrjnSpec,

    T_TiledRFPrjnSpec,
    T_TiledNovlpPrjnSpec,
    T_TiledGpMapConvergePrjnSpec,
    T_TiledDivGpRFPrjnSpec,

    T_GaussRFPrjnSpec,
    T_GradientWtsPrjnSpec,

    T_PFCPrjnSpec,
    T_BgPfcPrjnSpec,

    T_ConPoolPrjnSpec,          // not converted
    T_SmallWorldPrjnSpec,       // not converted
    T_ScalarValSelfPrjnSpec,    // not converted
    T_SaliencyPrjnSpec,         // not converted
    T_CerebConj2PrjnSpec,       // not converted
    T_TopoWtsPrjnSpec,          // not converted
    
    N_NetworkPrjnSpecs,     // derived classes start from this one -- use class name for subclasses
  };

  enum StateUnitSpecTypes { // manual type registry system for all spec types used in state code -- any new spec type MUST be added to this list, extending from N case in the last list, for any derived classes, and each spec must return its appropriate enum in GetStateSpecType() method
    T_UnitSpec,            // base UnitSpec type
    N_NetworkUnitSpecs,     // derived classes start from this one -- use class name for subclasses
  };

  enum StateConSpecTypes { // manual type registry system for all spec types used in state code -- any new spec type MUST be added to this list, extending from N case in the last list, for any derived classes, and each spec must return its appropriate enum in GetStateSpecType() method
    T_ConSpec,            // base ConSpec type
    N_NetworkConSpecs,     // derived classes start from this one -- use class name for subclasses
  };

  bool          main_obj;       // #NO_SAVE #READ_ONLY #CAT_State true if this is a main-side object (emergent, TA-enabled) as opposed to a State-side object 
  int           n_thrs_built;   // #NO_SAVE #READ_ONLY #CAT_State number of threads that the network was built for -- must use this number of threads for running network, and rebuild if the number changes
  int           layer_state_size;  // #NO_SAVE #READ_ONLY #CAT_State size in *bytes* of the layer_state LayerState
  int           prjn_state_size;  // #NO_SAVE #READ_ONLY #CAT_State size in *bytes* of the prjn_state LayerState
  int           ungp_state_size;  // #NO_SAVE #READ_ONLY #CAT_State size in *bytes* of the ungp_state LayerState
  int           unit_state_size;  // #NO_SAVE #READ_ONLY #CAT_State size in *bytes* of the UnitState
  int           con_state_size;  // #NO_SAVE #READ_ONLY #CAT_State size in *bytes* of con group objects actually built 
  int           n_layers_built; // #NO_SAVE #READ_ONLY #CAT_State number of state layers when built -- size of state_layers array
  int           n_prjns_built;  // #NO_SAVE #READ_ONLY #CAT_State number of state projections when builtsize of state_layers array
  int           n_ungps_built;  // #NO_SAVE #READ_ONLY #CAT_State number of state unit groups when built -- size of state_ungps array
  int           n_units_built;  // #NO_SAVE #READ_ONLY #CAT_State number of units built -- actually the n+1 size of units_flat
  int           n_layer_specs_built;  // #NO_SAVE #READ_ONLY #CAT_Specs number of specs 
  int           n_prjn_specs_built;  // #NO_SAVE #READ_ONLY #CAT_Specs number of specs 
  int           n_unit_specs_built;  // #NO_SAVE #READ_ONLY #CAT_Specs number of specs 
  int           n_con_specs_built;  // #NO_SAVE #READ_ONLY #CAT_Specs number of specs 


  NetFlags      flags;          // #MAIN #CONDEDIT_ON_main_obj #CAT_Structure flags controlling various aspects of network function
  TrainMode     train_mode;     // #MAIN #CONDEDIT_ON_main_obj #CAT_Learning training mode -- determines whether weights are updated or not (and other algorithm-dependent differences as well).  TEST turns off learning
  WtUpdate      wt_update;      // #MAIN #CONDEDIT_ON_main_obj #CAT_Learning #CONDSHOW_ON_train_mode:TRAIN weight update mode: when are weights updated (only applicable if train_mode = TRAIN)
  int           small_batch_n;  // #MAIN #CONDEDIT_ON_main_obj #CONDSHOW_ON_wt_update:SMALL_BATCH #CAT_Learning number of events for small_batch learning mode (specifies how often weight changes are synchronized in dmem)
  int           small_batch_n_eff; // #MAIN #CONDEDIT_ON_main_obj #GUI_READ_ONLY #EXPERT #NO_SAVE #CAT_Learning effective batch_n value = batch_n except for dmem when it = (batch_n / epc_nprocs) >= 1
  STATE_CLASS(NetStatsSpecs) stats; // #CAT_Statistic parameters controling the computation of statistics

  int           batch;          // #MAIN #CONDEDIT_ON_main_obj #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Counter #VIEW batch counter: number of times network has been trained over a full sequence of epochs (updated by program)
  int           epoch;          // #MAIN #CONDEDIT_ON_main_obj #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Counter #VIEW epoch counter: number of times a complete set of training patterns has been presented (updated by program)
  int           group;          // #MAIN #CONDEDIT_ON_main_obj #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Counter #VIEW group counter: optional extra counter to record sequence-level information (sequence = group of trials)
  int           trial;          // #MAIN #CONDEDIT_ON_main_obj #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Counter #VIEW trial counter: number of external input patterns that have been presented in the current epoch (updated by program)
  int           tick;           // #MAIN #CONDEDIT_ON_main_obj #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Counter #VIEW tick ..counter: optional extra counter to record a level of organization below the trial level (for cases where trials have multiple component elements)
  int           cycle;          // #MAIN #CONDEDIT_ON_main_obj #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Counter #VIEW cycle counter: number of iterations of activation updating (settling) on the current external input pattern (updated by program)
  float         time;           // #MAIN #CONDEDIT_ON_main_obj #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Counter #VIEW the current time, relative to some established starting point, in algorithm-specific units (often miliseconds)
  int           total_trials;   // #MAIN #CONDEDIT_ON_main_obj #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Counter #VIEW total number of trials counter: number of external input patterns that have been presented since the weights were initialized

  float         sse;            // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic #VIEW sum squared error over the network, for the current external input pattern
  float         sum_sse;        // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic total sum squared error over an epoch or similar larger set of external input patterns
  STATE_CLASS(Average)	avg_sse;	// #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic #DMEM_AGG_SUM average sum squared error over an epoch or similar larger set of external input patterns
  float         cnt_err;        // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic count of number of times the sum squared error was above cnt_err_tol over an epoch or similar larger set of external input patterns
  float         cur_cnt_err;    // #NO_SAVE #READ_ONLY #DMEM_AGG_SUM #CAT_Statistic current cnt_err -- used for computing cnt_err
  float         pct_err;        // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic epoch-wise average of count of number of times the sum squared error was above cnt_err_tol over an epoch or similar larger set of external input patterns (= cnt_err / n)
  float         pct_cor;        // #NO_SAVE #GUI_READ_ONLY #SHOW #CAT_Statistic epoch-wise average of count of number of times the sum squared error was below cnt_err_tol over an epoch or similar larger set of external input patterns (= 1 - pct_err -- just for convenience for whichever you want to plot)

  STATE_CLASS(PRerrVals)     prerr;          // #NO_SAVE #GUI_READ_ONLY #CONDSHOW_ON_stats.prerr #CAT_Statistic precision and recall error values for the entire network, for the current external input pattern
  STATE_CLASS(PRerrVals)     sum_prerr;      // #NO_SAVE #READ_ONLY #DMEM_AGG_SUM #CAT_Statistic precision and recall error values for the entire network, over an epoch or similar larger set of external input patterns -- these are always up-to-date as the system is aggregating, given the additive nature of the statistics
  STATE_CLASS(PRerrVals)     epc_prerr;      // #NO_SAVE #GUI_READ_ONLY #CONDSHOW_ON_stats.prerr #CAT_Statistic precision and recall error values for the entire network, over an epoch or similar larger set of external input patterns

  bool          needs_wt_sym;   // #HIDDEN #NO_SAVE tmp flag managed by Init_Weights to determine if any connections have the wt_limits.sym flag checked and thus need weight symmetrizing to happen

  int           n_units;        // #READ_ONLY #SHOW #CAT_Structure total number of units in the network
  int64_t       n_cons;         // #READ_ONLY #SHOW #CAT_Structure total number of connections in the network
  int           max_prjns;      // #READ_ONLY #EXPERT #CAT_Structure maximum number of prjns per any given layer or unit in the network

  INLINE void    SetNetFlag(NetFlags flg)   { flags = (NetFlags)(flags | flg); }
  // set flag state on
  INLINE void    ClearNetFlag(NetFlags flg) { flags = (NetFlags)(flags & ~flg); }
  // clear flag state (set off)
  INLINE bool    HasNetFlag(NetFlags flg) const { return (flags & flg); }
  // check if flag is set
  INLINE void    SetNetFlagState(NetFlags flg, bool on)
  { if(on) SetNetFlag(flg); else ClearNetFlag(flg); }
  // set flag state according to on bool (if true, set flag, if false, clear it)

  INLINE void    ClearIntact()  { ClearNetFlag(INTACT); }
  // call this when any element in the network is updated such that the current built status is no longer valid 
  INLINE bool    IsIntact() const { return HasNetFlag(INTACT); }
  // is this network currently intact?
  INLINE bool    IsBuiltIntact() const { return HasNetFlag(BUILT) && HasNetFlag(INTACT); }
  // is this network currently built and intact?
  INLINE bool    NetinPerPrjn() const { return HasNetFlag(NETIN_PER_PRJN); }
  // #CAT_Activation is this network configured to compute net input on a per-prjn basis?

  INLINE virtual bool  RecvOwnsCons() const { return true; }
  // #CAT_Structure does the receiver own the connections (default) or does the sender?

  // NOTE: specific classes must define these methods!

  // INLINE bool  NetStateMalloc(void** ptr, size_t sz) const { return false; }
  // // #IGNORE malloc function to allocate memory for network state into pointer -- each platform needs this defined
  // INLINE bool  NetStateFree(void** ptr) const { *ptr = NULL; return false; }
  // // #IGNORE free previously-malloc'd memory for network state in pointer and set *ptr = NULL -- each platform needs this defined

  INLINE const char* GetStateSuffix() const { return STATE_SUFFIX_STR; }
  // #CAT_State get the suffix string for this state type ("_cpp", "_cuda", or blank for main)
  
  INLINE void   SetStateSizes(int nthr, int lay_sz, int prjn_sz, int ungp_sz, int unit_sz,
                              int con_sz, int n_lays, int n_prjns, int n_ungps, int n_un) {
    n_thrs_built = nthr; layer_state_size = lay_sz; prjn_state_size = prjn_sz;
    ungp_state_size = ungp_sz; unit_state_size = unit_sz; con_state_size = con_sz;
    n_layers_built = n_lays; n_prjns_built = n_prjns; n_ungps_built = n_ungps; n_units_built = n_un;
  }
  // #IGNORE set state sizes

  INLINE void   SetSpecSizes(int n_lspec, int n_pspec, int n_uspec, int n_cspec) {
    n_layer_specs_built = n_lspec; n_prjn_specs_built = n_pspec; n_unit_specs_built = n_uspec;
    n_con_specs_built = n_cspec;
  }
  // #IGNORE set spec sizes


  INLINE virtual void Init_Counters_impl() {
    // this is one you do not reinit: loops over inits: batch = 0;
    epoch = 0;    group = 0;    trial = 0;    tick = 0;    cycle = 0;    time = 0.0f;
    total_trials = 0;
  }
  // #IGNORE init all counters -- todo: override and call inherited in subclasses

  INLINE void   Initialize_net_core() {
    main_obj = false;
    n_thrs_built = 0;     layer_state_size = 0;   prjn_state_size = 0;    ungp_state_size = 0;
    unit_state_size = 0;  con_state_size = 0;     n_layers_built = 0;     n_prjns_built = 0;
    n_ungps_built = 0;    n_units_built = 0;      n_layer_specs_built = 0;
    n_prjn_specs_built = 0; n_unit_specs_built = 0; n_con_specs_built = 0;
    
    train_mode = TRAIN;    wt_update = ON_LINE;    small_batch_n = 10;    small_batch_n_eff = 10;

    batch = 0;    epoch = 0;    group = 0;    trial = 0;    tick = 0;    cycle = 0;
    time = 0.0f;    total_trials = 0;
    
    sse = 0.0f;    sum_sse = 0.0f;    cnt_err = 0.0f;    pct_err = 0.0f;    pct_cor = 0.0f;
    cur_cnt_err = 0.0f;    needs_wt_sym = false;

    n_units = 0;    n_cons = 0;    max_prjns = 1;
  }
  // #IGNORE

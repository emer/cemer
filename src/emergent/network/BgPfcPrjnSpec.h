// this is included directly in AllProjectionSpecs_cpp / _cuda
// {

  static const int LAY_NAME_MAX_LEN=256; // #IGNORE max length of layer name

  int           n_pfcs;        // number of different PFC layers that map into a common BG layer -- this is the number of rows that will be enforced in the bg_table data table
  bool          cross_connect;       // if true, then this creates connections based on the PFC layer name specified in the connect_as field, instead of the actual name of the PFC layer (still needs to find the PFC layer name in bg_table to know which layer is the PFC layer and which is the BG layer)
  char          connect_as_chars[LAY_NAME_MAX_LEN];  // #HIDDEN chars rep of PFC layer name to connect as -- see cross_connect option for details
  int           n_pfc_layers;      // #READ_ONLY #NO_COPY #NO_SAVE number of pfc layers to use
  int           alloc_pfc_layers;  // #READ_ONLY #NO_COPY #NO_SAVE number of pfc layers allocated
  STATE_CLASS(BgPfcPrjnEl)*   pfc_layers_m; // #IGNORE n_pfc_layers pfc layers

  INIMPL void  AllocPfcLayers(int ns);
  // #IGNORE allocate the pfc_layers_m array -- done by parent emergent obj
  INIMPL void  FreePfcLayers();
  // #IGNORE free any allocated pfc_layers_m

  INLINE void   SetConnectAs(const char* con_as) {
    strncpy(connect_as_chars, con_as, LAY_NAME_MAX_LEN-1);
    connect_as_chars[LAY_NAME_MAX_LEN-1] = '\0'; // C is so lame!
  }
  INLINE bool   ConnectAsIs(const char* con_as) const {
    return (strncmp(connect_as_chars, con_as, LAY_NAME_MAX_LEN) == 0);
  }
  INLINE bool   ConnectAsContains(const char* con_as) const {
    return (strstr(connect_as_chars, con_as) != NULL);
  }


  INIMPL void  Connect_impl(PRJN_STATE* prjn, NETWORK_STATE* net, int make_cons) override;

  INLINE void Initialize_core() {
    n_pfcs = 2;    cross_connect = false;    n_pfc_layers = 0;
    alloc_pfc_layers = 0;    pfc_layers_m = NULL;
  }
  // #IGNORE

  INLINE int  GetStateSpecType() const override
  { return NETWORK_STATE::T_BgPfcPrjnSpec; }

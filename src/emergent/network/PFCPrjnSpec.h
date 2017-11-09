// this is included directly in AllProjectionSpecs_cpp / _cuda
// {

  enum LayerType {
    PFC,        // a PFC layer
    GATING,     // a gating-type of layer, e.g., Matrix, GPi, Thal
    OTHER,      // some other type of layer without specific stripe-level structure
  };
  
  enum PFCRows {        // #BITS rows of PFC
    IN_TRANS = 0x001,    // Input, Transient
    IN_MAINT = 0x002,    // Input, Maintaining
    OUT_TRANS = 0x004,   // Output, Transient
    OUT_MAINT = 0x008,   // Output, Maintaining
    ALL_PFC = IN_TRANS | IN_MAINT | OUT_TRANS | OUT_MAINT, // #NO_BIT
  };
  
  enum GatingRows {      // #BITS rows of Gating layers
    INPUT = 0x001,      // Input
    OUTPUT = 0x002,     // Output
    ALL_GATING = INPUT | OUTPUT, // #NO_BIT
  };

  LayerType     recv_layer;     // what type of layer is the receiving layer
  PFCRows       recv_pfc_rows;  // #CONDSHOW_ON_recv_layer:PFC which PFC rows should receive connections
  GatingRows    recv_gate_rows; // #CONDSHOW_ON_recv_layer:GATING which GATING rows should receive connections
  
  LayerType     send_layer;     // what type of layer is the receiving layer
  PFCRows       send_pfc_rows;  // #CONDSHOW_ON_send_layer:PFC which PFC rows should send connections
  GatingRows    send_gate_rows; // #CONDSHOW_ON_send_layer:GATING which GATING rows should send connections
  bool          row_1to1;       // make the row connections one-to-one for selected sender and receiver rows -- for PFC to GATING connections, this means INPUT to INPUT and OUTPUT to OUTPUT -- not relevant for connections with OTHER layer types -- otherwise it is all-to-all
  bool          col_1to1;       // make the column-wise connections one-to-one -- otherwise it is all-to-all
  bool          unit_1to1;      // for non-learning PFC representations that just have 1-to-1 copies of input representations, this establishes the 1-to-1 unit-level connectivity
  
  INIMPL void  Connect_impl(PRJN_STATE* prjn, NETWORK_STATE* net, bool make_cons) override;

  INIMPL virtual void Connect_row1to1(PRJN_STATE* prjn, NETWORK_STATE* net, bool make_cons,
                               int rx, int ry, int recv_x, bool recv_gps);
  INIMPL virtual void Connect_rowall(PRJN_STATE* prjn, NETWORK_STATE* net, bool make_cons,
                              int rx, int ry, int recv_x, bool recv_gps);
  INIMPL virtual void Connect_cols(PRJN_STATE* prjn, NETWORK_STATE* net, bool make_cons,
                            int rx, int ry, int recv_x, bool recv_gps,
                            int sy, int send_x, bool send_gps);
  INIMPL virtual void Connect_unit1to1(PRJN_STATE* prjn, NETWORK_STATE* net, bool make_cons,
                                int rx, int ry, int recv_x, bool recv_gps,
                                int sy, int send_x, bool send_gps);
  
  INLINE void Initialize_core() {
    recv_layer = GATING;    recv_pfc_rows = ALL_PFC;    recv_gate_rows = ALL_GATING;
    send_layer = PFC;    send_pfc_rows = ALL_PFC;    send_gate_rows = ALL_GATING;
    row_1to1 = true;    col_1to1 = true;    unit_1to1 = false;
  }
  // #IGNORE

  INLINE int  GetStateSpecType() const override
  { return NETWORK_STATE::T_PFCPrjnSpec; }

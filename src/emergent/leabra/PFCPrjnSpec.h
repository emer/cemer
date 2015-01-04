// Copyright, 1995-2013, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of Emergent
//
//   Emergent is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   Emergent is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.

#ifndef PFCPrjnSpec_h
#define PFCPrjnSpec_h 1

// parent includes:
#include <ProjectionSpec>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(PFCPrjnSpec);

class E_API PFCPrjnSpec : public ProjectionSpec {
  // projections involving a PFC layer with unit groups organized by rows into alternating transient and maintaining units, with the first two rows described as INPUT, and the last two rows as OUTPUT
INHERITED(ProjectionSpec)
public:
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
  
  void	Connect_impl(Projection* prjn, bool make_cons) override;

  virtual void Connect_row1to1(Projection* prjn, bool make_cons,
                               int rx, int ry, int recv_x, bool recv_gps);
  virtual void Connect_rowall(Projection* prjn, bool make_cons,
                              int rx, int ry, int recv_x, bool recv_gps);
  virtual void Connect_cols(Projection* prjn, bool make_cons,
                            int rx, int ry, int recv_x, bool recv_gps,
                            int sy, int send_x, bool send_gps);
  
  TA_SIMPLE_BASEFUNS(PFCPrjnSpec);
protected:
  void UpdateAfterEdit_impl();
  
private:
  void Initialize();
  void Destroy()     { };
};

#endif // PFCPrjnSpec_h

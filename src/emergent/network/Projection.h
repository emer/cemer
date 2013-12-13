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

#ifndef Projection_h
#define Projection_h 1

// parent includes:
#include <taBase>

// member includes:
#include <ProjectionRef>
#include <ProjectionSpec>
#include <LayerRef>
#include <RecvCons>
#include <taColor>

// declare all other types mentioned but not required to include:

eTypeDef_Of(Projection);

class E_API Projection: public taNBase {
  // #STEM_BASE ##CAT_Network ##SCOPE_Network Projection describes connectivity between layers (from receivers perspective)
INHERITED(taNBase)
public:
  enum PrjnSource {
    NEXT,               // Recv from the next layer in network
    PREV,               // Recv from the previous layer in network
    SELF,               // Recv from the same layer
    CUSTOM,             // Recv from the layer spec'd in the projection
    INIT                // #IGNORE used during create to skip err msgs
  };

  enum PrjnDirection {  // which direction does this projection come from
    FM_INPUT,           // from layer is closer to input signals than recv layer
    FM_OUTPUT,          // from layer is closer to output signals than recv layer
    LATERAL,            // from layer is same distance from input and output signals as this layer
    DIR_UNKNOWN,        // direction not set
  };

#ifdef __MAKETA__
  String                name;           // #READ_ONLY #SHOW name of the projection -- this is generated automatically based on the from name
#endif

  bool                  off;            // #DEF_false turn this projection off -- useful for experimenting with projections while being able to keep the specifications in place
  bool                  disp;           // display this projection in the network view display
  Layer*                layer;          // #READ_ONLY #NO_SAVE #HIDDEN #NO_SET_POINTER layer this prjn is in
  PrjnSource            from_type;      // #CAT_Structure Source of the projections
  LayerRef              from;           // #CAT_Structure #CONDEDIT_ON_from_type:CUSTOM layer receiving from (set this for custom)
  ProjectionSpec_SPtr   spec;           // #CAT_Structure spec for this item
  TypeDef*              con_type;       // #TYPE_Connection #CAT_Structure Type of connection
  TypeDef*              recvcons_type;  // #TYPE_RecvCons #CAT_Structure Type of receiving connection group to make
  TypeDef*              sendcons_type;  // #TYPE_SendCons #CAT_Structure Type of sending connection group to make
  ConSpec_SPtr          con_spec;       // #CAT_Structure conspec to use for creating connections

  int                   recv_idx;       // #READ_ONLY #CAT_Structure receiving con_group index
  int                   send_idx;       // #READ_ONLY #CAT_Structure sending con_group index
  int                   recv_n;         // #READ_ONLY #CAT_Structure #DEF_1 number of receiving con_groups allocated to this projection: almost always 1 -- some things won't work right if > 1 (e.g., copying)
  int                   send_n;         // #READ_ONLY #CAT_Structure number of sending con_groups: almost always 1 -- some things won't work right if > 1 (e.g., copying)

  bool                  projected;       // #HIDDEN #CAT_Structure t/f if connected

  PrjnDirection         direction;      // #CAT_Structure which direction does this projection go (in terms of distance from input and output layers) -- auto computed by Compute_PrjnDirection or you can manually set; optionally used by only some algorithms
  taColor               prjn_clr;       // #CAT_Structure Default color for the projection line and arrow (subservient to the Type-defined color, if applicable)

  inline ConSpec*       GetConSpec()    { return con_spec.spec.ptr(); }
  // #CAT_Structure get the connection spec for this projection
  inline ProjectionSpec* GetPrjnSpec()  { return spec.spec.ptr(); }
  // #CAT_Structure get the projection spec for this projection

  virtual void  UpdateName();
  // #CAT_Structure update the name of projection to reflect any changes in where it receives from
  virtual void  SetFrom();
  // #CAT_Structure set where to receive from based on selections

  virtual void  SetCustomFrom(Layer* from_lay);
  // #MENU #MENU_ON_Actions #MENU_CONTEXT #DROP1 #DYN1 #CAT_Structure #INIT_ARGVAL_ON_from set a CUSTOM projection from given layer (if from_lay == layer, turns into SELF)

  virtual void  RemoveCons();
  // #MENU #MENU_ON_Actions #CONFIRM #CAT_Structure Reset all connections for this projection

  virtual void  Copy_Weights(const Projection* src);
  // #MENU #MENU_ON_Object #MENU_SEP_BEFORE #CAT_Weights copies weights from other projection
  virtual void  SaveWeights_strm(std::ostream& strm, RecvCons::WtSaveFormat fmt = RecvCons::TEXT);
  // #EXT_wts #COMPRESS #CAT_File write weight values out in a simple ordered list of weights (optionally in binary fmt)
  virtual int   LoadWeights_strm(std::istream& strm, RecvCons::WtSaveFormat fmt = RecvCons::TEXT,
                                 bool quiet = false);
  // #EXT_wts #COMPRESS #CAT_File read weight values in from a simple ordered list of weights (optionally in binary fmt)

  virtual void  SaveWeights(const String& fname="", RecvCons::WtSaveFormat fmt = RecvCons::TEXT);
  // #MENU #EXT_wts #COMPRESS #CAT_File #FILE_DIALOG_SAVE write weight values out in a simple ordered list of weights (optionally in binary fmt) (leave fname empty to pull up file chooser)
  virtual int   LoadWeights(const String& fname="",
                            RecvCons::WtSaveFormat fmt = RecvCons::TEXT, bool quiet = false);
  // #MENU #EXT_wts #COMPRESS #CAT_File #FILE_DIALOG_LOAD read weight values in from a simple ordered list of weights (optionally in binary fmt) (leave fname empty to pull up file chooser)

  virtual void  CheckSpecs();
  // #CAT_Structure check to make sure that specs are not null and set to the right type, and update with new specs etc to fix any errors (with notify), so that at least network operations will not crash -- called in Build and CheckConfig

  // convenience functions for those defined in the spec
  void  PreConnect()            { spec->PreConnect(this); }
  // #CAT_Structure pre-configure connection state
  void  Connect()               { spec->Connect(this); }
  // #BUTTON #CONFIRM #CAT_Structure Make all connections for this projection (resets first)
  void  Connect_impl()          { spec->Connect_impl(this); }
  // #CAT_Structure actually do the connecting
  int   ProbAddCons(float p_add_con, float init_wt = 0.0) { return spec->ProbAddCons(this, p_add_con, init_wt); }
  // #MENU #MENU_ON_Actions #USE_RVAL #CAT_Structure probabilistically add a proportion of new connections to replace those pruned previously, init_wt = initial weight value of new connection
  void  Init_dWt()              { spec->Init_dWt(this); }
  // #MENU #MENU_SEP_BEFORE #CAT_Weights Initialize weight changes for this projection
  void  Init_Weights()          { spec->Init_Weights(this); }
  // #BUTTON #CONFIRM #CAT_Weights Initialize weight state for this projection
  void  Init_Weights_post()     { spec->Init_Weights_post(this); }
  // #CAT_Structure post-initialize state variables (ie. for scaling symmetrical weights, other wt state keyed off of weights, etc)

  void  C_Init_Weights(RecvCons* cg, Unit* ru)  { spec->C_Init_Weights(this, cg, ru); }
  // #CAT_Weights custom initialize weights in this con group for given receiving unit ru

  virtual void  TransformWeights(const SimpleMathSpec& trans);
  // #MENU #MENU_SEP_BEFORE #CAT_Weights apply given transformation to weights
  virtual void  AddNoiseToWeights(const Random& noise_spec);
  // #MENU #CAT_Weights add noise to weights using given noise specification
  virtual int   PruneCons(const SimpleMathSpec& pre_proc,
                             Relation::Relations rel, float cmp_val);
  // #MENU #USE_RVAL #CAT_Weights remove weights that (after pre-proc) meet relation to compare val
  virtual int   LesionCons(float p_lesion, bool permute=true);
  // #MENU #USE_RVAL #CAT_Structure remove connections with prob p_lesion (permute = fixed no. lesioned)

  virtual bool  UpdateConSpecs(bool force = false);
  // #CAT_Structure update con specs for all connection groups for this projection in the network to use con_spec (only if changed from last update -- force = do regardless); returns true if changed and all cons can use given spec

  virtual bool  SetPrjnSpec(ProjectionSpec* sp);
  // #BUTTON #DROP1 #DYN1 #CAT_Structure #INIT_ARGVAL_ON_spec.spec set the projection spec (connectivity pattern) for this projection
  virtual bool  SetConSpec(ConSpec* sp);
  // #BUTTON #DROP1 #DYN1 #CAT_Structure #INIT_ARGVAL_ON_con_spec.spec set the con spec for all connections in this prjn
  virtual bool  CheckConnect(bool quiet=false) { return spec->CheckConnect(this, quiet); }
  // #CAT_Structure check if projection is connected
  virtual void  FixPrjnIndexes();
  // #MENU #CAT_Structure fix the indexes of the connection groups (recv_idx, send_idx)

  virtual int   ReplaceConSpec(ConSpec* old_sp, ConSpec* new_sp);
  // #CAT_Structure switch any connections/projections using old_sp to using new_sp
  virtual int   ReplacePrjnSpec(ProjectionSpec* old_sp, ProjectionSpec* new_sp);
  // #CAT_Structure switch any projections using old_sp to using new_sp

  virtual bool  SetConType(TypeDef* td);
  // #BUTTON #DYN1 #CAT_Structure #TYPE_Connection #INIT_ARGVAL_ON_con_type set the connection type for all connections in this prjn
  virtual bool  SetRecvConsType(TypeDef* td);
  // #BUTTON #DYN1 #CAT_Structure #TYPE_RecvCons #INIT_ARGVAL_ON_recvcons_type set the receiving connection group type for all connections in this prjn
  virtual bool  SetSendConsType(TypeDef* td);
  // #BUTTON #DYN1 #CAT_Structure #TYPE_SendCons #INIT_ARGVAL_ON_sendcons_type set the connection group type for all connections in this prjn

  virtual void  MonitorVar(NetMonitor* net_mon, const String& variable);
  // #BUTTON #DYN1 #CAT_Statistic monitor (record in a datatable) the given variable on this projection

  virtual DataTable*    WeightsToTable(DataTable* dt, const String& col_nm = "");
  // #MENU #NULL_OK  #NULL_TEXT_0_NewTable #CAT_Structure copy entire set of projection weights to given table (e.g., for analysis), with one row per receiving unit, and one column (name is layer name if not otherwise specified) that has a float matrix cell of the geometry of the sending layer
  virtual DataTable*    VarToTable(DataTable* dt, const String& variable);
  // #MENU #NULL_OK_0 #NULL_TEXT_0_NewTable #CAT_Structure send given variable to data table -- number of columns depends on variable (for connection variables, specify r. or s. (e.g., r.wt)) -- this uses a NetMonitor internally, so see documentation there for more information
  virtual DataTable*    ConVarsToTable(DataTable* dt, const String& var1, const String& var2 = "",
               const String& var3 = "", const String& var4 = "", const String& var5 = "",
               const String& var6 = "", const String& var7 = "", const String& var8 = "",
               const String& var9 = "", const String& var10 = "", const String& var11 = "",
               const String& var12 = "", const String& var13 = "", const String& var14 = "");
  // #MENU #NULL_OK_0 #NULL_TEXT_0_NewTable #CAT_Statistics record given connection-level variable to data table with column names the same as the variable names, and one row per *connection* (unlike monitor-based operations which create matrix columns) -- this is useful for performing analyses on learning rules as a function of sending and receiving unit variables -- uses receiver-based connection traversal -- connection variables are just specified directly by name -- corresponding receiver unit variables are "r.var" and sending unit variables are "s.var"

  override String       GetTypeDecoKey() const { return "Projection"; }
  override int    GetEnabled() const    { return !off; }
  override void   SetEnabled(bool value) { off = !value; }

  override bool ChangeMyType(TypeDef* new_type);

  void  InitLinks();
  void  CutLinks();
  void  Copy_(const Projection& cp);
  TA_BASEFUNS(Projection);
protected:
  ConSpec*      m_prv_con_spec; // previous con spec set for cons

  override void UpdateAfterEdit_impl();
  override void UpdateAfterMove_impl(taBase* old_owner);
  override void CheckThisConfig_impl(bool quiet, bool& rval);
private:
  void  Initialize();
  void  Destroy();
};

// needs the actual layer include to compile TA file
#ifdef __TA_COMPILE__
#include <Layer>
#endif

#endif // Projection_h

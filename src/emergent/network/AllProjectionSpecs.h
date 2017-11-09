// Copyright 2017, Regents of the University of Colorado,
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

#ifndef AllProjectionSpecs_h
#define AllProjectionSpecs_h 1

#include <ProjectionSpec>

#include <RndSeed>
#include <taVector2i>
#include <taVector2f>
#include <MinMaxRange>
#include <DataTable>

#include <State_main>

// need to define all the ones for _mbrs here
eTypeDef_Of(TessEl);
eTypeDef_Of(GpTessEl);
eTypeDef_Of(GaussInitWtsSpec);
eTypeDef_Of(SigmoidInitWtsSpec);
eTypeDef_Of(BgPfcPrjnEl);

#include <AllProjectionSpecs_mbrs>

#include <State_main>


eTypeDef_Of(FullPrjnSpec);

class E_API FullPrjnSpec : public ProjectionSpec {
  // Full connectivity between layers
INHERITED(ProjectionSpec)
public:

#include <FullPrjnSpec>
  
  TA_BASEFUNS_NOCOPY(FullPrjnSpec);
private:
  void	Initialize() 		{ Initialize_core(); }
  void 	Destroy()		{ };
};


///////////////////////////////////////////////////////////
//      OneToOne

eTypeDef_Of(OneToOnePrjnSpec);

class E_API OneToOnePrjnSpec : public ProjectionSpec {
  // one-to-one connectivity (1st unit to 1st unit, etc)
INHERITED(ProjectionSpec)
public:

#include <OneToOnePrjnSpec>
  
  TA_SIMPLE_BASEFUNS(OneToOnePrjnSpec);
private:
  void	Initialize()            { Initialize_core(); SetUnique("self_con", true); self_con = true; }
  void 	Destroy()		{ };
};


eTypeDef_Of(GpOneToOnePrjnSpec);

class E_API GpOneToOnePrjnSpec : public OneToOnePrjnSpec {
  // #AKA_RndGpOneToOnePrjnSpec unit_group based one-to-one connectivity, with full or uniform random partial connectivity within unit groups -- if one layer has same number of units as the other does unit groups, then each unit connects to entire unit group
INHERITED(OneToOnePrjnSpec)
public:
#ifdef __MAKETA__
  int	n_conns;		// number of unit groups to connect (-1 for size of layer)
  int	recv_start;		// starting unit group index for recv layer connections -- start making connections in this group -- index goes through x dimension first (inner) then y dimension (outer)
  int	send_start;		// starting unit group index for send layer connections -- start making connections in this group -- index goes through x dimension first (inner) then y dimension (outer)
  bool  use_gp;                 // #HIDDEN this is not used for GpOneToOnePrjnSpec
#endif

#include <GpOneToOnePrjnSpec>

  TA_BASEFUNS_NOCOPY(GpOneToOnePrjnSpec);
private:
  void	Initialize()		{ Initialize_core(); }
  void 	Destroy()		{ };
};


eTypeDef_Of(MarkerGpOneToOnePrjnSpec);

class E_API MarkerGpOneToOnePrjnSpec : public OneToOnePrjnSpec {
  // unit_group based one-to-one connectivity for marking a projection -- all of the recv units receive from just the first unit in the sending unit group, thus providing a marker for where to receive information from the sending group
INHERITED(OneToOnePrjnSpec)
public:

#include <MarkerGpOneToOnePrjnSpec>  
  
  TA_SIMPLE_BASEFUNS(MarkerGpOneToOnePrjnSpec);
private:
  void Initialize()  { Initialize_core(); }
  void Destroy()     { };
};


eTypeDef_Of(GpMapConvergePrjnSpec);

class E_API GpMapConvergePrjnSpec : public ProjectionSpec {
  // #AKA_GpAggregatePrjnSpec generates a converging map of the units within a sending layer that has unit groups into a receiving layer that has the same geometry as one of the unit groups -- each recv unit receives from the corresponding unit in all of the sending unit groups
INHERITED(ProjectionSpec)
public:

#include <GpMapConvergePrjnSpec>

  TA_SIMPLE_BASEFUNS(GpMapConvergePrjnSpec);
private:
  void	Initialize()            { Initialize_core(); }
  void 	Destroy()		{ };
};

eTypeDef_Of(GpMapDivergePrjnSpec);

class E_API GpMapDivergePrjnSpec : public ProjectionSpec {
  // projects from a layer without unit groups into a receiving layer with unit groups and that has the same unit geometry in each of its unit groups as the sending layer -- each unit projects to the corresponding unit in all of the receiving unit groups
INHERITED(ProjectionSpec)
public:
  
#include <GpMapDivergePrjnSpec>

  TA_SIMPLE_BASEFUNS(GpMapDivergePrjnSpec);
private:
  void	Initialize()            { Initialize_core(); }
  void 	Destroy()		{ };
};


///////////////////////////////////////////////////////////
//      Random

eTypeDef_Of(RandomPrjnSpec);

class E_API RandomPrjnSpec : public ProjectionSpec {
  // Connects all units with probability p_con -- note it ALWAYS uses the same seed, because of the two-pass nature of the connection process -- you can update rndm_seed prior to connecting to get different patterns of connectivity
INHERITED(ProjectionSpec)
public:

#include <RandomPrjnSpec>
  
  TA_SIMPLE_BASEFUNS(RandomPrjnSpec);
protected:
  void UpdateAfterEdit_impl() override {
    inherited::UpdateAfterEdit_impl();
    if(p_con > 1.0f) p_con = 1.0f;
    if(p_con < 0.0f) p_con = 0.0f;
  }
   
private:
  void	Initialize()    { Initialize_core(); }
  void 	Destroy()	{ };
};


eTypeDef_Of(UniformRndPrjnSpec);

class E_API UniformRndPrjnSpec : public ProjectionSpec {
  // Uniform random connectivity between layers -- only 'permute' stye randomness is supported, creates same number of connections per unit
INHERITED(ProjectionSpec)
public:

#include <UniformRndPrjnSpec>
  
  TA_SIMPLE_BASEFUNS(UniformRndPrjnSpec);
protected:
  void UpdateAfterEdit_impl() override {
    inherited::UpdateAfterEdit_impl();
    if(p_con > 1.0f) p_con = 1.0f;
    if(p_con < 0.0f) p_con = 0.0f;
  }
   
private:
  void	Initialize()    { Initialize_core(); }
  void 	Destroy()	{ };
};


eTypeDef_Of(PolarRndPrjnSpec);

class E_API PolarRndPrjnSpec : public ProjectionSpec {
  // random connectivity defined as a function of distance and angle
INHERITED(ProjectionSpec)
public:

#include <PolarRndPrjnSpec>
  
  TA_SIMPLE_BASEFUNS(PolarRndPrjnSpec);
protected:
  void UpdateAfterEdit_impl() override;
private:
  void	Initialize()    { Initialize_core(); }
  void	Destroy()	{ };
};


///////////////////////////////////////////////////////////
//      Tessel 

eTypeDef_Of(TessEl_List);

class E_API TessEl_List : public taList<TessEl> {
  // ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Spec list of TessEl objects
INHERITED(taList<TessEl>)
public:
  TA_BASEFUNS_LITE_NOCOPY(TessEl_List);
private:
  void	Initialize() 		{ };
  void 	Destroy()		{ };
};

eTypeDef_Of(TesselPrjnSpec);

class E_API TesselPrjnSpec : public ProjectionSpec {
  // arbitrary tesselations (repeating patterns) of connectivity -- sweeps over receiving units and connects with sending units based on projection of recv unit position into sending layer, plus sending offsets that specify the connectivity pattern
INHERITED(ProjectionSpec)
public:

#include <TesselPrjnSpec>

  String	last_make_cmd; // #READ_ONLY #SHOW shows the last Make.. command that was run (if blank, none or it was done prior to the addition of this feature in version 4.1.0) -- useful for modifying later
  String	last_weights_cmd; // #READ_ONLY #SHOW shows the last Weights.. command that was run (if blank, none or it was done prior to the addition of this feature in version 4.1.0) -- useful for modifying later
  TessEl_List	send_offs;	// offsets of the sending units -- these are added to the location of the recv unit to determine which sending units to receive from -- can create any arbitrary patterns here, or use the MakeEllipse or MakeRectangle buttons to create those std patterns

  virtual void	MakeEllipse(int half_width, int half_height, int ctr_x, int ctr_y);
  // #BUTTON #MENU_SEP_BEFORE make a connection pattern in the form of an elipse: center is located at ctr_x,y and extends half_width and half_height therefrom
  virtual void	MakeRectangle(int width, int height, int left, int bottom);
  // #BUTTON make a connection pattern in the form of a rectangle starting at left, bottom coordinate and going right and up by width, height
//TODO  virtual void	MakeFromNetView(NetView* view);
  /* #MENU make connection pattern from selected units in netview,
     first unit is center, then sender positions */
  virtual void	WeightsFromDist(float scale);
  // #BUTTON assign weights as a linear function of sender distance
  virtual void	WeightsFromGausDist(float scale, float sigma);
  // #BUTTON assign weights as a Gaussian function of sender distance

  virtual void  SyncSendOffs();
  // #IGNORE sync our dynamic send_offs array with the compiled version used by actual State projection code
  virtual void  CopyToState_SendOffs(void* state_spec, const char* state_suffix);
  // #IGNORE copy our compiled send offs data to other state spec

  void    CopyToState(void* state_spec, const char* state_suffix) override;
  void    UpdateStateSpecs() override;
  
  TA_SIMPLE_BASEFUNS(TesselPrjnSpec);
protected:
  void UpdateAfterEdit_impl() override;
private:
  void	Initialize() { Initialize_core(); send_offs.SetBaseType(&TA_TessEl); }
  void	Destroy()    { CutLinks(); FreeSendOffs(); }
};


///////////////////////////////////////////////////////////
//      GpTessel 

eTypeDef_Of(GpTessEl_List);

class E_API GpTessEl_List : public taList<GpTessEl> {
  // ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Spec list of GpTessEl objects
INHERITED(taList<GpTessEl>)
public:
  TA_BASEFUNS_LITE_NOCOPY(GpTessEl_List);
private:
void	Initialize() 		{ SetBaseType(&TA_GpTessEl); }
  void 	Destroy()		{ };
};

eTypeDef_Of(GpTesselPrjnSpec);

class E_API GpTesselPrjnSpec : public ProjectionSpec {
  // #AKA_GpRndTesselPrjnSpec specifies tesselated patterns of groups to connect with (both recv and send layers must have unit groups), optionally with random connectivity within each group (also very useful for full connectivity -- has optimized support for that) -- only 'permute' style randomness is supported, producing same number of recv connections per unit
INHERITED(ProjectionSpec)
public:

#include <GpTesselPrjnSpec>

  String	last_make_cmd; // #READ_ONLY #SHOW shows the last Make.. command that was run (if blank, none or it was done prior to the addition of this feature in version 8.0.0) -- useful for modifying later
  GpTessEl_List	send_gp_offs;	// offsets of the sending unit groups

  
  virtual void	MakeRectangle(int width, int height, int left, int bottom);
  // #BUTTON make a connection pattern in the form of a rectangle starting at left, bottom coordinate and going right and up by width, height
  virtual void	MakeEllipse(int half_width, int half_height, int ctr_x, int ctr_y);
  // #BUTTON make a connection pattern in the form of an elipse: center is located at ctr_x,y and extends half_width and half_height therefrom
  virtual void	SetPCon(float p_con, int start = 0, int end = -1);
  // #BUTTON set p_con value for a range of send_gp_offs (default = all; end-1 = all)

  virtual void  SyncSendOffs();
  // #IGNORE sync our dynamic send_offs array with the compiled version used by actual State projection code
  virtual void  CopyToState_SendOffs(void* state_spec, const char* state_suffix);
  // #IGNORE copy our compiled send offs data to other state spec

  void    CopyToState(void* state_spec, const char* state_suffix) override;
  void    UpdateStateSpecs() override;

  TA_SIMPLE_BASEFUNS(GpTesselPrjnSpec);
protected:
  void UpdateAfterEdit_impl() override;

private:
  void	Initialize() { Initialize_core(); send_gp_offs.SetBaseType(&TA_GpTessEl); }
  void	Destroy()    { CutLinks(); FreeSendOffs(); }
};


///////////////////////////////////////////////////////////
//      Tiled..

eTypeDef_Of(TiledGpRFPrjnSpec);

class E_API TiledGpRFPrjnSpec : public ProjectionSpec {
  // Tiled receptive field projection spec for entirely group-to-group connections: connects entire receiving layer unit groups with overlapping tiled regions of sending layer groups -- if init_wts is on, gaussian or sigmoid topographic weights are initialized
INHERITED(ProjectionSpec)
public:

#include <TiledGpRFPrjnSpec>  

  taVector2i 	trg_recv_geom;	// #READ_ONLY #SHOW target receiving layer gp geometry -- computed from send and rf_width, move by TrgRecvFmSend button, or given by TrgSendFmRecv
  taVector2i 	trg_send_geom;	// #READ_ONLY #SHOW target sending layer geometry -- computed from recv and rf_width, move by TrgSendFmRecv button, or given by TrgRecvFmSend

  virtual bool	TrgRecvFmSend(int send_x, int send_y);
  // #BUTTON compute target recv layer geometry based on given sending layer geometry -- updates trg_recv_geom and trg_send_geom members, including fixing send to be an appropriate even multiple of rf_move -- returns true if send values provided result are same "good" ones that come out the end
  virtual bool	TrgSendFmRecv(int recv_x, int recv_y);
  // #BUTTON compute target recv layer geometry based on given sending layer geometry -- updates trg_recv_geom and trg_send_geom members, including fixing recv to be an appropriate even multiple of rf_move --  -- returns true if send values provided result are same "good" ones that come out the end

  TA_SIMPLE_BASEFUNS(TiledGpRFPrjnSpec);
protected:
  void  UpdateAfterEdit_impl() override;
  
private:
  void	Initialize()            { Initialize_core(); }
  void 	Destroy()		{ };
};


eTypeDef_Of(TiledGpRFOneToOnePrjnSpec);

class E_API TiledGpRFOneToOnePrjnSpec : public TiledGpRFPrjnSpec {
  // TiledGpRFPrjnSpec connectvity with one-to-one connections for units with the same index within a unit group -- useful for establishing connections among layers with the same unit-group structure (see also TiledGpRFOneToOneWtsPrjnSpec for a softer version where only weights are set)
INHERITED(TiledGpRFPrjnSpec)
public:

#include <TiledGpRFOneToOnePrjnSpec>
  
  TA_SIMPLE_BASEFUNS(TiledGpRFOneToOnePrjnSpec);
private:
  void	Initialize()    { Initialize_core(); }
  void	Destroy()	{ };
};


eTypeDef_Of(TiledGpRFOneToOneWtsPrjnSpec);

class E_API TiledGpRFOneToOneWtsPrjnSpec : public TiledGpRFPrjnSpec {
  // TiledGpRFPrjnSpec connectvity with initial weights (when init_wts is set) that have differential weights for units with the same index within a unit group vs. differential weights -- useful for establishing connections among layers with the same unit-group structure (see also TiledGpRFOnetToOnePrjnSpec for harder version where connections are only made among units with same index within group)
INHERITED(TiledGpRFPrjnSpec)
public:

#include <TiledGpRFOneToOneWtsPrjnSpec>

  TA_SIMPLE_BASEFUNS(TiledGpRFOneToOneWtsPrjnSpec);
private:
  void	Initialize()    { Initialize_core(); }
  void	Destroy()	{ };
};



eTypeDef_Of(TiledSubGpRFPrjnSpec);

class E_API TiledSubGpRFPrjnSpec : public ProjectionSpec {
  // Tiled receptive field projection spec for topographic tiled group-to-group connections, with a sub-tiling of unit groups within the larger receptive field tiling, to divide the larger problem into more managable subsets: connects entire receiving layer unit groups with overlapping tiled regions of sending layer groups -- if init_wts is on, gaussian topographic weights are initialized
INHERITED(ProjectionSpec)
public:

#include <TiledSubGpRFPrjnSpec>
  
  taVector2i 	trg_recv_geom;	// #READ_ONLY #SHOW target receiving layer gp geometry -- computed from send and rf_width, move by TrgRecvFmSend button, or given by TrgSendFmRecv
  taVector2i 	trg_send_geom;	// #READ_ONLY #SHOW target sending layer geometry -- computed from recv and rf_width, move by TrgSendFmRecv button, or given by TrgRecvFmSend

  virtual bool	TrgRecvFmSend(int send_x, int send_y);
  // #BUTTON compute target recv layer geometry based on given sending layer geometry -- updates trg_recv_geom and trg_send_geom members, including fixing send to be an appropriate even multiple of rf_move -- returns true if send values provided result are same "good" ones that come out the end
  virtual bool	TrgSendFmRecv(int recv_x, int recv_y);
  // #BUTTON compute target recv layer geometry based on given sending layer geometry -- updates trg_recv_geom and trg_send_geom members, including fixing recv to be an appropriate even multiple of rf_move --  -- returns true if send values provided result are same "good" ones that come out the end

  TA_SIMPLE_BASEFUNS(TiledSubGpRFPrjnSpec);
protected:
  void  UpdateAfterEdit_impl() override;
  
private:
  void Initialize()  { Initialize_core(); }
  void Destroy()     { };
};


eTypeDef_Of(TiledRFPrjnSpec);

class E_API TiledRFPrjnSpec : public ProjectionSpec {
  // Tiled receptive field projection spec: connects entire receiving layer unit groups with overlapping tiled regions of sending layers
INHERITED(ProjectionSpec)
public:

#include <TiledRFPrjnSpec>

  virtual void	SelectRF(Projection* prjn);
  // #BUTTON select all sending and receiving units in the receptive field of this projection

  TA_SIMPLE_BASEFUNS(TiledRFPrjnSpec);
private:
  void	Initialize()    { Initialize_core(); }
  void 	Destroy()	{ };
};


eTypeDef_Of(TiledNovlpPrjnSpec);

class E_API TiledNovlpPrjnSpec : public ProjectionSpec {
  // Tiled non-overlapping projection spec: connects entire receiving layer unit groups with non-overlapping tiled regions of sending units
INHERITED(ProjectionSpec)
public:

#include <TiledNovlpPrjnSpec>
  
//   virtual void	SelectRF(Projection* prjn);
  // #BUTTON select all sending and receiving units in the receptive field of this projection

  TA_SIMPLE_BASEFUNS(TiledNovlpPrjnSpec);
private:
  void	Initialize()    { Initialize_core(); }
  void 	Destroy()       { };
};


eTypeDef_Of(TiledGpMapConvergePrjnSpec);

class E_API TiledGpMapConvergePrjnSpec : public ProjectionSpec {
  // generates a converging map of the units within a sending layer with unit groups, using tiled overlapping receptive fields within each unit group -- each recv unit receives from the corresponding unit in all of the sending unit groups, with the recv units organized into unit groups that each recv from one tiled subset of sending units within all the sending unit groups -- there must be the same number of recv unit groups as tiled subsets within the sending unit groups
INHERITED(ProjectionSpec)
public:

#include <TiledGpMapConvergePrjnSpec>

  taVector2i 	 trg_recv_geom;	// #READ_ONLY #SHOW target receiving layer gp geometry -- computed from send and rf_width, move by TrgRecvFmSend button, or given by TrgSendFmRecv
  taVector2i 	 trg_send_geom;	// #READ_ONLY #SHOW target sending layer *unit group* geometry -- computed from recv and rf_width, move by TrgSendFmRecv button, or given by TrgRecvFmSend

  virtual bool	TrgRecvFmSend(int send_x, int send_y);
  // #BUTTON compute target recv layer geometry based on given sending unit group geometry (size of one unit group within sending layer) -- updates trg_recv_geom and trg_send_geom members, including fixing send to be an appropriate even multiple of rf_move -- returns true if send values provided result are same "good" ones that come out the end
  virtual bool	TrgSendFmRecv(int recv_x, int recv_y);
  // #BUTTON compute target recv layer geometry based on given sending layer geometry -- updates trg_recv_geom and trg_send_geom members, including fixing recv to be an appropriate even multiple of rf_move --  -- returns true if send values provided result are same "good" ones that come out the end

  TA_SIMPLE_BASEFUNS(TiledGpMapConvergePrjnSpec);
private:
  void	Initialize()    { Initialize_core(); }
  void 	Destroy()	{ };
};

///////////////////////////////////////////////////////////
//      Gaussian, Gradient..


eTypeDef_Of(GaussRFPrjnSpec);

class E_API GaussRFPrjnSpec : public ProjectionSpec {
  // a simple receptive-field (RF) projection spec with gaussian weight values over a receptive-field window onto the sending layer that moves as a function of the receiving unit's position (like TesselPrjnSpec and other RF prjn specs, but does NOT use unit groups) -- useful for reducing larger layers to smaller ones for example
INHERITED(ProjectionSpec)
public:

#include <GaussRFPrjnSpec>

  taVector2i 	 trg_recv_geom;	// #READ_ONLY #SHOW target receiving layer geometry (either gp or unit, depending on outer vs. inner) -- computed from send and rf_width, move by TrgRecvFmSend button, or given by TrgSendFmRecv
  taVector2i 	 trg_send_geom;	// #READ_ONLY #SHOW target sending layer geometry -- computed from recv and rf_width, move by TrgSendFmRecv button, or given by TrgRecvFmSend

  virtual bool	TrgRecvFmSend(int send_x, int send_y);
  // #BUTTON compute target recv layer geometry based on given sending layer geometry -- updates trg_recv_geom and trg_send_geom members, including fixing send to be an appropriate even multiple of rf_move -- returns true if send values provided result are same "good" ones that come out the end
  virtual bool	TrgSendFmRecv(int recv_x, int recv_y);
  // #BUTTON compute target recv layer geometry based on given sending layer geometry -- updates trg_recv_geom and trg_send_geom members, including fixing recv to be an appropriate even multiple of rf_move --  -- returns true if send values provided result are same "good" ones that come out the end

  TA_SIMPLE_BASEFUNS(GaussRFPrjnSpec);
private:
  void	Initialize()    { Initialize_core(); }
  void 	Destroy()	{ };
};


eTypeDef_Of(GradientWtsPrjnSpec);

class E_API GradientWtsPrjnSpec : public FullPrjnSpec {
  // full connectivity with a gradient of weight strengths (requires init_wts = true, otherwise is just like Full Prjn), where weights are strongest from sending units in same relative location as the receiving unit, and fall off from there (either linearly or as a Guassian) -- if recv layer has unit groups, then it is the unit group position that counts, and all units within the recv group have the same connectivity (can override with use_gps flag)
INHERITED(FullPrjnSpec)
public:

#include <GradientWtsPrjnSpec>
  
  TA_SIMPLE_BASEFUNS(GradientWtsPrjnSpec);
private:
 void	Initialize()    { Initialize_core(); }
  void 	Destroy()	{ };
};


///////////////////////////////////////////////////////////
//      PFC / BG


eTypeDef_Of(PFCPrjnSpec);

class E_API PFCPrjnSpec : public ProjectionSpec {
  // projections involving a PFC layer with unit groups organized by rows into alternating transient and maintaining units, with the first two rows described as INPUT, and the last two rows as OUTPUT
INHERITED(ProjectionSpec)
public:

#include <PFCPrjnSpec>  
  
  TA_SIMPLE_BASEFUNS(PFCPrjnSpec);
protected:
  void UpdateAfterEdit_impl() override;
  
private:
  void Initialize()  { Initialize_core(); }
  void Destroy()     { };
};


eTypeDef_Of(BgPfcPrjnSpec);

class E_API BgPfcPrjnSpec : public ProjectionSpec {
  // for connecting BG and PFC layers, where there are separate PFC layers that interconnect with a single BG layer (Matrix, GPi, etc), allowing competition within the BG -- has a customizable data table of the different PFC layers that all map to the same BG layer -- also supports connections from a Patch layer with same name root as PFC
INHERITED(ProjectionSpec)
public:

#include <BgPfcPrjnSpec>
  
  enum BgTableVals { // the different values stored in bg_table -- for rapid access
    BGT_NAME,
    BGT_SIZE_X,
    BGT_SIZE_Y,
    BGT_START_X,
    BGT_START_Y,
  };

  DataTable     bg_table;      // #TREE_SHOW #EXPERT table of PFC layers that map into a common BG layer -- one row per PFC layer -- you specify the name of each PFC layer and its size in unit groups (x,y), and optionally a starting x,y unit group offset within the BG layer (-1 means use default horizontal layout of pfc's within bg) -- mouse over the column headers for important further details
  String        connect_as;          // #CONDSHOW_ON_cross_connect PFC layer name to connect as -- see cross_connect option for details

  inline int    FindBgTableRow(const String& name) {
    return bg_table.FindVal(name, BGT_NAME, 0, true);
  }
  // find table row for given pfc name -- emits error if not found
  inline Variant  GetBgTableVal(BgTableVals val, int row) {
    return bg_table.GetVal(val, row);
  }
  // get specific dyn value for given row
  inline void   SetBgTableVal(const Variant& vl, BgTableVals val, int row) {
    bg_table.SetVal(vl, val, row);
  }
  // set specific dyn value for given row
  
  virtual void  FormatBgTable();
  // #IGNORE format the bg table
  virtual void  InitBgTable();
  // default initial bg table
  virtual void  UpdtBgTable();
  // #BUTTON update the bg table to ensure consistency of everything

  virtual void  SyncPfcLayers();
  // #IGNORE sync our dynamic pfc_layers array with the compiled version used by actual State projection code
  virtual void  CopyToState_PfcLayers(void* state_spec, const char* state_suffix);
  // #IGNORE copy our compiled pfc_layers data to other state spec

  void    CopyToState(void* state_spec, const char* state_suffix) override;
  void    UpdateStateSpecs() override;

  TA_SIMPLE_BASEFUNS(BgPfcPrjnSpec);
protected:
  void UpdateAfterEdit_impl() override;
  
private:
  void Initialize();
  void Destroy()     { CutLinks(); FreePfcLayers(); }
};

#endif // AllProjectionSpecs_h

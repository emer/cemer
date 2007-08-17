// Copyright, 1995-2005, Regents of the University of Colorado,
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



// netstru_extra.h
// extra netstru objects derived from basic ones that are included in standard
// pdp library

#ifndef netstru_extra_h
#define netstru_extra_h

#include "netstru.h"

class EMERGENT_API FullPrjnSpec : public ProjectionSpec {
  // Full connectivity between layers
INHERITED(ProjectionSpec)
public:
  void 	Connect_impl(Projection* prjn);
  // Connection function for full connectivity
  int 	ProbAddCons(Projection* prjn, float p_add_con, float init_wt = 0.0);

  TA_BASEFUNS_NOCOPY(FullPrjnSpec);
private:
  void	Initialize() 		{ };
  void 	Destroy()		{ };
};

class EMERGENT_API TessEl : public taOBase {
  // ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Spec one element of a tesselation specification
INHERITED(taOBase)
public:
  TwoDCoord	send_off;	// offset from current receiving unit
  float		wt_val;		// value to assign to weight

  TA_SIMPLE_BASEFUNS(TessEl);
private:
  void	Initialize();
  void	Destroy()	{ };
};

class EMERGENT_API TessEl_List : public taList<TessEl> {
  // ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Spec list of TessEl objects
INHERITED(taList<TessEl>)
public:
  TA_BASEFUNS_LITE_NOCOPY(TessEl_List);
private:
  void	Initialize() 		{ };
  void 	Destroy()		{ };
};

class EMERGENT_API TesselPrjnSpec : public ProjectionSpec {
  // arbitrary tesselations (repeating patterns) of connectivity
INHERITED(ProjectionSpec)
public:
  TwoDCoord	recv_off;	// offset in layer for start of recv units to begin connecting
  TwoDCoord	recv_n;		// number of receiving units to connect in each dimension (-1 for all)
  TwoDCoord	recv_skip;	// increment for recv units in each dimension -- 1 = connect all units; 2 = skip every other unit, etc
  TwoDCoord	recv_group;	// group together this many units under the same starting coord, resulting in a tile pattern
  bool		wrap;		// whether to wrap coordinates around (else clip)
  FloatTwoDCoord send_scale;	// scale to apply to transform receiving unit coords into sending unit coords
  TwoDCoord	send_border;	// border size around sending layer (constant offset to add to sending offsets)

  TessEl_List	send_offs;	// offsets of the sending units

  void		Connect_impl(Projection* prjn);
  void		C_Init_Weights(Projection* prjn, RecvCons* cg, Unit* ru);
  // uses weight values as specified in the tesselel's

  virtual void	GetCtrFmRecv(TwoDCoord& sctr, TwoDCoord ruc);
  // get center of sender coords from receiving coords
  virtual void	Connect_RecvUnit(Unit* ru_u, const TwoDCoord& ruc, Projection* prjn);
  // connects one recv unit to all senders (doesn't check for linking..)

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

  TA_SIMPLE_BASEFUNS(TesselPrjnSpec);
protected:
  override void UpdateAfterEdit_impl();
private:
  void	Initialize();
  void	Destroy()	{ };
};

class EMERGENT_API OneToOnePrjnSpec : public ProjectionSpec {
  // one-to-one connectivity (1st unit to 1st unit, etc)
INHERITED(ProjectionSpec)
public:
  int	n_conns;		// number of connections to make (-1 for size of layer)
  int	recv_start;		// starting unit index for recv connections
  int 	send_start;		// starting unit index for sending connections

  void	Connect_impl(Projection* prjn);

  TA_SIMPLE_BASEFUNS(OneToOnePrjnSpec);
private:
  void	Initialize();
  void 	Destroy()		{ };
};

class EMERGENT_API UniformRndPrjnSpec : public ProjectionSpec {
  // Uniform random connectivity between layers -- only 'permute' stye randomness is supported, creates same number of connections per unit
INHERITED(ProjectionSpec)
public:
  float		p_con;		// overall probability of connection
  bool		sym_self;	// if a self projection, make it symmetric (senders = receivers) otherwise it is not
  bool		same_seed;	// use the same random seed each time (same connect pattern)
  RndSeed	rndm_seed;	// #HIDDEN random seed

  void 	Connect_impl(Projection* prjn);
  // Connection function for full connectivity

  TA_SIMPLE_BASEFUNS(UniformRndPrjnSpec);
protected:
  override void UpdateAfterEdit_impl();
private:
  void	Initialize();
  void 	Destroy()		{ };
};

class EMERGENT_API PolarRndPrjnSpec : public ProjectionSpec {
  // random connectivity defined as a function of distance and angle
INHERITED(ProjectionSpec)
public:
  enum UnitDistType {	// how to compute the distance between two units
    XY_DIST,		// X-Y axis distance between units
    XY_DIST_CENTER, 	// centered distance (layers centered over each other)
    XY_DIST_NORM,	// normalized range (0-1 for each layer) distance
    XY_DIST_CENTER_NORM  // normalized and centered (-1:1 for each layer) distance
  };

  float		p_con;		// overall probability of connection (number of samples)
  Random 	rnd_dist;	// prob density of connectivity as a fctn of distance
  Random	rnd_angle;	// prob density of connectivity as a fctn of angle (1 = 2pi)
  UnitDistType	dist_type; 	// type of distance function to use
  bool		wrap;		// wrap around layer coordinates (else clip at ends)
  int	       	max_retries;	// maximum number of times attempt to con same sender allowed
  bool		same_seed;	// use the same random seed each time (same connect pattern)
  RndSeed	rndm_seed;	// #HIDDEN random seed

  void	Connect_impl(Projection* prjn);

  void	C_Init_Weights(Projection* prjn, RecvCons* cg, Unit* ru);
  // uses weight values equal to the distance probability

  static float	UnitDist(UnitDistType typ, Projection* prjn,
			 const TwoDCoord& ru, const TwoDCoord& su);
  // computes the distance between two units according to distance type
  static Unit*	GetUnitFmOff(UnitDistType typ, bool wrap, Projection* prjn,
			     const TwoDCoord& ru, const FloatTwoDCoord& su_off);
  // gets unit from real-valued offset scaled according to distance type

  virtual float	GetDistProb(Projection* prjn, Unit* ru, Unit* su);
  // compute the probability for connecting two units as a fctn of distance

  TA_SIMPLE_BASEFUNS(PolarRndPrjnSpec);
protected:
  override void UpdateAfterEdit_impl();
private:
  void	Initialize();
  void	Destroy()	{ };
};

class EMERGENT_API SymmetricPrjnSpec : public ProjectionSpec {
  // connects units with receiving connection where sending one already exists
INHERITED(ProjectionSpec)
public:
  void 	Connect_impl(Projection* prjn);

  TA_BASEFUNS_NOCOPY(SymmetricPrjnSpec);
private:
  void	Initialize()	{ };
  void	Destroy()	{ };
};

class EMERGENT_API ScriptPrjnSpec : public ProjectionSpec, public ScriptBase {
  // Script-controlled connectivity
INHERITED(ProjectionSpec)
public:
  Projection*	prjn;		// #READ_ONLY #NO_SAVE this holds the argument to the prjn
  SArg_Array	s_args;		// string-valued arguments to pass to script

  void		Connect_impl(Projection* prj);

  TypeDef*	GetThisTypeDef() const	{ return GetTypeDef(); }
  void*		GetThisPtr()		{ return (void*)this; }

  virtual void	Compile();
  // #BUTTON compile script from script file into internal runnable format

  void	InitLinks();
  void	Copy_(const ScriptPrjnSpec& cp);
  TA_BASEFUNS(ScriptPrjnSpec);
protected:
  override void UpdateAfterEdit_impl();
private:
  void	Initialize();
  void 	Destroy();
};

class EMERGENT_API CustomPrjnSpec : public ProjectionSpec {
  // connectivity is defined manually (i.e. unit-by-unit)
INHERITED(ProjectionSpec)
public:

  void	Connect(Projection* prjn);	// do nothing
  TA_BASEFUNS_NOCOPY(CustomPrjnSpec);
private:
  void	Initialize()	{ };
  void	Destroy()	{ };
};


//////////////////////////////////////////
//	UnitGroup-based PrjnSpecs	//
//////////////////////////////////////////

class EMERGENT_API GpOneToOnePrjnSpec : public OneToOnePrjnSpec {
  // unit_group based one-to-one connectivity (all in 1st group to all in 1st group, etc)
INHERITED(OneToOnePrjnSpec)
public:
  void	Connect_impl(Projection* prjn);

  TA_BASEFUNS_NOCOPY(GpOneToOnePrjnSpec);
private:
  void	Initialize()		{ };
  void 	Destroy()		{ };
};

class EMERGENT_API RndGpOneToOnePrjnSpec : public GpOneToOnePrjnSpec {
  // uniform random connectivity between one-to-one groups -- only 'permute' style random connectivity is supported (same number of connections across recv units)
INHERITED(GpOneToOnePrjnSpec)
public:
  float		p_con;		// overall probability of connection
  bool		same_seed;	// use the same random seed each time (same connect pattern)
  RndSeed	rndm_seed;	// #HIDDEN random seed

  void	Connect_impl(Projection* prjn);

  TA_SIMPLE_BASEFUNS(RndGpOneToOnePrjnSpec);
protected:
  override void UpdateAfterEdit_impl();
private:
  void	Initialize();
  void 	Destroy()		{ };
};

class EMERGENT_API GpOneToManyPrjnSpec : public OneToOnePrjnSpec {
  // unit_group based one-to-many connectivity (one sending gp to all recv units)
INHERITED(OneToOnePrjnSpec)
public:
  enum NConGroups {		// number of connection groups for this projection
    RECV_SEND_PAIR,		// create separate con_groups for each recv_send pair
    SEND_ONLY,			// create separate recv con_groups for ea sending gp
    ONE_GROUP 			// make only one con_group
  };

  NConGroups	n_con_groups;	// number of con_groups to create

  virtual void 	GetNGroups(Projection* prjn, int& r_n_ugp, int& s_n_ugp);
  // get number of connection groups for the projection

  void	PreConnect(Projection* prjn);
  void	Connect_impl(Projection* prjn);

  TA_SIMPLE_BASEFUNS(GpOneToManyPrjnSpec);
private:
  void	Initialize();
  void 	Destroy()		{ };
};

class EMERGENT_API GpTessEl : public taOBase {
  // ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Spec one element of a tesselation specification for groups
INHERITED(taOBase)
public:
  TwoDCoord	send_gp_off;	// offset of group from current receiving group
  float		p_con;		// proportion connectivity from this group -- negative value means just make symmetric cons

  TA_SIMPLE_BASEFUNS(GpTessEl);
private:
  void	Initialize();
  void	Destroy()	{ };
};

class EMERGENT_API GpTessEl_List : public taList<GpTessEl> {
  // ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Spec list of GpTessEl objects
INHERITED(taList<GpTessEl>)
public:
  TA_BASEFUNS_LITE_NOCOPY(GpTessEl_List);
private:
  void	Initialize() 		{ };
  void 	Destroy()		{ };
};

class EMERGENT_API GpRndTesselPrjnSpec : public ProjectionSpec {
  // specifies patterns of groups to connect with, with random connectivity within each group -- only 'permute' style randomness is suported, producing same number of recv connections per unit
INHERITED(ProjectionSpec)
public:
  TwoDCoord	recv_gp_off; 	// offset for start of recv group to begin connecting
  TwoDCoord	recv_gp_n;    	// number of receiving groups to connect in each dimension (-1 for all)
  TwoDCoord	recv_gp_skip; 	// increment for recv groups in each dimension -- 1 = connect all groups; 2 = skip every other group, etc
  TwoDCoord	recv_gp_group;	// group together this many units under the same starting coord, resulting in a tile pattern
  FloatTwoDCoord send_gp_scale;	// scale to apply to transform receiving unit coords into sending unit coords
  TwoDCoord	send_gp_border; // border size around sending layer (constant offset to add to sending offsets)
  TessEl_List	send_gp_offs;	// offsets of the sending units
  bool		wrap;		// whether to wrap coordinates around (else clip)
  float		def_p_con;	// default probability of connectivity when new send_gp_offs are created
  bool		sym_self;	// if a self projection, make it symmetric (senders = receivers) otherwise it is not
  bool		same_seed;	// use the same random seed each time (same connect pattern)
  RndSeed	rndm_seed;	// #HIDDEN random seed

  void		Connect_impl(Projection* prjn);

  virtual void	GetCtrFmRecv(TwoDCoord& sctr, TwoDCoord ruc);
  // get center of sender coords from receiving coords
  virtual void  Connect_Gps(Unit_Group* ru_gp, Unit_Group* su_gp, float p_con,
			    Projection* prjn);
  // #IGNORE impl connect send/recv gps
  virtual void  Connect_Gps_Sym(Unit_Group* ru_gp, Unit_Group* su_gp, float p_con,
				Projection* prjn);
  // #IGNORE symmetric (p_con < 0)
  virtual void  Connect_Gps_SymSameGp(Unit_Group* ru_gp, Unit_Group* su_gp, float p_con,
				      Projection* prjn);
  // #IGNORE symmetric, same unit group
  virtual void  Connect_Gps_SymSameLay(Unit_Group* ru_gp, Unit_Group* su_gp, float p_con,
				      Projection* prjn);
  // #IGNORE symmetric, same layer
  virtual void  Connect_Gps_Std(Unit_Group* ru_gp, Unit_Group* su_gp, float p_con,
				Projection* prjn);
  // #IGNORE standard, not symmetric/same
  virtual void	Connect_RecvGp(Unit_Group* ru_gp, const TwoDCoord& ruc, Projection* prjn);
  // connects one recv unit to all senders

  virtual void	MakeRectangle(int width, int height, int left, int bottom);
  // #BUTTON make a connection pattern in the form of a rectangle starting at left, bottom coordinate and going right and up by width, height
  virtual void	MakeEllipse(int half_width, int half_height, int ctr_x, int ctr_y);
  // #BUTTON make a connection pattern in the form of an elipse: center is located at ctr_x,y and extends half_width and half_height therefrom
  virtual void	SetPCon(float p_con, int start = 0, int end = -1);
  // #BUTTON set p_con value for a range of send_gp_offs (default = all; end-1 = all)

  TA_SIMPLE_BASEFUNS(GpRndTesselPrjnSpec);
protected:
  override void UpdateAfterEdit_impl();
private:
  void	Initialize();
  void	Destroy()	{ };
};

class EMERGENT_API TiledRFPrjnSpec : public ProjectionSpec {
  // Tiled receptive field projection spec: connects entire receiving layer unit groups with overlapping tiled regions of sending layers
INHERITED(ProjectionSpec)
public:
  TwoDCoord	recv_gp_border;		// number of groups around edge of layer to not connect 
  TwoDCoord	recv_gp_ex_st; 		// start of groups to exclude (e.g., from the middle; -1 = no exclude)
  TwoDCoord	recv_gp_ex_n; 		// number of groups to exclude
  TwoDCoord	send_border;		// number of units around edge of sending layer to not connect
  TwoDCoord	send_adj_rfsz;		// adjust the total number of sending units by this amount in computing rfield size
  TwoDCoord	send_adj_sndloc;	// adjust the total number of sending units by this amount in computing sending locations
  float		rf_width_mult;		// multiplier factor on the receptive field width: 1.0 = exactly half overlap of RF's across groups.  Larger number = more overlap

  // computed values below
  TwoDCoord ru_geo;		// #READ_ONLY receiving unit geometry
  TwoDCoord recv_gp_ed;		// #READ_ONLY recv gp end
  TwoDCoord recv_gp_ex_ed;	// #READ_ONLY recv gp ex end
  PosTwoDCoord su_act_geom;	// #READ_ONLY sending actual geometry
  TwoDCoord n_recv_gps;		// #READ_ONLY number of recv gps
  TwoDCoord n_send_units;	// #READ_ONLY number of sending units total 
  TwoDCoord rf_ovlp; 		// #READ_ONLY ovlp = send / (ng + 1)
  FloatTwoDCoord rf_move;	// #READ_ONLY how much to move sending rf per recv group
  TwoDCoord rf_width;		// #READ_ONLY width of the sending rf 

  virtual bool	InitRFSizes(Projection* prjn); // initialize sending receptive field sizes

  void 	Connect_impl(Projection* prjn);
  int 	ProbAddCons(Projection* prjn, float p_add_con, float init_wt = 0.0);

  virtual void	SelectRF(Projection* prjn);
  // #BUTTON select all sending and receiving units in the receptive field of this projection

  TA_SIMPLE_BASEFUNS(TiledRFPrjnSpec);
private:
  void	Initialize();
  void 	Destroy()		{ };
};

class EMERGENT_API TiledGpRFPrjnSpec : public ProjectionSpec {
  // Tiled receptive field projection spec for entirely group-to-group connections: connects entire receiving layer unit groups with overlapping tiled regions of sending layer groups
INHERITED(ProjectionSpec)
public:
  TwoDCoord	send_gp_size;		// number of groups in the sending receptive field
  TwoDCoord	send_gp_skip;		// number of groups to skip per each recv group (typically 1/2 of the size for nice overlap)
  bool		reciprocal;		// if true, make the appropriate reciprocal connections for a backwards projection from recv to send

  void 	Connect_impl(Projection* prjn);
  virtual void 	Connect_Reciprocal(Projection* prjn);
  int 	ProbAddCons(Projection* prjn, float p_add_con, float init_wt = 0.0);

  TA_SIMPLE_BASEFUNS(TiledGpRFPrjnSpec);
private:
  void	Initialize();
  void 	Destroy()		{ };
};

class EMERGENT_API TiledNovlpPrjnSpec : public ProjectionSpec {
  // Tiled non-overlapping projection spec: connects entire receiving layer unit groups with non-overlapping tiled regions of sending units
INHERITED(ProjectionSpec)
public:
  bool		reciprocal;	// if true, make the appropriate reciprocal connections for a backwards projection from recv to send

  TwoDCoord ru_geo;		// #READ_ONLY receiving unit geometry
  PosTwoDCoord su_act_geom;	// #READ_ONLY sending actual geometry
  FloatTwoDCoord rf_width;	// #READ_ONLY how much to move sending rf per recv group

  virtual bool	InitRFSizes(Projection* prjn); // initialize sending receptive field sizes

  void 	Connect_impl(Projection* prjn);
  virtual void 	Connect_Reciprocal(Projection* prjn);
//   int 	ProbAddCons(Projection* prjn, float p_add_con, float init_wt = 0.0);

//   virtual void	SelectRF(Projection* prjn);
  // #BUTTON select all sending and receiving units in the receptive field of this projection

  TA_SIMPLE_BASEFUNS(TiledNovlpPrjnSpec);
private:
  void	Initialize();
  void 	Destroy()		{ };
};

#endif /* netstru_extra_h */

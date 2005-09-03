/* -*- C++ -*- */
/*=============================================================================
//									      //
// This file is part of the PDP++ software package.			      //
//									      //
// Copyright (C) 1995 Randall C. O'Reilly, Chadley K. Dawson, 		      //
//		      James L. McClelland, and Carnegie Mellon University     //
//     									      //
// Permission to use, copy, and modify this software and its documentation    //
// for any purpose other than distribution-for-profit is hereby granted	      //
// without fee, provided that the above copyright notice and this permission  //
// notice appear in all copies of the software and related documentation.     //
//									      //
// Permission to distribute the software or modified or extended versions     //
// thereof on a not-for-profit basis is explicitly granted, under the above   //
// conditions. 	HOWEVER, THE RIGHT TO DISTRIBUTE THE SOFTWARE OR MODIFIED OR  //
// EXTENDED VERSIONS THEREOF FOR PROFIT IS *NOT* GRANTED EXCEPT BY PRIOR      //
// ARRANGEMENT AND WRITTEN CONSENT OF THE COPYRIGHT HOLDERS.                  //
// 									      //
// Note that the taString class, which is derived from the GNU String class,  //
// is Copyright (C) 1988 Free Software Foundation, written by Doug Lea, and   //
// is covered by the GNU General Public License, see ta_string.h.             //
// The iv_graphic library and some iv_misc classes were derived from the      //
// InterViews morpher example and other InterViews code, which is             //
// Copyright (C) 1987, 1988, 1989, 1990, 1991 Stanford University             //
// Copyright (C) 1991 Silicon Graphics, Inc.				      //
//									      //
// THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,         //
// EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY 	      //
// WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  	      //
// 									      //
// IN NO EVENT SHALL CARNEGIE MELLON UNIVERSITY BE LIABLE FOR ANY SPECIAL,    //
// INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND, OR ANY DAMAGES  //
// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER OR NOT     //
// ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF LIABILITY,      //
// ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS        //
// SOFTWARE. 								      //
==============================================================================*/

// netstru_extra.h
// extra netstru objects derived from basic ones that are included in standard
// pdp library

#ifndef netstru_extra_h
#define netstru_extra_h

#include "netstru.h"

class FullPrjnSpec : public ProjectionSpec {
  // Full connectivity between layers
public:
  void 	Connect_impl(Projection* prjn);
  // Connection function for full connectivity
  int 	ProbAddCons(Projection* prjn, float p_add_con, float init_wt = 0.0);

  void	Initialize() 		{ };
  void 	Destroy()		{ };
  TA_BASEFUNS(FullPrjnSpec);
};

class TessEl : public taOBase {
  // ##NO_TOKENS #NO_UPDATE_AFTER one element of a tesselation specification
public:
  TwoDCoord	send_off;	// offset from current receiving unit
  float		wt_val;		// value to assign to weight

  void	Initialize();
  void	Destroy()	{ };
  void	InitLinks();
  void	Copy_(const TessEl& cp);
  COPY_FUNS(TessEl, taOBase);
  TA_BASEFUNS(TessEl);
};

class TessEl_List : public taList<TessEl> {
  // ##NO_TOKENS #NO_UPDATE_AFTER list of TessEl objects
public:
  void	Initialize() 		{ };
  void 	Destroy()		{ };
  TA_BASEFUNS(TessEl_List);
};

class TesselPrjnSpec : public ProjectionSpec {
  // arbitrary tesselations (repeating patterns) of connectivity
public:
  enum LinkType {
    NO_LINK,			// weights are not linked
    GP_LINK,			// weights are linked by groups of units
    UN_LINK			// weights are linked among all units
  };

  TwoDCoord	recv_off;	// offset in layer for start of recv units to begin connecting
  TwoDCoord	recv_n;		// number of receiving units to connect in each dimension (-1 for all)
  TwoDCoord	recv_skip;	// increment for recv units in each dimension -- 1 = connect all units; 2 = skip every other unit, etc
  TwoDCoord	recv_group;	// group together this many units under the same starting coord, resulting in a tile pattern
  bool		wrap;		// whether to wrap coordinates around (else clip)
  LinkType	link_type;	// type of weight linking to use
  TwoDCoord	link_src;	// #CONDEDIT_OFF_link_type:NO_LINK index for source unit for unit linking (should have full set of connections)
  FloatTwoDCoord send_scale;	// scale to apply to transform receiving unit coords into sending unit coords
  TwoDCoord	send_border;	// border size around sending layer (constant offset to add to sending offsets)

  TessEl_List	send_offs;	// offsets of the sending units

  virtual void	Connect_NonLinked(Projection* prjn); // connect non-linked units
  virtual void	Connect_GpLinked(Projection* prjn); // connect group linked units
  virtual void	Connect_GpLinkFmSrc(Projection* prjn); // connect group linked units from source
  virtual void	Connect_UnLinked(Projection* prjn); // connect linked units
  virtual void	Connect_UnLinkFmSrc(Projection* prjn); // connect linked units from source

  void		Connect_impl(Projection* prjn);
  void 		ReConnect_Load(Projection* prjn); // #IGNORE
  void		C_InitWtState(Projection* prjn, Con_Group* cg, Unit* ru);
  // uses weight values as specified in the tesselel's

  virtual void	GetCtrFmRecv(TwoDCoord& sctr, TwoDCoord ruc);
  // get center of sender coords from receiving coords
  virtual void	Connect_RecvUnit(Unit* ru_u, const TwoDCoord& ruc, Projection* prjn);
  // connects one recv unit to all senders (doesn't check for linking..)

  virtual void	MakeEllipse(int half_width, int half_height, int ctr_x, int ctr_y);
  // #MENU #MENU_ON_Actions #MENU_SEP_BEFORE make a connection pattern in the form of an elipse: center is located at ctr_x,y and extends half_width and half_height therefrom
  virtual void	MakeRectangle(int width, int height, int left, int bottom);
  // #MENU make a connection pattern in the form of a rectangle starting at left, bottom coordinate and going right and up by width, height
//TODO  virtual void	MakeFromNetView(NetView* view);
  /* #MENU make connection pattern from selected units in netview,
     first unit is center, then sender positions */
  virtual void	WeightsFromDist(float scale);
  // #MENU #MENU_SEP_BEFORE assign weights as a linear function of sender distance
  virtual void	WeightsFromGausDist(float scale, float sigma);
  // #MENU assign weights as a Gaussian function of sender distance

  void	UpdateAfterEdit();
  void	Initialize();
  void	Destroy()	{ };
  void	InitLinks();
  void	Copy_(const TesselPrjnSpec& cp);
  COPY_FUNS(TesselPrjnSpec, ProjectionSpec);
  TA_BASEFUNS(TesselPrjnSpec);
};

class OneToOnePrjnSpec : public ProjectionSpec {
  // one-to-one connectivity (1st unit to 1st unit, etc)
public:
  int	n_conns;		// number of connections to make (-1 for size of layer)
  int	recv_start;		// starting unit index for recv connections
  int 	send_start;		// starting unit index for sending connections

  void	Connect_impl(Projection* prjn);

  void	Initialize();
  void 	Destroy()		{ };
  SIMPLE_COPY(OneToOnePrjnSpec);
  COPY_FUNS(OneToOnePrjnSpec, ProjectionSpec);
  TA_BASEFUNS(OneToOnePrjnSpec);
};

class UniformRndPrjnSpec : public ProjectionSpec {
  // Uniform random connectivity between layers
public:
  float		p_con;		// overall probability of connection
  bool		permute;	// use permuted order for same number of cons
  bool		sym_self;	// if a self projection, make it symmetric (senders = receivers) otherwise it is not
  bool		same_seed;	// use the same random seed each time (same connect pattern)
  RndSeed	rndm_seed;	// #HIDDEN random seed

  void 	Connect_impl(Projection* prjn);
  // Connection function for full connectivity

  void	UpdateAfterEdit();	// limit p_con
  void	Initialize();
  void 	Destroy()		{ };
  void	InitLinks();
  SIMPLE_COPY(UniformRndPrjnSpec);
  COPY_FUNS(UniformRndPrjnSpec, ProjectionSpec);
  TA_BASEFUNS(UniformRndPrjnSpec);
};

class PolarRndPrjnSpec : public ProjectionSpec {
  // random connectivity defined as a function of distance and angle
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

  void	C_InitWtState(Projection* prjn, Con_Group* cg, Unit* ru);
  // uses weight values equal to the distance probability

  static float	UnitDist(UnitDistType typ, Projection* prjn,
			 const TwoDCoord& ru, const TwoDCoord& su);
  // computes the distance between two units according to distance type
  static Unit*	GetUnitFmOff(UnitDistType typ, bool wrap, Projection* prjn,
			     const TwoDCoord& ru, const FloatTwoDCoord& su_off);
  // gets unit from real-valued offset scaled according to distance type

  virtual float	GetDistProb(Projection* prjn, Unit* ru, Unit* su);
  // compute the probability for connecting two units as a fctn of distance

  void	UpdateAfterEdit();	// limit p_con
  void	Initialize();
  void	Destroy()	{ };
  void	InitLinks();
  SIMPLE_COPY(PolarRndPrjnSpec);
  COPY_FUNS(PolarRndPrjnSpec, ProjectionSpec);
  TA_BASEFUNS(PolarRndPrjnSpec);
};

class SymmetricPrjnSpec : public ProjectionSpec {
  // connects units with receiving connection where sending one already exists
public:
  void 	Connect_impl(Projection* prjn);

  void	Initialize()	{ };
  void	Destroy()	{ };
  TA_BASEFUNS(SymmetricPrjnSpec);
};

class LinkPrjnConPtr : public taOBase {
  // ##NO_TOKENS #NO_UPDATE_AFTER specifies a connection to link
public:
  String	recv_layer;	// layer name receiving unit is in
  int		recv_idx;	// leaf index of receiving unit within layer
  String	send_layer;	// layer name sending unit is in
  int		send_idx;	// leaf index of sending unit within layer

  virtual Con_Group* GetCon(Network* net, int& idx, Unit*& ru, Unit*& su,
			    Con_Group*& su_cg, int& sg_idx);
  // #IGNORE get both sides of the connection (sending and receiving)

  void	Initialize();
  void	Destroy()	{ };
  void 	Copy_(const LinkPrjnConPtr& cp);
  COPY_FUNS(LinkPrjnConPtr, taOBase);
  TA_BASEFUNS(LinkPrjnConPtr);
};

class LinkPrjnConPtr_List : public taList<LinkPrjnConPtr> {
  // ##NO_TOKENS #NO_UPDATE_AFTER list of LinkPrjnConPtr objects
public:
  void	Initialize() 		{ };
  void 	Destroy()		{ };
  TA_BASEFUNS(LinkPrjnConPtr_List);
};

class LinkPrjnSpec : public ProjectionSpec {
  // links an arbitrary set of wts, which must already be created!
public:
  LinkPrjnConPtr_List	links;

  void		PreConnect(Projection*)		{ }; // don't preconnect
  void		Connect_impl(Projection* prjn);
  void		ReConnect_Load(Projection* prjn);
  void		CopyNetwork(Network* net, Network* cn, Projection* prjn, Projection* cp);

  void	Initialize();
  void	Destroy()	{ CutLinks(); }
  void	InitLinks();
  void	CutLinks();
  void	Copy_(const LinkPrjnSpec& cp);
  COPY_FUNS(LinkPrjnSpec, ProjectionSpec);
  TA_BASEFUNS(LinkPrjnSpec);
};

class ScriptPrjnSpec : public ProjectionSpec, public ScriptBase {
  // Script-controlled connectivity
public:
  Projection*	prjn;		// #READ_ONLY #NO_SAVE this holds the argument to the prjn
  SArg_Array	s_args;		// string-valued arguments to pass to script

  void		Connect_impl(Projection* prj);

  TypeDef*	GetThisTypeDef()	{ return GetTypeDef(); }
  void*		GetThisPtr()		{ return (void*)this; }

  virtual void	Interact();
  // #BUTTON change to this shell in script (terminal) window to interact, debug etc script
  virtual void	Compile();
  // #BUTTON compile script from script file into internal runnable format

  void	UpdateAfterEdit();

  void	Initialize();
  void 	Destroy();
  void	InitLinks();
  void	Copy_(const ScriptPrjnSpec& cp);
  COPY_FUNS(ScriptPrjnSpec, ProjectionSpec);
  TA_BASEFUNS(ScriptPrjnSpec);
};

class CustomPrjnSpec : public ProjectionSpec {
  // connectivity is defined manually (i.e. unit-by-unit)
public:

  void	Connect(Projection* prjn);	// do nothing
  void	Initialize()	{ };
  void	Destroy()	{ };
  TA_BASEFUNS(CustomPrjnSpec);
};


//////////////////////////////////////////
//	UnitGroup-based PrjnSpecs	//
//////////////////////////////////////////

class GpFullPrjnSpec : public FullPrjnSpec {
  // unit_group based full connectivity with distinct con_groups for each unit group
public:
  enum NConGroups {		// number of connection groups for this projection
    RECV_SEND_PAIR,		// create separate con_groups for each recv_send pair
    SEND_ONLY 			// create separate recv con_groups for ea sending gp
  };

  NConGroups	n_con_groups;	// number of con_groups to create

  virtual void 	GetNGroups(Projection* prjn, int& r_n_ugp, int& s_n_ugp);
  // get number of connection groups for the projection

  void	PreConnect(Projection* prjn);
  void	Connect_impl(Projection* prjn);

  void	Initialize();
  void 	Destroy()		{ };
  SIMPLE_COPY(GpFullPrjnSpec);
  COPY_FUNS(GpFullPrjnSpec, FullPrjnSpec);
  TA_BASEFUNS(GpFullPrjnSpec);
};

class GpOneToOnePrjnSpec : public OneToOnePrjnSpec {
  // unit_group based one-to-one connectivity (all in 1st group to all in 1st group, etc)
public:
  void	Connect_impl(Projection* prjn);

  void	Initialize()		{ };
  void 	Destroy()		{ };
  TA_BASEFUNS(GpOneToOnePrjnSpec);
};

class RndGpOneToOnePrjnSpec : public GpOneToOnePrjnSpec {
  // uniform random connectivity between one-to-one groups
public:
  float		p_con;		// overall probability of connection
  bool		permute;	// use permuted order for same number of cons
  bool		same_seed;	// use the same random seed each time (same connect pattern)
  RndSeed	rndm_seed;	// #HIDDEN random seed

  void	Connect_impl(Projection* prjn);

  void	UpdateAfterEdit();	// limit p_con
  void	Initialize();
  void 	Destroy()		{ };
  void 	InitLinks();
  SIMPLE_COPY(RndGpOneToOnePrjnSpec);
  COPY_FUNS(RndGpOneToOnePrjnSpec, GpOneToOnePrjnSpec);
  TA_BASEFUNS(RndGpOneToOnePrjnSpec);
};

class GpOneToManyPrjnSpec : public OneToOnePrjnSpec {
  // unit_group based one-to-many connectivity (one sending gp to all recv units)
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

  void	Initialize();
  void 	Destroy()		{ };
  SIMPLE_COPY(GpOneToManyPrjnSpec);
  COPY_FUNS(GpOneToManyPrjnSpec, OneToOnePrjnSpec);
  TA_BASEFUNS(GpOneToManyPrjnSpec);
};

class GpTessEl : public taOBase {
  // ##NO_TOKENS #NO_UPDATE_AFTER one element of a tesselation specification for groups
public:
  TwoDCoord	send_gp_off;	// offset of group from current receiving group
  float		p_con;		// proportion connectivity from this group -- negative value means just make symmetric cons

  void	Initialize();
  void	Destroy()	{ };
  void	InitLinks();
  void	Copy_(const GpTessEl& cp);
  COPY_FUNS(GpTessEl, taOBase);
  TA_BASEFUNS(GpTessEl);
};

class GpTessEl_List : public taList<GpTessEl> {
  // ##NO_TOKENS #NO_UPDATE_AFTER list of GpTessEl objects
public:
  void	Initialize() 		{ };
  void 	Destroy()		{ };
  TA_BASEFUNS(GpTessEl_List);
};

class GpRndTesselPrjnSpec : public ProjectionSpec {
  // specifies patterns of groups to connect with, with random connectivity within each group
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
  bool		permute;	// use permuted order for same number of cons
  bool		sym_self;	// if a self projection, make it symmetric (senders = receivers) otherwise it is not
  bool		same_seed;	// use the same random seed each time (same connect pattern)
  RndSeed	rndm_seed;	// #HIDDEN random seed

  void		Connect_impl(Projection* prjn);

  virtual void	GetCtrFmRecv(TwoDCoord& sctr, TwoDCoord ruc);
  // get center of sender coords from receiving coords
  virtual void  Connect_Gps(Unit_Group* ru_gp, Unit_Group* su_gp, float p_con, Projection* prjn);
  virtual void	Connect_RecvGp(Unit_Group* ru_gp, const TwoDCoord& ruc, Projection* prjn);
  // connects one recv unit to all senders (doesn't check for linking..)

  virtual void	MakeEllipse(int half_width, int half_height, int ctr_x, int ctr_y);
  // #MENU #MENU_ON_Actions #MENU_SEP_BEFORE make a connection pattern in the form of an elipse: center is located at ctr_x,y and extends half_width and half_height therefrom
  virtual void	MakeRectangle(int width, int height, int left, int bottom);
  // #MENU make a connection pattern in the form of a rectangle starting at left, bottom coordinate and going right and up by width, height
  virtual void	SetPCon(float p_con, int start = 0, int end = -1);
  // #MENU set p_con value for a range of send_gp_offs (default = all; end-1 = all)

  void	UpdateAfterEdit();
  void	Initialize();
  void	Destroy()	{ };
  void	InitLinks();
  SIMPLE_COPY(GpRndTesselPrjnSpec);
  COPY_FUNS(GpRndTesselPrjnSpec, ProjectionSpec);
  TA_BASEFUNS(GpRndTesselPrjnSpec);
};

class TiledRFPrjnSpec : public ProjectionSpec {
  // Tiled receptive field projection spec: connects entire receiving layer unit groups with overlapping tiled regions of sending layers
public:
  TwoDCoord	recv_gp_border;		// number of groups around edge of layer to not connect 
  TwoDCoord	recv_gp_ex_st; 		// start of groups to exclude (e.g., from the middle; -1 = no exclude)
  TwoDCoord	recv_gp_ex_n; 		// number of groups to exclude
  TwoDCoord	send_border;		// number of units around edge of sending layer to not connect
  TwoDCoord	send_adj_rfsz;		// adjust the total number of sending units by this amount in computing rfield size
  TwoDCoord	send_adj_sndloc;	// adjust the total number of sending units by this amount in computing sending locations

  void 	Connect_impl(Projection* prjn);
  int 	ProbAddCons(Projection* prjn, float p_add_con, float init_wt = 0.0);

  virtual void	SelectRF(Projection* prjn);
  // #BUTTON select all sending and receiving units in the receptive field of this projection

  void	Initialize();
  void 	Destroy()		{ };
  void	InitLinks();
  SIMPLE_COPY(TiledRFPrjnSpec);
  COPY_FUNS(TiledRFPrjnSpec, ProjectionSpec);
  TA_BASEFUNS(TiledRFPrjnSpec);
};

class TiledGpRFPrjnSpec : public ProjectionSpec {
  // Tiled receptive field projection spec for entirely group-to-group connections: connects entire receiving layer unit groups with overlapping tiled regions of sending layer groups
public:
  TwoDCoord	send_gp_size;		// number of groups in the sending receptive field
  TwoDCoord	send_gp_skip;		// number of groups to skip per each recv group (typically 1/2 of the size for nice overlap)
  bool		reciprocal;		// if true, make the appropriate reciprocal connections for a backwards projection from recv to send

  void 	Connect_impl(Projection* prjn);
  int 	ProbAddCons(Projection* prjn, float p_add_con, float init_wt = 0.0);

  void	Initialize();
  void 	Destroy()		{ };
  void	InitLinks();
  SIMPLE_COPY(TiledGpRFPrjnSpec);
  COPY_FUNS(TiledGpRFPrjnSpec, ProjectionSpec);
  TA_BASEFUNS(TiledGpRFPrjnSpec);
};

#endif /* netstru_extra_h */
